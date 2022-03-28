#include "CustomPlotView.h"

#include <Log.h>


CustomPlotView::CustomPlotView(QWidget *parent) : QWidget(parent)
{
    InitCtrl();
    InitPlot();

    connect(this, SIGNAL(replotSignal(bool)), this, SLOT(replotSlot(bool)));
}

bool CustomPlotView::InitCtrl(void)
{
    auto ymax = ConfigServer::GetInstance()->GetPlotYmax();
    auto ymin = ConfigServer::GetInstance()->GetPlotYmin();
    auto xtim = ConfigServer::GetInstance()->GetPlotXnum();
    m_dSampleRate = ConfigServer::GetInstance()->GetSampleRate();
    m_dPintNum = xtim * m_dSampleRate;

    m_plabYmax = new QLabel(tr("YMax"), this);
    m_pdspYmax = new QDoubleSpinBox(this);
    m_pdspYmax->setMaximum(m_cdYmax);
    m_pdspYmax->setMinimum(-m_cdYmax);
    m_pdspYmax->setValue(ymax);
    m_plabYmin = new QLabel(tr("YMin"), this);
    m_pdspYmin = new QDoubleSpinBox(this);
    m_pdspYmin->setMinimum(-m_cdYmax);
    m_pdspYmin->setMaximum(m_cdYmax);
    m_pdspYmin->setValue(ymin);
    m_plabPtnum = new QLabel(tr("Time"), this);
    m_pdspPtnum = new QDoubleSpinBox(this);
    m_pdspPtnum->setSuffix(" s");
    m_pdspPtnum->setMinimum(0.5);
    m_pdspPtnum->setMaximum(m_cdXmax);
    m_pdspPtnum->setValue(10.0);
    m_pbtnPlotApply = new QPushButton(tr("Apply"), this);
    connect(m_pbtnPlotApply, &QPushButton::clicked, this, &CustomPlotView::OnPlotApply);


    m_pCustomPlot = new QCustomPlot(this);
    //m_pCustomPlot->setFixedSize(500,600);

    QHBoxLayout* phorlayout1 = new QHBoxLayout();
    phorlayout1->addStretch(5);
    phorlayout1->addWidget(m_plabYmax);
    phorlayout1->addWidget(m_pdspYmax);
    phorlayout1->addWidget(m_plabYmin);
    phorlayout1->addWidget(m_pdspYmin);
    phorlayout1->addWidget(m_plabPtnum);
    phorlayout1->addWidget(m_pdspPtnum);
    phorlayout1->addWidget(m_pbtnPlotApply);
    phorlayout1->addStretch(5);

    QVBoxLayout* pverlayout1 = new QVBoxLayout();
    pverlayout1->addWidget(m_pCustomPlot);
    pverlayout1->addLayout(phorlayout1);

    setLayout(pverlayout1);
    
    return false;
}

bool CustomPlotView::InitPlot(void)
{
    QPen pen;
    pen.setColor(Qt::red);//主刻度红色
    pen.setWidth(1);//线宽2
    m_pCustomPlot->xAxis->setTickPen(pen);
    m_pCustomPlot->xAxis->setTickLengthIn(6);//主刻度向内延伸30
    m_pCustomPlot->xAxis->setTickLengthOut(2);//主刻度向外延伸10
    pen.setColor(Qt::blue);//子刻度蓝色
    m_pCustomPlot->xAxis->setSubTickPen(pen);
    m_pCustomPlot->xAxis->setSubTickLengthIn(3);//子刻度向内延伸15
    m_pCustomPlot->xAxis->setSubTickLengthOut(1);//子刻度向外延伸5

    m_pCustomPlot->axisRect()->setupFullAxesBox();
    m_pCustomPlot->xAxis->setLabel("Time s/ch");
    m_pCustomPlot->yAxis->setLabel("Current pA/ch");
    m_pCustomPlot->yAxis->ticker()->setTickCount(8);  //设置刻度数量
    m_pCustomPlot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability); ////可读性优于设置
    //使上下轴、左右轴范围同步
    connect(m_pCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_pCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_pCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_pCustomPlot->yAxis2, SLOT(setRange(QCPRange)));
    //定时器连接槽函数
    //m_pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_pCustomPlot->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_pCustomPlot->addLayer("layer1", 0, QCustomPlot::limAbove);
    m_pCustomPlot->layer("layer1")->setMode(QCPLayer::lmBuffered);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_pCustomPlot->addGraph();
        m_pCustomPlot->graph(i)->setPen(QPen(g_vetPenColor[i % 32]));
        m_pCustomPlot->graph(i)->setLayer("layer1");
    }

    //m_pCustomPlot->setOpenGl(true);
    //qDebug() << "Is start opengl?=" << m_pCustomPlot->openGl();
    //LOGI("Is start opengl?={}", m_pCustomPlot->openGl());

    return false;
}

