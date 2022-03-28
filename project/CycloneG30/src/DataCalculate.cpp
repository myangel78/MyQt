#include "DataCalculate.h"
#include "Log.h"

#include <QTimer>
#include <future>

//#include "SensorPanel.h"
//#include "PoreStateMapWdgt.h"

DataCalculate::DataCalculate(QObject *parent):QObject(parent)
{
    m_vetAverageData.resize(CHANNEL_NUM);
    m_vetAverResult.resize(CHANNEL_NUM);
    m_vetStdData.resize(CHANNEL_NUM);
    m_vetStdResult.resize(CHANNEL_NUM);
    m_vetAverUnitResult.resize(CHANNEL_NUM);
    m_vetAverageUnitData.resize(CHANNEL_NUM);


}

DataCalculate::~DataCalculate()
{

}



bool DataCalculate::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&DataCalculate::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

#if 0
void DataCalculate::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    const int samplerate = ConfigServer::GetInstance()->GetSampleRate();
    const int averSecCnt = samplerate/HANDLE_PACKAGE_SIZE;      // 1 second
    const int stdCalMaxPoit = samplerate * 0.6;  //0.6 second data points

    //CalTimeElapse elpase;
    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataCalc.size_approx() > 0; });
        }
        //elpase.StartCnt();
        //int quepacksz = m_queDataHandle.size_approx();
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataCalc.try_dequeue(dtpk);

        if (bret)
        {
            //int datasz = dtpk->dataHandle.size();
            {
//                std::lock_guard<std::mutex> lock(m_mutexavgstd);
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    auto& curval = dtpk->dataHandle[i];
                    int cursz = curval.size();
                    if (cursz <= 0)
                    {
                        continue;
                    }

                    {
                        auto& averdata = m_vetAverageData[i];
                        double sum = std::accumulate(curval.begin(), curval.end(), 0.0);
                        auto avg = sum / cursz;
                        if(averdata.size() > averSecCnt)
                        {
                           averdata.erase(averdata.begin()+1);
                        }
                        averdata.emplace_back(avg);

                        auto& stddata = m_vetStdData[i];
                        stddata.insert(stddata.end(), curval.begin(), curval.end());
                        if (stddata.size() > stdCalMaxPoit )
                        {
                            stddata.erase(stddata.begin(), stddata.begin() + curval.size());
                        }
                    }
                }

                if(m_bIsTimeToCalAvg)
                {
                    m_bIsTimeToCalAvg = false;
                    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
                    {
                        const auto &vctavg = m_vetAverageData[ch];
                        int size = (int)vctavg.size();
                        if(size > 0)
                        {
                            m_vetAverResult[ch] = std::accumulate(vctavg.begin(), vctavg.end(), 0.0)/size;
                        }
                        m_vetAverageData[ch].clear();
                    }
                }

                if(m_bIsTimeToCalStd)
                {
                    m_bIsTimeToCalStd = false;
                    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
                    {
                        auto& stdCurrent = m_vetStdData[ch];
                        int stdsz = (int)stdCurrent.size();
                        if (stdsz > 0)
                        {
                            double mean = m_vetAverResult[ch];
                            double sum = 0.0;
                            for (const auto& ite : stdCurrent)
                            {
                                sum += std::pow(ite - mean, 2);
                            }
                            m_vetStdResult[ch] = sqrt(sum / stdsz);
                        }
                        m_vetStdData[ch].clear();
                    }
                }
            }

        }
        //elpase.StopCnt();
