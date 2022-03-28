#include "DegatingPane.h"
#include <qlayout.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <clustering.h>
#include <Log.h>
#include "HttpClient.h"
#include "PoreStateMapTabModel.h"
#include "SensorPanel.h"


DegatingPane::DegatingPane(QWidget *parent) : QDialog(parent)
{
    InitCtrl();
    m_vetDegatingData.resize(CHANNEL_NUM);
    m_vetAvgCur.resize(CHANNEL_NUM);
    m_vctAllchFuncState.resize(CHANNEL_NUM);
    std::fill_n(m_vctAllchFuncState.begin(),CHANNEL_NUM,SENSORGRP_FUNC_STATE_NORMAL);

}

bool DegatingPane::InitCtrl(void)
{
    m_pgrpDegatingBase = new QGroupBox("Degating", this);
    m_pgrpDegatingBase->setContentsMargins(3, 1, 3, 0);
    m_plabDegatingVolt = new QLabel("Degating Volt:", m_pgrpDegatingBase);
    m_plabDegatingVolt->setVisible(false);
    m_pdspDegatingVolt = new QDoubleSpinBox(m_pgrpDegatingBase);
    m_pdspDegatingVolt->setDecimals(2);
    m_pdspDegatingVolt->setSuffix(" V");
    m_pdspDegatingVolt->setMinimum(-99.99);
    m_pdspDegatingVolt->setMaximum(99.99);
    m_pdspDegatingVolt->setSingleStep(0.01);
    m_pdspDegatingVolt->setValue(-0.1);
    m_pdspDegatingVolt->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置Degating电压</p></body></html>"));
    m_pdspDegatingVolt->setVisible(false);

    m_plabCyclePeriod = new QLabel("Cycle Period:", m_pgrpDegatingBase);
    m_pispCyclePeriod = new QSpinBox(m_pgrpDegatingBase);
    m_pispCyclePeriod->setSuffix(" s");
    m_pispCyclePeriod->setMinimum(1);
    m_pispCyclePeriod->setMaximum(99);
    m_pispCyclePeriod->setSingleStep(1);
    m_pispCyclePeriod->setValue(4);
    m_pispCyclePeriod->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置多久（几秒）检测一次gating状态</p></body></html>"));

    m_plabDurationTime = new QLabel("Duration Time:", m_pgrpDegatingBase);
    m_pispDurationTime = new QSpinBox(m_pgrpDegatingBase);
    m_pispDurationTime->setSuffix(" ms");
    m_pispDurationTime->setMinimum(100);
    m_pispDurationTime->setMaximum(10000);
    m_pispDurationTime->setSingleStep(100);
    m_pispDurationTime->setValue(200);
    m_pispDurationTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置施加Degating电压持续时间</p></body></html>"));

    m_plabSteppingTime = new QLabel("Stepping Time:", m_pgrpDegatingBase);
    m_plabSteppingTime->setVisible(false);
    m_pispSteppingTime = new QSpinBox(m_pgrpDegatingBase);
    m_pispSteppingTime->setSuffix(" ms");
    m_pispSteppingTime->setMinimum(0);
    m_pispSteppingTime->setMaximum(10000);
    m_pispSteppingTime->setSingleStep(10);
    m_pispSteppingTime->setValue(0);
    m_pispSteppingTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置gating检测增量时间（下一次比上一次增加的恢复检测时间）</p></body></html>"));
    m_pispSteppingTime->setVisible(false);

    m_plabAllowTimes = new QLabel("Allow Times:", m_pgrpDegatingBase);
    m_pispAllowTimes = new QSpinBox(m_pgrpDegatingBase);
    m_pispAllowTimes->setMinimum(0);
    m_pispAllowTimes->setMaximum(100);
    m_pispAllowTimes->setSingleStep(1);
    m_pispAllowTimes->setValue(10);
    m_pispAllowTimes->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间内的Degating次数，规定时间内达到此次数则关断该通道。0为表示此功能不开启</p></body></html>"));

    m_plabLimitCycle = new QLabel("Limit Cycle:", m_pgrpDegatingBase);
    m_plabLimitCycle->setVisible(false);
    m_pispLimitCycle = new QSpinBox(m_pgrpDegatingBase);
    m_pispLimitCycle->setMinimum(5);
    m_pispLimitCycle->setMaximum(10000);
    m_pispLimitCycle->setSingleStep(1);
    m_pispLimitCycle->setValue(14);
    m_pispLimitCycle->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间周期，在此周期内判定Degating次数</p></body></html>"));
    m_pispLimitCycle->setVisible(false);

    m_pchkAutoseqGating = new QCheckBox("Autoseq", m_pgrpDegatingBase);
    //m_pchkAutoseqGating->setChecked(true);
    m_pchkAutoseqGating->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否启用AutoseqGating算法</p></body></html>"));
    //m_pchkAutoseqGating->setVisible(false);

    m_pispGatingTime = new QSpinBox(m_pgrpDegatingBase);
    m_pispGatingTime->setRange(1, 10);
    m_pispGatingTime->setValue(2);
    m_pispGatingTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>连续Gating次数</p></body></html>"));
    //m_pispGatingTime->setVisible(false);

    m_pispGatingChannel = new QSpinBox(m_pgrpDegatingBase);
    m_pispGatingChannel->setRange(0, 1024);
    m_pispGatingChannel->setValue(0);
    m_pispGatingChannel->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>打印Gating日志的通道，0为所有通道</p></body></html>"));
    //m_pispGatingChannel->setVisible(false);

    m_pbtnDegatingEnable = new QPushButton("Enable", m_pgrpDegatingBase);
    m_pbtnDegatingEnable->setCheckable(true);
    m_pbtnDegatingEnable->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>启用/禁用gating监测功能</p></body></html>"));

    m_pbtnDegatingApply = new QPushButton("Apply", m_pgrpDegatingBase);
    m_pbtnDegatingApply->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置参数应用到gating功能内</p></body></html>"));


    int row = 0;
    int col = 0;
    QGridLayout* gdlayout1 = new QGridLayout();
    gdlayout1->setContentsMargins(1, 8, 1, 1);
    gdlayout1->addWidget(m_plabDegatingVolt, row, col++);
    gdlayout1->addWidget(m_pdspDegatingVolt, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabCyclePeriod, row, col++);
    gdlayout1->addWidget(m_pispCyclePeriod, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabDurationTime, row, col++);
    gdlayout1->addWidget(m_pispDurationTime, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabSteppingTime, row, col++);
    gdlayout1->addWidget(m_pispSteppingTime, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabAllowTimes, row, col++);
    gdlayout1->addWidget(m_pispAllowTimes, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabLimitCycle, row, col++);
    gdlayout1->addWidget(m_pispLimitCycle, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_pchkAutoseqGating, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_pispGatingTime, row, col++);
    gdlayout1->addWidget(m_pispGatingChannel, row, col++);

    QHBoxLayout* horlayout2 = new QHBoxLayout();
    //horlayout2->setContentsMargins(1, 1, 1, 1);
    //horlayout2->addWidget(m_pchkAutoseqGating);
    horlayout2->addWidget(m_pbtnDegatingEnable);
    //horlayout2->addWidget(m_pchkDegatingAllCheck);
    horlayout2->addWidget(m_pbtnDegatingApply);

    QVBoxLayout* verlayout4 = new QVBoxLayout();
    //verlayout4->setContentsMargins(1, 6, 1, 1);
    verlayout4->addLayout(gdlayout1);
    verlayout4->addLayout(horlayout2);

    m_pgrpDegatingBase->setLayout(verlayout4);


    //Advanced
    m_pgrpDegatingAdvanced = new QGroupBox("Advanced", this);
    m_pgrpDegatingAdvanced->setContentsMargins(3, 1, 3, 0);
    m_plabDurationThres = new QLabel("Durate Thres:", m_pgrpDegatingAdvanced);
    m_pdspDurationThres = new QDoubleSpinBox(m_pgrpDegatingAdvanced);
    m_pdspDurationThres->setDecimals(2);
    m_pdspDurationThres->setMinimum(0.2);
    m_pdspDurationThres->setMaximum(1);
    m_pdspDurationThres->setValue(0.1);
    m_pdspDurationThres->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>最大值不能超过1，如果有某段信号0.5s一致维持在同一个水平不变，那就认为是gating状态</p></body></html>"));

    m_plabGatingSTD = new QLabel("Seq STD Min:", m_pgrpDegatingAdvanced);
    m_pispGatingSTD = new QSpinBox(m_pgrpDegatingAdvanced);
    m_pispGatingSTD->setMinimum(0);
    m_pispGatingSTD->setMaximum(100);
    m_pispGatingSTD->setValue(5);
    m_pispGatingSTD->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>小于该STD min值，则认为是gating状态</p></body></html>"));

    m_plabGatingSTDMax = new QLabel("Seq STD Max:", m_pgrpDegatingAdvanced);
    m_pispGatingSTDMax = new QSpinBox(m_pgrpDegatingAdvanced);
    m_pispGatingSTDMax->setMinimum(0);
    m_pispGatingSTDMax->setMaximum(50000);
    m_pispGatingSTDMax->setValue(105);
    m_pispGatingSTDMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>大于该STD max值，则认为是gating状态</p></body></html>"));

    m_plabSignalMin = new QLabel("Signal Min:", m_pgrpDegatingAdvanced);
    m_pispSignalMin = new QSpinBox(m_pgrpDegatingAdvanced);
    m_pispSignalMin->setMinimum(-200);
    m_pispSignalMin->setMaximum(500);
    m_pispSignalMin->setValue(20);
    m_pispSignalMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，小于该值为非gating,大于此值才进一步作gating分析</p></body></html>"));

    m_plabSignalMax = new QLabel("Signal Max:", m_pgrpDegatingAdvanced);
    m_pispSignalMax = new QSpinBox(m_pgrpDegatingAdvanced);
    m_pispSignalMax->setMinimum(0);
    m_pispSignalMax->setMaximum(500);
    m_pispSignalMax->setValue(150);
    m_pispSignalMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，大于该值为非gating,小于此值才进一步作gating分析</p></body></html>"));

    m_plabGatingMin = new QLabel("Gating Min:", m_pgrpDegatingAdvanced);
    m_pispGatingMin = new QSpinBox(m_pgrpDegatingAdvanced);
    m_pispGatingMin->setMinimum(-20);
    m_pispGatingMin->setMaximum(500);
    m_pispGatingMin->setValue(40);
    m_pispGatingMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，大于此值作gating分析</p></body></html>"));

    m_plabGatingMax = new QLabel("Gating Max:", m_pgrpDegatingAdvanced);
    m_pispGatingMax = new QSpinBox(m_pgrpDegatingAdvanced);
    m_pispGatingMax->setMinimum(0);
    m_pispGatingMax->setMaximum(500);
    m_pispGatingMax->setValue(130);
    m_pispGatingMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，小于此值作gating分析</p></body></html>"));

    m_pbtnDefaultSet = new QPushButton("Default", m_pgrpDegatingAdvanced);
    m_pbtnDefaultSet->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>将高级参数重置为默认值</p></body></html>"));

    m_pbtnAdvancedSet = new QPushButton("Advanced Apply", m_pgrpDegatingAdvanced);
    m_pbtnAdvancedSet->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>将高级参数应用到gating检测功能内</p></body></html>"));


    int cnt = 0;
    QGridLayout* gdlayout2 = new QGridLayout();
    gdlayout2->setContentsMargins(1, 8, 1, 1);
    gdlayout2->addWidget(m_plabDurationThres, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTD, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTDMax, cnt++, 0);
    gdlayout2->addWidget(m_plabSignalMin, cnt++, 0);
    gdlayout2->addWidget(m_plabSignalMax, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingMin, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingMax, cnt++, 0);
    cnt = 0;
    gdlayout2->addWidget(m_pdspDurationThres, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTD, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTDMax, cnt++, 1);
    gdlayout2->addWidget(m_pispSignalMin, cnt++, 1);
    gdlayout2->addWidget(m_pispSignalMax, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingMin, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingMax, cnt++, 1);

    QHBoxLayout* horlayout5 = new QHBoxLayout();
    horlayout5->addSpacing(20);
    horlayout5->addWidget(m_pbtnDefaultSet);
    horlayout5->addStretch(1);
    horlayout5->addWidget(m_pbtnAdvancedSet);
    horlayout5->addSpacing(20);

    QVBoxLayout* verlayout6 = new QVBoxLayout();
    verlayout6->addLayout(gdlayout2);
    verlayout6->addLayout(horlayout5);

    m_pgrpDegatingAdvanced->setLayout(verlayout6);


    //all
    QHBoxLayout* horlayout9 = new QHBoxLayout();
    horlayout9->setContentsMargins(3, 1, 3, 1);
    horlayout9->addWidget(m_pgrpDegatingBase);
    horlayout9->addWidget(m_pgrpDegatingAdvanced);
    horlayout9->addStretch(1);

    setLayout(horlayout9);

    //..
    connect(m_pbtnDegatingEnable, &QPushButton::clicked, this, &DegatingPane::onClickEnableDegating);
    connect(m_pbtnDegatingApply, &QPushButton::clicked, this, &DegatingPane::onClickDegatingApply);
    connect(m_pbtnDefaultSet, &QPushButton::clicked, this, &DegatingPane::onClickDefaultApply);
    connect(m_pbtnAdvancedSet, &QPushButton::clicked, this, &DegatingPane::onClickAdvanceApply);

    return true;
}

