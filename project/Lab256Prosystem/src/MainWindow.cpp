#include "MainWindow.h"

#include <qmenubar.h>
#include <qtoolbar.h>
#include <qdockwidget.h>

#include <Log.h>

#include "CustomPlotView.h"
#include "ChannelStatusView.h"
#include "CapacitanceView.h"
#include "HistoryDataView.h"
#include "ControlView.h"
#include "LogPane.h"
#include "SavePane.h"
//#include "AdjustmentPane.h"
#include "FlowModulePane.h"
#include "CalibrationDialog.h"
#include "FeedbackDialog.h"
#include "AboutDialog.h"
#include "VerUpdateDialog.h"
#include "SampleRateDialog.h"
#include "PoreSelectDialog.h"
#include "mainversion.h"
#include "ConfigServer.h"
#include "UsbProtoMsg.h"
#include "TemperatureMonitor.h"
#include "UpdateDialog.h"


ecoli::CEcoliLog* ecoli::CEcoliLog::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    InitConfig();

    InitMenuToolbar();
    InitViewPane();


    //const char* pswapmem = "              total        used        free      shared  buff/cache   available\nMem:          15793        1101       13831          31         859       14364\nSwap: 2047           0        2047\n";
    //std::vector<std::string> vetswapmem;
    //ConfigServer::SplitString2(pswapmem, " ", vetswapmem);
    //union {
    //    struct {
    //        UCHAR Recipient : 5;
    //        UCHAR Type : 2;
    //        UCHAR Direction : 1;
    //    } bmRequest;
    //
    //    UCHAR bmReq;
    //};
    //bmRequest.Recipient = 0;
    //bmRequest.Type = 2;
    //bmRequest.Direction = 1;
    //auto xxyy = bmReq;
    //union _A
    //{
    //    int i;
    //    char x[2];
    //}A;
    //A.x[0] = 10;
    //A.x[1] = 1;
    //std::cout << A.i << std::endl;
    //int m = [](int x) { return [](int y) { return y * 2; }(x) * 3 + 6; }(5);
    //std::cout << "m:" << m << std::endl;
    //auto gFunc = [](int x) -> std::function<int(int)> { return [=](int y) { return x * 2 + y * 3; }; };
    //auto hFunc = [](const std::function<int(int)>& f, int z) { return f(z) * 2; };
    //auto ha = hFunc(gFunc(7), 8);
    //std::cout << ha << std::endl;
    //int a = 111, b = 222;
    //auto func = [=, &b]()mutable { a = 333; b = 444; };
    //func();
    //std::cout << "a:" << a << " b:" << b << std::endl;
    //int c = 333, d = 444;
    //auto func2 = [=, &c] { c = 555; };
    //func2();
    //std::cout << "c:" << c << " d:" << d << std::endl;


    connect(&m_tmrRunTimeRec, &QTimer::timeout, this, &MainWindow::UpdateRuntimeSlot);
    connect(&m_tmrAutoStopSaving, &QTimer::timeout, this, &MainWindow::UpdateAutoStopTimeSlot);

    m_tmrCheckVerupdate.setSingleShot(true);
    connect(&m_tmrCheckVerupdate, &QTimer::timeout, this, &MainWindow::timerCheckVerupdateSlot);

    connect(m_pactEnableOpenGl, &QAction::triggered, m_pCustomPlotView, &CustomPlotView::OnEnableOpengl);
    connect(m_pControlView, &ControlView::checkChangedSignal, m_pCustomPlotView, &CustomPlotView::checkChangedSlot);
    connect(m_pControlView, &ControlView::colorChangedSignal, m_pCustomPlotView, &CustomPlotView::colorChangedSlot);
    connect(m_pControlView, &ControlView::setCalibrationSignal, m_pCalibrationDlg, &CalibrationDialog::setCalibrationSlot);
    connect(m_pControlView, &ControlView::SetSelChannelSignal, m_pSavePane, &SavePane::SetSelChannelSlot);
    connect(m_pControlView, &ControlView::SetCloseChannelSignal, m_pSavePane, &SavePane::SetCloseChannelSlot);
    connect(m_pControlView, &ControlView::SetDataSaveSignal, m_pSavePane, &SavePane::SetDataSaveSlot);
    connect(m_pControlView, &ControlView::SetChannelStatusSignal, m_pChannelStatusView, &ChannelStatusView::SetChannelStatusSlot);
    connect(m_pControlView, &ControlView::setCapCalculationSignal, m_pCapacitanceView, &CapacitanceView::setCapCalculationSlot);
    connect(m_pControlView, &ControlView::getCapCalculationSignal, m_pCapacitanceView, &CapacitanceView::getCapCalculationSlot);

    connect(m_pControlView, &ControlView::transmitUsbData, this, &MainWindow::FPGARegisterWrite);
    //connect(m_pControlView, &ControlView::setSequencingVoltageSignal, this, &MainWindow::setSequencingVoltageSlot);
    //connect(m_pControlView, &ControlView::getSequencingVoltageSignal, this, &MainWindow::getSequencingVoltageSlot);
    connect(m_pControlView, &ControlView::ShowPoreSelectSignal, this, &MainWindow::ShowPoreSelectSlot);

    //connect(m_pControlView, &ControlView::GetNextContinueSignal, m_pModuleFlowPane, &FlowModulePane::GetNextContinueSlot);

    connect(m_pCalibrationDlg, &CalibrationDialog::SetAllChannelVoltSignal, m_pControlView, &ControlView::SetAllChannelVoltSlot);

    connect(m_pSavePane, &SavePane::startAutoStopTimerSignal, this, &MainWindow::startAutoStopTimerSlot);
    connect(m_pSavePane, &SavePane::stopAutoStopTimerSignal, this, &MainWindow::stopAutoStopTimerSlot);
    connect(m_pSavePane, &SavePane::resetAutoStopTimerSignal, this, &MainWindow::resetAutoStopTimerSlot);
    connect(m_pSavePane, &SavePane::onSequencingIdChangedSignal, this, &MainWindow::onSequencingIdChangedSlot);
    connect(m_pSavePane, &SavePane::startStopRecordVoltSignal, this, &MainWindow::startStopRecordVoltSlot);
    connect(m_pSavePane, &SavePane::setDegatingDataNameSignal, m_pControlView, &ControlView::setDegatingDataNameSlot);

    connect(m_pTemperatureMonitorView, &TemperatureMonitor::SetCurrentTempSignal, this, &MainWindow::SetCurrentTempSlot);

    //connect(m_pChannelStatusView, &ChannelStatusView::GetCapacitanceColorSignal, m_pCapacitanceView, &CapacitanceView::GetCapacitanceColorSlot);
    //connect(m_pChannelStatusView, &ChannelStatusView::GetCapacitanceMergeSignal, m_pCapacitanceView, &CapacitanceView::GetCapacitanceMergeSlot);
    connect(m_pCapacitanceView, &CapacitanceView::setTriangeVoltageSignal, m_pControlView, &ControlView::setTriangeVoltageSlot);

