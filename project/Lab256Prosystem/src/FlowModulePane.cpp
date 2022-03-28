#include "FlowModulePane.h"
#include <qlayout.h>
#include <Log.h>

#include "ConfigServer.h"
#include "MyQMessageBox.h"


FlowModulePane::FlowModulePane(QWidget *parent) : QWidget(parent)
{
	InitCtrl();

	m_tmrChannleQC.setSingleShot(true);
	connect(&m_tmrChannleQC, &QTimer::timeout, this, &FlowModulePane::timerChannleQCSlot);

	m_tmrMakeMembrane.setSingleShot(true);
	connect(&m_tmrMakeMembrane, &QTimer::timeout, this, &FlowModulePane::timerMakeMembraneSlot);

	m_tmrPoreInsert.setSingleShot(true);
	connect(&m_tmrPoreInsert, &QTimer::timeout, this, &FlowModulePane::timerPoreInsertSlot);

	m_tmrPoreQC.setSingleShot(true);
	connect(&m_tmrPoreQC, &QTimer::timeout, this, &FlowModulePane::timerPoreQCSlot);

	m_tmrSeqencing.setSingleShot(true);
	connect(&m_tmrSeqencing, &QTimer::timeout, this, &FlowModulePane::timerSeqencingSlot);
	connect(&m_tmrSeqencingSelect, &QTimer::timeout, this, &FlowModulePane::timerSeqencingSelectSlot);

	m_tmrSeqencingQC.setSingleShot(true);
	connect(&m_tmrSeqencingQC, &QTimer::timeout, this, &FlowModulePane::timerSeqencingQCSlot);
	connect(&m_tmrSeqencingQCSelect, &QTimer::timeout, this, &FlowModulePane::timerSeqencingQCSelectSlot);


	OnEnableAllCtrl(false);
}

FlowModulePane::~FlowModulePane()
{
	if (m_tmrSeqencingSelect.isActive())
	{
		m_tmrSeqencingSelect.stop();
	}
	if (m_tmrSeqencingQCSelect.isActive())
	{
		m_tmrSeqencingQCSelect.stop();
	}
}

