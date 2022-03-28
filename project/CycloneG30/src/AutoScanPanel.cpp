#include "AutoScanPanel.h"

#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QProgressBar>
#include <QDoubleSpinBox>
#include <QLCDNumber>
#include <QMessageBox>
#include <QProgressDialog>
#include "Global.h"
#include "Log.h"
#include "UsbFtdDevice.h"
#include "PoreStateMapWdgt.h"
#include "CustomPlotWidgt.h"
#include "PlotCfgTabModel.h"
#include "ValidOptimized.h"
#include "ModelSharedData.h"


AutoScanPanel::AutoScanPanel(QWidget *parent):QWidget(parent)
{
    InitCtrl();
}

AutoScanPanel::~AutoScanPanel()
{
    SAFE_DELETEP(m_pValidOptimized);
}

void AutoScanPanel::InitCtrl(void)
{
    m_iCycleCount = 0;
    m_bIsNextContinue = false;
    m_bIsAutoScanStop = false;


    m_pBigGrpBx = new QGroupBox(QStringLiteral("AutoScan Setting"),this);
    m_pScanGrpBx = new QGroupBox(QStringLiteral("Scan Group"),this);
    m_pScanGrpBx->setCheckable(true);
    m_pValidQcGrpBx = new QGroupBox(QStringLiteral("Valid QC"),this);
    m_pPeriodGrpBx = new QGroupBox(QStringLiteral("Period"),this);
    m_pPoreFiletGrpBx = new QGroupBox(QStringLiteral("Pore filt"),this);

    m_pSettingPreviousBtn = new QPushButton(this);
    m_pSettingNextBtn =  new QPushButton(this);
    m_pSettingPreviousBtn->setObjectName("AutoScanSetPreviousBtn");
    m_pSettingNextBtn->setObjectName("AutoScanSetNextBtn");
    m_pSettingPreviousBtn->setIcon(QIcon(":/img/img/arrow_double_left.png"));
    m_pSettingNextBtn->setIcon(QIcon(":/img/img/arrow_double_right.png"));
    m_pSettingNextBtn->setFlat(true);
    m_pSettingPreviousBtn->setFlat(true);
    m_pSettingPreviousBtn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
    m_pSettingNextBtn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);

    m_pSettingStackWgt = new QStackedWidget(this);
    m_pQCSettingWgt = new QWidget(this);
    m_pSeqSettingWgt =  new QWidget(this);

    m_pAutoStartProgresDlg = new QProgressDialog(QString("Now is PoreQc Procedure,please waitting.."), "Abort PoreQc", 0,5 , this);
    m_pAutoStartProgresDlg->reset();
    m_pAutoStartProgresDlg->hide();

    m_pSettingStackWgt->addWidget(m_pQCSettingWgt);
    m_pSettingStackWgt->addWidget(m_pSeqSettingWgt);

    QLabel *pScanDcVoltlab = new QLabel(QStringLiteral("DC volt"),this);
    QLabel *pScanAcVoltlab = new QLabel(QStringLiteral("AC volt"),this);
    QLabel *pScanTimeslab = new QLabel(QStringLiteral("Scan times"),this);
    m_pScanDcVoltDpbx = new QDoubleSpinBox(this);
    m_pScanACVoltDpbx = new QDoubleSpinBox(this);
    m_pScantimesDpbx = new QSpinBox(this);
    m_pScanProgresBar = new QProgressBar(this);
    m_pScanStartBtn =new QPushButton(QStringLiteral("Start"),this);
    m_pScanStartBtn->setCheckable(true);

    QLabel *pShuotffDcVoltlab = new QLabel(QStringLiteral("Volt:"),this);
    QLabel *m_pFilterCurlab = new QLabel(QStringLiteral("Current:"),this);
    QLabel *m_pFilterStdlab = new QLabel(QStringLiteral("Std:"),this);
    QLabel *m_pFilterWaitSeclab = new QLabel(QStringLiteral("Wait:"),this);
    m_pShuotffDcVoltDpbx= new QDoubleSpinBox(this);
    m_pFilterCurMinDpbx= new QDoubleSpinBox(this);
    m_pFilterCurMaxDpbx= new QDoubleSpinBox(this);
    m_pFilterStdMinDpbx= new QDoubleSpinBox(this);
    m_pFilterStdMaxDpbx= new QDoubleSpinBox(this);
    m_PFilterWaitSecSpbx= new QSpinBox(this);
    m_pValidFilterTimesSpbx= new QSpinBox(this);
    m_pFilterProgresBar= new QProgressBar(this);
    m_pFilterStartBtn =new QPushButton(QStringLiteral("Start"),this);
    m_pFilterStartBtn->setCheckable(true);


    QLabel *m_pPosNegaDcVoltLab = new QLabel(QStringLiteral("DegatVolt:"),this);
    QLabel *m_pDegatDelaymsLab = new QLabel(QStringLiteral("Delay:"),this);
    m_pPosNegaDcVoltDpbx= new QDoubleSpinBox(this);
    QLabel *m_pPorePrtCurrentlab = new QLabel(QStringLiteral("Pore Protect:"),this);
    m_pPoreprotCurrentDpbx= new QDoubleSpinBox(this);
    m_pDegateDelayMsSpbx = new QSpinBox(this);

    QLabel *plabCycleInp = new QLabel(QStringLiteral("Cycle"),this);
    QLabel *plabTimerPeriod = new QLabel(QStringLiteral("Timer Period"),this);
    m_pScanCycleDpbx= new QDoubleSpinBox(this);
    m_pCyclePeriodDpbx= new QDoubleSpinBox(this);
    m_plcdTimerPeriod = new QLCDNumber(8, this);
    m_plcdTimerPeriod->setSegmentStyle(QLCDNumber::Flat);
    m_plcdTimerPeriod->setFrameShape(QFrame::StyledPanel);
    m_pStartAutoScanBtn = new QPushButton(QIcon(":/img/img/start.ico"),QStringLiteral("Start"),this);
    m_pNextStepBtn = new QPushButton("Next", this);
    m_pCyclePeriodDpbx->setValue(30);
    m_pStartAutoScanBtn->setCheckable(true);
    m_pNextStepBtn->setVisible(false);

    QLabel *pDegateTimesLab = new QLabel(QStringLiteral("Times:"),this);
    m_pDegateTimesSpbx = new QSpinBox(this);
    m_pPosNegaProgresBar = new QProgressBar(this);
    m_pPosNegaStartBtn = new QPushButton(QStringLiteral("Start"),this);
    m_pPosNegaStartBtn->setCheckable(true);

    InitDegateCtrl();


    m_pScanDcVoltDpbx->setRange(0,2.5);
    m_pScanDcVoltDpbx->setDecimals(2);
    m_pScanDcVoltDpbx->setValue(0.18);
    m_pScanDcVoltDpbx->setSuffix(QStringLiteral(" V"));

    m_pScanACVoltDpbx->setRange(0,2.5);
    m_pScanACVoltDpbx->setDecimals(2);
    m_pScanACVoltDpbx->setValue(0.05);
    m_pScanACVoltDpbx->setSuffix(QStringLiteral(" V"));

    m_pScantimesDpbx->setSuffix(QStringLiteral(" cnt"));
    m_pScantimesDpbx->setValue(10);

    m_pPosNegaDcVoltDpbx->setRange(-2.5,2.5);
    m_pPosNegaDcVoltDpbx->setDecimals(2);
    m_pPosNegaDcVoltDpbx->setValue(0.18);
    m_pPosNegaDcVoltDpbx->setSuffix(QStringLiteral(" V"));

    m_pDegateTimesSpbx->setRange(1,50);
    m_pDegateTimesSpbx->setValue(3);
    m_pDegateTimesSpbx->setSuffix(QStringLiteral(" times"));

    m_pDegateDelayMsSpbx->setRange(200,5000);
    m_pDegateDelayMsSpbx->setValue(600);
    m_pDegateDelayMsSpbx->setSuffix(QStringLiteral(" ms"));

    m_pPoreprotCurrentDpbx->setRange(1,9999);
    m_pPoreprotCurrentDpbx->setDecimals(0);
    m_pPoreprotCurrentDpbx->setValue(300);
    m_pPoreprotCurrentDpbx->setSuffix(QStringLiteral(" pA"));


    m_pShuotffDcVoltDpbx->setRange(0,2.5);
    m_pShuotffDcVoltDpbx->setDecimals(2);
    m_pShuotffDcVoltDpbx->setValue(0.18);
    m_pShuotffDcVoltDpbx->setSuffix(QStringLiteral(" V"));

    m_pFilterCurMinDpbx->setDecimals(2);
    m_pFilterCurMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterCurMinDpbx->setMinimum(-999999.99);
    m_pFilterCurMinDpbx->setMaximum(999999.99);
    m_pFilterCurMinDpbx->setValue(20.0);
    m_pFilterCurMinDpbx->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>平均电流最小值</p></body></html>"));

    m_pFilterCurMaxDpbx->setDecimals(2);
    m_pFilterCurMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterCurMaxDpbx->setMinimum(-999999.99);
    m_pFilterCurMaxDpbx->setMaximum(999999.99);
    m_pFilterCurMaxDpbx->setValue(350.0);
    m_pFilterCurMaxDpbx->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>平均电流最大值</p></body></html>"));

    m_pFilterStdMinDpbx->setDecimals(4);
    m_pFilterStdMinDpbx->setMinimum(-999.9999);
    m_pFilterStdMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterStdMinDpbx->setMaximum(999.9999);
    m_pFilterStdMinDpbx->setValue(0.2);
    m_pFilterStdMinDpbx->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Std最小值</p></body></html>"));

    m_pFilterStdMaxDpbx->setDecimals(4);
    m_pFilterStdMaxDpbx->setMinimum(-999.9999);
    m_pFilterStdMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterStdMaxDpbx->setMaximum(999.9999);
    m_pFilterStdMaxDpbx->setValue(50);
    m_pFilterStdMaxDpbx->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Std最大值</p></body></html>"));
    m_PFilterWaitSecSpbx->setRange(1,1000);
    m_PFilterWaitSecSpbx->setSuffix(QStringLiteral(" s"));
    m_PFilterWaitSecSpbx->setValue(5);

    m_pValidFilterTimesSpbx->setRange(1,20);
    m_pValidFilterTimesSpbx->setSuffix(QStringLiteral(" times"));
    m_pValidFilterTimesSpbx->setValue(1);

    m_pScanCycleDpbx->setDecimals(0);
    m_pScanCycleDpbx->setRange(0,500);
    m_pScanCycleDpbx->setValue(10);

    m_pCyclePeriodDpbx->setDecimals(2);
    m_pCyclePeriodDpbx->setSuffix(" min");
    m_pCyclePeriodDpbx->setRange(0.1,360);
    m_pCyclePeriodDpbx->setValue(30);


    m_pValidOptimized = new ValidOptimized();


    QGridLayout *pscanGridlayout =new QGridLayout();
    pscanGridlayout->addWidget(pScanDcVoltlab,0,0,1,1);
    pscanGridlayout->addWidget(m_pScanDcVoltDpbx,0,1,1,1);
    pscanGridlayout->addWidget(pScanAcVoltlab,1,0,1,1);
    pscanGridlayout->addWidget(m_pScanACVoltDpbx,1,1,1,1);
    pscanGridlayout->addWidget(pScanTimeslab,2,0,1,1);
    pscanGridlayout->addWidget(m_pScantimesDpbx,2,1,1,1);
    pscanGridlayout->addWidget(m_pScanProgresBar,3,0,1,2);
    pscanGridlayout->addWidget(m_pScanStartBtn,4,0,1,1);
    pscanGridlayout->setRowStretch(5,10);
    m_pScanGrpBx->setLayout(pscanGridlayout);


    QGridLayout *pvalidgridlayout  = new QGridLayout();
    pvalidgridlayout->addWidget(pShuotffDcVoltlab,0,0,1,1);
    pvalidgridlayout->addWidget(m_pShuotffDcVoltDpbx,0,1,1,1);
    pvalidgridlayout->addWidget(m_pFilterCurlab,2,0,1,1);
    pvalidgridlayout->addWidget(m_pFilterCurMinDpbx,2,1,1,1);
    pvalidgridlayout->addWidget(m_pFilterCurMaxDpbx,2,2,1,1);
    pvalidgridlayout->addWidget(m_pFilterStdlab,3,0,1,1);
    pvalidgridlayout->addWidget(m_pFilterStdMinDpbx,3,1,1,1);
    pvalidgridlayout->addWidget(m_pFilterStdMaxDpbx,3,2,1,1);
    pvalidgridlayout->addWidget(m_pFilterWaitSeclab,4,0,1,1);
    pvalidgridlayout->addWidget(m_PFilterWaitSecSpbx,4,1,1,1);
    pvalidgridlayout->addWidget(m_pValidFilterTimesSpbx,4,2,1,1);
    pvalidgridlayout->addWidget(m_pFilterProgresBar,5,1,1,4);
    pvalidgridlayout->addWidget(m_pFilterStartBtn,5,0,1,1);
    pvalidgridlayout->setRowStretch(6,10);
    m_pValidQcGrpBx->setLayout(pvalidgridlayout);


    QGridLayout *pPorefiltGridlyout =new QGridLayout();
    pPorefiltGridlyout->addWidget(m_pPosNegaDcVoltLab,0,0,1,1);
    pPorefiltGridlyout->addWidget(m_pPosNegaDcVoltDpbx,0,1,1,1);
    pPorefiltGridlyout->addWidget(m_pDegatDelaymsLab,1,0,1,1);
    pPorefiltGridlyout->addWidget(m_pDegateDelayMsSpbx,1,1,1,1);
    pPorefiltGridlyout->addWidget(pDegateTimesLab,2,0,1,1);
    pPorefiltGridlyout->addWidget(m_pDegateTimesSpbx,2,1,1,1);
    pPorefiltGridlyout->addWidget(m_pPorePrtCurrentlab,3,0,1,1);
    pPorefiltGridlyout->addWidget(m_pPoreprotCurrentDpbx,3,1,1,1);
    pPorefiltGridlyout->addWidget(m_pPosNegaStartBtn,4,0,1,1);
    pPorefiltGridlyout->addWidget(m_pPosNegaProgresBar,4,1,1,1);
    pPorefiltGridlyout->setRowStretch(3,10);
    m_pPoreFiletGrpBx->setLayout(pPorefiltGridlyout);


    QHBoxLayout *qHQCsetlayout = new QHBoxLayout();
    qHQCsetlayout->addWidget(m_pScanGrpBx);
    qHQCsetlayout->addWidget(m_pValidQcGrpBx);
    qHQCsetlayout->addWidget(m_pPoreFiletGrpBx);
    m_pQCSettingWgt->setLayout(qHQCsetlayout);


    QHBoxLayout *phorilayout = new QHBoxLayout();
    phorilayout->addWidget(m_pgrpDegatingBase);
    phorilayout->addWidget(m_pDegatParmGrpBx);
    phorilayout->addStretch();
    m_pSeqSettingWgt->setLayout(phorilayout);


    QGridLayout *gridlayout3 = new QGridLayout();
    gridlayout3->addWidget(plabTimerPeriod,0,0,1,1);
    gridlayout3->addWidget(m_pCyclePeriodDpbx,0,1,1,1);
    gridlayout3->addWidget(plabCycleInp,1,0,1,1);
    gridlayout3->addWidget(m_pScanCycleDpbx,1,1,1,1);
    gridlayout3->addWidget(m_plcdTimerPeriod,2,0,1,2);
    gridlayout3->addWidget(m_pNextStepBtn,3,0,1,1);
    gridlayout3->addWidget(m_pStartAutoScanBtn,3,1,1,1);
    m_pPeriodGrpBx->setLayout(gridlayout3);


    QHBoxLayout *phorilayout1 = new QHBoxLayout();
    phorilayout1->addWidget(m_pSettingPreviousBtn);
    phorilayout1->addWidget(m_pSettingStackWgt);
    phorilayout1->addWidget(m_pSettingNextBtn);
    phorilayout1->addStretch();
    m_pBigGrpBx->setLayout(phorilayout1);

    QHBoxLayout *pbighorlayouy = new QHBoxLayout();
    pbighorlayouy->addWidget(m_pBigGrpBx);
    pbighorlayouy->addWidget(m_pPeriodGrpBx);
    pbighorlayouy->addStretch();
    setLayout(pbighorlayouy);

    SetTimehms();

    connect(m_pStartAutoScanBtn,&QPushButton::clicked,this,&AutoScanPanel::OnClickAutoScanStart);
    connect(&m_tmrAutoScanStart,&QTimer::timeout,this,&AutoScanPanel::OnSecondTimerTimeoutSlot);

    connect(m_pSettingPreviousBtn,&QPushButton::clicked,[=]()
    {
        int indexPage = m_pSettingStackWgt->currentIndex();
        if(indexPage == AUTOSCAN_SETTING_OF_SEQ_PAGE)
        {
            m_pSettingStackWgt->setCurrentIndex(AUTOSCAN_SETTING_OF_VALID_QC_PAGE);
        }
    });

    connect(m_pSettingNextBtn,&QPushButton::clicked,[=]()
    {
        int indexPage = m_pSettingStackWgt->currentIndex();
        if (indexPage == AUTOSCAN_SETTING_OF_VALID_QC_PAGE)
        {
            m_pSettingStackWgt->setCurrentIndex(AUTOSCAN_SETTING_OF_SEQ_PAGE);
        }
    });

    connect(m_pScanStartBtn,&QPushButton::toggled,this,&AutoScanPanel::OnScanStartBtnSlot);
    connect(m_pFilterStartBtn,&QPushButton::toggled,this,&AutoScanPanel::OnFilterStartBtnSlot);
    connect(m_pPosNegaStartBtn,&QPushButton::toggled,this,&AutoScanPanel::OnPosNegaStartBtnSlot);


}