#if 0
    connect(m_pModuleFlowPane, &FlowModulePane::SetAllChannelDCLevelSignal, m_pControlView, &ControlView::SetAllChannelDCLevelSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetShowChannelDCLevelSignal, m_pControlView, &ControlView::SetShowChannelDCLevelSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetSelChannelDCLevelSignal, m_pControlView, &ControlView::SetSelChannelDCLevelSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetDCLevelSelectedSignal, m_pControlView, &ControlView::SetDCLevelSelectedSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetDCLevelStdSelectedSignal, m_pControlView, &ControlView::SetDCLevelStdSelectedSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetEnableSingleHoleSignal, m_pControlView, &ControlView::SetEnableSingleHoleSlot);
    //connect(m_pModuleFlowPane, &FlowModulePane::SetAutoSingleHoleSignal, m_pControlView, &ControlView::SetAutoSingleHoleSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetClickZeroAdjustSignal, m_pControlView, &ControlView::SetClickZeroAdjustSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetEnableZeroAdjustSignal, m_pControlView, &ControlView::SetEnableZeroAdjustSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetEnableAutoDegatingSignal, m_pControlView, &ControlView::SetEnableAutoDegatingSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetCheckAutoDegatingSignal, m_pControlView, &ControlView::SetCheckAutoDegatingSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetSeqencingSelectSignal, m_pControlView, &ControlView::SetSeqencingSelectSlot);

    connect(m_pModuleFlowPane, &FlowModulePane::SetCancelZeroAdjustSignal, m_pControlView, &ControlView::SetCancelZeroAdjustSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetDCLevelStdShowSelectedSignal, m_pControlView, &ControlView::SetDCLevelStdShowSelectedSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetChannelSaveDataSelectedSignal, m_pControlView, &ControlView::SetChannelSaveDataSelectedSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetChannelStatusSignal, m_pChannelStatusView, &ChannelStatusView::SetChannelStatusSlot);
    connect(m_pModuleFlowPane, &FlowModulePane::SetDataSaveSignal, m_pSavePane, &SavePane::SetDataSaveSlot);
#endif

    //connect(m_pPoreSelectDlg, &PoreSelectDialog::SetAllChannelVoltSignal, m_pControlView, &ControlView::SetAllChannelVoltSlot);
    connect(m_pPoreSelectDlg, &PoreSelectDialog::SetSelChannelDCLevelSignal, m_pControlView, &ControlView::SetSelChannelDCLevelSlot);
    connect(m_pPoreSelectDlg, &PoreSelectDialog::SetPoreFilterChannelSignal, m_pControlView, &ControlView::SetPoreFilterChannelSlot);
    connect(m_pPoreSelectDlg, &PoreSelectDialog::SetNotPoreFilterDCLevelSignal, m_pControlView, &ControlView::SetNotPoreFilterDCLevelSlot);
    connect(m_pPoreSelectDlg, &PoreSelectDialog::SetPoreFilterDCLevelSignal, m_pControlView, &ControlView::SetPoreFilterDCLevelSlot);
    connect(m_pPoreSelectDlg, &PoreSelectDialog::EnablePoreInsertSignal, m_pControlView, &ControlView::EnablePoreInsertSlot);

    LoadConfig();

#ifdef Q_OS_WIN32
#else
    setWindowIcon(QIcon(":/img/img/BGI_LOGO.ico"));//res/img/BGI_LOGO.ico//
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
    OnSearchPort();

    //m_tmrCheckVerupdate.start(150000);//After 150s check verupdate.//Cancel auto check update.
    //if (!m_pactEnableExperiment->isChecked())
    //{
    //    m_pactEnableExperiment->trigger();
    //}
    //m_pactEnableExperiment->setEnabled(false);
    //char arcmd[256] = {0};//pgrep -f "Lab256Prosystem" | while read PID; do echo -17 > /proc/$PID/oom_adj; done
    //system("pgrep -f \"Lab256Prosystem\" | while read PID; do echo -17 > /proc/$PID/oom_adj; done;");
    //system("pgrep -f \"gnome-shell\" | while read PID; do echo -17 > /proc/$PID/oom_adj; done;");
    //system("pgrep -f \"gnome-software\" | while read PID; do echo -17 > /proc/$PID/oom_adj; done;");
    //system("pgrep -f \"Xorg\" | while read PID; do echo -17 > /proc/$PID/oom_adj; done;");
}

MainWindow::~MainWindow()
{
    EndAllThread();

    SaveConfig();

    if (ConfigServer::GetInstance()->GetSaveData())
    {
        m_pSavePane->OnClickStart(false);
    }

    if (m_tmrCheckVerupdate.isActive())
    {
        m_tmrCheckVerupdate.stop();
    }

    stopAutoStopTimerSlot();

    if (m_tmrRunTimeRec.isActive())
    {
        m_tmrRunTimeRec.stop();
    }

    StopAllThread();

    LOGI("App Exit....{}", SOFTWARE_VERSION);
    //m_pLogPane->EndThread();
    m_pLogPane->StopThread();
}

