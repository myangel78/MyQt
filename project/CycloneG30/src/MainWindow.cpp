#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <qdockwidget.h>

#include <QFileDialog>
#include <QHBoxLayout>
#include <qtoolbutton.h>
#include <QGroupBox>
#include <QLineEdit>
#include <QThread>
#include <QStateMachine>
#include <QState>

#include "mainversion.h"
#include "ConfigServer.h"
#include "Log.h"
#include "LogPane.h"
#include "AboutDialog.h"
#include "RegisterWidget.h"
#include "ConfigurationWidgt.h"
#include "DataAnalysWidgt.h"
#include "CustomSplitter.h"
#include "UDPSocket.h"
#include "CustomPlotWidgt.h"
#include "QNotify.h"
#include "SavePannel.h"
#include "SensorPanel.h"
#include "VoltCtrPanel.h"
#include "PoreCtrlPanel.h"
#include "DataCalculate.h"
#include "PoreStateMapWdgt.h"
#include "CapCaculate.h"
#include "MuxScanPanel.h"
#include "DegatingPane.h"
#include "TemperatureMonitor.h"
#include "ChartViewWidgt.h"
#include "FlowOpertPanel.h"

ecoli::CEcoliLog* ecoli::CEcoliLog::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
#ifdef Q_OS_WIN32
    OnShowConsole(false);
#endif
    InitConfig();
    InitMenuToolbar();
    InitViewPane();
    InitAnimation();
    m_pLogPane->StartThread();

    QString strTitle = QString("%1 %2").arg(SOFTWARE_NAME).arg(SOFTWARE_VERSION);
    setWindowTitle(strTitle);

    LoadConfig();

    OnSearchLocalIp();
    if(OnSearchPort())
    {
        m_pactOpen->trigger(); //打开软件自动连接USB设备
        if(ConfigServer::GetInstance()->GetIsUsbConnect()) //软件开启设置下初始电压
        {
            m_pVoltCtrPanel->VoltInital();
        }
    }

    LoadStyleSheet(":/qss/qss/myStyle.qss");

//    setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    showMaximized();
//    setMouseTracking(true);
}




MainWindow::~MainWindow()
{
    SaveConfig();

    CloseDataStream();
    EndAllThread();
    LOGI("App Exit.");
    StopAllThread();

    m_pDegatingPane->EndAndStopAllThread();

    m_pLogPane->EndThread();
    m_pLogPane->StopThread();

    StartOrStopRuntimer(false);
    stopAutoStopTimerSlot();

    delete m_pConfigurationWidgt;
    delete m_pRegisterWdgt;
    delete m_pDataAnalysWidgt;
    delete m_pUDPSocket;
    SAFE_DELETEP(m_pTemperatureMonitorView);
    SAFE_DELETEP(m_pNotify);

#ifdef Q_OS_WIN32
    if(GetConsoleWindow()!= nullptr)
    {
        FreeConsole();
    }
#endif

}

bool MainWindow::InitConfig(void)
{
    ecoli::CEcoliLog::GetInstance()->InitEcoliLogger(true);
    ecoli::CEcoliLog::GetInstance()->InitRunLogger();
    ecoli::CEcoliLog::GetInstance()->InitConsoleLogger(true);
//    ecoli::CEcoliLog::GetInstance()->EnaLogToConsole(true);

    std::string strconfig = QDir::currentPath().toStdString()+ "/etc/Conf/SysConfig.ini";
    const char* pconfig = strconfig.c_str();
    bool ret = ConfigServer::SyncDefaultConfig(":/etc/etc/default", pconfig);
    QString settingPath = QString::fromStdString(pconfig);
    QFile file(settingPath);
    if (file.exists())
    {
        m_pSysConfig = new QSettings(settingPath, QSettings::IniFormat, this);
        ConfigServer::GetInstance()->InitConfig(m_pSysConfig);
        if (ret)
        {
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        }
    }

    std::string strloadRegcsv = QString("etc/Conf/LoadReg.csv").toStdString();
    const char* pstrloadRegcsv = strloadRegcsv.c_str();
    ret =ConfigServer::SyncDefaultConfig(":/etc/etc/default", pstrloadRegcsv);
    QString strloadRegPath = QString::fromStdString(pstrloadRegcsv);
    QFile fileLoadReg(strloadRegPath);
    if (fileLoadReg.exists())
    {
        if (ret)
        {
            fileLoadReg.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        }
    }

    std::string strxmltable = QString("etc/Conf/Registers.xml").toStdString();
    const char* pxmlconfig = strxmltable.c_str();
    ConfigServer::SyncDefaultConfig(":/etc/etc/", pxmlconfig);

    std::string struserguide = QDir::currentPath().toStdString() + "/etc/Info/CycloneG30UserGuid.pdf";
    ConfigServer::SyncDefaultConfig(":/info/info/", struserguide.c_str());


    std::string  strLVDSTranscsv= QString("etc/Conf/LVDSTransform.csv").toStdString();
    const char* pstrLVDSTranscsv = strLVDSTranscsv.c_str();
    ret =ConfigServer::SyncDefaultConfig(":/etc/etc/default", pstrLVDSTranscsv);
    QString strLVDSTransPath = QString::fromStdString(pstrLVDSTranscsv);
    QFile fileLVDSTrans(strLVDSTransPath);
    if (fileLVDSTrans.exists())
    {
        if (ret)
        {
            fileLVDSTrans.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        }
        ConfigServer::GetInstance()->OnLoadLVDSTransCsv(strLVDSTransPath);
    }

    return true;
}

