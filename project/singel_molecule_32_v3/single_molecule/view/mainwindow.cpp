#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QWidget>
#include<QDesktopWidget>
#include"helper/stylesheethelper.h"
#include"logger/Log.h"
#include"helper/appconfig.h"
#include "helper/messagedialog.h"
#include "models/DataDefine.h"
#include "helper/version.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LOGI("Start single molecule.");
//    StyleSheetHelper::loadStyleSheet(QString(":/qss/ElegantDark.qss"));  //14
//    StyleSheetHelper::loadStyleSheet(QString(":/qss/aip001.css"));
//    StyleSheetHelper::loadStyleSheet(QString(":/qss/silvery.css"));  //14
//    StyleSheetHelper::loadStyleSheet(QString(":/qss/listWidget.css"));
    QString configPath = QString::fromStdString(AppConfig::getConfigFolder() + "calibration.ini");
    AppConfig::syncDefaultConfig(configPath);
    m_calibConfig = new QSettings(configPath, QSettings::IniFormat);

    QString nSettingPath = QString::fromStdString(AppConfig::getConfigFolder() + "setting.ini");
    AppConfig::syncDefaultConfig(nSettingPath);
    m_nSettingConfig = new QSettings(nSettingPath, QSettings::IniFormat);

    initWidget();

    m_pSerialPortControl = new SerialPortControl;
    m_pSerialPortOut = new SerialPortOut;
    m_pSerialPortOutThread = new QThread;
    m_pSerialPortOut->moveToThread(m_pSerialPortOutThread);
    m_pSerialPortOutThread->start();
    #ifndef DEMO_TEST
    m_pSerialPortControl->moveToThread(m_pSerialPortControl);
    #endif
    m_pSerialPortControl->start();

    m_pDataFunction = new DataFunction;
    m_pDataFunctionThread = new QThread;
    m_pDataFunction->startThread();
    m_pDataFunction->moveToThread(m_pDataFunctionThread);
    m_pDataFunctionThread->start();

    m_pDataRecord = new DataRecord;
    m_pDataRecordThread = new QThread;
    m_pDataRecord->moveToThread(m_pDataRecordThread);
    connect(m_pDataRecordThread, SIGNAL(finished()), m_pDataRecordThread, SLOT(deleteLater()));
    m_pDataRecordThread->start();


    m_pDataMonitor    = new DataMonitor;
    m_DataStackHandle = new DataStackHandle;
    m_pDataCollection = new DataCollection;
    m_pDataMeansCal   = new DataMeansCal;
    m_pDataCapCollection = new DataCapCollection;
    m_pDataCapCaculate = new DataCapCaculate;
    m_pDataOutputFormat = new DataOutputFormat;


    qRegisterMetaType<SerialSettingsDialog::Settings>("SerialSettingsDialog::Settings");
    connect(m_pSerialPortsettingsDialog,SIGNAL(openPortSignal(SerialSettingsDialog::Settings)),m_pSerialPortControl,SLOT(openPortSolt(SerialSettingsDialog::Settings)));
    connect(m_pSerialPortsettingsDialog,SIGNAL(openAoPortSignal(SerialSettingsDialog::Settings)),m_pSerialPortOut,SLOT(openPortSolt(SerialSettingsDialog::Settings)));
    connect(m_pSerialPortsettingsDialog,&SerialSettingsDialog::closePortSignal,m_pSerialPortControl,&SerialPortControl::closePort);
    connect(m_pSerialPortsettingsDialog,&SerialSettingsDialog::closePortSignal,m_pSerialPortOut,&SerialPortOut::closePort);
    connect(m_pCurrentSingelSettingPage, &CurrentSingelSettingPage::transmitData, m_pSerialPortOut, &SerialPortOut::writePortData);
    connect(m_pControlPanel,&ControlPanel::transmitData,m_pSerialPortOut,&SerialPortOut::writePortData);
    connect(m_pControlPanel,&ControlPanel::transmitAIData,m_pSerialPortControl,&SerialPortControl::writePortData);

    connect(this,SIGNAL(stopThread()),m_pSerialPortControl,SLOT(stopThread()));
    connect(this,SIGNAL(writeCmd(int)),m_pSerialPortControl,SLOT(writeCmdSlot(int)));
    connect(m_pSerialPortOutThread,SIGNAL(finished()),m_pSerialPortOut,SLOT(deleteLater()));
    connect(m_pSerialPortControl,&SerialPortControl::qMsgPortInfoSig,this,&MainWindow::recvqMsgPortInfo_Slot);
    connect(m_pSerialPortOut,&SerialPortOut::qMsgPortInfoSig,this,&MainWindow::recvqMsgPortInfo_Slot);
    qRegisterMetaType<QVector<float >>("QVector<float > ");
    qRegisterMetaType<QVector<QVector<float>> >("QVector<QVector<float>> &");
    connect(m_pDataCollection,&DataCollection::dispInstantCurrentSig,m_pCurrentDisplayView,&CurrentDisplayView::dispInstantSlot);
    connect(m_pDataMeansCal,&DataMeansCal::MeansResultSig,m_pCurrentDisplayView,&CurrentDisplayView::dispMeansSlot);
    connect(m_pDataMeansCal,&DataMeansCal::CaliMeanstResSig,m_pCalibrationdialog,&Calibrationdialog::meansResultSlot);
    connect(m_pDataMeansCal,&DataMeansCal::DegatingBufSig,m_pDataFunction,&DataFunction::AnalyDegatingChannel);
    connect(m_pDataFunction,&DataFunction::DegateResultSig,m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::recDegatResultSlot);
    connect(m_pCalibrationdialog,SIGNAL(CalibrationFinished(bool)), m_DataStackHandle,SLOT(updateFormula(bool)));
    connect(m_pDataOutputFormat,&DataOutputFormat::sendSaveDataBufSig,m_pDataRecord,&DataRecord::SaveDataSlot);
    connect(m_pSaveFIlePanel,&SaveFIlePanel::saveFilePathSig,m_pDataRecord,&DataRecord::updateSavePathSlot);
    connect(m_pSaveFIlePanel,&SaveFIlePanel::saveFileStopSig,m_pDataRecord,&DataRecord::closeFile);
    connect(m_pControlPanel,&ControlPanel::SendSampRateSig,m_pSaveFIlePanel,&SaveFIlePanel::UpdateSampRateSlot);

    connect(m_pControlPanel,&ControlPanel::setZeroAdjustSig,m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::SetZeroAdjustSlot);
    connect(m_pDataMeansCal,&DataMeansCal::ZeroOffsetResSig,m_DataStackHandle,&DataStackHandle::zeroOffsetAdjustSlot);
    connect(m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::setDegatingParameter,m_pDataFunction,&DataFunction::setDegatingParameterSlot);
    connect(m_pCapPanelView,&CapPanelView::setTrangularWave,m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::setCapTrangularWaveSlot);

    connect(m_pDataFunction,&DataFunction::destroyed,m_pDataFunctionThread,&QThread::quit);
    connect(m_pDataFunctionThread,&QThread::finished,m_pDataFunctionThread,&QThread::deleteLater);
    connect(m_pDataFunctionThread,&QThread::finished,this,&MainWindow::dataFunctionFinished);  //为了看de-gating 失效时是否是线程崩掉
    connect(m_pDataFunctionThread,&QThread::destroyed,this,&MainWindow::dataFunctionFinished);//为了看de-gating 失效时是否是线程崩掉

    connect(m_pControlPanel, &ControlPanel::StopRunningAndSaveSig, this, &MainWindow::StopRunnningAndSaveSlot);


    intiSetting();
