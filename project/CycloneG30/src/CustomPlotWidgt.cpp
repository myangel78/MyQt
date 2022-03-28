#include "CustomPlotWidgt.h"
#include "Log.h"
#include "table/PlotCfgTabModel.h"
#include "table/PlotCfgDelegate.h"
#include "table/PoreStateMapTabModel.h"
#include "SensorPanel.h"


CustomPlotWidgt::CustomPlotWidgt(QWidget *parent):QWidget(parent)
{
    memset(m_arrayCurRecod,0,CHANNEL_NUM);
    InitCtrl();
    InitPlot();
    connect(this, SIGNAL(replotSignal(bool)), this, SLOT(replotSlot(bool)));
}

CustomPlotWidgt::~CustomPlotWidgt()
{
//    bool bret = false;
//    bool bret = m_pCustomPlot->clearGraphs();
//    qDebug("m_pCustomPlot->clearGraphs() %d",bret);
//    for(int i = 0; i < CHANNEL_NUM; ++i)
//    {
//        bret = m_pCustomPlot->removeGraph(i);
//        qDebug("m_pCustomPlot->removeGraph(i)) %d %d",i,bret);
//    }
//    delete m_pCustomPlot;
}


void CustomPlotWidgt::InitCtrl(void)
{
    auto ymax = ConfigServer::GetInstance()->GetPlotYmax();
    auto ymin = ConfigServer::GetInstance()->GetPlotYmin();
    auto xtim = ConfigServer::GetInstance()->GetPlotXnum();
    m_dSampleRate = ConfigServer::GetInstance()->GetSampleRate();
    m_dPintNum = xtim * m_dSampleRate;
    m_dAxisUnit = 1.0/m_dSampleRate;

    m_pAixsSetBtn = new QPushButton(QStringLiteral("Apply"),this);
    m_pChanelSetBtn = new QPushButton(QStringLiteral("Apply"),this);

    m_plabYmin = new QLabel(QStringLiteral("Ymin(&I)"),this);
    m_plabYmax = new QLabel(QStringLiteral("Ymax(&B)"),this);
    m_plabXtime = new QLabel(QStringLiteral("Times(&T)"),this);
    m_pdspYmax = new QDoubleSpinBox(this);
    m_pdspYmin = new QDoubleSpinBox(this);
    m_pdspXtime = new QDoubleSpinBox(this);

    setTabOrder(m_pdspYmin,m_pdspYmax);
    setTabOrder(m_pdspYmax,m_pdspXtime);
    m_pdspXtime->setValue(10);
    m_pdspXtime->setSuffix(QStringLiteral(" s"));

    m_pdspYmax->setMaximum(m_cdYmax);
    m_pdspYmax->setMinimum(-m_cdYmax);
    m_pdspYmax->setValue(ymax);

    m_pdspYmin->setMinimum(-m_cdYmax);
    m_pdspYmin->setMaximum(m_cdYmax);
    m_pdspYmin->setValue(ymin);

    m_pdspXtime->setSuffix(" s");
    m_pdspXtime->setMinimum(0.5);
    m_pdspXtime->setMaximum(m_cdXmax);
    m_pdspXtime->setValue(10.0);

    m_plabYmax->setBuddy(m_pdspYmax);
    m_plabYmin->setBuddy(m_pdspYmin);
    m_plabXtime->setBuddy(m_pdspXtime);


    m_pPlotCfgToolBtn = new QToolButton(this);
    QMenu *pPlotCfgmenu=new QMenu(QStringLiteral("Menu"),this);
    m_pPlotColorVisAct = new QAction(QStringLiteral("Channel setting"),this);
    pPlotCfgmenu->addAction(m_pPlotColorVisAct);
    m_pPlotCfgToolBtn->setArrowType(Qt::NoArrow);
    m_pPlotCfgToolBtn->setIcon( QPixmap( ":/img/img/menu1.png" ) );
    m_pPlotCfgToolBtn->setIconSize(QSize(25,25));
    m_pPlotCfgToolBtn->setPopupMode(QToolButton::InstantPopup);
    m_pPlotCfgToolBtn->setMenu(pPlotCfgmenu);
    m_pPlotCfgToolBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);

    m_pLabChStartIndex = new QLabel(QStringLiteral("Start(&L)"),this);
    m_pLabChEndIndex = new QLabel(QStringLiteral("End(&H)"),this);
    m_pSpChStartIndex = new QSpinBox(this);
    m_pSpChEndIndex = new QSpinBox(this);
    m_pSpChStartIndex->setPrefix(QStringLiteral("Ch "));
    m_pSpChEndIndex->setPrefix(QStringLiteral("Ch "));
    m_pSpChStartIndex->setRange(1,1024);
    m_pSpChEndIndex->setRange(1,1024);
    m_pSpChStartIndex->setValue(1);
    m_pSpChEndIndex->setValue(1024);

    m_pLabChStartIndex->setBuddy(m_pSpChStartIndex);
    m_pLabChEndIndex->setBuddy(m_pSpChEndIndex);

    m_pCustomPlot = new QCustomPlot(this);
    m_pCusPlotCurvesCfgDlg = new CusPlotCurvesCfgDlg(this,this);
    m_pPLotCfgTabModel = m_pCusPlotCurvesCfgDlg->GetPLotCfgTabModelPtr();
