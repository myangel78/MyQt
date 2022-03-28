#include "DataMonitor.h"
#include<thread>
#include"QDebug"
#include"models/DatacCommunicaor.h"

#define g_pCommunicaor DataCommunicaor::Instance()

DataMonitor::DataMonitor()
{

}

DataMonitor::~DataMonitor()
{
}

void DataMonitor::threadFunc()
{    
    int nRet = THREAD_NORMAL_EXIT;
    uint16_t nChSemCnt = 0, nPlotSemCnt = 0;
    double dCurTime,nMsCnt = PLOT_REFRESH_INTREVAL_MS * 0.001;
    QCPGraphData temp;
    temp.key = 0;

    std::vector<float> vctTempBuf;
    for(int i = 0; i < CHANNEL_NUM; i++)
    {
        vctTempBuf.push_back(0);
    }
    float (*pChannel)[CHANNEL_NUM] = g_pCommunicaor->m_VctChannelBuf;
    float *pXtimeUs  =  &g_pCommunicaor->g_xTimeUs;

    LOGI("Start run DataMonitor thread.");
    initTimeCnt();

    int nSemTryTime = SEM_TRY_TIME;

    while(m_bRunThread)
    {
        try
        {
            if(g_pCommunicaor->m_sPlotSem.freeBuf.tryAcquire(1,SEM_TIMEOUT))
            {
               for(int i = 0; i < CHANNEL_NUM; i++)
               {
                    g_pCommunicaor->m_VctPlotBuf[i + CHANNEL_NUM*nPlotSemCnt].clear();
               }

               if(temp.key == 0)
               {
                    for(int i = 0; i < CHANNEL_NUM; i++)
                    {
                        temp.value =  vctTempBuf[i];
                        g_pCommunicaor->m_VctPlotBuf[i + CHANNEL_NUM*nPlotSemCnt].push_back(temp);
                    }
               }

               bool bGetPlotData = true;
               while(bGetPlotData && m_bRunThread)
               {
                   if(g_pCommunicaor->m_sChannelSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
                   {
                       dCurTime = getTimeDifference();  //s
                       if(dCurTime > temp.key)
                       {
                           temp.key = dCurTime;
    //                       if(temp.key >= XRANGE_S)
                           if(temp.key >= *pXtimeUs)
                           {
                               temp.key = 0.0;
                               nMsCnt = PLOT_REFRESH_INTREVAL_MS * 0.001;
                               setReStartCnt();

                               for(int i = 0; i < CHANNEL_NUM; i++)
                               {
                                   vctTempBuf[i] = *(*(pChannel+nChSemCnt)+i);
                               }
                               bGetPlotData = false;
                            }
                            else
                            {
                               for(int i = 0; i < CHANNEL_NUM; i++)
                               {
                                   temp.value = *(*(pChannel+nChSemCnt)+i);
                                   g_pCommunicaor->m_VctPlotBuf[i + CHANNEL_NUM*nPlotSemCnt].push_back(temp);
                               }

                               if(temp.key >= nMsCnt)
                               {
                                   nMsCnt += PLOT_REFRESH_INTREVAL_MS * 0.001;
                                   bGetPlotData = false;
                               }
                           }
                       }
                       g_pCommunicaor->m_sChannelSem.freeBuf.release();
                       nChSemCnt = (nChSemCnt == CH_BUF_BLOCK_END_INDEX) ? 0 : (nChSemCnt+1);
                   }else
                   {
                       nRet = THREAD_CHANNEL_USED_SEM_ACQ_TIMEOUT;
                       stopThread();
                   }
               }

               g_pCommunicaor->m_sPlotSem.usedBuf.release();
               nPlotSemCnt = (nPlotSemCnt == PLOT_BUF_BLOCK_END_INDEX) ? 0 : (nPlotSemCnt+1);
            }else
            {
                nRet = THREAD_PLOT_FREE_SEM_ACQ_TIMEOUT;
//                stopThread();
                if(nSemTryTime-- == 0)
                {
                    LOGI("m_sPlotSem.freeBuf.tryAcquire");
                    stopThread();
                    break;
                }
            }
        }
        catch (std::exception &e)
        {
            LOGI("DataMonitor thread {}",e.what());
            stopThread();
            continue;
        }
    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit DataMonitor thread.");
}



inline void DataMonitor::initTimeCnt()
{
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_startCnt);
}

inline void DataMonitor::setReStartCnt()
{
    QueryPerformanceCounter(&m_startCnt);
}

inline void DataMonitor::setStopCnt()
{
     QueryPerformanceCounter(&m_stopCnt);
}

inline double DataMonitor::getTimeDifference()
{
    QueryPerformanceCounter(&m_stopCnt);
    return ((m_stopCnt.QuadPart - m_startCnt.QuadPart) * 10000 / m_frequency.QuadPart)*0.0001;  //S
}


