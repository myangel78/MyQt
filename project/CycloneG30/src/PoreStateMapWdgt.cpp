#include "PoreStateMapWdgt.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QAction>
#include <QCheckBox>
#include <QSpinBox>
#include <QPainter>
#include <QColorDialog>
#include <QSignalMapper>
#include <QHeaderView>
#include <QTableView>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QTextStream>
#include <QProgressDialog>
#include <QApplication>
#include "Global.h"
#include "Log.h"
#include "UsbFtdDevice.h"
#include "PoreStateMapTabModel.h"
#include "CapCaculate.h"
#include "SensorPanel.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QLCDNumber>

#include <random>

#include <future>
#include <QTabWidget>


PoreStateMapWdgt::PoreStateMapWdgt(QWidget *parent):QWidget(parent)
{
    m_vctCapStateCnt.resize(CAP_STATE_NUM);
    std::fill_n(m_vctCapStateCnt.begin(),CAP_STATE_NUM,0);

    m_vctSensorStateCnt.resize(SENSOR_STATE_NUM);
    std::fill_n(m_vctSensorStateCnt.begin(),SENSOR_STATE_NUM,0);

    m_vctOVFStateCnt.resize(OVF_STATE_NUM);
    std::fill_n(m_vctOVFStateCnt.begin(),OVF_STATE_NUM,0);

    m_vctMuxStateCnt.resize(MUX_STATE_NUM);
    std::fill_n(m_vctMuxStateCnt.begin(),MUX_STATE_NUM,0);

    m_vctOVFCountToLock.resize(CHANNEL_NUM);
    std::fill_n(m_vctOVFCountToLock.begin(),CHANNEL_NUM,0);

    m_vctPoreStateCnt.resize(SENSOR_GROUP_NUM);
    for(auto &vctStateGrp : m_vctPoreStateCnt)
    {
        vctStateGrp.resize(PORE_STATE_NUM);
        std::fill_n(vctStateGrp.begin(),PORE_STATE_NUM,0);
    }

    m_vctLockStateCnt.resize(SENSOR_GROUP_NUM);
    for(auto &vctStateGrp : m_vctLockStateCnt)
    {
        vctStateGrp.resize(LOCK_OPERATE_STATE_NUM);
        std::fill_n(vctStateGrp.begin(),LOCK_OPERATE_STATE_NUM,0);
    }

    m_vctValidStateCnt.resize(SENSOR_GROUP_NUM);
    for(auto &vctStateGrp : m_vctValidStateCnt)
    {
        vctStateGrp.resize(VALID_STATE_NUM);
        std::fill_n(vctStateGrp.begin(),VALID_STATE_NUM,0);
    }
    m_vctPoreStatePrev.resize(CHANNEL_NUM);
    std::fill_n(m_vctPoreStatePrev.begin(),CHANNEL_NUM,NONE_PORE_STATE);

    InitCtrl();
}

PoreStateMapWdgt::~PoreStateMapWdgt()
{
    delete m_pPoreStateMapTabModel;
}


void PoreStateMapWdgt::InitCtrl(void)
{
    QGroupBox *groupbox = new QGroupBox(QStringLiteral("Channel state mapping"),this);
    QGroupBox *groupModebox = new QGroupBox(QStringLiteral("Display mode"),this);

    m_pProgressDlg = new QProgressDialog(QString("Begin Scanning ovf !!!"), "Abort scanning", 0,100 , this);
    m_pProgressDlg->setWindowModality(Qt::ApplicationModal);
//    m_pProgressDlg->setWindowModality(Qt::WindowModal);
    m_pProgressDlg->setModal(true);
    m_pProgressDlg->reset();
    m_pProgressDlg->hide();

    InitSensorStateCtrl();
    InitPoreStateCtrl();
    InitCapStateCtrl();
    InitOVFStateCtrl();
    InitLockOperateStateCtrl();
    InitValidStateCtrl();
    InitMuxStateCtrl();


    m_pPoreStateMapTabView = new QTableView(this);
    m_pPoreStateMapTabModel = new PoreStateMapTabModel(this);
    m_pPoreStateMapTabView->setModel(m_pPoreStateMapTabModel);

    m_pPoreStateMapTabView->horizontalHeader()->setVisible(false);
    m_pPoreStateMapTabView->verticalHeader()->setVisible(false);


    m_pPoreStateMapTabView->horizontalHeader()->setMinimumSectionSize(10);
    m_pPoreStateMapTabView->verticalHeader()->setMinimumSectionSize(10);
//    m_pPoreStateMapTabView->horizontalHeader()->setMaximumSectionSize(10);
//    m_pPoreStateMapTabView->verticalHeader()->setMaximumSectionSize(10);
    m_pPoreStateMapTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pPoreStateMapTabView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    m_pPoreStateMapTabView->setMaximumSize(800,800);

    QLabel *labDisplyContent = new QLabel(QStringLiteral("Content:"),this);
    m_pDispContentComb =new QComboBox(this);
    m_pDispContentComb->addItem(QStringLiteral("Pure"),NO_CONTENT_DISPLAY_MODE);
    m_pDispContentComb->addItem(QStringLiteral("Channel num"),CHANNEL_NUM_DISPLAY_MODE);
    m_pDispContentComb->addItem(QStringLiteral("Cap value"),CAP_VALUE_DISPLAY_MODE);
//    m_pDispContentComb->addItem(QStringLiteral("State value"),STATE_VALUE_DISPLAY_MODE);

    m_pIsResizeToContent = new QCheckBox(QStringLiteral("ResizeToContents"));

    m_pStateTabWidget = new QTabWidget(this);
    m_pStateTabWidget->insertTab(SENSOR_STATE_MAP_MODE,m_pGrpSensorbox,QStringLiteral("Sensor"));
    m_pStateTabWidget->insertTab(OVF_STATE_MAP_MODE,m_pGrpOVFbox,QStringLiteral("OVF"));
    m_pStateTabWidget->insertTab(CAP_STATE_MAP_MODE,m_pGrpCapbox,QStringLiteral("Cap"));
    m_pStateTabWidget->insertTab(PORE_STATE_MAP_MODE,m_pGrpPorebox,QStringLiteral("Pore"));
    m_pStateTabWidget->insertTab(LOCK_SENSOR_STATE_MAP_MODE,m_pGrpLockbox,QStringLiteral("Lock"));
    m_pStateTabWidget->insertTab(VALID_SENSOR_STATE_MAP_MODE,m_pGrpValidbox,QStringLiteral("Valid"));
    m_pStateTabWidget->insertTab(MUX_STATE_MAP_MODE,m_pGrpMuxbox,QStringLiteral("Mux"));
    m_pStateTabWidget->setTabPosition(QTabWidget::East);
    m_pStateTabWidget->setDocumentMode(true);

    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->addWidget(labDisplyContent,0,0,1,1);
    gridlayout->addWidget(m_pDispContentComb,0,1,1,1);
    gridlayout->addWidget(m_pIsResizeToContent,1,0,1,2);
    groupModebox->setLayout(gridlayout);

    QVBoxLayout *vertlayout2 = new QVBoxLayout();
    vertlayout2->addWidget(groupModebox);
    vertlayout2->addWidget(m_pStateTabWidget);
    vertlayout2->setStretch(0,1);
    vertlayout2->setStretch(1,10);

    QHBoxLayout *horilayout = new QHBoxLayout();
    horilayout->addWidget(m_pPoreStateMapTabView);
    groupbox->setLayout(horilayout);

    QHBoxLayout *horilayout1 = new QHBoxLayout();
    horilayout1->addWidget(groupbox);
    horilayout1->addLayout(vertlayout2);
    horilayout1->setStretch(0,10);
    horilayout1->setStretch(1,1);
    setLayout(horilayout1);


    connect(m_pStateTabWidget,&QTabWidget::currentChanged,this,&PoreStateMapWdgt::OnStateModeChangeSlot);
    connect(m_pDispContentComb, QOverload<int>::of(&QComboBox::currentIndexChanged),this,&PoreStateMapWdgt::OnDisplayContentModeChgeSlot);
    connect(m_pIsResizeToContent,&QCheckBox::toggled,this,&PoreStateMapWdgt::OnReizeCotentToViewSlot);

//#ifndef EASY_PROGRAM_DEBUGGING
//    m_pDispContentComb->setCurrentIndex(CHANNEL_NUM_DISPLAY_MODE);
//#endif
}

void PoreStateMapWdgt::InitSensorStateCtrl(void)
{
    m_pGrpSensorbox = new QGroupBox(QStringLiteral("Sensor state"),this);
    m_pSensorManualRefreshBtn = new QPushButton(QStringLiteral("Refresh"),this);
    m_pSensorClrBtnMapper = new QSignalMapper(this);

    QScrollArea *scrollarea = new QScrollArea(m_pGrpSensorbox);
    QGridLayout *gridlayout = new QGridLayout();
    m_SensorStateNum = SENSOR_STATE_NUM;
    for(int i = 0; i < m_SensorStateNum; ++i)
    {
        m_pbtnSensorState[i] = new QToolButton(scrollarea);
        auto pbtn = m_pbtnSensorState[i];
        QAction* act = new QAction(scrollarea);
        act->setToolTip(tr("Change color"));
        pbtn->setDefaultAction(act);
        m_clrSensorState[i] = gSensorStateColr[i];
        SetBtnColor(pbtn, m_clrSensorState[i]);
        connect(pbtn,SIGNAL(pressed()),m_pSensorClrBtnMapper,SLOT(map()));
        m_pSensorClrBtnMapper->setMapping(pbtn,i);

        QLabel *plabStateName = new QLabel(QString::fromStdString(gSensorStateStr[i]),scrollarea);
        m_plabSensorStateCnt[i] = new QLabel(QString("%1").arg(QString::number(i)),this);
        gridlayout->addWidget(pbtn,i,0,1,1);
        gridlayout->addWidget(plabStateName,i,1,1,1);
        gridlayout->addWidget(m_plabSensorStateCnt[i],i,2,1,1);
    }
    scrollarea->setLayout(gridlayout);

    QHBoxLayout *horilayout = new QHBoxLayout();
    horilayout->addWidget(m_pSensorManualRefreshBtn);

    QVBoxLayout *vertlayout =new QVBoxLayout();
    vertlayout->addLayout(horilayout);
    vertlayout->addWidget(scrollarea);
    m_pGrpSensorbox->setLayout(vertlayout);

    connect(m_pSensorClrBtnMapper, SIGNAL(mapped (const int &)), this, SLOT(OnClickChangeColor(const int &)));
    connect(m_pSensorManualRefreshBtn,&QPushButton::clicked,this,&PoreStateMapWdgt::OnSensorStateRefreshBtnSlot);
}



