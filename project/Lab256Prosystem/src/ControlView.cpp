#include "ControlView.h"
#include <qlayout.h>
#include <qgroupbox.h>
#include <qscrollarea.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>

#include <Log.h>
#include <clustering.h>

#include "UsbProtoMsg.h"
#include "MyQMessageBox.h"
#include "ZeroAdjustDialog.h"
#include "DegatingDialog.h"
#include "HttpClient.h"


ControlView::ControlView(QWidget *parent) : QWidget(parent)
{
    InitFlowCtrl();
	InitCurrentCtrl();
	InitVoltageCtrl();
    InitControlList();
    InitLayout();
	LayoutCtrl();

    m_pProgressDialog = new QProgressDialog("Zero Adjust in progress....", "Abort", 0, 100, this);
    m_pProgressDialog->reset();
    m_pProgressDialog->setModal(true);

    m_vetAverageData.resize(CHANNEL_NUM);
    m_vetStdData.resize(CHANNEL_NUM);
    m_vetDegatingData.resize(CHANNEL_NUM);

    connect(&m_tmrCur, SIGNAL(timeout()), this, SLOT(timerCurCurrentSlot()));
    connect(&m_tmrDegating, SIGNAL(timeout()), this, SLOT(timerDegatingSlot()));
    connect(&m_tmrAvg, SIGNAL(timeout()), this, SLOT(timerAvgCurrentSlot()));
    //m_timerStd.setSingleShot(true);
    connect(&m_tmrStd, SIGNAL(timeout()), this, SLOT(timerStdCurrentSlot()));
    connect(&m_tmrAutoseqGating, SIGNAL(timeout()), this, SLOT(onTimerAutoseqGating()));

    connect(&m_tmrAutoZero, SIGNAL(timeout()), this, SLOT(timerAutoZeroSlot()));
    connect(&m_tmrAutoProtect, SIGNAL(timeout()), this, SLOT(timerAutoProtectSlot()));

    //m_tmrCalculation.setSingleShot(true);
    connect(&m_tmrCalculation, SIGNAL(timeout()), this, SLOT(onTimerCalculationStart()));
    connect(&m_tmrAdjust, &QTimer::timeout, this, &ControlView::onTimerZeroAdjustStart);
    connect(&m_tmrPolymer, SIGNAL(timeout()), this, SLOT(onTimerPolymerStart()));
    connect(&m_tmrRotation, SIGNAL(timeout()), this, SLOT(onTimerRotationStart()));
    connect(&m_tmrSimulation, SIGNAL(timeout()), this, SLOT(onTimerSimulationStart()));

    //Flow2
    connect(&m_tmrMembraneForm, SIGNAL(timeout()), this, SLOT(onTimerMakeMembraneStartCapTest()));
    connect(&m_tmrPoreInsert, SIGNAL(timeout()), this, SLOT(onTimerPoreInsertStart()));
    connect(&m_tmrSeqZeroAdjust, SIGNAL(timeout()), this, SLOT(onTimerSequencingZeroAdjust()));
    //connect(&m_tmrSequencingFilter, SIGNAL(timeout()), this, SLOT(onTimerSequencingFilter()));
    connect(&m_tmrMuxScanStart, SIGNAL(timeout()), this, SLOT(onTimerMuxScanStart()));
    //connect(&m_tmrMuxScanFilter, SIGNAL(timeout()), this, SLOT(onTimerMuxScanFilter()));
    connect(&m_tmrRecordDegating, SIGNAL(timeout()), this, SLOT(onTimerRecordDegating()));
}

ControlView::~ControlView()
{
    EndThread();
    EndThread2();

    if (m_tmrCur.isActive())
    {
        m_tmrCur.stop();
    }
    if (m_tmrDegating.isActive())
    {
        m_tmrDegating.stop();
    }
    if (m_tmrAvg.isActive())
    {
        m_tmrAvg.stop();
    }
    if (m_tmrStd.isActive())
    {
        m_tmrStd.stop();
    }
    if (m_tmrCalculation.isActive())
    {
        m_tmrCalculation.stop();
    }
    if (m_tmrAdjust.isActive())
    {
        m_tmrAdjust.stop();
    }
    if (m_tmrAutoZero.isActive())
    {
        m_tmrAutoZero.stop();
    }
    if (m_tmrPolymer.isActive())
    {
        m_tmrPolymer.stop();
    }
    if (m_tmrRotation.isActive())
    {
        m_tmrRotation.stop();
    }
    if (m_tmrSimulation.isActive())
    {
        m_tmrSimulation.stop();
    }
    if (m_tmrMembraneForm.isActive())
    {
        m_tmrMembraneForm.stop();
    }
    if (m_tmrPoreInsert.isActive())
    {
        m_tmrPoreInsert.stop();
    }
    if (m_tmrSeqZeroAdjust.isActive())
    {
        m_tmrSeqZeroAdjust.stop();
    }
    if (m_tmrMuxScanStart.isActive())
    {
        m_tmrMuxScanStart.stop();
    }
    if (m_tmrRecordDegating.isActive())
    {
        m_tmrRecordDegating.stop();
    }

    StopThread();
    StopThread2();

    m_vetControlListItem.clear();
}

