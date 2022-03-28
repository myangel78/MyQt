#include "DataHandle.h"

#include <QDebug>
#include <QObject>

#include "Log.h"
#include "ConfigServer.h"
#include "CustomPlotWidgt.h"
#include "UsbFtdDevice.h"
#include "SavePannel.h"
#include "DegatingPane.h"
#include "DataCalculate.h"
#include "CapCaculate.h"


DataHandle::DataHandle()
{

}

DataHandle::~DataHandle()
{

}

/**
 * @brief DataHandle::ThreadRecvFunc simulated data generate
 */
void DataHandle::ThreadRecvFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());

    auto &vctFilePtr = m_pSavePane->GetVctSimulateFile();
    int requireSize = HANDLE_PACKAGE_SIZE;
    float readbuf[HANDLE_PACKAGE_SIZE];
    const int packetTimes = 1000.0/(ConfigServer::GetInstance()->GetSampleRate()/HANDLE_PACKAGE_SIZE);
    bool bIsReSetToHead = false;
    size_t actualReadSize  =0;

    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexRecv);
            m_cvRecv.wait(lck, [&] {return !m_bRunning || !m_bSuspend;});
        }
        if (!m_bRunning)
        {
            break;
        }
        //CalTimeElapse elpase;
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            auto vctTempPacketPtr = std::make_unique<ConfigServer::SDataHandle>(CHANNEL_NUM,HANDLE_PACKAGE_SIZE);
            for(int ch =0; ch < CHANNEL_NUM; ++ch)
            {
                actualReadSize = fread(&readbuf[0], sizeof(float), requireSize, vctFilePtr[ch]);
                if(actualReadSize == requireSize)
                {
                    vctTempPacketPtr->dataHandle[ch].resize(actualReadSize);
                    std::copy(&readbuf[0],&readbuf[0] + actualReadSize , vctTempPacketPtr->dataHandle[ch].begin());
                }
                else
                {
                    bIsReSetToHead = true;
                    break;
                }
            }
            if(bIsReSetToHead)
            {
                for(const auto &fileptr: vctFilePtr)
                {
                    fseek(fileptr,0,SEEK_SET);
                }
                bIsReSetToHead = false;
            }
            else
            {
                m_queDataPack.enqueue(std::move(vctTempPacketPtr));
                m_cvHandle.notify_one();

                auto endTime = std::chrono::high_resolution_clock::now();
                long long elapse = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
                if(elapse < packetTimes)
                {
                     std::this_thread::sleep_for(std::chrono::milliseconds(packetTimes - elapse));
//                     qDebug()<<elapse<<" ms";
                }
            }
        }
    }
    LOGI("ThreadExit={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());
}


