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


ConfigServer* ConfigServer::m_instance = nullptr;


ConfigServer::ConfigServer()
{
}

bool ConfigServer::InitData(void)
{
    m_mapChannelCoordinate.clear();
    if (m_iHardwareAmpBoardVer == 1)
    {
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_mapChannelCoordinate[COORDINATE_CHANNEL[i][0] - 1] = { COORDINATE_CHANNEL[i][1], COORDINATE_CHANNEL[i][2] };
        }
    }
    else
    {
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_mapChannelCoordinate[COORDINATE_CHANNEL2[i][0] - 1] = { COORDINATE_CHANNEL2[i][1], COORDINATE_CHANNEL2[i][2] };
        }
    }
    return false;
}

ConfigServer* ConfigServer::GetInstance(void)
{
    static std::once_flag of;
    std::call_once(of, [&]() {
        m_instance = new ConfigServer;
        });
    return m_instance;
}

bool ConfigServer::FindChannelCoordinate(int ch, int& ix, int& iy)
{
    for (auto&& ite : m_mapChannelCoordinate)
    {
        if (ite.first == ch)
        {
            ix = ite.second.first;
            iy = ite.second.second;
            return true;
        }
    }
    return false;
}

bool ConfigServer::FindCoordinateChannel(int ix, int iy, int& ch)
{
    for (auto&& ite : m_mapChannelCoordinate)
    {
        if (ite.second.first == ix && ite.second.second == iy)
        {
            ch = ite.first;
            return true;
        }
    }
    return false;
}

bool ConfigServer::InitConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    m_pSysConfig = pset;

    pset->beginGroup("mainCtrl");

    m_iUseBgiCyclone = pset->value("useBgiCyclone").toInt();
    m_iUseDemoData = pset->value("useDemoData").toInt();
    m_iPrintDemoData = pset->value("printDemoData").toInt();
    if (pset->contains("isWorknet"))
    {
        m_iWorknet = pset->value("isWorknet").toInt();
    }
    else
    {
        m_iWorknet = 0;
        pset->setValue("isWorknet", m_iWorknet);
    }
    if (pset->contains("mainUserName"))
    {
        m_strMainUserName = pset->value("mainUserName").toString().toStdString();
    }
    else
    {
        m_strMainUserName = "shanzhu";
        pset->setValue("mainUserName", QString::fromStdString(m_strMainUserName));
    }
    if (pset->contains("mockDataDemo"))
    {
        m_strMockDataDemo = pset->value("mockDataDemo").toString().toStdString();
    }
    else
    {
        m_strMockDataDemo = "E:\\20211010_LAB256_5K_test_yx\\Raw";
        pset->setValue("mockDataDemo", QString::fromStdString(m_strMockDataDemo));
    }

    m_iDeveloperMode = pset->value("developerMode").toInt();

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

    pset->beginGroup("mainCtrl");
    pset->endGroup();

    pset->beginGroup("HardwareCtrl");
    m_iHardwareAmpBoardVer = pset->value("ampBoardVer").toInt();
    pset->endGroup();

    pset->beginGroup("plotCtrl");
    m_dPlotYmax = pset->value("yPlotMax").toDouble();
    m_dPlotYmin = pset->value("yPlotMin").toDouble();
    m_dPlotXnum = pset->value("xPlotNum").toDouble();
    m_iUseOpenGL = pset->value("isOpenGL").toInt();
    pset->endGroup();

    pset->beginGroup("RecordConfig");
    m_iSequencingId = pset->value("SequencingId").toInt();
    m_strRecordConfig = pset->value("url").toString().toStdString();
    m_strSequenceConfig = pset->value("sequrl").toString().toStdString();
    pset->endGroup();

    pset->beginGroup("WithAlgorithm");
    m_strUploadAddr = pset->value("uploadaddr").toString().toStdString();
    m_strTaskAddr = pset->value("taskaddr").toString().toStdString();
    m_iTaskPort = pset->value("taskport").toInt();
    m_iTaskDbnum = pset->value("taskdb").toInt();
    pset->endGroup();

    pset->beginGroup("VerUpdate");
    m_strIgnoreVer = pset->value("ignore").toString().toStdString();
    m_strVerupdateAddr = pset->value("verurl").toString().toStdString();
    m_strVerloadAddr = pset->value("updurl").toString().toStdString();
    pset->endGroup();

    InitData();

    return true;
}