bool MainWindow::InitMenuToolbar(void)
{
    //add menu and toolbar
    m_pMenuFile = menuBar()->addMenu(tr("&File"));
    m_pMenuTool = menuBar()->addMenu(tr("&Tool"));
    m_pMenuHelp = menuBar()->addMenu(tr("&Help"));
    m_pToolBar = addToolBar(tr("toolsbar"));
    m_pToolBar->setObjectName("toolBar");

    //add Calibrate
    m_pactCalibration = new QAction(QIcon(":/img/img/mdc.png"), tr("Calibrate..."), this);
    m_pactCalibration->setStatusTip(tr("Param Calibration"));
    connect(m_pactCalibration, &QAction::triggered, this, &MainWindow::OnShowCalibration);
    m_pMenuFile->addAction(m_pactCalibration);
    //m_pToolBar->addAction(m_pactSetting);

    //add ram
    m_pactLoadFirmware = new QAction(QIcon(":/img/img/config.png"), tr("LoadFirmware"), this);
    m_pactLoadFirmware->setStatusTip(tr("Load Firmware"));
    connect(m_pactLoadFirmware, &QAction::triggered, this, &MainWindow::OnLoadFirmware);
    m_pMenuFile->addAction(m_pactLoadFirmware);
    //m_pToolBar->addAction(m_pactOption);

    m_pMenuFile->addSeparator();
    //add experiment
    m_pactEnableExperiment = new QAction(tr("EnableDeveloper"), this);
    m_pactEnableExperiment->setCheckable(true);
    m_pactEnableExperiment->setStatusTip(tr("Enable Developer Mode"));
    connect(m_pactEnableExperiment, &QAction::triggered, this, &MainWindow::OnEnableExperiment);
    m_pMenuFile->addAction(m_pactEnableExperiment);
    //m_pToolBar->addAction(m_pactOption);
    if (ConfigServer::GetInstance()->GetDeveloperMode() == 1)
    {
        m_pactEnableExperiment->setChecked(true);
    }


    m_pMenuFile->addSeparator();
    //add hardware version
    auto* pHardwarever = m_pMenuFile->addMenu("HardwareVersion");
    m_pactHardwareVersion1 = new QAction(tr("Ver1"), this);
    m_pactHardwareVersion1->setCheckable(true);
    m_pactHardwareVersion1->setStatusTip(tr("Hardware Version 1"));
    pHardwarever->addAction(m_pactHardwareVersion1);

    m_pactHardwareVersion2 = new QAction(tr("Ver2"), this);
    m_pactHardwareVersion2->setCheckable(true);
    m_pactHardwareVersion2->setStatusTip(tr("Hardware Version 2"));
    pHardwarever->addAction(m_pactHardwareVersion2);

    QActionGroup* phardwarever = new QActionGroup(this);
    m_pactHardwareVersion1->setActionGroup(phardwarever);
    m_pactHardwareVersion2->setActionGroup(phardwarever);
    //m_pactHardwareVersion1->setChecked(true);
    if (ConfigServer::GetInstance()->GetHardwareAmpBoardVer() == 1)
    {
        m_pactHardwareVersion1->setChecked(true);
    }
    else
    {
        m_pactHardwareVersion2->setChecked(true);
    }
    connect(m_pactHardwareVersion1, &QAction::triggered, this, &MainWindow::OnChangeHardwareVersionSlot);
    connect(m_pactHardwareVersion2, &QAction::triggered, this, &MainWindow::OnChangeHardwareVersionSlot);

    m_pMenuFile->addSeparator();
    m_pactEnableOpenGl = new QAction(tr("Enable OpenGL"), this);
    m_pactEnableOpenGl->setCheckable(true);
    //m_pactEnableOpenGl->setChecked(true);
    m_pactEnableOpenGl->setStatusTip(tr("Enable OpenGL"));
    m_pMenuFile->addAction(m_pactEnableOpenGl);

    m_pMenuFile->addSeparator();
    //add sample rate
    m_pactSampleRate = new QAction(tr("SampleRate..."), this);
    //m_pactSampleRate->setCheckable(true);
    m_pactSampleRate->setStatusTip(tr("Sample Rate Setting Dialog"));
    connect(m_pactSampleRate, &QAction::triggered, this, &MainWindow::OnShowSampleRate);
    m_pMenuFile->addAction(m_pactSampleRate);


    //add label
    m_plabDevice = new QLabel(QString("USB Devices:"), m_pToolBar);
    m_pToolBar->addWidget(m_plabDevice);

    //add combobox
    m_pcmbDevice = new QComboBox(m_pToolBar);
    m_pcmbDevice->setMinimumWidth(300);
    //m_pcmbDevice->addItem("BGI CYCLONE LAB256 Pro");
    m_pToolBar->addWidget(m_pcmbDevice);

    //add search btn
    m_pbtnSearch = new QPushButton(QIcon(":/img/img/search.png"), QString("Search"), m_pToolBar);
    connect(m_pbtnSearch, &QPushButton::clicked, this, &MainWindow::OnSearchPort);
    m_pToolBar->addWidget(m_pbtnSearch);

    m_pToolBar->addSeparator();
    //add sequencing id
    m_plabSequencingId = new QLabel(QString("1"), m_pToolBar);
    m_plabSequencingId->setMinimumWidth(20);
    m_plabSequencingId->setAlignment(Qt::AlignCenter);
    m_pToolBar->addWidget(m_plabSequencingId);


    m_pToolBar->addSeparator();
    //add open menu& tool item
    m_pactOpen = new QAction(QIcon(":/img/img/system.png"), tr("Open"), this);
    m_pactOpen->setCheckable(true);
    m_pactOpen->setStatusTip(tr("Open an exist port"));
    connect(m_pactOpen, &QAction::triggered, this, &MainWindow::OnOpenPort);
    m_pMenuTool->addAction(m_pactOpen);
    m_pToolBar->addAction(m_pactOpen);

    //add load menu& tool item
    m_pactLoad = new QAction(QIcon(":/img/img/install.png"), tr("LoadReg"), this);
    m_pactLoad->setStatusTip(tr("Load register info"));
    connect(m_pactLoad, &QAction::triggered, this, &MainWindow::OnLoadReg);
    m_pMenuTool->addAction(m_pactLoad);
    m_pToolBar->addAction(m_pactLoad);

    //add start menu& tool item
    m_pactStart = new QAction(QIcon(":/img/img/run.png"), tr("Start"), this);
    m_pactStart->setCheckable(true);
    m_pactStart->setStatusTip(tr("Start recv data"));
    connect(m_pactStart, &QAction::triggered, this, &MainWindow::OnStartStop);
    m_pMenuTool->addAction(m_pactStart);
    m_pToolBar->addAction(m_pactStart);

    m_pToolBar->addSeparator();
    //add temperature menu& tool item
    m_pactTemperature = new QAction(QIcon(":/img/img/temperature.png"), tr("Temperature"), this);
    m_pactTemperature->setStatusTip(tr("Temperature monitor"));
    connect(m_pactTemperature, &QAction::triggered, this, &MainWindow::OnShowTempureTool);
    m_pMenuTool->addAction(m_pactTemperature);
    m_pToolBar->addAction(m_pactTemperature);

    //add pore sel
    m_pMenuTool->addSeparator();
    m_pactPoreSel = new QAction(QIcon(":/img/img/setting_press.png"), tr("PoreFilter..."), this);
    m_pactPoreSel->setStatusTip(tr("Start Pore Filter Dialog"));
    connect(m_pactPoreSel, &QAction::triggered, this, &MainWindow::OnShowPoreSelect);
    m_pMenuTool->addAction(m_pactPoreSel);

    //add temperature val
    //m_pdspTemperature = new QDoubleSpinBox(m_pToolBar);
    //m_pdspTemperature->setSuffix(QStringLiteral(" ℃"));
    //m_pdspTemperature->setDecimals(1);
    //m_pdspTemperature->setMinimum(10.0);
    //m_pdspTemperature->setMaximum(50.0);
    //m_pdspTemperature->setReadOnly(true);
    //m_pdspTemperature->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>当前温度</p></body></html>"));
    //m_pdspTemperature->setStatusTip(tr("Current Temperature"));
    //m_pToolBar->addWidget(m_pdspTemperature);

    m_plcdTemperature = new QLCDNumber(m_pToolBar);
    m_plcdTemperature->setDigitCount(5);    //显示位数
    m_plcdTemperature->setMode(QLCDNumber::Dec);    //十进制
    m_plcdTemperature->setSegmentStyle(QLCDNumber::Flat);
    //m_plcdTemperature->setMaximumHeight(26);
    m_plcdTemperature->display(10.2);
    m_plcdTemperature->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>当前温度</p></body></html>"));
    m_plcdTemperature->setStatusTip(tr("Current Temperature"));
    m_pToolBar->addWidget(m_plcdTemperature);

    m_plabTemperature = new QLabel(QStringLiteral(" ℃ "), m_pToolBar);
    //m_plabTemperature->setMinimumWidth(50);
    m_plabTemperature->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_pToolBar->addWidget(m_plabTemperature);

    //m_pToolBar->addSeparator();
    ////add sequencing voltage
    //m_pdspSeqVolt = new QDoubleSpinBox(m_pToolBar);
    //m_pdspSeqVolt->setSuffix(" V");
    //m_pdspSeqVolt->setDecimals(2);
    //m_pdspSeqVolt->setMinimum(-10.0);
    //m_pdspSeqVolt->setMaximum(10.0);
    //m_pdspSeqVolt->setValue(0.18);
    //m_pdspSeqVolt->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>当前测序电压</p></body></html>"));
    //m_pdspSeqVolt->setStatusTip(tr("Sequencing Voltage"));
    //m_pdspSeqVolt->setReadOnly(true);
    //m_pToolBar->addWidget(m_pdspSeqVolt);

    //m_plabSeqVolt = new QLabel(QString("0.18 V"), m_pToolBar);
    //m_plabSeqVolt->setMinimumWidth(50);
    //m_plabSeqVolt->setAlignment(Qt::AlignCenter);
    //m_plabSeqVolt->setFrameShape(QFrame::Box);
    //m_plabSeqVolt->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>当前测序电压</p></body></html>"));
    //m_plabSeqVolt->setStatusTip(tr("Sequencing Voltage"));
    //m_pToolBar->addWidget(m_plabSeqVolt);


    //add User Guide
    m_pactUserGuide = new QAction(QIcon(":/img/img/version.png"), tr("User Guide"), this);
    m_pactUserGuide->setStatusTip(tr("LAB256 Pro User Guide"));
    connect(m_pactUserGuide, &QAction::triggered, this, &MainWindow::OnShowUserGuide);
    m_pMenuHelp->addAction(m_pactUserGuide);

    //add Feedback sub menu
    auto* pFeedback = m_pMenuHelp->addMenu("Feedback");

    //m_pactReportIssue = new QAction(QIcon(":/img/img/engineer.png"), tr("Report..."), this);
    //m_pactReportIssue->setStatusTip(tr("Report Issue"));
    //connect(m_pactReportIssue, &QAction::triggered, this, &MainWindow::OnSendFeedback);
    //pFeedback->addAction(m_pactReportIssue);
    //m_pactReportIssue->setEnabled(false);
    //
    //m_pactSuggest = new QAction(QIcon(":/img/img/Settings.png"), tr("Suggest..."), this);
    //m_pactSuggest->setStatusTip(tr("Suggest Features"));
    //connect(m_pactSuggest, &QAction::triggered, this, &MainWindow::OnSendFeedback);
    //pFeedback->addAction(m_pactSuggest);
    //m_pactSuggest->setEnabled(false);

    m_pactFeedback = new QAction(QIcon(":/img/img/chinaweather.png"), tr("Feedback..."), this);
    m_pactFeedback->setStatusTip(tr("Send Feedback, Report Issue or Suggest Features"));
    connect(m_pactFeedback, &QAction::triggered, this, &MainWindow::OnSendFeedback);
    pFeedback->addAction(m_pactFeedback);

    m_pMenuHelp->addSeparator();
    //add update
    m_pactUpdate = new QAction(QIcon(":/img/img/update.png"), tr("Check Update"), this);
    m_pactUpdate->setStatusTip(tr("Update LAB256 Pro"));
    connect(m_pactUpdate, &QAction::triggered, this, &MainWindow::OnShowUpdate);
    m_pMenuHelp->addAction(m_pactUpdate);

    //add about
    m_pactAbout = new QAction(QIcon(":/img/img/about.png"), tr("About"), this);
    m_pactAbout->setStatusTip(tr("About LAB256 Pro"));
    connect(m_pactAbout, &QAction::triggered, this, &MainWindow::OnShowAbout);
    m_pMenuHelp->addAction(m_pactAbout);

    //m_pToolBar->addSeparator();
    ////add stage label
    //m_plabCurrentStage = new QLabel(QString("Current Stage:"), m_pToolBar);
    //m_pToolBar->addWidget(m_plabCurrentStage);

    ////add stage combobox
    //m_pcmbCurrentStage = new QComboBox(m_pToolBar);
    ////m_pcmbCurrentStage->setMinimumWidth(300);
    //m_pcmbCurrentStage->addItem("PoreInsert", ECS_PORE_INSERT);
    //m_pcmbCurrentStage->addItem("PoreQC", ECS_PORE_QC);
    //m_pcmbCurrentStage->addItem("Seqencing", ECS_SEQENCING);
    //connect(m_pcmbCurrentStage, SIGNAL(activated(int)), this, SLOT(OnCurrentStageChanged(int)));
    //m_pToolBar->addWidget(m_pcmbCurrentStage);


    return false;
}