//    m_pSerialPortsettingsDialog->autoConnect();

    setWindowTitle(SOFTWARE_TITLE);
}

MainWindow::~MainWindow()
{
    on_actionStop_triggered();

    #ifndef DEMO_TEST
        emit stopThread();
    #else
        m_pSerialPortControl->stopThread();
    #endif

    m_pSerialPortControl->wait();
    m_pSerialPortOutThread->quit();
    m_pSerialPortOutThread->wait();

    m_pDataFunction->stopThread();
    if(m_pDataFunctionThread->isRunning())
    {
        m_pDataFunctionThread->quit();
        m_pDataFunctionThread->wait();
    }

    m_DataStackHandle->stopThread();
    m_DataStackHandle->wait();

    m_pDataRecordThread->quit();
    m_pDataRecordThread->wait();

    //等待线程结束，否则出错；
    m_pDataCollection->wait();
    m_pDataMeansCal->wait();
    m_pDataCapCollection->wait();
    m_pDataCapCaculate->wait();
    m_pDataOutputFormat->wait();

    m_pCurrentSingelSettingPage->onTriangularTestStop();

    SaveSetting();
    delete m_pSerialPortControl;
    delete m_pDataMonitor;
    delete m_DataStackHandle;
    delete m_pDataCollection;
    delete m_pDataMeansCal;
    delete m_pDataCapCollection;
    delete m_pDataCapCaculate;
    delete m_pDataOutputFormat;
    delete m_pDataRecord;


    delete m_pCurrcrnttWidget;
    delete m_pCapPanelView;
    delete m_pLogPage;
    delete m_pSerialPortsettingsDialog;
    delete m_pCurrentSingelSettingPage;
    delete m_pCurrentDisplayView;
    delete m_pCalibrationdialog;
    delete m_pSaveFIlePanel;
    delete m_pControlPanel;

    delete m_calibConfig;
    delete m_nSettingConfig;
    delete ui;
}