bool ControlView::InitFlowCtrl(void)
{
    {
        m_grpFlowset = new QGroupBox("Config", this);

        m_pbtnLoadConfig = new QPushButton("Load Config...", m_grpFlowset);
        m_pbtnLoadConfig->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>加载已保存的配置</p></body></html>"));

        m_pbtnSaveConfig = new QPushButton("Save Config...", m_grpFlowset);
        m_pbtnSaveConfig->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>保存当前的配置</p></body></html>"));
        //m_pbtnLoadConfig->setVisible(false);
        //m_pbtnSaveConfig->setVisible(false);


        //chk
        m_pchkAllSeq1 = new QCheckBox("AllSeq", m_grpFlowset);
        m_pchkAllSeq1->setChecked(true);
        m_pchkAllSeq1->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>勾选当前页面所有测序通道</p></body></html>"));

        m_pchkShowSeq1 = new QCheckBox("ShowSeq", m_grpFlowset);
        m_pchkShowSeq1->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>只显示勾选测序通道；如果勾选了保持显示复选框，则当前显示的通道无论勾选与否都不会被隐藏</p></body></html>"));

        m_pchkAllVisible1 = new QCheckBox("AllVisible", m_grpFlowset);
        m_pchkAllVisible1->setChecked(true);
        m_pchkAllVisible1->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>勾选当前页面所有通道</p></body></html>"));

        m_pchkShowVisible1 = new QCheckBox("ShowVisible", m_grpFlowset);
        m_pchkShowVisible1->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>只显示勾选通道；如果勾选了保持显示复选框，则当前显示的通道无论勾选与否都不会被隐藏</p></body></html>"));

        //count
        m_plabShowCount = new QLabel(QString::number(CHANNEL_NUM), m_grpFlowset);
        m_plabShowCount->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>显示符合筛选条件的通道个数</p></body></html>"));


        //config
        QVBoxLayout* verlayout1 = new QVBoxLayout();
        verlayout1->addWidget(m_pbtnLoadConfig);
        verlayout1->addWidget(m_pbtnSaveConfig);
        verlayout1->addStretch(5);
        verlayout1->addWidget(m_pchkAllVisible1);
        verlayout1->addWidget(m_pchkShowVisible1);
        verlayout1->addWidget(m_pchkAllSeq1);
        verlayout1->addWidget(m_pchkShowSeq1);
        verlayout1->addStretch(1);
        verlayout1->addWidget(m_plabShowCount);

        m_grpFlowset->setLayout(verlayout1);
    }


    m_ptabFlow = new QTabWidget(this);
    QWidget* pchanqc = new QWidget(this);
    QWidget* pmakemb = new QWidget(this);
    QWidget* pinsert = new QWidget(this);
    QWidget* pholeqc = new QWidget(this);
    QWidget* psequen = new QWidget(this);
    QWidget* psequqc = new QWidget(this);

    m_ptabFlow->addTab(pchanqc, "ChipQC");
    m_ptabFlow->addTab(pmakemb, "MembraneFormation");//MakeMembrane
    m_ptabFlow->addTab(pinsert, "PoreInsertion");
    m_ptabFlow->addTab(pholeqc, "FactoryPoreQC");//InitPoreQC//OrignalPoreQC
    //m_ptabFlow->addTab(psequen, "Sequencing");
    m_ptabFlow->addTab(psequqc, "MuxScan");

    psequen->setVisible(false);

    //ChannelQC
    {
        m_plabVolt1 = new QLabel("Volt1", pchanqc);

        m_pdspVolt1 = new QDoubleSpinBox(pchanqc);
        m_pdspVolt1->setDecimals(2);
        m_pdspVolt1->setSuffix(" V");
        m_pdspVolt1->setMinimum(-999.99);
        m_pdspVolt1->setMaximum(999.99);
        m_pdspVolt1->setSingleStep(0.01);
        m_pdspVolt1->setValue(0.18);

        m_plabDuration1 = new QLabel("Duration1", pchanqc);

        m_pdspDuration1 = new QDoubleSpinBox(pchanqc);
        m_pdspDuration1->setDecimals(2);
        m_pdspDuration1->setSuffix(" s");
        m_pdspDuration1->setMaximum(10.0);
        m_pdspDuration1->setValue(5.0);


        m_plabCurrent1 = new QLabel(QString::fromLocal8Bit("Current ≥"), pchanqc);//≥≧

        m_pdspCurrentMin1 = new QDoubleSpinBox(pchanqc);
        m_pdspCurrentMin1->setDecimals(2);
        m_pdspCurrentMin1->setSuffix(" pA");
        m_pdspCurrentMin1->setMinimum(-999999.99);
        m_pdspCurrentMin1->setMaximum(999999.99);
        m_pdspCurrentMin1->setValue(2000.0);

        //m_pdspCurrentMax1 = new QDoubleSpinBox(pchanqc);
        //m_pdspCurrentMax1->setDecimals(2);
        //m_pdspCurrentMax1->setSuffix(" pA");
        //m_pdspCurrentMax1->setMinimum(-999999.99);
        //m_pdspCurrentMax1->setMaximum(999999.99);
        //m_pdspCurrentMax1->setValue(999999.99);


        //m_plabStd1 = new QLabel("STD", pchanqc);
        //
        //m_pdspStdMin1 = new QDoubleSpinBox(pchanqc);
        //m_pdspStdMin1->setDecimals(4);
        //m_pdspStdMin1->setMinimum(-999.9999);
        //m_pdspStdMin1->setMaximum(999.9999);
        //m_pdspStdMin1->setValue(0.0001);
        //
        //m_pdspStdMax1 = new QDoubleSpinBox(pchanqc);
        //m_pdspStdMax1->setDecimals(4);
        //m_pdspStdMax1->setMinimum(-999.9999);
        //m_pdspStdMax1->setMaximum(999.9999);
        //m_pdspStdMax1->setValue(2.0);


        //m_pbtnChannelReset = new QPushButton("Reset", pchanqc);
        //m_pbtnChannelReset->setVisible(false);
        m_pbtnChannelQC = new QPushButton("Start", pchanqc);
        m_pbtnChannelQC->setCheckable(true);
        m_pbtnChannelQC->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Start开始通道QC，完成筛选后，可人工添加有效通道，然后点击Confirm确认有效通道，之后所有通道电压回到0</p></body></html>"));
        m_pbtnChannelQC->setIcon(QIcon(":/img/img/start.ico"));


        QGridLayout* gdlayout = new QGridLayout();
        gdlayout->addWidget(m_plabVolt1, 0, 0);
        gdlayout->addWidget(m_pdspVolt1, 0, 1);
        gdlayout->addWidget(m_plabDuration1, 0, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout->addWidget(m_pdspDuration1, 0, 3);

        gdlayout->addWidget(m_plabCurrent1, 1, 0);
        gdlayout->addWidget(m_pdspCurrentMin1, 1, 1);
        //gdlayout->addWidget(m_pdspCurrentMax1, 1, 2);

        //gdlayout->addWidget(m_plabStd1, 2, 0);
        //gdlayout->addWidget(m_pdspStdMin1, 2, 1);
        //gdlayout->addWidget(m_pdspStdMax1, 2, 2);

        //gdlayout->addWidget(m_pbtnChannelReset, 3, 1);
        gdlayout->addWidget(m_pbtnChannelQC, 3, 3);

        QVBoxLayout* verlayout6 = new QVBoxLayout();
        verlayout6->addLayout(gdlayout);
        //verlayout6->addStretch(1);
        //verlayout6->addLayout(horlayout1);
        //verlayout6->addStretch(1);
        //verlayout6->addLayout(gdlayout6);

        QHBoxLayout* horlayout9 = new QHBoxLayout();
        horlayout9->addLayout(verlayout6);
        horlayout9->addStretch(1);

        pchanqc->setLayout(horlayout9);
    }

    //MakeMembrane
    {
        //CapTest
        m_pgrpCapTest = new QGroupBox(QString::fromLocal8Bit("CapTest"), pmakemb);

        m_plabInterval2 = new QLabel("Intervals", m_pgrpCapTest);
        m_pispInterval2 = new QSpinBox(m_pgrpCapTest);
        m_pispInterval2->setSuffix(" s");
        m_pispInterval2->setMaximum(1);
        m_pispInterval2->setMaximum(99999);
        m_pispInterval2->setValue(60);
        m_pispInterval2->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>定时保存电容数据的时间间隔。保存目录为/data/Capacitance/</p></body></html>"));


        m_plabTimerDuration2 = new QLabel("Timer Duration", m_pgrpCapTest);
        m_pdspTimerDuration2 = new QDoubleSpinBox(m_pgrpCapTest);
        m_pdspTimerDuration2->setDecimals(1);
        m_pdspTimerDuration2->setSuffix(" min");
        m_pdspTimerDuration2->setMinimum(0.1);
        m_pdspTimerDuration2->setMaximum(1439.9);//23:59:54
        m_pdspTimerDuration2->setValue(30.0);
        m_pdspTimerDuration2->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设定电容测试的时间，周期最小为0.1min, 最大为1439.9min</p></body></html>"));

        m_plcdTimerDuration2 = new QLCDNumber(8, m_pgrpCapTest);
        m_plcdTimerDuration2->setSegmentStyle(QLCDNumber::Flat);
        m_plcdTimerDuration2->setFrameShape(QFrame::StyledPanel);
        SetTimehms2();

        m_pbtnCapTestTimer = new QPushButton("Start Timer", m_pgrpCapTest);
        m_pbtnCapTestTimer->setCheckable(true);
        m_pbtnCapTestTimer->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>开启/停止电容测试定时器</p></body></html>"));
        m_pbtnCapTestTimer->setIcon(QIcon(":/img/img/start.ico"));

        QGridLayout* gdlayout1 = new QGridLayout();
        gdlayout1->addWidget(m_plabInterval2, 0, 0);
        gdlayout1->addWidget(m_pispInterval2, 0, 1);
        gdlayout1->addWidget(m_plabTimerDuration2, 1, 0);
        gdlayout1->addWidget(m_pdspTimerDuration2, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
        gdlayout1->addWidget(m_plcdTimerDuration2, 1, 2, Qt::AlignVCenter | Qt::AlignLeft);
        gdlayout1->addWidget(m_pbtnCapTestTimer, 1, 3);

        m_pgrpCapTest->setLayout(gdlayout1);


        //MembraneQC
        m_pgrpMembraneQC = new QGroupBox("MembraneQC", pmakemb);

        m_plabVolt2 = new QLabel("Volt1", m_pgrpMembraneQC);
        m_pdspVolt2 = new QDoubleSpinBox(m_pgrpMembraneQC);
        m_pdspVolt2->setDecimals(2);
        m_pdspVolt2->setSuffix(" V");
        m_pdspVolt2->setMinimum(-999.99);
        m_pdspVolt2->setMaximum(999.99);
        m_pdspVolt2->setSingleStep(0.01);
        m_pdspVolt2->setValue(0.18);

        m_plabDuration2 = new QLabel("Duration1", m_pgrpMembraneQC);
        m_pdspDuration2 = new QDoubleSpinBox(m_pgrpMembraneQC);
        m_pdspDuration2->setDecimals(2);
        m_pdspDuration2->setSuffix(" s");
        m_pdspDuration2->setMaximum(10.0);
        m_pdspDuration2->setValue(5.0);


        //m_pbtnWaitingZeroAdjust2 = new QPushButton("Waiting For ZeroAdjust...", pholeqc);
        //m_pbtnWaitingZeroAdjust2->setVisible(false);


        m_plabCurrent2 = new QLabel("Current", m_pgrpMembraneQC);

        m_pdspCurrentMin2 = new QDoubleSpinBox(m_pgrpMembraneQC);
        m_pdspCurrentMin2->setDecimals(2);
        m_pdspCurrentMin2->setSuffix(" pA");
        m_pdspCurrentMin2->setMinimum(-999999.99);
        m_pdspCurrentMin2->setMaximum(999999.99);
        m_pdspCurrentMin2->setValue(-20.0);

        m_pdspCurrentMax2 = new QDoubleSpinBox(m_pgrpMembraneQC);
        m_pdspCurrentMax2->setDecimals(2);
        m_pdspCurrentMax2->setSuffix(" pA");
        m_pdspCurrentMax2->setMinimum(-999999.99);
        m_pdspCurrentMax2->setMaximum(999999.99);
        m_pdspCurrentMax2->setValue(20.0);


        m_plabStd2 = new QLabel("STD", m_pgrpMembraneQC);

        m_pdspStdMin2 = new QDoubleSpinBox(m_pgrpMembraneQC);
        m_pdspStdMin2->setDecimals(4);
        m_pdspStdMin2->setMinimum(-999.9999);
        m_pdspStdMin2->setMaximum(999.9999);
        m_pdspStdMin2->setValue(0.0001);

        m_pdspStdMax2 = new QDoubleSpinBox(m_pgrpMembraneQC);
        m_pdspStdMax2->setDecimals(4);
        m_pdspStdMax2->setMinimum(-999.9999);
        m_pdspStdMax2->setMaximum(999.9999);
        m_pdspStdMax2->setValue(5.0);

        m_pbtnMakeMembrane = new QPushButton("Start", m_pgrpMembraneQC);
        m_pbtnMakeMembrane->setCheckable(true);
        m_pbtnMakeMembrane->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>成膜完成后，Start开始加电压筛选出有效通道，然后人工添加完有效通道后，点击Confirm确认有效通道，之后可以开始电容计算筛选了</p></body></html>"));
        m_pbtnMakeMembrane->setIcon(QIcon(":/img/img/start.ico"));


        QGridLayout* gdlayout4 = new QGridLayout();
        gdlayout4->addWidget(m_plabVolt2, 0, 0);
        gdlayout4->addWidget(m_pdspVolt2, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
        gdlayout4->addWidget(m_plabDuration2, 0, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout4->addWidget(m_pdspDuration2, 0, 3);
        gdlayout4->addWidget(m_plabCurrent2, 1, 0);
        gdlayout4->addWidget(m_pdspCurrentMin2, 1, 1);
        gdlayout4->addWidget(m_pdspCurrentMax2, 1, 2);
        gdlayout4->addWidget(m_plabStd2, 2, 0);
        gdlayout4->addWidget(m_pdspStdMin2, 2, 1);
        gdlayout4->addWidget(m_pdspStdMax2, 2, 2);
        gdlayout4->addWidget(m_pbtnMakeMembrane, 2, 3);

        m_pgrpMembraneQC->setLayout(gdlayout4);


        //CapFilter
        m_pgrpCapFilter = new QGroupBox("CapFilter", pmakemb);

        m_plabCap2 = new QLabel("Cap", m_pgrpCapFilter);

        m_pdspCapMin2 = new QDoubleSpinBox(m_pgrpCapFilter);
        m_pdspCapMin2->setDecimals(2);
        m_pdspCapMin2->setMinimum(-999999.99);
        m_pdspCapMin2->setMaximum(999999.99);
        m_pdspCapMin2->setValue(10.0);

        m_pdspCapMax2 = new QDoubleSpinBox(m_pgrpCapFilter);
        m_pdspCapMax2->setDecimals(2);
        m_pdspCapMax2->setMinimum(-999999.99);
        m_pdspCapMax2->setMaximum(999999.99);
        m_pdspCapMax2->setValue(150.0);

        m_pbtnMakeMembraneStartCap = new QPushButton("Start Cap", m_pgrpCapFilter);
        m_pbtnMakeMembraneStartCap->setCheckable(true);
        m_pbtnMakeMembraneStartCap->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>StartCap开始计算电容，完成后点击StopCap筛选出电容值在给定范围内的通道，也可以人工添加有效通道，然后可以进入下一步了</p></body></html>"));
        m_pbtnMakeMembraneStartCap->setIcon(QIcon(":/img/img/start.ico"));


#if 1
        QGridLayout* gdlayout7 = new QGridLayout();
        gdlayout7->addWidget(m_plabCap2, 0, 0);
        gdlayout7->addWidget(m_pdspCapMin2, 0, 1);
        gdlayout7->addWidget(m_pdspCapMax2, 0, 2);
        gdlayout7->addWidget(m_pbtnMakeMembraneStartCap, 0, 3);

        m_pgrpCapFilter->setLayout(gdlayout7);
#else
        //gdlayout4->addWidget(m_plabCap2, 3, 0);
        //gdlayout4->addWidget(m_pdspCapMin2, 3, 1);
        //gdlayout4->addWidget(m_pdspCapMax2, 3, 2);
        //gdlayout4->addWidget(m_pbtnMakeMembraneStartCap, 3, 3);

        //m_pgrpMembraneQC->setLayout(gdlayout4);
#endif


        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addWidget(m_pgrpCapTest);
        verlayout9->addWidget(m_pgrpMembraneQC);
        verlayout9->addWidget(m_pgrpCapFilter);

        QHBoxLayout* horlayout9 = new QHBoxLayout();
        horlayout9->addLayout(verlayout9);
        horlayout9->addStretch(1);

        pmakemb->setLayout(horlayout9);
    }

    //PoreInsert
    {
        m_pchkWaitingZeroAdjust3 = new QCheckBox("ZeroAdjust", pinsert);
        m_pchkWaitingZeroAdjust3->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否需要调零</p></body></html>"));
        m_pbtnWaitingZeroAdjust3 = new QPushButton("Waiting For ZeroAdjust...", pinsert);
        m_pbtnWaitingZeroAdjust3->setEnabled(false);

        m_plabVolt3 = new QLabel("Volt1", pinsert);

        m_pdspVolt3 = new QDoubleSpinBox(pinsert);
        m_pdspVolt3->setDecimals(2);
        m_pdspVolt3->setSuffix(" V");
        m_pdspVolt3->setSingleStep(0.01);
        m_pdspVolt3->setMinimum(-999.99);
        m_pdspVolt3->setMaximum(999.99);
        m_pdspVolt3->setValue(0.18);

        m_plabDuration3 = new QLabel("Volt Duration", pinsert);

        m_pdspDuration3 = new QDoubleSpinBox(pinsert);
        m_pdspDuration3->setDecimals(2);
        m_pdspDuration3->setSuffix(" s");
        m_pdspDuration3->setMaximum(10.0);
        m_pdspDuration3->setValue(5.0);


        m_plabSingleLimit3 = new QLabel("SingleLimit:", pinsert);
        m_pdspSingleLimit3 = new QDoubleSpinBox(pinsert);
        m_pdspSingleLimit3->setSuffix(" pA");
        m_pdspSingleLimit3->setMinimum(0.0);
        m_pdspSingleLimit3->setMaximum(3000.0);
        m_pdspSingleLimit3->setSingleStep(10.0);
        m_pdspSingleLimit3->setValue(100.0);
        m_pdspSingleLimit3->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>单孔电流限制阈值</p></body></html>"));

        m_plabProtectVolt3 = new QLabel("Protect Volt", pinsert);
        m_pdspProtectVolt3 = new QDoubleSpinBox(pinsert);
        m_pdspProtectVolt3->setSuffix(" V");
        m_pdspProtectVolt3->setMinimum(-10.0);
        m_pdspProtectVolt3->setMaximum(10.0);
        m_pdspProtectVolt3->setSingleStep(1.0);
        m_pdspProtectVolt3->setValue(0.0);//0.05
        m_pdspProtectVolt3->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>单孔保护电压值</p></body></html>"));

        m_plabMultiLimit3 = new QLabel("MultiLimit: ", pinsert);
        m_pdspMultiLimit3 = new QDoubleSpinBox(pinsert);
        m_pdspMultiLimit3->setSuffix(" pA");
        m_pdspMultiLimit3->setMinimum(0.0);
        m_pdspMultiLimit3->setMaximum(4500.0);
        m_pdspMultiLimit3->setSingleStep(10.0);
        m_pdspMultiLimit3->setValue(400.0);
        m_pdspMultiLimit3->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>多孔电流限制阈值</p></body></html>"));

        m_plabPoreStatus3 = new QLabel("PoreStatusCount", pinsert);
        m_pispPoreStatus3 = new QSpinBox(pinsert);
        m_pispPoreStatus3->setMinimum(1);
        m_pispPoreStatus3->setMaximum(10);
        m_pispPoreStatus3->setSingleStep(1);
        m_pispPoreStatus3->setValue(3);
        m_pispPoreStatus3->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>判断孔状态的连续次数</p></body></html>"));

        m_plabSaturatedLimit3 = new QLabel("SaturatedLimit:", pinsert);
        m_pdspSaturatedLimit3 = new QDoubleSpinBox(pinsert);
        m_pdspSaturatedLimit3->setSuffix(" pA");
        m_pdspSaturatedLimit3->setMinimum(0.0);
        m_pdspSaturatedLimit3->setMaximum(6000.0);
        m_pdspSaturatedLimit3->setSingleStep(10.0);
        m_pdspSaturatedLimit3->setValue(1000.0);
        m_pdspSaturatedLimit3->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>膜破电流限制阈值</p></body></html>"));

        m_plabTimerDuration3 = new QLabel("Timer Duration", pinsert);

        m_pdspTimerDuration3 = new QDoubleSpinBox(pinsert);
        m_pdspTimerDuration3->setDecimals(1);
        m_pdspTimerDuration3->setSuffix(" min");
        m_pdspTimerDuration3->setMinimum(0.1);
        m_pdspTimerDuration3->setMaximum(1439.9);//23:59:54
        m_pdspTimerDuration3->setValue(30.0);
        m_pdspTimerDuration3->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设定插孔时间，周期最小为0.1min, 最大为1439.9min</p></body></html>"));

        m_plcdTimerDuration3 = new QLCDNumber(8, pinsert);
        m_plcdTimerDuration3->setSegmentStyle(QLCDNumber::Flat);
        m_plcdTimerDuration3->setFrameShape(QFrame::StyledPanel);

        SetTimehms3();


        m_pbtnPoreInsertTimer = new QPushButton("Start Timer", pinsert);
        m_pbtnPoreInsertTimer->setCheckable(true);
        m_pbtnPoreInsertTimer->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>StartTimer开始插孔，到达设定的时间后自动完成插孔，当然也可以提前点击StopTimer完成插孔</p></body></html>"));
        m_pbtnPoreInsertTimer->setIcon(QIcon(":/img/img/start.ico"));

        m_pbtnPoreInsert = new QPushButton("PoreFilter...", pinsert);
        m_pbtnPoreInsert->setIcon(QIcon(":/img/img/setting_press.png"));
        //m_pbtnPoreInsert->setIconSize(QSize(30, 30));
        //m_pbtnPoreInsert->setCheckable(true);
        //m_pbtnPoreInsert->setVisible(false);


        QHBoxLayout* horlayout1 = new QHBoxLayout();
        //horlayout1->addSpacing(20);
        horlayout1->addWidget(m_pchkWaitingZeroAdjust3);
        horlayout1->addWidget(m_pbtnWaitingZeroAdjust3);
        horlayout1->addStretch(1);

        QGridLayout* gdlayout1 = new QGridLayout();
        gdlayout1->addWidget(m_plabVolt3, 0, 0);
        gdlayout1->addWidget(m_pdspVolt3, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
        gdlayout1->addWidget(m_plabDuration3, 0, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout1->addWidget(m_pdspDuration3, 0, 3);

        gdlayout1->addWidget(m_plabSingleLimit3, 1, 0);
        gdlayout1->addWidget(m_pdspSingleLimit3, 1, 1);
        gdlayout1->addWidget(m_plabProtectVolt3, 1, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout1->addWidget(m_pdspProtectVolt3, 1, 3);
        gdlayout1->addWidget(m_plabMultiLimit3, 2, 0);
        gdlayout1->addWidget(m_pdspMultiLimit3, 2, 1);
        gdlayout1->addWidget(m_plabPoreStatus3, 2, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout1->addWidget(m_pispPoreStatus3, 2, 3);
        gdlayout1->addWidget(m_plabSaturatedLimit3, 3, 0);
        gdlayout1->addWidget(m_pdspSaturatedLimit3, 3, 1);

        gdlayout1->addWidget(m_plabTimerDuration3, 4, 0);
        gdlayout1->addWidget(m_pdspTimerDuration3, 4, 1);
        gdlayout1->addWidget(m_plcdTimerDuration3, 4, 2);
        gdlayout1->addWidget(m_pbtnPoreInsertTimer, 4, 3);

        QHBoxLayout* horlayout2 = new QHBoxLayout();
        horlayout2->addSpacing(20);
        horlayout2->addWidget(m_pbtnPoreInsert);
        horlayout2->addStretch(1);

        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addLayout(horlayout1);
        verlayout9->addLayout(gdlayout1);
        verlayout9->addLayout(horlayout2);

        QHBoxLayout* horlayout9 = new QHBoxLayout();
        horlayout9->addLayout(verlayout9);
        horlayout9->addStretch(1);

        pinsert->setLayout(horlayout9);
    }

    //InitPoreQC
    {
        m_pchkWaitingZeroAdjust4 = new QCheckBox("ZeroAdjust", pholeqc);
        m_pchkWaitingZeroAdjust4->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否需要调零</p></body></html>"));
        m_pbtnWaitingZeroAdjust4 = new QPushButton("Waiting For ZeroAdjust...", pholeqc);
        m_pbtnWaitingZeroAdjust4->setEnabled(false);

        m_plabVolt4 = new QLabel("Volt1", pholeqc);
        m_pdspVolt4 = new QDoubleSpinBox(pholeqc);
        m_pdspVolt4->setDecimals(2);
        m_pdspVolt4->setSuffix(" V");
        m_pdspVolt4->setMinimum(-999.99);
        m_pdspVolt4->setMaximum(999.99);
        m_pdspVolt4->setSingleStep(0.01);
        m_pdspVolt4->setValue(0.18);

        m_plabDuration4 = new QLabel("Duration1", pholeqc);
        m_pdspDuration4 = new QDoubleSpinBox(pholeqc);
        m_pdspDuration4->setDecimals(2);
        m_pdspDuration4->setSuffix(" s");
        m_pdspDuration4->setMaximum(10.0);
        m_pdspDuration4->setValue(1.0);

        m_plabVolt42 = new QLabel("Volt2", pholeqc);
        m_pdspVolt42 = new QDoubleSpinBox(pholeqc);
        m_pdspVolt42->setDecimals(2);
        m_pdspVolt42->setSuffix(" V");
        m_pdspVolt42->setMinimum(-999.99);
        m_pdspVolt42->setMaximum(999.99);
        m_pdspVolt42->setSingleStep(0.01);
        m_pdspVolt42->setValue(-0.18);

        m_plabDuration42 = new QLabel("Duration2", pholeqc);
        m_pdspDuration42 = new QDoubleSpinBox(pholeqc);
        m_pdspDuration42->setDecimals(2);
        m_pdspDuration42->setSuffix(" s");
        m_pdspDuration42->setMaximum(10.0);
        m_pdspDuration42->setValue(0.2);

        m_plabVolt43 = new QLabel("Volt3", pholeqc);
        m_pdspVolt43 = new QDoubleSpinBox(pholeqc);
        m_pdspVolt43->setDecimals(2);
        m_pdspVolt43->setSuffix(" V");
        m_pdspVolt43->setMinimum(-999.99);
        m_pdspVolt43->setMaximum(999.99);
        m_pdspVolt43->setSingleStep(0.01);
        m_pdspVolt43->setValue(0.18);

        m_plabDuration43 = new QLabel("Duration3", pholeqc);
        m_pdspDuration43 = new QDoubleSpinBox(pholeqc);
        m_pdspDuration43->setDecimals(2);
        m_pdspDuration43->setSuffix(" s");
        m_pdspDuration43->setMaximum(10.0);
        m_pdspDuration43->setValue(5.0);

        m_plabCurrent4 = new QLabel("Current", pholeqc);

        m_pdspCurrentMin4 = new QDoubleSpinBox(pholeqc);
        m_pdspCurrentMin4->setDecimals(2);
        m_pdspCurrentMin4->setSuffix(" pA");
        m_pdspCurrentMin4->setMinimum(-999999.99);
        m_pdspCurrentMin4->setMaximum(999999.99);
        m_pdspCurrentMin4->setValue(100.0);

        m_pdspCurrentMax4 = new QDoubleSpinBox(pholeqc);
        m_pdspCurrentMax4->setDecimals(2);
        m_pdspCurrentMax4->setSuffix(" pA");
        m_pdspCurrentMax4->setMinimum(-999999.99);
        m_pdspCurrentMax4->setMaximum(999999.99);
        m_pdspCurrentMax4->setValue(400.0);

        m_plabStd4 = new QLabel("STD", pholeqc);

        m_pdspStdMin4 = new QDoubleSpinBox(pholeqc);
        m_pdspStdMin4->setDecimals(4);
        m_pdspStdMin4->setMinimum(-999.9999);
        m_pdspStdMin4->setMaximum(999.9999);
        m_pdspStdMin4->setValue(0.0001);

        m_pdspStdMax4 = new QDoubleSpinBox(pholeqc);
        m_pdspStdMax4->setDecimals(4);
        m_pdspStdMax4->setMinimum(-999.9999);
        m_pdspStdMax4->setMaximum(999.9999);
        m_pdspStdMax4->setValue(5.0);


        m_pbtnPoreQC = new QPushButton("Start", pholeqc);
        m_pbtnPoreQC->setCheckable(true);
        m_pbtnPoreQC->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Start开始初始孔QC，自动筛选出有效通道后，可人工添加有效通道，然后点击Confirm确认有效通道</p></body></html>"));
        m_pbtnPoreQC->setIcon(QIcon(":/img/img/start.ico"));


        QHBoxLayout* horlayout1 = new QHBoxLayout();
        horlayout1->addWidget(m_pchkWaitingZeroAdjust4);
        horlayout1->addWidget(m_pbtnWaitingZeroAdjust4);
        horlayout1->addStretch(1);

        int item = 0;
        QGridLayout* gdlayout = new QGridLayout();

        gdlayout->addWidget(m_plabVolt4, item, 0);
        gdlayout->addWidget(m_pdspVolt4, item, 1);
        gdlayout->addWidget(m_plabDuration4, item, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout->addWidget(m_pdspDuration4, item, 3);

        ++item;
        gdlayout->addWidget(m_plabVolt42, item, 0);
        gdlayout->addWidget(m_pdspVolt42, item, 1);
        gdlayout->addWidget(m_plabDuration42, item, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout->addWidget(m_pdspDuration42, item, 3);

        ++item;
        gdlayout->addWidget(m_plabVolt43, item, 0);
        gdlayout->addWidget(m_pdspVolt43, item, 1);
        gdlayout->addWidget(m_plabDuration43, item, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout->addWidget(m_pdspDuration43, item, 3);

        ++item;
        gdlayout->addWidget(m_plabCurrent4, item, 0);
        gdlayout->addWidget(m_pdspCurrentMin4, item, 1);
        gdlayout->addWidget(m_pdspCurrentMax4, item, 2);

        ++item;
        gdlayout->addWidget(m_plabStd4, item, 0);
        gdlayout->addWidget(m_pdspStdMin4, item, 1);
        gdlayout->addWidget(m_pdspStdMax4, item, 2);

        gdlayout->addWidget(m_pbtnPoreQC, item, 3);

        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addLayout(horlayout1);
        verlayout9->addLayout(gdlayout);

        QHBoxLayout* horlayout9 = new QHBoxLayout();
        horlayout9->addLayout(verlayout9);
        horlayout9->addStretch(1);

        pholeqc->setLayout(horlayout9);
    }

    //Sequencing
    {
        m_plabVolt5 = new QLabel("Volt1", psequen);
        m_pdspVolt5 = new QDoubleSpinBox(psequen);
        m_pdspVolt5->setDecimals(2);
        m_pdspVolt5->setSuffix(" V");
        m_pdspVolt5->setMinimum(-999.99);
        m_pdspVolt5->setMaximum(999.99);
        m_pdspVolt5->setSingleStep(0.01);
        m_pdspVolt5->setValue(0.18);

        m_plabDuration5 = new QLabel("Volt Duration", psequen);

        m_pdspDuration5 = new QDoubleSpinBox(psequen);
        m_pdspDuration5->setDecimals(2);
        m_pdspDuration5->setSuffix(" s");
        m_pdspDuration5->setMaximum(10.0);
        m_pdspDuration5->setValue(3.0);

        m_pbtnStartAutoDegating5 = new QPushButton("Start Auto Degating...", psequen);
        m_pbtnStartAutoZeroAdjust5 = new QPushButton("Start Auto ZeroAdjust...", psequen);

        //m_plabTimerDuration5 = new QLabel("Timer Duration", psequen);
        //m_pdspTimerDuration5 = new QDoubleSpinBox(psequen);
        //m_pdspTimerDuration5->setDecimals(1);
        //m_pdspTimerDuration5->setSuffix(" min");
        //m_pdspTimerDuration5->setMinimum(0.1);
        //m_pdspTimerDuration5->setMaximum(1439.9);//23:59:54//600.0
        //m_pdspTimerDuration5->setValue(2.0);

        ////m_plcdTimerQC5 = new QLCDNumber(8, psequen);
        ////m_plcdTimerQC5->setSegmentStyle(QLCDNumber::Flat);
        ////m_plcdTimerQC5->setFrameShape(QFrame::StyledPanel);
        ////
        ////SetTimehms5();

        //m_plabCurrent5 = new QLabel("Current >", psequen);

        //m_pdspCurrentMin5 = new QDoubleSpinBox(psequen);
        //m_pdspCurrentMin5->setDecimals(2);
        //m_pdspCurrentMin5->setSuffix(" pA");
        //m_pdspCurrentMin5->setMinimum(-999999.99);
        //m_pdspCurrentMin5->setMaximum(999999.99);
        //m_pdspCurrentMin5->setValue(400.0);

        //m_pdspCurrentMax5 = new QDoubleSpinBox(psequen);
        //m_pdspCurrentMax5->setDecimals(2);
        //m_pdspCurrentMax5->setSuffix(" pA");
        //m_pdspCurrentMax5->setMinimum(-999999.99);
        //m_pdspCurrentMax5->setMaximum(999999.99);
        //m_pdspCurrentMax5->setValue(1000.0);

        m_pbtnSequencing = new QPushButton("Start", psequen);
        m_pbtnSequencing->setCheckable(true);
        m_pbtnSequencing->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Start开始测序，开启自动Degating、自动短路检测和自动调零</p></body></html>"));


        QGridLayout* gdlayout = new QGridLayout();
        gdlayout->addWidget(m_plabVolt5, 0, 0);
        gdlayout->addWidget(m_pdspVolt5, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
        gdlayout->addWidget(m_plabDuration5, 0, 2, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout->addWidget(m_pdspDuration5, 0, 3);

        //gdlayout->addWidget(m_plabTimerDuration5, 1, 0);
        //gdlayout->addWidget(m_pdspTimerDuration5, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
        ////gdlayout2->addWidget(m_plcdTimerQC5, 0, 2);
        //gdlayout->addWidget(m_plabCurrent5, 1, 2, Qt::AlignVCenter | Qt::AlignRight);
        //gdlayout->addWidget(m_pdspCurrentMin5, 1, 3);
        ////gdlayout2->addWidget(m_pdspCurrentMax5, 1, 2);

        QHBoxLayout* horlayout1 = new QHBoxLayout();
        horlayout1->addWidget(m_pbtnStartAutoDegating5);
        horlayout1->addSpacing(20);
        horlayout1->addWidget(m_pbtnStartAutoZeroAdjust5);
        horlayout1->addStretch(1);

        QHBoxLayout* horlayout2 = new QHBoxLayout();
        horlayout2->addStretch(1);
        horlayout2->addWidget(m_pbtnSequencing);
        horlayout2->addSpacing(20);


        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addLayout(gdlayout);
        verlayout9->addLayout(horlayout1);
        verlayout9->addLayout(horlayout2);

        QHBoxLayout* horlayout9 = new QHBoxLayout();
        horlayout9->addLayout(verlayout9);
        horlayout9->addStretch(1);

        psequen->setLayout(horlayout9);
    }

    //MuxScan
    {
        //QC
        //m_pgrpPoreQC = new QGroupBox("PoreQC", psequqc);

        m_pbtnWaitingZeroAdjust6 = new QPushButton("Waiting For ZeroAdjust...", psequqc);

        m_plabVolt6 = new QLabel("PoreQC Volt", psequqc);
        m_pdspVolt6 = new QDoubleSpinBox(psequqc);
        m_pdspVolt6->setDecimals(2);
        m_pdspVolt6->setSuffix(" V");
        m_pdspVolt6->setMinimum(-999.99);
        m_pdspVolt6->setMaximum(999.99);
        m_pdspVolt6->setValue(0.05);//0.02

        m_plabDuration6 = new QLabel("PoreQC Duration", psequqc);
        m_pdspDuration6 = new QDoubleSpinBox(psequqc);
        m_pdspDuration6->setDecimals(2);
        m_pdspDuration6->setSuffix(" s");
        //m_pdspDuration6->setSingleStep(0.1);
        m_pdspDuration6->setMaximum(10.0);
        m_pdspDuration6->setValue(5.0);

        m_plabCurrent6 = new QLabel("Current", psequqc);

        m_pdspCurrentMin6 = new QDoubleSpinBox(psequqc);
        m_pdspCurrentMin6->setDecimals(2);
        m_pdspCurrentMin6->setSuffix(" pA");
        m_pdspCurrentMin6->setMinimum(-999999.99);
        m_pdspCurrentMin6->setMaximum(999999.99);
        m_pdspCurrentMin6->setValue(10.0);

        m_pdspCurrentMax6 = new QDoubleSpinBox(psequqc);
        m_pdspCurrentMax6->setDecimals(2);
        m_pdspCurrentMax6->setSuffix(" pA");
        m_pdspCurrentMax6->setMinimum(-999999.99);
        m_pdspCurrentMax6->setMaximum(999999.99);
        m_pdspCurrentMax6->setValue(1000.0);

        m_plabStd6 = new QLabel("STD", psequqc);

        m_pdspStdMin6 = new QDoubleSpinBox(psequqc);
        m_pdspStdMin6->setDecimals(4);
        m_pdspStdMin6->setMinimum(-999.9999);
        m_pdspStdMin6->setMaximum(999.9999);
        m_pdspStdMin6->setValue(0.0001);

        m_pdspStdMax6 = new QDoubleSpinBox(psequqc);
        m_pdspStdMax6->setDecimals(4);
        m_pdspStdMax6->setMinimum(-999.9999);
        m_pdspStdMax6->setMaximum(999.9999);
        m_pdspStdMax6->setValue(5.0);

        m_pbtnNextStep = new QPushButton("Next", psequqc);
        m_pbtnNextStep->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>人工添加完通道后，点击Next确认并使流程继续往下走</p></body></html>"));

#if 0
        //qc
        QHBoxLayout* horlayout1 = new QHBoxLayout();
        horlayout1->addWidget(m_pbtnWaitingZeroAdjust6);
        horlayout1->addStretch(1);

        int row = 0;
        int col = 0;
        QGridLayout* gdlayout1 = new QGridLayout();
        gdlayout1->addWidget(m_plabVolt6, row, col++);
        gdlayout1->addWidget(m_pdspVolt6, row, col++);
        gdlayout1->addWidget(m_plabDuration6, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout1->addWidget(m_pdspDuration6, row, col++);

        ++row;
        col = 0;
        gdlayout1->addWidget(m_plabCurrent6, row, col++);
        gdlayout1->addWidget(m_pdspCurrentMin6, row, col++);
        gdlayout1->addWidget(m_pdspCurrentMax6, row, col++);

        ++row;
        col = 0;
        gdlayout1->addWidget(m_plabStd6, row, col++);
        gdlayout1->addWidget(m_pdspStdMin6, row, col++);
        gdlayout1->addWidget(m_pdspStdMax6, row, col++);
        gdlayout1->addWidget(m_pbtnNextStep, row, col++);

        QVBoxLayout* verlayout1 = new QVBoxLayout();
        verlayout1->addLayout(horlayout1);
        verlayout1->addLayout(gdlayout1);

        m_pgrpPoreQC->setLayout(verlayout1);
#endif


        //Seq
        //m_pgrpSequencing = new QGroupBox("Sequencing", psequqc);

        m_plabVolt62 = new QLabel("Zero Volt", psequqc);
        m_pdspVolt62 = new QDoubleSpinBox(psequqc);
        m_pdspVolt62->setDecimals(2);
        m_pdspVolt62->setSuffix(" V");
        m_pdspVolt62->setMinimum(-999.99);
        m_pdspVolt62->setMaximum(999.99);
        m_pdspVolt62->setSingleStep(0.01);
        m_pdspVolt62->setValue(0.0);
        m_pdspVolt62->setEnabled(false);

        m_plabDuration62 = new QLabel("ZeroVolt Duration", psequqc);
        m_pdspDuration62 = new QDoubleSpinBox(psequqc);
        m_pdspDuration62->setDecimals(2);
        m_pdspDuration62->setSuffix(" s");
        m_pdspDuration62->setMaximum(10.0);
        m_pdspDuration62->setValue(5.0);

        m_plabVolt63 = new QLabel("Seq Volt", psequqc);
        m_pdspVolt63 = new QDoubleSpinBox(psequqc);
        m_pdspVolt63->setDecimals(2);
        m_pdspVolt63->setSuffix(" V");
        m_pdspVolt63->setMinimum(-999.99);
        m_pdspVolt63->setMaximum(999.99);
        m_pdspVolt63->setSingleStep(0.01);
        m_pdspVolt63->setValue(0.18);

        m_plabDuration63 = new QLabel("SeqVolt Duration", psequqc);
        m_pdspDuration63 = new QDoubleSpinBox(psequqc);
        m_pdspDuration63->setDecimals(2);
        m_pdspDuration63->setSuffix(" s");
        m_pdspDuration63->setMaximum(10.0);
        m_pdspDuration63->setValue(3.0);

        m_pbtnStartAutoDegating6 = new QPushButton("Start Auto Degating...", psequqc);
        m_pbtnStartAutoZeroAdjust6 = new QPushButton("Start Auto ZeroAdjust...", psequqc);

        //m_plabTimerDuration6 = new QLabel("Timer Duration", psequqc);
        //m_pdspTimerDuration6 = new QDoubleSpinBox(psequqc);
        //m_pdspTimerDuration6->setDecimals(1);
        //m_pdspTimerDuration6->setSuffix(" min");
        //m_pdspTimerDuration6->setMinimum(0.1);
        //m_pdspTimerDuration6->setMaximum(1439.9);//23:59:54//600.0
        //m_pdspTimerDuration6->setValue(2.0);

        //m_plabCurrent62 = new QLabel("Current >", psequqc);

        //m_pdspCurrentMin62 = new QDoubleSpinBox(psequqc);
        //m_pdspCurrentMin62->setDecimals(2);
        //m_pdspCurrentMin62->setSuffix(" pA");
        //m_pdspCurrentMin62->setMinimum(-999999.99);
        //m_pdspCurrentMin62->setMaximum(999999.99);
        //m_pdspCurrentMin62->setValue(400.0);

        m_plabCycle6 = new QLabel("Cycle", psequqc);
        m_pispCycle6 = new QSpinBox(psequqc);
        m_pispCycle6->setMinimum(1);
        m_pispCycle6->setMaximum(999999);
        m_pispCycle6->setValue(10);

        m_plabTimerPeriod6 = new QLabel("Timer Period", psequqc);
        m_pdspTimerPeriod6 = new QDoubleSpinBox(psequqc);
        m_pdspTimerPeriod6->setDecimals(1);
        m_pdspTimerPeriod6->setSuffix(" min");
        m_pdspTimerPeriod6->setMinimum(0.1);
        m_pdspTimerPeriod6->setMaximum(1439.9);//23:59:54//600.0
        m_pdspTimerPeriod6->setValue(125.0);
        m_pdspTimerPeriod6->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>周期最小为0.1min, 最大为1439.9min</p></body></html>"));

        m_plcdTimerPeriod6 = new QLCDNumber(8, psequqc);
        m_plcdTimerPeriod6->setSegmentStyle(QLCDNumber::Flat);
        m_plcdTimerPeriod6->setFrameShape(QFrame::StyledPanel);

        SetTimehms6();

        m_pbtnSequencingQC = new QPushButton("Start", psequqc);
        m_pbtnSequencingQC->setIcon(QIcon(":/img/img/exaile.png"));
        m_pbtnSequencingQC->setIconSize(QSize(30, 30));
        m_pbtnSequencingQC->setCheckable(true);
        m_pbtnSequencingQC->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Start开始周期测序，开启自动Degating、自动短路检测和自动调零</p></body></html>"));

#if 0
        //seq
        row = 0;
        col = 0;
        QGridLayout* gdlayout6 = new QGridLayout();
        gdlayout6->addWidget(m_plabVolt62, row, col++);
        gdlayout6->addWidget(m_pdspVolt62, row, col++);
        gdlayout6->addWidget(m_plabDuration62, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout6->addWidget(m_pdspDuration62, row, col++);

        ++row;
        col = 0;
        gdlayout6->addWidget(m_plabVolt63, row, col++);
        gdlayout6->addWidget(m_pdspVolt63, row, col++);
        gdlayout6->addWidget(m_plabDuration63, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout6->addWidget(m_pdspDuration63, row, col++);

        //++row;
        //col = 0;
        //gdlayout1->addWidget(m_plabTimerDuration6, row, col++);
        //gdlayout1->addWidget(m_pdspTimerDuration6, row, col++);
        //gdlayout1->addWidget(m_plabCurrent62, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        //gdlayout1->addWidget(m_pdspCurrentMin62, row, col++);

        QHBoxLayout* horlayout6 = new QHBoxLayout();
        horlayout6->addWidget(m_pbtnStartAutoDegating6);
        horlayout6->addWidget(m_pbtnStartAutoZeroAdjust6);
        horlayout6->addSpacing(20);
        //horlayout6->addStretch(1);
        horlayout6->addWidget(m_plabCycle6);
        horlayout6->addWidget(m_pispCycle6);

        QHBoxLayout* horlayout7 = new QHBoxLayout();
        horlayout7->addWidget(m_plabTimerPeriod6);
        horlayout7->addWidget(m_pdspTimerPeriod6);
        horlayout7->addWidget(m_plcdTimerPeriod6);
        //horlayout7->addSpacing(20);
        horlayout7->addStretch(1);
        horlayout7->addWidget(m_pbtnSequencingQC);

        QVBoxLayout* verlayout6 = new QVBoxLayout();
        //verlayout9->setContentsMargins(5, 1, 5, 1);
        verlayout6->addLayout(gdlayout6);
        verlayout6->addLayout(horlayout6);
        verlayout6->addLayout(horlayout7);

        m_pgrpSequencing->setLayout(verlayout6);


        QVBoxLayout* verlayout9 = new QVBoxLayout();
        //verlayout9->setContentsMargins(5, 1, 5, 1);
        verlayout9->addWidget(m_pgrpPoreQC);
        verlayout9->addWidget(m_pgrpSequencing);
#endif

        QHBoxLayout* horlayout1 = new QHBoxLayout();
        horlayout1->addWidget(m_pbtnWaitingZeroAdjust6);
        horlayout1->addStretch(1);

        int row = 0;
        int col = 0;
        QGridLayout* gdlayout1 = new QGridLayout();
        gdlayout1->addWidget(m_plabVolt6, row, col++);
        gdlayout1->addWidget(m_pdspVolt6, row, col++);
        gdlayout1->addWidget(m_plabDuration6, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout1->addWidget(m_pdspDuration6, row, col++);

        ++row;
        col = 0;
        gdlayout1->addWidget(m_plabCurrent6, row, col++);
        gdlayout1->addWidget(m_pdspCurrentMin6, row, col++);
        gdlayout1->addWidget(m_pdspCurrentMax6, row, col++);

        ++row;
        col = 0;
        gdlayout1->addWidget(m_plabStd6, row, col++);
        gdlayout1->addWidget(m_pdspStdMin6, row, col++);
        gdlayout1->addWidget(m_pdspStdMax6, row, col++);
        gdlayout1->addWidget(m_pbtnNextStep, row, col++);


        ++row;
        col = 0;
        gdlayout1->addWidget(m_plabVolt62, row, col++);
        gdlayout1->addWidget(m_pdspVolt62, row, col++);
        gdlayout1->addWidget(m_plabDuration62, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout1->addWidget(m_pdspDuration62, row, col++);

        ++row;
        col = 0;
        gdlayout1->addWidget(m_plabVolt63, row, col++);
        gdlayout1->addWidget(m_pdspVolt63, row, col++);
        gdlayout1->addWidget(m_plabDuration63, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        gdlayout1->addWidget(m_pdspDuration63, row, col++);

        //++row;
        //col = 0;
        //gdlayout1->addWidget(m_plabTimerDuration6, row, col++);
        //gdlayout1->addWidget(m_pdspTimerDuration6, row, col++);
        //gdlayout1->addWidget(m_plabCurrent62, row, col++, Qt::AlignVCenter | Qt::AlignRight);
        //gdlayout1->addWidget(m_pdspCurrentMin62, row, col++);

        QHBoxLayout* horlayout6 = new QHBoxLayout();
        horlayout6->addWidget(m_pbtnStartAutoDegating6);
        horlayout6->addWidget(m_pbtnStartAutoZeroAdjust6);
        horlayout6->addSpacing(20);
        //horlayout6->addStretch(1);
        horlayout6->addWidget(m_plabCycle6);
        horlayout6->addWidget(m_pispCycle6);

        QHBoxLayout* horlayout7 = new QHBoxLayout();
        horlayout7->addWidget(m_plabTimerPeriod6);
        horlayout7->addWidget(m_pdspTimerPeriod6);
        horlayout7->addWidget(m_plcdTimerPeriod6);
        //horlayout7->addSpacing(20);
        horlayout7->addStretch(1);
        horlayout7->addWidget(m_pbtnSequencingQC);


        QVBoxLayout* verlayout9 = new QVBoxLayout();
        //verlayout9->setContentsMargins(5, 1, 5, 1);
        verlayout9->addLayout(horlayout1);
        verlayout9->addLayout(gdlayout1);
        verlayout9->addLayout(horlayout6);
        verlayout9->addLayout(horlayout7);
        //verlayout9->addStretch(1);

        QHBoxLayout* horlayout9 = new QHBoxLayout();
        horlayout9->addLayout(verlayout9);
        horlayout9->addStretch(1);

        psequqc->setLayout(horlayout9);
    }

    //config
    connect(m_pbtnLoadConfig, &QPushButton::clicked, this, &ControlView::onClickLoadConfig);
    connect(m_pbtnSaveConfig, &QPushButton::clicked, this, &ControlView::onClickSaveConfig);
    connect(m_pchkAllVisible1, &QCheckBox::stateChanged, this, &ControlView::onCheckAllVisible);
    connect(m_pchkShowVisible1, &QCheckBox::stateChanged, this, &ControlView::onCheckShowVisible);
    connect(m_pchkAllSeq1, &QCheckBox::stateChanged, this, &ControlView::onCheckAllSeq);
    connect(m_pchkShowSeq1, &QCheckBox::stateChanged, this, &ControlView::onCheckShowSeq);
    //channel qc
    //connect(m_pbtnChannelReset, &QPushButton::clicked, this, &ControlView::onClickChannelReset);
    connect(m_pbtnChannelQC, &QPushButton::clicked, this, &ControlView::onClickChannelQC);
    //make membrane
    //connect(m_pbtnWaitingZeroAdjust2, &QPushButton::clicked, this, &ControlView::onClickShowZeroAdjust);
    connect(m_pbtnCapTestTimer, &QPushButton::clicked, this, &ControlView::onClickMakeMembraneStartCapTest);
    connect(m_pbtnMakeMembrane, &QPushButton::clicked, this, &ControlView::onClickMakeMembraneStart);
    connect(m_pbtnMakeMembraneStartCap, &QPushButton::clicked, this, &ControlView::onClickMakeMembraneStartCapFilter);
    //pore insert
    connect(m_pchkWaitingZeroAdjust3, &QCheckBox::stateChanged, this, &ControlView::onCheckWaitingZeroAdjust3);
    connect(m_pbtnWaitingZeroAdjust3, &QPushButton::clicked, this, &ControlView::onClickShowZeroAdjust);
    connect(m_pbtnPoreInsertTimer, &QPushButton::clicked, this, &ControlView::onClickPoreInsertTimer);
    connect(m_pbtnPoreInsert, &QPushButton::clicked, this, &ControlView::onClickPoreInsert);
    //init pore qc
    connect(m_pchkWaitingZeroAdjust4, &QCheckBox::stateChanged, this, &ControlView::onCheckWaitingZeroAdjust4);
    connect(m_pbtnWaitingZeroAdjust4, &QPushButton::clicked, this, &ControlView::onClickShowZeroAdjust);
    connect(m_pbtnPoreQC, &QPushButton::clicked, this, &ControlView::onClickInitPoreQC);
    //sequencing
    connect(m_pbtnStartAutoDegating5, &QPushButton::clicked, this, &ControlView::onClickShowDegating);
    connect(m_pbtnStartAutoZeroAdjust5, &QPushButton::clicked, this, &ControlView::onClickShowZeroAdjust);
    connect(m_pbtnSequencing, &QPushButton::clicked, this, &ControlView::onClickSequencingStart);
    //mux scan
    connect(m_pbtnWaitingZeroAdjust6, &QPushButton::clicked, this, &ControlView::onClickShowZeroAdjust);
    connect(m_pbtnNextStep, &QPushButton::clicked, this, &ControlView::onClickNextStep);
    connect(m_pbtnStartAutoDegating6, &QPushButton::clicked, this, &ControlView::onClickShowDegating);
    connect(m_pbtnStartAutoZeroAdjust6, &QPushButton::clicked, this, &ControlView::onClickShowZeroAdjust);
    connect(m_pbtnSequencingQC, &QPushButton::clicked, this, &ControlView::onClickMuxScanStart);

    return false;
}

bool ControlView::InitCurrentCtrl(void)
{
	m_pgrpCurrent = new QGroupBox("Current", this);
    m_pgrpCurrent->setMaximumWidth(330);
    //m_pgrpCurrent->setContentsMargins(0, 0, 1, 1);

	m_plabSingleLimit = new QLabel("SingleLimit:", m_pgrpCurrent);
	m_pdspSingleLimit = new QDoubleSpinBox(m_pgrpCurrent);
    m_pdspSingleLimit->setSuffix(" pA");
	m_pdspSingleLimit->setMinimum(0.0);
	m_pdspSingleLimit->setMaximum(3000.0);
	m_pdspSingleLimit->setSingleStep(10.0);
    m_pdspSingleLimit->setValue(100.0);
    m_pdspSingleLimit->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>单孔电流限制阈值</p></body></html>"));


    m_pbtnEnableAuto = new QPushButton("EnableAuto", m_pgrpCurrent);
    m_pbtnEnableAuto->setCheckable(true);
    m_pbtnEnableAuto->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>启用/禁用电流监测自动保护/关断通道电压功能</p></body></html>"));

    //m_pchkAllAuto = new QCheckBox("Auto", m_pgrpCurrent);
    //m_pchkAllAuto->setChecked(true);
    //m_pchkAllAuto->setEnabled(false);


	m_plabMultiLimit = new QLabel("MultiLimit: ", m_pgrpCurrent);
	m_pdspMultiLimit = new QDoubleSpinBox(m_pgrpCurrent);
    m_pdspMultiLimit->setSuffix(" pA");
	m_pdspMultiLimit->setMinimum(0.0);
	m_pdspMultiLimit->setMaximum(4500.0);
	m_pdspMultiLimit->setSingleStep(10.0);
    m_pdspMultiLimit->setValue(400.0);
    m_pdspMultiLimit->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>多孔电流限制阈值</p></body></html>"));


    m_pbtnEnableProtect = new QPushButton("EnableAutoC", m_pgrpCurrent);
    m_pbtnEnableProtect->setCheckable(true);
    m_pbtnEnableProtect->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>启用/禁用测序阶段电流自动监测并关闭多孔和破膜功能</p></body></html>"));
    //m_pbtnAutoZero = new QPushButton("AutoZero", m_pgrpCurrent);
    //m_pbtnAutoZero->setCheckable(true);
    //m_pbtnAutoZero->setEnabled(false);
    //m_pbtnAutoZero->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>启用/禁用自动保护/关断通道电压</p></body></html>"));


    m_plabSaturatedLimit = new QLabel("SaturatedLimit:", m_pgrpCurrent);
    m_pdspSaturatedLimit = new QDoubleSpinBox(m_pgrpCurrent);
    m_pdspSaturatedLimit->setSuffix(" pA");
    m_pdspSaturatedLimit->setMinimum(0.0);
    m_pdspSaturatedLimit->setMaximum(6000.0);
    m_pdspSaturatedLimit->setSingleStep(10.0);
    m_pdspSaturatedLimit->setValue(1000.0);
    m_pdspSaturatedLimit->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>膜破电流限制阈值</p></body></html>"));

    m_pbtnOnlySinglePore = new QPushButton("SinglePore", m_pgrpCurrent);
    m_pbtnOnlySinglePore->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>当前显示通道只勾选单孔通道</p></body></html>"));


#if 0
	m_plabNonembed = new QLabel("NonePore:", m_pgrpCurrent);
	m_plabNonembed2 = new QLabel(m_pgrpCurrent);
	m_plabNonembed2->setMinimumWidth(20);
	m_plabNonembed2->setMaximumWidth(20);
	m_plabNonembed2->setMinimumHeight(20);
	m_plabNonembed2->setMaximumHeight(20);
	m_plabNonembed2->setStyleSheet(g_colorStyle[PoreHoleStatus::PHS_NONEMBED]);


	m_plabSingle = new QLabel("SinglePore:", m_pgrpCurrent);
	m_plabSingle2 = new QLabel(m_pgrpCurrent);
	m_plabSingle2->setMinimumWidth(20);
	m_plabSingle2->setMaximumWidth(20);
	m_plabSingle2->setMinimumHeight(20);
	m_plabSingle2->setMaximumHeight(20);
	m_plabSingle2->setStyleSheet(g_colorStyle[PoreHoleStatus::PHS_SINGLE]);


	m_plabMultiMar = new QLabel("MultiPore:", m_pgrpCurrent);
	m_plabMultiMar2 = new QLabel(m_pgrpCurrent);
	m_plabMultiMar2->setMinimumWidth(20);
	m_plabMultiMar2->setMaximumWidth(20);
	m_plabMultiMar2->setMinimumHeight(20);
	m_plabMultiMar2->setMaximumHeight(20);
	m_plabMultiMar2->setStyleSheet(g_colorStyle[PoreHoleStatus::PHS_MULTIMAR]);
#endif

	m_plabCloseVlot = new QLabel("ClosePore:", m_pgrpCurrent);
    m_plabCloseVlot->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>有黑边框的表示该孔为关断</p></body></html>"));
	m_plabCloseVlot2 = new QLabel(m_pgrpCurrent);
	m_plabCloseVlot2->setMinimumWidth(20);
	m_plabCloseVlot2->setMaximumWidth(20);
	m_plabCloseVlot2->setMinimumHeight(20);
	m_plabCloseVlot2->setMaximumHeight(20);
	m_plabCloseVlot2->setStyleSheet(g_borderStyle[1] + g_colorStyle[EEmbedPoreStatus::EPS_MULTIPORE]);

    m_plabSingleProtect = new QLabel("ProtectPore:", m_pgrpCurrent);
    m_plabSingleProtect->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>有蓝边框的表示该孔为单孔保护</p></body></html>"));
    m_plabSingleProtect2 = new QLabel(m_pgrpCurrent);
    m_plabSingleProtect2->setMinimumWidth(20);
    m_plabSingleProtect2->setMaximumWidth(20);
    m_plabSingleProtect2->setMinimumHeight(20);
    m_plabSingleProtect2->setMaximumHeight(20);
    m_plabSingleProtect2->setStyleSheet(g_borderStyle[0]);

	m_pdspVolt = new QDoubleSpinBox(m_pgrpCurrent);
	m_pdspVolt->setSuffix(" V");
	m_pdspVolt->setSingleStep(0.01);
	m_pdspVolt->setMaximum(999.99);
    m_pdspVolt->setValue(0.05);
    m_pdspVolt->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置通道保护电压值</p></body></html>"));


    m_pispPoreStatus = new QSpinBox(m_pgrpCurrent);
    m_pispPoreStatus->setMinimum(1);
    m_pispPoreStatus->setMaximum(10);
    m_pispPoreStatus->setSingleStep(1);
    m_pispPoreStatus->setValue(2);
    m_pispPoreStatus->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>判断孔状态的连续次数</p></body></html>"));

    m_pchkKeepShow = new QCheckBox("KeepShow", m_pgrpCurrent);
    m_pchkKeepShow->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>保持当前列表显示的通道不会被隐藏</p></body></html>"));


    m_pchkAllSeq = new QCheckBox("AllSeq", m_pgrpCurrent);
    m_pchkAllSeq->setChecked(true);
    m_pchkAllSeq->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>勾选当前页面所有测序通道</p></body></html>"));

    m_pchkShowSeq = new QCheckBox("ShowSeq", m_pgrpCurrent);
    m_pchkShowSeq->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>只显示勾选测序通道；如果勾选了保持显示复选框，则当前显示的通道无论勾选与否都不会被隐藏</p></body></html>"));

	m_pchkAllVisible = new QCheckBox("AllVisible", m_pgrpCurrent);
	m_pchkAllVisible->setChecked(true);
    m_pchkAllVisible->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>勾选当前页面所有通道</p></body></html>"));

	m_pchkShowVisible = new QCheckBox("ShowVisible", m_pgrpCurrent);
    m_pchkShowVisible->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>只显示勾选通道；如果勾选了保持显示复选框，则当前显示的通道无论勾选与否都不会被隐藏</p></body></html>"));

	m_pcmbShowChan = new QComboBox(m_pgrpCurrent);
	m_pcmbShowChan->addItem(tr("CH 1-32"));
	m_pcmbShowChan->addItem(tr("CH 33-64"));
	m_pcmbShowChan->addItem(QStringLiteral("CH 65-96"));
	m_pcmbShowChan->addItem(QStringLiteral("CH 97-128"));
	m_pcmbShowChan->addItem(QStringLiteral("CH 129-160"));
	m_pcmbShowChan->addItem(QStringLiteral("CH 161-192"));
	m_pcmbShowChan->addItem(QStringLiteral("CH 193-224"));
	m_pcmbShowChan->addItem(QStringLiteral("CH 225-256"));
	m_pcmbShowChan->addItem(tr("AllChannel"));
	m_pcmbShowChan->addItem(tr("First 128"));
	m_pcmbShowChan->addItem(tr("Last 128"));
	m_pcmbShowChan->setCurrentIndex(8);


    QGroupBox* pgrpFilter = new QGroupBox("Filter (pA)", m_pgrpCurrent);

    m_pchkFilter = new QCheckBox("Add", pgrpFilter);
    m_pchkFilter->setChecked(true);
    m_pchkFilter->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否加入电流值的筛选</p></body></html>"));

	m_pdspFilterMin = new QDoubleSpinBox(pgrpFilter);
	m_pdspFilterMin->setMinimum(-9999.99);
	m_pdspFilterMin->setMaximum(9999.99);
	m_pdspFilterMin->setValue(-15.0);
    m_pdspFilterMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>电流筛选下限</p></body></html>"));

	m_pdspFilterMax = new QDoubleSpinBox(pgrpFilter);
	m_pdspFilterMax->setMinimum(-9999.99);
	m_pdspFilterMax->setMaximum(9999.99);
	m_pdspFilterMax->setValue(15.0);
    m_pdspFilterMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>电流筛选上限</p></body></html>"));

	m_plabFilterCount = new QLabel(QString::number(CHANNEL_NUM), pgrpFilter);
    m_plabFilterCount->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>显示符合筛选条件的通道个数</p></body></html>"));


    m_pchkFilterSTD = new QCheckBox("STD", pgrpFilter);
    m_pchkFilterSTD->setChecked(true);
    m_pchkFilterSTD->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否加入STD值的筛选</p></body></html>"));

    m_pdspFilterSTDMin = new QDoubleSpinBox(pgrpFilter);
    m_pdspFilterSTDMin->setDecimals(4);
    m_pdspFilterSTDMin->setMinimum(0.0);
    m_pdspFilterSTDMin->setMaximum(999.9999);
    m_pdspFilterSTDMin->setValue(0.0001);
    m_pdspFilterSTDMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>电流STD筛选下限</p></body></html>"));

    m_pdspFilterSTD = new QDoubleSpinBox(pgrpFilter);
    m_pdspFilterSTD->setDecimals(4);
    m_pdspFilterSTD->setMinimum(0.0);
    m_pdspFilterSTD->setMaximum(999.9999);
    m_pdspFilterSTD->setValue(5.0);
    m_pdspFilterSTD->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>电流STD筛选上限</p></body></html>"));

	m_pbtnFilter = new QPushButton("Filter", pgrpFilter);
    m_pbtnFilter->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>执行筛选</p></body></html>"));


    QGridLayout* gdlayout1 = new QGridLayout();
    gdlayout1->addWidget(m_plabSingleLimit, 0, 0);
    gdlayout1->addWidget(m_pdspSingleLimit, 0, 1);
    gdlayout1->addWidget(m_pbtnEnableAuto, 0, 2);
    gdlayout1->addWidget(m_plabMultiLimit, 1, 0);
    gdlayout1->addWidget(m_pdspMultiLimit, 1, 1);
    gdlayout1->addWidget(m_pbtnEnableProtect, 1, 2);
    gdlayout1->addWidget(m_plabSaturatedLimit, 2, 0);
    gdlayout1->addWidget(m_pdspSaturatedLimit, 2, 1);
    gdlayout1->addWidget(m_pbtnOnlySinglePore, 2, 2);


#if 0
	QVBoxLayout* verlayout1 = new QVBoxLayout();
	verlayout1->addWidget(m_plabNonembed);
	verlayout1->addWidget(m_plabMultiMar);

	QVBoxLayout* verlayout2 = new QVBoxLayout();
	verlayout2->addWidget(m_plabNonembed2);
	verlayout2->addWidget(m_plabMultiMar2);

	QVBoxLayout* verlayout3 = new QVBoxLayout();
	verlayout3->addWidget(m_plabSingle);
	verlayout3->addWidget(m_plabCloseVlot);

	QVBoxLayout* verlayout4 = new QVBoxLayout();
	verlayout4->addWidget(m_plabSingle2);
	verlayout4->addWidget(m_plabCloseVlot2);

	QVBoxLayout* verlayout5 = new QVBoxLayout();
	verlayout5->addWidget(m_pdspVolt);
	verlayout5->addWidget(m_pchkKeepShow);
#endif

    QGridLayout* gdlayout2 = new QGridLayout();
    gdlayout2->addWidget(m_plabCloseVlot, 0, 0);
    gdlayout2->addWidget(m_plabCloseVlot2, 0, 1, Qt::AlignLeft);
    gdlayout2->addWidget(m_plabSingleProtect, 0, 2);
    gdlayout2->addWidget(m_plabSingleProtect2, 0, 3, Qt::AlignLeft);
    gdlayout2->addWidget(m_pdspVolt, 0, 4);


	QHBoxLayout* horlayout3 = new QHBoxLayout();
	//horlayout3->addLayout(verlayout1);
	//horlayout3->addLayout(verlayout2);
	//horlayout3->addStretch(1);
	//horlayout3->addLayout(verlayout3);
	//horlayout3->addLayout(verlayout4);
	//horlayout3->addStretch(1);
	//horlayout3->addLayout(verlayout5);

    horlayout3->addWidget(m_pispPoreStatus);
	horlayout3->addStretch(1);
	horlayout3->addWidget(m_pchkAllSeq);
	horlayout3->addWidget(m_pchkShowSeq);
	horlayout3->addStretch(1);
    horlayout3->addWidget(m_pchkKeepShow);


	QHBoxLayout* horlayout4 = new QHBoxLayout();
	horlayout4->addWidget(m_pchkAllVisible);
	horlayout4->addWidget(m_pchkShowVisible);
	horlayout4->addWidget(m_pcmbShowChan);


    QGridLayout* gdlayout5 = new QGridLayout();
    gdlayout5->addWidget(m_pchkFilter, 0, 0);
    gdlayout5->addWidget(m_pdspFilterMin, 0, 1);
    gdlayout5->addWidget(m_pdspFilterMax, 0, 2);
    gdlayout5->addWidget(m_plabFilterCount, 0, 3, Qt::AlignCenter);
    gdlayout5->addWidget(m_pchkFilterSTD, 1, 0);
    gdlayout5->addWidget(m_pdspFilterSTDMin, 1, 1);
    gdlayout5->addWidget(m_pdspFilterSTD, 1, 2);
    gdlayout5->addWidget(m_pbtnFilter, 1, 3);

    pgrpFilter->setLayout(gdlayout5);


	QVBoxLayout* verlayout9 = new QVBoxLayout();
	verlayout9->setContentsMargins(5, 0, -1, 5);
	verlayout9->addLayout(gdlayout1);
	verlayout9->addLayout(gdlayout2);
	verlayout9->addLayout(horlayout3);
	verlayout9->addLayout(horlayout4);
    verlayout9->addWidget(pgrpFilter);

	m_pgrpCurrent->setLayout(verlayout9);


    connect(m_pbtnEnableAuto, &QPushButton::clicked, this, &ControlView::onClickEnableAuto);
    connect(m_pbtnEnableProtect, &QPushButton::clicked, this, &ControlView::onClickEnableProtect);
    //connect(m_pbtnAutoZero, &QPushButton::clicked, this, &ControlView::onClickAutoSetZero);
	//connect(m_pchkAllAuto, &QCheckBox::stateChanged, this, &ControlView::onCheckAllAuto);
    connect(m_pchkAllSeq, & QCheckBox::stateChanged, this, & ControlView::onCheckAllSeq);
	connect(m_pchkShowSeq, &QCheckBox::stateChanged, this, &ControlView::onCheckShowSeq);
	connect(m_pchkAllVisible, &QCheckBox::stateChanged, this, &ControlView::onCheckAllVisible);
	connect(m_pchkShowVisible, &QCheckBox::stateChanged, this, &ControlView::onCheckShowVisible);
	connect(m_pcmbShowChan, SIGNAL(activated(int)), this, SLOT(onPlotPageViewChange(int)));
	connect(m_pbtnOnlySinglePore, &QPushButton::clicked, this, &ControlView::onClickShowSinglePore);
	connect(m_pchkFilter, &QCheckBox::stateChanged, this, &ControlView::onCheckCurrentFilter);
	connect(m_pchkFilterSTD, &QCheckBox::stateChanged, this, &ControlView::onCheckStdFilter);
	connect(m_pbtnFilter, &QPushButton::clicked, this, &ControlView::onClickFilter);

	return false;
}

bool ControlView::InitVoltageCtrl(void)
{
	m_pgrpVoltage = new QGroupBox("Voltage", this);
    //m_pgrpVoltage->setContentsMargins(0, 0, 1, 1);

    m_ptabVolt = new QTabWidget(m_pgrpVoltage);

    QWidget* pVolt = new QWidget(m_pgrpVoltage);
    QWidget* pDegating = new QWidget(m_pgrpVoltage);
    QWidget* pAdjust = new QWidget(m_pgrpVoltage);
    QWidget* pPolymer = new QWidget(m_pgrpVoltage);
    QWidget* pRotation = new QWidget(m_pgrpVoltage);
    QWidget* pSimulation = new QWidget(m_pgrpVoltage);

    m_ptabVolt->addTab(pVolt, "Volt");
    m_ptabVolt->addTab(pDegating, "Degating");
    m_ptabVolt->addTab(pAdjust, "Adjust");
    m_ptabVolt->addTab(pPolymer, "Polymer");
    m_ptabVolt->addTab(pRotation, "Rotation");
    m_ptabVolt->addTab(pSimulation, "Simulation");//simulation
    m_ptabVolt->setMaximumHeight(260);
    //m_ptabVolt->setFixedHeight(200);


    //tabitem volt
    {
        m_pradAllChannel = new QRadioButton("All Channel", pVolt);
        m_pradSingleChannel = new QRadioButton("Single Channel", pVolt);
        m_pradAllChannel->setChecked(true);
        m_pradAllChannel->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>所有通道施加电压</p></body></html>"));
        m_pradSingleChannel->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>勾选通道施加电压</p></body></html>"));

        m_plabVoltType = new QLabel(tr("Volt Type:"), pVolt);//Waveform type
        m_pcmbVolyType = new QComboBox(this);
        m_pcmbVolyType->addItem(tr("Direct"));
        m_pcmbVolyType->addItem(tr("Pulse"));
        m_pcmbVolyType->addItem(tr("Triangle"));
        m_pcmbVolyType->addItem(tr("Square"));

        m_pLabVoltAmpl = new QLabel(tr("Volt Amplitude:"), pVolt);//Amplitude
        m_pdspVoltAmpl = new QDoubleSpinBox(this);
        m_pdspVoltAmpl->setSuffix(" V");
        m_pdspVoltAmpl->setDecimals(2);
        m_pdspVoltAmpl->setMaximum(10.0);
        m_pdspVoltAmpl->setMinimum(-10.0);
        m_pdspVoltAmpl->setSingleStep(0.1);
        //m_pdspVolyAmpl->setValue(0.50);

        m_plabVoltFreq = new QLabel(tr("Volt Frequency:"), pVolt);//Frequency
        m_plabVoltFreq->setEnabled(false);//can't set freq now
        m_pdspVolyFreq = new QDoubleSpinBox(this);
        m_pdspVolyFreq->setDecimals(2);
        m_pdspVolyFreq->setMaximum(9999.99);
        m_pdspVolyFreq->setMinimum(-9999.99);
        m_pdspVolyFreq->setSingleStep(10.0);
        m_pdspVolyFreq->setValue(10.0);
        m_pdspVolyFreq->setEnabled(false);//can't set freq now


        m_pchkAllCheck = new QCheckBox("AllCheck", pVolt);
        //m_pchkAllChannel->setChecked(true);

        m_pbtnOpposite = new QPushButton(tr("+/-"), pVolt);

        m_pbtnVoltApply = new QPushButton(tr("Apply"), pVolt);


        QHBoxLayout* horlayout0 = new QHBoxLayout();
        horlayout0->addStretch(1);
        horlayout0->addWidget(m_pradAllChannel);
        horlayout0->addSpacing(20);
        horlayout0->addWidget(m_pradSingleChannel);
        horlayout0->addStretch(1);

        QVBoxLayout* verlayout1 = new QVBoxLayout();
        verlayout1->addWidget(m_plabVoltType);
        verlayout1->addWidget(m_pLabVoltAmpl);
        verlayout1->addWidget(m_plabVoltFreq);

        QVBoxLayout* verlayout2 = new QVBoxLayout();
        verlayout2->addWidget(m_pcmbVolyType);
        verlayout2->addWidget(m_pdspVoltAmpl);
        verlayout2->addWidget(m_pdspVolyFreq);

        QHBoxLayout* horlayout1 = new QHBoxLayout();
        horlayout1->addLayout(verlayout1, 0);
        horlayout1->addLayout(verlayout2, 0);

        QHBoxLayout* horlayout2 = new QHBoxLayout();
        horlayout2->addWidget(m_pchkAllCheck);
        horlayout2->addWidget(m_pbtnOpposite);
        horlayout2->addWidget(m_pbtnVoltApply);

        QVBoxLayout* verlayout3 = new QVBoxLayout();
        verlayout3->addLayout(horlayout0);
        verlayout3->addLayout(horlayout1);
        verlayout3->addLayout(horlayout2);

        pVolt->setLayout(verlayout3);
    }

    //tabitem degating
    {
        QGridLayout* pGridLayout = new QGridLayout(pDegating);
        pGridLayout->setSpacing(0);
        pGridLayout->setContentsMargins(0, 0, 0, 0);
        QGroupBox* pGroupBox = new QGroupBox(pDegating);
        pGroupBox->setContentsMargins(0, 0, 0, 0);
        pGridLayout->addWidget(pGroupBox, 0, 0, 1, 1);
        QHBoxLayout* pHBoxLayout = new QHBoxLayout(pGroupBox);
        pHBoxLayout->setSpacing(0);
        pHBoxLayout->setContentsMargins(0, 0, 0, 0);

        QWidget* pDegatingCtrl = new QWidget(pGroupBox);
        QScrollArea* pDegatingArea = new QScrollArea(pGroupBox);
        //pDegatingArea->setContentsMargins(0, 0, 0, 0);
        pDegatingArea->setWidgetResizable(true);

        m_pgrpDegatingBase = new QGroupBox("Degating", pDegatingCtrl);
        m_pgrpDegatingBase->setContentsMargins(3, 1, 3, 0);
        m_plabDegatingVolt = new QLabel("Degating Volt:", m_pgrpDegatingBase);
        m_pdspDegatingVolt = new QDoubleSpinBox(m_pgrpDegatingBase);
        m_pdspDegatingVolt->setDecimals(2);
        m_pdspDegatingVolt->setSuffix(" V");
        m_pdspDegatingVolt->setMinimum(-99.99);
        m_pdspDegatingVolt->setMaximum(99.99);
        m_pdspDegatingVolt->setSingleStep(0.01);
        m_pdspDegatingVolt->setValue(-0.1);
        m_pdspDegatingVolt->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置Degating电压</p></body></html>"));

        m_plabCyclePeriod = new QLabel("Cycle Period:", m_pgrpDegatingBase);
        m_pispCyclePeriod = new QSpinBox(m_pgrpDegatingBase);
        m_pispCyclePeriod->setSuffix(" s");
        m_pispCyclePeriod->setMinimum(1);
        m_pispCyclePeriod->setMaximum(99);
        m_pispCyclePeriod->setSingleStep(1);
        m_pispCyclePeriod->setValue(2);
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
        m_pispSteppingTime = new QSpinBox(m_pgrpDegatingBase);
        m_pispSteppingTime->setSuffix(" ms");
        m_pispSteppingTime->setMinimum(0);
        m_pispSteppingTime->setMaximum(10000);
        m_pispSteppingTime->setSingleStep(10);
        m_pispSteppingTime->setValue(0);
        m_pispSteppingTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置gating检测增量时间（下一次比上一次增加的恢复检测时间）</p></body></html>"));

        m_plabAllowTimes = new QLabel("Allow Times:", m_pgrpDegatingBase);
        m_pispAllowTimes = new QSpinBox(m_pgrpDegatingBase);
        //m_pispAllowTimes->setSuffix(" num");
        m_pispAllowTimes->setMinimum(0);
        m_pispAllowTimes->setMaximum(100);
        m_pispAllowTimes->setSingleStep(1);
        m_pispAllowTimes->setValue(10);
        m_pispAllowTimes->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间内的Degating次数，规定时间内达到此次数则关断该通道。0为表示此功能不开启</p></body></html>"));

        m_plabLimitCycle = new QLabel("Limit Cycle:", m_pgrpDegatingBase);
        m_pispLimitCycle = new QSpinBox(m_pgrpDegatingBase);
        //m_pispLimitCycle->setSuffix(" num");
        m_pispLimitCycle->setMinimum(5);
        m_pispLimitCycle->setMaximum(10000);
        m_pispLimitCycle->setSingleStep(1);
        m_pispLimitCycle->setValue(14);
        m_pispLimitCycle->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间周期，在此周期内判定Degating次数</p></body></html>"));

        m_pchkAutoseqGating = new QCheckBox("Autoseq", m_pgrpDegatingBase);
        //m_pchkAutoseqGating->setChecked(true);
        m_pchkAutoseqGating->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否启用AutoseqGating算法</p></body></html>"));

        m_pbtnDegatingEnable = new QPushButton("Enable", m_pgrpDegatingBase);
        m_pbtnDegatingEnable->setCheckable(true);
        m_pbtnDegatingEnable->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>启用/禁用gating监测功能</p></body></html>"));

        m_pbtnDegatingApply = new QPushButton("Apply", m_pgrpDegatingBase);
        m_pbtnDegatingApply->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置参数应用到gating功能内</p></body></html>"));

        //m_pchkDegatingAllCheck = new QCheckBox("AllCheck", pgrpDegatingBase);
        //m_pchkDegatingAllCheck->setEnabled(false);
        //m_pchkDegatingAllCheck->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>勾选需要自动Degating的通道</p></body></html>"));

        m_pbtnCalcDegating = new QPushButton("StartAvgCal", m_pgrpDegatingBase);
        m_pbtnCalcDegating->setCheckable(true);
        m_pbtnCalcDegating->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>开始/停止Degating在设定周期内次数统计</p></body></html>"));

        m_plabCalcDegating = new QLabel("Avg:0", m_pgrpDegatingBase);
        m_plabCalcDegating->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>统计一定时间内所有有效测序通道的Degating总次数与有效通道数的比值</p></body></html>"));


        //Advanced
        m_pgrpDegatingAdvanced = new QGroupBox("Advanced", pDegatingCtrl);
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


        QGridLayout* gdlayout1 = new QGridLayout();
        gdlayout1->setContentsMargins(1, 8, 1, 1);
        gdlayout1->addWidget(m_plabDegatingVolt, 0, 0);
        gdlayout1->addWidget(m_plabCyclePeriod, 1, 0);
        gdlayout1->addWidget(m_plabDurationTime, 2, 0);
        gdlayout1->addWidget(m_plabSteppingTime, 3, 0);
        gdlayout1->addWidget(m_plabAllowTimes, 4, 0);
        gdlayout1->addWidget(m_plabLimitCycle, 5, 0);
        gdlayout1->addWidget(m_pdspDegatingVolt, 0, 1);
        gdlayout1->addWidget(m_pispCyclePeriod, 1, 1);
        gdlayout1->addWidget(m_pispDurationTime, 2, 1);
        gdlayout1->addWidget(m_pispSteppingTime, 3, 1);
        gdlayout1->addWidget(m_pispAllowTimes, 4, 1);
        gdlayout1->addWidget(m_pispLimitCycle, 5, 1);

        QHBoxLayout* horlayout2 = new QHBoxLayout();
        //horlayout2->setContentsMargins(1, 1, 1, 1);
        horlayout2->addWidget(m_pchkAutoseqGating);
        horlayout2->addWidget(m_pbtnDegatingEnable);
        //horlayout2->addWidget(m_pchkDegatingAllCheck);
        horlayout2->addWidget(m_pbtnDegatingApply);

        QHBoxLayout* horlayout3 = new QHBoxLayout();
        horlayout3->addWidget(m_pbtnCalcDegating);
        horlayout3->addWidget(m_plabCalcDegating);

        QVBoxLayout* verlayout4 = new QVBoxLayout();
        //verlayout4->setContentsMargins(1, 6, 1, 1);
        verlayout4->addLayout(gdlayout1);
        verlayout4->addLayout(horlayout2);
        verlayout4->addLayout(horlayout3);

        m_pgrpDegatingBase->setLayout(verlayout4);


        int cnt = 0;
        //Advanced
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


        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->setContentsMargins(3, 1, 3, 1);
        verlayout9->addWidget(m_pgrpDegatingBase);
        verlayout9->addWidget(m_pgrpDegatingAdvanced);

        pDegatingCtrl->setLayout(verlayout9);
        pDegatingArea->setWidget(pDegatingCtrl);
        pHBoxLayout->addWidget(pDegatingArea);
    }

    //tabitem adjust
    {
        QGridLayout* pGridLayoutAdjust = new QGridLayout(pAdjust);
        pGridLayoutAdjust->setSpacing(0);
        pGridLayoutAdjust->setContentsMargins(0, 0, 0, 0);
        QGroupBox* pGroupBoxAdjust = new QGroupBox(pAdjust);
        pGroupBoxAdjust->setContentsMargins(0, 0, 0, 0);
        pGridLayoutAdjust->addWidget(pGroupBoxAdjust, 0, 0, 1, 1);
        QHBoxLayout* pHBoxLayoutAdjust = new QHBoxLayout(pGroupBoxAdjust);
        pHBoxLayoutAdjust->setSpacing(0);
        pHBoxLayoutAdjust->setContentsMargins(0, 0, 0, 0);

        QWidget* pAdjustCtrl = new QWidget(pGroupBoxAdjust);
        QScrollArea* pAdjustArea = new QScrollArea(pGroupBoxAdjust);
        pAdjustArea->setWidgetResizable(true);


        m_pgrpZeroAdjust = new QGroupBox("ZeroAdjust", pAdjustCtrl);
        m_pgrpZeroAdjust->setContentsMargins(3, 1, 3, 0);
        //ZeroAdjust
        m_plabBackwardVoltage = new QLabel("BackwardVolt:", m_pgrpZeroAdjust);
        m_pdspBackwardVoltage = new QDoubleSpinBox(m_pgrpZeroAdjust);
        m_pdspBackwardVoltage->setMinimum(-99.99);
        m_pdspBackwardVoltage->setMaximum(99.99);
        m_pdspBackwardVoltage->setSingleStep(0.01);
        m_pdspBackwardVoltage->setSuffix(" V");
        m_pdspBackwardVoltage->setValue(-0.18);
        m_pdspBackwardVoltage->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置调零前的回调电压值</p></body></html>"));

        m_plabBackwardDuration = new QLabel("BackwardTime:", m_pgrpZeroAdjust);
        m_pdspBackwardDuration = new QDoubleSpinBox(m_pgrpZeroAdjust);
        m_pdspBackwardDuration->setMinimum(1.00);
        m_pdspBackwardDuration->setMaximum(999.99);
        m_pdspBackwardDuration->setSuffix(" s");
        m_pdspBackwardDuration->setValue(1.00);
        m_pdspBackwardDuration->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置调零前的回调电压作用时间</p></body></html>"));

        m_plabZeroVoltDuration = new QLabel("ZeroVoltTime:", m_pgrpZeroAdjust);
        m_pdspZeroVoltDuration = new QDoubleSpinBox(m_pgrpZeroAdjust);
        m_pdspZeroVoltDuration->setMinimum(3.00);
        m_pdspZeroVoltDuration->setMaximum(999.99);
        m_pdspZeroVoltDuration->setSuffix(" s");
        m_pdspZeroVoltDuration->setValue(3.00);
        m_pdspZeroVoltDuration->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置所有通道为0电压的作用时间</p></body></html>"));

        m_plabZeroAdjustTime = new QLabel("AdjustTime:", m_pgrpZeroAdjust);
        m_pdspZeroAdjustTime = new QDoubleSpinBox(m_pgrpZeroAdjust);
        m_pdspZeroAdjustTime->setMinimum(6.00);
        m_pdspZeroAdjustTime->setMaximum(999.99);
        m_pdspZeroAdjustTime->setSuffix(" s");
        m_pdspZeroAdjustTime->setValue(6.00);
        m_pdspZeroAdjustTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置调零电压后的作用时间</p></body></html>"));


        m_pbtnZeroAdjustDefault = new QPushButton("Default", m_pgrpZeroAdjust);

        m_pbtnZeroAdjust = new QPushButton("ZeroAdjust", m_pgrpZeroAdjust);


        QGridLayout* gdlayout = new QGridLayout();
        gdlayout->setContentsMargins(1, 8, 1, 1);
        gdlayout->addWidget(m_plabBackwardVoltage, 0, 0);
        gdlayout->addWidget(m_plabBackwardDuration, 1, 0);
        gdlayout->addWidget(m_plabZeroVoltDuration, 2, 0);
        gdlayout->addWidget(m_plabZeroAdjustTime, 3, 0);
        gdlayout->addWidget(m_pdspBackwardVoltage, 0, 1);
        gdlayout->addWidget(m_pdspBackwardDuration, 1, 1);
        gdlayout->addWidget(m_pdspZeroVoltDuration, 2, 1);
        gdlayout->addWidget(m_pdspZeroAdjustTime, 3, 1);

        QHBoxLayout* horlayout6 = new QHBoxLayout();
        horlayout6->addSpacing(20);
        horlayout6->addWidget(m_pbtnZeroAdjustDefault);
        horlayout6->addStretch(1);
        horlayout6->addWidget(m_pbtnZeroAdjust);
        horlayout6->addSpacing(20);

        QVBoxLayout* verlayout6 = new QVBoxLayout();
        verlayout6->addLayout(gdlayout);
        verlayout6->addLayout(horlayout6);

        m_pgrpZeroAdjust->setLayout(verlayout6);


        m_pgrpAutoAdjust = new QGroupBox("AutoZeroAdjust", this);
        m_pgrpAutoAdjust->setContentsMargins(3, 1, 3, 0);
        //Auto ZeroAdjust
        m_plabIntervalsTimer = new QLabel("Intervals:", m_pgrpAutoAdjust);
        m_pispIntervalsTimer = new QSpinBox(m_pgrpAutoAdjust);
        m_pispIntervalsTimer->setMinimum(15);
        m_pispIntervalsTimer->setMaximum(86399);//23:59:59
        m_pispIntervalsTimer->setSuffix(" s");
        m_pispIntervalsTimer->setValue(80);

        SetTimehms();

        m_plcdIntervalsTimer = new QLCDNumber(8, m_pgrpAutoAdjust);
        m_plcdIntervalsTimer->setSegmentStyle(QLCDNumber::Flat);
        m_plcdIntervalsTimer->setFrameShape(QFrame::StyledPanel);
        m_plcdIntervalsTimer->display(m_tmAdjust.toString("hh:mm:ss"));


        m_pbtnZeroAdjustStart = new QPushButton("Start", m_pgrpAutoAdjust);
        m_pbtnZeroAdjustStart->setCheckable(true);

        m_pbtnZeroAdjustReset = new QPushButton("Reset", m_pgrpAutoAdjust);


        QHBoxLayout* horlayout1 = new QHBoxLayout();
        horlayout1->setContentsMargins(1, 10, 1, 1);
        horlayout1->addWidget(m_plabIntervalsTimer);
        horlayout1->addWidget(m_pispIntervalsTimer);
        horlayout1->addWidget(m_plcdIntervalsTimer);
        horlayout1->addStretch(1);

        QHBoxLayout* horlayout2 = new QHBoxLayout();
        //horlayout2->addWidget(m_plcdIntervalsTimer);
        //horlayout2->addStretch(1);
        horlayout2->addSpacing(20);
        horlayout2->addWidget(m_pbtnZeroAdjustStart);
        horlayout2->addStretch(1);
        horlayout2->addWidget(m_pbtnZeroAdjustReset);
        horlayout2->addSpacing(20);

        //QHBoxLayout* horlayout3 = new QHBoxLayout();
        //horlayout3->addWidget(m_pbtnZeroAdjustStart);
        //horlayout3->addWidget(m_pbtnZeroAdjustReset);
        //horlayout3->addStretch(1);

        QVBoxLayout* verlayout1 = new QVBoxLayout();
        verlayout1->addStretch(3);
        verlayout1->addLayout(horlayout1);
        verlayout1->addStretch(1);
        verlayout1->addLayout(horlayout2);
        verlayout1->addStretch(1);
        //verlayout1->addLayout(horlayout3);
        //verlayout1->addStretch(3);

        m_pgrpAutoAdjust->setLayout(verlayout1);


        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->setContentsMargins(3, 1, 3, 1);
        verlayout9->addWidget(m_pgrpZeroAdjust);
        verlayout9->addWidget(m_pgrpAutoAdjust);

        pAdjustCtrl->setLayout(verlayout9);
        pAdjustArea->setWidget(pAdjustCtrl);
        pHBoxLayoutAdjust->addWidget(pAdjustArea);
    }

    //tabitem polymer
    {
        QGridLayout* pGridLayout2 = new QGridLayout(pPolymer);
        pGridLayout2->setSpacing(0);
        pGridLayout2->setContentsMargins(0, 0, 0, 0);
        QGroupBox* pGroupBox2 = new QGroupBox(pPolymer);
        pGroupBox2->setContentsMargins(0, 0, 0, 0);
        pGridLayout2->addWidget(pGroupBox2, 0, 0, 1, 1);
        QHBoxLayout* pHBoxLayout2 = new QHBoxLayout(pGroupBox2);
        pHBoxLayout2->setSpacing(0);
        pHBoxLayout2->setContentsMargins(0, 0, 0, 0);

        QWidget* pPolymerCtrl = new QWidget(pGroupBox2);
        QScrollArea* pPolymerArea = new QScrollArea(pGroupBox2);
        pPolymerArea->setWidgetResizable(true);


        m_plabPoreLimit = new QLabel("Pore Limit:", pPolymerCtrl);
        m_pdspPoreLimit = new QDoubleSpinBox(pPolymerCtrl);
        m_pdspPoreLimit->setSuffix(" pA");
        m_pdspPoreLimit->setDecimals(2);
        m_pdspPoreLimit->setMinimum(0.0);
        m_pdspPoreLimit->setMaximum(5000.0);
        m_pdspPoreLimit->setSingleStep(100.0);
        m_pdspPoreLimit->setValue(100.0);

        m_plabPoreVolt = new QLabel("Pore Volt:", pPolymerCtrl);
        m_pdspPoreVolt = new QDoubleSpinBox(pPolymerCtrl);
        m_pdspPoreVolt->setSuffix(" V");
        m_pdspPoreVolt->setDecimals(3);
        m_pdspPoreVolt->setMinimum(0.0);
        m_pdspPoreVolt->setMaximum(5.0);
        m_pdspPoreVolt->setSingleStep(1.0);
        m_pdspPoreVolt->setValue(0.0);

        m_plabVoltStart = new QLabel("Volt Start:", pPolymerCtrl);
        m_pdspVoltStart = new QDoubleSpinBox(pPolymerCtrl);
        m_pdspVoltStart->setSuffix(" V");
        m_pdspVoltStart->setDecimals(3);
        m_pdspVoltStart->setMinimum(0.0);
        m_pdspVoltStart->setMaximum(5.0);
        m_pdspVoltStart->setSingleStep(1.0);
        m_pdspVoltStart->setValue(0.15);

        m_plabVoltEnd = new QLabel("Volt End:", pPolymerCtrl);
        m_pdspVoltEnd = new QDoubleSpinBox(pPolymerCtrl);
        m_pdspVoltEnd->setSuffix(" V");
        m_pdspVoltEnd->setDecimals(3);
        m_pdspVoltEnd->setMinimum(0.0);
        m_pdspVoltEnd->setMaximum(5.0);
        m_pdspVoltEnd->setSingleStep(1.0);
        m_pdspVoltEnd->setValue(0.45);

        m_plabVoltStep = new QLabel("Volt Step:", pPolymerCtrl);
        m_pdspVoltStep = new QDoubleSpinBox(pPolymerCtrl);
        m_pdspVoltStep->setSuffix(" V");
        m_pdspVoltStep->setDecimals(3);
        m_pdspVoltStep->setMinimum(0.0);
        m_pdspVoltStep->setMaximum(5.0);
        m_pdspVoltStep->setSingleStep(1.0);
        m_pdspVoltStep->setValue(0.01);

        m_plabTimeStep = new QLabel("Time Step:", pPolymerCtrl);
        m_pdspTimeStep = new QDoubleSpinBox(pPolymerCtrl);
        m_pdspTimeStep->setSuffix(" s");
        m_pdspTimeStep->setDecimals(2);
        m_pdspTimeStep->setMinimum(0.1);
        m_pdspTimeStep->setMaximum(5000.0);
        m_pdspTimeStep->setSingleStep(1.0);
        m_pdspTimeStep->setValue(1.0);

        m_pbtnPolymerDefault = new QPushButton("Default", pPolymerCtrl);
        m_pbtnPolymerStart = new QPushButton("Start Polymer", pPolymerCtrl);
        m_pbtnPolymerStart->setCheckable(true);


        QGridLayout* gdlayout1 = new QGridLayout();
        gdlayout1->addWidget(m_plabPoreLimit, 0, 0);
        gdlayout1->addWidget(m_plabPoreVolt, 1, 0);
        gdlayout1->addWidget(m_plabVoltStart, 2, 0);
        gdlayout1->addWidget(m_plabVoltEnd, 3, 0);
        gdlayout1->addWidget(m_plabVoltStep, 4, 0);
        gdlayout1->addWidget(m_plabTimeStep, 5, 0);
        gdlayout1->addWidget(m_pdspPoreLimit, 0, 1);
        gdlayout1->addWidget(m_pdspPoreVolt, 1, 1);
        gdlayout1->addWidget(m_pdspVoltStart, 2, 1);
        gdlayout1->addWidget(m_pdspVoltEnd, 3, 1);
        gdlayout1->addWidget(m_pdspVoltStep, 4, 1);
        gdlayout1->addWidget(m_pdspTimeStep, 5, 1);

        QHBoxLayout* horlayout2 = new QHBoxLayout();
        horlayout2->addSpacing(20);
        horlayout2->addWidget(m_pbtnPolymerDefault);
        horlayout2->addStretch(1);
        horlayout2->addWidget(m_pbtnPolymerStart);
        horlayout2->addSpacing(20);

        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addLayout(gdlayout1);
        verlayout9->addLayout(horlayout2);

        pPolymerCtrl->setLayout(verlayout9);
        pPolymerArea->setWidget(pPolymerCtrl);
        pHBoxLayout2->addWidget(pPolymerArea);
    }

    //rotation seqencing
    {
        QGridLayout* pGridLayout2 = new QGridLayout(pRotation);
        pGridLayout2->setSpacing(0);
        pGridLayout2->setContentsMargins(0, 0, 0, 0);
        QGroupBox* pGroupBox2 = new QGroupBox(pRotation);
        pGroupBox2->setContentsMargins(0, 0, 0, 0);
        pGridLayout2->addWidget(pGroupBox2, 0, 0, 1, 1);
        QHBoxLayout* pHBoxLayout2 = new QHBoxLayout(pGroupBox2);
        pHBoxLayout2->setSpacing(0);
        pHBoxLayout2->setContentsMargins(0, 0, 0, 0);

        QWidget* pRotationCtrl = new QWidget(pGroupBox2);
        QScrollArea* pRotationArea = new QScrollArea(pGroupBox2);
        pRotationArea->setWidgetResizable(true);


        m_plabRotationTime = new QLabel("Intervals", pRotationCtrl);
        m_pispRotationTime = new QSpinBox(pRotationCtrl);
        m_pispRotationTime->setSuffix(" min");
        m_pispRotationTime->setMinimum(1);
        m_pispRotationTime->setMaximum(1439);//23:59:00
        m_pispRotationTime->setValue(6);

        SetTimehmsMin();

        m_plcdRotationTime = new QLCDNumber(8, pRotationCtrl);
        m_plcdRotationTime->setSegmentStyle(QLCDNumber::Flat);
        m_plcdRotationTime->setFrameShape(QFrame::StyledPanel);
        m_plcdRotationTime->display(m_tmRotation.toString("hh:mm:ss"));

        m_plabSeqencingVolt = new QLabel("SeqencingVolt", pRotationCtrl);
        m_pdspSeqencingVolt = new QDoubleSpinBox(pRotationCtrl);
        m_pdspSeqencingVolt->setDecimals(2);
        m_pdspSeqencingVolt->setSuffix(" V");
        m_pdspSeqencingVolt->setMinimum(-9999.99);
        m_pdspSeqencingVolt->setMaximum(9999.99);
        m_pdspSeqencingVolt->setValue(0.18);

        m_plabWaitingVolt = new QLabel("WaitingVolt", pRotationCtrl);
        m_pdspWaitingVolt = new QDoubleSpinBox(pRotationCtrl);
        m_pdspWaitingVolt->setDecimals(2);
        m_pdspWaitingVolt->setSuffix(" V");
        m_pdspWaitingVolt->setMinimum(-9999.99);
        m_pdspWaitingVolt->setMaximum(9999.99);
        m_pdspWaitingVolt->setValue(0.0);

        m_plabCurseqChannel = new QLabel("CurseqChannel", pRotationCtrl);
        m_pcmbCurseqChannel = new QComboBox(pRotationCtrl);
        m_pcmbCurseqChannel->addItem("Even");
        m_pcmbCurseqChannel->addItem("Odd");

        m_pbtnStartRotation = new QPushButton("Start Rotation", pRotationCtrl);
        m_pbtnStartRotation->setCheckable(true);

        m_pbtnResetRotation = new QPushButton("Reset", pRotationCtrl);


        int row = 0;
        int col = 0;
        QGridLayout* gdlayout = new QGridLayout();
        gdlayout->addWidget(m_plabRotationTime, row, col++);
        gdlayout->addWidget(m_pispRotationTime, row, col++);

        ++row;
        col = 1;
        gdlayout->addWidget(m_plcdRotationTime, row, col++);

        ++row;
        col = 0;
        gdlayout->addWidget(m_plabSeqencingVolt, row, col++);
        gdlayout->addWidget(m_pdspSeqencingVolt, row, col++);

        ++row;
        col = 0;
        gdlayout->addWidget(m_plabWaitingVolt, row, col++);
        gdlayout->addWidget(m_pdspWaitingVolt, row, col++);

        ++row;
        col = 0;
        gdlayout->addWidget(m_plabCurseqChannel, row, col++);
        gdlayout->addWidget(m_pcmbCurseqChannel, row, col++);


        QHBoxLayout* horlayout6 = new QHBoxLayout();
        horlayout6->addSpacing(20);
        horlayout6->addWidget(m_pbtnResetRotation);
        horlayout6->addStretch(1);
        horlayout6->addWidget(m_pbtnStartRotation);
        horlayout6->addSpacing(20);

        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addLayout(gdlayout);
        verlayout9->addStretch(1);
        verlayout9->addLayout(horlayout6);
        verlayout9->addStretch(2);

        pRotationCtrl->setLayout(verlayout9);
        pRotationArea->setWidget(pRotationCtrl);
        pHBoxLayout2->addWidget(pRotationArea);

    }

    //tabitem simulation
    {
        QGridLayout* pGridLayout3 = new QGridLayout(pSimulation);
        pGridLayout3->setSpacing(0);
        pGridLayout3->setContentsMargins(0, 0, 0, 0);
        QGroupBox* pGroupBox3 = new QGroupBox(pSimulation);
        pGroupBox3->setContentsMargins(0, 0, 0, 0);
        pGridLayout3->addWidget(pGroupBox3, 0, 0, 1, 1);
        QHBoxLayout* pHBoxLayout3 = new QHBoxLayout(pGroupBox3);
        pHBoxLayout3->setSpacing(0);
        pHBoxLayout3->setContentsMargins(0, 0, 0, 0);

        QWidget* pSimulationCtrl = new QWidget(pGroupBox3);
        QScrollArea* pSimulationArea = new QScrollArea(pGroupBox3);
        pSimulationArea->setWidgetResizable(true);


        m_plabVoltStart3 = new QLabel("Volt Start:", pSimulationCtrl);
        m_pdspVoltStart3 = new QDoubleSpinBox(pSimulationCtrl);
        m_pdspVoltStart3->setSuffix(" V");
        m_pdspVoltStart3->setDecimals(2);
        m_pdspVoltStart3->setMinimum(-5.0);
        m_pdspVoltStart3->setMaximum(5.0);
        m_pdspVoltStart3->setSingleStep(1.0);
        m_pdspVoltStart3->setValue(0.0);

        m_plabVoltEnd3 = new QLabel("Volt End:", pSimulationCtrl);
        m_pdspVoltEnd3 = new QDoubleSpinBox(pSimulationCtrl);
        m_pdspVoltEnd3->setSuffix(" V");
        m_pdspVoltEnd3->setDecimals(2);
        m_pdspVoltEnd3->setMinimum(-5.0);
        m_pdspVoltEnd3->setMaximum(5.0);
        m_pdspVoltEnd3->setSingleStep(1.0);
        m_pdspVoltEnd3->setValue(1.0);

        m_plabVoltStep3 = new QLabel("Volt Step:", pSimulationCtrl);
        m_pdspVoltStep3 = new QDoubleSpinBox(pSimulationCtrl);
        m_pdspVoltStep3->setSuffix(" V");
        m_pdspVoltStep3->setDecimals(2);
        m_pdspVoltStep3->setMinimum(0.01);
        m_pdspVoltStep3->setMaximum(5.0);
        m_pdspVoltStep3->setSingleStep(0.1);
        m_pdspVoltStep3->setValue(0.1);

        m_plabTimeStep3 = new QLabel("Time Step:", pSimulationCtrl);
        m_pdspTimeStep3 = new QDoubleSpinBox(pSimulationCtrl);
        m_pdspTimeStep3->setSuffix(" s");
        m_pdspTimeStep3->setDecimals(3);
        m_pdspTimeStep3->setMinimum(0.001);
        m_pdspTimeStep3->setMaximum(60.0);
        m_pdspTimeStep3->setSingleStep(1.0);
        m_pdspTimeStep3->setValue(1.0);

        m_plabSimulationCycle = new QLabel("Cycle:", pSimulationCtrl);
        m_pispSimulationCycle = new QSpinBox(pSimulationCtrl);
        //m_pispSimulationCycle->setSuffix(" s");
        m_pispSimulationCycle->setMinimum(1);
        m_pispSimulationCycle->setMaximum(200);
        m_pispSimulationCycle->setSingleStep(1);
        m_pispSimulationCycle->setValue(30);

        m_plabRampPattern = new QLabel("Ramp Pattern:", pSimulationCtrl);
        m_pcmbRampPattern = new QComboBox(pSimulationCtrl);
        m_pcmbRampPattern->addItem("Mirror", SRP_MIRROR);
        m_pcmbRampPattern->addItem("Rise", SRP_RISE);
        m_pcmbRampPattern->addItem("Fall", SRP_FALL);

        m_plabVoltOffset = new QLabel("Volt Offset:", pSimulationCtrl);
        m_pdspVoltOffset = new QDoubleSpinBox(pSimulationCtrl);
        m_pdspVoltOffset->setSuffix(" V");
        m_pdspVoltOffset->setDecimals(2);
        m_pdspVoltOffset->setMinimum(-5.0);
        m_pdspVoltOffset->setMaximum(5.0);
        m_pdspVoltOffset->setSingleStep(1.0);
        m_pdspVoltOffset->setValue(0.0);

        m_pbtnSimulationDefault = new QPushButton("Default", pSimulationCtrl);
        m_pbtnSimulationStart = new QPushButton("Start Simulation", pSimulationCtrl);
        m_pbtnSimulationStart->setCheckable(true);


        QGridLayout* gdlayout1 = new QGridLayout();
        gdlayout1->addWidget(m_plabVoltStart3, 0, 0);
        gdlayout1->addWidget(m_plabVoltEnd3, 1, 0);
        gdlayout1->addWidget(m_plabVoltStep3, 2, 0);
        gdlayout1->addWidget(m_plabTimeStep3, 3, 0);
        gdlayout1->addWidget(m_plabSimulationCycle, 4, 0);
        gdlayout1->addWidget(m_plabRampPattern, 5, 0);
        gdlayout1->addWidget(m_plabVoltOffset, 6, 0);
        gdlayout1->addWidget(m_pdspVoltStart3, 0, 1);
        gdlayout1->addWidget(m_pdspVoltEnd3, 1, 1);
        gdlayout1->addWidget(m_pdspVoltStep3, 2, 1);
        gdlayout1->addWidget(m_pdspTimeStep3, 3, 1);
        gdlayout1->addWidget(m_pispSimulationCycle, 4, 1);
        gdlayout1->addWidget(m_pcmbRampPattern, 5, 1);
        gdlayout1->addWidget(m_pdspVoltOffset, 6, 1);

        QHBoxLayout* horlayout2 = new QHBoxLayout();
        horlayout2->addSpacing(20);
        horlayout2->addWidget(m_pbtnSimulationDefault);
        horlayout2->addStretch(1);
        horlayout2->addWidget(m_pbtnSimulationStart);
        horlayout2->addSpacing(20);

        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addLayout(gdlayout1);
        verlayout9->addLayout(horlayout2);

        pSimulationCtrl->setLayout(verlayout9);
        pSimulationArea->setWidget(pSimulationCtrl);
        pHBoxLayout3->addWidget(pSimulationArea);
    }


    QVBoxLayout* verlayout8 = new QVBoxLayout();
    verlayout8->setContentsMargins(0, 0, 0, 1);
    verlayout8->addWidget(m_ptabVolt);

    m_pgrpVoltage->setLayout(verlayout8);


    connect(m_pchkAllCheck, &QCheckBox::stateChanged, this, &ControlView::onCheckAllChannel);//QCheckBox::stateChanged
    connect(m_pbtnOpposite, &QPushButton::clicked, this, &ControlView::onClickVoltageOpposite);
    connect(m_pbtnVoltApply, &QPushButton::clicked, this, &ControlView::onClickVoltageApply);

    connect(m_pbtnDegatingEnable, &QPushButton::clicked, this, &ControlView::onClickEnableDegating);
    connect(m_pbtnDegatingApply, &QPushButton::clicked, this, &ControlView::onClickDegatingApply);
    //connect(m_pchkDegatingAllCheck, &QCheckBox::stateChanged, this, &ControlView::onCheckDegatingAllCheck);
    connect(m_pbtnCalcDegating, &QPushButton::clicked, this, &ControlView::onClickCalculation);
    connect(m_pbtnDefaultSet, &QPushButton::clicked, this, &ControlView::onClickDefaultApply);
    connect(m_pbtnAdvancedSet, &QPushButton::clicked, this, &ControlView::onClickAdvanceApply);

    connect(m_pbtnZeroAdjustDefault, &QPushButton::clicked, this, &ControlView::onClickZeroAdjustDefault);
    connect(m_pbtnZeroAdjust, &QPushButton::clicked, this, &ControlView::onClickZeroAdjust);
    connect(m_pbtnZeroAdjustStart, &QPushButton::clicked, this, &ControlView::onClickZeroAdjustStart);
    connect(m_pbtnZeroAdjustReset, &QPushButton::clicked, this, &ControlView::onClickZeroAdjustReset);

    connect(m_pbtnPolymerDefault, &QPushButton::clicked, this, &ControlView::onClickPolymerDefault);
    connect(m_pbtnPolymerStart, &QPushButton::clicked, this, &ControlView::onClickPolymerStart);

    connect(m_pbtnResetRotation, &QPushButton::clicked, this, &ControlView::onClickRotationDefault);
    connect(m_pbtnStartRotation, &QPushButton::clicked, this, &ControlView::onClickRotationStart);

    connect(m_pbtnSimulationDefault, &QPushButton::clicked, this, &ControlView::onClickSimulationDefault);
    connect(m_pbtnSimulationStart, &QPushButton::clicked, this, &ControlView::onClickSimulationStart);

	return false;
}

bool ControlView::InitControlList(void)
{
    m_plistControlShow = new QListWidget(this);
    m_vetControlListItem.resize(CHANNEL_NUM);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto widitem = new QListWidgetItem(m_plistControlShow);
        m_vetControlListItem[i] = std::make_shared<ControlListItem>(i, g_vetPenColor[i % 32], m_plistControlShow);
        //m_vetControlListItem[i]->setMinimumHeight(20);//Linux not adapt?
        m_plistControlShow->addItem(widitem);
        m_plistControlShow->setItemWidget(widitem, m_vetControlListItem[i].get());

        connect(m_vetControlListItem[i].get(), &ControlListItem::checkChangedSignal, this, &ControlView::checkChangedSlot);
        //connect(m_vetControlListItem[i].get(), SIGNAL(checkChangedSignal(int, bool)), this, SIGNAL(checkChangedSignal(int, bool)));
        connect(m_vetControlListItem[i].get(), SIGNAL(colorChangedSignal(int, const QColor&)), this, SIGNAL(colorChangedSignal(int, const QColor&)));

        //connect(m_vetControlListItem[i].get(), &ControlListItem::setZeroVoltSignal, this, &ControlView::setZeroVoltSlot);

        connect(m_vetControlListItem[i].get(), &ControlListItem::UpdateListItemSignal, this, &ControlView::UpdateListItemSlot);
        connect(m_vetControlListItem[i].get(), &ControlListItem::SetChannelDCLevelSignal, this, &ControlView::SetChannelDCLevelSlot);
        //connect(m_vetControlListItem[i].get(), SIGNAL(transmitUsbData(ushort, uint)), this, SIGNAL(transmitUsbData(ushort, uint)));
        connect(m_vetControlListItem[i].get(), SIGNAL(singleChanDegatingSignal(int, double)), this, SLOT(singleChanDegatingSlot(int, double)));
        connect(m_vetControlListItem[i].get(), SIGNAL(singleChanDegatingRecoverSignal(int)), this, SLOT(singleChanDegatingRecoverSlot(int)));
    }

    return false;
}

bool ControlView::InitLayout(void)
{
    m_grpFlowset->setVisible(true);
    m_ptabFlow->setVisible(true);
    m_pgrpCurrent->setVisible(false);
    m_pgrpVoltage->setVisible(false);

    m_phlay1 = new QHBoxLayout();
    m_phlay1->setContentsMargins(0, 0, 1, 1);
    m_phlay1->addWidget(m_pgrpCurrent);
    m_phlay1->addWidget(m_pgrpVoltage);

    m_phlay2 = new QHBoxLayout();
    m_phlay2->setContentsMargins(0, 0, 1, 1);
    m_phlay2->addWidget(m_grpFlowset);
    m_phlay2->addWidget(m_ptabFlow);


    m_pvlay = new QVBoxLayout();
    m_pvlay->setContentsMargins(5, 1, 5, 1);

    m_pvlay->addLayout(m_phlay1);
    m_pvlay->addLayout(m_phlay2);
    m_pvlay->addWidget(m_plistControlShow, 1);

    setLayout(m_pvlay);

    return false;
}

bool ControlView::SetTimehms2(void)
{
    int itm = m_pdspTimerDuration2->value() * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmMembraneForm.setHMS(ihour, imin, isec, 0);
        m_plcdTimerDuration2->display(m_tmMembraneForm.toString("hh:mm:ss"));

        return true;
    }
    return false;
}

bool ControlView::SetTimehms3(void)
{
    int itm = m_pdspTimerDuration3->value() * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmPoreInsert.setHMS(ihour, imin, isec, 0);
        m_plcdTimerDuration3->display(m_tmPoreInsert.toString("hh:mm:ss"));

        return true;
    }
    return false;
}

//bool ControlView::SetTimehms5(void)
//{
//    int itm = m_pdspTimerQC5->value() * 60;
//    if (itm > 0)
//    {
//        int ihour = itm / 3600;
//        int irest = itm % 3600;
//        int imin = irest / 60;
//        int isec = irest % 60;
//
//        m_tmSeqencingSelect.setHMS(ihour, imin, isec, 0);
//        m_plcdTimerQC5->display(m_tmSeqencingSelect.toString("hh:mm:ss"));
//
//        return true;
//    }
//    return false;
//}

bool ControlView::SetTimehms6(void)
{
    int itm = m_pdspTimerPeriod6->value() * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmMuxScanStart.setHMS(ihour, imin, isec, 0);
        m_plcdTimerPeriod6->display(m_tmMuxScanStart.toString("hh:mm:ss"));

        return true;
    }
    return false;
}

bool ControlView::LayoutCtrl(void)
{
    if (ConfigServer::GetInstance()->GetDeveloperMode() == 1)
    {
        m_grpFlowset->setVisible(false);
        m_ptabFlow->setVisible(false);
        m_pgrpCurrent->setVisible(true);
        m_pgrpVoltage->setVisible(true);
    }
    else
    {
        m_pgrpCurrent->setVisible(false);
        m_pgrpVoltage->setVisible(false);
        m_grpFlowset->setVisible(true);
        m_ptabFlow->setVisible(true);
    }
    //adjustSize();

	return false;
}

void ControlView::GetActiveChannelIndex(int curIndex)
{
    if (curIndex >= 0 && curIndex <= 7)
    {
        m_iStartIndex = 32 * curIndex;
        m_iEndIndex = 32 + m_iStartIndex;
    }
    else if (curIndex == 8)
    {
        m_iStartIndex = 0;
        m_iEndIndex = CHANNEL_NUM;
    }
    else if (curIndex == 9)
    {
        m_iStartIndex = 0;
        m_iEndIndex = 128;
    }
    else if (curIndex == 10)
    {
        m_iStartIndex = 128;
        m_iEndIndex = 256;
    }
    else
    {
        m_iStartIndex = 0;
        m_iEndIndex = CHANNEL_NUM;
    }
}

bool ControlView::LoadConfig(QSettings* pset)
{
    std::string strconfig = ConfigServer::GetUserPath() + "/etc/Conf/flowConfig.cof";
    DoLoadConfig(QString::fromStdString(strconfig));

    if (pset)
    {
        pset->beginGroup("HoleEmbedded");
        auto pore = pset->value("PoreStatusCount").toInt();
        auto sing = pset->value("SingelHoleLimit").toDouble();
        auto mult = pset->value("MultiHoleLimit").toDouble();
        auto satu = pset->value("SaturatedLimit").toDouble();
        m_pispPoreStatus->setValue(pore);
        m_pdspSingleLimit->setValue(sing);
        m_pdspMultiLimit->setValue(mult);
        m_pdspSaturatedLimit->setValue(satu);
        pset->endGroup();

        pset->beginGroup("DegatingCtrl");
        m_pdspDegatingVolt->setValue(pset->value("DegatingVolt").toDouble());
        m_pispCyclePeriod->setValue(pset->value("CyclePeriod").toInt());
        m_pispDurationTime->setValue(pset->value("DurationTime").toInt());
        m_pispSteppingTime->setValue(pset->value("SteppingTime").toInt());
        m_pispAllowTimes->setValue(pset->value("AllowTimes").toInt());
        m_pispLimitCycle->setValue(pset->value("LimitCycle").toInt());
        //Advance
        m_pdspDurationThres->setValue(pset->value("DurationThre").toDouble());
        m_pispGatingSTD->setValue(pset->value("GatingSTD").toInt());
        m_pispSignalMin->setValue(pset->value("SignalMin").toInt());
        m_pispSignalMax->setValue(pset->value("SignalMax").toInt());
        m_pispGatingMin->setValue(pset->value("GatingMin").toInt());
        m_pispGatingMax->setValue(pset->value("GatingMax").toInt());
        pset->endGroup();

        pset->beginGroup("AdjustCtrl");
        m_pdspBackwardVoltage->setValue(pset->value("BackwardVolt").toDouble());
        m_pdspBackwardDuration->setValue(pset->value("BackwardTime").toDouble());
        m_pdspZeroVoltDuration->setValue(pset->value("ZeroVoltTime").toDouble());
        m_pdspZeroAdjustTime->setValue(pset->value("AdjustTime").toDouble());
        m_pispIntervalsTimer->setValue(pset->value("Intervals").toInt());
        pset->endGroup();

        SetTimehms();
        m_plcdIntervalsTimer->display(m_tmAdjust.toString("hh:mm:ss"));


        pset->beginGroup("PolymerCtrl");
        m_pdspPoreLimit->setValue(pset->value("PoreLimit").toDouble());
        m_pdspPoreVolt->setValue(pset->value("PoreVolt").toDouble());
        m_pdspVoltStart->setValue(pset->value("VoltStart").toDouble());
        m_pdspVoltEnd->setValue(pset->value("VoltEnd").toDouble());
        m_pdspVoltStep->setValue(pset->value("VoltStep").toDouble());
        m_pdspTimeStep->setValue(pset->value("TimeStep").toDouble());
        pset->endGroup();

        pset->beginGroup("SimulationCtrl");
        m_pdspVoltStart3->setValue(pset->value("VoltStart").toDouble());
        m_pdspVoltEnd3->setValue(pset->value("VoltEnd").toDouble());
        m_pdspVoltStep3->setValue(pset->value("VoltStep").toDouble());
        m_pdspTimeStep3->setValue(pset->value("TimeStep").toDouble());
        m_pispSimulationCycle->setValue(pset->value("Cycle").toInt());
        m_pcmbRampPattern->setCurrentIndex(pset->value("RampPattern").toInt());
        m_pdspVoltOffset->setValue(pset->value("VoltOffset").toDouble());
        pset->endGroup();

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            //m_vetControlListItem[i]->SetPoreLimitVolt(pore, sing, mult, satu);

            pset->beginGroup(QString("ch%1").arg(i));

            if (pset->contains("Color"))
            {
                m_vetControlListItem[i]->SetChecked(pset->value("Visible").toBool());
                QColor color(pset->value("Color").toString());
                m_vetControlListItem[i]->SetColor(color);
            }
            m_vetControlListItem[i]->SetCheckedSeq(pset->value("Seqchan").toBool());

            pset->endGroup();
        }

        return true;
    }
    return false;
}

bool ControlView::SaveConfig(QSettings* pset)
{
    std::string strconfig = ConfigServer::GetUserPath() + "/etc/Conf/flowConfig.cof";
    DoSaveConfig(QString::fromStdString(strconfig));

    if (pset)
    {
        pset->beginGroup("HoleEmbedded");
        pset->setValue("PoreStatusCount", m_pispPoreStatus->value());
        pset->setValue("SingelHoleLimit", m_pdspSingleLimit->value());
        pset->setValue("MultiHoleLimit", m_pdspMultiLimit->value());
        pset->setValue("SaturatedLimit", m_pdspSaturatedLimit->value());
        pset->endGroup();

        pset->beginGroup("DegatingCtrl");
        pset->setValue("DegatingVolt", m_pdspDegatingVolt->value());
        pset->setValue("CyclePeriod", m_pispCyclePeriod->value());
        pset->setValue("DurationTime", m_pispDurationTime->value());
        pset->setValue("SteppingTime", m_pispSteppingTime->value());
        pset->setValue("AllowTimes", m_pispAllowTimes->value());
        pset->setValue("LimitCycle", m_pispLimitCycle->value());
        //Advance
        pset->setValue("DurationThre", m_pdspDurationThres->value());
        pset->setValue("GatingSTD", m_pispGatingSTD->value());
        pset->setValue("SignalMin", m_pispSignalMin->value());
        pset->setValue("SignalMax", m_pispSignalMax->value());
        pset->setValue("GatingMin", m_pispGatingMin->value());
        pset->setValue("GatingMax", m_pispGatingMax->value());
        pset->endGroup();

        pset->beginGroup("AdjustCtrl");
        pset->setValue("BackwardVolt", m_pdspBackwardVoltage->value());
        pset->setValue("BackwardTime", m_pdspBackwardDuration->value());
        pset->setValue("ZeroVoltTime", m_pdspZeroVoltDuration->value());
        pset->setValue("AdjustTime", m_pdspZeroAdjustTime->value());
        pset->setValue("Intervals", m_pispIntervalsTimer->value());
        pset->endGroup();

        pset->beginGroup("PolymerCtrl");
        pset->setValue("PoreLimit", m_pdspPoreLimit->value());
        pset->setValue("PoreVolt", m_pdspPoreVolt->value());
        pset->setValue("VoltStart", m_pdspVoltStart->value());
        pset->setValue("VoltEnd", m_pdspVoltEnd->value());
        pset->setValue("VoltStep", m_pdspVoltStep->value());
        pset->setValue("TimeStep", m_pdspTimeStep->value());
        pset->endGroup();

        pset->beginGroup("SimulationCtrl");
        pset->setValue("VoltStart", m_pdspVoltStart3->value());
        pset->setValue("VoltEnd", m_pdspVoltEnd3->value());
        pset->setValue("VoltStep", m_pdspVoltStep3->value());
        pset->setValue("TimeStep", m_pdspTimeStep3->value());
        pset->setValue("Cycle", m_pispSimulationCycle->value());
        pset->setValue("RampPattern", m_pcmbRampPattern->currentIndex());
        pset->setValue("VoltOffset", m_pdspVoltOffset->value());
        pset->endGroup();

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            pset->beginGroup(QString("ch%1").arg(i));

            pset->setValue("Visible", m_vetControlListItem[i]->GetChecked());
            pset->setValue("Color", m_vetControlListItem[i]->GetColor().name());
            pset->setValue("Seqchan", m_vetControlListItem[i]->GetCheckedSeq());

            pset->endGroup();
        }

        return true;
    }
    return false;
}

bool ControlView::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&ControlView::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void ControlView::ThreadFunc(void)
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
            //int datasz = dtpk->dataHandle.size();
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                auto& curval = dtpk->dataHandle[i];
                int cursz = curval.size();
                if (cursz <= 0)
                {
                    continue;
                }
                m_arCurrentInstant[i] = curval[cursz - 1];

                {
                    std::lock_guard<std::mutex> lock(m_mutexavg);
                    auto& data3 = m_vetAverageData[i];
                    double sum = std::accumulate(curval.begin(), curval.end(), 0.0);
                    auto avg = sum / cursz;
                    data3.emplace_back(avg);
                    m_arCurrentAverage[i] = avg;
                }

                if (ConfigServer::GetInstance()->GetAutoDegating())
                {
#if 1
                    if (!m_bAutoseqGating)
                    {
                        std::lock_guard<std::mutex> lock(m_mutexdegating);
                        //if (!m_vetControlListItem[i]->GetIsDegating())
                        {
                            auto& data1 = m_vetDegatingData[i];
                            //for (const auto& ite : curval)
                            //{
                            //    if (ite > ZERODOUBLE)
                            //    {
                            //        data1.insert(data1.end(), ite);
                            //    }
                            //}
                            data1.insert(data1.end(), curval.begin(), curval.end());
                        }
                    }
                    else
#endif
                    {
                        std::lock_guard<std::mutex> lock(m_mutexdegating);
                        //if (!m_vetControlListItem[i]->GetIsDegating())
                        if (allready)
                        {
                            auto& data1 = m_vetDegatingData[i];
                            //for (const auto& ite : curval)
                            //{
                            //    if (ite > ZERODOUBLE)
                            //    {
                            //        data1.insert(data1.end(), ite);
                            //    }
                            //}
                            data1.insert(data1.end(), curval.begin(), curval.end());
                        }
                    }
                }

                //if (!m_bStdCalculating)
                {
                    std::lock_guard<std::mutex> lock(m_mutexstd);
                    auto& data2 = m_vetStdData[i];
                    data2.insert(data2.end(), curval.begin(), curval.end());
                    if (data2.size() >= 1200)
                    {
                        data2.erase(data2.begin(), data2.begin() + 200);
                    }
                }
            }
            if (ConfigServer::GetInstance()->GetAutoDegating() && m_bAutoseqGating)
            {
                if (m_vetDegatingData[0].size() >= m_iDegatingDataPoint)
                {
                    for (int k = 0; k < m_ciThread; ++k)
                    {
                        m_bDegatingSample[k] = false;
                    }
                    //std::lock_guard<std::mutex> lock(m_mutexdegating);
                    if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
                    {
                        m_vetAvgCur = m_vetDegatingData;
                        m_cvGating.notify_all();
                    }
                    for (int i = 0; i < CHANNEL_NUM; ++i)
                    {
                        m_vetDegatingData[i].clear();
                    }
                }
            }
            if (ConfigServer::GetInstance()->GetCalibration())
            {
                emit setCalibrationSignal(m_arCurrentAverage);
            }
        }
    }
    LOGI("ThreadExit={}!Left={}!={}", __FUNCTION__, m_queDataHandle.size_approx(), ConfigServer::GetCurrentThreadSelf());

    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));
}

bool ControlView::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return false;
}

bool ControlView::StopThread(void)
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

void ControlView::StartSetTimer(int elapse)
{
    m_tmrCur.start(elapse);
    m_tmrAvg.start(1000);
    m_tmrStd.start(5000);
}

void ControlView::StopKillTimer(void)
{
    if (m_tmrCur.isActive())
    {
        m_tmrCur.stop();
    }
    if (m_tmrAvg.isActive())
    {
        m_tmrAvg.stop();
    }
    if (m_tmrStd.isActive())
    {
        m_tmrStd.stop();
    }
}

bool ControlView::SetTimehms(void)
{
    int itm = m_pispIntervalsTimer->value();
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;
        //GetTimehms(itm, ihour, imin, isec);
        m_tmAdjust.setHMS(ihour, imin, isec, 0);

        return true;
    }
    return false;
}

bool ControlView::SetTimehmsMin(void)
{
    int itm = m_pispRotationTime->value();
    if (itm > 0)
    {
        //int iday = itm / 1440;
        int iiday = itm % 1440;
        int ihour = iiday / 60;
        int imin = iiday % 60;

        m_tmRotation.setHMS(ihour, imin, 0, 0);

        return true;
    }

    return false;
}

bool ControlView::StartThread2(void)
{
    if (!m_bRunning2)
    {
        m_bRunning2 = true;
        const int block = CHANNEL_NUM / m_ciThread;
        for (int i = 0; i < m_ciThread; ++i)
        {
            m_pthrGating[i] = std::make_shared<std::thread>(&ControlView::ThreadFunc2Deg, this, i, block);
        }
        return m_pthrGating[0] != nullptr;
    }
    return false;
}

void ControlView::ThreadFunc2Deg(int index, int block)
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

        for (int i = startindex, n = block + startindex; i < n; ++i)
        {
            //int chan = i;// i + startindex;
            if (m_vetControlListItem[i]->isVisible()
                && (m_vetControlListItem[i]->GetDegatingSate() != ControlListItem::EGatingState::EGS_INACTIVE)
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
                //if (isgating == 1)
                //{
                //    ++m_arGatingStatus[i];
                //    int gatingcnt = m_arGatingStatus[i];
                //    if (gatingcnt >= 3)
                //    {
                //        m_vetControlListItem[i]->SetPollingDegatingState(ControlListItem::EGatingState::EGS_GATING);
                //    }
                //    else
                //    {
                //        m_vetControlListItem[i]->SetDegatingState(ControlListItem::EGatingState::EGS_GATING);
                //    }
                //}
                //else
                //{
                //    m_vetControlListItem[i]->SetDegatingState(ControlListItem::EGatingState::EGS_NORMAL);
                //    m_arGatingStatus[i] = 0;
                //}
                m_vetControlListItem[i]->SetDegatingState(isgating ? ControlListItem::EGatingState::EGS_GATING : ControlListItem::EGatingState::EGS_NORMAL);
                //m_vetControlListItem[i]->SetPollingDegatingState(isgating ? ControlListItem::EGatingState::EGS_GATING : ControlListItem::EGatingState::EGS_NORMAL);
            }
        }
        //m_bDegatingSample[index] = true;
    }

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool ControlView::EndThread2(void)
{
    m_bRunning2 = false;
    m_cvGating.notify_all();
    return false;
}

bool ControlView::StopThread2(void)
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

bool ControlView::InitGatingRedis(void)
{
    bool bret = false;
    int iport = 6379;
    int idbnum = 0;
    std::string strhost = ConfigServer::GetInstance()->GetTaskRedisParam(iport, idbnum);
    //if (strhost.size() <= 0)
    {
        strhost = "127.0.0.1";//"172.16.16.44";
        iport = 9099;//9096;
        idbnum = 2;//6;

        ConfigServer::GetInstance()->SetTaskRedisParam(strhost, iport, idbnum);
    }

    bret = m_redisAccess.SetAccessParam(strhost.c_str(), iport, nullptr);
    if (!bret)
    {
        LOGE("Redis set param failed!!!");
        return false;
    }
    bret = m_redisAccess.Connecting();
    if (!bret)
    {
        LOGE("Connect redis failed!!!");
        return false;
    }
    bret = m_redisAccess.selectDB(idbnum);
    if (!bret)
    {
        LOGE("Select redis db failed!!!");
        return false;
    }

    return true;
}

bool ControlView::StartThread3(void)
{
    if (!m_bRunning3 && m_ThreadPtr3 == nullptr)
    {
        //if (!m_bRedisReady)
        //{
        //    m_bRedisReady = InitGatingRedis();
        //    if (!m_bRedisReady)
        //    {
        //        LOGE("StartThread3 FAILED!!!");
        //        return false;
        //    }
        //}
        m_bRunning3 = true;
        const int block = CHANNEL_NUM / m_ciThread;
        for (int i = 0; i < m_ciThread; ++i)
        {
            m_pthrGating[i] = std::make_shared<std::thread>(&ControlView::ThreadFunc3Deg, this, i, block);
        }
        return m_pthrGating[0] != nullptr;
    }
    return false;
}

void ControlView::ThreadFunc3Deg(int index, int block)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    auto strUrl = std::string("localhost:9112/gating_clf");//ConfigServer::GetInstance()->GetVerupdateAddr();
    if (ConfigServer::GetInstance()->GetWorknet() == 1)
    {
        strUrl = std::string("http://172.16.18.12:9112/gating_clf");
    }
    const int startindex = index * block;
    while (m_bRunning3)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexGating);
            m_cvGating.wait(lck);
        }
        if (!m_bRunning3)
        {
            break;
        }

        const auto avgCurrent = m_vetAvgCur;
        if (avgCurrent.size() <= 0)
        {
            continue;
        }

        for (int i = startindex, n = block + startindex; i < n; ++i)
        {
            //int chan = i;// i + startindex;
            if (m_vetControlListItem[i]->isVisible()
                && (m_vetControlListItem[i]->GetDegatingSate() != ControlListItem::EGatingState::EGS_INACTIVE)
                && avgCurrent.size() > 0 && avgCurrent[i].size() > 0)
            {
                auto m_startCnt = std::chrono::steady_clock::now();
                auto openpore = m_redisAccess.Get((m_strDataName + "_last_openpore_" + std::to_string(i + 1)).c_str());
                auto opepo = ::atof(openpore.c_str());

                QJsonObject jsonObject;
                jsonObject.insert("data_name", m_strDataName.c_str());//
                jsonObject.insert("openpore", opepo);//123.4
                jsonObject.insert("chn_num", i + 1);//

                QJsonArray jsonArrayChan;
                for (int j = 0, n = avgCurrent[i].size(); j < n; ++j)
                {
                    jsonArrayChan.append(avgCurrent[i][j]);//
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
                    return;
                }

                size_t retsz = 0;
                auto retstr = httpcl.GetReadString(retsz);
                int isgating = ::atoi(retstr.c_str());
                if (isgating == 1)
                {
                    ++m_arGatingStatus[i];
                    int gatingcnt = m_arGatingStatus[i];
                    if (gatingcnt >= 3)
                    {
                        m_vetControlListItem[i]->SetPollingDegatingState(ControlListItem::EGatingState::EGS_GATING);
                    }
                    else
                    {
                        m_vetControlListItem[i]->SetDegatingState(ControlListItem::EGatingState::EGS_GATING);
                    }
                }
                else
                {
                    m_vetControlListItem[i]->SetDegatingState(ControlListItem::EGatingState::EGS_NORMAL);
                    m_arGatingStatus[i] = 0;
                }

                //auto attm = ConfigServer::GetInstance()->GetRawDataTime();
                auto m_stopCnt = std::chrono::steady_clock::now();
                auto distTime = std::chrono::duration<double>(m_stopCnt - m_startCnt).count();
                LOGI("SeqDegating={}, chk={}, openpore={}, chan={}, gating={}", distTime, (m_vetControlListItem[i]->GetCheckedSeq() ? 1 : 0), opepo, i, isgating);
            }
        }
        if (m_bRunning3)
        {
            std::thread thrd([=]()
                {
                    ConfigServer::MeSleep(1000);
                    m_bDegatingSample[index] = true;
                });
            thrd.detach();
        }

        //m_bDegatingSample[index] = true;
    }

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool ControlView::EndThread3(void)
{
    m_bRunning3 = false;
    m_cvGating.notify_all();
    return false;
}

bool ControlView::StopThread3(void)
{
    m_bRunning3 = false;
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

void ControlView::SetAllChannelDCLevel(double volt)
{
    int arout = 0;
    size_t len = UsbProtoMsg::SetAllDCLevel(volt, arout);

    emit transmitUsbData(arout, len);
}

void ControlView::SetSepChannelDCLevel(int chan, double volt)
{
    int arout = 0;
    auto len = UsbProtoMsg::SetOneDCLevel(chan, volt, arout);

    emit transmitUsbData(arout, len);
}

void ControlView::SetPolymerVoltage(double volt)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (!m_vetControlListItem[i]->GetProtectPore())
        {
            SetSepChannelDCLevel(i, volt);
            m_vetControlListItem[i]->SetVoltAmpl(volt);
        }
    }
}