bool MainWindow::InitViewPane(void)
{
    setDockNestingEnabled(true);
    takeCentralWidget();

    //plot
    QDockWidget* m_pPlotDockView = new QDockWidget(tr("Plot"), this);
    m_pPlotDockView->setMinimumHeight(300);
    m_pPlotDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pPlotDockView->setObjectName("Plots");

    m_pCustomPlotView = new CustomPlotView(this);
    m_pPlotDockView->setWidget(m_pCustomPlotView);
    addDockWidget(Qt::LeftDockWidgetArea, m_pPlotDockView);

    m_usbbulk.SetCustomPlotView(m_pCustomPlotView);


#if 1
    //channel status
    QDockWidget* m_pChanDockView = new QDockWidget(tr("Status"), this);

    m_pChanDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pChanDockView->setObjectName("Status");

    m_pChannelStatusView = new ChannelStatusView(this);
    m_pChanDockView->setWidget(m_pChannelStatusView);
    tabifyDockWidget(m_pPlotDockView, m_pChanDockView);

    //m_pChanDockView->setVisible(false);
#endif


#if 1
    //cap
    QDockWidget* m_pCapDockView = new QDockWidget(tr("Cap"), this);

    m_pCapDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pCapDockView->setObjectName("Caps");

    m_pCapacitanceView = new CapacitanceView(this);
    m_pCapDockView->setWidget(m_pCapacitanceView);
    tabifyDockWidget(m_pPlotDockView, m_pCapDockView);

    m_usbbulk.SetCollectCap(m_pCapacitanceView);
    m_pChannelStatusView->SetCapacitanceView(m_pCapacitanceView);
#endif


#if 1
    //history
    QDockWidget* m_pHistDockView = new QDockWidget(tr("History"), this);

    m_pHistDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pHistDockView->setObjectName("Historical");

    m_pHistoryDataView = new HistoryDataView(this);
    m_pHistDockView->setWidget(m_pHistoryDataView);
    tabifyDockWidget(m_pPlotDockView, m_pHistDockView);

#endif


    //log
    QDockWidget* m_pLogDockView = new QDockWidget(tr("Log"), this);
    m_pLogDockView->setMaximumHeight(190);
    m_pLogDockView->setMinimumHeight(190);
    m_pLogDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pLogDockView->setObjectName("Logs");

    m_pLogPane = new LogPane(this);
    m_pLogDockView->setWidget(m_pLogPane);
    addDockWidget(Qt::LeftDockWidgetArea, m_pLogDockView);

    QString strTitle = SOFTWARE_TITLE;
    if (ConfigServer::GetInstance()->GetHardwareAmpBoardVer() == 1)
    {
        strTitle.replace(" VHARDWARE_VERSION ", " V1 ");
    }
    else
    {
        strTitle.replace(" VHARDWARE_VERSION ", " V2 ");
    }
    setWindowTitle(strTitle);//+"(OutputOrigialData)".c_str()
    LOGI("App Start...{}", strTitle.toStdString());
    m_pLogPane->StartThread();


#if 1
    //save
    QDockWidget* m_pSaveDockView = new QDockWidget(tr("Save"), this);

    m_pSaveDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pSaveDockView->setObjectName("Saves");

    m_pSavePane = new SavePane(this);
    m_pSaveDockView->setWidget(m_pSavePane);
    tabifyDockWidget(m_pLogDockView, m_pSaveDockView);

    m_usbbulk.SetSavePane(m_pSavePane);
#endif


#if 0
    //adjust
    QDockWidget* m_pAdjustDockView = new QDockWidget(tr("Adjust"), this);
    m_pAdjustDockView->setMaximumHeight(190);
    m_pAdjustDockView->setMinimumHeight(190);
    m_pAdjustDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pAdjustDockView->setObjectName("Adjustments");

    m_pAdjustmentPane = new AdjustmentPane(this);
    m_pAdjustDockView->setWidget(m_pAdjustmentPane);
    tabifyDockWidget(m_pLogDockView, m_pAdjustDockView);
#endif


#if 0
    //flow
    QDockWidget* m_pFlowDockView = new QDockWidget(tr("Flow"), this);
    //m_pFlowDockView->setMaximumHeight(190);
    //m_pFlowDockView->setMinimumHeight(190);
    m_pFlowDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pFlowDockView->setObjectName("Flows");

    m_pModuleFlowPane = new FlowModulePane(this);
    m_pFlowDockView->setWidget(m_pModuleFlowPane);
    tabifyDockWidget(m_pLogDockView, m_pFlowDockView);
#endif


#if 1
    //control
    QDockWidget* m_pCtrlDockView = new QDockWidget(tr("Control"), this);
    m_pCtrlDockView->setMinimumWidth(660);
    m_pCtrlDockView->setMaximumWidth(660);
    m_pCtrlDockView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pCtrlDockView->setObjectName("Controls");

    m_pControlView = new ControlView(this);
    m_pCtrlDockView->setWidget(m_pControlView);
    addDockWidget(Qt::RightDockWidgetArea, m_pCtrlDockView);

    m_usbbulk.SetCurrentView(m_pControlView);
    m_pSavePane->SetControlView(m_pControlView);
    m_pChannelStatusView->SetControlView(m_pControlView);
#endif

    m_pTemperatureMonitorView  = new TemperatureMonitor(this);

    m_pCalibrationDlg = new CalibrationDialog(m_pactStart, this);
    m_usbbulk.SetCalibrationDlg(m_pCalibrationDlg);


    m_pPoreSelectDlg = new PoreSelectDialog(this);
    m_pPoreSelectDlg->SetControlView(m_pControlView);

    //m_pUpdateDialog = new UpdateDialog(this);

    m_pStatusBar = new QStatusBar(this);
    setStatusBar(m_pStatusBar);

    m_labAutoStopSaving.setText(g_qlabelStyle[0]);
    m_tmAutoStopSaving.setHMS(1, 30, 0, 0);
    m_lcdAutoStopSaving.setDigitCount(8);
    m_lcdAutoStopSaving.setSegmentStyle(QLCDNumber::Flat);
    m_lcdAutoStopSaving.setFrameShape(QFrame::Panel);
    m_lcdAutoStopSaving.display(m_tmAutoStopSaving.toString("hh:mm:ss"));

    m_pStatusBar->addPermanentWidget(&m_lcdAutoStopSaving);
    m_pStatusBar->addPermanentWidget(&m_labAutoStopSaving);
    m_lcdAutoStopSaving.setVisible(false);
    m_labAutoStopSaving.setVisible(false);


    m_labRunTimeRec.setText("RunningTime:");
    m_tmRunTimeRec.setHMS(0, 0, 0, 0);
    m_lcdRunTimeRec.setDigitCount(8);
    m_lcdRunTimeRec.setSegmentStyle(QLCDNumber::Flat);
    m_lcdRunTimeRec.setFrameShape(QFrame::NoFrame);
    m_lcdRunTimeRec.display(m_tmRunTimeRec.toString("hh:mm:ss"));

    m_pStatusBar->addPermanentWidget(&m_labRunTimeRec);
    m_pStatusBar->addPermanentWidget(&m_lcdRunTimeRec);


    return false;
}