void PoreStateMapWdgt::InitCapStateCtrl(void)
{
    m_pGrpCapbox = new QGroupBox(QStringLiteral("Cap state"),this);
    m_pCapClrBtnMapper = new QSignalMapper(this);

    m_pbtnCapSateRest =new QPushButton(QStringLiteral("Clear"),this);

    QGridLayout *gridlayout = new QGridLayout();
    m_CapStateNum = CAP_STATE_NUM;
    for(int i = 0; i < m_CapStateNum; ++i)
    {
        m_pbtnCapState[i] = new QToolButton(m_pGrpCapbox);
        auto pbtn = m_pbtnCapState[i];
        QAction* act = new QAction(m_pGrpCapbox);
        act->setToolTip(tr("Change color"));
        pbtn->setDefaultAction(act);
        m_clrCapState[i] = gCapStateColr[i];
        SetBtnColor(pbtn, m_clrCapState[i]);
        connect(pbtn,SIGNAL(pressed()),m_pCapClrBtnMapper,SLOT(map()));
        m_pCapClrBtnMapper->setMapping(pbtn,i);

        QLabel *plabStateName = new QLabel(QString::fromStdString(gCapStateStr[i]),m_pGrpCapbox);
        m_plabCapStateCnt[i] = new QLabel(QString("%1").arg(QString::number(i)),this);
        gridlayout->addWidget(pbtn,i,0,1,1);
        gridlayout->addWidget(plabStateName,i,1,1,1);
        gridlayout->addWidget(m_plabCapStateCnt[i],i,2,1,1);

    }

    for(int i = 0; i< m_CapStateNum; ++i)
    {
        m_plabCapLevelDescr[i] = new QLabel(QString("Level%1%2").arg(i+1).arg(i == CAP_LEVEL5_STATE ? ">" : "<"),this);
        m_pCaplevelLmtDpbx[i] = new QDoubleSpinBox(this);
        m_pCaplevelLmtDpbx[i]->setRange(0.0,2000);
        m_pCaplevelLmtDpbx[i]->setSuffix(QStringLiteral(" pF"));
    }

    m_capLevelLmit[CAP_LEVEL1_STATE] = 10;
    m_capLevelLmit[CAP_LEVEL2_STATE] = 40;
    m_capLevelLmit[CAP_LEVEL3_STATE] = 200;
    m_capLevelLmit[CAP_LEVEL4_STATE] = 300;
    m_capLevelLmit[CAP_LEVEL5_STATE] = 300;

    m_pCaplevelLmtDpbx[CAP_LEVEL1_STATE]->setValue(10);
    m_pCaplevelLmtDpbx[CAP_LEVEL2_STATE]->setValue(40);
    m_pCaplevelLmtDpbx[CAP_LEVEL3_STATE]->setValue(200);
    m_pCaplevelLmtDpbx[CAP_LEVEL4_STATE]->setValue(300);
    m_pCaplevelLmtDpbx[CAP_LEVEL5_STATE]->setValue(300);
    m_pCaplevelLmtDpbx[CAP_LEVEL5_STATE]->setEnabled(false);

    m_pCapCalStartBtn = new QPushButton(QIcon(":/img/img/start.ico"),"Start",this);
    m_pCapCalStartBtn->setCheckable(true);

    m_pbtnCapSave = new QPushButton("Save as...", this);
    int rowIdx = 5;
    for(int i = 0; i< m_CapStateNum; ++i)
    {
         gridlayout->addWidget(m_plabCapLevelDescr[i],rowIdx,0,1,1);
         gridlayout->addWidget(m_pCaplevelLmtDpbx[i],rowIdx++,1,1,1);
    }

    gridlayout->addWidget(m_pbtnCapSave,rowIdx,0,1,1);
    gridlayout->addWidget(m_pCapCalStartBtn,rowIdx++,1,1,1);
    gridlayout->setRowStretch(rowIdx++,10);
    gridlayout->addWidget(m_pbtnCapSateRest,rowIdx,1,1,1);
    m_pGrpCapbox->setLayout(gridlayout);

    connect(m_pCapClrBtnMapper, SIGNAL(mapped (const int &)), this, SLOT(OnClickChangeCapStaeColor(const int &)));
    connect(m_pCaplevelLmtDpbx[CAP_LEVEL4_STATE], QOverload<double>::of(&QDoubleSpinBox::valueChanged),m_pCaplevelLmtDpbx[CAP_LEVEL5_STATE],&QDoubleSpinBox::setValue);
    connect(m_pCapCalStartBtn,&QPushButton::clicked,this,&PoreStateMapWdgt::CapCalStartBtnSlot);
    connect(m_pbtnCapSave, &QPushButton::clicked, this, &PoreStateMapWdgt::OnClickCapSaveas);
    connect(m_pbtnCapSateRest,&QPushButton::clicked,[&]()
    {
        m_pPoreStateMapTabModel->RestState(CAP_STATE_MAP_MODE,CAP_LEVEL1_STATE);
        for (auto &cnt : m_vctCapStateCnt) {  cnt = 0;}
        for(int i = 0; i <m_vctCapStateCnt.size();++i )
        {
            m_plabCapStateCnt[i]->setText(QString::number(m_vctCapStateCnt[i]));
        }
    });
}


void PoreStateMapWdgt::InitOVFStateCtrl(void)
{
    m_ovfScanTimes = 2;
    m_pGrpOVFbox = new QGroupBox(QStringLiteral("OVF state"),this);
    m_pOVFClrBtnMapper = new QSignalMapper(this);

    m_pbtnOVFRefresh =new QPushButton(QStringLiteral("Refresh"),this);
    m_pbtnOVFShutoff =new QPushButton(QStringLiteral("Shut off"),this);
    m_pbtnOVFSateRest =new QPushButton(QStringLiteral("Clear"),this);
    m_pbtnOVFLock =new QPushButton(QStringLiteral("Lock"),this);
    m_pbtnOVFShutoff->setToolTip(QStringLiteral("Shut off current over flow channel"));

    m_pTimerToAutoOvfScan = new QTimer(this);
    m_pTimerOvfScanInptDpbx= new QDoubleSpinBox(this);
    m_pTimerAutoOvfSScanBtn = new QPushButton(QStringLiteral("AutoScan"),this);
    m_pTimerAutoOvfSScanBtn->setCheckable(true);
    m_pTimerOvfScanInptDpbx->setRange(0.1,360);
    m_pTimerOvfScanInptDpbx->setValue(30);
    m_pTimerOvfScanInptDpbx->setSuffix(QStringLiteral(" min"));
    m_plcdTimerOvfScanPeriod = new QLCDNumber(8, this);
    m_plcdTimerOvfScanPeriod->setSegmentStyle(QLCDNumber::Flat);
    m_plcdTimerOvfScanPeriod->setFrameShape(QFrame::StyledPanel);
    SetOVFTimehms();
    QGridLayout *gridlayout = new QGridLayout();
    m_OVFStateNum = OVF_STATE_NUM;
    for(int i = 0; i < m_OVFStateNum; ++i)
    {
        m_pbtnOVFState[i] = new QToolButton(m_pGrpOVFbox);
        auto pbtn = m_pbtnOVFState[i];
        QAction* act = new QAction(m_pGrpOVFbox);
        act->setToolTip(tr("Change color"));
        pbtn->setDefaultAction(act);
        m_clrOVFState[i] = gOVFStateColr[i];
        SetBtnColor(pbtn, m_clrOVFState[i]);
        connect(pbtn,SIGNAL(pressed()),m_pOVFClrBtnMapper,SLOT(map()));
        m_pOVFClrBtnMapper->setMapping(pbtn,i);

        QLabel *plabStateName = new QLabel(QString::fromStdString(gOVFStateStr[i]),m_pGrpOVFbox);
        m_plabOVFStateCnt[i] = new QLabel(QString("%1").arg(QString::number(i)),this);
        gridlayout->addWidget(pbtn,i,0,1,1);
        gridlayout->addWidget(plabStateName,i,1,1,1);
        gridlayout->addWidget(m_plabOVFStateCnt[i],i,2,1,1);
    }
    gridlayout->addWidget(m_pbtnOVFRefresh,m_OVFStateNum,1,1,1);
    gridlayout->addWidget(m_pbtnOVFShutoff,m_OVFStateNum+1,1,1,1);
    gridlayout->addWidget(m_pbtnOVFLock,m_OVFStateNum+2,1,1,1);
    gridlayout->setRowStretch(m_OVFStateNum+3,10);
    gridlayout->addWidget(m_pTimerAutoOvfSScanBtn,m_OVFStateNum+4,1,1,1);
    gridlayout->addWidget(m_pTimerOvfScanInptDpbx,m_OVFStateNum+5,1,1,1);
    gridlayout->addWidget(m_plcdTimerOvfScanPeriod,m_OVFStateNum+6,1,1,1);
    gridlayout->setRowStretch(m_OVFStateNum+7,10);
    gridlayout->addWidget(m_pbtnOVFSateRest,m_OVFStateNum+8,1,1,1);
    m_pGrpOVFbox->setLayout(gridlayout);

    connect(m_pOVFClrBtnMapper, SIGNAL(mapped (const int &)), this, SLOT(OnClickChangeOVFStaeColor(const int &)));
    connect(m_pbtnOVFSateRest,&QPushButton::clicked,[&](){
        m_pPoreStateMapTabModel->RestState(OVF_STATE_MAP_MODE,UN_OVER_FLOW_FLAG);
        for (auto &cnt : m_vctOVFStateCnt) {  cnt = 0;}
        for(int i = 0; i <OVF_STATE_NUM;++i )
        {
            m_plabOVFStateCnt[i]->setText(QString::number(m_vctOVFStateCnt[i]));
        }});

    connect(m_pbtnOVFRefresh,&QPushButton::clicked,this,&PoreStateMapWdgt::OnOVFStateRefreshBtnSlot);
    connect(m_pbtnOVFShutoff,&QPushButton::clicked,this,&PoreStateMapWdgt::OnOVFStateShutOffBtnSlot);
    connect(m_pbtnOVFLock,&QPushButton::clicked,this,&PoreStateMapWdgt::OnOVFStateLockOVFBtnSlot);
    connect(m_pTimerAutoOvfSScanBtn,&QPushButton::toggled,this,&PoreStateMapWdgt::OnAutoOvfScanBtnSlot);
    connect(m_pTimerToAutoOvfScan,&QTimer::timeout,this,&PoreStateMapWdgt::OnTimeUpToAutoOvfScanSlot);

}