void ControlView::SetSimulationVoltage(double volt)
{
    SetAllChannelDCLevel(volt);

    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetControlListItem[i]->SetVoltAmpl(volt);
    }
}

void ControlView::SetOneChannelDCLevel(int chan, double volt)
{
    SetSepChannelDCLevel(chan, volt);

    m_vetControlListItem[chan]->SetVoltAmpl(volt);
}

void ControlView::SetOneChannelPulseLevel(int chan, double volt)
{
    int arout = 0;
    auto len = UsbProtoMsg::SetOnePulseLevel(chan, volt, arout);
    emit transmitUsbData(arout, len);

    m_vetControlListItem[chan]->SetVoltAmpl(volt);
}

//bool ControlView::SetZeroAdjustInterval(int val)
//{
//    if (m_pispIntervalsTimer->value() != val)
//    {
//        m_pispIntervalsTimer->setValue(val);
//        m_pbtnZeroAdjustReset->click();
//        return true;
//    }
//    return false;
//}
//
//bool ControlView::SetEnableZeroAdjust(bool val)
//{
//    if (val)
//    {
//        if (!m_pbtnZeroAdjustStart->isChecked())
//        {
//            m_pbtnZeroAdjustStart->click();
//            return true;
//        }
//    }
//    else
//    {
//        if (m_pbtnZeroAdjustStart->isChecked())
//        {
//            m_pbtnZeroAdjustStart->click();
//            return true;
//        }
//    }
//    return false;
//}