void AutoScanPanel::InitDegateCtrl(void)
{
    m_pgrpDegatingBase = new QGroupBox("Degating Base", this);
    m_pDegatParmGrpBx = new QGroupBox(QStringLiteral("Degating Advanced"),this);

    m_pgrpDegatingBase->setContentsMargins(3, 1, 3, 0);
    m_plabCyclePeriod = new QLabel("Cycle Period:", this);
    m_pispCyclePeriod = new QSpinBox(this);
    m_pispCyclePeriod->setSuffix(" s");
    m_pispCyclePeriod->setMinimum(1);
    m_pispCyclePeriod->setMaximum(99);
    m_pispCyclePeriod->setSingleStep(1);
    m_pispCyclePeriod->setValue(2);
    m_pispCyclePeriod->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置多久（几秒）检测一次gating状态</p></body></html>"));

    m_plabDurationTime = new QLabel("Duration Time:", this);
    m_pispDurationTime = new QSpinBox(this);
    m_pispDurationTime->setSuffix(" ms");
    m_pispDurationTime->setMinimum(100);
    m_pispDurationTime->setMaximum(10000);
    m_pispDurationTime->setSingleStep(100);
    m_pispDurationTime->setValue(200);
    m_pispDurationTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置施加Degating电压持续时间</p></body></html>"));


    m_plabAllowTimes = new QLabel("Allow Times:", this);
    m_pispAllowTimes = new QSpinBox(this);
    m_pispAllowTimes->setMinimum(0);
    m_pispAllowTimes->setMaximum(100);
    m_pispAllowTimes->setSingleStep(1);
    m_pispAllowTimes->setValue(10);
    m_pispAllowTimes->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间内的Degating次数，规定时间内达到此次数则关断该通道。0为表示此功能不开启</p></body></html>"));

    m_pchkAutoseqGating = new QCheckBox("Autoseq", this);
    m_pchkAutoseqGating->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否启用AutoseqGating算法</p></body></html>"));

    m_plabDurationThres = new QLabel("Durate Thres:", this);
    m_pdspDurationThres = new QDoubleSpinBox(this);
    m_pdspDurationThres->setDecimals(2);
    m_pdspDurationThres->setMinimum(0.2);
    m_pdspDurationThres->setMaximum(1);
    m_pdspDurationThres->setValue(0.1);
    m_pdspDurationThres->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>最大值不能超过1，如果有某段信号0.5s一致维持在同一个水平不变，那就认为是gating状态</p></body></html>"));

    m_plabGatingSTD = new QLabel("Seq STD Min:", this);
    m_pispGatingSTD = new QSpinBox(this);
    m_pispGatingSTD->setMinimum(0);
    m_pispGatingSTD->setMaximum(100);
    m_pispGatingSTD->setValue(8);
    m_pispGatingSTD->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>小于该STD min值，则认为是gating状态</p></body></html>"));

    m_plabGatingSTDMax = new QLabel("Seq STD Max:", this);
    m_pispGatingSTDMax = new QSpinBox(this);
    m_pispGatingSTDMax->setMinimum(0);
    m_pispGatingSTDMax->setMaximum(50000);
    m_pispGatingSTDMax->setValue(18);
    m_pispGatingSTDMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>大于该STD max值，则认为是gating状态</p></body></html>"));

    m_plabSignalMin = new QLabel("Signal Min:", this);
    m_pispSignalMin = new QSpinBox(this);
    m_pispSignalMin->setMinimum(-200);
    m_pispSignalMin->setMaximum(500);
    m_pispSignalMin->setValue(1);
    m_pispSignalMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，小于该值为非gating,大于此值才进一步作gating分析</p></body></html>"));

    m_plabSignalMax = new QLabel("Signal Max:", this);
    m_pispSignalMax = new QSpinBox(this);
    m_pispSignalMax->setMinimum(0);
    m_pispSignalMax->setMaximum(500);
    m_pispSignalMax->setValue(300);
    m_pispSignalMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，大于该值为非gating,小于此值才进一步作gating分析</p></body></html>"));

    m_plabGatingMin = new QLabel("Gating Min:", this);
    m_pispGatingMin = new QSpinBox(this);
    m_pispGatingMin->setMinimum(-20);
    m_pispGatingMin->setMaximum(500);
    m_pispGatingMin->setValue(1);
    m_pispGatingMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，大于此值作gating分析</p></body></html>"));

    m_plabGatingMax = new QLabel("Gating Max:", this);
    m_pispGatingMax = new QSpinBox(this);
    m_pispGatingMax->setMinimum(0);
    m_pispGatingMax->setMaximum(500);
    m_pispGatingMax->setValue(180);
    m_pispGatingMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，小于此值作gating分析</p></body></html>"));

    int row = 0;
    int col = 0;
    QGridLayout* gdlayout1 = new QGridLayout();
    gdlayout1->setContentsMargins(1, 8, 1, 1);
    col = 0;
    gdlayout1->addWidget(m_plabCyclePeriod, row, col++);
    gdlayout1->addWidget(m_pispCyclePeriod, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabDurationTime, row, col++);
    gdlayout1->addWidget(m_pispDurationTime, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabAllowTimes, row, col++);
    gdlayout1->addWidget(m_pispAllowTimes, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_pchkAutoseqGating, row, col++);
    QVBoxLayout* verlayout4 = new QVBoxLayout();
    verlayout4->addLayout(gdlayout1);
    m_pgrpDegatingBase->setLayout(verlayout4);



    int cnt = 0;
    QGridLayout* gdlayout2 = new QGridLayout();
    gdlayout2->setContentsMargins(1, 8, 1, 1);
    gdlayout2->addWidget(m_plabDurationThres, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTD, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTDMax, cnt++, 0);
    gdlayout2->addWidget(m_plabSignalMin, cnt++, 0);
    cnt = 0;
    gdlayout2->addWidget(m_pdspDurationThres, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTD, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTDMax, cnt++, 1);
    gdlayout2->addWidget(m_pispSignalMin, cnt++, 1);
    cnt = 0;
    gdlayout2->addWidget(m_plabSignalMax, cnt++, 2);
    gdlayout2->addWidget(m_plabGatingMin, cnt++, 2);
    gdlayout2->addWidget(m_plabGatingMax, cnt++, 2);

    cnt = 0;
    gdlayout2->addWidget(m_pispSignalMax, cnt++, 3);
    gdlayout2->addWidget(m_pispGatingMin, cnt++, 3);
    gdlayout2->addWidget(m_pispGatingMax, cnt++, 3);
    gdlayout2->setRowStretch(5,10);
    m_pDegatParmGrpBx->setLayout(gdlayout2);

}