bool MainWindow::InitConfig(void)
{
    ecoli::CEcoliLog::GetInstance()->Init(true);

    std::string strconfig = ConfigServer::GetUserPath() + "/etc/Conf/SysConfig.ini";
    const char* pconfig = strconfig.c_str();
    bool ret = ConfigServer::SyncDefaultConfig(":/conf/conf/default", pconfig);
    if (ret)
    {
        QFile file(pconfig);
        if (file.exists())
        {
            file.setPermissions(file.permissions() | QFile::ReadOwner | QFile::WriteOwner);
        }

    }
    m_pSysConfig = new QSettings(pconfig, QSettings::IniFormat, this);
    ConfigServer::GetInstance()->InitConfig(m_pSysConfig);

    const char* pimg = (ConfigServer::GetInstance()->GetUseBgiCyclone() == 1) ? "/etc/Conf/BgiCyclone.img" : "/etc/Conf/AFE2256-v3_0.img";
    std::string strimg = ConfigServer::GetCurrentPath() + pimg;
    pimg = strimg.c_str();
    ConfigServer::SyncDefaultConfig(":/conf/conf/default", pimg);
    m_devusb.DownloadFirmware(pimg);
    m_usbbulk.SetUsbCyDevice(&m_devusb);

    std::string struserguide = ConfigServer::GetCurrentPath() + "/etc/Info/";
    const char* puserguide = "LAB256ProUserGuide.pdf";
    char arcmd[1024] = { 0 };
#ifdef Q_OS_WIN32
    sprintf(arcmd, "cd /d %s && del /f %s", struserguide.c_str(), puserguide);
#else
    sprintf(arcmd, "cd %s && rm -f %s", struserguide.c_str(), puserguide);
#endif
    system(arcmd);

    struserguide += puserguide;
    ConfigServer::SyncDefaultConfig(":/info/info/", struserguide.c_str());

    return false;
}

