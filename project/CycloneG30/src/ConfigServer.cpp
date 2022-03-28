#include "ConfigServer.h"

#include <qfile.h>
#include <math.h>
#include <mutex>
#include <filesystem>
#include <QWidget>

#ifdef Q_OS_WIN32
//#include <windows.h>
//#include <minwindef.h>

//#include <Shlobj.h>
#include <shlobj_core.h>
#else
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#endif

#include <Log.h>
#include "UsbProtoMsg.h"


ConfigServer* ConfigServer::m_instance = nullptr;


ConfigServer::ConfigServer()
{
    m_lvdsCurUnitPair = std::make_pair(1.0,1.0);
    m_vctLvdsConvert.resize(16);
    for(auto && ites : m_vctLvdsConvert)
    {
       for(int i = 0; i < LVDS_SAMPLE_KIINDS_NUM; ++i)
       {
           std::pair<float,float> &&p1 = std::make_pair(1.0,1.0);
           ites.emplace_back(p1);

       }
    }
//    for(auto && ites : m_vctLvdsConvert)
//    {
//       for(auto & ite: ites)
//       {
//          std::cout <<ite.first<<" "<< ite.second<<" ";
//       }
//       std::cout <<std::endl;
//    }
    m_vctSensorSteRegVal.resize(SENSOR_STATE_REG_NUM);
    for(auto &&regValue : m_vctSensorSteRegVal)
    {
        regValue.stateReg = 0x00000000;
    }
}


std::string ConfigServer::GetCurrentPath(void)
{
    return std::filesystem::current_path().string();
}

unsigned long ConfigServer::GetCurrentThreadSelf(void)
{
#ifdef Q_OS_WIN32

    return GetCurrentThreadId();

#else

    return pthread_self();

#endif
    return 0;
}

std::string ConfigServer::GetLocalIPAddress(void)
{
    std::string strRetip;
#ifdef Q_OS_WIN32

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
    {
        char name[255] = { 0 };
        if (gethostname(name, sizeof(name)) == 0)
        {
            PHOSTENT hostinfo;
            if ((hostinfo = gethostbyname(name)) != NULL)
            {
                char* tmpip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
                strRetip = tmpip;
            }
        }
        WSACleanup();
    }

#else

    struct ifaddrs* ifAddrStruct = NULL;
    getifaddrs(&ifAddrStruct);

    for (struct ifaddrs* ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr != nullptr && ifa->ifa_addr->sa_family == AF_INET)
        {
            //check it is a valid IP4 Address
            void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            std::string strname = ifa->ifa_name;
            if (strcmp(ifa->ifa_name, "lo") != 0 && strcmp("127.0.0.1", addressBuffer) != 0)
            {
                strRetip = addressBuffer;
                if (!strRetip.empty())
                {
                    break;
                }
            }
        }
    }
    if (ifAddrStruct != NULL)
    {
        freeifaddrs(ifAddrStruct);
    }

#endif
    return strRetip;
}

std::string ConfigServer::GetHomePath(void)
{
#ifdef Q_OS_WIN32

    char szAppData[MAX_PATH] = { 0 };
    HRESULT hRet = SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, szAppData);
    if (SUCCEEDED(hRet))
    {
        return std::string(szAppData);
    }

#else

    char* homepath = getenv("HOME");
    if (homepath != nullptr)
    {
        return std::string(homepath);
    }

#endif
    return std::string("./");
}

std::string ConfigServer::GetRecordPath(const RECORD_FILE_TYPE_ENUM &type)
{
#ifdef Q_OS_WIN32
        std::string strRecordPath = GetCurrentPath() + "/etc/Record/";
#else
        std::string strRecordPath = "/data/Record/";
#endif
    switch (type)
    {
    case RECORD_DEGATE_FILE:
        strRecordPath += "Degate/";
        break;
    case RECORD_READS_ADAPTERS_FILE:
        strRecordPath += "ReadsAdapters/";
        break;
    case RECORD_VALID_FILE:
        strRecordPath += "Valid/";
        break;
    case RECORD_MUX_FILE:
        strRecordPath += "Mux/";
        break;
    case RECORD_PORE_FILE:
        strRecordPath += "Pore/";
        break;
    default:
        break;
    }
    return strRecordPath;
}
std::string ConfigServer::GetLogPath(const LOG_FILE_TYPE_ENUM &type)
{
#ifdef Q_OS_WIN32
        std::string strLogPath = GetCurrentPath() + "/etc/CycloneLog/";
#else
        std::string strLogPath = "/data/CycloneLog/";
#endif
    switch (type)
    {
    case LOG_DEGATE_FILE:
        strLogPath += "Degate/";
        break;
    case LOG_READS_ADAPTERS_FILE:
        strLogPath += "ReadsAdapters/";
        break;
    case LOG_VALID_FILE:
        strLogPath += "Valid/";
        break;
    case LOG_MUX_FILE:
        strLogPath += "Mux/";
        break;
    default:
        break;
    }
    return strLogPath;
}
ConfigServer* ConfigServer::GetInstance(void)
{
    static std::once_flag of;
    std::call_once(of, [&]() {
        m_instance = new ConfigServer;
        });
    return m_instance;
}

