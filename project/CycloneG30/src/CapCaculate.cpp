#include "CapCaculate.h"
#include "Log.h"

CapCaculate::CapCaculate(QObject *parent):QObject(parent)
{
    m_vetCapValue.resize(CHANNEL_NUM);
    m_vetMeanCap.resize(CHANNEL_NUM);
}
CapCaculate::~CapCaculate()
{

}
bool CapCaculate::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&CapCaculate::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void CapCaculate::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    double triangeFreqence = ConfigServer::GetInstance()->GetTriangFrequence();
    float dutyTime = 20.0 / triangeFreqence;
    float distTime = 0.0;
    m_startCnt = std::chrono::steady_clock::now();
    //CalTimeElapse elpase;
    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queCapDataCalc.size_approx() > 0; });
        }

        //elpase.StartCnt();
        double triangeAmplitude = ConfigServer::GetInstance()->GetTriangAmplitude();
        if (ISDOUBLEZERO(triangeAmplitude))
        {
            triangeAmplitude = 1.0;
        }

        //int quepacksz = m_queCapDataCalc.size_approx();
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queCapDataCalc.try_dequeue(dtpk);
        if (bret)
        {
            m_stopCnt = std::chrono::steady_clock::now();
            distTime = std::chrono::duration<double>(m_stopCnt - m_startCnt).count();
            if (distTime <= dutyTime)
            {
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    if(dtpk->dataHandle[i].size() > 0)
                    {
                        auto &vctMeanCap = m_vetMeanCap[i];
                        std::for_each(dtpk->dataHandle[i].cbegin(), dtpk->dataHandle[i].cend(), [&vctMeanCap](const float& val) { if(val > 0) vctMeanCap.emplace_back(val);});

                    }
                }
            }
            else
            {
//                std::cout<<"vctMeanCap[0].size()"<<m_vetMeanCap[0].size()<<" "<<std::endl;
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    int avgsz = m_vetMeanCap[i].size();
                    if (avgsz > 0)
                    {
                        double sum = std::accumulate(m_vetMeanCap[i].cbegin(), m_vetMeanCap[i].cend(), 0.0);
                        double mean = sum / avgsz;

                        float cap = mean / 4.0 / triangeFreqence / triangeAmplitude;
                        m_vetCapValue[i] = cap;

                    }
                    else
                    {
                        m_vetCapValue[i] = 0.0;
                    }
                        m_vetMeanCap[i].clear();
                }
                emit SigCapResultFromCalculate(m_vetCapValue);

                m_startCnt = std::chrono::steady_clock::now();
            }
            //m_vetMeanCap
        }
        //elpase.StopCnt();
//        LOGCI(" {} elpase time us:{} queandle size:{} ",__FUNCTION__,  elpase.GetTimeUsDifference(),m_queCapDataCalc.size_approx());
    }
    for(int i = 0; i< CHANNEL_NUM; ++i)
    {
        m_vetCapValue[i] = 0.0;
        m_vetMeanCap[i].clear();
    }
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queCapDataCalc.try_dequeue(dtclear));

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool CapCaculate::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return false;
}

bool CapCaculate::StopThread(void)
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