bool AutoScanPanel::SetTimehms(void)
{
    int itm = m_pCyclePeriodDpbx->value() * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmAutoScanStart.setHMS(ihour, imin, isec, 0);
        m_plcdTimerPeriod->display(m_tmAutoScanStart.toString("hh:mm:ss"));

        return true;
    }
    return false;
}


void AutoScanPanel::OnClickAutoScanStart(const bool &checked)
{
    bool saveDirIsEmpty = false;
    emit GetDataSaveDirIsEmptySig(saveDirIsEmpty);
    if(saveDirIsEmpty)
    {
         m_pStartAutoScanBtn->setChecked(false);
         QMessageBox::warning(this, tr("Warning"), tr("Please set your save directory!!!"));
         return;
    }
    m_pStartAutoScanBtn->setEnabled(false);
    if (checked)
    {
        LOGI("Autoscan Start....");
        m_iCycleCount = 0;
        m_pStartAutoScanBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pStartAutoScanBtn->setText(QStringLiteral("Stop"));
        SetIsAutoScanStop(false);

        m_tmAutoScanStart.setHMS(0, 0, 1, 0);

//        emit NotifyRecordOperateSig(RECORD_READS_ADAPTERS_FILE,RECORD_FILE_OPEN);
//        emit NotifyRecordOperateSig(RECORD_VALID_FILE,RECORD_FILE_OPEN);
        if (!m_tmrAutoScanStart.isActive())
        {
            m_tmrAutoScanStart.start(1000);
        }
    }
    else
    {
        auto ret = QMessageBox::Yes;
        if (m_iCycleCount <= m_pScanCycleDpbx->value())
        {
            ret = QMessageBox::question(this, "Confirm Stop", "Really want to Stop Autoscan?");
        }
        if (ret == QMessageBox::Yes)
        {
            LOGI("Autoscan Stop....");
            m_bIsNextContinue = true;
            if (m_tmrAutoScanStart.isActive())
            {
                m_tmrAutoScanStart.stop();
            }


            m_iCycleCount = 0;

            m_pStartAutoScanBtn->setIcon(QIcon(":/img/img/start.ico"));
            m_pStartAutoScanBtn->setText(QStringLiteral("Start"));
            SetIsAutoScanStop(true);
            emit SetDataSaveSignal(false);
            ConfigServer::GetInstance()->SetNeedGenerateRunId(true);
//            emit NotifyRecordOperateSig(RECORD_READS_ADAPTERS_FILE,RECORD_FILE_CLOSE);
//            emit NotifyRecordOperateSig(RECORD_VALID_FILE,RECORD_FILE_CLOSE);
        }
        else
        {
            m_pStartAutoScanBtn->setChecked(true);
        }
    }
    m_pStartAutoScanBtn->setEnabled(true);
}

