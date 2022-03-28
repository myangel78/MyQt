#include"DataOutputFormat.h"
#include"models/DatacCommunicaor.h"
#include"logger/Log.h"
#include<thread>
#include<vector>

#define g_pCommunicaor DataCommunicaor::Instance()

using namespace std;


DataOutputFormat::DataOutputFormat()
{
//    QueryPerformanceFrequency(&m_frequency);
}

DataOutputFormat::~DataOutputFormat()
{
}


void DataOutputFormat::run()
{
    LOGI("Start DataOutputFormat thread.");
    int nRet = THREAD_NORMAL_EXIT;
    uint16_t nOutputFormatSemCnt = 0, nSaveSemCnt = 0;
    float (*pChannel)[CHANNEL_NUM] = g_pCommunicaor->m_VctChannelBuf;
    QTime startTime,stopTime;
    int nMsElapsed  = 0;
    bool bGetSaveData = false;
    QVector<QVector<float>> vctDataBuf;
    vctDataBuf.resize(CHANNEL_NUM);

    int nSemTryTime = SEM_TRY_TIME;

    while(m_bRunThread)
    {
        try
        {
            if(g_pCommunicaor->m_sOutputFormatSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
            {
                    if(bGetSaveData == false)
                    {
                        bGetSaveData = true;
                        startTime = QTime::currentTime();
                        for(auto && singleChannel: vctDataBuf)
                        {
                            singleChannel.clear();
                        }
                    }
                    if(bGetSaveData && m_bRunThread)
                    {
                        for(uint16_t ch = 0; ch < CHANNEL_NUM; ch++)
                        {
                             float temp = *(*(pChannel + nOutputFormatSemCnt) + ch);
                             vctDataBuf[ch].push_back(temp);
                        }
                        stopTime = QTime::currentTime();
                        nMsElapsed = startTime.msecsTo(stopTime);
                        if(nMsElapsed  > 50)
                        {
                            if(g_pCommunicaor->g_IsSaveToFiles)
                            {
                                emit sendSaveDataBufSig(vctDataBuf);
                            }
                            bGetSaveData = false;
                        }

                    }
                    g_pCommunicaor->m_sOutputFormatSem.freeBuf.release();
                    nOutputFormatSemCnt = (nOutputFormatSemCnt == CH_BUF_BLOCK_END_INDEX) ? 0 : (nOutputFormatSemCnt+1);
            }
            else
            {
                qDebug()<<"m_sOutputFormatSem usedBuf is tryacquire failed";
                nRet = THREAD_CHANNEL_USED_SEM_ACQ_TIMEOUT;
//                stopThread();
//                break;
                if(nSemTryTime-- == 0)
                {
                    LOGI("m_sOutputFormatSem usedBuf is tryacquire failed");
                    stopThread();
                    break;
                }
            }
        } catch (std::exception &e)
        {
            LOGI("DataOutputFormat thread {}",e.what());
            stopThread();
        }
    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit DataOutputFormat thread.");
}


void DataOutputFormat::startThread()
{
    qDebug()<<"DataOutputFormat startThread";
    m_bRunThread = true;
    this->start();

}
void DataOutputFormat::stopThread()
{
    qDebug()<<"DataOutputFormat stopThread";
    m_bRunThread = false;
}