bool MainWindow::InitMenuToolbar(void)
{
    //add menu and toolbar
    m_pMenuFile = menuBar()->addMenu(tr("&File"));
    m_pMenuTool = menuBar()->addMenu(tr("&Tool"));
    m_pMenuHelp = menuBar()->addMenu(tr("&Help"));
    m_pToolBar = addToolBar(tr("toolsbar"));
    m_pToolBar->setObjectName("toolBar");
    m_pStatusBar = new QStatusBar(this);
    setStatusBar(m_pStatusBar);

    //add User Guide
    m_pactUserGuide = new QAction(QIcon(":/img/img/userhelp.png"), tr("User Guide"), this);
    m_pactUserGuide->setStatusTip(tr("Cyclone G30 User Guide"));
    connect(m_pactUserGuide, &QAction::triggered, this, &MainWindow::OnShowUserGuide);
    m_pMenuHelp->addAction(m_pactUserGuide);

    //add about
    m_pactAbout = new QAction(QIcon(":/img/img/about.png"), tr("About"), this);
    m_pactAbout->setStatusTip(tr("About CycloneG30"));
    m_pMenuHelp->addAction(m_pactAbout);

    //add log
    m_pactConsole = new QAction(QIcon(":/img/img/console.png"), tr("Console"), this);
    m_pactConsole->setStatusTip(tr("Log Dlg"));
    m_pMenuTool->addAction(m_pactConsole);
    m_pactConsole->setCheckable(true);

    //add label
    m_plabDevice = new QLabel(QString("USB Devices:"), m_pToolBar);
    m_pToolBar->addWidget(m_plabDevice);

    //add combobox
    m_pcmbDevice = new QComboBox(m_pToolBar);
    m_pcmbDevice->setMinimumWidth(300);
    m_pToolBar->addWidget(m_pcmbDevice);

    //add search btn
    m_pbtnSearch = new QPushButton(QIcon(":/img/img/search.png"), QString("Search"), m_pToolBar);
    m_pToolBar->addWidget(m_pbtnSearch);

    m_pMenuFile->addSeparator();
    m_pactEnableOpenGl = new QAction(tr("Enable OpenGL"), this);
    m_pactEnableOpenGl->setCheckable(true);
    m_pactEnableOpenGl->setStatusTip(tr("Enable OpenGL"));
    m_pMenuFile->addAction(m_pactEnableOpenGl);

    m_pactExit = new QAction(tr("Exit"), this);
    m_pactExit->setStatusTip(tr("Exit"));
    m_pMenuFile->addAction(m_pactExit);

    m_pToolBar->addSeparator();
    //add open menu& tool item
    m_pactOpen = new QAction(QIcon(":/img/img/connect.ico"), tr("Open"), this);
    m_pactOpen->setCheckable(true);
    m_pactOpen->setStatusTip(tr("Open an exist port"));
    m_pMenuTool->addAction(m_pactOpen);
    m_pToolBar->addAction(m_pactOpen);



    QMenu* m_pMenuCrc = m_pMenuTool->addMenu("CRC Check");
    m_pactCrcCheck = new QAction(QIcon(":/img/img/crc_check.png"), tr("Pass CRC correct data"), this);
    m_pactCrcCheck->setCheckable(true);
    m_pactCrcCheck->setStatusTip(tr("Pass CRC correct data"));
    m_pMenuCrc->addAction(m_pactCrcCheck);

    m_pactCrcNfPrint = new QAction(QIcon(":/img/img/print_.png"), tr("Enable failed CRC print"), this);
    m_pactCrcNfPrint->setCheckable(true);
    m_pactCrcNfPrint->setStatusTip(tr("Enable failed CRC print"));
    m_pMenuCrc->addAction(m_pactCrcNfPrint);

    m_pactStreamPrintMode =  new QAction(QIcon(":/img/img/print.png"), tr("Enable all stream print"), this);
    m_pactStreamPrintMode->setCheckable(true);
    m_pactStreamPrintMode->setStatusTip(tr("Enable all stream print"));
    m_pMenuTool->addAction(m_pactStreamPrintMode);

    //add udp tool item
    m_plabUdpIcon = new QLabel(this);
    m_plabUdpIcon->setText("<img src=:/img/img/udp.png width=25 height=25><b font style = 'font-family:MicrosoftYaHei; font-size:15px; color:blue;'>UDP</b></font>");
    m_pUdpLocalIpSerchBtn = new QToolButton(this);
    m_plabUdpLocalPortDesc = new QLabel("Local Port: ",this);
    m_pUdpLocalIpInpComb = new QComboBox(this);
    m_pUdpLocalPortInpEdit =new QLineEdit(this);
    m_pUdpLocalIpSerchBtn->setText(QStringLiteral("Local Ip:"));
    m_pUdpLocalIpSerchBtn->setIcon(QIcon(":/img/img/search.png"));
    m_pUdpLocalIpSerchBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);


    QRegExp regExpNetPort("((6553[0-5])|[655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|[1-9][0-9]{2}|[1-9][0-9]|[0-9])");
    m_pUdpLocalPortInpEdit->setValidator(new QRegExpValidator(regExpNetPort, this));
    m_pUdpLocalPortInpEdit->setText("8080");

    m_pToolBar->addSeparator();

    m_pactlabUdpIcon = m_pToolBar->addWidget(m_plabUdpIcon);
    m_pactUdpLocalIpSerchBtn = m_pToolBar->addWidget(m_pUdpLocalIpSerchBtn);
    m_pactUdpLocalIpInpCombn = m_pToolBar->addWidget(m_pUdpLocalIpInpComb);
    m_pactlabUdpLocalPortDesc = m_pToolBar->addWidget(m_plabUdpLocalPortDesc);
    m_pactUdpLocalPortInpEdit = m_pToolBar->addWidget(m_pUdpLocalPortInpEdit);
    m_pUdpLocalIpInpComb->setMinimumWidth(120);
    m_pUdpLocalPortInpEdit->setMaximumWidth(60);

    //add start menu& tool item
    m_pactStart = new QAction(QIcon(":/img/img/run.png"), tr("Start"), this);
    m_pactStart->setCheckable(true);
    m_pactStart->setStatusTip(tr("Start recv data"));