bool FlowModulePane::InitCtrl(void)
{
	m_ptabFlow = new QTabWidget(this);
	QWidget* pchanqc = new QWidget(this);
	QWidget* pmakemb = new QWidget(this);
	QWidget* pinsert = new QWidget(this);
	QWidget* pholeqc = new QWidget(this);
	QWidget* psequen = new QWidget(this);
	QWidget* psequqc = new QWidget(this);

	m_ptabFlow->addTab(pchanqc, "ChannelQC");
	m_ptabFlow->addTab(pmakemb, "MakeMembrane");
	m_ptabFlow->addTab(pinsert, "PoreInsert");
	m_ptabFlow->addTab(pholeqc, "PoreQC");
	m_ptabFlow->addTab(psequen, "Sequencing");
	m_ptabFlow->addTab(psequqc, "MuxScan");


	//const QString qstrStyleSheet = "min-height:15px;border:1px solid black";

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
		m_pdspDuration1->setMaximum(999999.99);
		m_pdspDuration1->setValue(5);


		m_plabCurrent1 = new QLabel("Current", pchanqc);

		m_pdspCurrentMin1 = new QDoubleSpinBox(pchanqc);
		m_pdspCurrentMin1->setDecimals(2);
		m_pdspCurrentMin1->setSuffix(" pA");
		m_pdspCurrentMin1->setMinimum(-999999.99);
		m_pdspCurrentMin1->setMaximum(999999.99);
		m_pdspCurrentMin1->setValue(1000);

		m_pdspCurrentMax1 = new QDoubleSpinBox(pchanqc);
		m_pdspCurrentMax1->setDecimals(2);
		m_pdspCurrentMax1->setSuffix(" pA");
		m_pdspCurrentMax1->setMinimum(-999999.99);
		m_pdspCurrentMax1->setMaximum(999999.99);
		m_pdspCurrentMax1->setValue(999999.99);


		m_plabStd1 = new QLabel("STD", pchanqc);

		m_pdspStdMin1 = new QDoubleSpinBox(pchanqc);
		m_pdspStdMin1->setDecimals(4);
		m_pdspStdMin1->setMinimum(-999.9999);
		m_pdspStdMin1->setMaximum(999.9999);
		m_pdspStdMin1->setValue(0.0001);

		m_pdspStdMax1 = new QDoubleSpinBox(pchanqc);
		m_pdspStdMax1->setDecimals(4);
		m_pdspStdMax1->setMinimum(-999.9999);
		m_pdspStdMax1->setMaximum(999.9999);
		m_pdspStdMax1->setValue(2.0);

		m_plabDuration12 = new QLabel("Duration2", pchanqc);

		m_pdspDuration12 = new QDoubleSpinBox(pchanqc);
		m_pdspDuration12->setDecimals(2);
		m_pdspDuration12->setSuffix(" s");
		m_pdspDuration12->setMaximum(999999.99);
		m_pdspDuration12->setValue(3);


		m_pbtnChannelQC = new QPushButton("Apply", pchanqc);


		QGridLayout* gdlayout = new QGridLayout();
		gdlayout->addWidget(m_plabVolt1, 0, 0);
		gdlayout->addWidget(m_pdspVolt1, 0, 1);
		gdlayout->addWidget(m_plabDuration1, 0, 3);
		gdlayout->addWidget(m_pdspDuration1, 0, 4);

		gdlayout->addWidget(m_plabCurrent1, 1, 0);
		gdlayout->addWidget(m_pdspCurrentMin1, 1, 1);
		gdlayout->addWidget(m_pdspCurrentMax1, 1, 2);

		gdlayout->addWidget(m_plabStd1, 2, 0);
		gdlayout->addWidget(m_pdspStdMin1, 2, 1);
		gdlayout->addWidget(m_pdspStdMax1, 2, 2);
		gdlayout->addWidget(m_plabDuration12, 2, 3);
		gdlayout->addWidget(m_pdspDuration12, 2, 4);

		gdlayout->addWidget(m_pbtnChannelQC, 3, 4);

		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addLayout(gdlayout);
		horlayout1->addStretch(1);

		pchanqc->setLayout(horlayout1);
	}

	//MakeMembrane
	{
		m_plabVolt2 = new QLabel("Volt1", pmakemb);

		m_pdspVolt2 = new QDoubleSpinBox(pmakemb);
		m_pdspVolt2->setDecimals(2);
		m_pdspVolt2->setSuffix(" V");
		m_pdspVolt2->setMinimum(-999.99);
		m_pdspVolt2->setMaximum(999.99);
		m_pdspVolt2->setSingleStep(0.01);
		m_pdspVolt2->setValue(0.18);

		m_plabDuration2 = new QLabel("Duration1", pmakemb);

		m_pdspDuration2 = new QDoubleSpinBox(pmakemb);
		m_pdspDuration2->setDecimals(2);
		m_pdspDuration2->setSuffix(" s");
		m_pdspDuration2->setMaximum(999999.99);
		m_pdspDuration2->setValue(5);

		m_plabWaitingZeroAdjust2 = new QLabel("Waiting For ZeroAdjust", pholeqc);
		m_plabWaitingZeroAdjust2->setFrameShape(QFrame::Box);
		//m_pchkWaitingZeroAdjust2 = new QCheckBox("Waiting For ZeroAdjust", pholeqc);
		//m_pchkWaitingZeroAdjust2->setStyleSheet(qstrStyleSheet);
		//m_pchkWaitingZeroAdjust2->setChecked(true);


		m_plabCurrent2 = new QLabel("Current", pmakemb);

		m_pdspCurrentMin2 = new QDoubleSpinBox(pmakemb);
		m_pdspCurrentMin2->setDecimals(2);
		m_pdspCurrentMin2->setSuffix(" pA");
		m_pdspCurrentMin2->setMinimum(-999999.99);
		m_pdspCurrentMin2->setMaximum(999999.99);
		m_pdspCurrentMin2->setValue(-999999.99);

		m_pdspCurrentMax2 = new QDoubleSpinBox(pmakemb);
		m_pdspCurrentMax2->setDecimals(2);
		m_pdspCurrentMax2->setSuffix(" pA");
		m_pdspCurrentMax2->setMinimum(-999999.99);
		m_pdspCurrentMax2->setMaximum(999999.99);
		m_pdspCurrentMax2->setValue(20.0);


		m_plabStd2 = new QLabel("STD", pmakemb);

		m_pdspStdMin2 = new QDoubleSpinBox(pmakemb);
		m_pdspStdMin2->setDecimals(4);
		m_pdspStdMin2->setMinimum(-999.9999);
		m_pdspStdMin2->setMaximum(999.9999);
		m_pdspStdMin2->setValue(0.0001);

		m_pdspStdMax2 = new QDoubleSpinBox(pmakemb);
		m_pdspStdMax2->setDecimals(4);
		m_pdspStdMax2->setMinimum(-999.9999);
		m_pdspStdMax2->setMaximum(999.9999);
		m_pdspStdMax2->setValue(2.0);


		m_pbtnMakeMembrane = new QPushButton("Apply", pmakemb);


		QGridLayout* gdlayout1 = new QGridLayout();
		gdlayout1->addWidget(m_plabVolt2, 0, 0);
		gdlayout1->addWidget(m_pdspVolt2, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
		gdlayout1->addWidget(m_plabDuration2, 0, 2, Qt::AlignVCenter | Qt::AlignRight);
		gdlayout1->addWidget(m_pdspDuration2, 0, 3);

		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addSpacing(20);
		horlayout1->addWidget(m_plabWaitingZeroAdjust2);
		//horlayout1->addWidget(m_pchkWaitingZeroAdjust2);
		horlayout1->addStretch(1);

		QGridLayout* gdlayout2 = new QGridLayout();
		gdlayout2->addWidget(m_plabCurrent2, 0, 0);
		gdlayout2->addWidget(m_pdspCurrentMin2, 0, 1);
		gdlayout2->addWidget(m_pdspCurrentMax2, 0, 2);
		gdlayout2->addWidget(m_plabStd2, 1, 0);
		gdlayout2->addWidget(m_pdspStdMin2, 1, 1);
		gdlayout2->addWidget(m_pdspStdMax2, 1, 2);
		gdlayout2->addWidget(m_pbtnMakeMembrane, 1, 3);


		QVBoxLayout* verlayout9 = new QVBoxLayout();
		verlayout9->addLayout(gdlayout1);
		verlayout9->addLayout(horlayout1);
		verlayout9->addLayout(gdlayout2);

		QHBoxLayout* horlayout9 = new QHBoxLayout();
		horlayout9->addLayout(verlayout9);
		horlayout9->addStretch(1);

		pmakemb->setLayout(horlayout9);
	}

	//PoreInsert
	{
		m_plabVolt3 = new QLabel("Volt1", pinsert);

		m_pdspVolt3 = new QDoubleSpinBox(pinsert);
		m_pdspVolt3->setDecimals(2);
		m_pdspVolt3->setSuffix(" V");
		m_pdspVolt3->setSingleStep(0.01);
		m_pdspVolt3->setMinimum(-999.99);
		m_pdspVolt3->setMaximum(999.99);
		m_pdspVolt3->setValue(0.18);

		m_plabDuration3 = new QLabel("Duration1", pinsert);

		m_pdspDuration3 = new QDoubleSpinBox(pinsert);
		m_pdspDuration3->setDecimals(2);
		m_pdspDuration3->setSuffix(" s");
		m_pdspDuration3->setMaximum(999999.99);
		m_pdspDuration3->setValue(3);


		m_plabStartSinglePoreQC3 = new QLabel("Start Auto Single Pore Protect", pinsert);
		m_plabStartSinglePoreQC3->setFrameShape(QFrame::Box);
		//m_pchkStartSinglePoreQC3 = new QCheckBox("Start Single Pore QC & Auto Protect", pinsert);
		//m_pchkStartSinglePoreQC3->setStyleSheet(qstrStyleSheet);
		//m_pchkStartSinglePoreQC3->setChecked(true);

		m_plabDuration32 = new QLabel("Duration2", pmakemb);

		m_pdspDuration32 = new QDoubleSpinBox(pmakemb);
		m_pdspDuration32->setDecimals(2);
		m_pdspDuration32->setSuffix(" s");
		m_pdspDuration32->setMaximum(999999.99);
		m_pdspDuration32->setValue(5);


		m_plabCurrent3 = new QLabel("Current", pmakemb);

		m_pdspCurrentMin3 = new QDoubleSpinBox(pmakemb);
		m_pdspCurrentMin3->setDecimals(2);
		m_pdspCurrentMin3->setSuffix(" pA");
		m_pdspCurrentMin3->setMinimum(-999999.99);
		m_pdspCurrentMin3->setMaximum(999999.99);
		m_pdspCurrentMin3->setValue(-999999.99);

		m_pdspCurrentMax3 = new QDoubleSpinBox(pmakemb);
		m_pdspCurrentMax3->setDecimals(2);
		m_pdspCurrentMax3->setSuffix(" pA");
		m_pdspCurrentMax3->setMinimum(-999999.99);
		m_pdspCurrentMax3->setMaximum(999999.99);
		m_pdspCurrentMax3->setValue(1000.0);


		m_pbtnPoreInsert = new QPushButton("Apply", pinsert);


		QGridLayout* gdlayout1 = new QGridLayout();
		gdlayout1->addWidget(m_plabVolt3, 0, 0);
		gdlayout1->addWidget(m_pdspVolt3, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
		gdlayout1->addWidget(m_plabDuration3, 0, 3, Qt::AlignVCenter | Qt::AlignRight);
		gdlayout1->addWidget(m_pdspDuration3, 0, 4);

		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addWidget(m_plabStartSinglePoreQC3);
		//horlayout1->addWidget(m_pchkStartSinglePoreQC3);
		horlayout1->addStretch(1);
		horlayout1->addWidget(m_plabDuration32);
		horlayout1->addWidget(m_pdspDuration32);

		QGridLayout* gdlayout2 = new QGridLayout();
		gdlayout2->addWidget(m_plabCurrent3, 0, 0);
		gdlayout2->addWidget(m_pdspCurrentMin3, 0, 1);
		gdlayout2->addWidget(m_pdspCurrentMax3, 0, 2);
		gdlayout2->addWidget(m_pbtnPoreInsert, 1, 4);

		QVBoxLayout* verlayout9 = new QVBoxLayout();
		verlayout9->addLayout(gdlayout1);
		verlayout9->addLayout(horlayout1);
		verlayout9->addLayout(gdlayout2);

		QHBoxLayout* horlayout9 = new QHBoxLayout();
		horlayout9->addLayout(verlayout9);
		horlayout9->addStretch(1);

		pinsert->setLayout(horlayout9);
	}

	//PoreQC
	{
		//m_plabStopSinglePore = new QLabel("Stop Single Pore QC.", pholeqc);
		//m_plabStopSinglePore->setFrameShape(QFrame::Box);
		m_plabWaitingZeroAdjust4 = new QLabel("Waiting For ZeroAdjust", pholeqc);
		m_plabWaitingZeroAdjust4->setFrameShape(QFrame::Box);
		//m_pchkWaitingZeroAdjust4 = new QCheckBox("Waiting For ZeroAdjust", pholeqc);
		//m_pchkWaitingZeroAdjust4->setStyleSheet(qstrStyleSheet);
		//m_pchkWaitingZeroAdjust4->setChecked(true);

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
		m_pdspDuration4->setMaximum(999999.99);
		m_pdspDuration4->setValue(1);

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
		m_pdspDuration42->setMaximum(999999.99);
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
		m_pdspDuration43->setMaximum(999999.99);
		m_pdspDuration43->setValue(5);

		m_plabCurrent4 = new QLabel("Current", pholeqc);

		m_pdspCurrentMin4 = new QDoubleSpinBox(pholeqc);
		m_pdspCurrentMin4->setDecimals(2);
		m_pdspCurrentMin4->setSuffix(" pA");
		m_pdspCurrentMin4->setMinimum(-999999.99);
		m_pdspCurrentMin4->setMaximum(999999.99);
		m_pdspCurrentMin4->setValue(1000);

		m_pdspCurrentMax4 = new QDoubleSpinBox(pholeqc);
		m_pdspCurrentMax4->setDecimals(2);
		m_pdspCurrentMax4->setSuffix(" pA");
		m_pdspCurrentMax4->setMinimum(-999999.99);
		m_pdspCurrentMax4->setMaximum(999999.99);
		m_pdspCurrentMax4->setValue(999999.99);

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
		m_pdspStdMax4->setValue(2.0);


		m_pbtnPoreQC = new QPushButton("Apply", pholeqc);


		QHBoxLayout* horlayout1 = new QHBoxLayout();
		//horlayout1->addWidget(m_plabStopSinglePore);
		//horlayout1->addSpacing(20);
		horlayout1->addWidget(m_plabWaitingZeroAdjust4);
		//horlayout1->addWidget(m_pchkWaitingZeroAdjust4);
		horlayout1->addStretch(1);

		int item = 0;
		QGridLayout* gdlayout = new QGridLayout();

		gdlayout->addWidget(m_plabVolt4, item, 0);
		gdlayout->addWidget(m_pdspVolt4, item, 1);
		gdlayout->addWidget(m_plabDuration4, item, 2);
		gdlayout->addWidget(m_pdspDuration4, item, 3);

		gdlayout->addWidget(m_plabCurrent4, item, 5);
		gdlayout->addWidget(m_pdspCurrentMin4, item, 6);
		gdlayout->addWidget(m_pdspCurrentMax4, item, 7);
		++item;

		gdlayout->addWidget(m_plabVolt42, item, 0);
		gdlayout->addWidget(m_pdspVolt42, item, 1);
		gdlayout->addWidget(m_plabDuration42, item, 2);
		gdlayout->addWidget(m_pdspDuration42, item, 3);

		gdlayout->addWidget(m_plabStd4, item, 5);
		gdlayout->addWidget(m_pdspStdMin4, item, 6);
		gdlayout->addWidget(m_pdspStdMax4, item, 7);
		++item;

		gdlayout->addWidget(m_plabVolt43, item, 0);
		gdlayout->addWidget(m_pdspVolt43, item, 1);
		gdlayout->addWidget(m_plabDuration43, item, 2);
		gdlayout->addWidget(m_pdspDuration43, item, 3);

		gdlayout->addWidget(m_pbtnPoreQC, item, 4);

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

		m_plabDuration5 = new QLabel("Duration1", psequen);

		m_pdspDuration5 = new QDoubleSpinBox(psequen);
		m_pdspDuration5->setDecimals(2);
		m_pdspDuration5->setSuffix(" s");
		m_pdspDuration5->setMaximum(999999.99);
		m_pdspDuration5->setValue(3);

		m_plabStartAutoDegating5 = new QLabel("Start Auto Degating", psequen);
		m_plabStartAutoDegating5->setFrameShape(QFrame::Box);
		m_plabStartAutoZeroAdjust5 = new QLabel("Start Auto ZeroAdjust", psequen);
		m_plabStartAutoZeroAdjust5->setFrameShape(QFrame::Box);
		//m_pchkStartAutoDegating5 = new QCheckBox("Start Auto Degating", psequen);
		//m_pchkStartAutoDegating5->setStyleSheet(qstrStyleSheet);
		//m_pchkStartAutoDegating5->setChecked(true);
		//m_pchkStartAutoZeroAdjust5 = new QCheckBox("Start Auto ZeroAdjust", psequen);
		//m_pchkStartAutoZeroAdjust5->setStyleSheet(qstrStyleSheet);
		//m_pchkStartAutoZeroAdjust5->setChecked(true);

		m_plabTimerQC5 = new QLabel("TimerQC Duration2", psequen);
		m_pdspTimerQC5 = new QDoubleSpinBox(psequen);
		m_pdspTimerQC5->setDecimals(1);
		m_pdspTimerQC5->setSuffix(" min");
		m_pdspTimerQC5->setMinimum(0.1);
		m_pdspTimerQC5->setMaximum(1439.9);//23:59:54//600.0
		m_pdspTimerQC5->setValue(2.0);

		m_plcdTimerQC5 = new QLCDNumber(8, psequen);
		m_plcdTimerQC5->setSegmentStyle(QLCDNumber::Flat);
		m_plcdTimerQC5->setFrameShape(QFrame::StyledPanel);

		SetTimehms5();

		m_plabCurrent5 = new QLabel("Current", psequen);

		m_pdspCurrentMin5 = new QDoubleSpinBox(psequen);
		m_pdspCurrentMin5->setDecimals(2);
		m_pdspCurrentMin5->setSuffix(" pA");
		m_pdspCurrentMin5->setMinimum(-999999.99);
		m_pdspCurrentMin5->setMaximum(999999.99);
		m_pdspCurrentMin5->setValue(20.0);

		m_pdspCurrentMax5 = new QDoubleSpinBox(psequen);
		m_pdspCurrentMax5->setDecimals(2);
		m_pdspCurrentMax5->setSuffix(" pA");
		m_pdspCurrentMax5->setMinimum(-999999.99);
		m_pdspCurrentMax5->setMaximum(999999.99);
		m_pdspCurrentMax5->setValue(1000.0);

		m_pbtnSequencing = new QPushButton("Start", psequen);
		m_pbtnSequencing->setCheckable(true);


		QGridLayout* gdlayout = new QGridLayout();
		gdlayout->addWidget(m_plabVolt5, 0, 0);
		gdlayout->addWidget(m_pdspVolt5, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
		gdlayout->addWidget(m_plabDuration5, 0, 2, Qt::AlignVCenter | Qt::AlignRight);
		gdlayout->addWidget(m_pdspDuration5, 0, 3);

		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addWidget(m_plabStartAutoDegating5);
		//horlayout1->addWidget(m_pchkStartAutoDegating5);
		horlayout1->addSpacing(20);
		horlayout1->addWidget(m_plabStartAutoZeroAdjust5);
		//horlayout1->addWidget(m_pchkStartAutoZeroAdjust5);
		horlayout1->addStretch(1);

		QGridLayout* gdlayout2 = new QGridLayout();
		gdlayout2->addWidget(m_plabTimerQC5, 0, 0);
		gdlayout2->addWidget(m_pdspTimerQC5, 0, 1);
		gdlayout2->addWidget(m_plcdTimerQC5, 0, 2);
		gdlayout2->addWidget(m_plabCurrent5, 1, 0);
		gdlayout2->addWidget(m_pdspCurrentMin5, 1, 1);
		gdlayout2->addWidget(m_pdspCurrentMax5, 1, 2);

		gdlayout2->addWidget(m_pbtnSequencing, 1, 3);


		QVBoxLayout* verlayout9 = new QVBoxLayout();
		verlayout9->addLayout(gdlayout);
		verlayout9->addLayout(horlayout1);
		verlayout9->addLayout(gdlayout2);

		QHBoxLayout* horlayout9 = new QHBoxLayout();
		horlayout9->addLayout(verlayout9);
		horlayout9->addStretch(1);

		psequen->setLayout(horlayout9);
	}

	//MuxScan
	{
		m_pgrpQC = new QGroupBox(psequqc);
		m_pgrpQC->setContentsMargins(5, 1, 5, 1);

		m_plabWaitingZeroAdjust6 = new QLabel("Waiting For ZeroAdjust", m_pgrpQC);
		m_plabWaitingZeroAdjust6->setFrameShape(QFrame::Box);
		//m_pchkWaitingZeroAdjust6 = new QCheckBox("Waiting For ZeroAdjust", m_pgrpQC);
		//m_pchkWaitingZeroAdjust6->setStyleSheet(qstrStyleSheet);
		//m_pchkWaitingZeroAdjust6->setChecked(true);

		m_plabVolt6 = new QLabel("Volt1", m_pgrpQC);
		m_pdspVolt6 = new QDoubleSpinBox(m_pgrpQC);
		m_pdspVolt6->setDecimals(2);
		m_pdspVolt6->setSuffix(" V");
		m_pdspVolt6->setMinimum(-999.99);
		m_pdspVolt6->setMaximum(999.99);
		m_pdspVolt6->setValue(0.02);

		m_plabDuration6 = new QLabel("Duration1", m_pgrpQC);
		m_pdspDuration6 = new QDoubleSpinBox(m_pgrpQC);
		m_pdspDuration6->setDecimals(2);
		m_pdspDuration6->setSuffix(" s");
		//m_pdspDuration6->setSingleStep(0.1);
		m_pdspDuration6->setMaximum(999999.99);
		m_pdspDuration6->setValue(6);

		m_plabCurrent6 = new QLabel("Current", m_pgrpQC);

		m_pdspCurrentMin6 = new QDoubleSpinBox(m_pgrpQC);
		m_pdspCurrentMin6->setDecimals(2);
		m_pdspCurrentMin6->setSuffix(" pA");
		m_pdspCurrentMin6->setMinimum(-999999.99);
		m_pdspCurrentMin6->setMaximum(999999.99);
		m_pdspCurrentMin6->setValue(10.0);

		m_pdspCurrentMax6 = new QDoubleSpinBox(m_pgrpQC);
		m_pdspCurrentMax6->setDecimals(2);
		m_pdspCurrentMax6->setSuffix(" pA");
		m_pdspCurrentMax6->setMinimum(-999999.99);
		m_pdspCurrentMax6->setMaximum(999999.99);
		m_pdspCurrentMax6->setValue(1000.0);

		m_plabStd6 = new QLabel("STD", m_pgrpQC);

		m_pdspStdMin6 = new QDoubleSpinBox(m_pgrpQC);
		m_pdspStdMin6->setDecimals(4);
		m_pdspStdMin6->setMinimum(-999.9999);
		m_pdspStdMin6->setMaximum(999.9999);
		m_pdspStdMin6->setValue(0.0001);

		m_pdspStdMax6 = new QDoubleSpinBox(m_pgrpQC);
		m_pdspStdMax6->setDecimals(4);
		m_pdspStdMax6->setMinimum(-999.9999);
		m_pdspStdMax6->setMaximum(999.9999);
		m_pdspStdMax6->setValue(2.0);

		//m_pchkSaveDataStatus6 = new QCheckBox("DataStatus", m_pgrpQC);
		//m_pchkSaveDataStatus6->setChecked(true);
		//m_pchkSaveDataStatus6->setVisible(false);

		m_pbtnNextStep = new QPushButton("Next", m_pgrpQC);


		m_pgrpSequencing = new QGroupBox(psequqc);
		m_pgrpSequencing->setContentsMargins(5, 1, 5, 1);

		m_plabVolt62 = new QLabel("Volt2", m_pgrpSequencing);
		m_pdspVolt62 = new QDoubleSpinBox(m_pgrpSequencing);
		m_pdspVolt62->setDecimals(2);
		m_pdspVolt62->setSuffix(" V");
		m_pdspVolt62->setMinimum(-999.99);
		m_pdspVolt62->setMaximum(999.99);
		m_pdspVolt62->setSingleStep(0.01);
		m_pdspVolt62->setValue(0.0);

		m_plabDuration62 = new QLabel("Duration2", m_pgrpSequencing);
		m_pdspDuration62 = new QDoubleSpinBox(m_pgrpSequencing);
		m_pdspDuration62->setDecimals(2);
		m_pdspDuration62->setSuffix(" s");
		m_pdspDuration62->setMaximum(999999.99);
		m_pdspDuration62->setValue(5);

		m_plabVolt63 = new QLabel("Volt3", m_pgrpSequencing);
		m_pdspVolt63 = new QDoubleSpinBox(m_pgrpSequencing);
		m_pdspVolt63->setDecimals(2);
		m_pdspVolt63->setSuffix(" V");
		m_pdspVolt63->setMinimum(-999.99);
		m_pdspVolt63->setMaximum(999.99);
		m_pdspVolt63->setSingleStep(0.01);
		m_pdspVolt63->setValue(0.18);

		m_plabDuration63 = new QLabel("Duration3", m_pgrpSequencing);
		m_pdspDuration63 = new QDoubleSpinBox(m_pgrpSequencing);
		m_pdspDuration63->setDecimals(2);
		m_pdspDuration63->setSuffix(" s");
		m_pdspDuration63->setMaximum(999999.99);
		m_pdspDuration63->setValue(3);

		m_plabAutoProtectDegatingZeroAdjust6 = new QLabel("Start Auto Protect Degating ZeroAdjust", m_pgrpSequencing);
		m_plabAutoProtectDegatingZeroAdjust6->setFrameShape(QFrame::Box);
		//m_pchkStartAutoProtectDegatingZeroAdjust6 = new QCheckBox("Start Auto Protect Degating ZeroAdjust", m_pgrpSequencing);
		//m_pchkStartAutoProtectDegatingZeroAdjust6->setStyleSheet(qstrStyleSheet);
		//m_pchkStartAutoProtectDegatingZeroAdjust6->setChecked(true);

		m_plabCycle6 = new QLabel("Cycle", m_pgrpSequencing);
		m_pispCycle6 = new QSpinBox(m_pgrpSequencing);
		m_pispCycle6->setMinimum(1);
		m_pispCycle6->setMaximum(999999);
		m_pispCycle6->setValue(10);

		m_plabTimerPeriod6 = new QLabel("Timer Period", m_pgrpSequencing);
		m_pdspTimerPeriod6 = new QDoubleSpinBox(m_pgrpSequencing);
		m_pdspTimerPeriod6->setDecimals(1);
		m_pdspTimerPeriod6->setSuffix(" min");
		m_pdspTimerPeriod6->setMinimum(0.1);
		m_pdspTimerPeriod6->setMaximum(1439.9);//23:59:54//600.0
		m_pdspTimerPeriod6->setValue(125);

		m_plcdTimerPeriod6 = new QLCDNumber(8, m_pgrpSequencing);
		m_plcdTimerPeriod6->setSegmentStyle(QLCDNumber::Flat);
		m_plcdTimerPeriod6->setFrameShape(QFrame::StyledPanel);

		SetTimehms6();

		m_pbtnSequencingQC = new QPushButton("Start", m_pgrpSequencing);
		m_pbtnSequencingQC->setCheckable(true);


		QHBoxLayout* horlayout1 = new QHBoxLayout();
		horlayout1->addWidget(m_plabWaitingZeroAdjust6);
		//horlayout1->addWidget(m_pchkWaitingZeroAdjust6);
		//horlayout1->addSpacing(20);
		//horlayout1->addWidget(m_pchkSaveDataStatus6);
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


		QVBoxLayout* verlayout5 = new QVBoxLayout();
		verlayout5->addLayout(horlayout1);
		verlayout5->addLayout(gdlayout1);
		m_pgrpQC->setLayout(verlayout5);


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

		QHBoxLayout* horlayout6 = new QHBoxLayout();
		horlayout6->addWidget(m_plabAutoProtectDegatingZeroAdjust6);
		//horlayout6->addWidget(m_pchkStartAutoProtectDegatingZeroAdjust6);
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
		verlayout6->addLayout(gdlayout6);
		verlayout6->addLayout(horlayout6);
		verlayout6->addLayout(horlayout7);

		m_pgrpSequencing->setLayout(verlayout6);


		QHBoxLayout* horlayout9 = new QHBoxLayout();
		horlayout9->setContentsMargins(5, 1, 5, 1);
		horlayout9->addWidget(m_pgrpQC);
		horlayout9->addWidget(m_pgrpSequencing);
		horlayout9->addStretch(1);

		psequqc->setLayout(horlayout9);
	}

	QHBoxLayout* horlayout = new QHBoxLayout();
	horlayout->addWidget(m_ptabFlow);

	QVBoxLayout* verlayout = new QVBoxLayout();
	verlayout->addLayout(horlayout);

	setLayout(verlayout);


	m_pbtnChannelQC->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>施加电压0.18V，通过电流阈值或者范围判断故障通道并关断和隐藏故障通道。后电压回到0V.</p></body></html>"));
	m_pbtnMakeMembrane->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>成膜后，对未成膜的通道进行识别、关断、隐藏。</p></body></html>"));
	m_pbtnPoreInsert->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Lipid 膜： 施加电压0.18V，电流在单孔电流范围内（用户可输入值）的通道自动改施加50 mV，其余不变，知道插孔完成。</p></body></html>"));
	m_pbtnPoreQC->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Lipid 膜：首先进行调0操作。然后施加0.18V 1秒，施加-0.18V 0.2秒，施加电压0.18V，电流在单孔电流范围内（用户可输入值）的通道保留显示，定以为HP通道；其余通道关断隐藏。</p></body></html>"));
	m_pdspTimerQC5->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>周期最小为0.1min, 最大为1439.9min</p></body></html>"));
	m_pbtnSequencing->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>Lipid 膜：	0.18V 测序。 开启Degating功能，开启自动调零功能，开启定时短路检测功能。</p></body></html>"));
	m_pbtnNextStep->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>人工添加完通道，开始执行下一流程</p></body></html>"));
	m_pispCycle6->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>循环次数</p></body></html>"));
	m_pdspTimerPeriod6->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>周期最小为0.1min, 最大为1439.9min</p></body></html>"));
	m_pbtnSequencingQC->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>周期性执行QC检测功能和测序直到循环次数达到Cycle数。</p></body></html>"));

	connect(m_pbtnChannelQC, &QPushButton::clicked, this, &FlowModulePane::OnClickChannelQC);
	connect(m_pbtnMakeMembrane, &QPushButton::clicked, this, &FlowModulePane::OnClickMakeMembrane);
	connect(m_pbtnPoreInsert, &QPushButton::clicked, this, &FlowModulePane::OnClickPoreInsert);
	connect(m_pbtnPoreQC, &QPushButton::clicked, this, &FlowModulePane::OnClickPoreQC);
	connect(m_pbtnNextStep, &QPushButton::clicked, this, &FlowModulePane::OnClickNextStep);
	connect(m_pbtnSequencing, &QPushButton::clicked, this, &FlowModulePane::OnClickSeqencing);
	connect(m_pbtnSequencingQC, &QPushButton::clicked, this, &FlowModulePane::OnClickSeqencingQC);

	return false;
}

bool FlowModulePane::SetTimehms5(void)
{
	int itm = m_pdspTimerQC5->value() * 60;
	if (itm > 0)
	{
		int ihour = itm / 3600;
		int irest = itm % 3600;
		int imin = irest / 60;
		int isec = irest % 60;

		m_tmSeqencingSelect.setHMS(ihour, imin, isec, 0);
		m_plcdTimerQC5->display(m_tmSeqencingSelect.toString("hh:mm:ss"));

		return true;
	}
	return false;
}

bool FlowModulePane::SetTimehms6(void)
{
	int itm = m_pdspTimerPeriod6->value() * 60;
	if (itm > 0)
	{
		int ihour = itm / 3600;
		int irest = itm % 3600;
		int imin = irest / 60;
		int isec = irest % 60;

		m_tmSeqencingQC.setHMS(ihour, imin, isec, 0);
		m_plcdTimerPeriod6->display(m_tmSeqencingQC.toString("hh:mm:ss"));

		return true;
	}
	return false;
}

void FlowModulePane::DoSequencingFirst(void)
{
	//if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
	//{
	//	emit SetCancelZeroAdjustSignal();
	//	ConfigServer::caliSleep(2000);
	//}
	if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
	{
		emit SetSelChannelDCLevelSignal(m_pdspVolt62->value());

		//if (m_pchkSaveDataStatus6->isChecked())
		{
			emit SetDataSaveSignal(true);
		}
		ConfigServer::caliSleep(m_pdspDuration62->value() * 1000);

		emit SetSelChannelDCLevelSignal(m_pdspVolt63->value());
		ConfigServer::caliSleep(m_pdspDuration63->value() * 1000);

		//if (m_pchkSaveDataStatus6->isChecked())
		{
			emit SetChannelStatusSignal(true);
		}
	}
	else
	{
		//if (m_pchkSaveDataStatus6->isChecked())
		{
			emit SetDataSaveSignal(true);
			emit SetChannelStatusSignal(true);
		}
	}

	//if (m_pchkStartAutoProtectDegatingZeroAdjust6->isChecked())
	{
		bool bret = false;
		emit SetEnableSingleHoleSignal(true, bret);
		//ConfigServer::caliSleep(1000);
		//emit SetAutoSingleHoleSignal(true, bret);
		ConfigServer::caliSleep(1000);
		emit SetEnableAutoDegatingSignal(true, bret);
		ConfigServer::caliSleep(1000);
		emit SetEnableZeroAdjustSignal(true, bret);
		ConfigServer::caliSleep(1000);
	}

	if (!m_tmrSeqencingQCSelect.isActive())
	{
		m_tmrSeqencingQCSelect.start(1000);
	}

	m_pbtnSequencingQC->setEnabled(true);
}

void FlowModulePane::DoSequencingLoop(void)
{
	//if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
	//{
	//	emit SetCancelZeroAdjustSignal();
	//	ConfigServer::caliSleep(2000);
	//}
	if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
	{
		emit SetSelChannelDCLevelSignal(m_pdspVolt62->value());
		//ConfigServer::GetInstance()->SetSaveData(true);
		//emit SetChannelSaveDataSelectedSignal();

		//if (m_pchkSaveDataStatus6->isChecked())
		{
			emit SetDataSaveSignal(true);
		}
		ConfigServer::caliSleep(m_pdspDuration62->value() * 1000);

		emit SetSelChannelDCLevelSignal(m_pdspVolt63->value());
		ConfigServer::caliSleep(m_pdspDuration63->value() * 1000);
	}
	else
	{
		//ConfigServer::GetInstance()->SetSaveData(true);
		//emit SetChannelSaveDataSelectedSignal();
		//ConfigServer::caliSleep(2000);

		//if (m_pchkSaveDataStatus6->isChecked())
		{
			emit SetDataSaveSignal(true);
		}
	}

	if (m_pbtnSequencingQC->isChecked())
	{
		//if (m_pchkStartAutoProtectDegatingZeroAdjust6->isChecked())
		{
			bool bret = false;
			emit SetEnableSingleHoleSignal(true, bret);
			//ConfigServer::caliSleep(1000);
			//emit SetAutoSingleHoleSignal(true, bret);
			ConfigServer::caliSleep(1000);
			emit SetEnableAutoDegatingSignal(true, bret);
			ConfigServer::caliSleep(1000);
			emit SetEnableZeroAdjustSignal(true, bret);
			ConfigServer::caliSleep(1000);
		}
	}
}

void FlowModulePane::OnEnableAllCtrl(bool chk)
{
	//ChannelQC
	m_pdspVolt1->setEnabled(chk);
	m_pdspDuration1->setEnabled(chk);
	m_pdspCurrentMin1->setEnabled(chk);
	m_pdspCurrentMax1->setEnabled(chk);
	m_pdspStdMin1->setEnabled(chk);
	m_pdspStdMax1->setEnabled(chk);
	m_pdspDuration12->setEnabled(chk);

	//MakeMembrane
	m_pdspVolt2->setEnabled(chk);
	m_pdspDuration2->setEnabled(chk);
	//m_pchkWaitingZeroAdjust2->setEnabled(chk);
	m_pdspCurrentMin2->setEnabled(chk);
	m_pdspCurrentMax2->setEnabled(chk);
	m_pdspStdMin2->setEnabled(chk);
	m_pdspStdMax2->setEnabled(chk);

	//PoreInsert
	m_pdspVolt3->setEnabled(chk);
	m_pdspDuration3->setEnabled(chk);
	//m_pchkStartSinglePoreQC3->setEnabled(chk);
	m_pdspCurrentMin3->setEnabled(chk);
	m_pdspCurrentMax3->setEnabled(chk);
	m_pdspDuration32->setEnabled(chk);

	//PoreQC
	//m_pchkWaitingZeroAdjust4->setEnabled(chk);
	m_pdspVolt4->setEnabled(chk);
	m_pdspDuration4->setEnabled(chk);
	m_pdspVolt42->setEnabled(chk);
	m_pdspDuration42->setEnabled(chk);
	m_pdspVolt43->setEnabled(chk);
	m_pdspDuration43->setEnabled(chk);
	m_pdspCurrentMin4->setEnabled(chk);
	m_pdspCurrentMax4->setEnabled(chk);
	m_pdspStdMin4->setEnabled(chk);
	m_pdspStdMax4->setEnabled(chk);

	//Sequencing
	m_pdspVolt5->setEnabled(chk);
	m_pdspDuration5->setEnabled(chk);
	//m_pchkStartAutoDegating5->setEnabled(chk);
	//m_pchkStartAutoZeroAdjust5->setEnabled(chk);
	m_pdspTimerQC5->setEnabled(chk);
	m_pdspCurrentMin5->setEnabled(chk);
	m_pdspCurrentMax5->setEnabled(chk);

	//MuxScan
	//m_pchkWaitingZeroAdjust6->setEnabled(chk);
	m_pdspVolt6->setEnabled(chk);
	m_pdspDuration6->setEnabled(chk);
	m_pdspCurrentMin6->setEnabled(chk);
	m_pdspCurrentMax6->setEnabled(chk);
	m_pdspStdMin6->setEnabled(chk);
	m_pdspStdMax6->setEnabled(chk);
	m_pdspVolt62->setEnabled(chk);
	m_pdspDuration62->setEnabled(chk);
	m_pdspVolt63->setEnabled(chk);
	m_pdspDuration63->setEnabled(chk);
	//m_pchkStartAutoProtectDegatingZeroAdjust6->setEnabled(chk);
	m_pispCycle6->setEnabled(chk);
	m_pdspTimerPeriod6->setEnabled(chk);
}

void FlowModulePane::timerChannleQCSlot(void)
{
	m_pbtnChannelQC->setEnabled(false);

	emit SetAllChannelDCLevelSignal(0.0);
	ConfigServer::caliSleep(500);
	emit SetAllChannelDCLevelSignal(m_pdspVolt1->value());
	ConfigServer::caliSleep(m_pdspDuration1->value() * 1000);
	emit SetDCLevelStdSelectedSignal(m_pdspCurrentMin1->value(), m_pdspCurrentMax1->value(), m_pdspStdMin1->value(), m_pdspStdMax1->value());
	ConfigServer::caliSleep(m_pdspDuration12->value() * 1000);
	emit SetAllChannelDCLevelSignal(0.0);
	ConfigServer::caliSleep(500);

	m_pbtnChannelQC->setEnabled(true);
}

void FlowModulePane::timerMakeMembraneSlot(void)
{
	m_pbtnMakeMembrane->setEnabled(false);

	emit SetAllChannelDCLevelSignal(m_pdspVolt2->value());
	ConfigServer::caliSleep(m_pdspDuration2->value() * 1000);

	//if (m_pchkWaitingZeroAdjust2->isChecked())
	{
		//Set ZeroAdjust ???
		emit SetClickZeroAdjustSignal();
		ConfigServer::caliSleep(2000);
		while (ConfigServer::GetInstance()->GetZeroAdjustProcess())//Waiting for ZeroAdjust
		{
			ConfigServer::caliSleep(1000);
		}
	}

	emit SetDCLevelStdSelectedSignal(m_pdspCurrentMin2->value(), m_pdspCurrentMax2->value(), m_pdspStdMin2->value(), m_pdspStdMax2->value());
	ConfigServer::caliSleep(500);

	m_pbtnMakeMembrane->setEnabled(true);
}

void FlowModulePane::timerPoreInsertSlot(void)
{
	m_pbtnPoreInsert->setEnabled(false);

	emit SetSelChannelDCLevelSignal(m_pdspVolt3->value());
	ConfigServer::caliSleep(m_pdspDuration3->value() * 1000);

	//if (m_pchkStartSinglePoreQC3->isChecked())
	{
		bool bret = false;
		emit SetEnableSingleHoleSignal(true, bret);
		//ConfigServer::caliSleep(1000);
		//emit SetAutoSingleHoleSignal(true, bret);
		ConfigServer::caliSleep(m_pdspDuration32->value() * 1000);
	}

	emit SetDCLevelSelectedSignal(m_pdspCurrentMin3->value(), m_pdspCurrentMax3->value());
	ConfigServer::caliSleep(500);

	m_pbtnPoreInsert->setEnabled(true);
}

void FlowModulePane::timerPoreQCSlot(void)
{
	m_pbtnPoreQC->setEnabled(false);

	//bool bret = false;
	//emit SetAutoSingleHoleSignal(false, bret);
	//if (bret)
	//{
	//	ConfigServer::caliSleep(1000);
	//}
	//emit SetEnableSingleHoleSignal(false, bret);
	//if (bret)
	//{
	//	ConfigServer::caliSleep(1000);
	//}

	//if (m_pchkWaitingZeroAdjust4->isChecked())
	{
		//Set ZeroAdjust ???
		emit SetClickZeroAdjustSignal();
		ConfigServer::caliSleep(2000);
		while (ConfigServer::GetInstance()->GetZeroAdjustProcess())//Waiting for ZeroAdjust
		{
			ConfigServer::caliSleep(1000);
		}
	}

	emit SetSelChannelDCLevelSignal(m_pdspVolt4->value());
	ConfigServer::caliSleep(m_pdspDuration4->value() * 1000);
	emit SetSelChannelDCLevelSignal(m_pdspVolt42->value());
	ConfigServer::caliSleep(m_pdspDuration42->value() * 1000);
	emit SetSelChannelDCLevelSignal(m_pdspVolt43->value());
	ConfigServer::caliSleep(m_pdspDuration43->value() * 1000);
	emit SetDCLevelStdSelectedSignal(m_pdspCurrentMin4->value(), m_pdspCurrentMax4->value(), m_pdspStdMin4->value(), m_pdspStdMax4->value());
	ConfigServer::caliSleep(500);

	m_pbtnPoreQC->setEnabled(true);
}

void FlowModulePane::timerSeqencingSlot(void)
{
	m_pbtnSequencing->setEnabled(false);

	emit SetSelChannelDCLevelSignal(m_pdspVolt5->value());
	ConfigServer::caliSleep(m_pdspDuration5->value() * 1000);

	bool bret = false;
	//if (m_pchkStartAutoDegating5->isChecked())
	{
		emit SetEnableAutoDegatingSignal(true, bret);
		ConfigServer::caliSleep(1000);
		//emit SetCheckAutoDegatingSignal(true, bret);
		//ConfigServer::caliSleep(1000);
	}
	//if (m_pchkStartAutoZeroAdjust5->isChecked())
	{
		emit SetEnableZeroAdjustSignal(true, bret);
		ConfigServer::caliSleep(1000);
	}

	if (!m_tmrSeqencingSelect.isActive())
	{
		SetTimehms5();
		m_tmrSeqencingSelect.start(1000);
		//m_tmrSeqencingSelect.start(m_pdspStartTimerDur->value() * 60000);
	}

	m_pbtnSequencing->setEnabled(true);
}

void FlowModulePane::timerSeqencingSelectSlot(void)
{
	m_tmSeqencingSelect = m_tmSeqencingSelect.addSecs(-1);
	m_plcdTimerQC5->display(m_tmSeqencingSelect.toString("hh:mm:ss"));
	if (m_tmSeqencingSelect == QTime(0, 0, 0, 0))
	{
		SetTimehms5();
		m_plcdTimerQC5->display(m_tmSeqencingSelect.toString("hh:mm:ss"));

		if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())//when ZeroAdjust not Filter
		{
			emit SetSeqencingSelectSignal(m_pdspVolt5->value(), m_pdspCurrentMin5->value(), m_pdspCurrentMax5->value());
		}
	}
}

void FlowModulePane::timerSeqencingQCSlot(void)
{
	m_pbtnSequencingQC->setEnabled(false);

	//qc....
	//if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
	//{
	//	emit SetCancelZeroAdjustSignal();
	//	//ConfigServer::caliSleep(2000);
	//}
	if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
	{
		//if (m_pchkWaitingZeroAdjust6->isChecked())
		{
			emit SetShowChannelDCLevelSignal(0.0);
			ConfigServer::caliSleep(500);

			emit SetClickZeroAdjustSignal();
			//ConfigServer::caliSleep(2000);
			//Waiting for ZeroAdjust ???
			while (ConfigServer::GetInstance()->GetZeroAdjustProcess())
			{
				ConfigServer::caliSleep(1000);
			}
		}

		emit SetShowChannelDCLevelSignal(m_pdspVolt6->value());
		ConfigServer::caliSleep(m_pdspDuration6->value() * 1000);

		emit SetDCLevelStdShowSelectedSignal(m_pdspCurrentMin6->value(), m_pdspCurrentMax6->value(), m_pdspStdMin6->value(), m_pdspStdMax6->value());
	}

	MyQMessageBox mymsgbox(this);
	int iret = mymsgbox.exec();
	m_bIsNextContinue = (iret == QMessageBox::RejectRole);

	if (!m_bIsNextContinue)
	{
		return;
	}

	//sequencing....
	DoSequencingFirst();
}

void FlowModulePane::timerSeqencingQCSelectSlot(void)
{
	m_tmSeqencingQC = m_tmSeqencingQC.addSecs(-1);
	m_plcdTimerPeriod6->display(m_tmSeqencingQC.toString("hh:mm:ss"));
	if (m_tmSeqencingQC == QTime(0, 0, 0, 0))
	{
		++m_iCycleCount;
		m_pbtnSequencingQC->setText(QString("Stop (%1)").arg(m_pispCycle6->value() - m_iCycleCount));

		if (m_iCycleCount >= m_pispCycle6->value())
		{
			if (m_pbtnSequencingQC->isChecked())
			{
				m_pbtnSequencingQC->click();
			}
			LOGI("Time Period QC Seqpuencing Reach Total Cycle Auto Stop!");
			return;
		}

		//ConfigServer::GetInstance()->SetSaveData(false);
		emit SetDataSaveSignal(false);

		bool bret = false;
		emit SetEnableZeroAdjustSignal(false, bret);
		emit SetEnableAutoDegatingSignal(false, bret);
		//emit SetAutoSingleHoleSignal(false, bret);
		emit SetEnableSingleHoleSignal(false, bret);

		SetTimehms6();

		//qc....
		//if (ConfigServer::GetInstance()->GetZeroAdjustProcess())
		//{
		//	emit SetCancelZeroAdjustSignal();
		//	//ConfigServer::caliSleep(2000);
		//}
		if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
		{
			//if (m_pchkWaitingZeroAdjust6->isChecked())
			{
				emit SetShowChannelDCLevelSignal(0.0);
				ConfigServer::caliSleep(500);

				emit SetClickZeroAdjustSignal();
				//ConfigServer::caliSleep(2000);
				//Waiting for ZeroAdjust ???
				while (ConfigServer::GetInstance()->GetZeroAdjustProcess())
				{
					ConfigServer::caliSleep(1000);
				}
			}

			emit SetShowChannelDCLevelSignal(m_pdspVolt6->value());
			ConfigServer::caliSleep(m_pdspDuration6->value() * 1000);

			emit SetDCLevelStdShowSelectedSignal(m_pdspCurrentMin6->value(), m_pdspCurrentMax6->value(), m_pdspStdMin6->value(), m_pdspStdMax6->value());
		}

		if (m_pbtnSequencingQC->isChecked())
		{
			MyQMessageBox mymsgbox(this);
			int iret = mymsgbox.exec();
			m_bIsNextContinue = (iret == QMessageBox::RejectRole);

			if (!m_bIsNextContinue)
			{
				m_tmrSeqencingQCSelect.stop();
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

void FlowModulePane::OnClickChannelQC(void)
{
	m_pbtnChannelQC->setEnabled(false);

	if (!m_tmrChannleQC.isActive())
	{
		m_tmrChannleQC.start(1000);
	}
}

void FlowModulePane::OnClickMakeMembrane(void)
{
	m_pbtnMakeMembrane->setEnabled(false);

	if (!m_tmrMakeMembrane.isActive())
	{
		m_tmrMakeMembrane.start(1000);
	}
}

void FlowModulePane::OnClickPoreInsert(void)
{
	m_pbtnPoreInsert->setEnabled(false);

	if (!m_tmrPoreInsert.isActive())
	{
		m_tmrPoreInsert.start(1000);
	}
}

void FlowModulePane::OnClickPoreQC(void)
{
	m_pbtnPoreQC->setEnabled(false);

	if (!m_tmrPoreQC.isActive())
	{
		m_tmrPoreQC.start(1000);
	}
}

void FlowModulePane::OnClickNextStep(void)
{
	if (m_bIsNextContinue)
	{
		return;
	}
	m_bIsNextContinue = true;

	if (m_iCycleCount <= 0)
	{
		//sequencing....
		DoSequencingFirst();
	}
	else
	{
		//sequencing....
		DoSequencingLoop();
		if (m_pbtnSequencingQC->isChecked())
		{
			m_tmrSeqencingQCSelect.start(1000);
		}
	}
}

void FlowModulePane::OnClickSeqencing(bool chk)
{
	m_pbtnSequencing->setEnabled(false);

	if (chk)
	{
		SetTimehms5();

		if (!m_tmrSeqencing.isActive())
		{
			m_tmrSeqencing.start(1000);
		}
		m_pbtnSequencing->setText("Stop");
		//m_pbtnSeqencing->setEnabled(true);
	}
	else
	{
		if (m_tmrSeqencingSelect.isActive())
		{
			m_tmrSeqencingSelect.stop();
		}

		bool bret = false;
		emit SetEnableZeroAdjustSignal(false, bret);
		emit SetCheckAutoDegatingSignal(false, bret);
		emit SetEnableAutoDegatingSignal(false, bret);

		m_pbtnSequencing->setText("Start");
		m_pbtnSequencing->setEnabled(true);
	}
}

void FlowModulePane::OnClickSeqencingQC(bool chk)
{
	m_pbtnSequencingQC->setEnabled(false);
	if (chk)
	{
		bool bret = false;
		emit SetEnableZeroAdjustSignal(false, bret);
		emit SetCheckAutoDegatingSignal(false, bret);
		emit SetEnableAutoDegatingSignal(false, bret);
		//emit SetAutoSingleHoleSignal(false, bret);
		emit SetEnableSingleHoleSignal(false, bret);

		SetTimehms6();

		if (!m_tmrSeqencingQC.isActive())
		{
			m_tmrSeqencingQC.start(1000);
		}

		m_iCycleCount = 0;
		m_pbtnSequencingQC->setText(QString("Stop (%1)").arg(m_pispCycle6->value() - m_iCycleCount));
	}
	else
	{
		m_bIsNextContinue = true;
		if (m_tmrSeqencingQCSelect.isActive())
		{
			m_tmrSeqencingQCSelect.stop();
		}

		emit SetDataSaveSignal(false);
		emit SetChannelStatusSignal(false);
		bool bret = false;
		emit SetEnableZeroAdjustSignal(false, bret);
		emit SetCheckAutoDegatingSignal(false, bret);
		emit SetEnableAutoDegatingSignal(false, bret);
		//emit SetAutoSingleHoleSignal(false, bret);
		emit SetEnableSingleHoleSignal(false, bret);
		m_iCycleCount = 0;
		emit SetShowChannelDCLevelSignal(0.0);

		m_pbtnSequencingQC->setText("Start");
		m_pbtnSequencingQC->setEnabled(true);
	}
}

//void FlowModulePane::GetNextContinueSlot(bool& val)
//{
//	val = m_bIsNextContinue;
//}