bool DegatingPane::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&DegatingPane::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void DegatingPane::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataHandle.size_approx() > 0; });
        }

        //int quepacksz = m_queDataHandle.size_approx();
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataHandle.try_dequeue(dtpk);
        if (bret)
        {
            bool allready = true;
            for (int k = 0; k < m_ciThread; ++k)
            {
                if (!m_bDegatingSample[k])
                {
                    allready = false;
                    break;
                }
            }
            if (!allready)
            {
                continue;
            }
            //int datasz = dtpk->dataHandle.size();
            if (ConfigServer::GetInstance()->GetAutoDegating())
            {
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    auto& curval = dtpk->dataHandle[i];
                    std::lock_guard<std::mutex> lock(m_mutexdegating);
                    auto& data1 = m_vetDegatingData[i];
                    data1.insert(data1.end(), curval.begin(), curval.end());
                }

                if (m_vetDegatingData[0].size() >= m_iDegatingDataPoint)
                {
                    for (int k = 0; k < m_ciThread; ++k)
                    {
                        m_bDegatingSample[k] = false;
                    }
                    //std::lock_guard<std::mutex> lock(m_mutexdegating);
                    //if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
                    {
                        LOGDEGDATAI("DegData={}", m_iDegatingDataPoint);
                        m_vetAvgCur = m_vetDegatingData;
                        m_cvGating.notify_all();
                    }
                    for (int i = 0; i < CHANNEL_NUM; ++i)
                    {
                        m_vetDegatingData[i].clear();
                    }
                }
            }
        }
    }
    LOGI("ThreadExit={}!Left={}!={}", __FUNCTION__, m_queDataHandle.size_approx(), ConfigServer::GetCurrentThreadSelf());

    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));
}