void MainWindow::LoadConfig(QSettings *config)
{
    config->beginGroup("Workspace");
    restoreGeometry(config->value("MainWindowGeometry").toByteArray());
    restoreState(config->value("MainWindowState").toByteArray());
    config->endGroup();

}

void MainWindow::SaveConfig(QSettings *config)
{
    config->beginGroup("Workspace");
    config->setValue("MainWindowGeometry", saveGeometry());
    config->setValue("MainWindowState", saveState());
    config->endGroup();
}


void MainWindow::intiSetting()
{
    if(m_nSettingConfig)
        this->LoadConfig(m_nSettingConfig);
    if(m_pSerialPortsettingsDialog && m_nSettingConfig)
        m_pSerialPortsettingsDialog->LoadConfig(m_nSettingConfig);
    if(m_pCurrcrnttWidget && m_nSettingConfig)
        m_pCurrcrnttWidget->LoadConfig(m_nSettingConfig);
    if(m_pCurrentDisplayView && m_nSettingConfig)
        m_pCurrentDisplayView->LoadConfig(m_nSettingConfig);
    if(m_pCurrentSingelSettingPage && m_nSettingConfig)
        m_pCurrentSingelSettingPage->LoadConfig(m_nSettingConfig);

    if(m_DataStackHandle && m_calibConfig)
        m_DataStackHandle->LoadConfig(m_calibConfig);
    if(m_pCalibrationdialog && m_calibConfig)
        m_pCalibrationdialog->loadConfig(m_calibConfig);
}

void MainWindow::SaveSetting()
{
    if(m_nSettingConfig)
        this->SaveConfig(m_nSettingConfig);
    if(m_pCurrcrnttWidget && m_nSettingConfig)
        m_pCurrcrnttWidget->SaveConfig(m_nSettingConfig);
    if(m_pCurrentDisplayView && m_nSettingConfig)
        m_pCurrentDisplayView->SaveConfig(m_nSettingConfig);
    if(m_pCurrentSingelSettingPage && m_nSettingConfig)
        m_pCurrentSingelSettingPage->SaveConfig(m_nSettingConfig);

}