//    m_pCusPlotCurvesCfgDlg->setWindowModality(Qt::NonModal);

    int colIndx = 0;
    QGridLayout *gridlayout1 = new QGridLayout();
    gridlayout1->addItem(new QSpacerItem(70, 10),0,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->addItem(new QSpacerItem(70, 10),1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_plabYmin,0,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pdspYmin,1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_plabYmax,0,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pdspYmax,1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_plabXtime,0,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pdspXtime,1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pAixsSetBtn,1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addItem(new QSpacerItem(30, 10),1,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->setColumnStretch(colIndx++,10);
    gridlayout1->addWidget(m_pLabChStartIndex,0,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pSpChStartIndex,1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pLabChEndIndex,0,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pSpChEndIndex,1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pChanelSetBtn,1,colIndx++,1,1,Qt::AlignLeft);
    gridlayout1->addWidget(m_pPlotCfgToolBtn,1,colIndx,1,1,Qt::AlignLeft);
    gridlayout1->setVerticalSpacing(0);
    m_pPlotCfgToolBtn->setProperty("popupMode",1);



    QVBoxLayout* vertlayout = new QVBoxLayout();
    vertlayout->addWidget(m_pCustomPlot);
    vertlayout->addLayout(gridlayout1);
    vertlayout->setStretch(0,20);
    vertlayout->setSpacing(0);
//    vertlayout->setStretch(2,1);
    setLayout(vertlayout);

    connect(m_pAixsSetBtn, &QPushButton::clicked, this, &CustomPlotWidgt::OnPlotApply);
    connect(m_pChanelSetBtn,&QPushButton::clicked,this,&CustomPlotWidgt::OnChannelSetBtnSlot);
    connect(m_pPlotCfgToolBtn,&QPushButton::clicked,this,&CustomPlotWidgt::OnCurvesCfgClkSlot);
    connect(m_pPlotColorVisAct,&QAction::triggered,this,&CustomPlotWidgt::OnCurvesCfgClkSlot);

    connect(m_pCusPlotCurvesCfgDlg,&CusPlotCurvesCfgDlg::CurveVisibleChgeSig,this,&CustomPlotWidgt::CurveVisibleChgeSlot);
    connect(m_pCusPlotCurvesCfgDlg,&CusPlotCurvesCfgDlg::CurveColorChgeSig,this,&CustomPlotWidgt::CurveColorChgeSlot);
    connect(m_pCusPlotCurvesCfgDlg,&CusPlotCurvesCfgDlg::AllCurvesVisibleSig,this,&CustomPlotWidgt::AllCurvesVisibleSlot);

    connect(m_pCusPlotCurvesCfgDlg,&CusPlotCurvesCfgDlg::GetAllChAvgSig,this,&CustomPlotWidgt::GetAllChAvgSig);
    connect(m_pCusPlotCurvesCfgDlg,&CusPlotCurvesCfgDlg::GetAllChStdSig,this,&CustomPlotWidgt::GetAllChStdSig);


}

bool CustomPlotWidgt::InitPlot(void)
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
    m_pCustomPlot->xAxis->setRange(0,10000);

    m_pCustomPlot->axisRect()->setupFullAxesBox();
    m_pCustomPlot->xAxis->setLabel("Time s/ch");
    m_pCustomPlot->yAxis->setLabel("Current pA/ch");
    m_pCustomPlot->yAxis->setRange(0,6000);
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



#if 0
/**
 * @brief way1: singlethread processing
 */
void CustomPlotWidgt::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
    CalTimeElapse elpase;

    const double axisUnit = m_dAxisUnit;

    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataHandle.size_approx() > 0; });
        }

        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataHandle.try_dequeue(dtpk);
        if (bret)
        {
            bool breset = false;
            {
                std::lock_guard<std::mutex> lock(m_lock);
                elpase.StartCnt();
                if (m_pCustomPlot->graph(0)->dataCount() >= m_dPintNum)
                {
                    for (int i = 0; i < CHANNEL_NUM; ++i)
                    {
                        m_pCustomPlot->graph(i)->data()->clear();;
                    }
                    breset = true;

                }
                for (int ch = 0; ch < CHANNEL_NUM; ++ch)
                {
                    m_arrayCurRecod[ch] = dtpk->dataHandle[ch].back();
#if 0
                    for (size_t j = 0, m = dtpk->dataHandle[ch].size(); j < m; ++j)
                    {
                        m_pCustomPlot->graph(ch)->addData(m_pCustomPlot->graph(ch)->dataCount() * 1.0 / m_dSampleRate, dtpk->dataHandle[ch][j]);
                    }
#else
                    QVector<QCPGraphData> vctGrpahData;
                    vctGrpahData.resize((int)dtpk->dataHandle[ch].size());
                    int graphPointCnt = m_pCustomPlot->graph(ch)->dataCount();
                    std::transform(dtpk->dataHandle[ch].begin(), dtpk->dataHandle[ch].end(), vctGrpahData.begin(), [&axisUnit,&graphPointCnt](const float &value)
                    { return QCPGraphData(((graphPointCnt++) * axisUnit),value);});
                    m_pCustomPlot->graph(ch)->addData(std::move(vctGrpahData));
#endif
                }
            }
            elpase.StopCnt();
//            LOGCI("CustomPlotWidgt /*elpase*/ time ms: {}, m_queDataHandle.size_approx() {} thread: {}", elpase.GetTimeMsDifference(),m_queDataHandle.size_approx(),ConfigServer::GetInstance()->GetCurrentThreadSelf());

            emit replotSignal(breset);
        }
    }
    LOGI("m_queDataHandle size is {} ThreadExit={}!!!={}",m_queDataHandle.size_approx(), __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}
#else
/**
 * @brief way1: multithread processing
 */
#if 0
void CustomPlotWidgt::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
    CalTimeElapse elpase;

    std::vector<std::thread> vctThreads;
    const int THREAD_NUM = 4;
    const int block = CHANNEL_NUM / THREAD_NUM;
    const double axisUnit = m_dAxisUnit;

    while (m_bRunning)
    {

        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataHandle.size_approx() > 0; });
        }
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataHandle.try_dequeue(dtpk);

        if (bret)
        {
            bool breset = false;
            {

                std::lock_guard<std::mutex> lock(m_lock);
                elpase.StartCnt();
                if (m_pCustomPlot->graph(0)->dataCount() >= m_dPintNum)
                {
                    for (int i = 0; i < CHANNEL_NUM; ++i)
                    {
                        m_pCustomPlot->graph(i)->data()->clear();;
                    }
                    breset = true;
                }


                for (int i = 0; i < THREAD_NUM; ++i)
                {
                    vctThreads.emplace_back(std::thread( [&,i,block](){
                        int startch = i *block;
                        int endch = (i+1) * block;
                        for(int ch = startch; ch < endch; ++ch )
                        {
                            m_arrayCurRecod[ch] = dtpk->dataHandle[ch].back();
                            QVector<QCPGraphData> vctGrpahData;
                            vctGrpahData.resize(dtpk->dataHandle[ch].size());
                            int graphPointCnt = m_pCustomPlot->graph(ch)->dataCount();
                            std::transform(dtpk->dataHandle[ch].begin(), dtpk->dataHandle[ch].end(), vctGrpahData.begin(), [&axisUnit,&graphPointCnt](const float &value)
                            { return QCPGraphData(((graphPointCnt++) * axisUnit),value);});
                            m_pCustomPlot->graph(ch)->addData(std::move(vctGrpahData));
                        }}));
                }
                for(auto &thread : vctThreads){ thread.join();}
                vctThreads.clear();
                elpase.StopCnt();
//                LOGCI("CustomPlotWidgt /*elpase*/ time ms: {}, m_queDataHandle.size_approx() {} thread: {}", elpase.GetTimeMsDifference(),m_queDataHandle.size_approx(),ConfigServer::GetInstance()->GetCurrentThreadSelf());
            }
            emit replotSignal(breset);
        }
    }
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));
    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}
#else
void CustomPlotWidgt::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
    //CalTimeElapse elpase;

    const int futNum = 4;
    const int block = CHANNEL_NUM / futNum;

    while (m_bRunning)
    {

        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataHandle.size_approx() > 0; });
        }
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataHandle.try_dequeue(dtpk);

        if (bret)
        {
            bool breset = false;
            {

                std::lock_guard<std::mutex> lock(m_lock);
                //elpase.StartCnt();
                if (m_pCustomPlot->graph(0)->dataCount() >= m_dPintNum)
                {
                    for (int i = 0; i < CHANNEL_NUM; ++i)
                    {
                        m_pCustomPlot->graph(i)->data()->clear();;
                    }
                    breset = true;
                }

                auto handleFut1 = std::async(std::launch::async,&CustomPlotWidgt::AddPointsToGrpah,this,0,block,dtpk);
                auto handleFut2 = std::async(std::launch::async,&CustomPlotWidgt::AddPointsToGrpah,this,1,block,dtpk);
                auto handleFut3 = std::async(std::launch::async,&CustomPlotWidgt::AddPointsToGrpah,this,2,block,dtpk);
                auto handleFut4 = std::async(std::launch::async,&CustomPlotWidgt::AddPointsToGrpah,this,3,block,dtpk);

                handleFut1.get();
                handleFut2.get();
                handleFut3.get();
                handleFut4.get();

                //elpase.StopCnt();
//                LOGCI("CustomPlotWidgt /*elpase*/ time ms: {}, m_queDataHandle.size_approx() {} thread: {}", elpase.GetTimeMsDifference(),m_queDataHandle.size_approx(),ConfigServer::GetInstance()->GetCurrentThreadSelf());
            }
            emit replotSignal(breset);
        }
    }
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));
    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}