bool DegatingPane::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return false;
}

bool DegatingPane::StopThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    if (m_ThreadPtr.get() != nullptr)
    {
        if (m_ThreadPtr->joinable())
        {
            m_ThreadPtr->join();
        }
        m_ThreadPtr.reset();
    }
    return false;
}

bool DegatingPane::StartThread2(void)
{
    if (!m_bRunning2)
    {
        m_bRunning2 = true;
        const int block = CHANNEL_NUM / m_ciThread;
        if (m_pchkAutoseqGating->isChecked())
        {
            m_iDegatingDataTime = 2;
            m_iDegatingDataPoint = 10000;
            for (int i = 0; i < m_ciThread; ++i)
            {
                m_pthrGating[i] = std::make_shared<std::thread>(&DegatingPane::ThreadFunc2Deg2, this, i, block);
            }
        }
        else
        {
            m_iDegatingDataTime = 1;
            m_iDegatingDataPoint = 5000;
            for (int i = 0; i < m_ciThread; ++i)
            {
                m_pthrGating[i] = std::make_shared<std::thread>(&DegatingPane::ThreadFunc2Deg, this, i, block);
            }
        }
        return m_pthrGating[0] != nullptr;
    }
    return false;
}

#if 0
void DegatingPane::ThreadFunc2Deg(int index, int block)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    const int startindex = index * block;
    //std::vector<int> vetPoreStateData(block, 0);
    //std::vector<int> vetPoreStateData2(block, 0);
    m_arPoreStateData[index].resize(block);
    m_arPoreStateData2[index].resize(block);

    while (m_bRunning2)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexGating);
            m_cvGating.wait(lck);
        }

        if (!m_bRunning2)
        {
            break;
        }

        const auto& avgCurrent = m_vetAvgCur;
        if (avgCurrent.size() <= 0)
        {
            continue;
        }

        const auto& porestate = m_pModelSharedDataPtr->GetSensorStateData();
        int sensorGrp = ConfigServer::GetInstance()->GetCurSensorGroup();
        SENSOR_STATE_ENUM gatingstate = SENSOR_STATE_A_NORMAL;
        const auto& vctlockState = m_pModelSharedDataPtr->GetLockStateDataBySensorGrp(SENSOR_STATE_GROUP_ENUM(sensorGrp));
        if (sensorGrp == SENSOR_STATE_A_GROUP)
        {
            gatingstate = SENSOR_STATE_A_UNBLOCK_VOLT;
        }
        else if (sensorGrp == SENSOR_STATE_B_GROUP)
        {
            gatingstate = SENSOR_STATE_B_UNBLOCK_VOLT;
        }
        else if (sensorGrp == SENSOR_STATE_C_GROUP)
        {
            gatingstate = SENSOR_STATE_C_UNBLOCK_VOLT;
        }
        else if (sensorGrp == SENSOR_STATE_D_GROUP)
        {
            gatingstate = SENSOR_STATE_D_UNBLOCK_VOLT;
        }
        //auto& lockstate = m_pPoreStateMapTab->GetLockStateData(sensorGrp);
        for (int i = startindex, n = block + startindex; i < n; ++i)
        {
            int poreindex = i - startindex;
            //vetPoreStateData[poreindex] = porestate[i];
            //vetPoreStateData2[poreindex] = porestate[i];
            m_arPoreStateData[index][poreindex] = porestate[i];
            m_arPoreStateData2[index][poreindex] = porestate[i];
            m_arPoreState[i] = porestate[i];
            m_arPoreState2[i] = porestate[i];

            if (vctlockState[i] != PROHIBITED_OPERATION_STATE && (porestate[i] != SENSOR_STATE_OFF /*&& porestate[i] != SENSOR_STATE_A_UNBLOCK_VOLT
                && porestate[i] != SENSOR_STATE_B_UNBLOCK_VOLT && porestate[i] != SENSOR_STATE_C_UNBLOCK_VOLT && porestate[i] != SENSOR_STATE_D_UNBLOCK_VOLT*/)
                && avgCurrent.size() > 0 && avgCurrent[i].size() > 0)
            {
                auto buffer = avgCurrent[i];
                std::vector<int> label(buffer.size());
                int eps_p(50);
                int eps_c(10);
                int s_min_pts(10);
                int eps_d(10);
                int d_min_pts(1);
                bool pre_flag(true);

                std::vector<std::vector<double>> cluster_stats;
                LinearDBSCAN(cluster_stats, buffer, label, eps_p, eps_c, s_min_pts, eps_d, d_min_pts, pre_flag);

                bool isgating = isGating(label, cluster_stats, m_dSampleRate, m_dDurationThres, m_iSignalMin, m_iSignalMax, m_iGatingMin, m_iGatingMax, 10, m_iGatingSTD, m_iGatingSTDMax, 170);
                m_arIsGating[i] = isgating;

                if (isgating)
                {
                    ++m_ariCurTimes[i];
                    if (m_iAllowTime > 0 && m_ariCurTimes[i] >= m_iAllowTime)
                    {
                        //vetPoreStateData[poreindex] = SENSOR_STATE_OFF;
                        m_arPoreStateData[index][poreindex] = SENSOR_STATE_OFF;
                        m_arPoreState[i] = SENSOR_STATE_OFF;
                    }
                    else
                    {
                        //vetPoreStateData[poreindex] = gatingstate;
                        m_arPoreStateData[index][poreindex] = gatingstate;
                        m_arPoreState[i] = gatingstate;
                    }
                }
                else
                {
                    m_ariCurTimes[i] = 0;
                }
                //break;
            }
        }
        m_bDegatingCalc[index] = true;
        m_cvDegating.notify_all();

