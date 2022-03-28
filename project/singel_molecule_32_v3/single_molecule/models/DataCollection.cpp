#include "DataCollection.h"
#include "models/DataDefine.h"
#include"models/DatacCommunicaor.h"
#include"logger/Log.h"
//#include <iostream>
//using namespace  std;
//float sum = std::accumulate(std::begin(resultSet), std::end(resultSet), 0.0);
//float mean =  sum / resultSet.size(); //均值


DataCollection::DataCollection()
{


}

DataCollection::~DataCollection()
{


}

float DataCollection::GetMean(QVector<float> &in)
{
    float sum = std::accumulate(in.begin(), in.end(), 0.0);
    float mean =  sum / in.size();
    return mean;
}




void DataCollection::run()
{
    LOGI("Start run DataCollection thread.");
    qDebug()<<"DataCollection run thread id :" <<GetCurrentThreadId();
    int nRet = THREAD_NORMAL_EXIT;
    float (*pChannel)[CHANNEL_NUM] = g_pCommunicaor->m_VctChannelBuf;

    int nCollectSemCnt = 0;
    int nMeansSemCnt = 0;
    bool bTimeUp = false;
    float nDispValueArray[CHANNEL_NUM] = {0};

    initTimeCnt();

    float dCurTime = 0;
    float dMeanTime = 0;

    int nSemTryTime = SEM_TRY_TIME;

    while(m_bRunThread)
    {
        try {
            if(g_pCommunicaor->m_sMeansCalSem.freeBuf.tryAcquire(1,SEM_TIMEOUT))
            {
                for(int i = 0; i < CHANNEL_NUM; i++)
                {
                     g_pCommunicaor->m_vctMeansCalBuf[i + CHANNEL_NUM*nMeansSemCnt].clear();
                }
                bTimeUp = true;
                while(m_bRunThread && bTimeUp)
                {
                    if(g_pCommunicaor->m_sCollectSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
                    {
                        for(uint16_t ch = 0; ch < CHANNEL_NUM; ch++)
                        {
                            float temp = *(*(pChannel + nCollectSemCnt) + ch);
                            g_pCommunicaor->m_vctMeansCalBuf[ch + nMeansSemCnt * CHANNEL_NUM].push_back(temp);
                            nDispValueArray[ch] = temp;
                        }
                        dCurTime = getTimeMsDifference();  //ms
                        if(dCurTime > DISP_DUTY_TIME)
                        {
                            QVector<float> dispTmp(nDispValueArray,nDispValueArray+CHANNEL_NUM);
                            emit dispInstantCurrentSig(dispTmp);
                            setReStartCnt();
                            dMeanTime +=dCurTime;
                            if(dMeanTime > MEANS_DUTY_TIME)
                            {
                                dMeanTime = 0;
                                bTimeUp = false;
                            }

                        }
                        g_pCommunicaor->m_sCollectSem.freeBuf.release();
                        nCollectSemCnt = (nCollectSemCnt == CH_BUF_BLOCK_END_INDEX) ? 0 : (nCollectSemCnt+1);
                    }
                    else
                    {
                        qDebug()<<"m_sCollectSem usedBuf is tryacquire failed";
                        nRet = THREAD_CHANNEL_USED_SEM_ACQ_TIMEOUT;
                        stopThread();
                        break;
                    }
                }

                g_pCommunicaor->m_sMeansCalSem.usedBuf.release();
                nMeansSemCnt  = (nMeansSemCnt == (MEANSCAL_BUF_BLOCK_END_INDEX)) ? 0 : (nMeansSemCnt+1);
             }else
             {
                 nRet = THREAD_PLOT_FREE_SEM_ACQ_TIMEOUT;
//                 stopThread();
                 if(nSemTryTime-- == 0)
                 {
                     LOGI("DataCollection freeBuf is tryacquire failed");
                     stopThread();
                     break;
                 }
             }
        } catch (std::exception &e) {
            LOGI("DataCollection thread {}",e.what());
            stopThread();
        }
    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit DataCollection thread.");
}

void DataCollection::startThread()
{
    qDebug()<<"DataCollection startThread";
    m_bRunThread = true;
    this->start();

}
void DataCollection::stopThread()
{
    qDebug()<<"DataCollection stopThread";
    m_bRunThread = false;
}

inline void DataCollection::initTimeCnt()
{
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_startCnt);
}

inline void DataCollection::setReStartCnt()
{
    QueryPerformanceCounter(&m_startCnt);
}


inline float DataCollection::getTimeMsDifference()
{
    QueryPerformanceCounter(&m_stopCnt);
    return ((m_stopCnt.QuadPart - m_startCnt.QuadPart) * 1000 / m_frequency.QuadPart);  //mS
}