#endif
#endif


bool CustomPlotWidgt::AddPointsToGrpah(const int &indx, const int &block,std::shared_ptr<ConfigServer::SDataHandle> dtpk)
{
    int startch = indx *block;
    int endch = (indx+1) * block;
    const double axisUnit = m_dAxisUnit;
    for(int ch = startch; ch < endch; ++ch )
    {
        m_arrayCurRecod[ch] = dtpk->dataHandle[ch].back();
        QVector<QCPGraphData> vctGrpahData;
        vctGrpahData.resize(dtpk->dataHandle[ch].size());
        int graphPointCnt = m_pCustomPlot->graph(ch)->dataCount();
        std::transform(dtpk->dataHandle[ch].begin(), dtpk->dataHandle[ch].end(), vctGrpahData.begin(), [&axisUnit,&graphPointCnt](const float &value)
        { return QCPGraphData(((graphPointCnt++) * axisUnit),value);});
        m_pCustomPlot->graph(ch)->addData(std::move(vctGrpahData));
    }
    return true;
}


bool CustomPlotWidgt::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_pCusPlotCurvesCfgDlg->EnableAvgStdAutoRefresh(true);
        m_ThreadPtr = std::make_shared<std::thread>(&CustomPlotWidgt::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    m_pCusPlotCurvesCfgDlg->EnableAvgStdAutoRefresh(false);
    return false;
}

bool CustomPlotWidgt::EndThread(void)
{
    m_pCusPlotCurvesCfgDlg->EnableAvgStdAutoRefresh(false);
    m_bRunning = false;
    m_cv.notify_one();
    return true;
}

bool CustomPlotWidgt::StopThread(void)
{
    m_pCusPlotCurvesCfgDlg->EnableAvgStdAutoRefresh(false);
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
    return true;
}



void CustomPlotWidgt::OnEnableOpengl(const bool &check)
{
    if (m_pCustomPlot->openGl() != check)
    {
        m_pCustomPlot->setOpenGl(check);
        m_pCustomPlot->replot(QCustomPlot::rpQueuedReplot);
        ConfigServer::GetInstance()->SetUseOpenGL(check ? 1 : 0);
    }
}

void CustomPlotWidgt::replotSlot(bool isClear)
{
    if (isVisible())
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_pCustomPlot->layer("layer1")->replot(isClear);
    }
}


void CustomPlotWidgt::OnPlotApply(void)
{
    double ymax = m_pdspYmax->value();
    double ymin = m_pdspYmin->value();
    double xtim = m_pdspXtime->value();
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

void CustomPlotWidgt::OnSampRateChange(void)
{
    auto xtim = ConfigServer::GetInstance()->GetPlotXnum();
    m_dSampleRate = ConfigServer::GetInstance()->GetSampleRate();
    m_dPintNum = xtim * m_dSampleRate;
    m_dAxisUnit = 1.0/m_dSampleRate;
}


bool CustomPlotWidgt::LoadConfig(QSettings* pset)
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

    m_pdspXtime->setValue(xtim);
    m_pCustomPlot->xAxis->setRange(0.0, xtim);
    m_pCustomPlot->yAxis->setRange(ymin, ymax);

    //for (int ch = 0; ch < CHANNEL_NUM; ++ch)
    //{
    //    m_pCustomPlot->graph(ch)->preReserve(m_dPintNum + 1000);
    //}

//    for (int i = 0; i < CHANNEL_NUM; ++i)
//    {
//        pset->beginGroup(QString("ch%1").arg(i));

//        if (pset->contains("Color"))
//        {
//            QColor color(pset->value("Color").toString());
//            m_pCustomPlot->graph(i)->setPen(QPen(color));
//        }

//        pset->endGroup();
//    }

    #ifndef SHIELD_OPENGL_USE
    m_pCustomPlot->setOpenGl(ConfigServer::GetInstance()->GetUseOpenGL() == 1);
    LOGI("Is start opengl?={}", m_pCustomPlot->openGl());
    #endif

    return true;
}

bool CustomPlotWidgt::SaveConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    pset->beginGroup("plotCtrl");
    pset->setValue("yPlotMax", m_pdspYmax->value());
    pset->setValue("yPlotMin", m_pdspYmin->value());
    pset->setValue("xPlotNum", m_pdspXtime->value());
    pset->setValue("isOpenGL", ConfigServer::GetInstance()->GetUseOpenGL());
    pset->endGroup();

    return true;
}


void CustomPlotWidgt::OnChannelSetBtnSlot(void)
{
    int startch = m_pSpChStartIndex->value() -1;
    int endch = m_pSpChEndIndex->value()-1;

    auto &vctVisible = m_pPLotCfgTabModel->GetAllChVisibleRef();

    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        if( ch >= startch && ch <= endch )
        {
            m_pCustomPlot->graph(ch)->setVisible(true);
            vctVisible[ch] = true;
        }
        else
        {
             m_pCustomPlot->graph(ch)->setVisible(false);
             vctVisible[ch] = false;
        }
    }
    emit replotSignal(true);
}



void CustomPlotWidgt::OnCurvesCfgClkSlot(void)
{
#if 0
    m_pCusPlotCurvesCfgDlg->EnableAvgStdAutoRefresh(true);
//    m_pCusPlotCurvesCfgDlg->exec();
    m_pCusPlotCurvesCfgDlg->show();
    ConfigServer::WidgetPosAdjustByParent(this->parentWidget()->parentWidget(), m_pCusPlotCurvesCfgDlg);
    m_pCusPlotCurvesCfgDlg->EnableAvgStdAutoRefresh(false);
#endif
    m_pCusPlotCurvesCfgDlg->show();
    ConfigServer::WidgetPosAdjustByParent(this->nativeParentWidget(), m_pCusPlotCurvesCfgDlg);
}


void CustomPlotWidgt::CurveVisibleChgeSlot(int ch,bool visible)
{
    m_pCustomPlot->graph(ch)->setVisible(visible);
}
void CustomPlotWidgt::CurveColorChgeSlot(int ch,QColor color)
{
     m_pCustomPlot->graph(ch)->setPen(QPen(color));
}

void CustomPlotWidgt::AllCurvesVisibleSlot(const bool &chk)
{
    for(int ch  = 0; ch < m_pCustomPlot->graphCount(); ++ch)
    {
        m_pCustomPlot->graph(ch)->setVisible(chk);
    }

}

std::vector<bool> &CustomPlotWidgt::GetAllChSeqSteRef(void)
{
    return m_pPLotCfgTabModel->GetAllChSeqSteRef();
}

std::vector<bool> &CustomPlotWidgt::GetAllChVisibleRef(void)
{
    return m_pPLotCfgTabModel->GetAllChVisibleRef();
}