void ControlView::DoLoadConfig(const QString& qsconf)
{
    if (!qsconf.isEmpty())
    {
        if (!QFile::exists(qsconf))
        {
            return;
        }
        QSettings saveconf(qsconf, QSettings::IniFormat, this);
        saveconf.status();

        saveconf.beginGroup("ChipQC");
        m_pdspVolt1->setValue(saveconf.value("Volt1").toDouble());
        m_pdspDuration1->setValue(saveconf.value("Volt1Dur").toDouble());
        m_pdspCurrentMin1->setValue(saveconf.value("FilterCur1").toDouble());
        //m_pdspStdMin1->setValue(saveconf.value("FilterStd1").toDouble());
        //m_pdspStdMax1->setValue(saveconf.value("FilterStd2").toDouble());
        saveconf.endGroup();

        saveconf.beginGroup("MembraneFormation");
        m_pispInterval2->setValue(saveconf.value("Intervals").toInt());
        m_pdspTimerDuration2->setValue(saveconf.value("TimerDur").toDouble());
        m_pdspVolt2->setValue(saveconf.value("Volt1").toDouble());
        m_pdspDuration2->setValue(saveconf.value("Volt1Dur").toDouble());
        m_pdspCurrentMin2->setValue(saveconf.value("FilterCur1").toDouble());
        m_pdspCurrentMax2->setValue(saveconf.value("FilterCur2").toDouble());
        m_pdspStdMin2->setValue(saveconf.value("FilterStd1").toDouble());
        m_pdspStdMax2->setValue(saveconf.value("FilterStd2").toDouble());
        m_pdspCapMin2->setValue(saveconf.value("FilterCap1").toDouble());
        m_pdspCapMax2->setValue(saveconf.value("FilterCap2").toDouble());
        saveconf.endGroup();

        saveconf.beginGroup("PoreInsertion");
        m_pchkWaitingZeroAdjust3->setChecked(saveconf.value("WFZeroAdjust").toBool());
        m_pdspVolt3->setValue(saveconf.value("Volt1").toDouble());
        m_pdspDuration3->setValue(saveconf.value("Volt1Dur").toDouble());
        m_pdspSingleLimit3->setValue(saveconf.value("SingleLimit").toDouble());
        m_pdspProtectVolt3->setValue(saveconf.value("PortectVolt").toDouble());
        m_pdspMultiLimit3->setValue(saveconf.value("MultiLimit").toDouble());
        m_pispPoreStatus3->setValue(saveconf.value("PoreStatusCnt").toInt());
        m_pdspSaturatedLimit3->setValue(saveconf.value("SaturatedLimit").toDouble());
        m_pdspTimerDuration3->setValue(saveconf.value("TimerDur").toDouble());
        saveconf.endGroup();

        saveconf.beginGroup("FactoryPoreQC");
        m_pchkWaitingZeroAdjust4->setChecked(saveconf.value("WFZeroAdjust").toBool());
        m_pdspVolt4->setValue(saveconf.value("Volt1").toDouble());
        m_pdspDuration4->setValue(saveconf.value("Volt1Dur").toDouble());
        m_pdspVolt42->setValue(saveconf.value("Volt2").toDouble());
        m_pdspDuration42->setValue(saveconf.value("Volt2Dur").toDouble());
        m_pdspVolt43->setValue(saveconf.value("Volt3").toDouble());
        m_pdspDuration43->setValue(saveconf.value("Volt3Dur").toDouble());
        m_pdspCurrentMin4->setValue(saveconf.value("FilterCur1").toDouble());
        m_pdspCurrentMax4->setValue(saveconf.value("FilterCur2").toDouble());
        m_pdspStdMin4->setValue(saveconf.value("FilterStd1").toDouble());
        m_pdspStdMax4->setValue(saveconf.value("FilterStd2").toDouble());
        saveconf.endGroup();

        //saveconf.beginGroup("Sequencing");
        //m_pdspVolt5->setValue(saveconf.value("Volt1").toDouble());
        //m_pdspDuration5->setValue(saveconf.value("Volt1Dur").toDouble());
        ////m_pdspTimerDuration5->setValue(saveconf.value("TimerDur").toDouble());
        ////m_pdspCurrentMin5->setValue(saveconf.value("TimerFilterCur1").toDouble());
        //saveconf.endGroup();

        saveconf.beginGroup("MuxScan");
        m_pdspVolt6->setValue(saveconf.value("PoreQCVolt").toDouble());
        m_pdspDuration6->setValue(saveconf.value("PoreQCVoltDur").toDouble());
        m_pdspCurrentMin6->setValue(saveconf.value("FilterCur1").toDouble());
        m_pdspCurrentMax6->setValue(saveconf.value("FilterCur2").toDouble());
        m_pdspStdMin6->setValue(saveconf.value("FilterStd1").toDouble());
        m_pdspStdMax6->setValue(saveconf.value("FilterStd2").toDouble());
        //m_pdspVolt62->setValue(saveconf.value("ZeroVolt").toDouble());
        m_pdspDuration62->setValue(saveconf.value("ZeroVoltDur").toDouble());
        m_pdspVolt63->setValue(saveconf.value("SeqVolt").toDouble());
        m_pdspDuration63->setValue(saveconf.value("SeqVoltDur").toDouble());
        //m_pdspTimerDuration6->setValue(saveconf.value("TimerDur").toDouble());
        //m_pdspCurrentMin62->setValue(saveconf.value("TimerFilterCur1").toDouble());
        m_pispCycle6->setValue(saveconf.value("Cycle").toInt());
        m_pdspTimerPeriod6->setValue(saveconf.value("TimerPeriod").toDouble());
        saveconf.endGroup();

        SetTimehms6();

        saveconf.beginGroup("Degating");
        m_dDegatingVolt = saveconf.value("DegatingVolt").toDouble();
        m_iCyclePeriod = saveconf.value("CyclePeriod").toInt();
        m_iDurationTime = saveconf.value("DurationTime").toInt();
        m_iSteppingTime = saveconf.value("SteppingTime").toInt();
        m_iAllowTime = saveconf.value("AllowTime").toInt();
        m_iLimitCycle = saveconf.value("LimitCycle").toInt();
        m_dProtectCurrent = saveconf.value("ProtectCurrent").toDouble();

        m_bCheckAutoSeqGating = (saveconf.value("CheckAutoSeqGating").toInt() == 1);

        //advance
        m_dDurationThres = saveconf.value("DurationThres").toDouble();
        m_iGatingSTD = saveconf.value("GatingSTD").toInt();
        m_iGatingSTDMax = saveconf.value("GatingSTDMax").toInt();
        m_iSignalMin = saveconf.value("SignalMin").toInt();
        m_iSignalMax = saveconf.value("SignalMax").toInt();
        m_iGatingMin = saveconf.value("GatingMin").toInt();
        m_iGatingMax = saveconf.value("GatingMax").toInt();
        saveconf.endGroup();

        saveconf.beginGroup("ZeroAdjust");
        m_dBackwardVolt = saveconf.value("BackwardVolt").toDouble();
        m_dBackwardTime = saveconf.value("BackwardTime").toDouble();
        m_dZeroTime = saveconf.value("ZeroTime").toDouble();
        m_dAdjustTime = saveconf.value("AdjustTime").toDouble();
        m_iIntervalsTimer = saveconf.value("IntervalsTimer").toInt();
        saveconf.endGroup();
    }
}