void AutoScanPanel::OnSecondTimerTimeoutSlot(void)
{
    m_tmAutoScanStart = m_tmAutoScanStart.addSecs(-1);
    m_plcdTimerPeriod->display(m_tmAutoScanStart.toString("hh:mm:ss"));
    bool bIsAutoTimerDegatRunning = false;
    if (m_tmAutoScanStart == QTime(0, 0, 0, 0))
    {
        ++m_iCycleCount;

        if (m_iCycleCount > m_pScanCycleDpbx->value())
        {
            if (m_pStartAutoScanBtn->isChecked())
            {
                m_pStartAutoScanBtn->click();
            }
            LOGI("Time Period QC Seqpuencing Reach Total Cycle Auto Stop!");
            return;
        }
        m_pStartAutoScanBtn->setText(QString("Stop (%1)").arg(m_pScanCycleDpbx->value() - m_iCycleCount));

        emit AcquireAutoTimerDegatIsRunningSig(bIsAutoTimerDegatRunning);
        if(bIsAutoTimerDegatRunning)
        {
            emit AutoTimerDegatControlSig(false); //先暂停定时degating的计时
        }

        emit SetDataSaveSignal(false);
        emit NotifyRecordOperateSig(RECORD_READS_ADAPTERS_FILE,RECORD_FILE_RECORD);

        SetTimehms();

        int progresscnt = 0;
        OnStopAllActivity();
        OnResetProgressBar();
        m_pAutoStartProgresDlg->setWindowModality(Qt::ApplicationModal);
        m_pAutoStartProgresDlg->reset();
        m_pAutoStartProgresDlg->setValue(0);
        m_pAutoStartProgresDlg->show();

        emit EnablePoreProtectSig(false,m_pPoreprotCurrentDpbx->value());

        bool bIsDegatRunning = false;
        emit AcquireDegateIsRunningSig(bIsDegatRunning);
        emit TempControlDegateSig(false);

        m_pAutoStartProgresDlg->setValue(progresscnt++);
        m_pAutoStartProgresDlg->setLabelText(QStringLiteral("SCAN Sensor Group!!"));
        if (m_pAutoStartProgresDlg->wasCanceled()) { m_pAutoStartProgresDlg->reset(); goto stop; }
        if(m_pScanGrpBx->isChecked())
        {
            m_pScanStartBtn->click();
        }

        m_pAutoStartProgresDlg->setValue(progresscnt++);
        m_pAutoStartProgresDlg->setLabelText(QStringLiteral("Valid Channel QC and Generate Mux channels!!"));
        if (m_pAutoStartProgresDlg->wasCanceled()) { m_pAutoStartProgresDlg->reset(); goto stop; }
        m_pFilterStartBtn->click();
        emit NotifyRecordOperateSig(RECORD_VALID_FILE,RECORD_FILE_RECORD);

        m_pAutoStartProgresDlg->setValue(progresscnt++);
        m_pAutoStartProgresDlg->setLabelText(QStringLiteral("Unblock all channel several times and enable pore protect!!"));
        if (m_pAutoStartProgresDlg->wasCanceled()) { m_pAutoStartProgresDlg->reset(); goto stop; }
        m_pPosNegaStartBtn->click();


        m_pAutoStartProgresDlg->setValue(progresscnt++);
        m_pAutoStartProgresDlg->setLabelText(QStringLiteral("Enable Degating and start Save!!!"));
        if (m_pAutoStartProgresDlg->wasCanceled()) { m_pAutoStartProgresDlg->reset(); goto stop; }
        emit UpdateDegateBaseParametersSig(m_pispCyclePeriod->value(),m_pispDurationTime->value(),m_pispAllowTimes->value(),m_pchkAutoseqGating->isChecked());
        emit UpdateDegatingParametersSig(m_pdspDurationThres->value(),m_pispGatingSTD->value(),m_pispGatingSTDMax->value(),m_pispSignalMin->value(),\
                                         m_pispSignalMax->value(),m_pispGatingMin->value(),m_pispGatingMax->value());
        emit TempControlDegateSig(true);
        ConfigServer::GetInstance()->SetNeedGenerateRunId(m_iCycleCount >1 ? false : true);
        emit SetDataSaveSignal(true);

        m_pAutoStartProgresDlg->setMaximum(m_pAutoStartProgresDlg->maximum());
        m_pAutoStartProgresDlg->reset();

        if(bIsAutoTimerDegatRunning)
        {
            emit AutoTimerDegatControlSig(true); //恢复定时degating的计时
        }

    }
    return;
stop:
    if (m_pStartAutoScanBtn->isChecked())
    {
        m_pStartAutoScanBtn->click();
    }
    if(bIsAutoTimerDegatRunning)
    {
        emit AutoTimerDegatControlSig(true); //恢复定时degating的计时
    }
    SetIsAutoScanStop(true);
    OnStopAllActivity();
}