bool MainWindow::LoadConfig(void)
{
    if (m_pSysConfig != nullptr)
    {
        m_pSysConfig->beginGroup("Workspace");
        restoreGeometry(m_pSysConfig->value("MainWindowGeometry").toByteArray());
        restoreState(m_pSysConfig->value("MainWindowState").toByteArray());
        m_pSysConfig->endGroup();

        if (m_pCustomPlotView != nullptr)
        {
            m_pCustomPlotView->LoadConfig(m_pSysConfig);
        }
        if (m_pCapacitanceView != nullptr)
        {
            m_pCapacitanceView->LoadConfig(m_pSysConfig);
        }
        if (m_pControlView != nullptr)
        {
            m_pControlView->LoadConfig(m_pSysConfig);
        }
        if (m_pSavePane != nullptr)
        {
            m_pSavePane->LoadConfig(m_pSysConfig);
        }
        if (m_pTemperatureMonitorView != nullptr)
        {
            m_pTemperatureMonitorView->LoadConfig(m_pSysConfig);
        }

        m_pactEnableOpenGl->setChecked(ConfigServer::GetInstance()->GetUseOpenGL() == 1);
    }
    return false;
}

bool MainWindow::SaveConfig(void)
{
    if (m_pSysConfig != nullptr)
    {
        m_pSysConfig->beginGroup("Workspace");
        m_pSysConfig->setValue("MainWindowGeometry", saveGeometry());
        m_pSysConfig->setValue("MainWindowState", saveState());
        m_pSysConfig->endGroup();

        if (m_pCustomPlotView != nullptr)
        {
            m_pCustomPlotView->SaveConfig(m_pSysConfig);
        }
        if (m_pCapacitanceView != nullptr)
        {
            m_pCapacitanceView->SaveConfig(m_pSysConfig);
        }
        if (m_pControlView != nullptr)
        {
            m_pControlView->SaveConfig(m_pSysConfig);
        }
        if (m_pSavePane != nullptr)
        {
            m_pSavePane->SaveConfig(m_pSysConfig);
        }
        if (m_pTemperatureMonitorView != nullptr)
        {
            m_pTemperatureMonitorView->SaveConfig(m_pSysConfig);
        }
    }
    return false;
}

bool MainWindow::DoShowPoreSelect(bool val)
{
    m_pPoreSelectDlg->SetNeedZeroAdjust(val);
    m_pPoreSelectDlg->show();
    ConfigServer::WidgetPosAdjustByParent(this, m_pPoreSelectDlg);

    return false;
}

bool MainWindow::DoShowUpdate(bool val)
{
    if (!ConfigServer::GetInstance()->GetShowVerupdate())
    {
        ConfigServer::GetInstance()->SetShowVerupdate(true);
        VerUpdateDialog verdlg(this);
        if (val || (verdlg.GetHasUpdate() && (verdlg.GetVersion() != ConfigServer::GetInstance()->GetIgnoreVer())))
        {
            auto ret = verdlg.exec();
            if (ret == QDialog::Rejected)
            {
                if (verdlg.GetExitMain())
                {
                    //not update and exit...
                    QApplication::exit();
                }
            }
            else
            {
                //updated retart?...
                //if (QMessageBox::Yes == QMessageBox::question(this, "Update SUCCESS! Restart now?", "Update SUCCESS! Restart now to take effect?"))
                //{
                //    QApplication::exit();
                //}
            }
        }
        ConfigServer::GetInstance()->SetShowVerupdate(false);
    }

    return false;
}

void MainWindow::OnShowCalibration(void)
{
    m_pCalibrationDlg->show();
    ConfigServer::WidgetPosAdjustByParent(this, m_pCalibrationDlg);
}

void MainWindow::OnLoadFirmware(void)
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Download Program File to FX3 Ram"));
    fileDialog->setNameFilter(tr("Firmware Image files(*.img)"));
    fileDialog->setViewMode(QFileDialog::Detail);

    if (fileDialog->exec())
    {
        QString loadFileNamePath = fileDialog->selectedFiles().at(0);
        LOGE(loadFileNamePath.toStdString().c_str());
        if (loadFileNamePath != "")
        {
            if (m_devusb.DownloadFirmware(loadFileNamePath.toLocal8Bit().constData()))
            {
                QMessageBox::information(this, tr("Info"), tr("Download Firmware Image files Success"));
                OnSearchPort();
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"), tr("Download Firmware Image files Failed"));
                OnSearchPort();
            }
        }
    }
    SAFE_DELETEP(fileDialog);
}

void MainWindow::OnEnableExperiment(bool chk)
{
    //m_pModuleFlowPane->OnEnableAllCtrl(chk);
    int devmode = 0;
    if (chk)
    {
        devmode = 1;
        m_pactEnableExperiment->setText("Disable Developer");
    }
    else
    {
        devmode = 0;
        m_pactEnableExperiment->setText("Enable Developer");
    }
    ConfigServer::GetInstance()->SetDeveloperMode(devmode);
    m_pControlView->LayoutCtrl();

    m_pSysConfig->beginGroup("mainCtrl");
    m_pSysConfig->setValue("developerMode", devmode);
    m_pSysConfig->endGroup();
}

void MainWindow::OnShowSampleRate(void)
{
    SampleRateDialog dlg(this);
    dlg.exec();
}

void MainWindow::OnSearchPort(void)
{
    m_pcmbDevice->clear();
    m_vetDevice.clear();
    m_devusb.SearchDeviceInfo(this->windowHandle(), m_vetDevice);
    for (auto&& ite : m_vetDevice)
    {
        m_pcmbDevice->addItem(QString(std::get<0>(ite).c_str()));
    }
}

void MainWindow::OnOpenPort(bool checked)
{
    if (checked)
    {
#if MOCKDATA_DEMO_TEST
        //if (ConfigServer::GetInstance()->GetUseDemoData() != 0)
        {
            m_pactOpen->setIcon(QIcon(":/img/img/shutdown.png"));
            m_pactOpen->setText(QString("Close"));
            return;
        }
#else
        int cursel = m_pcmbDevice->currentIndex();
        if (cursel >= 0 && m_vetDevice.size() > cursel)
        {
            bool bret = m_devusb.OpenUsbDevice(std::get<1>(m_vetDevice[cursel]), std::get<2>(m_vetDevice[cursel]));
            if (bret)
            {
                m_pactOpen->setIcon(QIcon(":/img/img/shutdown.png"));
                m_pactOpen->setText(QString("Close"));
                return;
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"), tr("Open device failed!!!"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"), tr("No available device!!!"));
        }
        m_pactOpen->setChecked(false);
#endif
    }
    else
    {
        EndAllThread();

        StopAllThread();

        m_devusb.CloseUsbDevice();

        ClearBuf();

        if (m_pactStart->isChecked())
        {
            m_pactStart->trigger();

            m_bResetRunTimeRec = true;
        }
        m_pactOpen->setIcon(QIcon(":/img/img/system.png"));
        m_pactOpen->setText(QString("Open"));
    }
}