void MainWindow::initWidget()
{
    initRunTimeView();

    setDockNestingEnabled(true);
    takeCentralWidget();

    QDockWidget *m_pDockPlotView   =new QDockWidget(tr("Plot"),this);
    QDockWidget *m_pCapDockView    =new QDockWidget(tr("Cap"),this);
    QDockWidget *m_pDockLogView    =new QDockWidget(tr("Log"),this);
    QDockWidget *m_pDockVoltView   =new QDockWidget(tr("Volt"),this);
    QDockWidget *m_pDockDispView   =new QDockWidget(tr("Current"),this);
    QDockWidget *m_pDockSaveView   =new QDockWidget(tr("Save"),this);
    QDockWidget *m_pDockControlView=new QDockWidget(tr("Control"),this);


    m_pDockPlotView->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_pDockPlotView->setObjectName("Plot");
    m_pCurrcrnttWidget = new QPlotwidget();
    m_pDockPlotView->setWidget(m_pCurrcrnttWidget);
    addDockWidget(Qt::LeftDockWidgetArea,m_pDockPlotView);

    m_pCapDockView->setFeatures(QDockWidget::AllDockWidgetFeatures); //具有全部特性
    m_pCapDockView->setObjectName("Cap");
    m_pCapPanelView = new CapPanelView();
    m_pCapDockView->setWidget(m_pCapPanelView);
    tabifyDockWidget(m_pDockPlotView,m_pCapDockView);

    m_pDockLogView->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_pDockLogView->setObjectName("LogView");
    m_pLogPage = new LogPage();
    m_pDockLogView->setWidget(m_pLogPage);
    addDockWidget(Qt::LeftDockWidgetArea,m_pDockLogView);

    m_pDockSaveView->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_pDockSaveView->setObjectName("Save");
    m_pSaveFIlePanel = new SaveFIlePanel();
    m_pDockSaveView->setWidget(m_pSaveFIlePanel);
    tabifyDockWidget(m_pDockLogView,m_pDockSaveView);

    m_pDockControlView->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_pDockControlView->setObjectName("Control");
    m_pControlPanel = new ControlPanel();
    m_pDockControlView->setWidget(m_pControlPanel);
    tabifyDockWidget(m_pDockLogView,m_pDockControlView);


    m_pDockDispView->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    m_pDockDispView->setObjectName("Current");
    m_pCurrentDisplayView = new CurrentDisplayView();
    m_pDockDispView->setWidget(m_pCurrentDisplayView);
    addDockWidget(Qt::RightDockWidgetArea,m_pDockDispView);


    m_pDockVoltView->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    m_pDockVoltView->setObjectName("VoltView");
    m_pCurrentSingelSettingPage = new CurrentSingelSettingPage();
    m_pDockVoltView->setWidget(m_pCurrentSingelSettingPage);
    splitDockWidget(m_pDockDispView,m_pDockVoltView,Qt::Horizontal);

    m_pCalibrationdialog = new Calibrationdialog(ui->actionStart,ui->actionStop,ui->actionClearPort, this);

    connect(m_pCurrentDisplayView,&CurrentDisplayView::setChannelZeroSig,m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::setSpecChanelDirect);
    connect(m_pCalibrationdialog,&Calibrationdialog::caliDirectVoltSetSig,m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::setAllChannelDirect);
    connect(m_pCurrentDisplayView,&CurrentDisplayView::GraphColorChangeSig,m_pCurrcrnttWidget,&QPlotwidget::GraphColorChangeSlot);
    connect(m_pCurrentDisplayView,&CurrentDisplayView::GraphVisibleSig,m_pCurrcrnttWidget,&QPlotwidget::GraphVisibleSlot);
    connect(m_pCurrentDisplayView,&CurrentDisplayView::GraphAllVisibleSig,m_pCurrcrnttWidget,&QPlotwidget::GraphAllVisibleSlot);
    connect(m_pCurrentDisplayView,&CurrentDisplayView::ListItemVisableSig,m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::setListItemVisiableSlot);

    connect(m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::setPoreInsertionWorkSig,m_pCurrentDisplayView,&CurrentDisplayView::setPoreInsertionWorkSlot);
    connect(m_pCurrentSingelSettingPage,&CurrentSingelSettingPage::getPoreStateBackSig,m_pCurrentDisplayView,&CurrentDisplayView::getPoreStateBackSlot);
}

void MainWindow::on_actionCheck_port_triggered()
{
    m_pSerialPortsettingsDialog->show();
}