void AutoScanPanel::OnScanStartBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pScanStartBtn->setText(QStringLiteral("Stop"));
        m_bScanLockStop = false;
        StartScanChannelProcedure(m_bScanLockStop);
        m_pScanStartBtn->setChecked(false);
    }
    else
    {
        m_pScanStartBtn->setText(QStringLiteral("Start"));
        m_pPoreStateMapWdgt->OnCancelScanProgress();
        m_bScanLockStop = true;
    }
}


bool AutoScanPanel::StartScanChannelProcedure(const bool &bStop)
{
    m_pScanProgresBar->setRange(0,SENSOR_GROUP_NUM * 2);
    m_pScanProgresBar->reset();
    bool bSucceed= true;
    int cnt = 0;
    for(int sensorgrp = 0; sensorgrp< SENSOR_GROUP_NUM; ++sensorgrp)
    {
        SENSOR_STATE_GROUP_ENUM sensorGroup = SENSOR_STATE_GROUP_ENUM(sensorgrp);
        emit SensorGroupChangeSig(sensorGroup);
        m_pScanProgresBar->setValue(cnt++);
        emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,m_pScanDcVoltDpbx->value(),0);
        m_pPoreStateMapWdgt->SetLockScanGroup(SENSOR_STATE_GROUP_ENUM(sensorgrp),m_pScantimesDpbx->value());
        if(!m_pPoreStateMapWdgt->OnLockScanForAllCh() || bStop)
        {
            bSucceed = false;
            break;
        }

        m_pScanProgresBar->setValue(cnt++);
        emit VoltageSetSig(VOLTAGE_TRIANGULAR_ENUM,m_pScanACVoltDpbx->value(),0);
        if(!m_pPoreStateMapWdgt->OnLockScanForAllCh() || bStop)
        {
            bSucceed = false;
            break;
        }
    }
    m_pScanProgresBar->setValue((bSucceed ?  m_pScanProgresBar->maximum() : m_pScanProgresBar->value()));
    return bSucceed;
}