void MainWindow::OnLoadReg(void)
{
    std::string strreg = ConfigServer::GetCurrentPath() + "/etc/Conf/LoadReg.csv";
    const char* preg = strreg.c_str();

    ConfigServer::SyncDefaultConfig(":/conf/conf/default", preg);
    struct stat filestat;
    if (stat(preg, &filestat) != 0)
    {
        //print error..
    }
    //int filelen = filestat.st_size;
    FILE* pf = ::fopen(preg, "r");
    if (pf != nullptr)
    {
        ::fseek(pf, 0L, SEEK_END);
        int len = ::ftell(pf) + 1;

        char* pbuf = new char[len];
        memset(pbuf, 0, len);

        ::fseek(pf, 0L, SEEK_SET);
        int buflen = ::fread(pbuf, 1, len, pf);

        std::vector<std::vector<std::string>> vetvetdt;
        std::vector<std::string> vetrow;
        UsbProtoMsg::SplitString(pbuf, "\n", vetrow);

        delete[] pbuf;
        pbuf = nullptr;
        fclose(pf);
        pf = nullptr;

        if (vetrow.size() > 0)
        {
            for (auto&& ite : vetrow)
            {
                std::vector<std::string> vetdt;
                UsbProtoMsg::SplitString(ite.c_str(), ",", vetdt);
                vetvetdt.emplace_back(vetdt);
            }

            for (auto&& ite : vetvetdt)
            {
                if (ite.size() >= 2)
                {
                    if (!ite[0].empty() && !ite[1].empty())
                    {
                        m_devusb.FPGARegisterWrite(::atoi(ite[0].c_str()), UsbProtoMsg::String2Hex(ite[1].c_str()));
                    }
                }
                //std::this_thread::sleep_for(std::chrono::microseconds(1));
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            LOGI("Load LoadReg.csv register config Success.");

            return;
        }
    }
    LOGW("Load LoadReg.csv register config Failed!!!");
    QMessageBox::warning(this, tr("Warning"), tr("Load register config Failed!!!"));
}

void MainWindow::OnShowTempureTool(void)
{
    m_pTemperatureMonitorView->show();
    ConfigServer::WidgetPosAdjustByParent(this, m_pTemperatureMonitorView);
}

void MainWindow::OnStartStop(bool checked)
{
    if (checked)
    {
        if (m_pactOpen->isChecked())
        {
            bool bret = m_usbbulk.StartThread();
            if (bret)
            {
                bret = m_pCustomPlotView->StartThread();
                bret &= m_pControlView->StartThread();
                if (bret)
                {
                    m_pControlView->StartSetTimer(200);

                    bret = StartThread();
                    if (bret)
                    {
                        int arout = 0;
                        auto len = UsbProtoMsg::StartCollect(arout);
                        FPGARegisterWrite(arout, len);

                        m_pactStart->setIcon(QIcon(":/img/img/pause_press.png"));
                        m_pactStart->setText(QString("Stop"));


                        if (m_bResetRunTimeRec)
                        {
                            m_tmRunTimeRec.setHMS(0, 0, 0, 0);
                            m_bResetRunTimeRec = false;
                        }
                        m_tmrRunTimeRec.start(1000);

                        return;
                    }
                }
            }
        }
        QMessageBox::information(this, tr("Warning"), tr("No USB device opened"));
        m_pactStart->setChecked(false);
    }
    else
    {
        EndAllThread();

        StopAllThread();

        int arout = 0;
        auto len = UsbProtoMsg::StopCollect(arout);
        FPGARegisterWrite(arout, len);

        m_pactStart->setIcon(QIcon(":/img/img/run.png"));
        m_pactStart->setText(QString("Start"));

        if (m_tmrRunTimeRec.isActive())
        {
            m_tmrRunTimeRec.stop();
        }
    }
}

void MainWindow::OnShowPoreSelect(void)
{
    DoShowPoreSelect(false);
}

void MainWindow::OnShowUserGuide(void)
{
    std::string strfile = ConfigServer::GetCurrentPath() + "/etc/Info/LAB256ProUserGuide.pdf";

#ifdef Q_OS_WIN32

    ::ShellExecuteA(nullptr, "open", strfile.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

#else

    QString cmd = QString("sudo -u %1 xdg-open %2").arg(QString::fromStdString(ConfigServer::GetInstance()->GetLoginUserName())).arg(strfile.c_str());//evince //nautilus
    QProcess::startDetached(cmd);

#endif
}

void MainWindow::OnSendFeedback(void)
{
    FeedbackDialog fddlg(this);
    fddlg.exec();
}

void MainWindow::OnShowUpdate(void)
{
    //ConfigServer::WidgetPosAdjustByParent(this,m_pUpdateDialog);
    //m_pUpdateDialog->show();
    //m_pUpdateDialog->AutoCheckUpdate();

    DoShowUpdate(true);
}

void MainWindow::OnShowAbout(void)
{
    //QMessageBox::information(this, tr("Info"), tr("This is LAB256 Pro System."));
    AboutDialog dlgAbout(this);
    dlgAbout.exec();
}

//void MainWindow::OnCurrentStageChanged(int val)
//{
//    ConfigServer::GetInstance()->SetCurrentStage(ECurrentStage(val));
//}

bool MainWindow::ClearBuf(void)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_usbbulk.m_dataHandle[i].clear();
        m_pControlView->m_vetAverageData[i].clear();
        m_pCapacitanceView->m_vetMeanCap[i].clear();
    }
    return false;
}

bool MainWindow::FPGARegisterWrite(unsigned short addr, unsigned int data)
{
    return m_devusb.FPGARegisterWrite(addr, data);
}

void MainWindow::SetSampleRateChanged(int val)
{
    int arout = 0;
    size_t len = UsbProtoMsg::SetAllChannelRate(val, arout);
    FPGARegisterWrite(arout, len);

    ConfigServer::GetInstance()->SetSampleRate(val);
    m_pCustomPlotView->setSamplingRateSlot(val);
    m_pControlView->setSamplingRateSlot(val);
    m_pSavePane->setSamplingRateSlot(val);
}

void MainWindow::SetOverMultipleChanged(int val)
{
    int arout = 0;
    size_t len = UsbProtoMsg::SetOverSampMultiple(val, arout);
    FPGARegisterWrite(arout, len);
}

void MainWindow::onSequencingIdChangedSlot(int val)
{
    m_plabSequencingId->setText(QString::number(val + 1));
}

//void MainWindow::setSequencingVoltageSlot(double val)
//{
//    m_pdspSeqVolt->setValue(val);
//    //m_plabSeqVolt->setText(QString("%1 V").arg(val));
//}

//void MainWindow::getSequencingVoltageSlot(double& val)
//{
//    val = m_pdspSeqVolt->value();
//}

void MainWindow::FPGARegisterReadBack(unsigned short addr, unsigned int& rValue, bool& result)
{
    m_devusb.FPGARegisterReadBack(addr, rValue, result);
}