void PoreStateMapWdgt::InitPoreStateCtrl(void)
{
    m_pGrpPorebox = new QGroupBox(QStringLiteral("Pore state"),this);
    m_pPoreClrBtnMapper = new QSignalMapper(this);

    m_pbtnPoreSateRest =new QPushButton(QStringLiteral("Clear"),this);
    m_pbtnPoreSateCount =new QPushButton(QStringLiteral("Count"),this);
    m_PoreStateNum = PORE_STATE_NUM;

    m_pPoreTabWidgt = new QTabWidget(this);
    QVBoxLayout *vertlayout = new QVBoxLayout();

    for(int senGrp = 0; senGrp < SENSOR_GROUP_NUM; ++senGrp)
    {
        QGridLayout *gridGrp = new QGridLayout();

        QLabel *pDescrlabel = new QLabel(QString("Sensor%1").arg(SENSRO_STR[senGrp]),this);
        int grpRowIndx = 0;
        gridGrp->addWidget(pDescrlabel,grpRowIndx++,1,1,1);
        for(int i = 0; i < PORE_STATE_NUM; ++i)
        {
            m_pbtnPoreStateClrChose[senGrp][i] = new QToolButton(this);
            auto pbtn = m_pbtnPoreStateClrChose[senGrp][i];
            QAction* act = new QAction(this);
            act->setToolTip(tr("Change color"));
            pbtn->setDefaultAction(act);
            m_poreStateColor[senGrp][i] = gPoreStateColr[i];
            SetBtnColor(pbtn, m_poreStateColor[senGrp][i]);
            connect(pbtn,SIGNAL(pressed()),m_pPoreClrBtnMapper,SLOT(map()));
            int buttonIndx = 0;
            buttonIndx |= (senGrp &0x0f) << 4 | (i &0x0f);
            m_pPoreClrBtnMapper->setMapping(pbtn, buttonIndx);
            QLabel *plabStateName = new QLabel(QString::fromStdString(gPoreStateStr[i]),this);
            m_plabPoreStateCnt[senGrp][i] = new QLabel(QString("%1").arg(QString::number(i)),this);

            gridGrp->addWidget(pbtn,grpRowIndx,0,1,1);
            gridGrp->addWidget(plabStateName,grpRowIndx,1,1,1);
            gridGrp->addWidget(m_plabPoreStateCnt[senGrp][i],grpRowIndx++,2,1,1);
        }
        gridGrp->setRowStretch(grpRowIndx++,10);
        m_pPoreGrpWdgt[senGrp] = new QWidget(this);
        m_pPoreGrpWdgt[senGrp]->setLayout(gridGrp);
        m_pPoreTabWidgt->insertTab(senGrp,m_pPoreGrpWdgt[senGrp],SENSRO_STR[senGrp]);
    }
    QHBoxLayout *btnhorlayout =new QHBoxLayout();
    btnhorlayout->addWidget(m_pbtnPoreSateCount);
    btnhorlayout->addWidget(m_pbtnPoreSateRest);

    vertlayout->addWidget(m_pPoreTabWidgt);
    vertlayout->addLayout(btnhorlayout);
    vertlayout->addStretch();
    m_pGrpPorebox->setLayout(vertlayout);
//    m_pPoreTabWidgt->setTabPosition(QTabWidget::East);


    connect(m_pPoreClrBtnMapper, SIGNAL(mapped (const int &)), this, SLOT(OnClickChangePoreStaeColor(const int &)));
    connect(m_pPoreTabWidgt,&QTabWidget::currentChanged,this,&PoreStateMapWdgt::OnPoreSensorGrpChgeSlot);
    connect(m_pbtnPoreSateRest,&QPushButton::clicked,[&](){
        int sensorGrp = m_pPoreTabWidgt->currentIndex();
         std::fill_n(m_vctPoreStatePrev.begin(),CHANNEL_NUM,NONE_PORE_STATE);
        m_pPoreStateMapTabModel->RestState(PORE_STATE_MAP_MODE,NONE_PORE_STATE,sensorGrp);
        for (auto &cnt : m_vctPoreStateCnt[sensorGrp]) {  cnt = 0;}
        for(int i = 0; i <PORE_STATE_NUM;++i )
        {
            m_plabPoreStateCnt[sensorGrp][i]->setText(QString::number(m_vctPoreStateCnt[sensorGrp][i]));
        }
    });
    connect(m_pbtnPoreSateCount,&QPushButton::clicked,this,&PoreStateMapWdgt::OnCountPoreStateCnt);

}

void PoreStateMapWdgt::InitLockOperateStateCtrl(void)
{
    m_pGrpLockbox = new QGroupBox(QStringLiteral("Lock operate"),this);
    m_pLockClrBtnMapper = new QSignalMapper(this);

    m_pLockTabWidgt = new QTabWidget(this);
    QVBoxLayout *vertlayout = new QVBoxLayout();

    for(int senGrp = 0; senGrp < SENSOR_GROUP_NUM; ++senGrp)
    {
        QGridLayout *gridGrp = new QGridLayout();

        QLabel *pDescrlabel = new QLabel(QString("Sensor%1").arg(SENSRO_STR[senGrp]),this);
        int grpRowIndx = 0;
        gridGrp->addWidget(pDescrlabel,grpRowIndx++,1,1,1);
        for(int i = 0; i < LOCK_OPERATE_STATE_NUM; ++i)
        {
            m_pbtnLockState[senGrp][i] = new QToolButton(m_pGrpLockbox);
            auto pbtn = m_pbtnLockState[senGrp][i];
            QAction* act = new QAction(m_pGrpLockbox);
            act->setToolTip(tr("Change color"));
            pbtn->setDefaultAction(act);
            m_clrLockState[senGrp][i] = gLockStateColr[i];
            SetBtnColor(pbtn, m_clrLockState[senGrp][i]);
            connect(pbtn,SIGNAL(pressed()),m_pLockClrBtnMapper,SLOT(map()));
            int buttonIndx = 0;
            buttonIndx |= (senGrp &0x0f) << 4 | (i &0x0f);
            m_pLockClrBtnMapper->setMapping(pbtn, buttonIndx);
            QLabel *plabStateName = new QLabel(QString::fromStdString(gLockStateStr[i]),m_pGrpLockbox);
            m_plabLockStateCnt[senGrp][i] = new QLabel(QString("%1").arg(QString::number(i)),this);

            gridGrp->addWidget(pbtn,grpRowIndx,0,1,1);
            gridGrp->addWidget(plabStateName,grpRowIndx,1,1,1);
            gridGrp->addWidget(m_plabLockStateCnt[senGrp][i],grpRowIndx++,2,1,1);
        }
        gridGrp->setRowStretch(grpRowIndx++,10);
        m_pSensorGrpwidget[senGrp] = new QWidget(this);
        m_pSensorGrpwidget[senGrp]->setLayout(gridGrp);
        m_pLockTabWidgt->insertTab(senGrp,m_pSensorGrpwidget[senGrp],SENSRO_STR[senGrp]);
    }

    QGridLayout *gridOprate = new QGridLayout();
    int grpRowIndx = 0;
    m_pbtnAllowOperate =new QPushButton(QStringLiteral("Allow"),this);
    m_pbtnLockSateRest =new QPushButton(QStringLiteral("Clear"),this);
    m_pbtnLockSateCount =new QPushButton(QStringLiteral("Count"),this);
    m_pbtnProhibitedOperate=new QPushButton(QStringLiteral("Prohibited"),this);
    m_pbtnLockScan=new QPushButton(QStringLiteral("Scan"),this);
    m_pbtnAllowOperate->setObjectName("LockAllowOperateBtn");
    m_pbtnProhibitedOperate->setObjectName("LockProhibitOperateBtn");
    m_pbtnUnblock=new QPushButton(QStringLiteral("Unblock"),this);
    m_pbtnNormal=new QPushButton(QStringLiteral("Normal"),this);
    m_pbtnUnblock->setObjectName("Unblock");
    m_pbtnNormal->setObjectName("Normal");
    m_pScanCntSpbx= new QSpinBox(this);
    m_pScanCntSpbx->setRange(1,30);
    m_pScanCntSpbx->setSuffix(" times");
    m_pScanCntSpbx->setValue(10);

    m_pbtnAllowOperate->setToolTip(QStringLiteral("Allow operations on selected channels "));
    m_pbtnProhibitedOperate->setToolTip(QStringLiteral("Prohibit operations on selected channels "));
    gridOprate->addWidget(m_pbtnLockScan,grpRowIndx,0,1,1);
    gridOprate->addWidget(m_pScanCntSpbx,grpRowIndx++,1,1,1);
    gridOprate->addWidget(m_pbtnAllowOperate,grpRowIndx,0,1,1);
    gridOprate->addWidget(m_pbtnProhibitedOperate,grpRowIndx++,1,1,1);
    gridOprate->addWidget(m_pbtnNormal,grpRowIndx,0,1,1);
    gridOprate->addWidget(m_pbtnUnblock,grpRowIndx++,1,1,1);
    gridOprate->setRowStretch(grpRowIndx++,10);
    gridOprate->addWidget(m_pbtnLockSateCount,grpRowIndx,0,1,1);
    gridOprate->addWidget(m_pbtnLockSateRest,grpRowIndx++,1,1,1);

//    m_pLockTabWidgt->setTabPosition(QTabWidget::East);
//    m_pLockTabWidgt->setDocumentMode(true);


    vertlayout->addWidget(m_pLockTabWidgt);
    vertlayout->addLayout(gridOprate);
    vertlayout->setStretch(0,10);
    vertlayout->setStretch(1,1);
    m_pGrpLockbox->setLayout(vertlayout);

    connect(m_pLockClrBtnMapper, SIGNAL(mapped (const int &)), this, SLOT(OnClickChangeLockStaeColor(const int &)));
    connect(m_pLockTabWidgt,&QTabWidget::currentChanged,this,&PoreStateMapWdgt::OnLockGrpChgeSlot);
    connect(m_pbtnLockSateRest,&QPushButton::clicked,this,&PoreStateMapWdgt::OnLockClearClickSlot);
    connect(m_pbtnLockScan,&QPushButton::clicked,this,&PoreStateMapWdgt::OnLockScanClickSlot);
    connect(m_pbtnAllowOperate,&QPushButton::clicked,this,&PoreStateMapWdgt::OnLockAllowForbitBtnClickSlot);
    connect(m_pbtnProhibitedOperate,&QPushButton::clicked,this,&PoreStateMapWdgt::OnLockAllowForbitBtnClickSlot);
    connect(m_pbtnUnblock,&QPushButton::clicked,this,&PoreStateMapWdgt::OnLockAllowForbitBtnClickSlot);
    connect(m_pbtnNormal,&QPushButton::clicked,this,&PoreStateMapWdgt::OnLockAllowForbitBtnClickSlot);
    connect(m_pbtnLockSateCount,&QPushButton::clicked,this,&PoreStateMapWdgt::OnCountLockStateCnt);


}