void CustomPlotWidgt::UpdateAllChVisibleView(const std::vector<bool> &vctVisible)
{
    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        m_pCustomPlot->graph(ch)->setVisible(vctVisible[ch]);
    }
}



CusPlotCurvesCfgDlg::CusPlotCurvesCfgDlg(CustomPlotWidgt *pCutomplotwgt,QWidget *parent):m_pCustomPlotWidgt(pCutomplotwgt),QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
    InitCtrl();
    setMinimumSize(230,800);
    setWindowTitle(QStringLiteral("Curves configuration"));
}

CusPlotCurvesCfgDlg::~CusPlotCurvesCfgDlg()
{
    if(m_pTmrUpdateAvgStd->isActive())
    {
        m_pTmrUpdateAvgStd->stop();
    }
    m_pTmrUpdateAvgStd->deleteLater();
    m_pSortFilterProxyModel->deleteLater();
}

void CusPlotCurvesCfgDlg::InitCtrl(void)
{

    m_curFilterType = PlotCfgSortFilterModel::FILTER_TYPE_CHANNEL;
    m_pTmrUpdateAvgStd = new QTimer(this);
    m_pCurvesCfgTbleView = new QTableView(this);
    m_pCurvesCfgTbleView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_pPlotCfgDelegate = new PlotCfgDelegate(m_pCurvesCfgTbleView);
    m_pPlotCfgModel = new PLotCfgTabModel(m_pCurvesCfgTbleView);


#if 0
    m_pCurvesCfgTbleView->setModel(m_pPlotCfgModel);
    m_pCurvesCfgTbleView->setSortingEnabled(true);
#else
    m_pSortFilterProxyModel = new PlotCfgSortFilterModel(m_pCurvesCfgTbleView);
    m_pSortFilterProxyModel->setSourceModel(m_pPlotCfgModel);
    m_pSortFilterProxyModel->setDynamicSortFilter(true); //改动单元格时会调用模型的filterAcceptsRow
    m_pSortFilterProxyModel->setFilterRole(Qt::DisplayRole);
    m_pCurvesCfgTbleView->setModel(m_pSortFilterProxyModel);
//    m_pCurvesCfgTbleView->setSortingEnabled(true); //开启后会对通道进行排序，对GUI的操作增多。暂不开启
#endif
    m_pCurvesCfgTbleView->setItemDelegate(m_pPlotCfgDelegate);
    m_pCurvesCfgTbleView->verticalHeader()->setHidden(true);
    m_pCurvesCfgTbleView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pCurvesCfgTbleView->setAlternatingRowColors(true);
    m_pCurvesCfgTbleView->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(156,156,156);color: white;}");