bool MainWindow::StartThread(void)
{
    //auto pusbDevice = m_devusb.GetCurDevice();
    //if (pusbDevice == nullptr)
    //{
    //    LOGE("get mainusbdev addr null");
    //    return false;
    //}
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&MainWindow::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void MainWindow::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());//__PRETTY_FUNCTION__

    static int siflag = 0;
    while (m_bRunning)
    {
        auto quelen = m_pCustomPlotView->m_queDataHandle.size_approx();
        if ((quelen > 100) && (siflag == 0))//5000
        {
            //int arout = 0;
            //auto len = UsbProtoMsg::StopCollect(arout);
            //FPGARegisterWrite(arout, len);
            m_usbbulk.SetThreadSuspend(true);
            siflag = 1;
        }
        else if ((quelen <= 50) && (siflag == 1))
        {
            //int arout = 0;
            //auto len = UsbProtoMsg::StartCollect(arout);
            //FPGARegisterWrite(arout, len);
            m_usbbulk.SetThreadSuspend(false);
            m_usbbulk.m_cvRecv.notify_one();
            siflag = 0;
        }
        else
        {
            std::string vinfo;
            if (ConfigServer::GetInstance()->DequeueVoltageInfo(vinfo))
            {
                if (m_pFile != nullptr)
                {
                    fwrite(vinfo.c_str(), vinfo.size(), 1, m_pFile);
                }
            }
            else
            {
                //normal status nothing todo
                std::this_thread::sleep_for(std::chrono::milliseconds(700));
            }
        }
    }
    std::string sinfo;
    while (ConfigServer::GetInstance()->DequeueVoltageInfo(sinfo));

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool MainWindow::EndThread(void)
{
    m_bRunning = false;
    return false;
}

bool MainWindow::StopThread(void)
{
    m_bRunning = false;
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

void MainWindow::EndAllThread(void)
{
    //m_pCapacitanceView->EndThread();
    m_pControlView->EndThread2();
    m_pControlView->EndThread3();
    EndThread();
    m_usbbulk.EndThread(true);

    m_pCustomPlotView->EndThread();

    m_pControlView->EndThread();
    m_pControlView->StopKillTimer();
}

void MainWindow::StopAllThread(void)
{
    //m_pCapacitanceView->StopThread();
    m_pControlView->StopThread();

    m_pCustomPlotView->StopThread();

    m_usbbulk.StopThread(true);
    StopThread();
    m_pControlView->StopThread2();
    m_pControlView->StopThread3();
}

void MainWindow::UpdateRuntimeSlot(void)
{
    m_tmRunTimeRec = m_tmRunTimeRec.addSecs(1);
    m_lcdRunTimeRec.display(m_tmRunTimeRec.toString("hh:mm:ss"));
}

void MainWindow::UpdateAutoStopTimeSlot(void)
{
    if (m_tmAutoStopSaving <= QTime(0, 0, 0, 0))
    {
        if (m_tmrAutoStopSaving.isActive())
        {
            m_tmrAutoStopSaving.stop();
        }
        if (m_pactStart->isChecked())
        {
            m_pactStart->trigger();
            m_pactStart->setChecked(false);
        }
        m_labAutoStopSaving.setText(g_qlabelStyle[1]);
    }
    else
    {
        m_tmAutoStopSaving = m_tmAutoStopSaving.addSecs(-1);
    }

    m_lcdAutoStopSaving.display(m_tmAutoStopSaving.toString("hh:mm:ss"));
}

void MainWindow::startAutoStopTimerSlot(int msec)
{
    m_labAutoStopSaving.setText(g_qlabelStyle[0]);
    m_lcdAutoStopSaving.setVisible(true);
    m_labAutoStopSaving.setVisible(true);

    m_tmrAutoStopSaving.start(msec);
}

void MainWindow::ShowPoreSelectSlot(bool val)
{
    DoShowPoreSelect(val);
}

void MainWindow::stopAutoStopTimerSlot(void)
{
    if (m_tmrAutoStopSaving.isActive())
    {
        m_tmrAutoStopSaving.stop();
    }
}

void MainWindow::resetAutoStopTimerSlot(int min)
{
    if (!m_tmrAutoStopSaving.isActive())
    {
        m_lcdAutoStopSaving.setVisible(false);
        m_labAutoStopSaving.setVisible(false);
        m_labAutoStopSaving.setText(g_qlabelStyle[0]);
    }

    int hour = min / 60;
    int imin = min % 60;
    m_tmAutoStopSaving.setHMS(hour, imin, 0, 0);

    m_lcdAutoStopSaving.display(m_tmAutoStopSaving.toString("hh:mm:ss"));
}

void MainWindow::OnChangeHardwareVersionSlot(void)
{
    int hdver = 1;
    QString strTitle = SOFTWARE_TITLE;
    if (m_pactHardwareVersion1->isChecked())
    {
        if (ConfigServer::GetInstance()->GetHardwareAmpBoardVer() == 1)
        {
            return;
        }
        strTitle.replace(" VHARDWARE_VERSION ", " V1 ");
        hdver = 1;
    }
    else if (m_pactHardwareVersion2->isChecked())
    {
        if (ConfigServer::GetInstance()->GetHardwareAmpBoardVer() == 2)
        {
            return;
        }
        strTitle.replace(" VHARDWARE_VERSION ", " V2 ");
        hdver = 2;
    }
    bool sta = false;
    if (m_pactStart->isChecked())
    {
        m_pactStart->trigger();
        sta = true;
    }
    setWindowTitle(strTitle);
    ConfigServer::GetInstance()->SetHardwareAmpBoardVer(hdver);
    ConfigServer::GetInstance()->InitData();
    m_pCapacitanceView->ReloadCtrl();
    m_pChannelStatusView->ReloadCtrl();
    LOGI("Changed Hardware Version={}", strTitle.toStdString().c_str());

    m_pSysConfig->beginGroup("HardwareCtrl");
    m_pSysConfig->setValue("ampBoardVer", hdver);
    m_pSysConfig->endGroup();
    if (sta)
    {
        m_pactStart->trigger();
    }
}

void MainWindow::timerCheckVerupdateSlot(void)
{
    DoShowUpdate(false);
}

void MainWindow::SetCurrentTempSlot(double temp)
{
    //m_pdspTemperature->setValue(temp);
    m_plcdTemperature->display(temp);
}

void MainWindow::startStopRecordVoltSlot(int val, const char* ppath)
{
    if (val == 1)
    {
//#ifdef Q_OS_WIN32
//        std::string strstatus = ConfigServer::GetCurrentPath() + "/etc/Voltage/";
//#else
//        std::string strstatus = "/data/Voltage/";
//#endif
        std::string strstatus(ppath);
        strstatus += "/";
        std::filesystem::path pathstatus(strstatus);
        if (!std::filesystem::exists(pathstatus))
        {
            std::filesystem::create_directories(pathstatus);
        }
        std::string datetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toStdString();
        ConfigServer::GetInstance()->SetStartSaveData(time(nullptr));
        m_pFile = fopen((strstatus + datetime + ".txt").c_str(), "w");
        if (m_pFile == nullptr)
        {
            LOGW("Record Voltage FAILED!!!");
        }
    }
    else
    {
        if (m_pFile != nullptr)
        {
            fclose(m_pFile);
            m_pFile = nullptr;
        }
    }
}