bool ConfigServer::InitConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    m_pSysConfig = pset;

    pset->beginGroup("plotCtrl");
    m_dPlotYmax = pset->value("yPlotMax").toDouble();
    m_dPlotYmin = pset->value("yPlotMin").toDouble();
    m_dPlotXnum = pset->value("xPlotNum").toDouble();
    m_iUseOpenGL = pset->value("isOpenGL").toInt();
    if (pset->contains("isWorknet"))
    {
        m_iWorknet = pset->value("isWorknet").toInt();
    }
    else
    {
        m_iWorknet = 0;
        pset->setValue("isWorknet", m_iWorknet);
    }
    pset->endGroup();
    pset->beginGroup("RecordConfig");
    m_iSequencingId = pset->value("SequencingId").toInt();
    m_strRecordConfig = pset->value("url").toString().toStdString();
    m_strSequenceConfig = pset->value("sequrl").toString().toStdString();
    pset->endGroup();

    pset->beginGroup("RecordConfig");
    m_iSequencingId = pset->value("SequencingId").toInt();
    m_strRecordConfig = pset->value("url").toString().toStdString();
    m_strSequenceConfig = pset->value("sequrl").toString().toStdString();
    pset->endGroup();
    pset->beginGroup("AdvanceConfig");
    m_bIsVoltInvert = pset->value("VoltageInvert").toBool();
    pset->endGroup();
    if (m_strLoginUserName.empty())
      {
          char arusername[1024] = { 0 };
  #ifdef Q_OS_WIN32
          //GetFileMD5Sum("D:\\env.txt", arusername);
          //GetExecuteCMD("cd /d D:\\src && dir", arusername, sizeof (arusername));
          //const char* pcmd = "echo %username%";
          //GetExecuteCMD(pcmd, arusername, sizeof (arusername));
  #else
          const char* pcmd = "echo $SUDO_USER";
          GetExecuteCMD(pcmd, arusername, sizeof(arusername));
  #endif
          //strcpy(arusername, "qwe   r fsf\r\r\n\n\r\n fsf  g\tf gfd  g");
          //std::regex regrep("\\s+");
          std::regex regrep("\\s|\r|\n");//filter space enter LR
          m_strLoginUserName = std::regex_replace(arusername, regrep, "");
      }



    return true;
}


bool ConfigServer::SyncDefaultConfig(const char* pprefix, const char* pconf)
{
    std::filesystem::path confpath(pconf);
    if (!std::filesystem::exists(confpath))
    {
        auto confname = confpath.filename();
        auto parentpath = confpath.parent_path();
        if (!std::filesystem::exists(parentpath))
        {
            std::filesystem::create_directories(parentpath);
        }
        //std::string strdef = ":/conf/conf/default" + confname.string();
        //std::filesystem::copy_file(strdef, pconfig);":/conf/conf/default"
        QString deffile = QString(pprefix) + confname.string().c_str();
        QFile::copy(deffile, QString(pconf));

        return true;
    }
    return false;
}