bool ConfigServer::EnqueueVoltageInfo(const std::string& voltinfo)
{
    return m_queVoltageInfo.enqueue(voltinfo);
}

bool ConfigServer::DequeueVoltageInfo(std::string& voltinfo)
{
    return m_queVoltageInfo.try_dequeue(voltinfo);
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
        QString srcfile = QString(pprefix) + confname.string().c_str();
        QFile::copy(srcfile, QString(pconf));

        return true;
    }
    return false;
}

bool ConfigServer::GetCalibrationLineFit(const std::vector<std::pair<double, double>>& vctPoints, double& dSlope, double& dOffset)
{
    double m_xx = 0.0;
    double m_xSum = 0.0;
    double m_xy = 0.0;
    double m_ySum = 0.0;
    int pointSize = vctPoints.size();
    if (pointSize > 0)
    {
        for (int i = 0; i < pointSize; ++i)
        {
            auto& tppt = vctPoints[i];
            m_xx += tppt.first * tppt.first;
            m_xSum += tppt.first;
            m_xy += tppt.first * tppt.second;
            m_ySum += tppt.second;
        }
        auto div = (m_xx * pointSize - m_xSum * m_xSum);
        if (!ISDOUBLEZERO(div))
        {
            dSlope = (m_xy * pointSize - m_xSum * m_ySum) / (m_xx * pointSize - m_xSum * m_xSum);
            dOffset = (m_xx * m_ySum - m_xSum * m_xy) / (m_xx * pointSize - m_xSum * m_xSum);
            if (ISDOUBLEZERO(dSlope))
            {
                dSlope = 1.0;
                dOffset = 0.0;
            }
        }

        return true;
    }

    return false;
}

int ConfigServer::GetFileMD5Sum(const char* filefullpath, char* MD5key)
{
#ifdef Q_OS_WIN32
    if (strlen(filefullpath) > MAX_PATH)
        return -1;
    char szFilePath[MAX_PATH] = { 0 };
    strcpy(szFilePath, filefullpath);
    int i = 0;
    while (szFilePath[i] != 0)
    {
        if (szFilePath[i] == ('/'))
            szFilePath[i] = ('\\');
        i++;
    }
    char* pos = NULL;
    if ((pos = strchr(szFilePath, '\\')) == NULL)
    {
        return -1;
    }

    char szfilename[MAX_PATH] = { 0 };
    strcpy(szfilename, pos + 1);
    *pos = 0;
    if (strlen(szfilename) == 0 || strlen(szFilePath) == 0 || MD5key == NULL)
    {
        return -1;
    }
    char szCmdLine[MAX_PATH] = { 0 };
    sprintf(szCmdLine, "cmd.exe /c md5sum \"%s\" ", szfilename);
    //eg: cmd.exe /c md5sum "for_text.txt"
    SECURITY_ATTRIBUTES sa = { 0 };
    HANDLE hRead = NULL, hWrite = NULL;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return -2;
    }
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!CreateProcessA(NULL, szCmdLine, NULL, NULL, TRUE, NULL, NULL, szFilePath, &si, &pi))//¦Ð
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
    char buffer[MAX_PATH] = { 0 };
    DWORD bytesRead = 0;
    ReadFile(hRead, buffer, MAX_PATH, &bytesRead, NULL);
    CloseHandle(hRead);
    if (NULL != strstr(buffer, "md5sum"))
    {
        return -4;
    }
    else if (!strnicmp(buffer, "No such file:", strlen("No such file:")))
    {
        return -5;
    }
    if (strlen(buffer) < 32)
    {
        return -6;
    }
    strncpy(MD5key, buffer, 32);
    strcat(MD5key, "\0");

    return 0;