#if 0
        m_pSensorPane->SpeChanSensorSpeciSetSlot(startindex, startindex + block, vetPoreStateData);

        if (m_bRunning2)
        {
            std::thread thrd([=]()
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(m_iDurationTime));
                    m_pSensorPane->SpeChanSensorSpeciSetSlot(startindex, startindex + block, vetPoreStateData2);

                    std::this_thread::sleep_for(std::chrono::milliseconds((m_iCyclePeriod - 1) * 1000));
                    m_bDegatingSample[index] = true;
                });
            thrd.detach();
        }
#endif
    }

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

void DegatingPane::ThreadFunc2Deg2(int index, int block)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    auto strUrl = std::string("localhost:9211/gating_clf");//http://192.168.0.221:8000/gating_clf//ConfigServer::GetInstance()->GetVerupdateAddr();
    //if (ConfigServer::GetInstance()->GetWorknet() == 1)
    //{
    //    strUrl = std::string("http://172.16.18.12:9112/gating_clf");
    //}
    const int startindex = index * block;
    if (startindex > 511)
    {
        strUrl = std::string("localhost:9212/gating_clf");
    }
    m_arPoreStateData[index].resize(block);
    m_arPoreStateData2[index].resize(block);

    while (m_bRunning2)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexGating);
            m_cvGating.wait(lck);
        }

        if (!m_bRunning2)
        {
            break;
        }

        const auto& avgCurrent = m_vetAvgCur;
        if (avgCurrent.size() <= 0)
        {
            continue;
        }

        const auto& porestate = m_pModelSharedDataPtr->GetSensorStateData();
        int sensorGrp = ConfigServer::GetInstance()->GetCurSensorGroup();
        const auto& vctlockState = m_pModelSharedDataPtr->GetLockStateDataBySensorGrp(SENSOR_STATE_GROUP_ENUM(sensorGrp));
        SENSOR_STATE_ENUM gatingstate = SENSOR_STATE_A_NORMAL;
        if (sensorGrp == SENSOR_STATE_A_GROUP)
        {
            gatingstate = SENSOR_STATE_A_UNBLOCK_VOLT;
        }
        else if (sensorGrp == SENSOR_STATE_B_GROUP)
        {
            gatingstate = SENSOR_STATE_B_UNBLOCK_VOLT;
        }
        else if (sensorGrp == SENSOR_STATE_C_GROUP)
        {
            gatingstate = SENSOR_STATE_C_UNBLOCK_VOLT;
        }
        else if (sensorGrp == SENSOR_STATE_D_GROUP)
        {
            gatingstate = SENSOR_STATE_D_UNBLOCK_VOLT;
        }
        //auto& lockstate = m_pPoreStateMapTab->GetLockStateData(sensorGrp);
        for (int i = startindex, n = block + startindex; i < n; ++i)
        {
            int poreindex = i - startindex;
            m_arPoreStateData[index][poreindex] = porestate[i];
            m_arPoreStateData2[index][poreindex] = porestate[i];
            m_arPoreState[i] = porestate[i];
            m_arPoreState2[i] = porestate[i];

            if (vctlockState[i] != PROHIBITED_OPERATION_STATE && (porestate[i] != SENSOR_STATE_OFF /*&& porestate[i] != SENSOR_STATE_A_UNBLOCK_VOLT
                && porestate[i] != SENSOR_STATE_B_UNBLOCK_VOLT && porestate[i] != SENSOR_STATE_C_UNBLOCK_VOLT && porestate[i] != SENSOR_STATE_D_UNBLOCK_VOLT*/)
                && avgCurrent.size() > 0 && avgCurrent[i].size() > 0)
            {
                auto m_startCnt = std::chrono::steady_clock::now();

                auto buffer = avgCurrent[i];
                QJsonObject jsonObject;
                jsonObject.insert("data_name", m_strDataName.c_str());//
                jsonObject.insert("chn_num", i + 1);//

                QJsonArray jsonArrayChan;
                for (int j = 0, n = buffer.size(); j < n; ++j)
                {
                    jsonArrayChan.append(buffer[j]);//
                }
                jsonObject.insert("data", jsonArrayChan);

                QJsonDocument jsonDocDataFrame;
                jsonDocDataFrame.setObject(jsonObject);
                std::string strData = jsonDocDataFrame.toJson().toStdString();
                HttpClient httpcl;
                httpcl.SetIngorSSL(true);
                httpcl.GetReqHeader()["Content-Type"] = "application/json";
                bool ret = httpcl.RequestPost(strUrl, strData);
                if (!ret)
                {
                    LOGW("Get Gating status FAILED!!!url={}", strUrl.c_str());
                    continue;
                }

                size_t retsz = 0;
                auto retstr = httpcl.GetReadString(retsz);
                int isgating = ::atoi(retstr.c_str());
                if (isgating == 1)
                {
                    ++m_ariCurTimes[i];
                    int gatingcnt = m_ariCurTimes[i];
                    if (gatingcnt >= 3)
                    {
                        if (m_iAllowTime > 0 && gatingcnt >= (m_iAllowTime + 3))
                        {
                            //vetPoreStateData[poreindex] = SENSOR_STATE_OFF;
                            m_arPoreStateData[index][poreindex] = SENSOR_STATE_OFF;
                            m_arPoreState[i] = SENSOR_STATE_OFF;
                        }
                        else
                        {
                            //vetPoreStateData[poreindex] = gatingstate;
                            m_arPoreStateData[index][poreindex] = gatingstate;
                            m_arPoreState[i] = gatingstate;
                        }
                    }
                }
                else
                {
                    m_ariCurTimes[i] = 0;
                }

                auto m_stopCnt = std::chrono::steady_clock::now();
                auto distTime = std::chrono::duration<double>(m_stopCnt - m_startCnt).count();
                LOGI("SeqDegating={}, porestate={}, chan={}, gating={}", distTime, porestate[i], i, isgating);
            }
        }
        m_bDegatingCalc[index] = true;
        m_cvDegating.notify_all();
    }

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}
#endif