//    m_pCurvesCfgTbleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pCurvesCfgTbleView->setColumnWidth(PLotCfgTabModel::Channel,60);
    m_pCurvesCfgTbleView->setColumnWidth(PLotCfgTabModel::Visible,60);
    m_pCurvesCfgTbleView->setColumnWidth(PLotCfgTabModel::Color,40);
    m_pCurvesCfgTbleView->setColumnWidth(PLotCfgTabModel::AVG,60);
    m_pCurvesCfgTbleView->setColumnWidth(PLotCfgTabModel::STD,60);
    m_pCurvesCfgTbleView->setColumnWidth(PLotCfgTabModel::Sequence,62);
    m_pCurvesCfgTbleView->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_pCfgMenu = new QMenu(m_pCurvesCfgTbleView);
    m_pVisibleMenu = m_pCfgMenu->addMenu(QStringLiteral("Visible"));
    m_pAllChanInVisiAction = new QAction(QStringLiteral("All invisible"),this);
    m_pAllChanVisiAction = new QAction(QStringLiteral("All visible"),this);
    m_pAvgStdVisiAction = new QAction(QStringLiteral("Only AvgStd visible"),this);
    m_pSeqVisiAction = new QAction(QStringLiteral("Only Seq visible"),this);
    m_pvalidVisiAction = new QAction(QStringLiteral("Only Valid visible"),this);

    m_pVisibleMenu->addAction(m_pAllChanInVisiAction);
    m_pVisibleMenu->addAction(m_pAllChanVisiAction);
    m_pVisibleMenu->addAction(m_pAvgStdVisiAction);
    m_pVisibleMenu->addAction(m_pSeqVisiAction);
    m_pVisibleMenu->addAction(m_pvalidVisiAction);

    m_pVisibleOrNotMapper = new QSignalMapper(this);
    connect(m_pAllChanInVisiAction,SIGNAL(triggered()),m_pVisibleOrNotMapper,SLOT(map()));
    connect(m_pAllChanVisiAction,SIGNAL(triggered()),m_pVisibleOrNotMapper,SLOT(map()));
    connect(m_pAvgStdVisiAction,SIGNAL(triggered()),m_pVisibleOrNotMapper,SLOT(map()));
    connect(m_pSeqVisiAction,SIGNAL(triggered()),m_pVisibleOrNotMapper,SLOT(map()));
    connect(m_pvalidVisiAction,SIGNAL(triggered()),m_pVisibleOrNotMapper,SLOT(map()));

    m_pVisibleOrNotMapper->setMapping(m_pAllChanInVisiAction,ALL_CHAN_INVIVISBLE_TRIGER_ENUM);
    m_pVisibleOrNotMapper->setMapping(m_pAllChanVisiAction,ALL_CHAN_VIVISBLE_TRIGER_ENUM);
    m_pVisibleOrNotMapper->setMapping(m_pAvgStdVisiAction,AVGSTD_VIVISBLE_TRIGER_ENUM);
    m_pVisibleOrNotMapper->setMapping(m_pSeqVisiAction,SEQ_VIVISBLE_TRIGER_ENUM);
    m_pVisibleOrNotMapper->setMapping(m_pvalidVisiAction,VALID_VIVISBLE_TRIGER_ENUM);


    QLabel *pFiltTypeDescrlab = new QLabel(QStringLiteral("Type"),this);
    m_pCurFiltRowsCntLab = new QLabel(QStringLiteral("Count:"),this);
    m_pFiltTypeSelectComb = new QComboBox(this);
    m_pFiltTypeSelectComb->addItem(QStringLiteral("Channel"),PlotCfgSortFilterModel::FILTER_TYPE_CHANNEL);
    m_pFiltTypeSelectComb->addItem(QStringLiteral("Visible"),PlotCfgSortFilterModel::FILTER_TYPE_VISIBLE);
    m_pFiltTypeSelectComb->addItem(QStringLiteral("AvgStd"), PlotCfgSortFilterModel::FILTER_TYPE_AVG_STD);
    m_pFiltTypeSelectComb->addItem(QStringLiteral("Seq"),PlotCfgSortFilterModel::FILTER_TYPE_SEQUENCY);
    m_pFiltTypeSelectComb->addItem(QStringLiteral("Valid"),PlotCfgSortFilterModel::FILTER_TYPE_VALID);


    QGroupBox *groupbox = new QGroupBox(QStringLiteral("Curves table"),this);
    m_ctrlgroupbox = new QGroupBox(QStringLiteral("Control"),this);
    m_pFilterCurMinDpbx= new QDoubleSpinBox(this);
    m_pFilterCurMaxDpbx= new QDoubleSpinBox(this);
    m_pFilterStdMinDpbx= new QDoubleSpinBox(this);
    m_pFilterStdMaxDpbx= new QDoubleSpinBox(this);
    m_pAutoRefreshBtn = new QPushButton(QStringLiteral("AutoRefresh"),this);
    m_pAutoRefreshBtn->setCheckable(true);


    QLabel *pFilterMinLab = new QLabel(QStringLiteral("Min"),this);
    QLabel *pFilterMaxLab = new QLabel(QStringLiteral("Max"),this);
    QLabel *pFilterStdLab = new QLabel(QStringLiteral("Std"),this);
    QLabel *pFilterAvgLab = new QLabel(QStringLiteral("Avg"),this);


    m_pFiltShutoffBtn=  new QPushButton(QStringLiteral("Shut Off"),this);
    m_pFiltLockBtn=  new QPushButton(QStringLiteral("Lock"),this);
    m_pFiltHideOutRangeBtn = new QPushButton(QStringLiteral("Hide"),this);
    m_pFiltHideOutRangeBtn->setToolTip(QStringLiteral("Hide the curves of channels which out of range"));
    m_pFiltShutoffBtn->setObjectName(QStringLiteral("ShutOff"));
    m_pFiltLockBtn->setObjectName(QStringLiteral("ShutOffLock"));

    m_pAllVisChk = new QCheckBox(QStringLiteral("All Visible"),this);
    m_pAllVisChk->setChecked(true);

    m_pFiltValidBtn = new QPushButton(QStringLiteral("Transform to valid"),this);
    m_pFiltValidShutOtherBtn = new QPushButton(QStringLiteral("Only valid"),this);
    m_pFiltValidBtn->setToolTip(QStringLiteral("Shut off channel's current out off avgstd range,remain turn to valid"));
    m_pFiltValidShutOtherBtn->setToolTip(QStringLiteral("Only remain valid, other shut off"));

    QLabel *pOpenPoreFiltAvgLab = new QLabel(QStringLiteral("Openpore Avg"),this);
    QLabel *pOpenPoreFiltStdLab = new QLabel(QStringLiteral("Openpore Std"),this);
    m_pOpenPoreFiltChkbox = new QCheckBox(QStringLiteral("Openpore Filt"), this);
    m_pOpenPoreFiltCurMinDpbx =  new QDoubleSpinBox(this);
    m_pOpenPoreFiltCurMaxDpbx =  new QDoubleSpinBox(this);
    m_pOpenPoreFiltStdMinDpbx =  new QDoubleSpinBox(this);
    m_pOpenPoreFiltStdMaxDpbx =  new QDoubleSpinBox(this);

    m_pOpenPoreFiltChkbox->setChecked(false);
    m_pOpenPoreFiltCurMinDpbx->setEnabled(false);
    m_pOpenPoreFiltCurMaxDpbx->setEnabled(false);
    m_pOpenPoreFiltStdMinDpbx->setEnabled(false);
    m_pOpenPoreFiltStdMaxDpbx->setEnabled(false);

    QGroupBox *pAutolimtGrpbx = new QGroupBox(QStringLiteral("Auto Lock bad channel.."),this);
    m_pAutoLockLimtAvgDpbox= new QDoubleSpinBox(this);
    m_pAutoLockLimtStdDpbox= new QDoubleSpinBox(this);
    m_pAutoLockLimtAvgDpbox->setVisible(false);

    m_pOpenPoreFiltCurMinDpbx->setDecimals(2);
    m_pOpenPoreFiltCurMinDpbx->setRange(-10000,10000);
    m_pOpenPoreFiltCurMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pOpenPoreFiltCurMinDpbx->setValue(30);

    m_pOpenPoreFiltCurMaxDpbx->setDecimals(2);
    m_pOpenPoreFiltCurMaxDpbx->setRange(-10000,10000);
    m_pOpenPoreFiltCurMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pOpenPoreFiltCurMaxDpbx->setValue(350);

    m_pOpenPoreFiltStdMinDpbx->setDecimals(2);
    m_pOpenPoreFiltStdMinDpbx->setRange(-10000,10000);
    m_pOpenPoreFiltStdMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pOpenPoreFiltStdMinDpbx->setValue(0.2);

    m_pOpenPoreFiltStdMaxDpbx->setDecimals(2);
    m_pOpenPoreFiltStdMaxDpbx->setRange(-10000,10000);
    m_pOpenPoreFiltStdMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pOpenPoreFiltStdMaxDpbx->setValue(50);


    m_pAutoLockLimtAvgDpbox->setDecimals(2);
    m_pAutoLockLimtAvgDpbox->setSuffix(QStringLiteral(" pA"));
    m_pAutoLockLimtAvgDpbox->setMinimum(-10000);
    m_pAutoLockLimtAvgDpbox->setMaximum(10000);
    m_pAutoLockLimtAvgDpbox->setValue(0.2);

    m_pAutoLockLimtStdDpbox->setDecimals(2);
    m_pAutoLockLimtStdDpbox->setSuffix(QStringLiteral(" pA"));
    m_pAutoLockLimtStdDpbox->setMinimum(-10000);
    m_pAutoLockLimtStdDpbox->setMaximum(10000);
    m_pAutoLockLimtStdDpbox->setValue(0.2);

    m_pFilterCurMinDpbx->setDecimals(2);
    m_pFilterCurMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterCurMinDpbx->setMinimum(-999999.99);
    m_pFilterCurMinDpbx->setMaximum(999999.99);
    m_pFilterCurMinDpbx->setValue(10.0);

    m_pFilterCurMaxDpbx->setDecimals(2);
    m_pFilterCurMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterCurMaxDpbx->setMinimum(-999999.99);
    m_pFilterCurMaxDpbx->setMaximum(999999.99);
    m_pFilterCurMaxDpbx->setValue(1000.0);

    m_pFilterStdMinDpbx->setDecimals(4);
    m_pFilterStdMinDpbx->setMinimum(-999.9999);
    m_pFilterStdMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterStdMinDpbx->setMaximum(999.9999);
    m_pFilterStdMinDpbx->setValue(0.2);

    m_pFilterStdMaxDpbx->setDecimals(4);
    m_pFilterStdMaxDpbx->setMinimum(-999.9999);
    m_pFilterStdMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterStdMaxDpbx->setMaximum(999.9999);
    m_pFilterStdMaxDpbx->setValue(30);

    QGridLayout *filtgridlayout = new QGridLayout();
    filtgridlayout->addWidget(m_pAllVisChk,0,0,1,1);
    filtgridlayout->addWidget(pFiltTypeDescrlab,0,1,1,1);
    filtgridlayout->addWidget(m_pFiltTypeSelectComb,0,2,1,1);
    filtgridlayout->addWidget(m_pCurFiltRowsCntLab,0,3,1,1);
    filtgridlayout->addWidget(m_pAutoRefreshBtn,0,4,1,1);

    QVBoxLayout *grpvertlatout = new QVBoxLayout();
    grpvertlatout->addWidget(m_pCurvesCfgTbleView);
    grpvertlatout->addLayout(filtgridlayout);
    groupbox->setLayout(grpvertlatout);

    QGridLayout *ctlGridlayout = new QGridLayout();
    ctlGridlayout->addWidget(pFilterMinLab,0,1,1,1);
    ctlGridlayout->addWidget(pFilterMaxLab,0,2,1,1);
    ctlGridlayout->addWidget(pFilterAvgLab,1,0,1,1);
    ctlGridlayout->addWidget(m_pFilterCurMinDpbx,1,1,1,1);
    ctlGridlayout->addWidget(m_pFilterCurMaxDpbx,1,2,1,1);
    ctlGridlayout->addWidget(m_pFiltHideOutRangeBtn,1,3,1,1);
    ctlGridlayout->addWidget(pFilterStdLab,2,0,1,1);
    ctlGridlayout->addWidget(m_pFilterStdMinDpbx,2,1,1,1);
    ctlGridlayout->addWidget(m_pFilterStdMaxDpbx,2,2,1,1);
    ctlGridlayout->addWidget(m_pFiltShutoffBtn,2,3,1,1);
    ctlGridlayout->addWidget(m_pFiltLockBtn,2,4,1,1);

    ctlGridlayout->addWidget(pOpenPoreFiltAvgLab,3,0,1,1);
    ctlGridlayout->addWidget(m_pOpenPoreFiltCurMinDpbx,3,1,1,1);
    ctlGridlayout->addWidget(m_pOpenPoreFiltCurMaxDpbx,3,2,1,1);

    ctlGridlayout->addWidget(pOpenPoreFiltStdLab,4,0,1,1);
    ctlGridlayout->addWidget(m_pOpenPoreFiltStdMinDpbx,4,1,1,1);
    ctlGridlayout->addWidget(m_pOpenPoreFiltStdMaxDpbx,4,2,1,1);
    ctlGridlayout->addWidget(m_pOpenPoreFiltChkbox,4,3,1,1);

    ctlGridlayout->addWidget(new QLabel(QStringLiteral("Valid:"),this),5,0,1,1);
    ctlGridlayout->addWidget(m_pFiltValidBtn,5,1,1,1);
    ctlGridlayout->addWidget(m_pFiltValidShutOtherBtn,5,2,1,1);
    ctlGridlayout->setColumnStretch(5,10);
    m_ctrlgroupbox->setLayout(ctlGridlayout);
    m_ctrlgroupbox->setEnabled(false);

    QHBoxLayout *pautolimthorlayout = new QHBoxLayout();
    pautolimthorlayout->addWidget(new QLabel(QStringLiteral("Std Limt <="),this));
    pautolimthorlayout->addWidget(m_pAutoLockLimtStdDpbox);
    pautolimthorlayout->addStretch();
    pAutolimtGrpbx->setLayout(pautolimthorlayout);
    pAutolimtGrpbx->setVisible(false);


    QVBoxLayout *vertlatout = new QVBoxLayout();
    vertlatout->addWidget(groupbox);
    vertlatout->addWidget(m_ctrlgroupbox);
    vertlatout->addWidget(pAutolimtGrpbx);
    setLayout(vertlatout);

    connect(m_pAllVisChk,&QCheckBox::clicked,this,&CusPlotCurvesCfgDlg::AllChannelVisiChkSlot);
    connect(m_pPlotCfgModel,&PLotCfgTabModel::dataChanged,this,&CusPlotCurvesCfgDlg::DataChangedSlot);
    connect(m_pTmrUpdateAvgStd,&QTimer::timeout,this,&CusPlotCurvesCfgDlg::TimeoutGetAllChAvgStd);
    connect(m_pAutoRefreshBtn,&QPushButton::clicked,this,&CusPlotCurvesCfgDlg::OnAutoRefreshAvgStdBtnSlot);

    connect(m_pFiltShutoffBtn,&QPushButton::clicked,this,&CusPlotCurvesCfgDlg::OnFiltApplyBtnSlot);
    connect(m_pFiltLockBtn,&QPushButton::clicked,this,&CusPlotCurvesCfgDlg::OnFiltApplyBtnSlot);
    connect(m_pFiltValidBtn,&QPushButton::clicked,this,&CusPlotCurvesCfgDlg::OnValidApplyBtnSlot);
    connect(m_pFiltValidShutOtherBtn,&QPushButton::clicked,this,&CusPlotCurvesCfgDlg::OnFiltValidShutOtherBtnSlot);

    connect(m_pFiltTypeSelectComb, QOverload<int>::of(&QComboBox::currentIndexChanged),this,&CusPlotCurvesCfgDlg::ChangeFiltTypeSlot);

    connect(m_pFiltHideOutRangeBtn,&QPushButton::clicked,this,&CusPlotCurvesCfgDlg::OnFiltHideOutRangeBtnSlot);
    connect(m_pCurvesCfgTbleView,&QTableView::customContextMenuRequested,this,&CusPlotCurvesCfgDlg::TabViewCreatMenuSlot);
    connect(m_pVisibleOrNotMapper, SIGNAL(mapped (const int &)), this, SLOT(OnChgeVisibleActMapSlot(const int &)));
    connect(m_pOpenPoreFiltChkbox,&QCheckBox::stateChanged,[=](int state){
        bool enable =  (state == Qt::Checked ? true : false);
        m_pOpenPoreFiltCurMinDpbx->setEnabled(enable);
        m_pOpenPoreFiltCurMaxDpbx->setEnabled(enable);
        m_pOpenPoreFiltStdMinDpbx->setEnabled(enable);
        m_pOpenPoreFiltStdMaxDpbx->setEnabled(enable);
    });


}