void ControlView::DoSaveConfig(const QString& qsconf)
{
    if (!qsconf.isEmpty())
    {
        QString fileName = qsconf;
#ifdef Q_OS_WIN32
#else
        auto suf = fileName.right(4);
        if (suf != ".cof")
        {
            fileName += ".cof";
        }
#endif
        QSettings saveconf(fileName, QSettings::IniFormat, this);

        saveconf.beginGroup("ChipQC");
        saveconf.setValue("Volt1", m_pdspVolt1->value());
        saveconf.setValue("Volt1Dur", m_pdspDuration1->value());
        saveconf.setValue("FilterCur1", m_pdspCurrentMin1->value());
        //saveconf.setValue("FilterStd1", m_pdspStdMin1->value());
        //saveconf.setValue("FilterStd2", m_pdspStdMax1->value());
        saveconf.endGroup();

        saveconf.beginGroup("MembraneFormation");
        saveconf.setValue("Intervals", m_pispInterval2->value());
        saveconf.setValue("TimerDur", m_pdspTimerDuration2->value());
        saveconf.setValue("Volt1", m_pdspVolt2->value());
        saveconf.setValue("Volt1Dur", m_pdspDuration2->value());
        saveconf.setValue("FilterCur1", m_pdspCurrentMin2->value());
        saveconf.setValue("FilterCur2", m_pdspCurrentMax2->value());
        saveconf.setValue("FilterStd1", m_pdspStdMin2->value());
        saveconf.setValue("FilterStd2", m_pdspStdMax2->value());
        saveconf.setValue("FilterCap1", m_pdspCapMin2->value());
        saveconf.setValue("FilterCap2", m_pdspCapMax2->value());
        saveconf.endGroup();

        saveconf.beginGroup("PoreInsertion");
        saveconf.setValue("WFZeroAdjust", m_pchkWaitingZeroAdjust3->isChecked());
        saveconf.setValue("Volt1", m_pdspVolt3->value());
        saveconf.setValue("Volt1Dur", m_pdspDuration3->value());
        saveconf.setValue("SingleLimit", m_pdspSingleLimit3->value());
        saveconf.setValue("PortectVolt", m_pdspProtectVolt3->value());
        saveconf.setValue("MultiLimit", m_pdspMultiLimit3->value());
        saveconf.setValue("PoreStatusCnt", m_pispPoreStatus3->value());
        saveconf.setValue("SaturatedLimit", m_pdspSaturatedLimit3->value());
        saveconf.setValue("TimerDur", m_pdspTimerDuration3->value());
        saveconf.endGroup();

        saveconf.beginGroup("FactoryPoreQC");
        saveconf.setValue("WFZeroAdjust", m_pchkWaitingZeroAdjust4->isChecked());
        saveconf.setValue("Volt1", m_pdspVolt4->value());
        saveconf.setValue("Volt1Dur", m_pdspDuration4->value());
        saveconf.setValue("Volt2", m_pdspVolt42->value());
        saveconf.setValue("Volt2Dur", m_pdspDuration42->value());
        saveconf.setValue("Volt3", m_pdspVolt43->value());
        saveconf.setValue("Volt3Dur", m_pdspDuration43->value());
        saveconf.setValue("FilterCur1", m_pdspCurrentMin4->value());
        saveconf.setValue("FilterCur2", m_pdspCurrentMax4->value());
        saveconf.setValue("FilterStd1", m_pdspStdMin4->value());
        saveconf.setValue("FilterStd2", m_pdspStdMax4->value());
        saveconf.endGroup();

        //saveconf.beginGroup("Sequencing");
        //saveconf.setValue("Volt1", m_pdspVolt5->value());
        //saveconf.setValue("Volt1Dur", m_pdspDuration5->value());
        ////saveconf.setValue("TimerDur", m_pdspTimerDuration5->value());
        ////saveconf.setValue("TimerFilterCur1", m_pdspCurrentMin5->value());
        //saveconf.endGroup();

        saveconf.beginGroup("MuxScan");
        saveconf.setValue("PoreQCVolt", m_pdspVolt6->value());
        saveconf.setValue("PoreQCVoltDur", m_pdspDuration6->value());
        saveconf.setValue("FilterCur1", m_pdspCurrentMin6->value());
        saveconf.setValue("FilterCur2", m_pdspCurrentMax6->value());
        saveconf.setValue("FilterStd1", m_pdspStdMin6->value());
        saveconf.setValue("FilterStd2", m_pdspStdMax6->value());
        //saveconf.setValue("ZeroVolt", m_pdspVolt62->value());
        saveconf.setValue("ZeroVoltDur", m_pdspDuration62->value());
        saveconf.setValue("SeqVolt", m_pdspVolt63->value());
        saveconf.setValue("SeqVoltDur", m_pdspDuration63->value());
        //saveconf.setValue("TimerDur", m_pdspTimerDuration6->value());
        //saveconf.setValue("TimerFilterCur1", m_pdspCurrentMin62->value());
        saveconf.setValue("Cycle", m_pispCycle6->value());
        saveconf.setValue("TimerPeriod", m_pdspTimerPeriod6->value());
        saveconf.endGroup();

        saveconf.beginGroup("Degating");
        saveconf.setValue("DegatingVolt", m_dDegatingVolt);
        saveconf.setValue("CyclePeriod", m_iCyclePeriod);
        saveconf.setValue("DurationTime", m_iDurationTime);
        saveconf.setValue("SteppingTime", m_iSteppingTime);
        saveconf.setValue("AllowTime", m_iAllowTime);
        saveconf.setValue("LimitCycle", m_iLimitCycle);
        saveconf.setValue("ProtectCurrent", m_dProtectCurrent);

        saveconf.setValue("CheckAutoSeqGating", m_bCheckAutoSeqGating ? 1 : 0);

        //advance
        saveconf.setValue("DurationThres", m_dDurationThres);
        saveconf.setValue("GatingSTD", m_iGatingSTD);
        saveconf.setValue("GatingSTDMax", m_iGatingSTDMax);
        saveconf.setValue("SignalMin", m_iSignalMin);
        saveconf.setValue("SignalMax", m_iSignalMax);
        saveconf.setValue("GatingMin", m_iGatingMin);
        saveconf.setValue("GatingMax", m_iGatingMax);
        saveconf.endGroup();

        saveconf.beginGroup("ZeroAdjust");
        saveconf.setValue("BackwardVolt", m_dBackwardVolt);
        saveconf.setValue("BackwardTime", m_dBackwardTime);
        saveconf.setValue("ZeroTime", m_dZeroTime);
        saveconf.setValue("AdjustTime", m_dAdjustTime);
        saveconf.setValue("IntervalsTimer", m_iIntervalsTimer);
        saveconf.endGroup();
    }
}