void DataHandle::ThreadDecodeFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());

    m_vctRawData.resize(CHANNEL_NUM);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vctRawData[i].reserve(300);
    }

    std::vector<unsigned char> vctDataCache;
    std::vector<unsigned char> vctDataAnalysis;
    vctDataCache.clear();
    vctDataAnalysis.clear();

    static int temCnt = 0;

    while (m_bRunningDecode)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexDecode);
            m_cvDcode.wait(lck, [this] {return !m_bRunningDecode || m_queDecodePack.size_approx() > 0; });
        }

        m_bCrcCheck = ConfigServer::GetInstance()->GetIsCrcCaculate();
        m_bCrcCheckPrint = ConfigServer::GetInstance()->GetIsCrcCalPrint();
        m_bRawData = ConfigServer::GetInstance()->GetIsRawData();
        m_bNeedTrans = ConfigServer::GetInstance()->GetIsNeedToTrans();

        //CalTimeElapse elpase,elpase1;
        std::shared_ptr<SDataPack> dtpk;
        bool bret = m_queDecodePack.try_dequeue(dtpk);
        if (bret)
        {
            //elpase.StartCnt();

            auto& dpRaw = dtpk->dataPack;
            int dtRawsz = dpRaw.size();

#if 1
            vctDataAnalysis.resize(vctDataCache.size() + dtRawsz);
            std::memcpy(vctDataAnalysis.data(),vctDataCache.data(),vctDataCache.size());
            std::memcpy(&vctDataAnalysis[vctDataCache.size()],dpRaw.data(),dtRawsz);

            auto&& dp = vctDataAnalysis;
            int dtsz = vctDataAnalysis.size();
            int indx = 0;
#else
            auto&& dp = dpRaw;
            int dtsz = dtRawsz;
            int indx = 0;
#endif

            while (m_bRunningDecode && indx + COMPLETE_FRAME_64CH_SIZE <= dtsz)
            {
                bool bfound = false;
                for (; indx <= dtsz - 3; ++indx)
                {
                    if(IsRightFrameHeaderFmt(&dp[indx]))
                    {
                        bfound = true;
                        break;
                    }
                }
                if (bfound)
                {
                    if (indx + COMPLETE_FRAME_64CH_SIZE > dtsz)
                    {
                        //LOGW("Now index {} Pack remain Length is not enough {}!!!!",indx,COMPLETE_FRAME_SIZE);
                        break;
                    }

                    if (indx + COMPLETE_FRAME_64CH_SIZE + 3 <= dtsz)
                    {
                        auto *dpp = &dp[indx + COMPLETE_FRAME_64CH_SIZE];
                        if(!IsRightFrameHeaderFmt(dpp))
                        {
                            bfound = false;
                            //LOGW(">>>>>>>>> the next frame is not 2d 5a!!!");
//                            LOGCD(">>>>>>>>> the next frame is not 2d 5a!!! Print Binary : {:32}",spdlog::to_hex(&dp[indx],&dp[indx+ COMPLETE_FRAME_64CH_SIZE + 3]));
                            indx +=2;
                            continue;
                        }
                    }
                    uchar frame = 0;
                    bfound = TryToAcquireFrameNum(&dp[indx],frame);
                    if (bfound)
                    {
                        DecodeOneFrameRawData(&dp[indx],frame,m_bRawData);

                        if (m_vctRawData[0].size() >=HANDLE_PACKAGE_SIZE)
                        {
                            if(m_bNeedTrans)
                            {
                                LVDSCurrentConvert(m_vctRawData,m_bRawData);
                            }

                            //elpase1.StartCnt();
                            {
                                auto dtptr = std::make_shared<ConfigServer::SDataHandle>(std::move(m_vctRawData));
                                m_queDataPack.enqueue(std::move(dtptr));
                                m_cvHandle.notify_one();
                            }

                            for (auto &vctchRaw: m_vctRawData)
                            {
                                vctchRaw.reserve(HANDLE_PACKAGE_SIZE+50);
                            }
                            //elpase1.StopCnt();

//                            LOGCI(" {} elpase1 time us:{} queDataPack size:{} ",__FUNCTION__,  elpase1.GetTimeUsDifference(),m_queDataPack.size_approx());
                        }
                        indx += COMPLETE_FRAME_64CH_SIZE;
                    }
                    else
                    {
                         indx += 2;
                         LOGW("--------------Found the frame {},but the header is not right!!!",frame);
                    }
                }
                else
                {
                    LOGW("--------------No Found the header in whole frame!!!!!");
                }
            }
            if(indx < dtsz)
            {
                vctDataCache.clear();
                vctDataCache.resize(dtsz - indx);
                std::memcpy(vctDataCache.data(),&dp[indx],dtsz-indx);
            }

            //elpase.StopCnt();
//            if(temCnt++ >= 50)
//            {
//                temCnt = 0;
//                LOGCI("handle elpase time ms: {} queDecode size:{} dtpk size{}", elpase.GetTimeMsDifference(),m_queDecodePack.size_approx(),dtRawsz);
//            }
        }
    }
    LOGI("ThreadExit={}!!!={},m_queDecodePack size is {} ", __FUNCTION__, ConfigServer::GetCurrentThreadSelf(),m_queDecodePack.size_approx());

    for (auto &vctchRaw: m_vctRawData)
    {
        vctchRaw.clear();
    }

    std::shared_ptr<SDataPack> dtclear;
    while (m_queDecodePack.try_dequeue(dtclear));
//    LOGI("ThreadExit={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());
}