bool ConfigServer::OnLoadLVDSTransCsv(const QString &filePath)
{
    FILE* pf = ::fopen(filePath.toStdString().c_str(), "r");
    if (pf != nullptr)
    {
        ::fseek(pf, 0L, SEEK_END);
        int len = ::ftell(pf);

        char* pbuf = new char[len];
        memset(pbuf, 0, len);

        ::fseek(pf, 0L, SEEK_SET);
        int buflen = ::fread(pbuf, 1, len, pf);

        std::vector<std::vector<std::string>> vetvetdt;
        std::vector<std::string> vetrow;
        UsbProtoMsg::SplitString(pbuf, "\n", vetrow);

        delete[] pbuf;
        pbuf = nullptr;
        fclose(pf);
        pf = nullptr;

        if (vetrow.size() > 0)
        {
            for (auto&& ite : vetrow)
            {
                std::vector<std::string> vetdt;
                UsbProtoMsg::SplitString(ite.c_str(), ",", vetdt);
                vetvetdt.emplace_back(vetdt);
            }
            int maxNum = m_vctLvdsConvert.size() > vetvetdt.size() - 2 ?  vetvetdt.size() - 2: m_vctLvdsConvert.size();
            for (int start = 2, i =0;  i < maxNum ; ++start,++i)
            {
                auto& ite = vetvetdt.at(start);
                // std::cout<<ite.size()<<std::endl;
                if (ite.size() >= 14 && !ite[LVDS_CVT_LSB_RAW_5K_COL].empty() && !ite[LVDS_CVT_LSB_5K_COL].empty() \
                        && !ite[LVDS_CVT_LSB_RAW_10K_COL].empty() && !ite[LVDS_CVT_LSB_10K_COL].empty() \
                        && !ite[LVDS_CVT_LSB_RAW_20K_COL].empty() && !ite[LVDS_CVT_LSB_20K_COL].empty()\
                        && !ite[LVDS_CVT_LSB_RAW_3_75K_COL].empty() && !ite[LVDS_CVT_LSB_3_75K_COL].empty())
                {
                    auto p1 = std::make_pair(::atof(ite[LVDS_CVT_LSB_RAW_5K_COL].c_str()),::atof(ite[LVDS_CVT_LSB_5K_COL].c_str()));
                    m_vctLvdsConvert[i][0] = p1;
                    auto p2 = std::make_pair(::atof(ite[LVDS_CVT_LSB_RAW_10K_COL].c_str()),::atof(ite[LVDS_CVT_LSB_10K_COL].c_str()));
                    m_vctLvdsConvert[i][1] = p2;
                    auto p3 = std::make_pair(::atof(ite[LVDS_CVT_LSB_RAW_20K_COL].c_str()),::atof(ite[LVDS_CVT_LSB_20K_COL].c_str()));
                    m_vctLvdsConvert[i][2] = p3;
                    auto p4 = std::make_pair(::atof(ite[LVDS_CVT_LSB_RAW_3_75K_COL].c_str()),::atof(ite[LVDS_CVT_LSB_3_75K_COL].c_str()));
                    m_vctLvdsConvert[i][3] = p4;
                }
                else if (ite.size() >= 11 && !ite[LVDS_CVT_LSB_RAW_5K_COL].empty() && !ite[LVDS_CVT_LSB_5K_COL].empty() \
                           && !ite[LVDS_CVT_LSB_RAW_10K_COL].empty() && !ite[LVDS_CVT_LSB_10K_COL].empty() \
                        && !ite[LVDS_CVT_LSB_RAW_20K_COL].empty() && !ite[LVDS_CVT_LSB_20K_COL].empty())
                {
                    auto p1 = std::make_pair(::atof(ite[LVDS_CVT_LSB_RAW_5K_COL].c_str()),::atof(ite[LVDS_CVT_LSB_5K_COL].c_str()));
                    m_vctLvdsConvert[i][0] = p1;
                    auto p2 = std::make_pair(::atof(ite[LVDS_CVT_LSB_RAW_10K_COL].c_str()),::atof(ite[LVDS_CVT_LSB_10K_COL].c_str()));
                    m_vctLvdsConvert[i][1] = p2;
                    auto p3 = std::make_pair(::atof(ite[LVDS_CVT_LSB_RAW_20K_COL].c_str()),::atof(ite[LVDS_CVT_LSB_20K_COL].c_str()));
                    m_vctLvdsConvert[i][2] = p3;
                }
            }
#if 0
            int cnt = 0;
            for(auto && ites : m_vctLvdsConvert)
            {
                std::cout.setf(std::ios::left); //设置对齐方式为left
                std::cout.width(10);
                std::cout<<std::setprecision(8); //精度为18，正常为6
                std::cout <<cnt++<< "\t";
               for(auto & ite: ites)
               {
                  std::cout <<ite.first<<" \t"<< ite.second<<" \t";
               }
               std::cout <<std::endl;
            }
#endif
            return true;
        }
    }
    return false;
}