void PoreStateMapWdgt::InitValidStateCtrl(void)
{
    m_pGrpValidbox = new QGroupBox(QStringLiteral("Valid State"),this);
    m_pValidClrBtnMapper = new QSignalMapper(this);

    m_pValidTabWidgt = new QTabWidget(this);
    QVBoxLayout *vertlayout = new QVBoxLayout();

    for(int senGrp = 0; senGrp < SENSOR_GROUP_NUM; ++senGrp)
    {
        QGridLayout *gridGrp = new QGridLayout();
        QLabel *pDescrlabel = new QLabel(QString("Valid Sensor%1").arg(SENSRO_STR[senGrp]),this);
        int grpRowIndx = 0;
        gridGrp->addWidget(pDescrlabel,grpRowIndx++,1,1,1);
        for(int i = 0; i < VALID_STATE_NUM; ++i)
        {
            m_pbtnValidState[senGrp][i] = new QToolButton(this);
            auto pbtn = m_pbtnValidState[senGrp][i];
            QAction* act = new QAction(this);
            act->setToolTip(tr("Change color"));
            pbtn->setDefaultAction(act);
            m_clrValidState[senGrp][i] = gValidStateColr[i];
            SetBtnColor(pbtn, m_clrValidState[senGrp][i]);
            connect(pbtn,SIGNAL(pressed()),m_pValidClrBtnMapper,SLOT(map()));
            int buttonIndx = 0;
            buttonIndx |= (senGrp &0x0f) << 4 | (i &0x0f);
            m_pValidClrBtnMapper->setMapping(pbtn, buttonIndx);
            QLabel *plabStateName = new QLabel(QString::fromStdString(gValidStateStr[i]),m_pGrpValidbox);
            m_plabValidStateCnt[senGrp][i] = new QLabel(QString("%1").arg(QString::number(i)),this);

            gridGrp->addWidget(pbtn,grpRowIndx,0,1,1);
            gridGrp->addWidget(plabStateName,grpRowIndx,1,1,1);
            gridGrp->addWidget(m_plabValidStateCnt[senGrp][i],grpRowIndx++,2,1,1);
        }
        gridGrp->setRowStretch(grpRowIndx++,10);
        m_pValidGrpwidget[senGrp] = new QWidget(this);
        m_pValidGrpwidget[senGrp]->setLayout(gridGrp);
        m_pValidTabWidgt->insertTab(senGrp,m_pValidGrpwidget[senGrp],SENSRO_STR[senGrp]);
    }

    m_pbtnValidSateRest =new QPushButton(QStringLiteral("Clear"),this);
    m_pbtnValidOperate =new QPushButton(QStringLiteral("Valid"),this);
    m_pbtnInValidOperate =new QPushButton(QStringLiteral("Invalid"),this);
    m_pbtnValidCount =new QPushButton(QStringLiteral("Count"),this);
    m_pbtnValidSateRest->setObjectName("ClearBtn");
    m_pbtnValidOperate->setObjectName("ValidBtn");
    m_pbtnInValidOperate->setObjectName("InvalidBtn");
    m_pbtnValidCount->setObjectName("Count");

    QGridLayout *gridOprate = new QGridLayout();
    gridOprate->addWidget(m_pbtnValidOperate,0,0,1,1);
    gridOprate->addWidget(m_pbtnInValidOperate,0,1,1,1);
    gridOprate->addWidget(m_pbtnValidCount,2,0,1,1);
    gridOprate->addWidget(m_pbtnValidSateRest,2,1,1,1);
    gridOprate->setRowStretch(1,10);

    vertlayout->addWidget(m_pValidTabWidgt);
    vertlayout->addLayout(gridOprate);
    vertlayout->setStretch(0,10);
    vertlayout->setStretch(1,1);

    m_pGrpValidbox->setLayout(vertlayout);

    connect(m_pValidClrBtnMapper, SIGNAL(mapped (const int &)), this, SLOT(OnClickChangeValidStaeColor(const int &)));
    connect(m_pValidTabWidgt,&QTabWidget::currentChanged,[=](const int &index){ m_pPoreStateMapTabModel->ChangeSensorValidGroup( SENSOR_STATE_GROUP_ENUM(index));});
    connect(m_pbtnValidOperate,&QPushButton::clicked,this,&PoreStateMapWdgt::OnValidOrNotOperateBtnSlot);
    connect(m_pbtnInValidOperate,&QPushButton::clicked,this,&PoreStateMapWdgt::OnValidOrNotOperateBtnSlot);
    connect(m_pbtnValidCount,&QPushButton::clicked,this,&PoreStateMapWdgt::OnCountValidStateCnt);

    connect(m_pbtnValidSateRest,&QPushButton::clicked,[&](){
        SENSOR_STATE_GROUP_ENUM sensorGrp = (SENSOR_STATE_GROUP_ENUM)m_pValidTabWidgt->currentIndex();
        m_pPoreStateMapTabModel->RestState(VALID_SENSOR_STATE_MAP_MODE,INVALID_CHANNEL_ENUM,sensorGrp);
        for (auto &cnt : m_vctValidStateCnt[sensorGrp]) {  cnt = 0;}
        for(int i = 0; i <VALID_STATE_NUM;++i )
        {
            m_plabValidStateCnt[sensorGrp][i]->setText(QString::number(m_vctValidStateCnt[sensorGrp][i]));
        }
    });

}


void PoreStateMapWdgt::InitMuxStateCtrl(void)
{
    m_pGrpMuxbox = new QGroupBox(QStringLiteral("Mux State"),this);

    QVBoxLayout *vertlayout = new QVBoxLayout();
    m_pMuxClrBtnMapper =  new QSignalMapper(this);
    int grpRowIndx = 0;
    QGridLayout *muxGridGrp = new QGridLayout();
    for(int sensorGrp = 0;sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
    {
        m_pbtnMuxGrpColor[sensorGrp] =new QToolButton(this);
        auto pbtn = m_pbtnMuxGrpColor[sensorGrp];
        QAction* act = new QAction(this);
        act->setToolTip(tr("Change color"));
        pbtn->setDefaultAction(act);
        m_pMuxGrpColor[sensorGrp] = gMuxGroupStateColr[sensorGrp];
        SetBtnColor(pbtn, m_pMuxGrpColor[sensorGrp]);
        connect(pbtn,SIGNAL(pressed()),m_pMuxClrBtnMapper,SLOT(map()));
        m_pMuxClrBtnMapper->setMapping(pbtn, sensorGrp);
        QLabel *plabStateName = new QLabel(QString::fromStdString(gMuxGroupStateStr[sensorGrp]),this);
        m_plabMuxStateCnt[sensorGrp] = new QLabel(QString("%1").arg(sensorGrp),this);

        muxGridGrp->addWidget(pbtn,grpRowIndx,0,1,1);
        muxGridGrp->addWidget(plabStateName,grpRowIndx,1,1,1);
        muxGridGrp->addWidget(m_plabMuxStateCnt[sensorGrp],grpRowIndx++,2,1,1);
        muxGridGrp->setRowStretch(grpRowIndx,10);
    }

    m_pMuxValidCntDisplay = new QLabel("Valid: 0",this);
    m_pMuxSinglePoreCntDisplay = new QLabel("Single pore: 0",this);

    m_pbtnMuxStateRest =new QPushButton(QStringLiteral("Clear"),this);
    m_pbtnAllchMuxRandom =new QPushButton(QStringLiteral("Random"),this);
    m_pbtnAllchMuxValidState =new QPushButton(QStringLiteral("Mux"),this);
    m_pbtnAllchMuxValidCount =new QPushButton(QStringLiteral("Count"),this);
    m_pbtnMuxStateRest->setObjectName("ClearBtn");

    m_pbtnAllchMuxRandom->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Eg:Sensor有效组为2组则2组随机选一，若无有效组则4组随机选一组</p></body></html>"));
    m_pbtnAllchMuxValidState->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Eg:Sensor有效组为2组则在当前有效2组顺移切换，若无有效组则在根据当前所在组在4组顺移切换</p></body></html>"));

    QVBoxLayout *vertlayout1 = new QVBoxLayout();
    vertlayout1->addWidget(m_pMuxValidCntDisplay);
    vertlayout1->addWidget(m_pMuxSinglePoreCntDisplay);
    QGridLayout *gridOprate = new QGridLayout();
    gridOprate->addWidget(m_pbtnAllchMuxRandom,0,0,1,1);
    gridOprate->addWidget(m_pbtnAllchMuxValidState,0,1,1,1);

    gridOprate->addWidget(m_pbtnAllchMuxValidCount,2,0,1,1);
    gridOprate->addWidget(m_pbtnMuxStateRest,2,1,1,1);
    gridOprate->setRowStretch(1,10);

    vertlayout->addLayout(muxGridGrp);
    vertlayout->addLayout(vertlayout1);
    vertlayout->addStretch(10);
    vertlayout->addLayout(gridOprate);



    m_pGrpMuxbox->setLayout(vertlayout);
    connect(m_pMuxClrBtnMapper, SIGNAL(mapped (const int &)), this, SLOT(OnClickChangeMuxStaeColor(const int &)));
    connect(m_pbtnAllchMuxRandom,&QPushButton::clicked,this,&PoreStateMapWdgt::OnAllChValidGenerateMuxRandomBtnSlot);
    connect(m_pbtnAllchMuxValidState,&QPushButton::clicked,this,&PoreStateMapWdgt::OnAllChValidGenerateMuxBtnSlot);
    connect(m_pbtnAllchMuxValidCount,&QPushButton::clicked,this,&PoreStateMapWdgt::OnCountMuxStateCnt);
    connect(m_pbtnMuxStateRest,&QPushButton::clicked,[&](){
        m_pPoreStateMapTabModel->RestState(MUX_STATE_MAP_MODE,SENSOR_STATE_A_GROUP);
        std::fill_n(m_vctMuxStateCnt.begin(),m_vctMuxStateCnt.size(),0);
        for(int i = 0; i <MUX_STATE_NUM;++i )
        {
            m_plabMuxStateCnt[i]->setText(QString::number(m_vctMuxStateCnt[i]));
        }
    });



}




bool PoreStateMapWdgt::SetBtnColor(QToolButton* pbtn, QColor clr)
{
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(QRect(0, 0, 15, 15));
    painter.fillRect(QRect(1, 1, 14, 14), clr);
    pbtn->defaultAction()->setIcon(QIcon(pixmap));
    return false;
}

void PoreStateMapWdgt::OnSensorStateRefreshBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }

    std::vector<StateRegUnion> &vctSensorRegAry =  ConfigServer::GetInstance()->GetSensorStateRegCache();

    uchar addr = 0x20;
    bool res = false;
    for(int i = 0; i < vctSensorRegAry.size();++i)
    {
        m_pUsbFtdDevice->ASICRegisterReadOne(addr+i,vctSensorRegAry[i].stateReg,res);
        if(!res)
        {
            QMessageBox::critical(this, tr("Read sensor state failed!!!"), tr(" Read sensor state  %1 failed!!!").arg(QString::number(addr+i,16)));
            return;
        }
    }

    auto &vctSensorState = m_pModelSharedDataPtr->GetSensorStateData();
    for(int i = 0,regNum = 0; regNum< vctSensorRegAry.size(); ++regNum,i +=8)
    {
        vctSensorState[i+0] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch0);
        vctSensorState[i+1] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch1);
        vctSensorState[i+2] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch2);
        vctSensorState[i+3] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch3);
        vctSensorState[i+4] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch4);
        vctSensorState[i+5] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch5);
        vctSensorState[i+6] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch6);
        vctSensorState[i+7] = SENSOR_STATE_ENUM(vctSensorRegAry[regNum].SensorState.ch7);
    }

    m_pModelSharedDataPtr->UpdateAllChSensorGroupAndFuncStateBySensorState();
    m_pPoreStateMapTabModel->UpdateAllChDataDisplay();
//    m_pPoreStateMapTabModel->ResetModel();

    OnCountSensorStateCnt();

}