void DataHandle::ThreadHandleFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());
    while (m_bRunningHnadle)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexHandle);
            m_cvHandle.wait(lck, [this] {return !m_bRunningHnadle || m_queDataPack.size_approx() > 0; });
        }

        //CalTimeElapse elpase;
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataPack.try_dequeue(dtpk);
        if (bret)
        {
            //elpase.StartCnt();

            auto dtptr = std::make_shared<ConfigServer::SDataHandle>(dtpk->dataHandle);
            m_pCustomPlot->m_queDataHandle.enqueue(std::move(dtptr));
            m_pCustomPlot->m_cv.notify_one();

            if (ConfigServer::GetInstance()->GetSaveData())
            {
                auto dtsaveptr = std::make_shared<ConfigServer::SDataHandle>(dtpk->dataHandle);
                m_pSavePane->m_queDataHandle.enqueue(std::move(dtsaveptr));
                m_pSavePane->m_cv.notify_one();

                if (ConfigServer::GetInstance()->GetRealtimeSequence())
                {
                    //static bool direct = false;
                    //static int sicnt = 0;
                    //if (!direct && sicnt<40)
                    //{
                        auto dtrtseqptr = std::make_shared<ConfigServer::SDataHandle>(dtpk->dataHandle);
                        m_pSavePane->m_queDataHandle2.enqueue(std::move(dtrtseqptr));
                        m_pSavePane->m_cv2.notify_one();
                    //    ++sicnt;
                    //}
                    //else if (sicnt > 1)
                    //{
                    //    direct = true;
                    //    sicnt -= 2;
                    //}
                    //else
                    //{
                    //    direct = false;
                    //}
                }
            }

//            if (ConfigServer::GetInstance()->GetIsCalculate())
            {
                auto dtCalptr = std::make_shared<ConfigServer::SDataHandle>(dtpk->dataHandle);
                m_pDataCalculate->m_queDataCalc.enqueue(std::move(dtCalptr));
                m_pDataCalculate->m_cv.notify_one();
            }

            if (ConfigServer::GetInstance()->GetAutoDegating())
            {
                auto dtsaveptr = std::make_shared<ConfigServer::SDataHandle>(dtpk->dataHandle);
                m_pDegatingPane->m_queDataHandle.enqueue(std::move(dtsaveptr));
                m_pDegatingPane->m_cv.notify_one();
            }

            if (ConfigServer::GetInstance()->GetIsCapCollect())
            {
#if 0
                auto dtCapptr = std::make_shared<ConfigServer::SDataHandle>(dtpk->dataHandle);
                m_pCapCaculate->m_queCapDataCalc.enqueue(std::move(dtCapptr));
                m_pCapCaculate->m_cv.notify_one();
#else
                m_pCapCaculate->m_queCapDataCalc.enqueue(std::move(dtpk));
                m_pCapCaculate->m_cv.notify_one();
#endif
            }


            //elpase.StopCnt();
//            LOGCI("handle elpase time ms: {} queHandle size:{}", elpase.GetTimeMsDifference(),m_queDataPack.size_approx());
        }
    }
    LOGI("ThreadExit={}!!!={},m_queDataPack size is {} ", __FUNCTION__, ConfigServer::GetCurrentThreadSelf(),m_queDataPack.size_approx());
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataPack.try_dequeue(dtclear));
//    LOGI("ThreadExit={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());
}



bool DataHandle::StartThread(void)
{

    if(m_bSimulateData)
    {
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&DataHandle::ThreadRecvFunc, this);
    }
    }

    if (!m_bRunningDecode && m_ThreadDecodePtr == nullptr)
    {
        m_bRunningDecode = true;
        m_ThreadDecodePtr = std::make_shared<std::thread>(&DataHandle::ThreadDecodeFunc, this);

        if(m_ThreadDecodePtr != nullptr)
        {
            m_bRunningHnadle = true;
            m_ThreadHandlePtr = std::make_shared<std::thread>(&DataHandle::ThreadHandleFunc, this);
            return m_ThreadHandlePtr != nullptr;
        }
    }
    return false;
}

bool DataHandle::EndThread(void)
{

    if(m_bSimulateData)
    {
    m_bRunning = false;
    m_cvRecv.notify_one();
    }

    m_bRunningDecode = false;
    m_cvDcode.notify_one();

    m_bRunningHnadle = false;
    m_cvHandle.notify_one();

    return true;
}

bool DataHandle::StopThread(void)
{
    if(m_bSimulateData)
    {
    m_bRunning = false;
    m_cvRecv.notify_one();

    if (m_ThreadPtr.get() != nullptr)
    {
        if (m_ThreadPtr->joinable())
        {
            m_ThreadPtr->join();
        }
        m_ThreadPtr.reset();
    }
    }

    m_bRunningDecode = false;
    m_cvDcode.notify_one();

    m_bRunningHnadle = false;
    m_cvHandle.notify_one();

    if (m_ThreadDecodePtr.get() != nullptr)
    {
        if (m_ThreadDecodePtr->joinable())
        {
            m_ThreadDecodePtr->join();
        }
        m_ThreadDecodePtr.reset();
    }

    if (m_ThreadHandlePtr.get() != nullptr)
    {
        if (m_ThreadHandlePtr->joinable())
        {
            m_ThreadHandlePtr->join();
        }
        m_ThreadHandlePtr.reset();
    }
    return true;
}