void AutoScanPanel::OnFilterStartBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pFilterStartBtn->setText(QStringLiteral("Stop"));
        m_pValidFilterTimesSpbx->setEnabled(false);
        m_bFiterValidStop = false;
        StartFilterValidProcedure(m_bFiterValidStop);
    }
    else
    {
        qDebug()<<__FUNCTION__<<checked;
        m_pFilterStartBtn->setText(QStringLiteral("Start"));
        m_bFiterValidStop = true;
        m_pValidFilterTimesSpbx->setEnabled(true);
    }
    return;
}



//flow procedure filter valid
bool AutoScanPanel::StartFilterValidProcedure(const bool &bStop)
{
    int validtimes = m_pValidFilterTimesSpbx->value();
    m_pValidOptimized->ResetTimes();
    bool isCancel =false;
    ConfigServer::WidgetPosAdjustByParent(this->parentWidget()->parentWidget(), m_pCusPlotCurvesCfgDlg);
    m_pCusPlotCurvesCfgDlg->show();
    const int totalCnt = SENSOR_GROUP_NUM * validtimes;
    int cnt = 0;
    m_pFilterProgresBar->setRange(0,totalCnt);
    m_pFilterProgresBar->reset();
    for(int curTime = 0; curTime < validtimes; ++curTime)
    {
        if(bStop)
        {
            isCancel = true;
            break;
        }

        emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,m_pShuotffDcVoltDpbx->value(),0);

        m_pCusPlotCurvesCfgDlg->SetCurFiltType(PlotCfgSortFilterModel::FILTER_TYPE_AVG_STD);
        m_pCusPlotCurvesCfgDlg->UpdateAvgStdParameterSlot(m_pFilterCurMinDpbx->value(),m_pFilterCurMaxDpbx->value(),m_pFilterStdMinDpbx->value(),m_pFilterStdMaxDpbx->value());

        for(int sensorgrp = 0; sensorgrp< SENSOR_GROUP_NUM; ++sensorgrp)
        {
            if(bStop)
            {
                isCancel = true;
                break;
            }
            SENSOR_STATE_GROUP_ENUM sensorGroup = SENSOR_STATE_GROUP_ENUM(sensorgrp);
            emit SensorGroupChangeSig(sensorGroup);
            m_pFilterProgresBar->setValue(cnt++);

//            ConfigServer::caliSleep(m_PFilterWaitSecSpbx->value() * 1000);
             ConfigServer::SleepForTimeoutOrStop(m_PFilterWaitSecSpbx->value() * 1000,bStop);

            m_pCusPlotCurvesCfgDlg->OnValidApplyBtnSlot();
        }

        if(!isCancel)
        {
            //存储valid筛选的结果，存储起来；
            const auto &vctValidState = m_pCusPlotCurvesCfgDlg->GetModelShareDataPt()->GetAllChFourValidStateRef();
            const auto vctValidCnt = m_pPoreStateMapWdgt->GetValidStateCntVct();
            m_pValidOptimized->StoreValidState(curTime,vctValidState,vctValidCnt);
        }
    }
    m_pFilterProgresBar->setValue((isCancel ? m_pFilterProgresBar->value() : m_pFilterProgresBar->maximum()));
    m_pFilterStartBtn->setChecked(false);
    if(!isCancel)
    {
        //根据valid多次筛选，选择最佳的组合
        auto &vctValidStat = m_pCusPlotCurvesCfgDlg->GetModelShareDataPt()->GetAllChFourValidStateRef();
        m_pValidOptimized->MultiValidFlitOptimize(vctValidStat);
        emit GenerateMuxModeSig();
        //关闭非valid通道
        m_pCusPlotCurvesCfgDlg->OnFiltValidShutOtherBtnSlot();
        m_pPoreStateMapWdgt->OnRefreshCountSlot(VALID_SENSOR_STATE_MAP_MODE);
    }
    m_pCusPlotCurvesCfgDlg->hide();
    return !isCancel;
}