void PoreStateMapWdgt::OnClickChangeColor(const int & state)
{
    QToolButton* pbtn = m_pbtnSensorState[state];
    QColor color = QColorDialog::getColor(m_clrSensorState[state], this);
    if (color.isValid())
    {
        m_clrSensorState[state] = color;
        m_pPoreStateMapTabModel->SetStateColor(SENSOR_STATE_MAP_MODE,state,color);
        SetBtnColor(pbtn, color);
    }
}


void PoreStateMapWdgt::OnClickChangeCapStaeColor(const int & state)
{
    QToolButton* pbtn = m_pbtnCapState[state];
    QColor color = QColorDialog::getColor(m_clrCapState[state], this);
    if (color.isValid())
    {
        m_clrCapState[state] = color;
        m_pPoreStateMapTabModel->SetStateColor(CAP_STATE_MAP_MODE,state,color);
        SetBtnColor(pbtn, color);
    }
}

void PoreStateMapWdgt::OnClickChangeOVFStaeColor(const int & state)
{
    QToolButton* pbtn = m_pbtnOVFState[state];
    QColor color = QColorDialog::getColor(m_clrOVFState[state], this);
    if (color.isValid())
    {
        m_clrOVFState[state] = color;
        m_pPoreStateMapTabModel->SetStateColor(OVF_STATE_MAP_MODE,state,color);
        SetBtnColor(pbtn, color);
    }
}

void PoreStateMapWdgt::OnClickChangePoreStaeColor(const int & mux)
{
    int sensorGrp = (mux &0xf0) >> 4 ;
    int sensorState = (mux &0x0f) ;
    QToolButton* pbtn = m_pbtnPoreStateClrChose[sensorGrp][sensorState];
    QColor color = QColorDialog::getColor(m_poreStateColor[sensorGrp][sensorState], this);
    if (color.isValid())
    {
        m_poreStateColor[sensorGrp][sensorState] = color;
        m_pPoreStateMapTabModel->SetStateColor(PORE_STATE_MAP_MODE,sensorState,color,sensorGrp);
        SetBtnColor(pbtn, color);
    }
}

void PoreStateMapWdgt::OnClickChangeLockStaeColor(const int & state)
{
    int sensorGrp = (state &0xf0) >> 4 ;
    int sensorState = (state &0x0f) ;
    QToolButton* pbtn = m_pbtnLockState[sensorGrp][sensorState];
    QColor color = QColorDialog::getColor(m_clrLockState[sensorGrp][sensorState], this);
    if (color.isValid())
    {
        m_clrLockState[sensorGrp][sensorState] = color;
        m_pPoreStateMapTabModel->SetStateColor(LOCK_SENSOR_STATE_MAP_MODE,sensorState,color,sensorGrp);
        SetBtnColor(pbtn, color);
    }
}

void PoreStateMapWdgt::OnClickChangeValidStaeColor(const int & state)
{
    int sensorGrp = (state &0xf0) >> 4 ;
    int sensorState = (state &0x0f) ;
    QToolButton* pbtn = m_pbtnValidState[sensorGrp][sensorState];
    QColor color = QColorDialog::getColor(m_clrValidState[sensorGrp][sensorState], this);
    if (color.isValid())
    {
        m_clrValidState[sensorGrp][sensorState] = color;
        m_pPoreStateMapTabModel->SetStateColor(VALID_SENSOR_STATE_MAP_MODE,sensorState,color,sensorGrp);
        SetBtnColor(pbtn, color);
    }
}

void PoreStateMapWdgt::OnClickChangeMuxStaeColor(const int & state)
{
    QToolButton* pbtn = m_pbtnMuxGrpColor[state];
    QColor color = QColorDialog::getColor(m_pMuxGrpColor[state], this);
    if (color.isValid())
    {
        m_pMuxGrpColor[state] = color;
        m_pPoreStateMapTabModel->SetStateColor(MUX_STATE_MAP_MODE,state,color);
        SetBtnColor(pbtn, color);
    }
}




void PoreStateMapWdgt::OnLockClearClickSlot(void)
{
//    int sensorGrp = m_pLockTabWidgt->currentIndex();
    for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
    {
    m_pPoreStateMapTabModel->RestState(LOCK_SENSOR_STATE_MAP_MODE,ALLOW_OPERATION_STATE,sensorGrp);
    for (auto &cnt : m_vctLockStateCnt[sensorGrp]) {  cnt = 0;}
    for(int i = 0; i <LOCK_OPERATE_STATE_NUM;++i )
    {
        m_plabLockStateCnt[sensorGrp][i]->setText(QString::number(m_vctLockStateCnt[sensorGrp][i]));
        }
    }
}
void PoreStateMapWdgt::SetLockScanGroup(const SENSOR_STATE_GROUP_ENUM &sensorGrp,const int &scantimes)
{
    m_pLockTabWidgt->setCurrentIndex(sensorGrp);
    m_pScanCntSpbx->setValue(scantimes);
}

void PoreStateMapWdgt::OnLockScanClickSlot(void)
{
    m_pbtnLockScan->setEnabled(false);
    m_pScanCntSpbx->setEnabled(false);
    OnLockScanForAllCh();
    m_pScanCntSpbx->setEnabled(true);
    m_pbtnLockScan->setEnabled(true);
}


bool PoreStateMapWdgt::OnLockScanForAllCh(void)
{
    int totalCnt = m_pScanCntSpbx->value();
    int curCnt = 0;

    m_pProgressDlg->setWindowModality(Qt::NonModal);
    m_pProgressDlg->setRange(0,totalCnt);
    m_pProgressDlg->reset();
    m_pProgressDlg->setValue(0);
    m_pProgressDlg->show();
    do
    {
        if (m_pProgressDlg->wasCanceled())
        {
            m_pProgressDlg->reset();
            return false;
        }
        m_pProgressDlg->setValue(curCnt);
        m_pProgressDlg->setLabelText(QString("Begin Scanning ovf %1th!").arg(curCnt+1));

//        QCoreApplication::processEvents();
        std::array<std::array<int,LVDS_HARD_MAP_COLS>,LVDS_HARD_MAP_ROWS> matrix{};
        for(int cycle = 0; cycle < 16; ++cycle)
        {
            LockScanMappingByCycle(cycle,matrix);
            const auto &vctFuncState = TransScanMatrixToFuncStat(matrix);
            m_pSensorPanel->AllChDifferFuncStateSetSlot(vctFuncState);

            OnCountSensorStateCnt();

            std::fill(m_vctOVFCountToLock.begin(),m_vctOVFCountToLock.end(),0);

            for(int i = 0; i< 3; ++i)
            {
                OnOVFStateRefreshBtnSlot();
                const auto &vctOvfState = m_pModelSharedDataPtr->GetOVFStateData();
                for(int ch =0; ch < CHANNEL_NUM; ++ch)
                {
                    if(vctOvfState[ch] == OVER_FLOW_FLAG)
                        m_vctOVFCountToLock[ch]++;
                }
            }
            auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();
            const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
            auto &vctAllChFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
            for(int ch =0; ch < CHANNEL_NUM; ++ch)
            {
#ifdef EASY_PROGRAM_DEBUGGING
                if(m_vctOVFCountToLock[ch] >= 1 )
#else
                if(m_vctOVFCountToLock[ch] >= 3 )
#endif
                {
                    const auto &chSensorGrp = vctAllChSensorGrp[ch];
                    auto &chLockState = vctAllChFourLockState[chSensorGrp][ch];
                    if(chLockState != PROHIBITED_OPERATION_STATE)
                    {
                        chLockState = PROHIBITED_OPERATION_STATE;
                        vctAllChFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
                        emit SigSpecificChanSetFunctState(ch,SENSORGRP_FUNC_STATE_OFF);
                    }
                }

            }
            if(isVisible() && m_pStateTabWidget->currentIndex() == SENSOR_STATE_MAP_MODE)
            {
                m_pPoreStateMapTabModel->UpdateAllChDataDisplay();
            }
        }

    }while(++curCnt < totalCnt);
    m_pProgressDlg->setValue(m_pProgressDlg->maximum());
    if(isVisible())
    {
        m_pPoreStateMapTabModel->UpdateAllChDataDisplay();
    }
    OnCountLockStateCnt();
    return true;
}

void PoreStateMapWdgt::OnCancelScanProgress(void)
{
    if(!m_pProgressDlg->wasCanceled())
    m_pProgressDlg->cancel();
}

void PoreStateMapWdgt::OnLockAllowForbitBtnClickSlot(void)
{
    QModelIndexList indexlist = m_pPoreStateMapTabView->selectionModel()->selectedIndexes();
    QPushButton *btn=qobject_cast<QPushButton*>(sender()); //得到当前信号来源的对象
    if(indexlist.size() > 0  && btn)
    {
        SENSOR_STATE_GROUP_ENUM curLockGrp = (SENSOR_STATE_GROUP_ENUM)m_pLockTabWidgt->currentIndex();
        auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();

        for(const auto &index : indexlist)
        {
            int ch = UNORDERCHANNEL_GET_BY_INDEX_ARRAY[index.row()][index.column()];
            if(btn->objectName() == "LockAllowOperateBtn")
            {
                vctAllChFourLockState[curLockGrp][ch] = ALLOW_OPERATION_STATE;
            }
            else if(btn->objectName() == "LockProhibitOperateBtn")
            {
                vctAllChFourLockState[curLockGrp][ch] = PROHIBITED_OPERATION_STATE;
                emit SigSpecificChanSetFunctState(ch,SENSORGRP_FUNC_STATE_OFF);
            }
            else if(btn->objectName() == "Unblock")
            {
                if(vctAllChFourLockState[curLockGrp][ch] == ALLOW_OPERATION_STATE)
                {
                    emit SigSpecificChanSetFunctState(ch, SENSORGRP_FUNC_STATE_UNBLOCK);
                }
            }
            else if(btn->objectName() == "Normal")
            {
                if(vctAllChFourLockState[curLockGrp][ch] == ALLOW_OPERATION_STATE)
                {
                    emit SigSpecificChanSetFunctState(ch, SENSORGRP_FUNC_STATE_NORMAL);
                }
            }

        }
        QMessageBox::information(this, tr("Successed!!!"), tr("Change locks state successfully !!!"));
        return;
    }
    QMessageBox::critical(this, tr("Error!!!"), tr("Change locks state failed !!!"));
}


std::vector<SENSORGRP_FUNC_STATE_ENUM> &PoreStateMapWdgt::TransScanMatrixToFuncStat(const std::array<std::array<int,LVDS_HARD_MAP_COLS>,LVDS_HARD_MAP_ROWS> & matrix)
{
    auto &vctAllChFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    for(int row = 0; row < LVDS_HARD_MAP_ROWS; ++row)
    {
        for(int col = 0; col < LVDS_HARD_MAP_COLS; ++col)
        {
            int ch = LVDS_HARDWARE_INDEX_ARRAY[row][col];
            if(matrix[row][col] == 1)
            {
#ifdef EASY_PROGRAM_DEBUGGING
                vctAllChFuncState[ch] = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
#else
                vctAllChFuncState[ch] = SENSORGRP_FUNC_STATE_NORMAL;
#endif
            }
            else
            {
                vctAllChFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
            }
        }
    }
    return vctAllChFuncState;
}