//    m_pactStart->setVisible(false);
    m_pToolBar->addAction(m_pactStart);
    m_pToolBar->addSeparator();


    //add temperature menu& tool item
    m_pactTemperature = new QAction(QIcon(":/img/img/temperature.png"), tr("Temperature"), this);
    m_pactTemperature->setStatusTip(tr("Temperature monitor"));
    m_pMenuTool->addAction(m_pactTemperature);
    m_pToolBar->addAction(m_pactTemperature);

    m_plcdTemperature = new QLCDNumber(this);
    m_plcdTemperature->setDigitCount(5);    //显示位数
    m_plcdTemperature->setMode(QLCDNumber::Dec);    //十进制
    m_plcdTemperature->setSegmentStyle(QLCDNumber::Flat);
    m_plcdTemperature->display(10.2);
    m_plcdTemperature->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>当前温度</p></body></html>"));
    m_plcdTemperature->setStatusTip(tr("Current Temperature"));
    m_pToolBar->addWidget(m_plcdTemperature);

    m_plabTemperature = new QLabel(QStringLiteral(" ℃ "), m_pToolBar);
    m_plabTemperature->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_pToolBar->addWidget(m_plabTemperature);

    m_pStatusPowerConsumpLab = new QLabel(this);
    m_pStatusPowerConsumpLab->setText("<img src=:/img/img/power_low.png alt=BGI width=25 height=25>");
    m_pStatusBar->addPermanentWidget(m_pStatusPowerConsumpLab);
    m_pStatusPowerConsumpLab->setToolTip(QStringLiteral("AFE_Power_Consumption:Low!!"));
    m_pLabCurSensorDisp = new QLabel(QString("Current group: <font color=blue><b>Off</b></font>"),this);
    m_pStatusBar->addPermanentWidget(m_pLabCurSensorDisp,0);
//    m_pToolBar->addWidget(m_pLabCurSensorDisp);
//    m_pToolBar->addSeparator();

    m_pactUdpTestMode = new QAction(QIcon(":/img/img/simulate.png"), tr("Udp Test Mode"), this);
    m_pactUdpTestMode->setCheckable(true);
    m_pactUdpTestMode->setStatusTip(tr("UDP Test Mode!!"));
    m_pMenuTool->addAction(m_pactUdpTestMode);
    m_pactSimulateDataTest = new QAction(QIcon(":/img/img/simulate.png"), tr("Simulate Data Mode"), this);
    m_pactSimulateDataTest->setStatusTip(tr("Simulate Data Mode!!"));
    m_pactSimulateDataTest->setCheckable(true);
    m_pMenuTool->addAction(m_pactSimulateDataTest);


    //add statusbar widget
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

    m_labSpeedDisp.setText("0 MB/s");
    m_labSpeedDisp.setVisible(true);
    m_pStatusBar->addPermanentWidget(&m_labSpeedDisp);

    m_pRunTimeRecLab = new QLabel(QStringLiteral("RunningTime:"),this);
    m_tmRunTimeRec.setHMS(0, 0, 0, 0);
    m_pTmrRunTimeRec= new QTimer(this);
    m_plcdRunTimeRec= new QLCDNumber(8,this);
    m_plcdRunTimeRec->setDigitCount(8);
    m_plcdRunTimeRec->setSegmentStyle(QLCDNumber::Flat);
    m_plcdRunTimeRec->setFrameShape(QFrame::NoFrame);
    m_plcdRunTimeRec->display(m_tmRunTimeRec.toString("hh:mm:ss"));
    m_pStatusBar->addPermanentWidget(m_pRunTimeRecLab);
    m_pStatusBar->addPermanentWidget(m_plcdRunTimeRec);


    m_pStatusBgiIconLab = new QLabel(this);
    m_pStatuConnetLab = new QLabel(this);
    m_pStatusBgiIconLab->setText("<img src=:/img/img/BGI_01.png alt=BGI width=70 height=25>");
    m_pStatuConnetLab->setText("<b font style = 'font-family:MicrosoftYaHei; font-size:15px; color:red; background-color:yellow;'>Disconnected</b></font>");
    m_pStatusBar->addPermanentWidget(m_pStatuConnetLab);
    m_pStatusBar->addPermanentWidget(m_pStatusBgiIconLab);


    connect(m_pactAbout, &QAction::triggered, this, &MainWindow::OnShowAbout);
    connect(m_pactConsole, &QAction::triggered, this, &MainWindow::OnShowConsole);
    connect(m_pbtnSearch, &QPushButton::clicked, this, &MainWindow::OnSearchPort);
    connect(m_pactOpen, &QAction::triggered, this, &MainWindow::OnOpenPort);
    connect(m_pactStart, &QAction::triggered, this, &MainWindow::OnStartStop);
    connect(m_pactExit, &QAction::triggered, this,  &MainWindow::OnExit);
    connect(m_pactUdpTestMode,&QAction::triggered,this,&MainWindow::OnEnableUdpTestMode);
    connect(m_pactStreamPrintMode,&QAction::toggled,this,&MainWindow::OnEnableStreamPrintMode);
    connect(m_pactSimulateDataTest,&QAction::triggered,this,[&](const bool &enable){
         ConfigServer::GetInstance()->SetSimulateRunning(enable);
         m_dataHandle.SetIsSimulateRunning(enable);
         m_pSavePane->EnableShowSimulateGrpbx(enable);
    });

    connect(m_pactCrcCheck,&QAction::toggled,[&](bool state){
        m_pactCrcNfPrint->setEnabled(state);
        ConfigServer::GetInstance()->SetCrcCaculate(state);
        if(!state)
        {
            m_pactCrcNfPrint->setChecked(false);
        }});
    connect(m_pactCrcNfPrint,&QAction::toggled,[&](bool state){ConfigServer::GetInstance()->SetCrcCalPrint(state);});

    connect(m_pTmrRunTimeRec, &QTimer::timeout, this, &MainWindow::UpdateRuntimeSlot);
    connect(m_pactTemperature, &QAction::triggered, this, &MainWindow::OnShowTempureTool);

