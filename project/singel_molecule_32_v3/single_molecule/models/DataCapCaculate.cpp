#include "DataCapCaculate.h"
#include "models/DataDefine.h"
#include "models/DatacCommunicaor.h"
#include "logger/Log.h"


DataCapCaculate::DataCapCaculate()
{
     QueryPerformanceFrequency(&m_frequency);

}

DataCapCaculate::~DataCapCaculate()
{


}
float DataCapCaculate::GetMean(const QVector<float> &in)
{
    float sum = std::accumulate(in.begin(), in.end(), 0.0);
    float mean =  sum / in.size();
    return mean;
}

float DataCapCaculate::CalCapacitance(const float &meanCurrent)
{
    g_pCommunicaor->g_capParameter.CapMutex.lockForRead();
    float div = g_pCommunicaor->g_capParameter.triangleAmp * g_pCommunicaor->g_capParameter.triangleFreq;
    g_pCommunicaor->g_capParameter.CapMutex.unlock();
    if(div == 0)
        return 0;
    return meanCurrent*1.0/(4*div);
}



void DataCapCaculate::run()
{
    LOGI("Start run DataCapCaculate thread.");
    qDebug()<<"DataCapCaculate run thread id :" <<GetCurrentThreadId();
    int nRet = THREAD_NORMAL_EXIT;


    int nMeansCapSemCnt = 0;

    bool bCapResult = false;
    int nCapResSemCnt= 0;
    int nCapResPointCnt =0;

    while(m_bRunThread)
    {
        try
        {
            if(g_pCommunicaor->m_sCapCatchSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
            {

                QVector<float> result;
                result.resize(CHANNEL_NUM);
                if(g_pCommunicaor->m_vctMeansCapBuf[0 +  nMeansCapSemCnt * CHANNEL_NUM].size() > 0)
                {
                    for(int ch = 0; ch < CHANNEL_NUM; ch++)
                    {
                        float average = GetMean(g_pCommunicaor->m_vctMeansCapBuf[ch +  nMeansCapSemCnt * CHANNEL_NUM]);
                        result[ch] = CalCapacitance(average);
    //                      result[ch] = average;
                    }
                }

                if(bCapResult == false)
                {
                    if(g_pCommunicaor->m_sCapResSem.freeBuf.tryAcquire(1))
                    {
                        for(int ch = 0; ch < CHANNEL_NUM; ch++)
                        {
                            g_pCommunicaor->m_vctCapResBuf[ch + CHANNEL_NUM * nCapResSemCnt].clear();
                        }
                        bCapResult = true;
                    }
    //                else
    //                    qDebug()<<"m_sCapResSem freeBuf is tryacquire failed";
                }
                if(bCapResult == true)
                {
                    for(uint16_t ch = 0; ch < CHANNEL_NUM; ch++)
                    {
                        g_pCommunicaor->m_vctCapResBuf[ch + nCapResSemCnt * CHANNEL_NUM].push_back(result.at(ch));
                    }

                    if(nCapResPointCnt++ >= MAX_POINTS_FOR_CAPPOINTS)
                    {
                         g_pCommunicaor->m_sCapResSem.usedBuf.release();
                         nCapResSemCnt  = (nCapResSemCnt == (MEANSCAPRES_BUF_BLOCK_END_INDEX)) ? 0 : (nCapResSemCnt+1);
                         nCapResPointCnt = 0;
                         bCapResult = false;
                    }
                }


    //            qDebug()<<result;

                g_pCommunicaor->m_sCapCatchSem.freeBuf.release();
                nMeansCapSemCnt = (nMeansCapSemCnt == MEANSCAP_BUF_BLOCK_END_INDEX) ? 0 : (nMeansCapSemCnt+1);
            }
        }
        catch (std::exception &e)
       {
            LOGI("DataCapCaculate thread {}",e.what());
            continue;
       }
    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit DataCapCaculate thread.");
}

void DataCapCaculate::startThread()
{
    qDebug()<<"DataCollection startThread";
    m_bRunThread = true;
    this->start();

}
void DataCapCaculate::stopThread()
{
    qDebug()<<"DataCollection stopThread";
    m_bRunThread = false;
}