void DegatingPane::ThreadFunc2Deg(int index, int block)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    const int startindex = index * block;



    while (m_bRunning2)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexGating);
            m_cvGating.wait(lck);
        }

        if (!m_bRunning2)
        {
            break;
        }

        const auto& avgCurrent = m_vetAvgCur;
        if (avgCurrent.size() <= 0)
        {
            continue;
        }

        const auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
        auto &vctAllchDegateResultState = m_pModelSharedDataPtr->GetAllChDegateStateData();
        for (int i = startindex, n = block + startindex; i < n; ++i)
        {
            m_vctAllchFuncState[i] = vctAllchFuncState[i];

            if (vctAllchFuncState[i] != SENSORGRP_FUNC_STATE_OFF && avgCurrent.size() > 0 && avgCurrent[i].size() > 0)
            {
                auto buffer = avgCurrent[i];
                std::vector<int> label(buffer.size());
                int eps_p(50);
                int eps_c(10);
                int s_min_pts(10);
                int eps_d(10);
                int d_min_pts(1);
                bool pre_flag(true);

                std::vector<std::vector<double>> cluster_stats;
                LinearDBSCAN(cluster_stats, buffer, label, eps_p, eps_c, s_min_pts, eps_d, d_min_pts, pre_flag);

                bool isgating = isGating(label, cluster_stats, m_dSampleRate, m_dDurationThres, m_iSignalMin, m_iSignalMax, m_iGatingMin, m_iGatingMax, 10, m_iGatingSTD, m_iGatingSTDMax, 170);
                m_arIsGating[i] = isgating;

                if (isgating)
                {
                    ++m_ariCurTimes[i];
                    if (m_iAllowTime > 0 && m_ariCurTimes[i] >= m_iAllowTime)
                    {
                        m_vctAllchFuncState[i] = SENSORGRP_FUNC_STATE_OFF;
                    }
                    else
                    {
                        m_vctAllchFuncState[i] = SENSORGRP_FUNC_STATE_UNBLOCK;
                    }
                }
                else
                {
                    m_ariCurTimes[i] = 0;
                }
                vctAllchDegateResultState[i] = (isgating ?  RECORD_IS_DEGATE_STATE : RECORD_NONE_DEGATE_STATE);
                //break;
            }
        }
        m_bDegatingCalc[index] = true;
        m_cvDegating.notify_all();
    }

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

