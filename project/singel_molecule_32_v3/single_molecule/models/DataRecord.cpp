#include "DataRecord.h"
#include<thread>
#include"models/DatacCommunicaor.h"
#include"logger/Log.h"
#include <time.h>


#define g_pCommunicaor DataCommunicaor::Instance()

#define TIME_STR 0
#define TIME_STD 1
const long WRITE_FILE_SIZE = 100*1024*1024;
using namespace std;

DataRecord::DataRecord()
{
    m_pFile.resize(CHANNEL_NUM);
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        m_pFile[ch] = nullptr;
    }
    m_flushTimer = nullptr;

}

DataRecord::~DataRecord()
{
    closeFile();
}


void DataRecord::updateSavePathSlot(const QString savePath)
{
    closeFile();
    if(openFile(savePath.toStdString()))
    {
        g_pCommunicaor->g_IsSaveToFiles = true;
        m_flushTimer = new QTimer;
        connect(m_flushTimer, SIGNAL(timeout()), this, SLOT(fflushFile()));
        m_flushTimer->start(5000);
    }
}


bool DataRecord::CreatNewFileName(const QString &dirUrl,int channel,QString *reName)
{
     QDir nRecordDir(dirUrl);
     if(!nRecordDir.exists()) //判断文件夹是否存在
     {
        *reName = "";
        return false;
     }
    QStringList nNameFilter(QStringLiteral("channel%1_*").arg(channel));
    QFileInfoList fileList = nRecordDir.entryInfoList(nNameFilter,QDir::Files,QDir::Name);  //获取本文件夹内所有文件的信息
    int fileCount = fileList.count(); //获取本文件夹内的文件个数
    qDebug()<<"count:"<<fileCount;
    *reName =dirUrl + QString("channel%1_%2.dat").arg(channel).arg(QString::number(fileCount),3,'0');
    return true;
}

bool DataRecord::openFile(const std::string &savePath)
{
    bool bRet = true;
    m_sFileSavePath = savePath;
    std::string sFileName = m_sFileSavePath;

    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        if(m_pFile.at(ch) == nullptr)
        {
            std::string nChFileNameStr =  sFileName + "channel"+QString::number(ch+1).toStdString() + ".dat";
            m_pFile[ch] = fopen(nChFileNameStr.c_str(),"ab");
            if(m_pFile.at(ch) == nullptr)
            {
                bRet = false;
                LOGE("Open file: {} failed.", sFileName);
            }
        }
    }

    return bRet;
}


bool DataRecord::reOpenFile(int channel)
{
    bool bRet = true;
    int ch = channel;
    if(m_pFile.at(ch) != nullptr)
    {
        fclose(m_pFile.at(ch));
        m_pFile[ch] =  nullptr;
        std::string nChFileNameStr =  m_sFileSavePath + "channel"+QString::number(ch+1).toStdString() + ".dat";
        std::string sNewFileName = m_sFileSavePath + "channel" +QString::number(ch+1).toStdString() \
                + (QTime::currentTime().toString("_hhmmss")).toStdString()+ ".dat";
        if(rename(nChFileNameStr.c_str(),sNewFileName.c_str())<0)
        {
            bRet = false;
            LOGI("Rename file: {} Failed.", nChFileNameStr);
        }
        m_pFile[ch] = fopen(nChFileNameStr.c_str(),"wb");
        if(m_pFile.at(ch) == nullptr)
        {
            bRet = false;
            LOGE("reOpen file: {} failed.", nChFileNameStr);
        }
    }
    else
    {
         bRet = false;
         LOGI("Channel {} file is nullptr.", ch);

    }
    return bRet;
}


void DataRecord::fflushFile(void)
{
    for(int _ch = 0; _ch < CHANNEL_NUM; _ch++)
    {
        if (m_pFile.at(_ch) != NULL)
        {
            fflush(m_pFile.at(_ch));
        }
    }
}


void DataRecord::SaveDataSlot(const QVector<QVector<float>> vctData)
{
    if(!g_pCommunicaor->g_IsSaveToFiles)
    {
        return;
    }
    for(int ch = 0; ch < vctData.size(); ch ++)
    {
        if (m_pFile.at(ch) != NULL)
        {
           if(vctData.at(ch).size() == 0)
           {
               qDebug()<<ch<<"m_sSaveSem FILE_WRITE_ERROR";
               throw FILE_WRITE_ERROR;
           }

           if(fwrite(&vctData.at(ch).at(0),sizeof(float),vctData.at(ch).size(),m_pFile.at(ch)) != vctData.at(ch).size())
           {
               qDebug()<<ch<<"m_sSaveSem FILE_WRITE_ERROR";
               throw FILE_WRITE_ERROR;
           }
         }
    }
}

void DataRecord::closeFile()
{
    if(m_flushTimer != nullptr)
    {
        if(m_flushTimer->isActive())
        {
            m_flushTimer->stop();
        }
        delete m_flushTimer;
        m_flushTimer = nullptr;
    }
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        if(m_pFile.at(ch) != NULL)
        {
            fflush(m_pFile.at(ch));
            fclose(m_pFile.at(ch));
            m_pFile[ch] = nullptr;
        }
    }
}



std::string DataRecord::getTimeStr(int nFormat)
{
    SYSTEMTIME sysTime;
    char dateStr[50] = {0};
    GetLocalTime(&sysTime);
    if(nFormat == 1)
    {
        sprintf(dateStr, "%4d/%02d/%02d_%02d:%02d:%02d.%03d,",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds);
    }else
    {
        sprintf(dateStr, "%4d_%02d_%02d_%02d_%02d_%02d_%03d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds);
    }
    return(string)dateStr;
}