bool PoreStateMapWdgt::LockScanMappingByCycle(const int &cycle,std::array<std::array<int,LVDS_HARD_MAP_COLS>,LVDS_HARD_MAP_ROWS> & matrix)
{
    if(cycle >= LVDS_HARD_MAP_ROWS)
        return false;

    for(auto &rowArry: matrix)
    {
        rowArry.fill(0);
    }

    int rowsTot = LVDS_HARD_MAP_ROWS;
    int halfrow = rowsTot/2;
    for(int row = 0; row <LVDS_HARD_MAP_ROWS; ++row )
    {
        for(int col = 0; col < LVDS_HARD_MAP_COLS; ++col)
        {
            if(col%2 == 0)  //偶数列  row15 -> row0
            {
                matrix[rowsTot - cycle -1][col] = 1;
            }
            else  //奇数列 前半周期 row8->row15 后半周期 row0->row7
            {
                int absDiff =  qAbs(halfrow - cycle);
                if(cycle < halfrow)
                {
                    matrix[absDiff - 1][col] = 1;
                }
                else
                {
                    matrix[rowsTot - absDiff -1][col] = 1;
                }

            }
//            std::cout <<std::setw(1)<<matrix[row][col];
        }
//        std::cout <<std::endl;
    }
//    std::cout <<std::endl;
    return true;
}

#if 0
void PoreStateMapWdgt::PoreProtectByMontorAvegeSlot(const std::vector<float> &average)
{
    float siglePoreLimit = 0,multPoreLimit = 0,membrkLimit = 0;
    emit SigGetPoreLimitCurrInp(siglePoreLimit,multPoreLimit,membrkLimit);

    bool bPoreNeedToLock = false;
    emit AcquireWhetherPoreLockSig(bPoreNeedToLock);
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFourPoreState = m_pModelSharedDataPtr->GetFourPoreStateData();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();

    int changeCnt = 0;
    for(int ch = 0; ch< CHANNEL_NUM; ++ch)
    {
        auto &aver = average[ch];
        auto &funcSta = vctAllchFuncState[ch];
        const auto &chSensorGrp = vctAllChSensorGrp[ch];
        auto &poreSta = vctAllChFourPoreState[chSensorGrp][ch];
        auto &lockSta = vctAllChFourLockState[chSensorGrp][ch];
        if(aver >siglePoreLimit && funcSta != SENSORGRP_FUNC_STATE_OFF)
        {
            funcSta = SENSORGRP_FUNC_STATE_OFF;
            changeCnt++;
            if(aver >membrkLimit)
            {
                poreSta = SATURATED_PORE_STATE;
            }else if(aver >multPoreLimit)
            {
                poreSta = MULTI_PORE_STATE;
            }else
            {
                poreSta = SINGLE_PORE_STATE;
            }
            if(bPoreNeedToLock && lockSta != PROHIBITED_OPERATION_STATE)
            {
                lockSta = PROHIBITED_OPERATION_STATE;
            }
        }
    }
    OnCountPoreStateCnt();

    if(changeCnt > 0)
    {
        emit SigAllChDifferFuncStateSet(vctAllchFuncState); //选择128寄存器一起发送
        OnCountSensorStateCnt();
    }

    if(isVisible())
    {
        m_pPoreStateMapTabModel->UpdateAllChDataDisplay();
//        m_pPoreStateMapTabModel->ResetModel();
    }
}
#endif
void PoreStateMapWdgt::PoreProtectByMontorAvegeSlot(const std::vector<float> &average)
{
    float siglePoreLimit = 0,multPoreLimit = 0,membrkLimit = 0;
    emit SigGetPoreLimitCurrInp(siglePoreLimit,multPoreLimit,membrkLimit);
    bool bPoreNeedToLock = false;
    emit AcquireWhetherPoreLockSig(bPoreNeedToLock);
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFourPoreState = m_pModelSharedDataPtr->GetFourPoreStateData();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();

    int changeCnt = 0;
    bool bChange = false;
    for(int ch = 0; ch< CHANNEL_NUM; ++ch)
    {
        bChange = false;
        auto &aver = average[ch];
        auto &funcSta = vctAllchFuncState[ch];
        const auto &chSensorGrp = vctAllChSensorGrp[ch];
        auto &poreSta = vctAllChFourPoreState[chSensorGrp][ch];
        auto &lockSta = vctAllChFourLockState[chSensorGrp][ch];
        auto &prevPoreState = m_vctPoreStatePrev[ch];
        if(aver >siglePoreLimit && funcSta != SENSORGRP_FUNC_STATE_OFF)
        {
            if(aver >membrkLimit)
            {
                if(ComaprePreviousPoreState(SATURATED_PORE_STATE,prevPoreState))
                {
                    poreSta = SATURATED_PORE_STATE;
                    funcSta = SENSORGRP_FUNC_STATE_OFF;
                    changeCnt++;
                    bChange = true;
                }
                prevPoreState = SATURATED_PORE_STATE;

            }else if(aver >multPoreLimit)
            {
                if(ComaprePreviousPoreState(MULTI_PORE_STATE,prevPoreState))
                {
                    poreSta = MULTI_PORE_STATE;
                    funcSta = SENSORGRP_FUNC_STATE_OFF;
                    changeCnt++;
                    bChange = true;
                }
                prevPoreState = MULTI_PORE_STATE;
            }else
            {
                if(ComaprePreviousPoreState(SINGLE_PORE_STATE,prevPoreState))
                {
                    poreSta = SINGLE_PORE_STATE;
                    funcSta = SENSORGRP_FUNC_STATE_OFF;
                    changeCnt++;
                    bChange = true;
                }
                prevPoreState = SINGLE_PORE_STATE;
            }
            if(bChange && bPoreNeedToLock && lockSta != PROHIBITED_OPERATION_STATE)
            {
                lockSta = PROHIBITED_OPERATION_STATE;
            }
        }
    }

    if(changeCnt > 0)
    {
        OnCountPoreStateCnt();
        emit SigAllChDifferFuncStateSet(vctAllchFuncState); //选择128寄存器一起发送
        OnCountSensorStateCnt();
    }

    if(isVisible())
    {
        m_pPoreStateMapTabModel->UpdateAllChDataDisplay();
//        m_pPoreStateMapTabModel->ResetModel();
    }
}

void PoreStateMapWdgt::MembrokeProtectByMontorAvegeSlot(const std::vector<float> &average)
{
    float membrkLimit = 0;
    emit SigGetMembraneimitCurrInp(membrkLimit);

    bool bPoreNeedToLock = false;
    emit AcquireWhetherPoreLockSig(bPoreNeedToLock);
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFourPoreState = m_pModelSharedDataPtr->GetFourPoreStateData();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();

    int changeCnt = 0;
    bool bChange = false;
    for(int ch = 0; ch< CHANNEL_NUM; ++ch)
    {
        bChange = false;
        auto &aver = average[ch];
        auto &funcSta = vctAllchFuncState[ch];
        const auto &chSensorGrp = vctAllChSensorGrp[ch];
        auto &poreSta = vctAllChFourPoreState[chSensorGrp][ch];
        auto &lockSta = vctAllChFourLockState[chSensorGrp][ch];
        auto &prevPoreState = m_vctPoreStatePrev[ch];
        if(aver >membrkLimit && funcSta != SENSORGRP_FUNC_STATE_OFF)
        {
            if(ComaprePreviousPoreState(SATURATED_PORE_STATE,prevPoreState))
            {
                poreSta = SATURATED_PORE_STATE;
                funcSta = SENSORGRP_FUNC_STATE_OFF;
                changeCnt++;
                bChange = true;
            }
            prevPoreState = SATURATED_PORE_STATE;

            if(bChange && bPoreNeedToLock && lockSta != PROHIBITED_OPERATION_STATE)
            {
                lockSta = PROHIBITED_OPERATION_STATE;
            }
        }
    }

    if(changeCnt > 0)
    {
        OnCountPoreStateCnt();
        emit SigAllChDifferFuncStateSet(vctAllchFuncState); //选择128寄存器一起发送
        OnCountSensorStateCnt();
    }

    if(isVisible())
    {
        m_pPoreStateMapTabModel->UpdateAllChDataDisplay();
//        m_pPoreStateMapTabModel->ResetModel();
    }
}

bool PoreStateMapWdgt::ComaprePreviousPoreState(const PORE_STATE_ENUM &judgeState, PORE_STATE_ENUM &prevPoreState)
{
    return judgeState == prevPoreState ? true : false;
}

void PoreStateMapWdgt::ShutOffChanByMontorStdSlot(const std::vector<float> &vetStd)
{
    float stdMin = 0,stdMax = 0;
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();

    emit SigGetStdLimitInp(stdMin,stdMax);

    int changeCnt = 0;
    for(int ch = 0; ch< CHANNEL_NUM; ++ch)
    {
        auto &chFuncState = vctAllchFuncState[ch];
        if(chFuncState != SENSORGRP_FUNC_STATE_OFF)
        {
            auto &stdCh = vetStd[ch];
            if(stdCh <= stdMin ||  stdCh >= stdMax)
            {
                changeCnt++;
                chFuncState= SENSORGRP_FUNC_STATE_OFF;
            }
        }
    }
    if(changeCnt > 0)
    {
        emit SigAllChDifferFuncStateSet(vctAllchFuncState); //选择128寄存器一起发送
        OnCountSensorStateCnt();
    }
}

template <typename T>
bool PoreStateMapWdgt::CountStateNum(const std::vector<T> &vctState,std::vector<int32_t> &vctStateCntRe)
{
    for(auto &stateCnt : vctStateCntRe)
    {
        stateCnt = 0;
    }

    typename std::vector<T>::const_iterator it;
    for (it = vctState.cbegin(); it != vctState.cend(); ++it)
    {
        vctStateCntRe[*it]++;
    }
    return true;
}


void PoreStateMapWdgt::CapCalStartBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pCapCalStartBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pCapCalStartBtn->setText("Stop");
        UpdateCapLevelLimtInpt();
        double triangularAmplit = 1.0;
        emit SigTriangularStartAndGetAmp(true,triangularAmplit);
        ConfigServer::GetInstance()->SetTriangAmplitude(triangularAmplit);
        qDebug()<<"triangularAmplit"<<triangularAmplit;
        ConfigServer::GetInstance()->SetCapCollect(true);
        m_pCapCaculate->StartThread();

    }
    else
    {
        m_pCapCalStartBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pCapCalStartBtn->setText("Start");
        double triangularAmplit = 1.0;
        ConfigServer::GetInstance()->SetCapCollect(false);
        m_pCapCaculate->EndThread();
        m_pCapCaculate->StopThread();
        emit SigTriangularStartAndGetAmp(false,triangularAmplit);

    }
    m_pCaplevelLmtDpbx[CAP_LEVEL1_STATE]->setEnabled(!checked);
    m_pCaplevelLmtDpbx[CAP_LEVEL2_STATE]->setEnabled(!checked);
    m_pCaplevelLmtDpbx[CAP_LEVEL3_STATE]->setEnabled(!checked);
    m_pCaplevelLmtDpbx[CAP_LEVEL4_STATE]->setEnabled(!checked);
}