#ifdef SHIELDING_TEMPERATURE_CONTROL
    m_tempSharedMem.setKey(QStringLiteral("CycloneG30TempSharedMem"));
    connect(&m_tempUpdateTimer,&QTimer::timeout,this,&MainWindow::OnTimerUpdateTempSlot);
    m_tempUpdateTimer.start(1000);
#endif

    LOGI("MainWindwos thread Id = {}",ConfigServer::GetInstance()->GetCurrentThreadSelf());
    return true;
}

bool MainWindow::InitViewPane(void)
{
//    m_pUDPSocket = new UDPSocket(this);
    m_pUDPSocket = new UDPSocket(this);
    m_usbFtdDevice.SetDataHandlePtr(&m_dataHandle);
    m_pUDPSocket->SetDataHandlePtr(&m_dataHandle);

    m_pCenterWidget = new QWidget(this);

    m_pNotify = new QNotify(this);
    ConfigServer::GetInstance()->SetNotificationPtr(m_pNotify);

    //listwidget
    m_pListWdiget = new QListWidget(this);
    //stackwidget
    m_pFuncViewStackWidget = new QStackedWidget(this);
    //Configuration widget
    m_pConfigurationWidgt = new ConfigurationWidgt(&m_usbFtdDevice,this);
    //Register widget
    m_pRegisterWdgt = new RegisterWidget(&m_usbFtdDevice,this);
    //logpanel widget
    m_pLogPane = new LogPane(this);
    //temperature widget
#ifndef SHIELDING_TEMPERATURE_CONTROL
    m_pTemperatureMonitorView  = new TemperatureMonitor(this);
#endif

    //Data handle thread
    m_pDataCalculate = new DataCalculate(this);
    m_pCapCaculate = new CapCaculate(this);
    m_dataHandle.SetDataCalculate(m_pDataCalculate);
    m_dataHandle.SetCapCalculate(m_pCapCaculate);

    //Collect data widget
    m_pDataAnalysWidgt = new DataAnalysWidgt(this);
    m_pCustomPlotWidgt = m_pDataAnalysWidgt->GetCustomPlotWidgt();
    m_pSavePane = m_pDataAnalysWidgt->GetSavePanelWdgt();
    m_pSensorPanel = m_pDataAnalysWidgt->GetSensorPanelWdgt();
    m_pVoltCtrPanel = m_pDataAnalysWidgt->GetVoltCtrlPanelWdgt();
    m_pPoreCtrlPanel = m_pDataAnalysWidgt->GetPoreCtrlPanelWgt();
    m_pDegatingPane = m_pDataAnalysWidgt->GetDegatingPanelWgt();
    m_pMuxScanPanel = m_pDataAnalysWidgt->GetMuxScanPanelWgt();
    m_pPoreStateMapWdgt = m_pDataAnalysWidgt->GetPoreStateMapWgt();
    m_pChartViewWidgt = m_pDataAnalysWidgt->GetChartViewWgt();
    m_pFlowOpertPanel = m_pDataAnalysWidgt->GetFlowOpertWgt();


    m_pMuxScanPanel->SetDataCalculatePtr(m_pDataCalculate);
    m_pMuxScanPanel->SetCustomPlotWdgtPtr(m_pCustomPlotWidgt);
    m_pSensorPanel->SetUsbDevicePtr(&m_usbFtdDevice);
    m_pVoltCtrPanel->SetUsbDevicePtr(&m_usbFtdDevice);
    m_pPoreStateMapWdgt->SetUsbDevicePtr(&m_usbFtdDevice);
    m_pPoreStateMapWdgt->SetCapCalculate(m_pCapCaculate);


    m_dataHandle.SetCustomPlotView(m_pCustomPlotWidgt);
    m_dataHandle.SetSavePane(m_pSavePane);
    m_dataHandle.SetDegatingPane(m_pDegatingPane);


    //widget switch
    m_pListWdiget->insertItem(0, new QListWidgetItem(QIcon(":/img/img/configuration.png"),tr("Configuration"),m_pListWdiget));
    m_pListWdiget->insertItem(1, new QListWidgetItem(QIcon(":/img/img/Registers.png"),tr("Register"),m_pListWdiget));
    m_pListWdiget->insertItem(2, new QListWidgetItem(QIcon(":/img/img/collectdata.png"),tr("Data analysis"),m_pListWdiget));
    m_pListWdiget->insertItem(3, new QListWidgetItem(QIcon(":/img/img/logPage.png"),tr("LogPanel"),m_pListWdiget));
    m_pListWdiget->setIconSize(QSize(100, 40));
    m_pListWdiget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pListWdiget->setFrameShape(QListWidget::NoFrame);
    m_pListWdiget->setObjectName("ViewListWidgt");

    m_pFuncViewStackWidget->addWidget(m_pConfigurationWidgt);
    m_pFuncViewStackWidget->addWidget(m_pRegisterWdgt);
    m_pFuncViewStackWidget->addWidget(m_pDataAnalysWidgt);
    m_pFuncViewStackWidget->addWidget(m_pLogPane);


    //add spliter
    m_pMainSplitter = new CustomSplitter(Qt::Horizontal, SPLITTER_HIDE_DIRECT_LEFT,this);

    QGroupBox *listGrpbox = new QGroupBox(QStringLiteral("View"), this);
    listGrpbox->setObjectName("ViewListGrpbx");
    QVBoxLayout *listvertlayout = new QVBoxLayout();
    listvertlayout->addWidget(m_pListWdiget);
    listGrpbox->setLayout(listvertlayout);
    listGrpbox->setFixedWidth(160);


    QGroupBox *funcGrpbox = new QGroupBox(QStringLiteral("Function"), this);
    funcGrpbox->setObjectName("ViewSpectGrpbx");
    QVBoxLayout *funcvertlayout = new QVBoxLayout();
    funcvertlayout->addWidget(m_pFuncViewStackWidget);
    funcGrpbox->setLayout(funcvertlayout);

    m_pMainSplitter->addWidget(listGrpbox);
    m_pMainSplitter->addWidget(funcGrpbox);
    m_pMainSplitter->setStretchFactor(0, 1);
    m_pMainSplitter->setStretchFactor(1, 10);

    QHBoxLayout *horlayout = new QHBoxLayout();
    horlayout->addWidget(m_pMainSplitter);

    m_pCenterWidget->setLayout(horlayout);
    setCentralWidget(m_pCenterWidget);

    connect(m_pListWdiget,&QListWidget::currentRowChanged,[&](int index){ m_pFuncViewStackWidget->setCurrentIndex(index);});
    connect(m_pRegisterWdgt,&RegisterWidget::ASICRegisterWiteOneSig,&m_usbFtdDevice,&UsbFtdDevice::ASICRegisterWiteOne);
    connect(m_pRegisterWdgt,&RegisterWidget::ASICRegisterReadOneSig,&m_usbFtdDevice,&UsbFtdDevice::ASICRegisterReadOne);
    connect(m_pRegisterWdgt,&RegisterWidget::ASICRegisterReadAllSig,&m_usbFtdDevice,&UsbFtdDevice::ASICRegisterReadAll);
    connect(m_pactEnableOpenGl, &QAction::triggered, m_pCustomPlotWidgt, &CustomPlotWidgt::OnEnableOpengl);

    connect(&m_tmrAutoStopSaving, &QTimer::timeout, this, &MainWindow::UpdateAutoStopTimeSlot);
    connect(m_pSavePane, &SavePannel::startAutoStopTimerSignal, this, &MainWindow::startAutoStopTimerSlot);
    connect(m_pSavePane, &SavePannel::stopAutoStopTimerSignal, this, &MainWindow::stopAutoStopTimerSlot);
    connect(m_pSavePane, &SavePannel::resetAutoStopTimerSignal, this, &MainWindow::resetAutoStopTimerSlot);

    connect(m_pUdpLocalIpSerchBtn,&QToolButton::clicked,this,&MainWindow::OnSearchLocalIp);

    connect(m_pUDPSocket,&UDPSocket::SpeedDispSig,this,&MainWindow::OnDispDeviceSpeed);
    connect(&m_usbFtdDevice,&UsbFtdDevice::SpeedDispSig,this,&MainWindow::OnDispDeviceSpeed);

    qRegisterMetaType<std::vector<float>>("std::vector<float>&");
    connect(m_pDataCalculate,&DataCalculate::SendAverageRultForPoreInsertSig,m_pPoreStateMapWdgt,&PoreStateMapWdgt::PoreProtectByMontorAvegeSlot);
    connect(m_pDataCalculate,&DataCalculate::SendAverageRultForMembroProtectSig,m_pPoreStateMapWdgt,&PoreStateMapWdgt::MembrokeProtectByMontorAvegeSlot);
    connect(m_pCapCaculate,&CapCaculate::SigCapResultFromCalculate,m_pPoreStateMapWdgt,&PoreStateMapWdgt::CapResultFromCalculateSlot);
    connect(m_pDataCalculate,&DataCalculate::SendStdRultForShutOffChannSig,m_pPoreStateMapWdgt,&PoreStateMapWdgt::ShutOffChanByMontorStdSlot);
    connect(m_pConfigurationWidgt,&ConfigurationWidgt::SigSampleRateChanged,this,&MainWindow::OnSampleRateChangeSlot);
    connect(m_pPoreCtrlPanel,&PoreCtrlPanel::SigCalcCurStdForShut,m_pDataCalculate,&DataCalculate::GetCurStdForShutOffSlot);
    connect(m_pPoreCtrlPanel,&PoreCtrlPanel::SigCalCurForPoreInsert,m_pDataCalculate,&DataCalculate::GetCurForPoreInsertSlot);
    connect(m_pPoreCtrlPanel,&PoreCtrlPanel::SigCalCurForMembrokProtect,m_pDataCalculate,&DataCalculate::GetCurForMembrokProtectSlot);

    connect(m_pCustomPlotWidgt,&CustomPlotWidgt::GetAllChAvgSig,m_pDataCalculate,&DataCalculate::GetAllChAvgSlot);
    connect(m_pCustomPlotWidgt,&CustomPlotWidgt::GetAllChStdSig,m_pDataCalculate,&DataCalculate::GetAllChStdSlot);

    connect(m_pSensorPanel,&SensorPanel::UpdateCurrentSensorGrpStrSig,this,&MainWindow::UpdateCurrentSensorGrpStrSlot);

    connect(m_pConfigurationWidgt,&ConfigurationWidgt::SigAFEPowerConsumpChange,this,&MainWindow::AFEPowerConsumpChangeSlot);
#ifndef SHIELDING_TEMPERATURE_CONTROL
    connect(m_pTemperatureMonitorView, &TemperatureMonitor::SetCurrentTempSignal, this, &MainWindow::SetCurrentTempSlot);
#endif
    connect(m_pFlowOpertPanel,&FlowOpertPanel::GetAllChAvgSig,m_pDataCalculate,&DataCalculate::GetAllChAvgSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::GetAllChStdSig,m_pDataCalculate,&DataCalculate::GetAllChStdSlot);

    connect(m_pConfigurationWidgt,&ConfigurationWidgt::SigSetAllChIntalSource,m_pSensorPanel,&SensorPanel::AllChannelAllInternalSourceSet);

    return true;
}