void CusPlotCurvesCfgDlg::StartAcquireAvgStdTimer(const int mils)
{
    if(!m_pTmrUpdateAvgStd->isActive())
    {
//        m_pTmrUpdateAvgStd->start(mils);
        //为了获取1秒后的Std/Avg，刚开启时Avg/std默认值有可能为0
        QTimer::singleShot(2000, this,[&,mils](){m_pTmrUpdateAvgStd->start(mils);});
    }

}

void CusPlotCurvesCfgDlg::StopAcquireAvgStdTimer(void)
{
    if(m_pTmrUpdateAvgStd->isActive())
    {
        m_pTmrUpdateAvgStd->stop();
    }
}

void CusPlotCurvesCfgDlg::DataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if(topLeft == bottomRight)
    {
        QModelIndex index = topLeft;
        if(index.column() == PLotCfgTabModel::Visible)
        {
            emit CurveVisibleChgeSig(index.row(),index.data(Qt::EditRole).toBool());
        }
        else if(index.column() == PLotCfgTabModel::Color)
        {
             emit CurveColorChgeSig(index.row(),index.data(Qt::EditRole).value<QColor>());
        }
    }
}

void CusPlotCurvesCfgDlg::AllChannelVisiChkSlot(const bool &chk)
{
    m_pPlotCfgModel->UpdateAllChVisible(chk);
    emit AllCurvesVisibleSig(chk);
}

void CusPlotCurvesCfgDlg::UpdateVisiModelData(const std::vector<bool> &vetIsVisible)
{
    m_pPlotCfgModel->UpdateAllChVisible(vetIsVisible);
}