//        LOGCI(" {} elpase time us:{} queandle size:{} = {} {} {}",__FUNCTION__,  elpase.GetTimeUsDifference(),m_queDataCalc.size_approx(),ConfigServer::GetCurrentThreadSelf(),m_vetAverageData[0].size(),m_vetStdData[1].size());

    }

    for (int ch =0; ch < CHANNEL_NUM; ++ch)
    {
       m_vetAverageData[ch].clear();
       m_vetStdData[ch].clear();
    }

    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataCalc.try_dequeue(dtclear));

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}
#endif
void DataCalculate::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
    const int samplerate = ConfigServer::GetInstance()->GetSampleRate();
    const int averSecCnt = samplerate/HANDLE_PACKAGE_SIZE;      // 1 second
    const int averCalTimes =4;
    const int averCalUnit = averSecCnt/averCalTimes;      // 1 second
    const int stdCalMaxPoit = samplerate * 0.6;  //0.6 second data points
    bool bEnoughToCalAvg = false;
    bool bEnoughToCalStd = false;
    int bCurCalAvgTimes = 0;
    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataCalc.size_approx() > 0; });
        }
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataCalc.try_dequeue(dtpk);
        if (bret)
        {
            {
//                std::lock_guard<std::mutex> lock(m_mutexavgstd);
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    auto& curval = dtpk->dataHandle[i];
                    int cursz = curval.size();
                    if (cursz <= 0)
                    {
                        continue;
                    }

                    {
                        auto& averdata = m_vetAverageData[i];
                        double sum = std::accumulate(curval.begin(), curval.end(), 0.0);
                        auto avg = sum / cursz;
                        averdata.emplace_back(avg);
                        if(i == 0 && averdata.size() >= averCalUnit)
                        {
                            bEnoughToCalAvg = true;
                        }

                        auto& stddata = m_vetStdData[i];
                        stddata.insert(stddata.end(), curval.begin(), curval.end());
                        if(i == 0 && stddata.size() >= stdCalMaxPoit)
                        {
                            bEnoughToCalStd = true;
                        }
                    }
                }


                if(bEnoughToCalAvg)
                {
                    bEnoughToCalAvg = false;
                    if(bCurCalAvgTimes++ < averCalTimes)
                    {
                        for(int ch = 0; ch < CHANNEL_NUM; ++ch)
                        {
                            const auto &vctavg = m_vetAverageData[ch];
                            int size = (int)vctavg.size();
                            if(size > 0)
                            {
                                //计算1/4秒的平均值
                                m_vetAverUnitResult[ch] = std::accumulate(vctavg.begin(), vctavg.end(), 0.0)/size;
                                m_vetAverageUnitData[ch].emplace_back(m_vetAverUnitResult[ch]);
                                m_vetAverageData[ch].clear();
                            }
                        }
                    }
                    else
                    {
                        for(int ch = 0; ch < CHANNEL_NUM; ++ch)
                        {
                            const auto &vctUnitData = m_vetAverageUnitData[ch];
                            if(vctUnitData.size() > 0)
                            {
                                //计算1秒的平均值
                                m_vetAverResult[ch] = std::accumulate(vctUnitData.cbegin(), vctUnitData.cend(), 0.0)/vctUnitData.size();
                                m_vetAverageUnitData[ch].clear();
                            }
                        }
                        bCurCalAvgTimes = 0;
                    }
                }

                if(bEnoughToCalStd)
                {
                    bEnoughToCalStd = false;
                    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
                    {
                        auto& stdCurrent = m_vetStdData[ch];
                        int stdsz = (int)stdCurrent.size();
                        if (stdsz > 0)
                        {
                            double mean = m_vetAverResult[ch];
                            double sum = 0.0;
                            for (const auto& ite : stdCurrent)
                            {
                                sum += std::pow(ite - mean, 2);
                            }
                            m_vetStdResult[ch] = sqrt(sum / stdsz);
                        }
                        m_vetStdData[ch].clear();
                    }
                }
            }
        }
    }

    for (int ch =0; ch < CHANNEL_NUM; ++ch)
    {
       m_vetAverageData[ch].clear();
       m_vetStdData[ch].clear();
       m_vetAverageUnitData[ch].clear();
    }

    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataCalc.try_dequeue(dtclear));

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool DataCalculate::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return false;
}

bool DataCalculate::StopThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    if (m_ThreadPtr != nullptr)
    {
        if (m_ThreadPtr->joinable())
        {
            m_ThreadPtr->join();
        }
        m_ThreadPtr.reset();
    }
    return false;
}


void DataCalculate::GetCurForPoreInsertSlot(void)
{
    emit SendAverageRultForPoreInsertSig(m_vetAverUnitResult);
}

void DataCalculate::GetCurForMembrokProtectSlot(void)
{
    emit SendAverageRultForMembroProtectSig(m_vetAverResult);
}

void DataCalculate::GetCurStdForShutOffSlot(void)
{
    emit SendStdRultForShutOffChannSig(m_vetStdResult);
}



void DataCalculate::GetAllChAvgSlot(std::vector<float> &vetAvg)
{
    std::copy(m_vetAverResult.cbegin(),m_vetAverResult.cend(), vetAvg.begin());
}
void DataCalculate::GetAllChStdSlot(std::vector<float> &vetStd)
{
     std::copy(m_vetStdResult.cbegin(),m_vetStdResult.cend(), vetStd.begin());
}