void ConfigServer::SetLvdsCvtUnitArry(const int rowIndx,const int colIndx)
{
    if(rowIndx < m_vctLvdsConvert.size() && colIndx < LVDS_SAMPLE_KIINDS_NUM)
    {
        m_lvdsCurUnitPair = m_vctLvdsConvert[rowIndx][colIndx];
    }
}
void ConfigServer::WidgetPosAdjustByParent(QWidget* parent, QWidget* widget)
{
    if (parent && widget)
    {
        QPoint globalPos = parent->mapToGlobal(QPoint(0, 0));
        int x = globalPos.x() + (parent->width() - widget->width()) / 2;
        int y = globalPos.y() + (parent->height() - widget->height()) / 2;
        widget->move(x, y);
        widget->update();
    }
}

int ConfigServer::GetExecuteCMD(const char* cmd, char* result, int len)
{
#ifdef Q_OS_WIN32

    char szCmdLine[MAX_PATH] = { 0 };
    sprintf(szCmdLine, "cmd.exe /c \"%s\" ", cmd);
    //eg: cmd.exe /c "dir"
    SECURITY_ATTRIBUTES sa = { 0 };
    HANDLE hRead = NULL, hWrite = NULL;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return -2;
    }
    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFOA);
    GetStartupInfoA(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!CreateProcessA(NULL, szCmdLine, NULL, NULL, TRUE, NULL, NULL, nullptr, &si, &pi))//π
    {
        CloseHandle(hWrite);
        CloseHandle(hRead);
        return -3;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    //Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hWrite);

    DWORD bytesRead = 0;
    BOOL ret = ReadFile(hRead, result, len, &bytesRead, NULL);
    CloseHandle(hRead);

    return ret;

#else

    char bufps[1024] = { 0 };
    FILE* ptr = nullptr;
    if ((ptr = popen(cmd, "r")) != nullptr)
    {
        while (fgets(bufps, 1024, ptr) != nullptr)
        {
            strcat(result, bufps);
            if (strlen(result) >= len)
                break;
        }
        pclose(ptr);
        ptr = nullptr;

        return 0;
    }
    //printf("popen %s error\n", ps);
    return -1;

#endif
}

bool ConfigServer::GetRunLog(const char* txt, const char* cmd, int len, bool stp)
{
#ifdef Q_OS_WIN32
#else
    std::thread thrd([=]()
        {
            LOGRUNI("cmdlog={}", txt);
            std::string strtop;
            char bufps[1024] = { 0 };
            FILE* ptr = nullptr;
            if ((ptr = popen(cmd, "r")) != nullptr)
            {
                int cnt = 0;
                while (fgets(bufps, 1024, ptr) != nullptr)
                {
                    //strtop += bufps;
                    std::regex regrep("\\r|\n");//filter space enter LR
                    strtop = std::regex_replace(bufps, regrep, "");
                    if (cnt > 2)//cnt>len
                    {
                        if (strncmp(bufps, "top - ", 6) == 0 || cnt > len)
                            break;
                    }
                    LOGRUNI("{}={}", cnt, strtop);
                    ++cnt;
                }
                pclose(ptr);
                ptr = nullptr;
            }

            if (stp)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                char arcmd[1024] = { 0 };
                sprintf(arcmd, "sudo killall -v supervisord ; sudo killall -v gunicorn ; sudo killall -v celery ; sudo killall -v python3.7 ; sudo killall -v redis-server");
                system(arcmd);
            }
        });
    thrd.detach();
#endif

    return true;
}

bool ConfigServer::SplitString2(const char* pSrc, const char* pde, std::vector<std::string>& vetOut)
{
    if (pSrc == nullptr)
    {
        return false;
    }
    std::string strSrc = pSrc;
    size_t beg = strlen(pde);
    size_t pos = strSrc.find_first_of(pde);
    while (pos != std::string::npos)
    {
        std::string temp = strSrc.substr(0, pos);
        if (temp.size() > 0)
        {
            vetOut.push_back(temp);
        }
        pos += beg;
        strSrc = strSrc.substr(pos);
        pos = strSrc.find_first_of(pde);
    }
    if (strSrc.size() > 0)
    {
        vetOut.push_back(strSrc);
    }

    return false;
}