void AutoScanPanel::OnPosNegaStartBtnSlot(const bool &checked)
{
    if(checked)
    {
        emit EnablePoreProtectSig(false,m_pPoreprotCurrentDpbx->value());
        int degateTimes = m_pDegateTimesSpbx->value();
        m_pPosNegaProgresBar->setRange(0,degateTimes);
        m_pPosNegaProgresBar->reset();
        m_pPosNegaStartBtn->setText(QStringLiteral("Stop"));
        bool bisCancel =false;

        emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,qAbs(m_pPosNegaDcVoltDpbx->value()),0);
        emit DegatVoltSetSig(-qAbs(m_pPosNegaDcVoltDpbx->value()));

        int cnt = 0;
        for(int i = 0; i< degateTimes; ++i)
        {
            if(!m_pPosNegaStartBtn->isChecked() || GetIsAutoScanStop())
            {
                bisCancel = true;
                break;
            }
            emit OnUnblockAllChannelSig(m_pDegateDelayMsSpbx->value());
            m_pPosNegaProgresBar->setValue(cnt++);
        }

        m_pPosNegaStartBtn->setChecked(false);
        m_pPosNegaProgresBar->setValue((bisCancel ? m_pPosNegaProgresBar->value() : m_pPosNegaProgresBar->maximum()));
        emit EnablePoreProtectSig(!bisCancel,m_pPoreprotCurrentDpbx->value());

    }
    else
    {
        m_pPosNegaStartBtn->setText(QStringLiteral("Start"));
    }
}