bool DataHandle::VertifyCompleteFrames(const unsigned char * psrc)
{
    bool bret = true;
    for(int frame = 0 ; frame < 16; ++frame)
    {
        int indx = frame * 258;
        if(psrc[indx] != LVDS_FRAMEHEAD_FIRST_2D || psrc[indx +1] != LVDS_FRAMEHEAD_SECOND_5A  || (psrc[indx +2] &0x3E) != TAGSTART[frame])
        {
            return false;
        }
    }
    return bret;
}


bool DataHandle::TryToAcquireFrameNum(const unsigned char * psrc,uchar & reframe)
{
    if(psrc[0] != LVDS_FRAMEHEAD_FIRST_2D || psrc[1] != LVDS_FRAMEHEAD_SECOND_5A  || (psrc[2] &0x20) !=0x20)
    {
        return false;
    }
    reframe = (psrc[2] >> 1) & 0x0f;
    return (psrc[2] &0x3E) == TAGSTART[reframe];
}


#if 1
bool DataHandle::DecodeOneFrameRawData(const unsigned char* psrc,const char &frame,const bool bRawData)
{
    if(psrc[0] != LVDS_FRAMEHEAD_FIRST_2D || psrc[1] != LVDS_FRAMEHEAD_SECOND_5A  || (psrc[2] &0x3E) != TAGSTART[frame])
    {
        return false;
    }
    uint32_t channel = 0,channel2 = 0;
    uint16_t channelData = 0,channelData2 = 0;
    uint32_t byte = 2;
    uint8_t crcExtract = 0, crcCalRes = 0;

    for(uchar i = 0;i < CHANNELS_IN_ONE_FRAME; i +=2)
    {
        auto &&ptr = &psrc[byte];
        channel = (ptr[0] >>1 & 0x0F )+ i *16;
        channelData = (uint16_t)(ptr[1] &0x03) << 14 |  (uint16_t)(ptr[2] & 0x7f) << 7 | (uint16_t)(ptr[3] >> 2 & 0x1f) <<2 ;

        channel2 = (ptr[4] >>1 & 0x0F )+ (i+1) *16;
        channelData2 = (uint16_t)(ptr[5] &0x03) << 14 |  (uint16_t)(ptr[6] & 0x7f) << 7 | (uint16_t)(ptr[7] >> 2 & 0x1f) <<2 ;

        //是否计算CRC校验
        if(m_bCrcCheck)
        {
            if(UsbProtoMsg::CalculateCRCForTwoCh(ptr,crcExtract,crcCalRes)) //通过则压入数据
            {
                if(bRawData)
                {
                    m_vctRawData[channel].emplace_back(channelData);
                    m_vctRawData[channel2].emplace_back(channelData2);
                }
                else
                {
                    m_vctRawData[channel].emplace_back((int16_t)channelData);
                    m_vctRawData[channel2].emplace_back((int16_t)channelData2);
                }
            }
            else
            {
                if(m_bCrcCheckPrint)
                    LOGCE("channel {0} and channel {1} extract CRC: {2:x}, Calculate CRC result: {3:x}",channel,channel2,crcExtract,crcCalRes);
            }
        }
        else  //不校验则全部压入数据
        {
            if(bRawData)
            {
                m_vctRawData[channel].emplace_back(channelData);
                m_vctRawData[channel2].emplace_back(channelData2);
            }
            else
            {
                m_vctRawData[channel].emplace_back((int16_t)channelData);
                m_vctRawData[channel2].emplace_back((int16_t)channelData2);
            }
        }
        byte +=8;
    }

    return true;
}
#endif



bool DataHandle::IsRightFrameHeaderFmt(const unsigned char *ptrSrc)
{
    return (ptrSrc[0] == LVDS_FRAMEHEAD_FIRST_2D && ptrSrc[1] == LVDS_FRAMEHEAD_SECOND_5A && (ptrSrc[2] &0x20) == 0x20);
}

void DataHandle::LVDSCurrentConvert(std::vector<std::vector<float>> &vctRawData,const bool &bIsRaw)
{
    const float rawUnit = ConfigServer::GetInstance()->GetLvdsCurConvt().first;
    const float adcUnit = ConfigServer::GetInstance()->GetLvdsCurConvt().second;

    if(bIsRaw)
    {
        for(auto &&ite: vctRawData)
        {
            std::transform(ite.begin(), ite.end(), ite.begin(), [&rawUnit](float &val)
            { return (val - LVDS_CVT_RAW_MIDDLE) * rawUnit;});
        }
    }
    else
    {
        for(auto &&ite: vctRawData)
        {
            std::transform(ite.begin(), ite.end(), ite.begin(), [&adcUnit](float &val)
            { return (val - LVDS_CVT_ADC_MIDDLE) * adcUnit;});
        }
    }
}