void MainWindow::InitAnimation(void)
{
    m_pMoveOpacMachine = new QStateMachine(this);
    m_pMoveOpacStatStart = new QState(m_pMoveOpacMachine);
    m_pMoveOpacStatEnd = new QState(m_pMoveOpacMachine);
    m_pMoveOpacMachine->setInitialState(m_pMoveOpacStatStart);

    m_pMoveOpacStatStart->assignProperty(this, "windowOpacity", 1);
//    m_pMoveOpacStatStart->assignProperty(this, "geometry",QApplication::desktop()->screenGeometry());

    m_pMoveOpacStatEnd->assignProperty(this, "windowOpacity", 0);
//    m_pMoveOpacStatEnd->assignProperty(this, "geometry", QRectF(geometry().x() , geometry().y(),
//                                                                geometry().width() /2, geometry().height() /2));
//    QSignalTransition *transition = m_pMoveOpacStatStart->addTransition(m_pactExit,&QAction::triggered, m_pMoveOpacStatEnd);
    QSignalTransition *transition = m_pMoveOpacStatStart->addTransition(m_pactExit,&QAction::triggered, m_pMoveOpacStatEnd);

    m_pOpacityAnimation = new QPropertyAnimation(this,"windowOpacity");
    m_pOpacityAnimation->setDuration(1000);
    m_pOpacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);