void PoreStateMapWdgt::CapResultFromCalculateSlot(const std::vector<float> &capVue)
{
    auto &vctCapState = m_pModelSharedDataPtr->GetCapStateData();
    std::transform(capVue.cbegin(), capVue.cend(), vctCapState.begin(), [&](const float &val)
    { return (CAP_STATE_ENUM)AdjudgeCapLevelByValue(val);});

    m_pPoreStateMapTabModel->UpdateCapValueModelData(capVue);
    OnCountCapStateCnt();
}

int PoreStateMapWdgt::AdjudgeCapLevelByValue(const float &val)
{
    int level = CAP_LEVEL1_STATE;
    if(val < m_capLevelLmit[CAP_LEVEL1_STATE])
    {
        level = CAP_LEVEL1_STATE;
    }else  if (val < m_capLevelLmit[CAP_LEVEL2_STATE])
    {
        level = CAP_LEVEL2_STATE;
    }
    else if (val < m_capLevelLmit[CAP_LEVEL3_STATE])
    {
        level = CAP_LEVEL3_STATE;
    }
    else if (val <= m_capLevelLmit[CAP_LEVEL4_STATE])
    {
        level = CAP_LEVEL4_STATE;
    }
    else
    {
        level = CAP_LEVEL5_STATE;
    }
    return level;
}

void PoreStateMapWdgt::UpdateCapLevelLimtInpt(void)
{
    for(int i= 0; i< CAP_STATE_NUM;++i)
    {
        m_capLevelLmit[i] = m_pCaplevelLmtDpbx[i]->value();
    }
}

bool PoreStateMapWdgt::UpdateCapLevelLimtInpt(const std::array<float,CAP_STATE_NUM> &levelParams)
{
    if(levelParams.size() != CAP_STATE_NUM)
        return false;
    for(int i = 0; i <CAP_STATE_NUM; ++i )
    {
        m_pCaplevelLmtDpbx[i]->setValue(levelParams[i]);
        m_capLevelLmit[i] = levelParams[i];
    }
    return true;
}

bool PoreStateMapWdgt::OnLockChGreaterCapLevel(const CAP_STATE_ENUM &capLevel)
{
    const float &capVueLimit =  m_capLevelLmit[capLevel];
    const auto &vctCapValue = m_pModelSharedDataPtr->GetCapVueData();
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();

    int cnt = 0;
    for(int ch =0; ch < CHANNEL_NUM; ++ch)
    {
        if(vctCapValue[ch] >= capVueLimit)
        {
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
            const auto &chSensorGrp = vctAllChSensorGrp[ch];
            vctAllChFourLockState[chSensorGrp][ch] =PROHIBITED_OPERATION_STATE;
            cnt++;
        }
    }
    if(cnt > 0)
    {
        emit SigAllChDifferFuncStateSet(vctAllchFuncState);
        OnCountLockStateCnt();
    }
    return true;

}

void PoreStateMapWdgt::OnStateModeChangeSlot(const int &index)
{
    m_pPoreStateMapTabModel->ChangeDisplayMode(index);
}


void PoreStateMapWdgt::OnDisplayContentModeChgeSlot(void)
{
    int contentMode =m_pDispContentComb->currentData().toInt();
    m_pPoreStateMapTabModel->ChangeDisplayContentMode(contentMode);
}

void PoreStateMapWdgt::OnLockGrpChgeSlot(const int &index)
{
    SENSOR_STATE_GROUP_ENUM sensorGrp = SENSOR_STATE_GROUP_ENUM(index);
    m_pPoreStateMapTabModel->ChangeSensorLockGroup(sensorGrp);
}

void PoreStateMapWdgt::OnPoreSensorGrpChgeSlot(const int &index)
{
    SENSOR_STATE_GROUP_ENUM sensorGrp = SENSOR_STATE_GROUP_ENUM(index);
    m_pPoreStateMapTabModel->ChangeSensorPoreGroup(sensorGrp);
}


void PoreStateMapWdgt::OnReizeCotentToViewSlot(bool checked)
{
    if(checked)
    {
        m_pPoreStateMapTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        m_pPoreStateMapTabView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    else
    {
        m_pPoreStateMapTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_pPoreStateMapTabView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
}

void PoreStateMapWdgt::OnClickCapSaveas(void)
{
    QFileDialog fileDialog;
    QString fileName = fileDialog.getSaveFileName(this, tr("Open File"), "CapacitanceValue", tr("Text File(*.txt);;csv File(*.csv)"));
    if (fileName == "")
    {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("error"), tr("open file failed"));
        return;
    }
    else
    {
        QTextStream textStream(&file);
        QString str = "Ch,Capacitance Value,Unit,Level,PoreState\n";
        textStream << str;
        const auto & vetCapValue = m_pModelSharedDataPtr->GetCapVueData();
        const auto & vctCapState = m_pModelSharedDataPtr->GetCapStateData();
        const auto & vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
        const auto & vctAllChFourPoreState = m_pModelSharedDataPtr->GetFourPoreStateData();
        for (int ch = 0; ch < CHANNEL_NUM; ++ch)
        {
            const auto &chSensorGrp = vctAllChSensorGrp[ch];
            textStream << "CH" << ch + 1 << "," << vetCapValue[ch] << ",pf"<<","<< gCapStateStr[vctCapState[ch]].c_str()<<","<<gPoreStateStr[vctAllChFourPoreState[chSensorGrp][ch]].c_str()<<"\n";
        }
        QMessageBox::information(this, QString::fromLocal8Bit("Tips"), QString::fromLocal8Bit("Saved File Success!"));
        file.close();
    }
}


void PoreStateMapWdgt::ClearPoreAndCapStateSlot(void)
{
//    m_pbtnPoreSateRest->click();
    m_pbtnCapSateRest->click();
    m_pSensorManualRefreshBtn->click();
}

void PoreStateMapWdgt::UpdateSensorStateSlot(void)
{
    m_pSensorManualRefreshBtn->click();
}


void PoreStateMapWdgt::OnOVFStateRefreshBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }

    std::vector<uint32_t> vctOVFreg(32,0);
    std::future<bool> future = std::async(std::launch::async, [this,&vctOVFreg]()
    {
        uchar addr = 0xD0;
        bool res = false;
        for(int i = 0; i < vctOVFreg.size();++i)
        {
            m_pUsbFtdDevice->ASICRegisterReadOne(addr+i,vctOVFreg[i],res);
            if(!res)
            {
                return false;
            }
        }
        return true;
    });

    std::future_status status;
    do {
        status = future.wait_for(std::chrono::milliseconds(1));
        QCoreApplication::processEvents();   //处理事件
    } while (status != std::future_status::ready);


    auto &vctOVFState = m_pModelSharedDataPtr->GetOVFStateData();

    for(int i = 0; i< vctOVFreg.size();++i)
    {
        auto  OVFStateBegin = vctOVFState.begin()+ i*32;
        QString valueHexStr = QString("%1").arg(vctOVFreg[i], 32 , 2, QLatin1Char('0'));
        std::transform(valueHexStr.crbegin(),valueHexStr.crend(),OVFStateBegin,[](const QChar &valChar)
        { return valChar == QChar('0') ? UN_OVER_FLOW_FLAG : OVER_FLOW_FLAG; });
    }

//    OnCountOVFStateCnt();
//    m_pPoreStateMapTabModel->UpdateAllChDataDisplay();
}

void PoreStateMapWdgt::OnOVFStateShutOffBtnSlot(void)
{
    const auto &vctOVFState = m_pModelSharedDataPtr->GetOVFStateData();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();

    for(int ch =0; ch < CHANNEL_NUM; ++ch)
    {
        if(vctOVFState[ch] == OVER_FLOW_FLAG && vctAllchFuncState[ch] != SENSORGRP_FUNC_STATE_OFF)
        {
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
        }
    }
    emit SigAllChDifferFuncStateSet(vctAllchFuncState);
}
void PoreStateMapWdgt::OnOVFStateLockOVFBtnSlot(void)
{
    const auto &vctOVFState = m_pModelSharedDataPtr->GetOVFStateData();
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();

    for(int ch =0; ch < CHANNEL_NUM; ++ch)
    {
        if(vctOVFState[ch] == OVER_FLOW_FLAG)
        {
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
            const auto &chSensorGrp = vctAllChSensorGrp[ch];
            vctAllChFourLockState[chSensorGrp][ch] =PROHIBITED_OPERATION_STATE;
        }
    }
    emit SigAllChDifferFuncStateSet(vctAllchFuncState);
}

void PoreStateMapWdgt::OnCountSensorStateCnt(void)
{
    const auto &vctSensorState =  m_pModelSharedDataPtr->GetSensorStateData();
    if(CountStateNum(vctSensorState,m_vctSensorStateCnt))
    {
        for(int i = 0; i <m_vctSensorStateCnt.size();++i )
        {
            m_plabSensorStateCnt[i]->setText(QString::number(m_vctSensorStateCnt[i]));
        }
    }
}

void PoreStateMapWdgt::OnCountPoreStateCnt(void)
{
    const auto &vctAllChFourPoreState = m_pModelSharedDataPtr->GetFourPoreStateData();
    for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
    {
        if(CountStateNum(vctAllChFourPoreState[sensorGrp],m_vctPoreStateCnt[sensorGrp]))
        {
            for(int i = 0; i <PORE_STATE_NUM;++i )
            {
                m_plabPoreStateCnt[sensorGrp][i]->setText(QString::number(m_vctPoreStateCnt[sensorGrp][i]));
            }
        }
    }
}

void PoreStateMapWdgt::OnCountLockStateCnt(void)
{
    const auto &vctAllChFourLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
    {
        if(CountStateNum(vctAllChFourLockState[sensorGrp],m_vctLockStateCnt[sensorGrp]))
        {
            int vctSize = (int)m_vctLockStateCnt[sensorGrp].size();
            for(int i = 0; i < vctSize;++i)
            {
                m_plabLockStateCnt[sensorGrp][i]->setText(QString::number(m_vctLockStateCnt[sensorGrp][i]));
            }
        }
    }
}

void PoreStateMapWdgt::OnCountOVFStateCnt(void)
{
    const auto &vctOVFState = m_pModelSharedDataPtr->GetOVFStateData();
    if(CountStateNum(vctOVFState,m_vctOVFStateCnt))
    {
        for(int i = 0; i <m_vctOVFStateCnt.size();++i )
        {
            m_plabOVFStateCnt[i]->setText(QString::number(m_vctOVFStateCnt[i]));
        }
    }
}

void PoreStateMapWdgt::OnCountCapStateCnt(void)
{
    const auto &vctCapState = m_pModelSharedDataPtr->GetCapStateData();
    if(CountStateNum(vctCapState,m_vctCapStateCnt))
    {
        for(int i = 0; i <m_vctCapStateCnt.size();++i )
        {
            m_plabCapStateCnt[i]->setText(QString::number(m_vctCapStateCnt[i]));
        }
    }
}

