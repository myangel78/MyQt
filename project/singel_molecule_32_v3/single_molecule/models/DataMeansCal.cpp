#include "DataMeansCal.h"
#include "models/DataDefine.h"
#include"models/DatacCommunicaor.h"
#include"logger/Log.h"


DataMeansCal::DataMeansCal(QWidget *parent)
{
    QueryPerformanceFrequency(&frequency);

}

DataMeansCal::~DataMeansCal()
{


}



float DataMeansCal::GetMean(QVector<float> &in)
{
    float sum = std::accumulate(in.begin(), in.end(), 0.0);
    float mean =  sum / in.size();
    return mean;
}


void DataMeansCal::run()
{
    LOGI("Start run DataMeansCal thread.");
    qDebug()<<"DataMeansCal run thread id :" <<GetCurrentThreadId();
    int nRet = THREAD_NORMAL_EXIT;
    int nMeansSemCnt = 0;
    QVector<QVector<float>> vctDegatttingBuf;
    vctDegatttingBuf.resize(CHANNEL_NUM);


    while(m_bRunThread)
    {
        try
        {
            QueryPerformanceCounter(&startCount);
            if(g_pCommunicaor->m_sMeansCalSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
            {
                QVector<float> result;
                result.resize(CHANNEL_NUM);
                if(g_pCommunicaor->m_vctMeansCalBuf[0 +  nMeansSemCnt * CHANNEL_NUM].size() > 0)
                {
                    for(int ch = 0; ch < CHANNEL_NUM; ch++)
                    {
                        float average = GetMean(g_pCommunicaor->m_vctMeansCalBuf[ch +  nMeansSemCnt * CHANNEL_NUM]);
                        result[ch] = average;
                    }
                }

                if(g_pCommunicaor->g_bDegattingRunning && (g_pCommunicaor->g_bZeroAdjustProcess  == false)) //调零跟degating冲突
                {
                    for(int ch = 0; ch < CHANNEL_NUM; ch++)
                    {
                        vctDegatttingBuf[ch].clear();
                        vctDegatttingBuf[ch].swap(g_pCommunicaor->m_vctMeansCalBuf[ch +  nMeansSemCnt * CHANNEL_NUM]);
    //                    qDebug()<<"after "<<g_pCommunicaor->m_vctMeansCalBuf[ch +  nMeansSemCnt * CHANNEL_NUM].size()<<vctDegatttingBuf[ch].size();
                    }
                    emit DegatingBufSig(vctDegatttingBuf);
                }
                if(g_pCommunicaor->g_bZeroAdjust) //调零校准
                {
                    emit ZeroOffsetResSig(result); //发送均值；
                }

                emit MeansResultSig(result);

                if(g_pCommunicaor->g_bCalibration)
                {
                    emit CaliMeanstResSig(result);
                }

                g_pCommunicaor->m_sMeansCalSem.freeBuf.release();
                nMeansSemCnt = (nMeansSemCnt == MEANSCAL_BUF_BLOCK_END_INDEX) ? 0 : (nMeansSemCnt+1);
                QueryPerformanceCounter(&stopCount);
                elapsed = (stopCount.QuadPart - startCount.QuadPart) * 1000000 / frequency.QuadPart;
    //            qDebug()<<"elapsed = "<<elapsed<<"us ";
            }
    //        else
    //        {
    //            qDebug()<<"m_sMeansCalSem usedBuf is tryacquire failed";
    //            nRet = THREAD_CHANNEL_USED_SEM_ACQ_TIMEOUT;
    //            stopThread();
    //            break;
    //        }

        }
        catch (std::exception &e)
        {
            LOGI("DataMeansCal thread {}",e.what());
        }
    }

    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit DataMeansCal thread.");
}

void DataMeansCal::startThread()
{
    qDebug()<<"DataMeansCal startThread";
    m_bRunThread = true;
    this->start();

}
void DataMeansCal::stopThread()
{
    qDebug()<<"DataMeansCal stopThread";
    m_bRunThread = false;
}