void DegatingPane::ThreadFunc2Deg2(int index, int block)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    auto strUrl = std::string("localhost:9112/gating_clf");//http://192.168.0.221:8000/gating_clf//ConfigServer::GetInstance()->GetVerupdateAddr();
    //if (ConfigServer::GetInstance()->GetWorknet() == 1)
    //{
    //    strUrl = std::string("http://172.16.18.12:9112/gating_clf");
    //}
    const int startindex = index * block;
    //if (startindex < 256)//0-255
    //{
    //    strUrl = std::string("localhost:9209/gating_clf");
    //}
    //else if (startindex < 512)//256-511
    //{
    //    strUrl = std::string("localhost:9210/gating_clf");
    //}
    //else if (startindex < 768)//512-767
    //{
    //    strUrl = std::string("localhost:9211/gating_clf");
    //}
    //else//768-1023
    //{
    //    strUrl = std::string("localhost:9212/gating_clf");
    //}


    while (m_bRunning2)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexGating);
            m_cvGating.wait(lck);
        }

        if (!m_bRunning2)
        {
            break;
        }

        const auto& avgCurrent = m_vetAvgCur;
        if (avgCurrent.size() <= 0)
        {
            continue;
        }
        int gatimes = m_pispGatingTime->value();
        int prtchan = m_pispGatingChannel->value();

        const auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
        auto &vctAllchDegateResultState = m_pModelSharedDataPtr->GetAllChDegateStateData();
        for (int i = startindex, n = block + startindex; i < n; ++i)
        {
                m_vctAllchFuncState[i] = vctAllchFuncState[i];
            if (vctAllchFuncState[i] != SENSORGRP_FUNC_STATE_OFF && avgCurrent.size() > 0 && avgCurrent[i].size() > 0)
            {
                auto m_startCnt = std::chrono::steady_clock::now();

                auto buffer = avgCurrent[i];
                QJsonObject jsonObject;
                jsonObject.insert("data_name", m_strDataName.c_str());//
                jsonObject.insert("chn_num", i + 1);//

                QJsonArray jsonArrayChan;
                for (int j = 0, n = buffer.size(); j < n; ++j)
                {
                    jsonArrayChan.append(buffer[j]);//
                }
                jsonObject.insert("data", jsonArrayChan);

                QJsonDocument jsonDocDataFrame;
                jsonDocDataFrame.setObject(jsonObject);
                std::string strData = jsonDocDataFrame.toJson().toStdString();
                HttpClient httpcl;
                httpcl.SetIngorSSL(true);
                httpcl.GetReqHeader()["Content-Type"] = "application/json";
                bool ret = httpcl.RequestPost(strUrl, strData);
                if (!ret)
                {
                    LOGW("Get Gating status FAILED!!!url={}", strUrl.c_str());
                    continue;
                }

                size_t retsz = 0;
                auto retstr = httpcl.GetReadString(retsz);
                int isgating = ::atoi(retstr.c_str());
                if (isgating == 1)
                {
                    ++m_ariCurTimes[i];
                    int gatingcnt = m_ariCurTimes[i];
                    if (gatingcnt >= gatimes)
                    {
                        if (m_iAllowTime > 0 && gatingcnt >= (m_iAllowTime + gatimes))
                        {
                            m_vctAllchFuncState[i] = SENSORGRP_FUNC_STATE_OFF;
                        }
                        else
                        {
                            m_vctAllchFuncState[i] = SENSORGRP_FUNC_STATE_UNBLOCK;
                        }
                    }
                }
                else
                {
                    m_ariCurTimes[i] = 0;
                }
                vctAllchDegateResultState[i] = (isgating ==1 ?  RECORD_IS_DEGATE_STATE : RECORD_NONE_DEGATE_STATE);

                if (prtchan == 0 || prtchan == i + 1)
                {
                    auto m_stopCnt = std::chrono::steady_clock::now();
                    auto distTime = std::chrono::duration<double>(m_stopCnt - m_startCnt).count();
                    LOGDEGDATAI("SeqDegating={}, funcstate={}, chan={}, times={}, gating={}", distTime, vctAllchFuncState[i], i, m_ariCurTimes[i], isgating);
                }
            }
        }
        m_bDegatingCalc[index] = true;
        m_cvDegating.notify_all();
    }

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool DegatingPane::EndThread2(void)
{
    m_bRunning2 = false;
    m_cvGating.notify_all();
    return false;
}

bool DegatingPane::StopThread2(void)
{
    m_bRunning2 = false;
    m_cvGating.notify_all();
    for (int i = 0; i < m_ciThread; ++i)
    {
        auto& pthr = m_pthrGating[i];
        if (pthr != nullptr)
        {
            if (pthr->joinable())
            {
                pthr->join();
            }
            pthr.reset();
        }
    }
    return false;
}

bool DegatingPane::StartThread3(void)
{
    if (!m_bRunning3 && m_pthrDegating == nullptr)
    {
        m_bRunning3 = true;
        const int block = CHANNEL_NUM / m_ciThread;
        m_pthrDegating = std::make_shared<std::thread>(&DegatingPane::ThreadFunc3Deg, this, block);
        return m_pthrDegating != nullptr;
    }
    return false;
}