void ControlView::DoZeroAdjust(double bkwvolt, double bkwtime, double zerotime, double adjusttime)
{
    ConfigServer::GetInstance()->SetZeroAdjustProcess(true);

    int tottime = bkwtime + zerotime + adjusttime;
    m_pProgressDialog->setRange(0, tottime);
    int progress = 0;
    m_pProgressDialog->setValue(progress);

    setZeroAdjustSpeVoltSlot(bkwvolt);
    for (int i = 0; i < bkwtime; ++i)
    {
        if (m_pProgressDialog->wasCanceled())
        {
            setZeroAdjustEndSlot();
            m_pProgressDialog->reset();
            return;
        }
        ConfigServer::caliSleep(1000);
        m_pProgressDialog->setValue(++progress);
    }

    setZeroAdjustAllzeroVoltSlot();
    for (int i = 0; i < zerotime; ++i)
    {
        if (m_pProgressDialog->wasCanceled())
        {
            setZeroAdjustEndSlot();
            m_pProgressDialog->reset();
            return;
        }
        ConfigServer::caliSleep(1000);
        m_pProgressDialog->setValue(++progress);
    }

    setZeroAdjustStartSlot();
    for (int i = 0; i < adjusttime; ++i)
    {
        if (m_pProgressDialog->wasCanceled())
        {
            setZeroAdjustEndSlot();
            m_pProgressDialog->reset();
            return;
        }
        ConfigServer::caliSleep(1000);
        m_pProgressDialog->setValue(++progress);
    }

    setZeroAdjustEndSlot();
}

void ControlView::onClickShowZeroAdjust(void)
{
    ZeroAdjustDialog dlg(this);
    dlg.SetBackwardVolt(m_dBackwardVolt);
    dlg.SetBackwardTime(m_dBackwardTime);
    dlg.SetZeroTime(m_dZeroTime);
    dlg.SetAdjustTime(m_dAdjustTime);
    dlg.SetIntervalsTimer(m_iIntervalsTimer);
    int ret = dlg.exec();
    if (ret == QDialog::Accepted)
    {
        m_dBackwardVolt = dlg.GetBackwardVolt();
        m_dBackwardTime = dlg.GetBackwardTime();
        m_dZeroTime = dlg.GetZeroTime();
        m_dAdjustTime = dlg.GetAdjustTime();
        m_iIntervalsTimer = dlg.GetIntervalsTimer();

        LOGI("Flow ZeroAdjust Param={},{},{},{},{}", m_dBackwardVolt, m_dBackwardTime, m_dZeroTime, m_dAdjustTime, m_iIntervalsTimer);
    }
}

void ControlView::onClickShowDegating(void)
{
    DegatingDialog dlg(this);
    dlg.SetAutoSeqGating(m_bCheckAutoSeqGating);
    dlg.SetDegatingBaseParam(m_dDegatingVolt, m_iCyclePeriod, m_iDurationTime, m_iSteppingTime, m_iAllowTime, m_iLimitCycle, m_dProtectCurrent);
    dlg.SetDegatingAdvanceParam(m_dDurationThres, m_iGatingSTD, m_iGatingSTDMax, m_iSignalMin, m_iSignalMax, m_iGatingMin, m_iGatingMax);
    int ret = dlg.exec();
    if (ret == QDialog::Accepted)
    {
        m_bCheckAutoSeqGating = dlg.GetAutoSeqGating();
        dlg.GetDegatingBaseParam(m_dDegatingVolt, m_iCyclePeriod, m_iDurationTime, m_iSteppingTime, m_iAllowTime, m_iLimitCycle, m_dProtectCurrent);
        dlg.GetDegatingAdvanceParam(m_dDurationThres, m_iGatingSTD, m_iGatingSTDMax, m_iSignalMin, m_iSignalMax, m_iGatingMin, m_iGatingMax);
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->SetDegatingParam(m_dDegatingVolt, m_iCyclePeriod, m_iDurationTime, m_iSteppingTime, m_iAllowTime, m_iLimitCycle);
        }

        LOGI("Flow Degating Param={},{},{},{},{},{},{},{};{},{},{},{},{},{}", m_bCheckAutoSeqGating, m_dDegatingVolt, m_iCyclePeriod, m_iDurationTime, m_iSteppingTime, m_iAllowTime, m_iLimitCycle, m_dProtectCurrent
            , m_dDurationThres, m_iGatingSTD, m_iSignalMin, m_iSignalMax, m_iGatingMin, m_iGatingMax);
    }
}

void ControlView::onWaitingZeroAdjust(void)
{
    //Set ZeroAdjust ???
    DoZeroAdjust(m_dBackwardVolt, m_dBackwardTime, m_dZeroTime, m_dAdjustTime);
    while (ConfigServer::GetInstance()->GetZeroAdjustProcess())//Waiting for ZeroAdjust
    {
        ConfigServer::caliSleep(1000);
    }
}

void ControlView::onStartAutoDegating(bool chk)
{
    if (chk)
    {
        //double seqvolt = 0.18;
        //emit getSequencingVoltageSignal(seqvolt);
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->SetDegatingParam(m_dDegatingVolt, m_iCyclePeriod, m_iDurationTime, m_iSteppingTime, m_iAllowTime, m_iLimitCycle);

            m_vetControlListItem[i]->ResetMuxScanDegatingState();
            m_vetControlListItem[i]->SetDegatingTimer();
            //m_vetControlListItem[i]->SetDegatingCount("0");

            m_vetControlListItem[i]->SetPorePortect(true);

            m_arGatingStatus[i] = 0;
        }

        if (m_bCheckAutoSeqGating)
        {
            if (!m_bRedisReady)
            {
                m_bRedisReady = InitGatingRedis();
            }
            if (!m_bRedisReady)
            {
                LOGE("Init Redis FAILED!!!");
            }
            else
            {
                StartThread3();
                ConfigServer::GetInstance()->SetAutoDegating(true);
                if (!m_tmrDegating.isActive())
                {
                    m_tmrDegating.start(2000);
                }
            }
        }
        else
        {
            StartThread2();
            ConfigServer::GetInstance()->SetAutoDegating(true);
            if (!m_tmrDegating.isActive())
            {
                m_tmrDegating.start(1000);
            }
        }


        //enable protect
        m_pdspSingleLimit3->setEnabled(false);
        m_pdspMultiLimit3->setEnabled(false);
        m_pdspSaturatedLimit3->setEnabled(false);
        if (!m_tmrAutoProtect.isActive())
        {
            m_tmrAutoProtect.start(2000);
        }

#ifdef Q_OS_WIN32
        std::string strdegating = ConfigServer::GetCurrentPath() + "/etc/Degating/";
#else
        std::string strdegating = "/data/Degating/";
#endif
        std::filesystem::path pathdegating(strdegating);
        if (!std::filesystem::exists(pathdegating))
        {
            std::filesystem::create_directories(pathdegating);
        }
        std::string datetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toStdString();
        m_pFile = new QFile((strdegating + datetime + ".csv").c_str());
        if (!m_pFile->open(QIODevice::WriteOnly | QIODevice::Text))
        {
            LOGE("Save status file FAILED!!!");
            return;
        }
        else
        {
            m_pTextStream = new QTextStream(m_pFile);
            QString strhead("Time,");
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                strhead += QString("CH%1,").arg(i + 1);
            }
            strhead += "\n";
            *m_pTextStream << strhead;
        }
        m_tmrRecordDegating.start(2000);
    }
    else
    {
        if (m_bCheckAutoSeqGating)
        {
            EndThread3();
        }
        else
        {
            EndThread2();
        }
        ConfigServer::GetInstance()->SetAutoDegating(false);
        if (m_tmrDegating.isActive())
        {
            m_tmrDegating.stop();
        }

        //disable protect
        if (m_tmrAutoProtect.isActive())
        {
            m_tmrAutoProtect.stop();
        }
        m_pdspSingleLimit3->setEnabled(true);
        m_pdspMultiLimit3->setEnabled(true);
        m_pdspSaturatedLimit3->setEnabled(true);

        if (m_tmrRecordDegating.isActive())
        {
            m_tmrRecordDegating.stop();
        }
        if (m_pTextStream != nullptr)
        {
            delete m_pTextStream;
            m_pTextStream = nullptr;
        }
        if (m_pFile != nullptr)
        {
            m_pFile->close();
            delete m_pFile;
            m_pFile = nullptr;
        }
        if (m_bCheckAutoSeqGating)
        {
            StopThread3();
        }
        else
        {
            StopThread2();
        }

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetDegatingData[i].clear();

            m_vetControlListItem[i]->SetPorePortect(false);

            m_vetControlListItem[i]->ResetDegatingTimer();

            m_vetControlListItem[i]->ResetHoleStatus();
        }
    }
}

void ControlView::onClickLoadConfig(void)
{
    //QMessageBox::information(this, "Load", "Please Looking forward....");
    //return;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Config File"), QString::fromStdString(ConfigServer::GetCurrentPath()),
        "Automation Flow Config (*.cof)", nullptr, QFileDialog::HideNameFilterDetails);
    DoLoadConfig(fileName);
}

void ControlView::onClickSaveConfig(void)
{
    //QMessageBox::information(this, "Save", "Please Looking forward....");
    //return;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Config File"), QString::fromStdString(ConfigServer::GetCurrentPath()),
        "Automation Flow Config (*.cof)", nullptr, QFileDialog::HideNameFilterDetails);//
    DoSaveConfig(fileName);
}

void ControlView::setVoltchkshowEnable(bool chk)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetControlListItem[i]->SetCheck2Enable(chk);
        m_vetControlListItem[i]->SetVoltEnable(chk);
    }
}

void ControlView::onClickChannelReset(void)
{
    if (m_pbtnChannelQC->isChecked())
    {
        m_pbtnChannelQC->click();
    }
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetControlListItem[i]->SetChecked(true);
        m_vetControlListItem[i]->SetCheckedSeq(true);
        m_plistControlShow->item(i)->setHidden(false);
    }
}

void ControlView::onClickChannelQC(bool chk)
{
    if (chk)
    {
        LOGI("ChipQC Start....");
        m_pbtnChannelQC->setEnabled(false);
        m_pbtnChannelQC->setText("Confirm");
        m_pbtnChannelQC->setIcon(QIcon(":/img/img/reset_press.png"));

        SetAllChannelDCLevelSlot(m_pdspVolt1->value());
        ConfigServer::caliSleep(m_pdspDuration1->value() * 1000);

        SetDCLevelAllSlot(m_pdspCurrentMin1->value(), 999999.99);

        //QMessageBox::information(this, "Confirm Valid Channel", "Channel QC Finished! Please Confirm The Valid Channels!");

        m_pbtnChannelQC->setEnabled(true);
    }
    else
    {
        LOGI("ChipQC Finished....");
        //save the valid channels....
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->SetValidChannel(m_vetControlListItem[i]->GetCheckedSeq());
        }
        SetAllChannelDCLevelSlot(0.0);
        UpdateListItemSlot();

        m_pbtnChannelQC->setText("Start");
        m_pbtnChannelQC->setIcon(QIcon(":/img/img/start.ico"));
    }
}

void ControlView::onClickMakeMembraneStartCapTest(bool chk)
{
    if (chk)
    {
        LOGI("CapTest Start....");
        m_pbtnCapTestTimer->setEnabled(false);
        m_pbtnMakeMembrane->setEnabled(false);
        m_pbtnMakeMembraneStartCap->setEnabled(false);
        m_pbtnCapTestTimer->setText("Stop  Timer");
        m_pbtnCapTestTimer->setIcon(QIcon(":/img/img/reset_press.png"));
        m_pispInterval2->setEnabled(false);
        m_pdspTimerDuration2->setEnabled(false);

        SetTimehms2();

        emit setCapCalculationSignal(1, m_pispInterval2->value());
        if (!m_tmrMembraneForm.isActive())
        {
            m_tmrMembraneForm.start(1000);
        }
        m_pbtnCapTestTimer->setEnabled(true);
    }
    else
    {
        LOGI("CapTest Stop....");
        if (m_tmrMembraneForm.isActive())
        {
            m_tmrMembraneForm.stop();
        }
        emit setCapCalculationSignal(0, m_pispInterval2->value());

        m_pispInterval2->setEnabled(true);
        m_pdspTimerDuration2->setEnabled(true);
        m_pbtnCapTestTimer->setText("Start Timer");
        m_pbtnCapTestTimer->setIcon(QIcon(":/img/img/start.ico"));
        m_pbtnMakeMembrane->setEnabled(true);
        m_pbtnMakeMembraneStartCap->setEnabled(true);
    }
}

void ControlView::onTimerMakeMembraneStartCapTest(void)
{
    m_tmMembraneForm = m_tmMembraneForm.addSecs(-1);
    m_plcdTimerDuration2->display(m_tmMembraneForm.toString("hh:mm:ss"));
    if (m_tmMembraneForm == QTime(0, 0, 0, 0))
    {
        m_pbtnCapTestTimer->click();
        if (m_pbtnCapTestTimer->isChecked())
        {
            m_pbtnCapTestTimer->click();
        }
    }
}

void ControlView::onClickMakeMembraneStart(bool chk)
{
    if (chk)
    {
        LOGI("MembraneQC Start....");
        m_pbtnCapTestTimer->setEnabled(false);
        m_pbtnMakeMembraneStartCap->setEnabled(false);
        m_pbtnMakeMembrane->setEnabled(false);
        m_pbtnMakeMembrane->setText("Confirm");
        m_pbtnMakeMembrane->setIcon(QIcon(":/img/img/reset_press.png"));

        SetSelChannelDCLevelSlot(m_pdspVolt2->value());
        ConfigServer::caliSleep(m_pdspDuration2->value() * 1000);

        //onWaitingZeroAdjust();//no need za
        SetDCLevelStdSelectedSlot(m_pdspCurrentMin2->value(), m_pdspCurrentMax2->value(), m_pdspStdMin2->value(), m_pdspStdMax2->value());

        //QMessageBox::information(this, "Confirm Valid Channel", "Make Membrane Current Filter Finished! Please Confirm The Valid Channels!");

        m_pbtnMakeMembrane->setEnabled(true);

    }
    else
    {
        LOGI("MembraneQC Stop....");
        //save the valid channels....
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->SetValidChannel(m_vetControlListItem[i]->GetCheckedSeq());
        }
        SetAllChannelDCLevelSlot(0.0);
        UpdateListItemSlot();

        m_pbtnMakeMembrane->setText("Start");
        m_pbtnMakeMembrane->setIcon(QIcon(":/img/img/start.ico"));
        m_pbtnCapTestTimer->setEnabled(true);
        m_pbtnMakeMembraneStartCap->setEnabled(true);
    }
}

void ControlView::onClickMakeMembraneStartCapFilter(bool chk)
{
    if (chk)
    {
        LOGI("CapFilter Start....");
        m_pbtnCapTestTimer->setEnabled(false);
        m_pbtnMakeMembrane->setEnabled(false);
        m_pbtnMakeMembraneStartCap->setEnabled(false);
        m_pbtnMakeMembraneStartCap->setText("Stop Cap");
        m_pbtnMakeMembraneStartCap->setIcon(QIcon(":/img/img/reset_press.png"));
        m_pispInterval2->setEnabled(false);

        //setTriangeVoltageSlot(1);
        emit setCapCalculationSignal(1, m_pispInterval2->value());

        m_pbtnMakeMembraneStartCap->setEnabled(true);
    }
    else
    {
        LOGI("CapFilter Stop....");
        //setTriangeVoltageSlot(0);
        SetDCLevelCapSelectedSlot(m_pdspCapMin2->value(), m_pdspCapMax2->value());

        emit setCapCalculationSignal(0, m_pispInterval2->value());

        SetAllChannelDCLevelSlot(0.0);
        UpdateListItemSlot();

        m_pispInterval2->setEnabled(true);
        m_pbtnMakeMembraneStartCap->setText("Start Cap");
        m_pbtnMakeMembraneStartCap->setIcon(QIcon(":/img/img/start.ico"));
        m_pbtnCapTestTimer->setEnabled(true);
        m_pbtnMakeMembrane->setEnabled(true);
    }
}

void ControlView::onCheckWaitingZeroAdjust3(bool chk)
{
    m_pbtnWaitingZeroAdjust3->setEnabled(chk);
}

void ControlView::onClickPoreInsertTimer(bool chk)
{
    if (chk)
    {
        LOGI("PoreInsertTimer Start....{}", m_pchkWaitingZeroAdjust3->isChecked());
        m_pbtnPoreInsert->setEnabled(false);
        m_pbtnPoreInsertTimer->setEnabled(false);
        m_pbtnPoreInsertTimer->setText("Stop Timer");
        m_pbtnPoreInsertTimer->setIcon(QIcon(":/img/img/reset_press.png"));

        m_pdspProtectVolt3->setEnabled(false);

        SetTimehms3();

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->ResetHoleStatus();
            m_vetControlListItem[i]->ResetDegatingState();
            m_vetControlListItem[i]->SetProtectVolt(m_pdspProtectVolt3->value());
        }

        if (m_pchkWaitingZeroAdjust3->isChecked())
        {
            onWaitingZeroAdjust();
        }

        SetSelChannelDCLevelSlot(m_pdspVolt3->value());
        ConfigServer::caliSleep(m_pdspDuration3->value() * 1000);

        if (!m_tmrPoreInsert.isActive())
        {
            m_tmrPoreInsert.start(1000);
        }

        if (!m_tmrAutoZero.isActive())
        {
            m_tmrAutoZero.start(500);
        }

        m_pbtnPoreInsertTimer->setEnabled(true);
    }
    else
    {
        LOGI("PoreInsertTimer Stop....");
        if (m_tmrPoreInsert.isActive())
        {
            m_tmrPoreInsert.stop();
        }
        if (m_tmrAutoZero.isActive())
        {
            m_tmrAutoZero.stop();
        }
        //SetSinglePoreSelectedSlot();
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            bool sing = (m_vetControlListItem[i]->GetHoleStatus() == EEmbedPoreStatus::EPS_SINGLEPORE);
            m_vetControlListItem[i]->SetChecked(sing);
            m_vetControlListItem[i]->SetCheckedSeq(sing);
            m_vetControlListItem[i]->ResetHoleStatus();
        }
        SetAllChannelDCLevelSlot(0.0);
        UpdateListItemSlot();

        m_pbtnPoreInsertTimer->setText("Start Timer");
        m_pbtnPoreInsertTimer->setIcon(QIcon(":/img/img/start.ico"));
        m_pbtnPoreInsert->setEnabled(true);
        m_pdspProtectVolt3->setEnabled(true);
    }
}

void ControlView::onTimerPoreInsertStart(void)
{
    m_tmPoreInsert = m_tmPoreInsert.addSecs(-1);
    m_plcdTimerDuration3->display(m_tmPoreInsert.toString("hh:mm:ss"));
    if (m_tmPoreInsert == QTime(0, 0, 0, 0))
    {
        m_pbtnPoreInsertTimer->click();
        if (m_pbtnPoreInsertTimer->isChecked())
        {
            m_pbtnPoreInsertTimer->click();
        }
    }
}

void ControlView::onClickPoreInsert(void)
{
    m_pbtnPoreInsertTimer->setEnabled(false);

    //onWaitingZeroAdjust();
    emit ShowPoreSelectSignal(m_pchkWaitingZeroAdjust3->isChecked());
}

void ControlView::onCheckWaitingZeroAdjust4(bool chk)
{
    m_pbtnWaitingZeroAdjust4->setEnabled(chk);
}

void ControlView::onClickInitPoreQC(bool chk)
{
    if (chk)
    {
        LOGI("FactoryPoreQC Start....{}", m_pchkWaitingZeroAdjust4->isChecked());
        m_pbtnPoreQC->setEnabled(false);
        m_pbtnPoreQC->setText("Confirm");
        m_pbtnPoreQC->setIcon(QIcon(":/img/img/reset_press.png"));

        if (m_pchkWaitingZeroAdjust4->isChecked())
        {
            onWaitingZeroAdjust();
        }

        SetSelChannelDCLevelSlot(m_pdspVolt4->value());
        ConfigServer::caliSleep(m_pdspDuration4->value() * 1000);
        SetSelChannelDCLevelSlot(m_pdspVolt42->value());
        ConfigServer::caliSleep(m_pdspDuration42->value() * 1000);
        SetSelChannelDCLevelSlot(m_pdspVolt43->value());
        ConfigServer::caliSleep(m_pdspDuration43->value() * 1000);
        SetDCLevelStdSelectedSlot(m_pdspCurrentMin4->value(), m_pdspCurrentMax4->value(), m_pdspStdMin4->value(), m_pdspStdMax4->value());
        ConfigServer::caliSleep(500);

        //QMessageBox::information(this, "Confirm Valid Channel", "Init Pore QC Finished! Please Confirm The Valid Channels!");

        m_pbtnPoreQC->setEnabled(true);
    }
    else
    {
        LOGI("FactoryPoreQC Finished....");

        //save the valid channels....
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->SetValidChannel(m_vetControlListItem[i]->GetCheckedSeq());
        }

        SetAllChannelDCLevelSlot(0.0);
        UpdateListItemSlot();

        m_pbtnPoreQC->setText("Start");
        m_pbtnPoreQC->setIcon(QIcon(":/img/img/start.ico"));
    }
}

void ControlView::onClickSequencingStart(bool chk)
{
    m_pbtnSequencing->setEnabled(false);
    if (chk)
    {
        LOGI("Sequencing Start....");
        m_pbtnSequencing->setText("Stop");
        m_pbtnSequencingQC->setEnabled(false);
        setVoltchkshowEnable(false);

        if (!ConfigServer::GetInstance()->GetAutoDegating())
        {
            //emit setSequencingVoltageSignal(m_pdspVolt5->value());
            SetSelChannelDCLevelSlot(m_pdspVolt5->value());
            ConfigServer::caliSleep(m_pdspDuration5->value() * 1000);

            onStartAutoDegating(true);
            if (!m_tmrSeqZeroAdjust.isActive())
            {
                m_tmrSeqZeroAdjust.start(m_iIntervalsTimer * 1000);
            }
            //if (!m_tmrSequencingFilter.isActive())
            //{
            //    m_tmrSequencingFilter.start(m_pdspTimerDuration5->value() * 60000);
            //}
        }
    }
    else
    {
        auto ret = QMessageBox::question(this, "Confirm Stop", "Really want to Stop Sequencing?");
        if (ret == QMessageBox::Yes)
        {
            LOGI("Sequencing Stop....");
            if (m_tmrSeqZeroAdjust.isActive())
            {
                m_tmrSeqZeroAdjust.stop();
            }
            //if (m_tmrSequencingFilter.isActive())
            //{
            //    m_tmrSequencingFilter.stop();
            //}
            onStartAutoDegating(false);

            m_pbtnSequencing->setText("Start");
            m_pbtnSequencingQC->setEnabled(true);
            setVoltchkshowEnable(true);
        }
        else
        {
            m_pbtnSequencing->setChecked(true);
        }
    }
    m_pbtnSequencing->setEnabled(true);
}

void ControlView::onTimerSequencingZeroAdjust(void)
{
    onWaitingZeroAdjust();
}

//void ControlView::onTimerSequencingFilter(void)
//{
//    if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())//when ZeroAdjust not Filter
//    {
//        SetDCLevelBiggerCloseSlot(m_pdspCurrentMin5->value());
//    }
//}

void ControlView::DoSequencingLoop(void)
{
    if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
    {
        SetSelChannelDCLevelSlot(m_pdspVolt62->value());
        emit SetDataSaveSignal(true);
        LOGI("Startsavedata...");
        ConfigServer::caliSleep(m_pdspDuration62->value() * 1000);

        SetSelChannelDCLevelSlot(m_pdspVolt63->value());
        ConfigServer::caliSleep(m_pdspDuration63->value() * 1000);
        LOGI("Startsavedata2...");

        emit SetChannelStatusSignal(true);
    }
    else
    {
        emit SetDataSaveSignal(true);

        emit SetChannelStatusSignal(true);
    }

    if (m_pbtnSequencingQC->isChecked())
    {
        onStartAutoDegating(true);
        if (!m_tmrSeqZeroAdjust.isActive())
        {
            m_tmrSeqZeroAdjust.start(m_iIntervalsTimer * 1000);
        }
        //if (!m_tmrMuxScanFilter.isActive())
        //{
        //    m_tmrMuxScanFilter.start(m_pdspTimerDuration6->value() * 60000);
        //}
    }
}

void ControlView::onClickNextStep(void)
{
    if (m_bIsNextContinue)
    {
        return;
    }
    m_bIsNextContinue = true;

    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetControlListItem[i]->SetValidChannel(m_vetControlListItem[i]->GetCheckedSeq());
    }

    DoSequencingLoop();
    if (m_pbtnSequencingQC->isChecked())
    {
        if (!m_tmrMuxScanStart.isActive())
        {
            m_tmrMuxScanStart.start(1000);
        }
    }
}

void ControlView::onClickMuxScanStart(bool chk)
{
    m_pbtnSequencingQC->setEnabled(false);
    if (chk)
    {
        LOGI("MuxScan Start....");
        m_pbtnSequencing->setEnabled(false);

        m_iCycleCount = 0;
        m_pbtnSequencingQC->setText(QString("Stop"));
        m_pbtnSequencingQC->setIcon(QIcon(":/img/img/stop.png"));

        m_tmMuxScanStart.setHMS(0, 0, 1, 0);
        if (!m_tmrMuxScanStart.isActive())
        {
            m_tmrMuxScanStart.start(1000);
        }
        //emit setSequencingVoltageSignal(m_pdspVolt63->value());
        setVoltchkshowEnable(false);
    }
    else
    {
        auto ret = QMessageBox::Yes;
        if (m_iCycleCount <= m_pispCycle6->value())
        {
            ret = QMessageBox::question(this, "Confirm Stop", "Really want to Stop MuxScan?");
        }
        if (ret == QMessageBox::Yes)
        {
            LOGI("MuxScan Stop....");
            m_bIsNextContinue = true;
            if (m_tmrMuxScanStart.isActive())
            {
                m_tmrMuxScanStart.stop();
            }

            emit SetDataSaveSignal(false);
            emit SetChannelStatusSignal(false);
            if (m_tmrSeqZeroAdjust.isActive())
            {
                m_tmrSeqZeroAdjust.stop();
            }
            //if (m_tmrMuxScanFilter.isActive())
            //{
            //    m_tmrMuxScanFilter.stop();
            //}
            onStartAutoDegating(false);
            m_iCycleCount = 0;
            SetShowChannelDCLevelSlot(0.0);

            m_pbtnSequencingQC->setText("Start");
            m_pbtnSequencingQC->setIcon(QIcon(":/img/img/exaile.png"));
            m_pbtnSequencing->setEnabled(true);
            setVoltchkshowEnable(true);
        }
        else
        {
            m_pbtnSequencingQC->setChecked(true);
        }
    }
    m_pbtnSequencingQC->setEnabled(true);
}

void ControlView::onTimerMuxScanStart(void)
{
    m_tmMuxScanStart = m_tmMuxScanStart.addSecs(-1);
    m_plcdTimerPeriod6->display(m_tmMuxScanStart.toString("hh:mm:ss"));
    if (m_tmMuxScanStart == QTime(0, 0, 0, 0))
    {
        ++m_iCycleCount;

        if (m_iCycleCount > m_pispCycle6->value())
        {
            if (m_pbtnSequencingQC->isChecked())
            {
                m_pbtnSequencingQC->click();
            }
            LOGI("Time Period QC Seqpuencing Reach Total Cycle Auto Stop!");
            return;
        }
        m_pbtnSequencingQC->setText(QString("Stop (%1)").arg(m_pispCycle6->value() - m_iCycleCount));

        emit SetDataSaveSignal(false);

        if (m_tmrSeqZeroAdjust.isActive())
        {
            m_tmrSeqZeroAdjust.stop();
        }
        //if (m_tmrMuxScanFilter.isActive())
        //{
        //    m_tmrMuxScanFilter.stop();
        //}
        onStartAutoDegating(false);

        SetTimehms6();

        //qc....
        //if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
        //{
        //	emit SetCancelZeroAdjustSignal();
        //	//ConfigServer::caliSleep(2000);
        //}
        if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
        {
            SetShowChannelDCLevelSlot(0.0);
            ConfigServer::caliSleep(500);

            onWaitingZeroAdjust();

            SetShowChannelDCLevelSlot(m_pdspVolt6->value());
            ConfigServer::caliSleep(m_pdspDuration6->value() * 1000);

            SetDCLevelStdShowSelectedSlot(m_pdspCurrentMin6->value(), m_pdspCurrentMax6->value(), m_pdspStdMin6->value(), m_pdspStdMax6->value());
        }

        if (m_pbtnSequencingQC->isChecked())
        {
            MyQMessageBox mymsgbox(this);
            int iret = mymsgbox.exec();
            m_bIsNextContinue = (iret == QMessageBox::RejectRole);

            if (!m_bIsNextContinue)
            {
                if (m_tmrMuxScanStart.isActive())
                {
                    m_tmrMuxScanStart.stop();
                }
                return;
            }
        }
        else
        {
            return;
        }

        //sequencing....
        DoSequencingLoop();
    }
}