void AutoScanPanel::OnStopAllActivity(void)
{
    m_bScanLockStop  = true;
    m_bFiterValidStop = true;
    m_pPoreStateMapWdgt->OnCancelScanProgress();

    if(m_pScanStartBtn->isChecked())
    {
        m_pScanStartBtn->click();
    }

    if(m_pFilterStartBtn->isChecked())
    {
        m_pFilterStartBtn->click();
    }

    if(m_pPosNegaStartBtn->isChecked())
    {
        m_pPosNegaStartBtn->click();
    }
}

void AutoScanPanel::OnResetProgressBar(void)
{
   m_pScanProgresBar->reset();
   m_pFilterProgresBar->reset();
   m_pPosNegaProgresBar->reset();
}


void AutoScanPanel::AutoScanControlPauseSlot(const bool &runOrPause)
{
    if(runOrPause) //true  control run
    {
        if(m_pStartAutoScanBtn->isChecked() && !m_tmrAutoScanStart.isActive())
        {
            m_tmrAutoScanStart.start(1000);  //continue
        }
    }
    else
    {
        if(m_pStartAutoScanBtn->isChecked() && m_tmrAutoScanStart.isActive())
        {
            m_tmrAutoScanStart.stop();  //continue
        }

    }
}
void AutoScanPanel::AcquireAutoScanIsRunningSlot(bool &bIsRunning)
{
    bIsRunning =(m_pStartAutoScanBtn->isChecked() && m_tmrAutoScanStart.isActive()) ;
}

void AutoScanPanel::SetScanLockParams(const float &dcVolt,const float &acVolt,const int &times)
{
    m_pScanDcVoltDpbx->setValue(dcVolt);
    m_pScanACVoltDpbx->setValue(acVolt);
    m_pScantimesDpbx->setValue(times);
}

void AutoScanPanel::SetFilterValidParams(const float &dcVolt,const float &curMin,const float &curMax,\
                          const float &stdMin,const float &stdMax,const int &wait,const int times)
{
    m_pShuotffDcVoltDpbx->setValue(dcVolt);
    m_pFilterCurMinDpbx->setValue(curMin);
    m_pFilterCurMaxDpbx->setValue(curMax);
    m_pFilterStdMinDpbx->setValue(stdMin);
    m_pFilterStdMaxDpbx->setValue(stdMax);
    m_PFilterWaitSecSpbx->setValue(wait);
    m_pValidFilterTimesSpbx->setValue(times);
}