//    m_pMoveAnimation = new QPropertyAnimation(this,"geometry");
//    m_pMoveAnimation->setDuration(1000);
//    m_pMoveAnimation->setEasingCurve(QEasingCurve::OutQuad);

    transition->addAnimation(m_pOpacityAnimation);
//    transition->addAnimation(m_pMoveAnimation);

    connect(&(*m_pOpacityAnimation), SIGNAL(finished()), this, SLOT(close()));

    m_pMoveOpacMachine->start();
}


void MainWindow::OnShowAbout(void)
{
    //QMessageBox::information(this, tr("Info"), tr("This is LAB256 Pro System."));
    AboutDialog dlgAbout(this);
    dlgAbout.exec();
}


void MainWindow::OnShowConsole(bool checked)
{
#ifdef Q_OS_WIN32
    if(checked)
    {
        if (::GetConsoleWindow() == nullptr)
        {
          if (::AllocConsole())
          {
            (void)freopen("CONIN$", "r", stdin);
            (void)freopen("CONOUT$", "w", stdout);
            (void)freopen("CONOUT$", "w", stderr);
            SetFocus(::GetConsoleWindow());
          }
        }

        ShowWindow(GetConsoleWindow(), SW_SHOW);
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(hStdin, &mode);
        mode &= ~ENABLE_QUICK_EDIT_MODE;  //移除快速编辑模式
        mode &= ~ENABLE_INSERT_MODE;      //移除插入模式
        mode &= ~ENABLE_MOUSE_INPUT;
        SetConsoleMode(hStdin, mode);
    }
    else
    {
        if(GetConsoleWindow()!= nullptr)
        {
            ShowWindow(GetConsoleWindow(), SW_HIDE);
//            FreeConsole();
        }
    }
#endif
    ecoli::CEcoliLog::GetInstance()->EnaLogToConsole(checked ? true : false);

}

