#include "models/DataStackHandle.h"
#include "models/DataDefine.h"
#include"models/DatacCommunicaor.h"
#include<models/doubleToString/fpconv.h>
#include"logger/Log.h"
#include<thread>
#include "models/DataPackageMode.h"
#include "helper/appconfig.h"


#define g_pCommunicaor DataCommunicaor::Instance()
#define ADC_LSB 152



DataStackHandle::DataStackHandle(QObject *parent):
    QThread(parent)
{

    for(auto &value: m_nSlopeArray)
        value = 2.0;
    for(auto &value: m_nOffsetArray)
        value = 0;


    QueryPerformanceFrequency(&frequency);

}


DataStackHandle::~DataStackHandle()
{

}


void DataStackHandle::run()
{
    qDebug()<<"DataStackHandle run thread id :" <<GetCurrentThreadId();
    int nRet = THREAD_NORMAL_EXIT;
    uint16_t nRawSemCnt = 0, nCHSemCnt = 0;
    uint16_t nRawCH = 0, nRealCH =0,dataLen=0;

    LOGI("Start run DataStackHandle thread.");
    short int tempValue[FRAME_GROUP_NUM][CHANNEL_NUM] = {0};
    QByteArray *pRaw = &g_pCommunicaor->m_VctRawBuf[0];
    float (*pChannel)[CHANNEL_NUM] = g_pCommunicaor->m_VctChannelBuf;
    float *pChannelDisp = g_pCommunicaor->m_VctDispBuf;

    short int temp  = 0;
    float nVoltmv  = 0.0;
    float result   = 0;

    int nSemTryTime = SEM_TRY_TIME;

    while(m_bRunThread)
    {
        try {
                if(g_pCommunicaor->m_sRawSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
                {
                    dataLen = (*(pRaw+nRawSemCnt)).size();
                    uint16_t nGroup = 0;
                    for(uint16_t i = 0,s = 0; s < dataLen; s=s+2, ++i)
                    {
                        if(i == FRAME_DATA_TOTAL_CNT)
                        {
                            i = 0;
                            nGroup=0;
                        }
                        #if(CHANNEL_NUM == 256)
                            nRawCH = (i & 0xff);
                        #else if(CHANNEL_NUM == 32)
                            nRawCH = (i & 0x1f);
                        #endif

                        #ifndef DEMO_TEST
                            nRealCH = CHANNEL_TRANSLATION_TABLE[nRawCH];
                        #else
                            nRealCH = nRawCH;
                        #endif

                        tempValue[nGroup][nRealCH]= static_cast<short int>((uchar)g_pCommunicaor->m_VctRawBuf[nRawSemCnt].at(s) \
                                + (((uchar)g_pCommunicaor->m_VctRawBuf[nRawSemCnt].at(s+1))<<8));

                        if(nRawCH == 0)
                        {
                            if(!g_pCommunicaor->m_sChannelSem.freeBuf.tryAcquire(1,SEM_TIMEOUT))
                            {
                                nRet = THREAD_CHANNEL_FREE_SEM_ACQ_TIMEOUT;
                                LOGI("m_sChannelSem.freeBuf.tryAcquire failed");
                                continue;
//                                stopThread();
//                                break;
                            }
                            if(!g_pCommunicaor->m_sCollectSem.freeBuf.tryAcquire(1,SEM_TIMEOUT))
                            {
                                nRet = THREAD_OUTPUT_FREE_SEM_ACQ_TIMEOUT;
                                LOGI("m_sCollectSem.freeBuf.tryAcquire failed");
                                g_pCommunicaor->m_sChannelSem.freeBuf.release();
                                continue;
//                                stopThread();
//                                break;
                            }
                            if(!g_pCommunicaor->m_sOutputFormatSem.freeBuf.tryAcquire(1,SEM_TIMEOUT))
                            {
                                nRet = THREAD_OUTPUT_FREE_SEM_ACQ_TIMEOUT;
                                LOGI("m_sOutputFormatSem.freeBuf.tryAcquire failed");
                                g_pCommunicaor->m_sChannelSem.freeBuf.release();
                                g_pCommunicaor->m_sCollectSem.freeBuf.release();
                                continue;
//                                stopThread();
//                                break;
                            }
                            if(!g_pCommunicaor->m_sCapCollectSem.freeBuf.tryAcquire(1,SEM_TIMEOUT))
                            {
                                nRet = THREAD_OUTPUT_FREE_SEM_ACQ_TIMEOUT;
                                qDebug()<<"m_sCapCollectSem.freeBuf.tryAcquire failed";
                                LOGI("m_sCapCollectSem.freeBuf.tryAcquire failed");
                                g_pCommunicaor->m_sChannelSem.freeBuf.release();
                                g_pCommunicaor->m_sCollectSem.freeBuf.release();
                                g_pCommunicaor->m_sOutputFormatSem.freeBuf.release();
                                continue;
//                                stopThread();
//                                break;
                            }
                        }


                        temp = tempValue[nGroup][nRealCH];
                        nVoltmv = ProtocolModule_DacHexToVolageDouble(temp,DACMAX_FIVEVOLT,DACMAX_BITRANGE_SIXTEEN)*1000;
                        if(g_pCommunicaor->g_bCalibration)
                        {
                             result = nVoltmv;
                        }
                        else
                        {
                            if(g_pCommunicaor->g_bZeroAdjust)
                            {
                                result = (nVoltmv - m_nOffsetArray[nRealCH])/m_nSlopeArray[nRealCH] - readZeroOffset(nRealCH);
                            }
                            else
                            {
                                result  = (nVoltmv - m_nOffsetArray[nRealCH])/m_nSlopeArray[nRealCH]- m_nOffsetZeroArray[nRealCH];
                            }

                        }

                       *(*(pChannel + nCHSemCnt) + nRealCH) = result;                                   //给moitor模块
                       *(pChannelDisp + nRealCH) =  result;              //给displya模块

                        if(nRawCH == CHANNEL_NUM_END_INDEX)
                        {

                           g_pCommunicaor->m_sChannelSem.usedBuf.release();
                           g_pCommunicaor->m_sCollectSem.usedBuf.release();
                           g_pCommunicaor->m_sCapCollectSem.usedBuf.release();
                           g_pCommunicaor->m_sOutputFormatSem.usedBuf.release();
                           nCHSemCnt = (nCHSemCnt == CH_BUF_BLOCK_END_INDEX) ? 0 : (nCHSemCnt + 1);
                           nGroup++;
                        }

                    }
                    g_pCommunicaor->m_VctRawBuf[nRawSemCnt].clear();
                    g_pCommunicaor->m_sRawSem.freeBuf.release();
                    nRawSemCnt = (nRawSemCnt == (RAW_BUF_BLOCK_END_INDEX)) ? 0 : (nRawSemCnt+1);
                }else
                {
                    qDebug()<<"m_sRawSem.usedBuf.tryAcquire failed";
                    nRet = THREAD_RAW_USED_SEM_ACQ_TIMEOUT;
//                    stopThread();
                    if(nSemTryTime-- == 0)
                    {
                        LOGI("m_sRawSem.usedBuf.tryAcquire failed");
                        stopThread();
                        break;
                    }
                }
            }
            catch (std::exception &e)
            {
                LOGI("DataStackHandle thread {}",e.what());
                stopThread();
            }
    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit DataStackHandle thread.");
}




void DataStackHandle::LoadConfig(QSettings *nConfig)
{
    if(!nConfig)
    {
        qDebug()<<"DataStackHandle nConfig is nullptr";
        return;
    }

    m_nConfig = nConfig;
    nConfig->beginGroup("Calibration");
    // load channels settings
    nConfig->beginReadArray("Channels");
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        nConfig->setArrayIndex(i);
        m_nSlopeArray[i]  = nConfig->value("Slop").toDouble();
        m_nOffsetArray[i] = nConfig->value("Offset").toDouble();

    }
    nConfig->endArray();
    nConfig->endGroup();
}


void DataStackHandle::updateFormula(bool bState)
{
    if(!m_nConfig)
    {
        qDebug()<<"DataStackHandle m_nConfig is nullptr";
        return;
    }
    if(bState)
        LoadConfig(m_nConfig);
}




void DataStackHandle::startThread()
{
    qDebug()<<"DataStackHandle startThread";
    m_bRunThread = true;
    this->start();

}
void DataStackHandle::stopThread()
{
    qDebug()<<"DataStackHandle stopThread";
    m_bRunThread = false;
}

void DataStackHandle::zeroOffsetAdjustSlot(QVector<float> vctZeroOffset)
{
//    qDebug()<<vctZeroOffset;
    std::lock_guard<std::mutex> lock(m_zeroMutex);
    for(int ch = 0; ch < vctZeroOffset.size(); ch++)
    {
        m_nOffsetZeroArray[ch] += vctZeroOffset.at(ch);
    }

}

const float & DataStackHandle::readZeroOffset(uint16_t &channel)
{
    std::lock_guard<std::mutex> lock(m_zeroMutex);
    return m_nOffsetZeroArray[channel];
}