//void ControlView::onTimerMuxScanFilter(void)
//{
//    if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())//when ZeroAdjust not Filter
//    {
//        SetDCLevelBiggerCloseSlot(m_pdspCurrentMin62->value());
//    }
//}

void ControlView::onTimerRecordDegating(void)
{
    if (m_pFile != nullptr && m_pTextStream != nullptr)
    {
        auto hms = QDateTime::currentDateTime().toString("hh:mm:ss");
        *m_pTextStream << hms << ",";
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            *m_pTextStream << (m_vetControlListItem[i]->GetCheckedSeq() ? (int)m_vetControlListItem[i]->GetDegatingSate() : 2) << ",";
        }
        *m_pTextStream << "\n";
    }
}

void ControlView::onCheckAllSeq(bool chk)
{
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        if (!m_plistControlShow->item(i)->isHidden())
        {
            m_vetControlListItem[i]->SetCheckedSeq(chk);
        }
    }
}

void ControlView::onCheckShowSeq(bool chk)
{
    if (chk)
    {
        int cnt = 0;
        for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
        {
            bool chk2 = m_vetControlListItem[i]->GetCheckedSeq();
            if (!chk2)
            {
                const auto& iteFind = m_mapChannlShow.find(i);
                if (iteFind != m_mapChannlShow.end())
                {
                    m_vetControlListItem[i]->SetCheckedSeq(iteFind->second);
                    chk2 = true;
                }
            }
            m_plistControlShow->item(i)->setHidden(!chk2);

            if (chk2)
            {
                ++cnt;
            }
        }

        m_plabFilterCount->setText(QString::number(cnt));
        m_plabShowCount->setText(QString::number(cnt));
    }
    else
    {
        m_mapChannlShow.clear();
        if (m_pchkKeepShow->isChecked())
        {
            for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
            {
                if (!m_plistControlShow->item(i)->isHidden())
                {
                    m_mapChannlShow[i] = m_vetControlListItem[i]->GetCheckedSeq();
                }
            }
        }
        for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
        {
            m_plistControlShow->item(i)->setHidden(false);
        }
    }
}

void ControlView::onCheckAllVisible(bool chk)
{
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        if (!m_plistControlShow->item(i)->isHidden())
        {
            m_vetControlListItem[i]->SetChecked(chk);
        }
    }
}

void ControlView::onCheckShowVisible(bool chk)
{
    if (chk)
    {
        int cnt = 0;
        for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
        {
            bool chk2 = m_vetControlListItem[i]->GetChecked();
            if (!chk2)
            {
                const auto& iteFind = m_mapChannlShow.find(i);
                if (iteFind != m_mapChannlShow.end())
                {
                    m_vetControlListItem[i]->SetChecked(iteFind->second);
                    chk2 = true;
                }
            }
            m_plistControlShow->item(i)->setHidden(!chk2);

            if (chk2)
            {
                ++cnt;
            }
        }

        m_plabFilterCount->setText(QString::number(cnt));
        m_plabShowCount->setText(QString::number(cnt));
    }
    else
    {
        m_mapChannlShow.clear();
        if (m_pchkKeepShow->isChecked())
        {
            for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
            {
                if (!m_plistControlShow->item(i)->isHidden())
                {
                    m_mapChannlShow[i] = m_vetControlListItem[i]->GetChecked();
                }
            }
        }
        for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
        {
            m_plistControlShow->item(i)->setHidden(false);
        }
    }
}

void ControlView::onPlotPageViewChange(int index)
{
    //std::vector<bool> vetChecked(CHANNEL_NUM, false);
    GetActiveChannelIndex(index);
    bool onlyShow = m_pchkShowVisible->isChecked();
    int cnt = 0;
    for (int i = 0; i < CHANNEL_NUM; i++)
    {
        if (i >= m_iStartIndex && i < m_iEndIndex)
        {
            bool chk = m_vetControlListItem[i]->GetChecked();
            //vetChecked[i] = chk;
            bool hide = onlyShow ? (!chk) : false;
            m_plistControlShow->item(i)->setHidden(hide);
            if (!hide)
            {
                ++cnt;
            }
            emit checkChangedSignal(i, chk);
        }
        else
        {
            m_plistControlShow->item(i)->setHidden(true);
            emit checkChangedSignal(i, false);
        }
    }

    m_plabFilterCount->setText(QString::number(cnt));
}

void ControlView::onClickEnableAuto(bool chk)
{
    if (chk)
    {
        if (m_pbtnEnableProtect->isChecked())
        {
            m_pbtnEnableProtect->click();
        }
        m_pbtnEnableProtect->setEnabled(false);

        m_pdspSingleLimit->setEnabled(false);
        m_pdspMultiLimit->setEnabled(false);
        m_pdspSaturatedLimit->setEnabled(false);
        //m_pbtnAutoZero->setEnabled(true);
        //m_pchkAllAuto->setEnabled(true);
        m_pbtnEnableAuto->setText("DisableAuto");

        //int porecnt = m_pispPoreStatus->value();
        //double sing = m_pdspSingleLimit->value();
        //double mult = m_pdspMultiLimit->value();
        //double satu = m_pdspSaturatedLimit->value();
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            //m_vetControlListItem[i]->SetPoreLimitVolt(porecnt, sing, mult, satu);
            //m_vetControlListItem[i]->SetEnableAutoZero(chk);
            m_vetControlListItem[i]->SetProtectVolt(m_pdspVolt->value());
        }
        if (!m_tmrAutoZero.isActive())
        {
            m_tmrAutoZero.start(500);
        }
    }
    else
    {
        //if (m_pbtnAutoZero->isChecked())
        //{
        //    m_pbtnAutoZero->click();
        //}
        //m_pchkAllAuto->setEnabled(false);
        //m_pbtnAutoZero->setChecked(false);
        //m_pbtnAutoZero->setEnabled(false);
        m_pdspSingleLimit->setEnabled(true);
        m_pdspMultiLimit->setEnabled(true);
        m_pdspSaturatedLimit->setEnabled(true);
        m_pbtnEnableAuto->setText("EnableAuto");

        m_pbtnEnableProtect->setEnabled(true);

        //double sing = m_pdspSingleLimit->value();
        //double mult = m_pdspMultiLimit->value();
        //double satu = m_pdspSaturatedLimit->value();
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            //m_vetControlListItem[i]->SetEnableAutoZero(chk);
            m_vetControlListItem[i]->ResetHoleStatus();
        }
        if (m_tmrAutoZero.isActive())
        {
            m_tmrAutoZero.stop();
        }
    }
    //m_bEnableAuto = chk;
}

void ControlView::onClickEnableProtect(bool chk)
{
    if (chk)
    {
        if (m_pbtnEnableAuto->isChecked())
        {
            m_pbtnEnableAuto->click();
        }
        m_pbtnEnableAuto->setEnabled(false);
        m_pdspVolt->setEnabled(false);

        m_pdspSingleLimit->setEnabled(false);
        m_pdspMultiLimit->setEnabled(false);
        m_pdspSaturatedLimit->setEnabled(false);
        m_pbtnEnableProtect->setText("DisableAutoC");

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->SetPorePortect(true);
        }
        if (!m_tmrAutoProtect.isActive())
        {
            m_tmrAutoProtect.start(2000);
        }
    }
    else
    {
        if (m_tmrAutoProtect.isActive())
        {
            m_tmrAutoProtect.stop();
        }

        m_pbtnEnableAuto->setEnabled(true);
        m_pdspVolt->setEnabled(true);

        m_pdspSingleLimit->setEnabled(true);
        m_pdspMultiLimit->setEnabled(true);
        m_pdspSaturatedLimit->setEnabled(true);
        m_pbtnEnableProtect->setText("EnableAutoC");

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->ResetHoleStatus();
            m_vetControlListItem[i]->SetPorePortect(false);
        }
    }
}

//void ControlView::onClickAutoSetZero(bool chk)
//{
//    if (chk)
//    {
//        m_pbtnAutoZero->setText("DisableAuto");
//        m_tmrAutoZero.start(500);
//    }
//    else
//    {
//        m_pbtnAutoZero->setText("AutoZero");
//        if (m_tmrAutoZero.isActive())
//        {
//            m_tmrAutoZero.stop();
//        }
//    }
//}

//void ControlView::onCheckAllAuto(bool chk)
//{
//    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
//    {
//        m_vetControlListItem[i]->SetAutoZeroCheck(chk);
//    }
//}

void ControlView::onClickShowSinglePore(void)
{
    //m_pchkShowVisible->setChecked(false);
    //for (auto&& ite : m_vetControlListItem)
    //{
    //    ite->SetChecked(ite->GetHoleState() == PoreHoleStatus::PHS_SINGLE);
    //}
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (!m_plistControlShow->item(i)->isHidden())
        {
            auto& ite = m_vetControlListItem[i];
            ite->SetChecked(ite->GetHoleStatus() == EEmbedPoreStatus::EPS_SINGLEPORE);
            ite->SetCheckedSeq(ite->GetHoleStatus() == EEmbedPoreStatus::EPS_SINGLEPORE);
        }
    }
}

void ControlView::onCheckCurrentFilter(bool chk)
{
    m_pdspFilterMin->setEnabled(chk);
    m_pdspFilterMax->setEnabled(chk);
}

void ControlView::onCheckStdFilter(bool chk)
{
    m_pdspFilterSTDMin->setEnabled(chk);
    m_pdspFilterSTD->setEnabled(chk);
}

void ControlView::onClickFilter(void)
{
    bool chkval = m_pchkFilter->isChecked();
    bool chkstd = m_pchkFilterSTD->isChecked();
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        bool chk = true;
        if (chkval)
        {
            const auto& stravg = m_vetControlListItem[i]->GetAvgCurrent();
            const auto& avg = ::atof(stravg.toStdString().c_str());
            chk = avg >= m_pdspFilterMin->value() && avg <= m_pdspFilterMax->value();
        }
        if (chkstd)
        {
            bool stdchk = (m_arCurrentStd[i] >= m_pdspFilterSTDMin->value()) && (m_arCurrentStd[i] <= m_pdspFilterSTD->value());
            chk &= stdchk;
        }
        m_vetControlListItem[i]->SetChecked(chk);
        m_vetControlListItem[i]->SetCheckedSeq(chk);
    }
    m_pchkShowVisible->setChecked(true);
    onCheckShowVisible(true);
    //emit m_pchkShowVisible->toggled(true);
}

void ControlView::timerCurCurrentSlot(void)
{
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        auto cur = QString::number(m_arCurrentInstant[i], 'f', 2);
        m_vetControlListItem[i]->SetCurCurrent(cur);
    }
}

void ControlView::timerDegatingSlot(void)
{
    std::lock_guard<std::mutex> lock(m_mutexdegating);
    if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
    {
        m_vetAvgCur = m_vetDegatingData;
        m_cvGating.notify_all();
    }
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetDegatingData[i].clear();
    }
}

void ControlView::timerAvgCurrentSlot(void)
{
    std::lock_guard<std::mutex> lock(m_mutexavg);
    if (ConfigServer::GetInstance()->GetZeroAdjust())
    {
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            auto& avgCurrent = m_vetAverageData[i];
            int avgsz = avgCurrent.size();
            if (avgsz > 0)
            {
                double sum = std::accumulate(avgCurrent.begin(), avgCurrent.end(), 0.0);
                double mean = sum / avgsz;
                QString avg = QString::number(mean, 'f', 2);
                m_vetControlListItem[i]->SetAvgCurrent(avg);

                std::lock_guard<std::mutex> lock(m_mutexZeroAdjust);
                m_arZeroAdjustOffset[i] += mean;
            }
            avgCurrent.clear();
        }
    }
    else if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
    {
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            auto& avgCurrent = m_vetAverageData[i];
            int avgsz = avgCurrent.size();
            if (avgsz > 0)
            {
                double sum = std::accumulate(avgCurrent.begin(), avgCurrent.end(), 0.0);
                double mean = sum / avgsz;
                QString avg = QString::number(mean, 'f', 2);
                m_vetControlListItem[i]->SetAvgCurrent(avg);
            }
            avgCurrent.clear();
        }
    }
    else
    {
        int porecnt = m_pispPoreStatus->value();
        double sing = m_pdspSingleLimit->value();
        double mult = m_pdspMultiLimit->value();
        double satu = m_pdspSaturatedLimit->value();
        if (ConfigServer::GetInstance()->GetDeveloperMode() != 1)
        {
            porecnt = m_pispPoreStatus3->value();
            sing = m_pdspSingleLimit3->value();
            mult = m_pdspMultiLimit3->value();
            satu = m_pdspSaturatedLimit3->value();
        }
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            auto& avgCurrent = m_vetAverageData[i];
            int avgsz = avgCurrent.size();
            if (avgsz > 0)
            {
                double sum = std::accumulate(avgCurrent.begin(), avgCurrent.end(), 0.0);
                double mean = sum / avgsz;
                QString avg = QString::number(mean, 'f', 2);
                m_vetControlListItem[i]->SetAvgCurrent(avg);
                m_vetControlListItem[i]->SetHoleStatus(porecnt, sing, mult, satu, mean);
                //m_vetControlListItem[i]->SetHoleStatus(mean);
            }
            avgCurrent.clear();
        }
    }
}

void ControlView::timerStdCurrentSlot(void)
{
    //m_bStdCalculating = true;
    {
        std::lock_guard<std::mutex> lock(m_mutexstd);
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            auto& stdCurrent = m_vetStdData[i];
            int stdsz = stdCurrent.size();
            if (stdsz > 0)
            {
                double sum = std::accumulate(stdCurrent.begin(), stdCurrent.end(), 0.0);
                double mean = sum / stdsz;

                sum = 0.0;
                for (const auto& ite : stdCurrent)
                {
                    sum += std::pow(ite - mean, 2);
                }
                m_arCurrentStd[i] = sqrt(sum / stdsz);
                QString avg = QString::number(m_arCurrentStd[i], 'f', 4);
                m_vetControlListItem[i]->SetStdCurrent(avg);
            }
            stdCurrent.clear();
        }
    }
    //m_bStdCalculating = false;
}

void ControlView::timerAutoZeroSlot(void)
{
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        m_vetControlListItem[i]->AutoSetStatusChange();
    }
}

void ControlView::timerAutoProtectSlot(void)
{
    if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())//when ZeroAdjust not Filter
    {
        if (ConfigServer::GetInstance()->GetDeveloperMode() == 1)
        {
            SetDCLevelBiggerCloseSlot(m_pdspMultiLimit->value(), m_pdspSaturatedLimit->value());//m_pdspSaturatedLimit
        }
        else
        {
            SetDCLevelBiggerCloseSlot(m_pdspMultiLimit3->value(), m_pdspSaturatedLimit3->value());//m_dProtectCurrent
        }
    }
}

//void ControlView::setZeroVoltSlot(int chan, int stat)
//{
//    double volt = 0.0;
//    if (stat == EEmbedPoreStatus::EPS_SINGLEPORE)
//    {
//        volt = m_pdspVolt->value();
//        m_vetControlListItem[chan]->SetStausStyleSheet(g_colorStyle[EEmbedPoreStatus::EPS_SINGLEPORE_PROTECT]);
//    }
//    else
//    {
//        m_vetControlListItem[chan]->SetStausStyleSheet(g_borderStyle[1] + g_colorStyle[stat]);
//    }
//
//    SetOneChannelDCLevel(chan, volt);
//}

void ControlView::onCheckAllChannel(bool chk)
{
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        if (!m_plistControlShow->item(i)->isHidden())
        {
            m_vetControlListItem[i]->SetChecked2(chk);
        }
    }
}

void ControlView::onClickVoltageOpposite(void)
{
    double voltampl = m_pdspVoltAmpl->value();
    m_pdspVoltAmpl->setValue(-voltampl);
    onClickVoltageApply();
}

void ControlView::onClickVoltageApply(void)
{
    int voltmode = m_pcmbVolyType->currentIndex();
    double voltampl = m_pdspVoltAmpl->value();
    double voltfreq = m_pdspVolyFreq->value();
    ConfigServer::GetInstance()->SetTriangAmplitude(voltampl);
    ConfigServer::GetInstance()->SetTriangFrequence(voltfreq);

    if (m_pradAllChannel->isChecked())//m_pchkAllChannel->checkState() == Qt::CheckState::Checked
    {
        int arout = 0;
        size_t len = 0;
        //All Check
        switch (voltmode)
        {
        case 0:
        {
            len = UsbProtoMsg::SetAllDCLevel(voltampl, arout);
            if (ConfigServer::GetInstance()->GetSaveData())
            {
                time_t setvoltmt = time(nullptr);
                char arvoltinfo[1024] = { 0 };
                sprintf(arvoltinfo, "[%f] AllCH Volt0=%.2f\n", difftime(setvoltmt, ConfigServer::GetInstance()->GetStartSaveData()) / 60.0, voltampl);
                //sprintf(arvoltinfo, "[%f] AllCH Volt0=%.2f\n", ConfigServer::GetInstance()->GetRawDataTime(), voltampl);
                ConfigServer::GetInstance()->EnqueueVoltageInfo(std::string(arvoltinfo));
                //LOGI(arvoltinfo);
            }

            break;
        }
        case 1:
            len = UsbProtoMsg::SetAllPulseLevel(voltampl, arout);
            break;
        case 2:
            len = UsbProtoMsg::SetAllTrianglewaveLevel(voltampl, voltfreq, arout);
            break;
        case 3:
            len = UsbProtoMsg::SetAllRectanglewaveLevel(voltampl, voltfreq, arout);
            break;
        default:
            QMessageBox::warning(this, tr("Warning"), QStringLiteral("Not support Volt Type = %1").arg(voltmode));
            return;
            break;
        }

        emit transmitUsbData(arout, len);

        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetControlListItem[i]->SetVoltAmpl(voltampl);
        }
    }
    else
    {
        //One Check
        switch (voltmode)
        {
        case 0:
            for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
            {
                if (m_vetControlListItem[i]->GetChecked2())
                {
                    SetOneChannelDCLevel(i, voltampl);
                    if (ConfigServer::GetInstance()->GetSaveData())
                    {
                        time_t setvoltmt = time(nullptr);
                        char arvoltinfo[1024] = { 0 };
                        sprintf(arvoltinfo, "[%f] CH%d Volt1=%.2f\n", difftime(setvoltmt, ConfigServer::GetInstance()->GetStartSaveData()) / 60.0, i + 1, voltampl);
                        //sprintf(arvoltinfo, "[%f] CH%d Volt1=%.2f\n", ConfigServer::GetInstance()->GetRawDataTime(), i + 1, voltampl);
                        ConfigServer::GetInstance()->EnqueueVoltageInfo(std::string(arvoltinfo));
                        //LOGI(arvoltinfo);
                    }
                }
            }
            break;
        case 1:
            for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
            {
                if (m_vetControlListItem[i]->GetChecked2())
                {
                    SetOneChannelPulseLevel(i, voltampl);
                }
            }
            break;
        case 2:
            QMessageBox::information(this, tr("Tips"), QStringLiteral("Not support set single trianglewave = %1").arg(voltmode));
            return;
            break;
        case 3:
            QMessageBox::information(this, tr("Tips"), QStringLiteral("Not support set single rectanglewave = %1").arg(voltmode));
            return;
            break;
        default:
            QMessageBox::warning(this, tr("Warning"), QStringLiteral("Not support Volt Type2 = %1").arg(voltmode));
            return;
            break;
        }
    }
}

void ControlView::onClickEnableDegating(bool chk)
{
#if 1

    if (chk)
    {
        m_pispCyclePeriod->setEnabled(false);
        m_pispAllowTimes->setEnabled(false);
        m_pchkAutoseqGating->setEnabled(false);

        onClickDegatingApply();
        onClickAdvanceApply();

        m_bAutoseqGating = m_pchkAutoseqGating->isChecked();
        if (m_bAutoseqGating)
        {
            if (!m_bRedisReady)
            {
                m_bRedisReady = InitGatingRedis();
            }
            if (!m_bRedisReady)
            {
                LOGE("Init Redis FAILED!!!");
            }
            else
            {
                m_iDegatingDataPoint = m_pispCyclePeriod->value() * m_dSampleRate;
                for (int k = 0; k < m_ciThread; ++k)
                {
                    m_bDegatingSample[k] = true;
                }

                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    //m_vetControlListItem[i]->SetDegatingEnable(true);
                    m_vetControlListItem[i]->ResetDegatingState();
                    m_vetControlListItem[i]->SetDegatingLimitTimer();
                    //m_vetControlListItem[i]->SetDegatingTimer();
                    //m_vetControlListItem[i]->SetDegatingCount("0");

                    m_arGatingStatus[i] = 0;
                }

                StartThread3();
                ConfigServer::GetInstance()->SetAutoDegating(true);
                //if (!m_tmrDegating.isActive())
                //{
                //    m_tmrDegating.start(2000);
                //}
            }
        }
        else
        {
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                //m_vetControlListItem[i]->SetDegatingEnable(true);
                m_vetControlListItem[i]->ResetDegatingState();
                //m_vetControlListItem[i]->SetDegatingLimitTimer();
                m_vetControlListItem[i]->SetDegatingTimer();
                //m_vetControlListItem[i]->SetDegatingCount("0");

                m_arGatingStatus[i] = 0;
            }

            StartThread2();
            ConfigServer::GetInstance()->SetAutoDegating(true);
            if (!m_tmrDegating.isActive())
            {
                m_tmrDegating.start(1000);////m_pispCyclePeriod->value() * 1000
            }
        }

        m_pbtnDegatingEnable->setText("Disable");
    }
    else
    {

        if (m_pchkAutoseqGating->isChecked())
        {
            EndThread3();
            ConfigServer::GetInstance()->SetAutoDegating(false);
            for (int k = 0; k < m_ciThread; ++k)
            {
                m_bDegatingSample[k] = false;
            }
            if (m_tmrDegating.isActive())
            {
                m_tmrDegating.stop();
            }
            StopThread3();
        }
        else
        {
            EndThread2();
            ConfigServer::GetInstance()->SetAutoDegating(false);
            for (int k = 0; k < m_ciThread; ++k)
            {
                m_bDegatingSample[k] = false;
            }
            if (m_tmrDegating.isActive())
            {
                m_tmrDegating.stop();
            }
            StopThread2();
        }

        m_pispCyclePeriod->setEnabled(true);
        m_pispAllowTimes->setEnabled(true);
        m_pchkAutoseqGating->setEnabled(true);

        m_pbtnDegatingEnable->setText("Enable");

        int cnt = 0;
        int chn = 0;
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetDegatingData[i].clear();

            //m_vetControlListItem[i]->SetDegatingEnable(false);
            m_vetControlListItem[i]->ResetDegatingTimer();

            if (!m_plistControlShow->item(i)->isHidden() && m_vetControlListItem[i]->GetCheckedSeq())
            {
                ++chn;
                cnt += m_vetControlListItem[i]->GetDegatingCount().toInt();
            }
        }
        m_plabCalcDegating->setText(QString("Avg:%1/%2=%3").arg(cnt).arg(chn).arg((chn != 0) ? (double)((double)cnt / chn) : 0.0));
    }
    //m_pchkDegatingAllCheck->setEnabled(chk);

#else

    if (chk)
    {
        //if (m_pbtnEnableAuto->isChecked())
        //{
        //    m_pbtnEnableAuto->click();
        //}
        //m_pbtnEnableAuto->setEnabled(false);

        m_pispCyclePeriod->setEnabled(false);
        m_pispAllowTimes->setEnabled(false);

        m_pchkAutoseqGating->setEnabled(false);
        onClickDegatingApply();
        if (m_pchkAutoseqGating->isChecked())
        {
            m_pgrpDegatingAdvanced->setEnabled(false);
            //if (!m_bRedisReady)
            //{
            //    m_bRedisReady = InitGatingRedis();
            //}
            //if (!m_bRedisReady)
            //{
            //    LOGE("Init Redis FAILED!!!");
            //}
            //else
            {
#if 0
                if (!m_tmrAutoseqGating.isActive())
                {
                    m_tmrAutoseqGating.start(m_pispCyclePeriod->value() * 1000);
                }
#else

                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    m_vetControlListItem[i]->ResetDegatingState();
                    m_vetControlListItem[i]->SetDegatingLimitTimer();

                    m_arGatingStatus[i] = 0;
                }

                StartThread3();
                ConfigServer::GetInstance()->SetAutoDegating(true);
                if (!m_tmrDegating.isActive())
                {
                    m_tmrDegating.start(2000);
                }
#endif
            }
        }
        else
        {
            onClickAdvanceApply();

            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                //m_vetControlListItem[i]->SetDegatingEnable(true);
                m_vetControlListItem[i]->ResetDegatingState();
                m_vetControlListItem[i]->SetDegatingTimer();
                //m_vetControlListItem[i]->SetDegatingCount("0");

                m_arGatingStatus[i] = 0;
            }

            StartThread2();
            ConfigServer::GetInstance()->SetAutoDegating(true);
            if (!m_tmrDegating.isActive())
            {
                m_tmrDegating.start(1000);
            }
        }

#if 0
        m_pchkAutoseqGating->setEnabled(false);
        if (m_pchkAutoseqGating->isChecked())
        {
            m_pgrpDegatingAdvanced->setEnabled(false);
            //if (!StartThread3())
            //{
            //    LOGW("Start Autoseq Gating FAILED!!!");
            //}
            if (!m_bRedisReady)
            {
                m_bRedisReady = InitGatingRedis();
            }
            if (!m_bRedisReady)
            {
                LOGE("Init Redis FAILED!!!");
            }
            else
            {
                if (!m_tmrAutoseqGating.isActive())
                {
                    m_tmrAutoseqGating.start(1200);
                }
            }
        }
        else
        {
            StartThread2();
            ConfigServer::GetInstance()->SetAutoDegating(true);
            if (!m_tmrDegating.isActive())
            {
                m_tmrDegating.start(1000);
            }
        }
#endif

        m_pbtnDegatingEnable->setText("Disable");
    }
    else
    {
        if (m_pchkAutoseqGating->isChecked())
        {
#if 0
            if (m_tmrAutoseqGating.isActive())
            {
                m_tmrAutoseqGating.stop();
            }
#else
            EndThread3();
            ConfigServer::GetInstance()->SetAutoDegating(false);
            if (m_tmrDegating.isActive())
            {
                m_tmrDegating.stop();
            }
            StopThread3();
#endif
            m_pgrpDegatingAdvanced->setEnabled(true);
        }
        else
        {
            EndThread2();
            ConfigServer::GetInstance()->SetAutoDegating(false);
            if (m_tmrDegating.isActive())
            {
                m_tmrDegating.stop();
            }
            StopThread2();
        }
        m_pchkAutoseqGating->setEnabled(true);

        m_pbtnDegatingEnable->setText("Enable");

        //m_pbtnEnableAuto->setEnabled(true);

        m_pispCyclePeriod->setEnabled(true);
        m_pispAllowTimes->setEnabled(true);

        int cnt = 0;
        int chn = 0;
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            m_vetDegatingData[i].clear();

            //m_vetControlListItem[i]->SetDegatingEnable(false);
            m_vetControlListItem[i]->ResetDegatingTimer();

            if (!m_plistControlShow->item(i)->isHidden() && m_vetControlListItem[i]->GetCheckedSeq())
            {
                ++chn;
                cnt += m_vetControlListItem[i]->GetDegatingCount().toInt();
            }
        }
        m_plabCalcDegating->setText(QString("Avg:%1/%2=%3").arg(cnt).arg(chn).arg((chn != 0) ? (double)((double)cnt / chn) : 0.0));
    }
    //m_pchkDegatingAllCheck->setEnabled(chk);
#endif
}

void ControlView::onClickDegatingApply(void)
{
    auto degatingvolt = m_pdspDegatingVolt->value();
    auto cycle = m_pispCyclePeriod->value();
    auto duration = m_pispDurationTime->value();
    auto step = m_pispSteppingTime->value();
    auto allow = m_pispAllowTimes->value();
    auto limit = m_pispLimitCycle->value();
    //double seqvolt = 0.18;
    //emit getSequencingVoltageSignal(seqvolt);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetControlListItem[i]->SetDegatingParam(degatingvolt, cycle, duration, step, allow, limit);
    }
}

void ControlView::onTimerAutoseqGating(void)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto gating = m_redisAccess.GetHash((m_strDataName + "_gating").c_str(), std::to_string(i + 1).c_str());
        int gat = ::atoi(gating.c_str());
        if (gat == 1)
        {
            ++m_arGatingStatus[i];
            int gatingcnt = m_arGatingStatus[i];
            if (gatingcnt >= 3)
            {
                m_vetControlListItem[i]->SetDegatingState(ControlListItem::EGatingState::EGS_GATING);
                //m_arGatingStatus[i] = 0;
            }
        }
        else
        {
            m_vetControlListItem[i]->SetDegatingState(ControlListItem::EGatingState::EGS_NORMAL);
            m_arGatingStatus[i] = 0;
        }
        //m_vetControlListItem[i]->SetDegatingState((gat == 1) ? ControlListItem::EGatingState::EGS_GATING : ControlListItem::EGatingState::EGS_NORMAL);
    }
}

//void ControlView::onCheckDegatingAllCheck(bool chk)
//{
//    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
//    {
//        if (!m_plistControlShow->item(i)->isHidden())
//        {
//            m_vetControlListItem[i]->SetDegatingChecked(chk);
//        }
//    }
//}

void ControlView::onClickCalculation(bool chk)
{
    if (chk)
    {
        int sec = m_pispLimitCycle->value() * m_pispCyclePeriod->value();
        int rest = sec % 3600;
        m_tmCalculation.setHMS(sec / 3600, rest / 60, rest % 60);
        m_pbtnDegatingEnable->click();
        if (!m_pbtnDegatingEnable->isChecked())
        {
            m_pbtnDegatingEnable->click();
        }
        //m_pchkDegatingAllCheck->click();
        //if (!m_pchkDegatingAllCheck->isChecked())
        //{
        //    m_pchkDegatingAllCheck->click();
        //}
        m_tmrCalculation.start(1000);

        m_pbtnCalcDegating->setText(QString("StopAvgCal (%1)").arg(sec));
    }
    else
    {
        if (m_tmrCalculation.isActive())
        {
            m_tmrCalculation.stop();
        }
        if (m_pbtnDegatingEnable->isChecked())
        {
            m_pbtnDegatingEnable->click();
        }

        m_pbtnCalcDegating->setText(QString("StartAvgCal"));
    }
}