void CusPlotCurvesCfgDlg::TimeoutGetAllChAvgStd(void)
{
    bool isStreamRunning =  ConfigServer::GetInstance()->GetIsStreamRunning();
    if(!isStreamRunning)
        return;
    auto &vetAvg = m_pPlotCfgModel->GetAllChAvgRef();
    auto &vetStd = m_pPlotCfgModel->GetAllChStdRef();

    emit GetAllChAvgSig(vetAvg);
    emit GetAllChStdSig(vetStd);

    //自动lock std<=0.2 avg <= 0.2的通道
    double limtAvg = m_pAutoLockLimtAvgDpbox->value();
    double limtStd = m_pAutoLockLimtStdDpbox->value();
    OnLockUnqualifiedChBySTD(limtAvg,limtStd);

    if(isVisible()) //如果打开了对话框，则开始刷新
    {
        OnFiltChanByAvgStdInTab(vetAvg,vetStd);
        m_pPlotCfgModel->UpdateAllChDataDisplayByColumn(PLotCfgTabModel::AVG,PLotCfgTabModel::STD,false); //更新列表显示
//        m_pCurvesCfgTbleView->update();
        m_pCurFiltRowsCntLab->setText(QString("Count: %1").arg(m_pSortFilterProxyModel->rowCount()));
    }
}



void CusPlotCurvesCfgDlg::OnAutoRefreshAvgStdBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pAutoRefreshBtn->setText(QStringLiteral("Stop refresh"));
        m_pCurvesCfgTbleView->setSortingEnabled(false);
        StartAcquireAvgStdTimer(1000);
    }
    else
    {
        m_pAutoRefreshBtn->setText(QStringLiteral("AutoRefresh"));
        StopAcquireAvgStdTimer();
        m_pCurvesCfgTbleView->setSortingEnabled(true);
    }
}

void CusPlotCurvesCfgDlg::EnableAvgStdAutoRefresh(const bool &enable)
{
    if(enable)
    {
        if(!m_pAutoRefreshBtn->isChecked())
        {
            m_pAutoRefreshBtn->click();
        }
    }
    else
    {
        if(m_pAutoRefreshBtn->isChecked())
        {
            m_pAutoRefreshBtn->click();
        }
    }

}
void CusPlotCurvesCfgDlg::OnValidApplyBtnSlot(void)
{
    const auto &vctbFitAvgStd = m_pSortFilterProxyModel->GetAllChAvgStdFitRef();
    auto &vctAllChFourValid = m_pModelSharedDataPtr->GetAllChFourValidStateRef();
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();

    for(int ch  =0; ch < CHANNEL_NUM; ++ch)
    {
        const auto &chSensorGrp = vctAllChSensorGrp[ch];
        auto &vctValidState = vctAllChFourValid[chSensorGrp];
        vctValidState[ch] = vctbFitAvgStd[ch] ?  VALID_CHANNEL_ENUM : INVALID_CHANNEL_ENUM;
    }

    if(isVisible())
    {
    m_pPlotCfgModel->UpdateAllChDataDisplayByColumn(PLotCfgTabModel::Valid); //更新列表显示
    }
#ifdef EASY_PROGRAM_DEBUGGING
    std::transform(vctbFitAvgStd.cbegin(),vctbFitAvgStd.cend(),vctAllchFuncState.begin(),[](const bool &bIsFit){ return (bIsFit ? SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE :  SENSORGRP_FUNC_STATE_OFF); });
#else
    std::transform(vctbFitAvgStd.cbegin(),vctbFitAvgStd.cend(),vctAllchFuncState.begin(),[](const bool &bIsFit){ return (bIsFit ? SENSORGRP_FUNC_STATE_NORMAL :  SENSORGRP_FUNC_STATE_OFF); });
#endif
    m_pSensorPanel->AllChDifferFuncStateSetSlot(vctAllchFuncState);
    emit UpdateCountValidStateCntSig();

}

void CusPlotCurvesCfgDlg::OnFiltValidShutOtherBtnSlot(void)
{
    auto &vctAllChFourValid = m_pModelSharedDataPtr->GetAllChFourValidStateRef();
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();

    for(int ch  =0; ch < CHANNEL_NUM; ++ch)
    {
        const auto &chSensorGrp = vctAllChSensorGrp[ch];
        auto &validState = vctAllChFourValid[chSensorGrp][ch];
        if(validState == VALID_CHANNEL_ENUM)
        {
#ifdef EASY_PROGRAM_DEBUGGING
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
#else
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_NORMAL;
#endif
        }
        else
        {
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
        }
    }
    m_pSensorPanel->AllChDifferFuncStateSetSlot(vctAllchFuncState);
}

void CusPlotCurvesCfgDlg::OnFiltApplyBtnSlot(void)
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if(!btn)
    {
        return;
    }
    bool bNeedToLock = false;
    if(btn->objectName() == QStringLiteral("ShutOffLock"))
    {
        bNeedToLock = true;
    }

    const auto &vctbFitAvgStd = m_pSortFilterProxyModel->GetAllChAvgStdFitRef();
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();

    for(int ch  =0  ; ch < CHANNEL_NUM; ++ch)
    {
        if(vctbFitAvgStd[ch] == FILTER_AVG_STD_UNFIT)
        {
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
            if(bNeedToLock)
            {
                const auto &chSensorGrp = vctAllChSensorGrp[ch];
                vctAllChFourLockState[chSensorGrp][ch] = PROHIBITED_OPERATION_STATE;
            }
        }
    }
    m_pSensorPanel->AllChDifferFuncStateSetSlot(vctAllchFuncState);
}



void CusPlotCurvesCfgDlg::OnLockUnqualifiedChBySTD(const double &limtAvg,const double &limtStd)
{
    Q_UNUSED(limtAvg)
    const auto &vctStd = m_pPlotCfgModel->GetAllChStdRef();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    int cnt =0;
    for(int ch = 0; ch <CHANNEL_NUM; ++ch)
    {
        if(vctStd[ch] < limtStd)
        {
            const auto &chSensorGrp = vctAllChSensorGrp[ch];
            auto &chLockState = vctAllChFourLockState[chSensorGrp][ch];
            if(chLockState == ALLOW_OPERATION_STATE)
            {
                chLockState = PROHIBITED_OPERATION_STATE;
                vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
                cnt++;
            }
        }
    }
    if(cnt > 0)
    {
        m_pSensorPanel->AllChDifferFuncStateSetSlot(vctAllchFuncState);
    }
}

void CusPlotCurvesCfgDlg::OnLockChGreaterAvgOrStdLimit(const float &avgLimt,const float &stdLimt)
{
    const auto &vetAvg = m_pPlotCfgModel->GetAllChAvgRef();
    const auto &vetStd = m_pPlotCfgModel->GetAllChStdRef();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    int cnt =0;
    for(int ch = 0; ch <CHANNEL_NUM; ++ch)
    {
        const auto &curAvg = vetAvg[ch];
        const auto &curStd = vetStd[ch];
        if(curAvg > avgLimt  || curStd > stdLimt)
        {
            const auto &chSensorGrp = vctAllChSensorGrp[ch];
            auto &chLockState = vctAllChFourLockState[chSensorGrp][ch];
            if(chLockState == ALLOW_OPERATION_STATE)
            {
                chLockState = PROHIBITED_OPERATION_STATE;
                vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
                cnt++;
            }
        }
    }
    if(cnt > 0)
    {
        m_pSensorPanel->AllChDifferFuncStateSetSlot(vctAllchFuncState);
    }
}