void CustomPlotView::OnPlotApply(void)
{
    double ymax = m_pdspYmax->value();
    double ymin = m_pdspYmin->value();
    double xtim = m_pdspPtnum->value();
    m_dSampleRate = ConfigServer::GetInstance()->GetSampleRate();
    m_dPintNum = xtim * m_dSampleRate;

    ConfigServer::GetInstance()->SetPlotYmax(ymax);
    ConfigServer::GetInstance()->SetPlotYmin(ymin);
    ConfigServer::GetInstance()->SetPlotXnum(xtim);

    m_pCustomPlot->xAxis->setRange(0.0, xtim);
    m_pCustomPlot->yAxis->setRange(ymin, ymax);
    //m_pCustomPlot->xAxis->rescale();
    //m_pCustomPlot->yAxis->rescale();

    m_pCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

bool CustomPlotView::LoadConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    auto ymax = ConfigServer::GetInstance()->GetPlotYmax();
    auto ymin = ConfigServer::GetInstance()->GetPlotYmin();
    auto xtim = ConfigServer::GetInstance()->GetPlotXnum();
    m_dSampleRate = ConfigServer::GetInstance()->GetSampleRate();
    m_dPintNum = xtim * m_dSampleRate;

    m_pdspPtnum->setValue(xtim);
    m_pCustomPlot->xAxis->setRange(0.0, xtim);
    m_pCustomPlot->yAxis->setRange(ymin, ymax);

    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        pset->beginGroup(QString("ch%1").arg(i));

        if (pset->contains("Color"))
        {
            QColor color(pset->value("Color").toString());
            m_pCustomPlot->graph(i)->setPen(QPen(color));
        }

        pset->endGroup();
    }

    m_pCustomPlot->setOpenGl(ConfigServer::GetInstance()->GetUseOpenGL() == 1);

    return true;
}

bool CustomPlotView::SaveConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    pset->beginGroup("plotCtrl");
    pset->setValue("yPlotMax", m_pdspYmax->value());
    pset->setValue("yPlotMin", m_pdspYmin->value());
    pset->setValue("xPlotNum", m_pdspPtnum->value());
    pset->setValue("isOpenGL", ConfigServer::GetInstance()->GetUseOpenGL());
    pset->endGroup();

    return true;
}

void CustomPlotView::replotSlot(bool isClear)
{
    if (isVisible())
    {
        std::lock_guard<std::mutex> lock(m_lock);
        //m_pCustomPlot->replot();
        //m_pCustomPlot->replot(QCustomPlot::rpQueuedReplot);
        m_pCustomPlot->layer("layer1")->replot(isClear);
    }
}

bool CustomPlotView::StartThread(void)
{
    //if (m_pUsbbulk == nullptr)
    //{
    //    LOGE("getUsbBulk null!!!");
    //    return false;
    //}
    //auto pusbDevice = m_pUsbbulk->GetUsbCyDevice()->GetCurDevice();
    //if (pusbDevice == nullptr)
    //{
    //    LOGE("get plotcurusbdev addr null");
    //    return false;
    //}
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&CustomPlotView::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void CustomPlotView::ThreadFunc(void)
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
            bool breset = false;
            {
                std::lock_guard<std::mutex> lock(m_lock);
                if (m_pCustomPlot->graph(30)->dataCount() >= m_dPintNum)
                {
                    for (int i = 0; i < CHANNEL_NUM; ++i)
                    {
                        m_pCustomPlot->graph(i)->data()->clear();
                    }
                    breset = true;
                }

                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    for (size_t j = 0, m = dtpk->dataHandle[i].size(); j < m; ++j)
                    {
                        m_pCustomPlot->graph(i)->addData(m_pCustomPlot->graph(i)->dataCount() * 1.0 / m_dSampleRate, dtpk->dataHandle[i][j]);
                        //m_pCustomPlot->graph(i)->addData(m_pCustomPlot->graph(i)->dataCount(), 0.0387 * (dtpk->dataHandle[i][j] - 2150.0));
                    }
                }
            }
            if (m_bChangedSample)
            {
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    m_pCustomPlot->graph(i)->data()->clear();
                }
                m_bChangedSample = false;
            }

            //replotSlot(breset);
            emit replotSignal(breset);
        }
    }
    LOGI("ThreadExit={}!Left={}!={}", __FUNCTION__, m_queDataHandle.size_approx(), ConfigServer::GetCurrentThreadSelf());

    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));
}

bool CustomPlotView::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return true;
}

bool CustomPlotView::StopThread(void)
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
    //for (int i = 0; i < CHANNEL_NUM; ++i)
    //{
    //    m_pCustomPlot->graph(i)->data()->clear();
    //}
    return true;
}

void CustomPlotView::checkChangedSlot(int chan, bool chk)
{
    m_pCustomPlot->graph(chan)->setVisible(chk);
}

void CustomPlotView::colorChangedSlot(int chan, const QColor& clr)
{
    m_pCustomPlot->graph(chan)->setPen(QPen(clr));
}

void CustomPlotView::setSamplingRateSlot(int rate)
{
    if (rate != m_dSampleRate)
    {
        //std::lock_guard<std::mutex> lock(m_lock);
        m_bChangedSample = true;
        double xtim = m_pdspPtnum->value();
        m_dSampleRate = rate;
        m_dPintNum = xtim * rate;

        //m_pCustomPlot->xAxis->setRange(0.0, xtim);
        //m_pCustomPlot->replot(QCustomPlot::rpQueuedReplot);
        //emit replotSignal(true);
    }
}

void CustomPlotView::OnEnableOpengl(const bool &check)
{
    if (m_pCustomPlot->openGl() != check)
    {
        m_pCustomPlot->setOpenGl(check);
        m_pCustomPlot->replot(QCustomPlot::rpQueuedReplot);
        ConfigServer::GetInstance()->SetUseOpenGL(check ? 1 : 0);
        LOGI("Is start opengl?={}", m_pCustomPlot->openGl());
    }
}
