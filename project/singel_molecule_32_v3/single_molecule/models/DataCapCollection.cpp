#include "DataCapCollection.h"
#include "models/DataDefine.h"
#include"models/DatacCommunicaor.h"
#include"logger/Log.h"

DataCapCollection::DataCapCollection()
{


}

DataCapCollection::~DataCapCollection()
{


}

void DataCapCollection::run()
{
    LOGI("Start run DataCapCollection thread.");
    qDebug()<<"DataCapCollection run thread id :" <<GetCurrentThreadId();
    int nRet = THREAD_NORMAL_EXIT;
    float (*pChannel)[CHANNEL_NUM] = g_pCommunicaor->m_VctChannelBuf;

    int nCapCollectSemCnt = 0;
    int nMeansCapSemCnt = 0;
    bool bCapStart = false;

    initTimeCnt();

    float dCurTime = 0;
    float dutyTrangle = 0;

    int nSemTryTime  = SEM_TRY_TIME;

    while(m_bRunThread)
    {
        try
        {
            if(g_pCommunicaor->m_sCapCollectSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
            {
                if(true == g_pCommunicaor->g_capParameter.IsTrangleWave)
                {
                    g_pCommunicaor->g_capParameter.CapMutex.lockForRead();
                    dutyTrangle = 1.0/g_pCommunicaor->g_capParameter.triangleFreq *1000 ;//Ms
                    g_pCommunicaor->g_capParameter.CapMutex.unlock();

                    if(bCapStart == false)
                    {
                      if(g_pCommunicaor->m_sCapCatchSem.freeBuf.tryAcquire(1))
                      {
                          for(int ch = 0; ch < CHANNEL_NUM; ch++)
                          {
                              g_pCommunicaor->m_vctMeansCapBuf[ch + CHANNEL_NUM * nMeansCapSemCnt].clear();
                          }
                          bCapStart = true;
                      }
                      else
                           qDebug()<<"m_sCapCatchSem freeBuf is tryacquire failed";
                    }
                    if(bCapStart == true)
                    {
                      dCurTime = getTimeMsDifference();  //Ms
                      for(uint16_t ch = 0; ch < CHANNEL_NUM; ch++)
                      {
                         float temp = *(*(pChannel + nCapCollectSemCnt) + ch);
                         if(temp > 0 )
                         {
                            g_pCommunicaor->m_vctMeansCapBuf[ch + nMeansCapSemCnt * CHANNEL_NUM].push_back(temp);
                         }
                      }
                      if(dCurTime > dutyTrangle)
                      {
                          setReStartCnt();
                          g_pCommunicaor->m_sCapCatchSem.usedBuf.release();
                          nMeansCapSemCnt  = (nMeansCapSemCnt == (MEANSCAP_BUF_BLOCK_END_INDEX)) ? 0 : (nMeansCapSemCnt+1);
                          bCapStart = false;
                      }
                    }
                }
                else
                {
                    setReStartCnt();
                }

                g_pCommunicaor->m_sCapCollectSem.freeBuf.release();
                nCapCollectSemCnt = (nCapCollectSemCnt == CH_BUF_BLOCK_END_INDEX) ? 0 : (nCapCollectSemCnt+1);
            }
            else
            {
                qDebug()<<"m_sCapCollectSem usedBuf is tryacquire failed";
                nRet = THREAD_CHANNEL_USED_SEM_ACQ_TIMEOUT;
                if(nSemTryTime-- == 0)
                {
                    LOGI("m_sCapCollectSem usedBuf is tryacquire failed");
                    stopThread();
                    break;
                }
            }
        } catch (std::exception &e) {
            LOGI("DataCapCollection thread {}",e.what());
            stopThread();
        }

    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit DataCapCollection thread.");
}

void DataCapCollection::startThread()
{
    qDebug()<<"DataCapCollection startThread";
    m_bRunThread = true;
    this->start();

}
void DataCapCollection::stopThread()
{
    qDebug()<<"DataCapCollection stopThread";
    m_bRunThread = false;
}

inline void DataCapCollection::initTimeCnt()
{
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_startCnt);
}

inline void DataCapCollection::setReStartCnt()
{
    QueryPerformanceCounter(&m_startCnt);
}


inline float DataCapCollection::getTimeMsDifference()
{
    QueryPerformanceCounter(&m_stopCnt);
    return ((m_stopCnt.QuadPart - m_startCnt.QuadPart) * 1000 / m_frequency.QuadPart);  //mS
}