void CusPlotCurvesCfgDlg::OnFiltChanByAvgStdInTab(const std::vector<float> &vctAvg,const std::vector<float> &vctStd)
{
    const double avgMin = m_pFilterCurMinDpbx->value();
    const double avgMax = m_pFilterCurMaxDpbx->value();
    const double stdMin = m_pFilterStdMinDpbx->value();
    const double stdMax = m_pFilterStdMaxDpbx->value();

    bool isOpenPoreFilt = m_pOpenPoreFiltChkbox->isChecked();
    const double openPoreAvgMin = m_pOpenPoreFiltCurMinDpbx->value();
    const double openPoreAvgMax = m_pOpenPoreFiltCurMaxDpbx->value();
    const double openPoreStdMin = m_pOpenPoreFiltStdMinDpbx->value();
    const double openPorestdMax = m_pOpenPoreFiltStdMaxDpbx->value();

    auto &vctbFitAvgStd = m_pSortFilterProxyModel->GetAllChAvgStdFitRef();

    if(isOpenPoreFilt)
    {
        for(int ch = 0; ch < CHANNEL_NUM; ++ch)
        {
            const auto &curAvg = vctAvg[ch];
            const auto &curStd = vctStd[ch];

            if((curAvg >= avgMin && curAvg <= avgMax  &&  curStd >= stdMin && curStd <= stdMax) \
                    || (curAvg >= openPoreAvgMin && curAvg <= openPoreAvgMax  &&  curStd >= openPoreStdMin && curStd <= openPorestdMax))
            {
                vctbFitAvgStd[ch] = FILTER_AVG_STD_FIT;
            }
            else
            {
                vctbFitAvgStd[ch] = FILTER_AVG_STD_UNFIT;
            }

        }
    }
    else
    {
        for(int ch = 0; ch < CHANNEL_NUM; ++ch)
        {
            const auto &curAvg = vctAvg[ch];
            const auto &curStd = vctStd[ch];

            if(curAvg >= avgMin && curAvg <= avgMax  &&  curStd >= stdMin && curStd <= stdMax)
            {
                vctbFitAvgStd[ch] = FILTER_AVG_STD_FIT;
            }
            else
            {
                vctbFitAvgStd[ch] = FILTER_AVG_STD_UNFIT;
            }
        }

    }
}

void CusPlotCurvesCfgDlg::OnFiltHideOutRangeBtnSlot(void)
{
    const auto &vctbFitAvgStd = m_pSortFilterProxyModel->GetAllChAvgStdFitRef();
    auto &vctVisible = m_pPlotCfgModel->GetAllChVisibleRef();

    std::transform(vctbFitAvgStd.cbegin(),vctbFitAvgStd.cend(),vctVisible.begin(),[](const bool &state){ return state;});
    m_pCustomPlotWidgt->UpdateAllChVisibleView(vctVisible);
}

void CusPlotCurvesCfgDlg::TabViewCreatMenuSlot(const QPoint & p)
{
    QModelIndex index = m_pCurvesCfgTbleView->indexAt(p);//获取鼠标点击位置项的索引
    if(index.isValid())//数据项是否有效，空白处点击无菜单s
    {
        m_pCfgMenu->exec(QCursor::pos());
    }
}

void CusPlotCurvesCfgDlg::OnChgeVisibleActMapSlot(const int & actionId)
{
    switch (actionId) {
    case ALL_CHAN_INVIVISBLE_TRIGER_ENUM:
    {
        auto &vctVisible = m_pPlotCfgModel->GetAllChVisibleRef();
        for(auto state : vctVisible)
        {
            state = false;
        }
        m_pCustomPlotWidgt->UpdateAllChVisibleView(vctVisible);
        return;
    }
    case ALL_CHAN_VIVISBLE_TRIGER_ENUM:
    {
        auto &vctVisible = m_pPlotCfgModel->GetAllChVisibleRef();
        for(auto state : vctVisible)
        {
            state = true;
        }
        m_pCustomPlotWidgt->UpdateAllChVisibleView(vctVisible);
        return;
    }
    case AVGSTD_VIVISBLE_TRIGER_ENUM:
    {
        const auto &vctbFitAvgStd = m_pSortFilterProxyModel->GetAllChAvgStdFitRef();
        auto &vctVisible = m_pPlotCfgModel->GetAllChVisibleRef();
        std::transform(vctbFitAvgStd.cbegin(),vctbFitAvgStd.cend(),vctVisible.begin(),[](const bool &state){ return state;});
        m_pCustomPlotWidgt->UpdateAllChVisibleView(vctVisible);
        return;
    }
    case SEQ_VIVISBLE_TRIGER_ENUM:
    {
        const auto &vctSeqsta = m_pPlotCfgModel->GetAllChSeqSteRef();
        auto &vctVisible = m_pPlotCfgModel->GetAllChVisibleRef();
        std::transform(vctSeqsta.cbegin(),vctSeqsta.cend(),vctVisible.begin(),[](const bool &state){ return state;});
        m_pCustomPlotWidgt->UpdateAllChVisibleView(vctVisible);
        return;
    }
    case VALID_VIVISBLE_TRIGER_ENUM:
    {
        auto &vctVisible = m_pPlotCfgModel->GetAllChVisibleRef();
        for(int ch = 0; ch < CHANNEL_NUM; ++ch)
        {
            vctVisible[ch] = m_pModelSharedDataPtr->GetCurChanValidState(ch);
        }
        m_pCustomPlotWidgt->UpdateAllChVisibleView(vctVisible);
        return;
    }
    default:
        break;
    }

}
void CusPlotCurvesCfgDlg::UpdateAvgStdParameterSlot(const float &avgMin,const float &avgMax,const float &stdMin,const float &stdMax)
{
    m_pFilterCurMinDpbx->setValue(avgMin);
    m_pFilterCurMaxDpbx->setValue(avgMax);
    m_pFilterStdMinDpbx->setValue(stdMin);
    m_pFilterStdMaxDpbx->setValue(stdMax);
}


void CusPlotCurvesCfgDlg::SetCurFiltType(const int &type)
{
    m_pFiltTypeSelectComb->setCurrentIndex(type);
}


void CusPlotCurvesCfgDlg::ChangeFiltTypeSlot(const int &type)
{
    bool ctrlEnable = (type == PlotCfgSortFilterModel::FILTER_TYPE_AVG_STD || type == PlotCfgSortFilterModel::FILTER_TYPE_VALID);
    m_ctrlgroupbox->setEnabled(ctrlEnable);

    PlotCfgSortFilterModel::FILTER_TYPE_ENUM mode = PlotCfgSortFilterModel::FILTER_TYPE_ENUM (type);
    m_pSortFilterProxyModel->ChangeFilterType(mode);
    m_pPlotCfgModel->UpdateAllChDataDisplayByColumn(PLotCfgTabModel::Channel,PLotCfgTabModel::Valid,true);
}

int CusPlotCurvesCfgDlg::GetFinalMuxCount(void)
{
    return m_pSortFilterProxyModel->rowCount();
}