void MainWindow::OnShowUserGuide(void)
{
    std::string strfile =  QDir::currentPath().toStdString()+ "/etc/Info/CycloneG30UserGuid.pdf";

#ifdef Q_OS_WIN32
    ::ShellExecuteA(nullptr, "open", strfile.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else

    QString cmd = QString("sudo -u %1 xdg-open %2").arg(QString::fromStdString(ConfigServer::GetInstance()->GetLoginUserName())).arg(strfile.c_str());//evince //nautilus
    QProcess::startDetached(cmd);

#endif
}


bool MainWindow::OnSearchPort(void)
{
    m_pcmbDevice->clear();
    QStringList devDecrList;
    m_usbFtdDevice.OnSearchUsbDevice(devDecrList);
    if(!devDecrList.isEmpty())
    {
        m_pcmbDevice->addItems(devDecrList);
    }
    return m_pcmbDevice->count() != 0;
}



void MainWindow::OnOpenPort(bool checked)
{
    if (checked)
    {
        if(m_dataHandle.GetIsSimulateRunning()) //是否开启了仿真数据模式
        {
         m_pactOpen->setChecked(true);
            return;
        }

        int cursel = m_pcmbDevice->currentIndex();
        if (cursel >= 0 )
        {
            bool bret = m_usbFtdDevice.OpenDeviceByDescr(m_pcmbDevice->currentText());
            if (bret)
            {
                m_pactOpen->setIcon(QIcon(":/img/img/close.ico"));
                m_pactOpen->setText(QString("Close"));
                m_pStatuConnetLab->setText("<b font style = 'font-family:MicrosoftYaHei; font-size:15px; color:green;'>Connected</b></font>");
                ConfigServer::GetInstance()->SetUSBConnect(true);
                return;
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"), tr("Open device failed!!!"));
                ConfigServer::GetInstance()->SetUSBConnect(false);
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"), tr("No available device!!!"));
            ConfigServer::GetInstance()->SetUSBConnect(false);
        }
        m_pactOpen->setChecked(false);
    }
    else
    {
        CloseDataStream();
        m_usbFtdDevice.CloseDevice();
        m_pactOpen->setIcon(QIcon(":/img/img/connect.ico"));
        m_pactOpen->setText(QString("Open"));
        m_pStatuConnetLab->setText("<b font style = 'font-family:MicrosoftYaHei; font-size:15px; color:red; background-color:yellow;'>Disconnected</b></font>");
        ConfigServer::GetInstance()->SetUSBConnect(false);
    }
}

bool MainWindow::OnUdpPortSetting(bool enable)
{
    if(m_pUdpLocalIpInpComb->currentText().isEmpty() ||  m_pUdpLocalPortInpEdit->text().isEmpty())
    {
        QMessageBox::information(this, tr("Warning"), tr("Ip or Port invalid,please try again!!!"));
        return false;
    }
    if (enable)
    {
        m_pUdpLocalIpInpComb->setEnabled(false);
        m_pUdpLocalPortInpEdit->setEnabled(false);
        m_pUDPSocket->OnSetUdpSocket(m_pUdpLocalIpInpComb->currentText(),m_pUdpLocalPortInpEdit->text().toInt());
    }
    else
    {
        m_pUdpLocalIpInpComb->setEnabled(true);
        m_pUdpLocalPortInpEdit->setEnabled(true);
    }
    return true;
}

void MainWindow::OnExit(void)
{
    setFixedSize(width(),height());
    setMaximumSize(65535, 65535);
    setMinimumSize(0, 0);
}

void MainWindow::OnStartStop(bool checked)
{
    bool bret =false;
    if (checked)
    {
        if (!m_pactOpen->isChecked())
        {
            QMessageBox::information(this, tr("Warning"), tr("No USB device opened"));
            goto failed;
        }

        if(!OnUdpPortSetting(true))
        {
            QMessageBox::information(this, tr("Warning"), tr("Please fill in valid udp port !!!"));
            goto failed;
        }

        if(m_dataHandle.GetIsSimulateRunning())
        {
            bool ret = m_pSavePane->OpenLoadSimulateFiles(true);
            m_pSavePane->EnableChooseSimulateDir(!ret);
            if(!ret)
            {
                QMessageBox::information(this, tr("Warning"), tr("Please check whether 1024 channels' files exist in this directory !!!"));
                goto failed;
            }
        }
        if (m_pactOpen->isChecked())
        {
            m_pactStart->setIcon(QIcon(":/img/img/stop.png"));
            m_pactStart->setText(QString("Stop"));
            m_pactUdpTestMode->setEnabled(false);
            m_pactStreamPrintMode->setEnabled(false);
            m_pConfigurationWidgt->SetEnable(false);
            m_pactSimulateDataTest->setEnabled(false);
            if(!m_dataHandle.GetIsSimulateRunning())
            {
            bret = m_pUDPSocket->StartThread();
            m_usbFtdDevice.SwitchOnUsbDataStream(true);
            }
            m_pChartViewWidgt->OpenRecordFile(true);
            bret = m_dataHandle.StartThread();
            bret = m_pDataCalculate->StartThread();
            bret = m_pCustomPlotWidgt->StartThread();
            bret = StartThread();
            ConfigServer::GetInstance()->SetStreamRunning(true);
            StartOrStopRuntimer(true);
            return;
        }
    }
    else
    {
        OnUdpPortSetting(false);
        ConfigServer::GetInstance()->SetStreamRunning(false);
        m_pChartViewWidgt->OpenRecordFile(false);

        bret = m_pUDPSocket->EndThread();
        bret = m_pUDPSocket->StopThread();
        m_usbFtdDevice.SwitchOnUsbDataStream(false);
        EndAllThread();
        StopAllThread();
        m_pactStart->setIcon(QIcon(":/img/img/run.png"));
        m_pactStart->setText(QString("Start"));
        m_pactUdpTestMode->setEnabled(true);
        m_pactStreamPrintMode->setEnabled(true);
        m_pConfigurationWidgt->SetEnable(true);
        m_pactSimulateDataTest->setEnabled(true);
        OnDispDeviceSpeed(0);
        StartOrStopRuntimer(false);
        if(m_dataHandle.GetIsSimulateRunning())
        {
            m_pSavePane->OpenLoadSimulateFiles(false);
            m_pSavePane->EnableChooseSimulateDir(true);
        }
    }
    return;
failed:
    m_pactStart->setChecked(false);
    ConfigServer::GetInstance()->SetStreamRunning(false);
    StartOrStopRuntimer(false);

}


bool MainWindow::StartThread(void)
{
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
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
    static int siflag = 0;
    int maxquelen = 1000;
    while (m_bRunning)
    {
//        auto quelen = m_pCustomPlotWidgt->m_queDataHandle.size_approx();
        auto quelen = m_dataHandle.m_queDecodePack.size_approx();
//        auto quelen = m_dataHandle.m_queDataPack.size_approx();
        if (quelen > maxquelen)//5000
        {
            if (siflag == 0)
            {
                //int arout = 0;
                //auto len = UsbProtoMsg::StopCollect(arout);
                //FPGARegisterWrite(arout, len);
                if(m_dataHandle.GetIsSimulateRunning())
                {
                m_dataHandle.SetTestThreadSuspend(true);
                }
                else
                {
                m_pUDPSocket->SetThreadSuspend(true);
                }
                LOGI("quelen {} {} SetThreadSuspend {}",quelen, __FUNCTION__,true);
                siflag = 1;
            }
            else
            {
                //normal status nothing todo
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        else if (quelen <= 50)
        {
            if (siflag == 1)
            {
                //int arout = 0;
                //auto len = UsbProtoMsg::StartCollect(arout);
                //FPGARegisterWrite(arout, len);
                if(m_dataHandle.GetIsSimulateRunning())
                {
                m_dataHandle.SetTestThreadSuspend(false);
                m_dataHandle.m_cvRecv.notify_one();
                }
                else
                {
                    m_pUDPSocket->SetThreadSuspend(false);
                    m_pUDPSocket->m_cvRecv.notify_one();
                }
                LOGI("quelen {} {} SetThreadSuspend {}",quelen, __FUNCTION__,false);
                siflag = 0;
            }
            else
            {
                //normal status nothing todo
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
//        else
//        {
//            //normal status nothing todo
//            std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        }
    }

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
    m_dataHandle.EndThread();
    m_pDataCalculate->EndThread();
    m_pCapCaculate->EndThread();
    m_pCustomPlotWidgt->EndThread();
    EndThread();
}

void MainWindow::StopAllThread(void)
{
    m_dataHandle.StopThread();
    m_pDataCalculate->StopThread();
    m_pCapCaculate->StopThread();
    m_pCustomPlotWidgt->StopThread();
    StopThread();
}

bool MainWindow::LoadConfig(void)
{
    if (m_pSysConfig != nullptr)
    {
        m_pSysConfig->beginGroup("Workspace");
        restoreGeometry(m_pSysConfig->value("MainWindowGeometry").toByteArray());
        restoreState(m_pSysConfig->value("MainWindowState").toByteArray());
        m_pSysConfig->endGroup();

        if(m_pConfigurationWidgt != nullptr)
        {
            m_pConfigurationWidgt->LoadConfig(m_pSysConfig);
        }
        if (m_pCustomPlotWidgt != nullptr)
        {
            m_pCustomPlotWidgt->LoadConfig(m_pSysConfig);
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
        if(m_pConfigurationWidgt != nullptr)
        {
            m_pConfigurationWidgt->SaveConfig(m_pSysConfig);
        }

        if (m_pCustomPlotWidgt != nullptr)
        {
            m_pCustomPlotWidgt->SaveConfig(m_pSysConfig);
        }
        if (m_pTemperatureMonitorView != nullptr)
        {
            m_pTemperatureMonitorView->SaveConfig(m_pSysConfig);
        }
    }
    return false;
}


void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   if(m_pNotify)
   {
       m_pNotify->AdjustInViewport();
   }
}
//void MainWindow::mouseMoveEvent(QMouseEvent *event)
//{
//    QString pos =  QString("%1 , %2").arg(event->pos().x()).arg(event->pos().y());
//    qDebug()<<pos;
//}


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

void MainWindow::OnSearchLocalIp(void)
{
    if(m_pUDPSocket)
    {
        m_pUdpLocalIpInpComb->clear();
        m_pUdpLocalIpInpComb->addItems( m_pUDPSocket->OnSearchLocalIp());
    }
}


void MainWindow::CloseDataStream(void)
{
    if(m_pactStart->isChecked())
        m_pactStart->trigger();

    m_pUDPSocket->EndThread();
    m_pUDPSocket->StopThread();
}

void MainWindow::OnEnableUdpTestMode(const bool enable)
{
    m_usbFtdDevice.SwitchOnUdpTestMode(enable);
    m_pactStreamPrintMode->setChecked(enable);
}

void MainWindow::OnEnableStreamPrintMode(const bool enable)
{
    ConfigServer::GetInstance()->SetIsStreamPrint(enable);
}

void MainWindow::OnDispDeviceSpeed(const float &speed)
{
     m_labSpeedDisp.setText(QString("%1 MB/s").arg(QString::number(speed,'f',2)));
}
void MainWindow::OnSampleRateChangeSlot(void)
{
//    LOGCI("Sample rate {}",samprate);
    m_pCustomPlotWidgt->OnSampRateChange();

}

void MainWindow::LoadStyleSheet(const QString &style, QWidget* pObject)
{
    QFile styleSheet(style);
    if (!styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LOGCI("Can't open the style sheet file.");
        return;
    }
    if(pObject != NULL)
        pObject->setStyleSheet(styleSheet.readAll());
    else
        qApp->setStyleSheet(styleSheet.readAll());

    styleSheet.close();
}

void MainWindow::UpdateCurrentSensorGrpStrSlot(const QString &sensorGrpStr)
{
    m_pLabCurSensorDisp->setText(sensorGrpStr);
}

void MainWindow::SetCurrentTempSlot(const QString &tempstr)
{
    m_plcdTemperature->display(tempstr);
}

void MainWindow::OnShowTempureTool(void)
{
    if(m_pTemperatureMonitorView != nullptr)
    {
    m_pTemperatureMonitorView->show();
    ConfigServer::WidgetPosAdjustByParent(this, m_pTemperatureMonitorView);
    }
}

void MainWindow::UpdateRuntimeSlot(void)
{
    m_tmRunTimeRec = m_tmRunTimeRec.addSecs(1);
    m_plcdRunTimeRec->display(m_tmRunTimeRec.toString("hh:mm:ss"));
}

void MainWindow::StartOrStopRuntimer(const bool &bStart)
{
    if(bStart)
    {
        if (!m_pTmrRunTimeRec->isActive())
        {
            m_tmRunTimeRec.setHMS(0, 0, 0, 0);
            m_pTmrRunTimeRec->start(1000);
        }
    }
    else
    {
        if (m_pTmrRunTimeRec->isActive())
        {
            m_pTmrRunTimeRec->stop();
        }
    }
}

void MainWindow::AFEPowerConsumpChangeSlot(const POWER_CONSUMPTION_ENUM &flag)
{
    if(flag == ENUM_POWER_CONSUMPTION_LOW)
    {
        m_pStatusPowerConsumpLab->setText("<img src=:/img/img/power_low.png alt=BGI width=25 height=25>");
        m_pStatusPowerConsumpLab->setToolTip(QStringLiteral("AFE_Power_Consumption: Low!!"));
    }
    else if(flag == ENUM_POWER_CONSUMPTION_HIGH)
    {
        m_pStatusPowerConsumpLab->setText("<img src=:/img/img/power_high.png alt=BGI width=25 height=25>");
        m_pStatusPowerConsumpLab->setToolTip(QStringLiteral("AFE_Power_Consumption: High!!"));
    }
}
void MainWindow::OnTimerUpdateTempSlot(void)
{
    if (!m_tempSharedMem.isAttached())
    {
        if (!m_tempSharedMem.attach())
        {
#if 0
            if(m_tempSharedMem.error()!=QSharedMemory::NoError)
            {
                LOGE("Attach Error : {}",m_tempSharedMem.errorString().toStdString());
            }
#endif
            m_plcdTemperature->display(QStringLiteral("Error"));
            return;
        }
        else
        {
            m_tempSharedMem.lock();
            QByteArray arr((char*)m_tempSharedMem.data(), m_tempSharedMem.size());
            m_plcdTemperature->display(QString(arr));
            m_tempSharedMem.unlock();
            m_tempSharedMem.detach();
        }
    }
}