void ControlView::onTimerCalculationStart(void)
{
    m_tmCalculation = m_tmCalculation.addSecs(-1);
    m_pbtnCalcDegating->setText(QString("StopAvgCal (%1)").arg(3600 * m_tmCalculation.hour() + 60 * m_tmCalculation.minute() + m_tmCalculation.second()));
    if (m_tmCalculation == QTime(0, 0, 0, 0))
    {
        if (m_pbtnCalcDegating->isChecked())
        {
            m_pbtnCalcDegating->click();
            ConfigServer::caliSleep(1000);
            if (!m_pbtnDegatingEnable->isChecked())
            {
                m_pbtnDegatingEnable->click();
            }
        }
    }
}

void ControlView::onClickDefaultApply(void)
{
    m_pdspDegatingVolt->setValue(-0.1);
    m_pispCyclePeriod->setValue(2);
    m_pispDurationTime->setValue(200);

    m_pdspDurationThres->setValue(0.5);
    m_pispGatingSTD->setValue(5);
    m_pispSignalMin->setValue(20);
    m_pispSignalMax->setValue(150);
    m_pispGatingMin->setValue(40);
    m_pispGatingMax->setValue(130);
}

void ControlView::onClickAdvanceApply(void)
{
    m_dDurationThres = m_pdspDurationThres->value();
    m_iGatingSTD = m_pispGatingSTD->value();
    m_iGatingSTDMax = m_pispGatingSTDMax->value();
    m_iSignalMin = m_pispSignalMin->value();
    m_iSignalMax = m_pispSignalMax->value();
    m_iGatingMin = m_pispGatingMin->value();
    m_iGatingMax = m_pispGatingMax->value();
}

void ControlView::singleChanDegatingSlot(int chan, double volt)
{
    SetSepChannelDCLevel(chan, volt);
}

void ControlView::singleChanDegatingRecoverSlot(int chan)
{
    double volt = m_vetControlListItem[chan]->GetVoltAmpl();
    //emit getSequencingVoltageSignal(volt);

    SetSepChannelDCLevel(chan, volt);
}

void ControlView::onClickZeroAdjustDefault(void)
{
    m_pdspBackwardVoltage->setValue(-0.18);
    m_pdspBackwardDuration->setValue(1.0);
    m_pdspZeroVoltDuration->setValue(3.0);
    m_pdspZeroAdjustTime->setValue(6.0);
}

void ControlView::onClickZeroAdjust(void)
{
    if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
    {
        return;
    }
    double spevolt = m_pdspBackwardVoltage->value();
    double spetime = m_pdspBackwardDuration->value();
    double zerotime = m_pdspZeroVoltDuration->value();
    double adjusttime = m_pdspZeroAdjustTime->value();

    DoZeroAdjust(spevolt, spetime, zerotime, adjusttime);
    return;

    ConfigServer::GetInstance()->SetZeroAdjustProcess(true);

    int tottime = spetime + zerotime + adjusttime;
    m_pProgressDialog->setRange(0, tottime);
    int progress = 0;
    m_pProgressDialog->setValue(progress);

    setZeroAdjustSpeVoltSlot(spevolt);
    for (int i = 0; i < spetime; ++i)
    {
        if (m_pProgressDialog->wasCanceled())
        {
            setZeroAdjustEndSlot();
            m_pProgressDialog->reset();
            return;
        }
        ConfigServer::caliSleep(1000);
        m_pProgressDialog->setValue(++progress);
    }

    setZeroAdjustAllzeroVoltSlot();
    for (int i = 0; i < zerotime; ++i)
    {
        if (m_pProgressDialog->wasCanceled())
        {
            setZeroAdjustEndSlot();
            m_pProgressDialog->reset();
            return;
        }
        ConfigServer::caliSleep(1000);
        m_pProgressDialog->setValue(++progress);
    }

    setZeroAdjustStartSlot();
    for (int i = 0; i < adjusttime; ++i)
    {
        if (m_pProgressDialog->wasCanceled())
        {
            setZeroAdjustEndSlot();
            m_pProgressDialog->reset();
            return;
        }
        ConfigServer::caliSleep(1000);
        m_pProgressDialog->setValue(++progress);
    }

    setZeroAdjustEndSlot();
    //ConfigServer::GetInstance()->SetZeroAdjust(false);
    //for (int i = 0; i < 2; ++i)
    //{
    //    if (m_pProgressDialog->wasCanceled())
    //    {
    //        setZeroAdjustEndSlot();
    //        m_pProgressDialog->reset();
    //        return;
    //    }
    //    ConfigServer::caliSleep(1000);
    //    m_pProgressDialog->setValue(++progress);
    //}
    //for (int i = 0; i < CHANNEL_NUM; ++i)
    //{
    //    m_vetControlListItem[i]->onClickVoltageApply();
    //}
    //ConfigServer::GetInstance()->SetZeroAdjustProcess(false);

}

void ControlView::onClickZeroAdjustStart(bool chk)
{
    if (chk)
    {
        m_pbtnZeroAdjustStart->setText("Stop");
        //SetTimehms();
        m_tmrAdjust.start(1000);
    }
    else
    {
        if (m_tmrAdjust.isActive())
        {
            m_tmrAdjust.stop();
        }
        m_pbtnZeroAdjustStart->setText("Start");
    }
}

void ControlView::onClickZeroAdjustReset(void)
{
    SetTimehms();
    m_plcdIntervalsTimer->display(m_tmAdjust.toString("hh:mm:ss"));
}

void ControlView::onTimerZeroAdjustStart(void)
{
    if (m_tmAdjust == QTime(0, 0, 0, 0))
    {
        if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
        {
            onClickZeroAdjust();
        }
        SetTimehms();
    }
    else
    {
        m_tmAdjust = m_tmAdjust.addSecs(-1);
    }
    m_plcdIntervalsTimer->display(m_tmAdjust.toString("hh:mm:ss"));
}

void ControlView::onClickPolymerDefault(void)
{
    m_pdspPoreLimit->setValue(100.0);
    m_pdspPoreVolt->setValue(0.0);
    m_pdspVoltStart->setValue(0.15);
    m_pdspVoltEnd->setValue(0.45);
    m_pdspVoltStep->setValue(0.01);
    m_pdspTimeStep->setValue(1.0);
}

void ControlView::onClickPolymerStart(bool chk)
{
    bool enb = !chk;
    m_pdspPoreLimit->setEnabled(enb);
    m_pdspPoreVolt->setEnabled(enb);
    m_pdspVoltStart->setEnabled(enb);
    m_pdspVoltEnd->setEnabled(enb);
    m_pdspVoltStep->setEnabled(enb);
    m_pdspTimeStep->setEnabled(enb);
    if (chk)
    {
        m_pbtnPolymerStart->setText("Stop");
        m_dPoreLimit = m_pdspPoreLimit->value();
        m_dPoreVolt = m_pdspPoreVolt->value();
        m_dVoltStart = m_pdspVoltStart->value();
        m_dVoltEnd = m_pdspVoltEnd->value();
        m_dVoltStep = m_pdspVoltStep->value();
        m_dTimeStep = m_pdspTimeStep->value();

        m_dCurVolt = m_dVoltStart;
        SetPolymerVoltage(m_dCurVolt);

        m_pdspSingleLimit->setValue(m_dPoreLimit);
        m_pdspVolt->setValue(m_dPoreVolt);
        //if (!m_pbtnEnableAuto->isChecked())
        //{
        //    m_pbtnEnableAuto->click();
        //}

        m_tmrPolymer.start(m_dTimeStep * 1000);
    }
    else
    {
        m_pbtnPolymerStart->setText("Start");
        if (m_tmrPolymer.isActive())
        {
            m_tmrPolymer.stop();
        }
    }
}

void ControlView::onTimerPolymerStart(void)
{
    if (m_dCurVolt < m_dVoltEnd)
    {
        m_dCurVolt += m_dVoltStep;
    }
    else
    {
        m_dCurVolt = m_dVoltStart;
    }
    SetPolymerVoltage(m_dCurVolt);
}

void ControlView::onClickRotationDefault(void)
{
    SetTimehmsMin();
    m_plcdRotationTime->display(m_tmRotation.toString("hh:mm:ss"));
}

void ControlView::onClickRotationStart(bool chk)
{
    if (chk)
    {
        //m_pispRotationTime->setEnabled(false);
        //m_pdspSeqencingVolt->setEnabled(false);
        //m_pdspWaitingVolt->setEnabled(false);
        //m_pcmbCurseqChannel->setEnabled(false);

        m_tmrRotation.start(1000);

        m_pbtnStartRotation->setText("Stop");
    }
    else
    {
        if (m_tmrRotation.isActive())
        {
            m_tmrRotation.stop();
        }

        //m_pispRotationTime->setEnabled(true);
        //m_pdspSeqencingVolt->setEnabled(true);
        //m_pdspWaitingVolt->setEnabled(true);
        //m_pcmbCurseqChannel->setEnabled(true);

        m_pbtnStartRotation->setText("Start Rotation");
    }
}

void ControlView::onTimerRotationStart(void)
{
    if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
    {
        return;
    }
    m_tmRotation = m_tmRotation.addSecs(-1);
    m_plcdRotationTime->display(m_tmRotation.toString("hh:mm:ss"));
    if (m_tmRotation == QTime(0, 0, 0, 0))
    {
        SetTimehmsMin();
        m_plcdRotationTime->display(m_tmRotation.toString("hh:mm:ss"));
        int cur = m_pcmbCurseqChannel->currentIndex();
        m_pcmbCurseqChannel->setCurrentIndex(cur == 0 ? 1 : 0);

        bool isDegating = m_pbtnDegatingEnable->isChecked();
        if (isDegating)
        {
            m_pbtnDegatingEnable->click();
            m_pbtnDegatingEnable->click();
        }

        //int cnt = 0;
        bool selchan[CHANNEL_NUM] = { false };
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            if (!m_plistControlShow->item(i)->isHidden())
            {
                //++cnt;
                //if (cnt % 2 == cur)
                if (i % 2 != cur)
                {
                    m_vetControlListItem[i]->SetChecked(false);
                    //m_vetControlListItem[i]->SetDegatingChecked(false);
                    m_vetControlListItem[i]->SetCheckedSeq(false);
                    SetOneChannelDCLevel(i, m_pdspWaitingVolt->value());
                    selchan[i] = false;
                }
                else
                {
                    m_vetControlListItem[i]->SetChecked(true);
                    //m_vetControlListItem[i]->SetDegatingChecked(true);
                    m_vetControlListItem[i]->SetCheckedSeq(true);
                    SetOneChannelDCLevel(i, m_pdspSeqencingVolt->value());
                    selchan[i] = true;
                }
            }
        }
        //if (ConfigServer::GetInstance()->GetSaveData())
        //{
        //    emit SetSelChannelSignal(selchan);
        //}
    }
}

void ControlView::onClickSimulationDefault(void)
{
    m_pdspVoltStart3->setValue(0.0);
    m_pdspVoltEnd3->setValue(1.0);
    m_pdspVoltStep3->setValue(0.1);
    m_pdspTimeStep3->setValue(1.0);
    m_pispSimulationCycle->setValue(30);
    m_pdspVoltOffset->setValue(0.0);
}

void ControlView::onClickSimulationStart(bool chk)
{
    bool enb = !chk;
    m_pdspVoltStart3->setEnabled(enb);
    m_pdspVoltEnd3->setEnabled(enb);
    m_pdspVoltStep3->setEnabled(enb);
    m_pdspTimeStep3->setEnabled(enb);
    m_pispSimulationCycle->setEnabled(enb);
    m_pcmbRampPattern->setEnabled(enb);
    m_pdspVoltOffset->setEnabled(enb);
    if (chk)
    {
        m_pbtnSimulationStart->setText("Stop");

        m_fVoltOffset = m_pdspVoltOffset->value();
        m_fVoltStart3 = m_pdspVoltStart3->value() + m_fVoltOffset;
        m_fVoltEnd3 = m_pdspVoltEnd3->value() + m_fVoltOffset;
        m_fVoltStep3 = m_pdspVoltStep3->value();
        m_fTimeStep3 = m_pdspTimeStep3->value();
        m_iSimulationCycle = m_pispSimulationCycle->value();
        m_eRampPattern = (ESimulationRampPattern)m_pcmbRampPattern->currentData().toInt();

        m_fCurVolt3 = m_fVoltStart3;
        SetSimulationVoltage(m_fCurVolt3);

        m_tmrSimulation.start(m_fTimeStep3 * 1000);
    }
    else
    {
        m_pbtnSimulationStart->setText("Start");

        if (m_tmrSimulation.isActive())
        {
            m_tmrSimulation.stop();
        }
    }
}

void ControlView::onTimerSimulationStart(void)
{
    if (m_eRampPattern == SRP_MIRROR)
    {
        if (m_bSimulRise)
        {
            if (m_fCurVolt3 < m_fVoltEnd3)
            {
                m_fCurVolt3 += m_fVoltStep3;
            }
            else
            {
                m_bSimulRise = false;
                m_fCurVolt3 -= m_fVoltStep3;
            }
        }
        else
        {
            auto dist = m_fCurVolt3 - m_fVoltStep3;
            if (dist > m_fVoltStart3 || ISDOUBLEZERO((dist - m_fVoltStart3)))
            {
                m_fCurVolt3 -= m_fVoltStep3;
            }
            else
            {
                m_bSimulRise = true;
                m_fCurVolt3 += m_fVoltStep3;
                --m_iSimulationCycle;
            }
        }
    }
    else if (m_eRampPattern == SRP_RISE)
    {
        if (m_fCurVolt3 < m_fVoltEnd3)
        {
            m_fCurVolt3 += m_fVoltStep3;
        }
        else
        {
            m_fCurVolt3 = m_fVoltStart3;
            --m_iSimulationCycle;
        }
    }
    else if (m_eRampPattern == SRP_FALL)
    {
        if (m_fCurVolt3 > m_fVoltStart3)
        {
            m_fCurVolt3 -= m_fVoltStep3;
        }
        else
        {
            m_fCurVolt3 = m_fVoltEnd3;
            --m_iSimulationCycle;
        }
    }
    else
    {
        //wrong....
        if (m_tmrSimulation.isActive())
        {
            m_tmrSimulation.stop();
        }
        return;
    }

    SetSimulationVoltage(m_fCurVolt3);
    if (m_iSimulationCycle <= 0)
    {
        if (m_pbtnSimulationStart->isChecked())
        {
            m_pbtnSimulationStart->click();
        }
        //if (m_timerSimulation.isActive())
        //{
        //    m_timerSimulation.stop();
        //}
    }
}

void ControlView::SetAllChannelVoltSlot(double ampl)
{
    m_pchkAllCheck->setChecked(true);
    m_pcmbVolyType->setCurrentIndex(0);
    m_pdspVoltAmpl->setValue(ampl);
    onClickVoltageApply();
}

void ControlView::SetChannelDCLevelSlot(int chan, double volt)
{
    SetSepChannelDCLevel(chan, volt);
}

void ControlView::setZeroAdjustSpeVoltSlot(double val)
{
    //for (int i = 0; i < CHANNEL_NUM; ++i)
    //{
    //    //if (m_vetControlListItem[i]->GetPoreState())
    //    if (!m_plistControlShow->item(i)->isHidden())
    //    {
    //        //SetOneChannelDCLevel(i, val);
    //        int arout = 0;
    //        auto len = UsbProtoMsg::SetOneDCLevel(i, val, arout);
    //        emit transmitUsbData(arout, len);
    //    }
    //}
    SetAllChannelDCLevel(val);
}

void ControlView::setZeroAdjustAllzeroVoltSlot(void)
{
    SetAllChannelDCLevel(0.0);
}

void ControlView::setZeroAdjustStartSlot(void)
{
    ConfigServer::GetInstance()->SetZeroAdjust(true);
    //memset(m_arZeroAdjustOffset, 0.0, CHANNEL_NUM * sizeof(float));
    //setZeroAdjustEndAvgSlot();
}

void ControlView::setZeroAdjustEndSlot(void)
{
    ConfigServer::GetInstance()->SetZeroAdjust(false);

    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetControlListItem[i]->onClickVoltageApply();
    }
    ConfigServer::GetInstance()->SetZeroAdjustProcess(false);
}

//void ControlView::setZeroAdjustEndAvgSlot(void)
//{
//    std::lock_guard<std::mutex> lock(m_mutexavg);
//
//    for (int i = 0; i < CHANNEL_NUM; ++i)
//    {
//        m_vetAverageData[i].clear();
//    }
//}

void ControlView::setTriangeVoltageSlot(int val)
{
    if (val == 1)
    {
        m_dLastVoltage = m_pdspVoltAmpl->value();
        m_pradAllChannel->setChecked(true);
        m_pcmbVolyType->setCurrentIndex(2);
        m_pdspVoltAmpl->setValue(0.05);
    }
    else
    {
        m_pradAllChannel->setChecked(true);
        m_pcmbVolyType->setCurrentIndex(0);
        m_pdspVoltAmpl->setValue(m_dLastVoltage);
    }
    onClickVoltageApply();
}

void ControlView::setSamplingRateSlot(int rate)
{
    if (m_bRunning2)
    {
        StopThread2();
        m_dSampleRate = rate;
        StartThread2();
    }
    else
    {
        m_dSampleRate = rate;
    }
}

void ControlView::SetAllChannelDCLevelSlot(double volt)
{
    m_pradAllChannel->setChecked(true);
    m_pcmbVolyType->setCurrentIndex(0);
    m_pdspVoltAmpl->setValue(volt);
    onClickVoltageApply();
}

void ControlView::SetShowChannelDCLevelSlot(double volt)
{
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        //if (!m_plistControlShow->item(i)->isHidden())
        if (m_vetControlListItem[i]->GetValidChannel())
        {
            SetOneChannelDCLevel(i, volt);
        }
    }
}

void ControlView::SetSelChannelDCLevelSlot(double volt)
{
    for (int i = m_iStartIndex; i < m_iEndIndex; ++i)
    {
        if (m_vetControlListItem[i]->GetCheckedSeq())
        {
            SetOneChannelDCLevel(i, volt);
        }
    }
}

void ControlView::SetDCLevelSelectedSlot(double minval, double maxval)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (m_vetControlListItem[i]->GetCheckedSeq())
        {
            const auto& stravg = m_vetControlListItem[i]->GetAvgCurrent();
            const auto& avg = ::atof(stravg.toStdString().c_str());
            bool chk = (avg >= minval && avg <= maxval);
            m_vetControlListItem[i]->SetChecked(chk);
            //m_vetControlListItem[i]->SetDegatingChecked(chk);
            m_vetControlListItem[i]->SetCheckedSeq(chk);
            m_vetControlListItem[i]->SetValidChannel(chk);
            if (!chk)
            {
                SetOneChannelDCLevel(i, 0.0);
            }
        }
    }
}

void ControlView::SetDCLevelBiggerCloseSlot(double val, double val2)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (m_vetControlListItem[i]->GetCheckedSeq() && !m_vetControlListItem[i]->GetIsDegating())
        {
            auto avg = m_vetControlListItem[i]->GetAvgCurrent().toFloat();
            if (avg > val2)
            {
                m_vetControlListItem[i]->SetSteadyPoreStatus(EEmbedPoreStatus::EPS_SATURATEDPORE);
            }
            else if (avg > val)
            {
                m_vetControlListItem[i]->SetSteadyPoreStatus(EEmbedPoreStatus::EPS_MULTIPORE);
            }
        }
    }
}

void ControlView::SetSinglePoreSelectedSlot(void)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        bool sing = (m_vetControlListItem[i]->GetHoleStatus() == EEmbedPoreStatus::EPS_SINGLEPORE);
        m_vetControlListItem[i]->SetChecked(sing);
        m_vetControlListItem[i]->SetCheckedSeq(sing);
        m_vetControlListItem[i]->ResetHoleStatus();
    }
}

void ControlView::SetDCLevelCapSelectedSlot(double mincap, double maxcap)
{
    float fCapValue[CHANNEL_NUM] = { 0.0 };
    emit getCapCalculationSignal(fCapValue);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (m_vetControlListItem[i]->GetCheckedSeq())
        {
            bool chk = (fCapValue[i] >= mincap) && (fCapValue[i] <= maxcap);
            m_vetControlListItem[i]->SetChecked(chk);
            m_vetControlListItem[i]->SetCheckedSeq(chk);
            m_vetControlListItem[i]->SetValidChannel(chk);
            if (!chk)
            {
                SetOneChannelDCLevel(i, 0.0);
            }
        }
        else
        {
            m_vetControlListItem[i]->SetChecked(false);
            m_vetControlListItem[i]->SetValidChannel(false);
            SetOneChannelDCLevel(i, 0.0);
        }
    }
}

void ControlView::SetDCLevelAllSlot(double minval, double maxval)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        const auto& avg = m_vetControlListItem[i]->GetAvgCurrent().toDouble();
        bool chk = (avg >= minval && avg <= maxval);
        m_vetControlListItem[i]->SetChecked(chk);
        //m_vetControlListItem[i]->SetDegatingChecked(chk);
        m_vetControlListItem[i]->SetCheckedSeq(chk);
        m_vetControlListItem[i]->SetValidChannel(chk);
        if (!chk)
        {
            SetOneChannelDCLevel(i, 0.0);
        }
    }
}

void ControlView::SetDCLevelStdAllSlot(double minval, double maxval, double minstd, double maxstd)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        const auto& avg = m_vetControlListItem[i]->GetAvgCurrent().toDouble();
        const auto& std = m_vetControlListItem[i]->GetStdCurrent().toDouble();
        bool chk = (avg >= minval && avg <= maxval && std >= minstd && std <= maxstd);
        m_vetControlListItem[i]->SetChecked(chk);
        //m_vetControlListItem[i]->SetDegatingChecked(chk);
        m_vetControlListItem[i]->SetCheckedSeq(chk);
        m_vetControlListItem[i]->SetValidChannel(chk);
        if (!chk)
        {
            SetOneChannelDCLevel(i, 0.0);
        }
    }
}

void ControlView::SetDCLevelStdSelectedSlot(double minval, double maxval, double minstd, double maxstd)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (m_vetControlListItem[i]->GetCheckedSeq())
        {
            const auto& avg = m_vetControlListItem[i]->GetAvgCurrent().toDouble();
            const auto& std = m_vetControlListItem[i]->GetStdCurrent().toDouble();
            bool chk = (avg >= minval && avg <= maxval && std >= minstd && std <= maxstd);
            m_vetControlListItem[i]->SetChecked(chk);
            //m_vetControlListItem[i]->SetDegatingChecked(chk);
            m_vetControlListItem[i]->SetCheckedSeq(chk);
            m_vetControlListItem[i]->SetValidChannel(chk);
            if (!chk)
            {
                SetOneChannelDCLevel(i, 0.0);
            }
        }
        else
        {
            m_vetControlListItem[i]->SetChecked(false);
            m_vetControlListItem[i]->SetValidChannel(false);
            SetOneChannelDCLevel(i, 0.0);
        }
    }
}

void ControlView::SetOnlyShowSelectedSlot(bool val)
{
    if (val)
    {
        if (m_pchkShowVisible->isChecked())
        {
            m_pchkShowVisible->setChecked(false);
        }
        m_pchkShowVisible->click();
    }
    else
    {
        if (!m_pchkShowVisible->isChecked())
        {
            m_pchkShowVisible->setChecked(true);
        }
        m_pchkShowVisible->click();
    }
}

void ControlView::SetClickZeroAdjustSlot(void)
{
    m_pbtnZeroAdjust->click();
}

void ControlView::SetEnableZeroAdjustSlot(bool val, bool& ret)
{
    ret = false;
    if (val)
    {
        if (!m_pbtnZeroAdjustStart->isChecked())
        {
            m_pbtnZeroAdjustStart->click();
            ret = true;
        }
    }
    else
    {
        if (m_pbtnZeroAdjustStart->isChecked())
        {
            m_pbtnZeroAdjustStart->click();
            ret = true;
        }
    }
}

void ControlView::SetEnableSingleHoleSlot(bool val, bool& ret)
{
    ret = false;
    if (val)
    {
        if (!m_pbtnEnableAuto->isChecked())
        {
            m_pbtnEnableAuto->click();
            ret = true;
        }
    }
    else
    {
        if (m_pbtnEnableAuto->isChecked())
        {
            m_pbtnEnableAuto->click();
            ret = true;
        }
    }
}

//void ControlView::SetAutoSingleHoleSlot(bool val, bool& ret)
//{
//    ret = false;
//    if (val)
//    {
//        if (!m_pbtnAutoZero->isChecked())
//        {
//            m_pbtnAutoZero->click();
//            ret = true;
//        }
//    }
//    else
//    {
//        if (m_pbtnAutoZero->isChecked())
//        {
//            m_pbtnAutoZero->click();
//            ret = true;
//        }
//    }
//}

void ControlView::SetEnableAutoDegatingSlot(bool val, bool& ret)
{
    ret = false;
    if (val)
    {
        if (!m_pbtnDegatingEnable->isChecked())
        {
            m_pbtnDegatingEnable->click();
            ret = true;
        }
    }
    else
    {
        if (m_pbtnDegatingEnable->isChecked())
        {
            m_pbtnDegatingEnable->click();
            ret = true;
        }
    }
}

//void ControlView::SetCheckAutoDegatingSlot(bool val, bool& ret)
//{
//    ret = false;
//    if (val)
//    {
//        if (!m_pchkDegatingAllCheck->isChecked())
//        {
//            m_pchkDegatingAllCheck->click();
//            ret = true;
//        }
//    }
//    else
//    {
//        if (m_pchkDegatingAllCheck->isChecked())
//        {
//            m_pchkDegatingAllCheck->click();
//            ret = true;
//        }
//    }
//}

void ControlView::SetSeqencingSelectSlot(double val, double minval, double maxval)
{
    //SetDCLevelSelectedSlot(minval, maxval);
    //SetOnlyShowSelectedSlot(true);
    bool selchan[CHANNEL_NUM] = { false };
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        //if (m_vetControlListItem[i]->GetValidChannel())
        if (!m_plistControlShow->item(i)->isHidden())
        {
            const auto& avg = m_vetControlListItem[i]->GetAvgCurrent().toDouble();
            bool chk = (avg >= minval && avg <= maxval);
            m_vetControlListItem[i]->SetChecked(chk);
            //m_vetControlListItem[i]->SetDegatingChecked(chk);
            m_vetControlListItem[i]->SetCheckedSeq(chk);
            if (!chk)
            {
                SetOneChannelDCLevel(i, 0.0);
            }
            //m_plistControlShow->item(i)->setHidden(!chk);
            //auto volt = m_vetControlListItem[i]->GetVoltAmpl();
            //if (!chk)
            //{
            //    if (!ISDOUBLEZERO(volt))
            //    {
            //        SetOneChannelDCLevel(i, 0.0);
            //    }
            //}
            //else
            //{
            //    if (!ISDOUBLEZERO(volt - val))
            //    {
            //        SetOneChannelDCLevel(i, val);
            //    }
            //}
            selchan[i] = chk;
        }
    }
    //if (ConfigServer::GetInstance()->GetSaveData())
    //{
    //    emit SetCloseChannelSignal(selchan);
    //}
}

void ControlView::SetCancelZeroAdjustSlot(void)
{
    m_pProgressDialog->cancel();
    onClickZeroAdjustReset();
    setZeroAdjustEndSlot();
    m_pProgressDialog->reset();
}

void ControlView::SetDCLevelStdShowSelectedSlot(double minval, double maxval, double minstd, double maxstd)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        //if (!m_plistControlShow->item(i)->isHidden())
        if (m_vetControlListItem[i]->GetValidChannel())
        {
            const auto& avg = m_vetControlListItem[i]->GetAvgCurrent().toDouble();
            const auto& std = m_vetControlListItem[i]->GetStdCurrent().toDouble();
            bool chk = (avg >= minval && avg <= maxval && std >= minstd && std <= maxstd);
            m_vetControlListItem[i]->SetChecked(chk);
            //m_vetControlListItem[i]->SetDegatingChecked(chk);
            m_vetControlListItem[i]->SetCheckedSeq(chk);
            //m_vetControlListItem[i]->SetValidChannel(chk);
            if (!chk)
            {
                //m_vetControlListItem[i]->SetHoleLastStatus();
                SetOneChannelDCLevel(i, 0.0);
            }
        }
    }
}

//void ControlView::SetChannelSaveDataSelectedSlot(void)
//{
//    bool selchan[CHANNEL_NUM] = { false };
//    for (int i = 0; i < CHANNEL_NUM; ++i)
//    {
//        if (!m_plistControlShow->item(i)->isHidden())
//        {
//            selchan[i] = m_vetControlListItem[i]->GetCheckedSeq();
//        }
//    }
//    if (ConfigServer::GetInstance()->GetSaveData())
//    {
//        emit SetCloseChannelSignal(selchan);
//    }
//}

void ControlView::checkChangedSlot(int chan, bool chk)
{
    emit checkChangedSignal(chan, chk);

    //bool ret = false;
    //emit GetNextContinueSignal(ret);
    //if (!ret)
    //{
    //    //m_vetControlListItem[chan]->SetDegatingChecked(chk);
    //    m_vetControlListItem[chan]->SetCheckedSeq(chk);
    //}
}

void ControlView::SetPoreFilterChannelSlot(double minval, double maxval)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto avg = m_vetControlListItem[i]->GetAvgCurrent().toFloat();
        bool chk = (avg >= minval && avg <= maxval);
        if (m_vetControlListItem[i]->GetCheckedSeq())
        {
            m_vetControlListItem[i]->SetPoreFilterChannel(chk);
        }
        if (avg > maxval)
        {
            m_vetControlListItem[i]->SetChecked(false);
            m_vetControlListItem[i]->SetCheckedSeq(false);
            SetOneChannelDCLevel(i, 0.0);
        }
    }
}

void ControlView::SetNotPoreFilterDCLevelSlot(double volt)
{
    //m_pradSingleChannel->setChecked(true);
    //m_pcmbVolyType->setCurrentIndex(0);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (m_vetControlListItem[i]->GetCheckedSeq() && !m_vetControlListItem[i]->GetPoreFilterChannel())
        {
            SetOneChannelDCLevel(i, volt);
        }
    }
}

void ControlView::SetPoreFilterDCLevelSlot(double volt)
{
    //m_pradSingleChannel->setChecked(true);
    //m_pcmbVolyType->setCurrentIndex(0);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (m_vetControlListItem[i]->GetCheckedSeq() && m_vetControlListItem[i]->GetPoreFilterChannel())
        {
            SetOneChannelDCLevel(i, volt);
        }
    }
}

void ControlView::UpdateListItemSlot(void)
{
    m_pchkShowSeq1->click();
    if (!m_pchkShowSeq1->isChecked())
    {
        m_pchkShowSeq1->click();
    }
}

void ControlView::EnablePoreInsertSlot(bool val)
{
    m_pbtnPoreInsertTimer->setEnabled(val);
}

void ControlView::setDegatingDataNameSlot(const QString& qsname)
{
    m_strDataName = qsname.toStdString();
}