void MainWindow::on_actionOpen_data_folder_D_triggered()
{
    QString filePath = QString::fromStdString(AppConfig::getDataFolder());
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::on_actionOpen_log_folder_L_triggered()
{
    QString filePath = QString::fromStdString(AppConfig::getLogFolder());
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::on_actionAbout_triggered()
{
    AboutBox aboutBox(this);
    aboutBox.exec();
}

void MainWindow::on_actionStart_triggered()
{
    LOGI("Start collecting data.");
    g_pCommunicaor->reset();

    startRunTimeDisplay();
    m_DataStackHandle->startThread();
    m_pDataMonitor->startThread();
    m_pCurrcrnttWidget->startThread();
    m_pDataCollection->startThread();
    m_pDataMeansCal->startThread();
    m_pDataCapCollection->startThread();
    m_pDataCapCaculate->startThread();
    m_pDataOutputFormat ->startThread();
//    m_pDataRecord->startThread();

    #ifdef DEMO_TEST
    m_pSerialPortControl->start();
    #endif


//    m_pSerialPortControl->writeCmdSlot(CMD_START);
    emit writeCmd(CMD_STOP);
    emit writeCmd(CMD_START);
    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);

}



void MainWindow::on_actionStop_triggered()
{
    #ifndef DEMO_TEST
        emit writeCmd(CMD_STOP);
    #else
        emit writeCmd(CMD_STOP);
    #endif
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);

    stopRunTimeDisplay();
    //结束接收栈线程
    m_DataStackHandle->stopThread();
    m_pDataMonitor->stopThread();
    m_pCurrcrnttWidget->stopThread();
    m_pDataCollection->stopThread();
    m_pDataMeansCal->stopThread();
    m_pDataCapCollection->stopThread();
    m_pDataCapCaculate->stopThread();
    m_pDataOutputFormat ->stopThread();
//    m_pDataRecord->stopThread();

    m_pSaveFIlePanel->onFileSettingEnable(true);  //运行前可以选择文件路径
    m_pCurrentSingelSettingPage->onTriangularTestStop();

}

void MainWindow::on_actionCalibration_triggered()
{
    m_pCalibrationdialog->show();
}

void MainWindow::on_actionClearPort_triggered()
{
    m_pSerialPortControl->ClearPort();
}


void MainWindow::recvqMsgPortInfo_Slot(QString info)
{
    QMessageBox::critical(this,"Port Error ",info,QMessageBox::Ok);
}


void MainWindow::initRunTimeView(void)
{
    m_runTimeRecord.setHMS(0,0,0,0);
    m_runTimeLCDNumber.setDigitCount(8);
    m_runTimeLCDNumber.setSegmentStyle(QLCDNumber::Flat);
    m_runTimeLCDNumber.setFrameShape(QFrame::NoFrame);
    m_runTimeLCDNumber.display(m_runTimeRecord.toString("hh:mm:ss"));
    m_runTimeLab.setText(QStringLiteral("Run Time: "));

    connect(&m_runSecTimer,SIGNAL(timeout()),this,SLOT(updataRunTimeDisplay()));
    ui->statusbar->addPermanentWidget(&m_runTimeLab,0);
    ui->statusbar->addPermanentWidget(&m_runTimeLCDNumber,0);
}
void MainWindow::updataRunTimeDisplay(void)
{
    m_runTimeRecord = m_runTimeRecord.addSecs(1);
    m_runTimeLCDNumber.display(m_runTimeRecord.toString("hh:mm:ss"));
}

void MainWindow::startRunTimeDisplay(void)
{
    m_runTimeRecord.setHMS(0,0,0,0);
    m_runSecTimer.start(1000);
}

void MainWindow::stopRunTimeDisplay(void)
{
    if(m_runSecTimer.isActive())
    {
        m_runSecTimer.stop();
    }
}

void MainWindow::dataFunctionFinished(void)
{
    LOGI("quit dataFunctionFinished.");
    qDebug()<<"quit dataFunctionFinished.";
//    QMessageBox::critical(this,"thread Error ","Quit DataFunction.",QMessageBox::Ok);
}

void MainWindow::StopRunnningAndSaveSlot(void)
{
    on_actionStop_triggered();
    m_pSaveFIlePanel->on_stopSaveBtn_clicked();
}