void PoreStateMapWdgt::OnCountValidStateCnt(void)
{
    const auto &vctAllChFourValidState = m_pModelSharedDataPtr->GetAllChFourValidStateRef();
    for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
    {
        if(CountStateNum(vctAllChFourValidState[sensorGrp],m_vctValidStateCnt[sensorGrp]))
        {
            int vctSize = (int)m_vctValidStateCnt[sensorGrp].size();
            for(int i = 0; i < vctSize;++i)
            {
                m_plabValidStateCnt[sensorGrp][i]->setText(QString::number(m_vctValidStateCnt[sensorGrp][i]));
            }
        }
    }
}

void PoreStateMapWdgt::OnCountValidStateCntForSensorGroup(const SENSOR_STATE_GROUP_ENUM &sensorGrp)
{
    const auto &vctAllChFourValidState = m_pModelSharedDataPtr->GetAllChFourValidStateRef();
    if(CountStateNum(vctAllChFourValidState[sensorGrp],m_vctValidStateCnt[sensorGrp]))
    {
        int vctSize = (int)m_vctValidStateCnt[sensorGrp].size();
        for(int i = 0; i < vctSize;++i)
        {
            m_plabValidStateCnt[sensorGrp][i]->setText(QString::number(m_vctValidStateCnt[sensorGrp][i]));
        }
    }
}



void PoreStateMapWdgt::OnCountMuxStateCnt(void)
{
    const auto &vctMuxState = m_pModelSharedDataPtr->GetAllChMuxStateRef();
    if(CountStateNum(vctMuxState,m_vctMuxStateCnt))
    {
        for(int i = 0; i <m_vctMuxStateCnt.size();++i )
        {
            m_plabMuxStateCnt[i]->setText(QString::number(m_vctMuxStateCnt[i]));
        }
    }
    OnCountValidPoreStateCntInMux();
}

/**
 * @brief PoreStateMapWdgt::OnAllChValidGenerateMuxBtnSlot 根据valid group 移位产生通道
 */
void PoreStateMapWdgt::OnAllChValidGenerateMuxBtnSlot(void)
{
    const auto &vctAllChFourValidState = m_pModelSharedDataPtr->GetAllChFourValidStateRef();
    auto &vctAllChMuxState = m_pModelSharedDataPtr->GetAllChMuxStateRef();
//    std::vector<SENSOR_STATE_GROUP_ENUM> vctAllSensorGrp {SENSOR_STATE_A_GROUP,SENSOR_STATE_B_GROUP,SENSOR_STATE_C_GROUP,SENSOR_STATE_D_GROUP};

    SENSOR_STATE_GROUP_ENUM curChSensorGrp = SENSOR_STATE_A_GROUP;
    bool bFindValid = false;

    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        curChSensorGrp = vctAllChMuxState[ch];
        bFindValid = false;
        for(int i =0; i < SENSOR_GROUP_NUM; ++i)
        {
            curChSensorGrp = (SENSOR_STATE_GROUP_ENUM) (curChSensorGrp+1 > SENSOR_STATE_D_GROUP ? SENSOR_STATE_A_GROUP : curChSensorGrp+1 );
            if(vctAllChFourValidState[curChSensorGrp][ch] == VALID_CHANNEL_ENUM)
            {
                vctAllChMuxState[ch] = curChSensorGrp;
                bFindValid = true;
                break;
            }
        }

        if(!bFindValid)
        {
            curChSensorGrp = vctAllChMuxState[ch];
            curChSensorGrp = (SENSOR_STATE_GROUP_ENUM) (curChSensorGrp+1 > SENSOR_STATE_D_GROUP ? SENSOR_STATE_A_GROUP : curChSensorGrp+1 );
            vctAllChMuxState[ch] = curChSensorGrp;
        }
    }
    OnCountMuxStateCnt();

}

/**
 * @brief PoreStateMapWdgt::OnAllChValidGenerateMuxBtnSlot 根据valid group 随机产生group
 */
void PoreStateMapWdgt::OnAllChValidGenerateMuxRandomBtnSlot(void)
{
    const auto &vctAllChFourValidState = m_pModelSharedDataPtr->GetAllChFourValidStateRef();
    auto &vctAllChMuxState = m_pModelSharedDataPtr->GetAllChMuxStateRef();
    std::vector<std::vector<SENSOR_STATE_GROUP_ENUM>> vctAllChValidGroup;

    std::random_device rd;
    std::mt19937 random(rd());

    vctAllChValidGroup.resize(CHANNEL_NUM);
    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
        {
            const auto &chValidState = vctAllChFourValidState[sensorGrp][ch];
            if(chValidState == VALID_CHANNEL_ENUM)
            {
                SENSOR_STATE_GROUP_ENUM validGrp =  SENSOR_STATE_GROUP_ENUM(sensorGrp);
                vctAllChValidGroup[ch].push_back(validGrp);
            }
        }

        if( vctAllChValidGroup[ch].size() >= 2)
        {
//            std::shuffle(vctAllChValidGroup[ch].begin(), vctAllChValidGroup[ch].end(),std::mt19937{std::random_device{}()});
            std::shuffle(vctAllChValidGroup[ch].begin(), vctAllChValidGroup[ch].end(),random);
        }
    }

    for(int ch =0; ch < CHANNEL_NUM; ++ch)
    {
        if(!vctAllChValidGroup[ch].empty())
        {
            vctAllChMuxState[ch] = vctAllChValidGroup[ch][0];
        }
        else
        {
             std::uniform_int_distribution<int> distSensorGrp(SENSOR_STATE_A_GROUP, SENSOR_STATE_D_GROUP);
             vctAllChMuxState[ch] = SENSOR_STATE_GROUP_ENUM(distSensorGrp(random));
        }
    }
    OnCountMuxStateCnt();
}

void PoreStateMapWdgt::OnValidOrNotOperateBtnSlot(void)
{
    QModelIndexList indexlist = m_pPoreStateMapTabView->selectionModel()->selectedIndexes();
    QPushButton *btn=qobject_cast<QPushButton*>(sender());
    if(indexlist.size() > 0  && btn)
    {
        SENSOR_STATE_GROUP_ENUM curValidGrp = (SENSOR_STATE_GROUP_ENUM)m_pValidTabWidgt->currentIndex();
        auto &vctAllChFourValidState = m_pModelSharedDataPtr->GetAllChFourValidStateRef();

        for(const auto &index : indexlist)
        {
            int ch = UNORDERCHANNEL_GET_BY_INDEX_ARRAY[index.row()][index.column()];
            if(btn->objectName() == "ValidBtn")
            {
                vctAllChFourValidState[curValidGrp][ch] = VALID_CHANNEL_ENUM;
            }
            else if(btn->objectName() == "InvalidBtn")
            {
                vctAllChFourValidState[curValidGrp][ch] = INVALID_CHANNEL_ENUM;
            }
        }
        OnCountValidStateCntForSensorGroup(curValidGrp);
        QMessageBox::information(this, tr("Successed!!!"), tr("Change valid state successfully !!!"));
        return;
    }
    QMessageBox::critical(this, tr("Error!!!"), tr("Change valid state failed !!!"));
}


void PoreStateMapWdgt::GenerateMuxModeSlot(void)
{
    m_pbtnAllchMuxValidState->click();
    m_pSensorPanel->OnMuxModeSet();

}
void PoreStateMapWdgt::OnRefreshCountSlot(const STATE_MAP_MODE &state)
{
    switch (state) {
    case SENSOR_STATE_MAP_MODE:
        OnSensorStateRefreshBtnSlot();
        break;
    case CAP_STATE_MAP_MODE:
        OnCountCapStateCnt();
        break;
    case PORE_STATE_MAP_MODE:
        OnCountPoreStateCnt();
        break;
    case VALID_SENSOR_STATE_MAP_MODE:
        OnCountValidStateCnt();
        break;
    case MUX_STATE_MAP_MODE:
    case OVF_STATE_MAP_MODE:
    case LOCK_SENSOR_STATE_MAP_MODE:
        break;
    default:
        break;
    }
}

bool PoreStateMapWdgt::SetOVFTimehms(void)
{
    int itm = m_pTimerOvfScanInptDpbx->value() * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmAutoOvfScan.setHMS(ihour, imin, isec, 0);
        m_plcdTimerOvfScanPeriod->display(m_tmAutoOvfScan.toString("hh:mm:ss"));

        return true;
    }
    return false;
}

void PoreStateMapWdgt::OnAutoOvfScanBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pTimerAutoOvfSScanBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pTimerAutoOvfSScanBtn->setText(QStringLiteral("Stop"));
        if(!m_pTimerToAutoOvfScan->isActive())
            m_pTimerToAutoOvfScan->start(1000);
        SetOVFTimehms();
    }
    else
    {
        m_pTimerAutoOvfSScanBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pTimerAutoOvfSScanBtn->setText(QStringLiteral("AutoScan"));
        if(m_pTimerToAutoOvfScan->isActive())
            m_pTimerToAutoOvfScan->stop();
    }

}



void PoreStateMapWdgt::OnTimeUpToAutoOvfScanSlot(void)
{
    m_tmAutoOvfScan = m_tmAutoOvfScan.addSecs(-1);
    m_plcdTimerOvfScanPeriod->display(m_tmAutoOvfScan.toString("hh:mm:ss"));
    if (m_tmAutoOvfScan == QTime(0, 0, 0, 0))
    {
        m_pTimerToAutoOvfScan->stop();

        for(int i = 0; i < m_ovfScanTimes; ++i)
        {
            m_pbtnOVFRefresh->click();
            m_pbtnOVFLock->click();
        }
        OnCountOVFStateCnt();

        SetOVFTimehms();
        m_pTimerToAutoOvfScan->start(1000);
    }
    return;

}
void PoreStateMapWdgt::OnCountValidPoreStateCntInMux(void)
{
    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFourPoreState = m_pModelSharedDataPtr->GetFourPoreStateData();
    auto &vctAllChFourValidState = m_pModelSharedDataPtr->GetAllChFourValidStateRef();
    int validcnt = 0;
    int porecnt = 0;
    for(int ch= 0; ch < CHANNEL_NUM; ++ch)
    {
        const auto &chSensorGrp = vctAllChSensorGrp[ch];
        const auto &validstate = vctAllChFourValidState[chSensorGrp][ch];
        const auto &porestate = vctAllChFourPoreState[chSensorGrp][ch];
        if(validstate == VALID_CHANNEL_ENUM)
        {
            validcnt++;
        }
        if(porestate == SINGLE_PORE_STATE)
        {
            porecnt++;
        }
    }

    m_pMuxValidCntDisplay->setText(QString("Valid :%1").arg(validcnt));
    m_pMuxSinglePoreCntDisplay->setText(QString("Single pore ::%1").arg(porecnt));
}
void PoreStateMapWdgt::EnableOVFAutoScanSlot(const float &duration,const int &scantimes,const bool &enable)
{
    if(enable)
    {
        m_pTimerOvfScanInptDpbx->setValue(duration);
        m_ovfScanTimes = scantimes;
    }
    m_pTimerAutoOvfSScanBtn->setChecked(enable);
}