void DegatingPane::ThreadFunc3Deg(int block)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    for(int thd = 0; thd < m_ciThread; ++thd)
    {
        m_arryBlockCh[thd].resize(block);
        for (int index = 0; index < block; ++index)
        {
            int ch = index * m_ciThread + thd;
            m_arryBlockCh[thd][index] = ch;
        }
        m_arrBlockChFuncState[thd].resize(block);
        m_arrBlockChFuncStateBackup[thd].resize(block);
        std::fill_n(m_arrBlockChFuncState[thd].begin(),block,SENSORGRP_FUNC_STATE_OFF);
        std::fill_n(m_arrBlockChFuncStateBackup[thd].begin(),block,SENSORGRP_FUNC_STATE_OFF);
    }
    while (m_bRunning3)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexDegating);
            m_cvDegating.wait(lck, [&] {
                bool calcready = true;
                for (int i = 0; i < m_ciThread; ++i)
                {
                    if (!m_bDegatingCalc[i])
                    {
                        calcready = false;
                        break;
                    }
                }
                return (!m_bRunning3 || calcready);
                });
        }
        if (!m_bRunning3)
        {
            break;
        }

        bool allready = true;
        for (int k = 0; k < m_ciThread; ++k)
        {
            if (!m_bDegatingCalc[k])
            {
                allready = false;
                break;
            }
        }
        if (!allready)
        {
            continue;
        }
        else
        {
#if 0
            for (int i = 0; i < m_ciThread; ++i)
            {
                m_bDegatingCalc[i] = false;

                int startindex = i * block;
                m_pSensorPane->SpeChanSensorSpeciSetSlot(startindex, startindex + block, m_arPoreStateData[i]);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(m_iDurationTime));
            for (int i = 0; i < m_ciThread; ++i)
            {
                int startindex = i * block;
                m_pSensorPane->SpeChanSensorSpeciSetSlot(startindex, startindex + block, m_arPoreStateData2[i]);
            }
#endif

#if 0
            for (int i = 0; i < m_ciThread; ++i)
            {
                m_bDegatingCalc[i] = false;

                int startindex = i * block;
                m_pSensorPane->SpeChanSensorSpeciSetSlot(startindex, startindex + block, m_arPoreStateData[i]);
                std::this_thread::sleep_for(std::chrono::milliseconds(m_iDurationTime));
                m_pSensorPane->SpeChanSensorSpeciSetSlot(startindex, startindex + block, m_arPoreStateData2[i]);

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

#elif 0
            auto& sensorstate = m_pModelSharedDataPtr->GetSensorStateData();
            for (int i = 0; i < m_ciThread; ++i)
            {
                m_bDegatingCalc[i] = false;
                for (int j = 0; j < block; ++j)
                {
                    int startindex = j * m_ciThread + i;
                    m_pSensorPane->OneChanSensorSpeciSetSlot(startindex, m_arPoreState[startindex]);
                    if (m_arPoreState[startindex] == SENSOR_STATE_OFF)
                    {
                        sensorstate[startindex] = SENSOR_STATE_OFF;
                    }
                }
                //ConfigServer::InterruptSleep(m_iDurationTime, m_bRunning3);
                std::this_thread::sleep_for(std::chrono::milliseconds(m_iDurationTime));
                for (int j = 0; j < block; ++j)
                {
                    int startindex = j * m_ciThread + i;
                    if (m_arPoreState[startindex] != SENSOR_STATE_OFF)
                    {
                        auto& state2 = m_arPoreState2[startindex];
                        if (state2 == SENSOR_STATE_A_UNBLOCK_VOLT)
                        {
                            state2 = SENSOR_STATE_A_NORMAL;
                        }
                        else if (state2 == SENSOR_STATE_B_UNBLOCK_VOLT)
                        {
                            state2 = SENSOR_STATE_B_NORMAL;
                        }
                        else if (state2 == SENSOR_STATE_C_UNBLOCK_VOLT)
                        {
                            state2 = SENSOR_STATE_C_NORMAL;
                        }
                        else if (state2 == SENSOR_STATE_D_UNBLOCK_VOLT)
                        {
                            state2 = SENSOR_STATE_D_NORMAL;
                        }
                        m_pSensorPane->OneChanSensorSpeciSetSlot(startindex, m_arPoreState2[startindex]);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
#else
            for (int thd = 0; thd < m_ciThread; ++thd)
            {
                m_bDegatingCalc[thd] = false;
                for (int j = 0; j < block; ++j)
                {
                    int ch = j * m_ciThread + thd;
                    const auto &funcState = m_vctAllchFuncState[ch];
                    m_arrBlockChFuncState[thd][j] = funcState;
                    m_arrBlockChFuncStateBackup[thd][j] = funcState;
                    if(funcState  == SENSORGRP_FUNC_STATE_UNBLOCK)
                    {
#ifdef EASY_PROGRAM_DEBUGGING
                        m_arrBlockChFuncStateBackup[thd][j] = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
#else
                        m_arrBlockChFuncStateBackup[thd][j] = SENSORGRP_FUNC_STATE_NORMAL;
#endif
                    }
                }

                m_pSensorPane->RecordDegateLog(m_arryBlockCh[thd],m_arrBlockChFuncState[thd]);
                //设置相应channel为unblock
                m_pSensorPane->IncontinueChDifferFuncStateSetSlot(m_arryBlockCh[thd],m_arrBlockChFuncState[thd]);

                //ConfigServer::InterruptSleep(m_iDurationTime, m_bRunning3);
                std::this_thread::sleep_for(std::chrono::milliseconds(m_iDurationTime));

                //设置相应channel为normal
                m_pSensorPane->IncontinueChDifferFuncStateSetSlot(m_arryBlockCh[thd],m_arrBlockChFuncStateBackup[thd]);

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
#endif

            ConfigServer::InterruptSleep((m_iCyclePeriod - m_iDegatingDataTime) * 1000, m_bRunning3);
            //std::this_thread::sleep_for(std::chrono::milliseconds((m_iCyclePeriod - m_iDegatingDataTime) * 1000));
            for (int i = 0; i < m_ciThread; ++i)
            {
                m_bDegatingSample[i] = true;
            }
        }
    }
    LOGI("ThreadExit={}!Left={}!={}", __FUNCTION__, m_queDataHandle.size_approx(), ConfigServer::GetCurrentThreadSelf());
}

bool DegatingPane::EndThread3(void)
{
    m_bRunning3 = false;
    m_cvDegating.notify_all();
    return false;
}

bool DegatingPane::StopThread3(void)
{
    m_bRunning3 = false;
    m_cvDegating.notify_one();
    if (m_pthrDegating.get() != nullptr)
    {
        if (m_pthrDegating->joinable())
        {
            m_pthrDegating->join();
        }
        m_pthrDegating.reset();
    }
    return false;
}

void DegatingPane::onClickEnableDegating(bool chk)
{
    if (chk)
    {
        m_pispCyclePeriod->setEnabled(false);
        m_pispAllowTimes->setEnabled(false);
        m_pchkAutoseqGating->setEnabled(false);

        onClickDegatingApply();
        onClickAdvanceApply();

        for (int k = 0; k < m_ciThread; ++k)
        {
            m_bDegatingSample[k] = true;
            m_bDegatingCalc[k] = false;
        }
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_ariCurTimes[i] = 0;
        }

        StartThread2();
        StartThread();
        StartThread3();
        ConfigServer::GetInstance()->SetAutoDegating(true);

        m_pbtnDegatingEnable->setText("Disable");
    }
    else
    {

        EndThread2();
        EndThread();
        EndThread3();
        ConfigServer::GetInstance()->SetAutoDegating(false);
        for (int k = 0; k < m_ciThread; ++k)
        {
            m_bDegatingSample[k] = false;
        }
        StopThread();
        StopThread2();
        StopThread3();

        m_pispCyclePeriod->setEnabled(true);
        m_pispAllowTimes->setEnabled(true);
        m_pchkAutoseqGating->setEnabled(true);

        m_pbtnDegatingEnable->setText("Enable");
    }
}

void DegatingPane::onClickDegatingApply(void)
{
    m_dDegatingVolt = m_pdspDegatingVolt->value();
    m_iCyclePeriod = m_pispCyclePeriod->value();
    m_iDurationTime = m_pispDurationTime->value();
    m_iSteppingTime = m_pispSteppingTime->value();
    m_iAllowTime = m_pispAllowTimes->value();
    m_iLimitCycle = m_pispLimitCycle->value();
}

void DegatingPane::onClickDefaultApply(void)
{
    m_pdspDegatingVolt->setValue(-0.1);
    m_pispCyclePeriod->setValue(2);
    m_pispDurationTime->setValue(200);

    m_pdspDurationThres->setValue(0.5);
    m_pispGatingSTD->setValue(5);
    m_pispGatingSTDMax->setValue(105);
    m_pispSignalMin->setValue(20);
    m_pispSignalMax->setValue(150);
    m_pispGatingMin->setValue(40);
    m_pispGatingMax->setValue(130);
}

void DegatingPane::onClickAdvanceApply(void)
{
    m_dDurationThres = m_pdspDurationThres->value();
    m_iGatingSTD = m_pispGatingSTD->value();
    m_iGatingSTDMax = m_pispGatingSTDMax->value();
    m_iSignalMin = m_pispSignalMin->value();
    m_iSignalMax = m_pispSignalMax->value();
    m_iGatingMin = m_pispGatingMin->value();
    m_iGatingMax = m_pispGatingMax->value();
}

void DegatingPane::setDegatingDataNameSlot(const QString& qsname)
{
    m_strDataName = qsname.toStdString();
}
void DegatingPane::TempControlDegateSlot(const bool &enable)
{
    if(enable)
    {
        if(!m_pbtnDegatingEnable->isChecked())
        {
            m_pbtnDegatingEnable->click();
        }
    }
    else
    {
        if(m_pbtnDegatingEnable->isChecked())
        {
            m_pbtnDegatingEnable->click();
        }
    }
}


void DegatingPane::AcquireDegateDelaymsSlot(int &delayms)
{
    delayms = m_pispDurationTime->value();
}

void DegatingPane::AcquireDegateIsRunningSlot(bool &bIsRuning)
{
    bIsRuning = m_pbtnDegatingEnable->isChecked();
}
void DegatingPane::UpdateDegatingParametersSlot(const float &duratethres,const int &seqStdMin,const int &seqStdMax, const int &signalMin, const int &signalMax,const int &gatingMin,const int &gatingMax)
{
    m_pdspDurationThres->setValue(duratethres);
    m_pispGatingSTD->setValue(seqStdMin);
    m_pispGatingSTDMax->setValue(seqStdMax);
    m_pispSignalMin->setValue(signalMin);
    m_pispSignalMax->setValue(signalMax);
    m_pispGatingMin->setValue(gatingMin);
    m_pispGatingMax->setValue(gatingMax);
    onClickAdvanceApply();
}
void DegatingPane::UpdateDegateBaseParametersSlot(const int &cyclePeriod,const int &durationTime,const int &allowtimes, const bool &bAutoseqgating)
{
    m_pispCyclePeriod->setValue(cyclePeriod);
    m_pispDurationTime->setValue(durationTime);
    m_pispAllowTimes->setValue(allowtimes);
    m_pchkAutoseqGating->setChecked(bAutoseqgating);
    onClickDegatingApply();
}
bool DegatingPane::EndAndStopAllThread(void)
{
    EndThread();
    EndThread2();
    EndThread3();
    StopThread();
    StopThread2();
    StopThread3();
    return true;
}