#else
    char buf_ps[1024];
    char ps[1024] = { 0 };
    FILE* ptr;
    strcpy(ps, filefullpath);
    if ((ptr = popen(ps, "r")) != NULL)
    {
        while (fgets(buf_ps, 1024, ptr) != NULL)
        {
            strcat(MD5key, buf_ps);
            if (strlen(MD5key) > 1024)
                break;
        }
        pclose(ptr);
        ptr = NULL;

        return 0;
    }
    //printf("popen %s error\n", ps);
    return -1;
#endif
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
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!CreateProcessA(NULL, szCmdLine, NULL, NULL, TRUE, NULL, NULL, nullptr, &si, &pi))//¦Ð
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

std::string ConfigServer::tostring(double val, int bit)
{
    char arform[64] = "%f";
    if (bit != -1)
    {
        sprintf(arform, "%%.%df", bit);
    }
    char arval[512] = { 0 };
    sprintf(arval, arform, val);
    return std::string(arval);
}

std::string ConfigServer::GetCurrentPath(void)
{
    return std::filesystem::current_path().string();
}

std::string ConfigServer::GetCurrentDateTime(void)
{
    time_t tt = ::time(nullptr);
    tm tm1;
#ifdef Q_OS_WIN32
    localtime_s(&tm1, &tt);
#else
    localtime_r(&tt, &tm1);
#endif
    char szTime[64] = { 0 };
    sprintf(szTime, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d",
        tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
        tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
    return std::string(szTime);
}

std::string ConfigServer::GetCurrentTimeMore(void)
{
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(std::chrono::milliseconds(timestamp));
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
    std::time_t t = sec.count();
    //os << '[' << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << '.' << timestamp % 1000 << ']';
    auto ptm = std::localtime(&t);
    char arret[1024] = { 0 };
    sprintf(arret, "%02d:%02d:%02d.%03lld", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, timestamp % 1000);
    return std::string(arret);
}

long long ConfigServer::GetCurrentTimestamp(void)
{
    auto curnow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return curnow.count();
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

#if 1

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

#else
    int sock_fd, intrface;
    struct ifreq buf[INET_ADDRSTRLEN];
    struct ifconf ifc;
    char* local_ip = NULL;

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(sock_fd, SIOCGIFCONF, (char*)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);
            while (intrface-- > 0)
            {
                if (!(ioctl(sock_fd, SIOCGIFADDR, (char*)&buf[intrface])))
                {
                    local_ip = NULL;
                    local_ip = inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr);
                    if (local_ip)
                    {
                        if (strcmp("127.0.0.1", local_ip) != 0)
                        {
                            strRetip = local_ip;
                            break;
                        }
                    }

                }
            }
        }
        close(sock_fd);
    }
#endif

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

std::string ConfigServer::GetUserPath(void)
{
#ifdef Q_OS_WIN32

    char szAppData[MAX_PATH] = { 0 };
    HRESULT hRet = SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, szAppData);
    if (SUCCEEDED(hRet))
    {
        return std::string(szAppData) + "/bgiCyclone/";
    }

#else

    char* homepath = getenv("HOME");
    if (homepath != nullptr)
    {
        return std::string(homepath) + "/bgiCyclone/";
    }

#endif
    return std::string("./");
}

std::string ConfigServer::GetDumpPath(void)
{
    auto strdump = ConfigServer::GetCurrentPath() + "/etc/Dump/";
    std::filesystem::path pathDump(strdump);
    if (!std::filesystem::exists(pathDump))
    {
        std::filesystem::create_directories(pathDump);
    }
    return strdump;
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

