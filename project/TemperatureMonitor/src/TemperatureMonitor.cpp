#include "TemperatureMonitor.h"
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QSpacerItem>
#include <QAbstractItemView>
#include <QStackedWidget>
#include <QTextEdit>
#include <QRadioButton>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QTextCodec>
#include <QTimer>
#include <QTime>
#include <QDoubleSpinBox>
#include <QLCDNumber>
#include "LogPane.h"
#include "Log.h"
#include "SerialPort.h"

#include <iostream>
using namespace std;
const std::string PROGRAME_PASSWORD = {"WTG30"};
static const QString WINDOWS_CUSTOM_PORT_NAME = "Prolific USB-to-Serial Comm Port";
static const QString LINUX_CUSTOM_PORT_NAME = "USB-Serial Controller";
#define TEMP_VIEW_CONTROL_INDEX 0
#define TEMP_VIEW_CURVE_INDEX   1
#define TEMP_VIEW_LOG_INDEX   2




TemperatureMonitor::TemperatureMonitor(QWidget* parent) : QWidget(parent),m_tempSharedMem("CycloneG30TempSharedMem")
{
    InitConfig();
    InitPortCtrl();
    LoadConfig(m_config);
}

TemperatureMonitor::~TemperatureMonitor()
{
    SaveConfig(m_config);

    OnPortConnectBtnSlot(false);
    UpLoadStrToSharedMem(QStringLiteral("Off"));

    m_pSerialPortThread->quit();
    m_pSerialPortThread->wait();
    m_pLogPane->EndThread();
    m_pLogPane->StopThread();

    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }
    if (m_tempLogFile.isOpen())
    {
        m_tempLogFile.close();
    }

    if(m_pTempCustomPlot)
    {
        delete m_pTempCustomPlot;
        m_pTempCustomPlot = nullptr;
    }
}

void TemperatureMonitor::InitConfig(void)
{

#ifdef Q_OS_WIN32
    std::string strConfpath = QDir::currentPath().toStdString()+ "/etc/Conf/";
    std::cout<<"strConfpath " <<strConfpath<<" "<<std::endl;
#else
//    std::string strConfpath = "~/etc/Conf/";
    std::string strConfpath = QDir::currentPath().toStdString()+ "/etc/Conf/";
#endif

    std::filesystem::path pathdatapath(strConfpath);
    if (!std::filesystem::exists(pathdatapath))
    {
        std::filesystem::create_directories(pathdatapath);
    }
    std::string strconfig = strConfpath+ "TemperConfig.ini";
    std::cout<< strconfig;
    m_config = new QSettings(QString::fromStdString(strconfig), QSettings::IniFormat, this);
}

void TemperatureMonitor::LoadConfig(QSettings* config)
{
    if (config)
    {
        config->beginGroup("Tempertarue");
        m_pCaliFactDpbox->setValue(config->value("CalibrationFactor").toDouble());
        config->endGroup();
    }
}

void TemperatureMonitor::SaveConfig(QSettings* config)
{
    if (config)
    {
        config->beginGroup("Tempertarue");
        config->setValue("CalibrationFactor", m_pCaliFactDpbox->value());
        config->endGroup();
    }
}

/**
 * @brief 初始化串口UI
 */
void TemperatureMonitor::InitPortCtrl(void)
{
    m_dFactorParam = 0;
    m_curTempStr = "0.0";
    m_targetTempStr = "0.0";

    m_pSearchPortGpbox = new QGroupBox(QStringLiteral("PortControl"), this);
    m_pSearchPortNameLab = new QLabel(QStringLiteral("Port"), m_pSearchPortGpbox);
    m_pSearchPortComb = new QComboBox(m_pSearchPortGpbox);
    m_pPortBaudRateLab = new QLabel(QStringLiteral("Baud Rate"), m_pSearchPortGpbox);
    m_pPortBaudRateComb = new QComboBox(m_pSearchPortGpbox);
    m_pPortBaudRateComb->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_pPortBaudRateComb->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_pPortBaudRateComb->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_pPortBaudRateComb->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    m_pPortSearchBtn = new QPushButton(QStringLiteral("Search"), m_pSearchPortGpbox);
    m_pPortConnectBtn = new QPushButton(tr("Open"), m_pSearchPortGpbox);
    m_pPortSettingBtn = new QPushButton(tr("Setting"), m_pSearchPortGpbox);
    m_pSwithViewComb = new QComboBox(m_pSearchPortGpbox);
    m_pSwithViewComb->addItem(QStringLiteral("Command View"));
    m_pSwithViewComb->addItem(QStringLiteral("Temperatrue View"));
    m_pSwithViewComb->addItem(QStringLiteral("Log View"));

    m_pPortSearchBtn->setIcon(QIcon(":/img/img/search.png"));
    m_pPortConnectBtn->setIcon(QIcon(":img/img/connect.ico"));
    m_pPortConnectBtn->setCheckable(true);

    QHBoxLayout* horlayout = new QHBoxLayout();
    horlayout->addWidget(m_pSearchPortNameLab);
    horlayout->addWidget(m_pSearchPortComb);
    horlayout->addWidget(m_pPortBaudRateLab);
    horlayout->addWidget(m_pPortBaudRateComb);
    horlayout->addWidget(m_pPortSearchBtn);
    horlayout->addWidget(m_pPortConnectBtn);
    horlayout->addWidget(m_pPortSettingBtn);
    horlayout->addWidget(m_pSwithViewComb);

    horlayout->setSpacing(10);
    horlayout->addStretch();
    m_pSearchPortGpbox->setLayout(horlayout);

    m_pInteractViewGpbox = new QGroupBox(QStringLiteral("Function View"), this);
    m_pViewStackWidget = new QStackedWidget(m_pInteractViewGpbox);
    m_pViewStackWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout* vertStacklayout = new QVBoxLayout();
    vertStacklayout->addWidget(m_pViewStackWidget);
    m_pInteractViewGpbox->setLayout(vertStacklayout);

    QVBoxLayout* vertlayout = new QVBoxLayout();
    vertlayout->addWidget(m_pSearchPortGpbox);
    vertlayout->addWidget(m_pInteractViewGpbox);
    setLayout(vertlayout);

    InitInteractView();
    InitTempDispView();
    m_pLogPane = new LogPane(this);
    m_pViewStackWidget->addWidget(m_pLogPane);
    m_pLogPane->StartThread();


    connect(m_pPortSearchBtn, SIGNAL(clicked()), this, SLOT(ScanPort()));
    connect(m_pPortSettingBtn, SIGNAL(clicked()), this, SLOT(OnPortSettingBtnSlot()));
    connect(m_pPortConnectBtn,&QPushButton::toggled,this,&TemperatureMonitor::OnPortConnectBtnSlot);
    //    connect(m_pSwithViewComb,static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),\
    //          [=](int index){m_pViewStackWidget->setCurrentIndex(index);});
    connect(m_pSwithViewComb, SIGNAL(currentIndexChanged(int)), SLOT(OnSwichFunctViewChange(int)));

    m_pSerialPort = new SerialPort();
    m_pSerialPort->InitLogFile(&m_logFile);
    m_pSerialPort->InitTempLogFile(&m_tempLogFile);

    m_pPortSettingDialog = new SerialSettingDialog(this);
    m_pSerialPortThread = new QThread();
    m_pSerialPort->moveToThread(m_pSerialPortThread);
    m_pSerialPortThread->start();

    connect(m_pSerialPortThread, SIGNAL(finished()), m_pSerialPort, SLOT(deleteLater()));
    connect(m_pSerialPortThread, SIGNAL(finished()), m_pSerialPortThread, SLOT(deleteLater()));
    connect(m_pSerialPort, SIGNAL(UpdateSerialRcvInfoSig(const QString, const QByteArray)), this, SLOT(UpdateSerialRcvInfoSlot(const QString, const QByteArray)));
    connect(m_pSerialPort, SIGNAL(UpdateTemperaInfoSig(const QString, const double, const double)), this, SLOT(UpdateTemperaInfoSlot(const QString, const double, const double)));
    connect(this, SIGNAL(transmitData(const QByteArray&)), m_pSerialPort, SLOT(WritePortData(const QByteArray&)));
    m_pSwithViewComb->setCurrentIndex(TEMP_VIEW_CURVE_INDEX);
    m_pViewStackWidget->setCurrentIndex(TEMP_VIEW_CURVE_INDEX);

}


/**
 * @brief 初始化串口收发UI
 */
void TemperatureMonitor::InitInteractView(void)
{
    m_pSerialTextrrView = new QWidget(m_pViewStackWidget);

    QGroupBox* rxGroupbox = new QGroupBox(QStringLiteral("Recive"), m_pSerialTextrrView);
    QGroupBox* txGroupbox = new QGroupBox(QStringLiteral("Send"), m_pSerialTextrrView);
    m_pRecvDataTextEdit = new QTextEdit(rxGroupbox);
    m_pSendDataTextEdit = new QTextEdit(txGroupbox);
    m_pRxClearBtn = new QPushButton(QStringLiteral("Clear"), rxGroupbox);

    m_pLogToFileChk = new QCheckBox(QStringLiteral("Log file:"), rxGroupbox);
    m_pLogFilePathEdit = new QLineEdit(rxGroupbox);
    m_pLogFileLoacatBtn = new QPushButton(QStringLiteral("..."), rxGroupbox);
    m_pLogFileAppendChk = new QCheckBox(QStringLiteral("append"), rxGroupbox);
    m_pLogFilePathEdit->setText(QDir::currentPath());
    m_pLogFileLoacatBtn->setFixedSize(30, 20);

    m_pRxMaxlineCountLab = new QLabel(tr("Max Record:"), rxGroupbox);
    m_pRxMaxlineCountSpbox = new QSpinBox();
    m_pRxMaxlineCountSpbox->setMinimum(10);
    m_pRxMaxlineCountSpbox->setMaximum(10000);
    m_pRxMaxlineCountSpbox->setValue(2000);
    m_pRxMaxlineCountSpbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pRxMaxlineCountSpbox->setMaximumWidth(100);

    QHBoxLayout* horLogfilelyout = new QHBoxLayout();
    horLogfilelyout->addWidget(m_pLogToFileChk);
    horLogfilelyout->addWidget(m_pLogFilePathEdit);
    horLogfilelyout->addWidget(m_pLogFileLoacatBtn);
    horLogfilelyout->addWidget(m_pLogFileAppendChk);

    QHBoxLayout* horcoutlayout = new QHBoxLayout();
    horcoutlayout->addWidget(m_pRxMaxlineCountLab);
    horcoutlayout->addWidget(m_pRxMaxlineCountSpbox);

    m_pTxClearBtn = new QPushButton(QStringLiteral("Clear"), txGroupbox);
    m_pTxSendBtn = new QPushButton(QStringLiteral("Send"), txGroupbox);
    m_pTxHistoryComb = new QComboBox(txGroupbox);
    m_pTxFormaTailComb = new QComboBox(txGroupbox);
    m_pRxSettingsGpbox = new QGroupBox(QStringLiteral("Rx Settings"), m_pSerialTextrrView);
    m_pTxSettingsGpbox = new QGroupBox(QStringLiteral("Tx Settings"), m_pSerialTextrrView);
    m_pTxAsciiRaBtn = new QRadioButton(QStringLiteral("ASCII"), m_pTxSettingsGpbox);
    m_pRxAsciiRaBtn = new QRadioButton(QStringLiteral("ASCII"), m_pRxSettingsGpbox);
    m_pTxHexRaBtn = new QRadioButton(QStringLiteral("Hex"), m_pTxSettingsGpbox);
    m_pRxHexRaBtn = new QRadioButton(QStringLiteral("Hex"), m_pRxSettingsGpbox);
    m_pIsTimerToSend = new QCheckBox(QStringLiteral("Resend"), m_pTxSettingsGpbox);
    m_pTimerToSendSpbox = new QSpinBox(m_pTxSettingsGpbox);
    m_pResendTimer = new QTimer(this);

    m_pTimerToSendSpbox->setSuffix(QStringLiteral(" ms"));
    m_pTimerToSendSpbox->setMaximum(10000);
    m_pTimerToSendSpbox->setMinimum(100);
    m_pTimerToSendSpbox->setValue(1000);

    m_pRecvDataTextEdit->setReadOnly(true);
    m_pRxAsciiRaBtn->setChecked(true);
    m_pTxAsciiRaBtn->setChecked(true);

    m_pTxFormaTailComb->addItem("CR/LF");

    QHBoxLayout* hRxlayout = new QHBoxLayout();
    hRxlayout->addWidget(m_pRxAsciiRaBtn);
    hRxlayout->addWidget(m_pRxHexRaBtn);
    m_pRxSettingsGpbox->setLayout(hRxlayout);
    //    m_pRxSettingsGpbox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    //    m_pTxSettingsGpbox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    m_pTxHistoryComb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout* hRxBtnlayout = new QHBoxLayout();
    hRxBtnlayout->addLayout(horLogfilelyout);
    hRxBtnlayout->addStretch();
    hRxBtnlayout->addLayout(horcoutlayout);
    hRxBtnlayout->addWidget(m_pRxClearBtn, 0, Qt::AlignRight);


    QVBoxLayout* vRxRcvlayout = new QVBoxLayout();
    vRxRcvlayout->addWidget(m_pRecvDataTextEdit);
    vRxRcvlayout->addLayout(hRxBtnlayout);
    rxGroupbox->setLayout(vRxRcvlayout);

    QHBoxLayout* hTxBtnlayout = new QHBoxLayout();
    hTxBtnlayout->addWidget(m_pTxHistoryComb);
    //    hTxBtnlayout->addStretch();
    hTxBtnlayout->addWidget(m_pTxClearBtn, 0, Qt::AlignRight);
    hTxBtnlayout->addWidget(m_pTxSendBtn, 0, Qt::AlignRight);
    hTxBtnlayout->addWidget(m_pTxFormaTailComb, 0, Qt::AlignRight);


    QHBoxLayout* hTxFormlayout = new QHBoxLayout();
    hTxFormlayout->addWidget(m_pTxAsciiRaBtn);
    hTxFormlayout->addWidget(m_pTxHexRaBtn);

    QHBoxLayout* hTxResendlayout = new QHBoxLayout();
    hTxResendlayout->addWidget(m_pIsTimerToSend);
    hTxResendlayout->addWidget(m_pTimerToSendSpbox);

    QVBoxLayout* vTxSelayout = new QVBoxLayout();
    vTxSelayout->addLayout(hTxFormlayout);
    vTxSelayout->addLayout(hTxResendlayout);
    m_pTxSettingsGpbox->setLayout(vTxSelayout);

    QVBoxLayout* vSetlayout = new QVBoxLayout();
    vSetlayout->addWidget(m_pRxSettingsGpbox);
    vSetlayout->addWidget(m_pTxSettingsGpbox);

    QVBoxLayout* vTxSeBtlayout = new QVBoxLayout();
    vTxSeBtlayout->addWidget(m_pSendDataTextEdit);
    vTxSeBtlayout->addLayout(hTxBtnlayout);
    txGroupbox->setLayout(vTxSeBtlayout);

    QHBoxLayout* hTxlayout = new QHBoxLayout();
    hTxlayout->addLayout(vSetlayout);
    hTxlayout->addWidget(txGroupbox);

    QVBoxLayout* vTextrrlayout = new QVBoxLayout();
    vTextrrlayout->addWidget(rxGroupbox);
    vTextrrlayout->addLayout(hTxlayout);
    //    QSpacerItem *vertSpacer = new QSpacerItem(50,50, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //    vTextrrlayout->addItem(vertSpacer);

    m_pSerialTextrrView->setLayout(vTextrrlayout);
    m_pViewStackWidget->addWidget(m_pSerialTextrrView);



    connect(m_pRxClearBtn, &QPushButton::clicked, [this]() {m_pRecvDataTextEdit->clear(); });
    connect(m_pTxClearBtn, &QPushButton::clicked, [this]() {m_pSendDataTextEdit->clear(); });
    connect(m_pTxSendBtn, &QPushButton::clicked, this, &TemperatureMonitor::OnSendButtonClickedSlot);
    connect(m_pResendTimer, SIGNAL(timeout()), this, SLOT(ResendTimerSlot()));
    connect(m_pIsTimerToSend, SIGNAL(toggled(bool)), this, SLOT(OnPeriodSendToggleSlot(bool)));
    connect(m_pLogFileLoacatBtn, SIGNAL(clicked()), this, SLOT(OnChooseLogFileBtnSlot()));
    connect(m_pLogToFileChk, SIGNAL(toggled(bool)), SLOT(OnEnableLoggingSlot(bool)));

}


/**
 * @brief 初始化温度显示UI
 */
void TemperatureMonitor::InitTempDispView(void)
{
    m_pTempViewWidget = new QWidget(m_pViewStackWidget);
//    m_config = nullptr;
    m_tempTableRawCnt = 0;
    m_pTempCtrlGpbox = new QGroupBox(QStringLiteral("Temperatrue Control"), m_pTempViewWidget);


    m_pTempCurDescrLab = new QLabel(QStringLiteral("当前温度:"), m_pTempCtrlGpbox);
    m_pTempCurDispLcd = new QLCDNumber(m_pTempCtrlGpbox);
    m_pTempCurDescrUnitrLab = new QLabel(QStringLiteral("℃"), m_pTempCtrlGpbox);

    m_pTempCurTargetDescrLab = new QLabel(QStringLiteral("目标温度:"), m_pTempCtrlGpbox);;
    m_pTempCurTargetDescrLcd = new QLCDNumber(m_pTempCtrlGpbox);
    m_pTempCurTargetUnitrLab = new QLabel(QStringLiteral("℃"), m_pTempCtrlGpbox);
    m_pTempCurTargetDescrLab->setVisible(false);
    m_pTempCurTargetDescrLcd->setVisible(false);
    m_pTempCurTargetUnitrLab->setVisible(false);

    m_pTempTargDescrLab = new QLabel(QStringLiteral("设定目标温度:"), m_pTempCtrlGpbox);
    m_pTempTargSetDpbox = new QDoubleSpinBox(m_pTempCtrlGpbox);
    m_pTempTargSetDpbox->setSuffix(QStringLiteral(" ℃"));
    m_pTempTargSetDpbox->setValue(28);
    m_pTempTargetSetBtn = new QPushButton(QStringLiteral("Apply"), m_pTempCtrlGpbox);

    m_pCaliFactDescrLab = new QLabel(QStringLiteral("当前校准系数A:"), m_pTempCtrlGpbox);
    m_pCaliFactDpbox = new QDoubleSpinBox(m_pTempCtrlGpbox);
    m_pCaliFactChgeBtn = new QPushButton(QStringLiteral("更改校准系数A"), m_pTempCtrlGpbox);
    m_pCaliFactDpbox->setValue(1);
    m_pCaliFactDpbox->setReadOnly(true);
    m_pCaliFactDpbox->setEnabled(false);


    m_pTempRecTabGpbox = new QGroupBox(QStringLiteral("Record Table"), m_pTempViewWidget);

    m_pRecMaxlineCountLab = new QLabel(QStringLiteral("Max Record:"), m_pTempRecTabGpbox);
    m_pRecMaxlineCountSpbox = new QSpinBox(m_pTempRecTabGpbox);
    m_pRecMaxlineCountSpbox->setMinimum(100);
    m_pRecMaxlineCountSpbox->setMaximum(10000);
    m_pRecMaxlineCountSpbox->setValue(2000);
    m_pRecMaxlineCountSpbox->setReadOnly(true);
    m_pTempRecTableWdget = new QTableWidget(m_pRecMaxlineCountSpbox->value(),2,m_pTempRecTabGpbox);
    m_pTempRecTabClearBtn = new QPushButton(QStringLiteral("Clear"), m_pTempRecTabGpbox);

    m_pTempLogToFileChk = new QCheckBox(QStringLiteral("Log"), m_pTempRecTabGpbox);
    m_pTempLogFilePathEdit = new QLineEdit(m_pTempRecTabGpbox);
    m_pTempLogFileLoacatBtn = new QPushButton(QStringLiteral("..."), m_pTempRecTabGpbox);
    m_pTempLogFileAppendChk = new QCheckBox(QStringLiteral("Append"), m_pTempRecTabGpbox);
    m_pTempLogFileLoacatBtn->setFixedSize(20, 20);


    // lcd 初始化
    m_pTempCurDispLcd->setDigitCount(5);    //显示位数
    m_pTempCurDispLcd->setMode(QLCDNumber::Dec);    //十进制
    m_pTempCurDispLcd->setSegmentStyle(QLCDNumber::Flat);

    m_pTempCurTargetDescrLcd->setDigitCount(5);    //显示位数
    m_pTempCurTargetDescrLcd->setMode(QLCDNumber::Dec);    //十进制
    m_pTempCurTargetDescrLcd->setSegmentStyle(QLCDNumber::Flat);
    // 表格初始化
    m_pTempRecTableWdget->setColumnCount(2);
    m_pTempRecTableWdget->setAlternatingRowColors(true);     //设置隔行变颜色
    m_pTempRecTableWdget->setSelectionBehavior(QAbstractItemView::SelectRows);       //选中整行
    m_pTempRecTableWdget->verticalHeader()->setVisible(false);
    //设置表头
    QStringList header;
    header << QString::fromLocal8Bit("时间") << QString::fromLocal8Bit("温度℃");
    m_pTempRecTableWdget->setHorizontalHeaderLabels(header);
    m_pTempRecTableWdget->setColumnWidth(0, 120);
    m_pTempRecTableWdget->setColumnWidth(1, 50);
    m_pTempRecTableWdget->setFixedWidth(200);
    m_pTempRecTableWdget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //m_pTempRecTabGpbox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    QHBoxLayout* thorlayout = new QHBoxLayout();
    QGridLayout* tDispgridlayout = new QGridLayout;
    tDispgridlayout->addWidget(m_pTempCurDescrLab, 0, 0, 1, 1);
    tDispgridlayout->addWidget(m_pTempCurDispLcd, 0, 1, 1, 1);
    tDispgridlayout->addWidget(m_pTempCurDescrUnitrLab, 0, 2, 1, 1);
    tDispgridlayout->addWidget(m_pTempCurTargetDescrLab, 1, 0, 1, 1);
    tDispgridlayout->addWidget(m_pTempCurTargetDescrLcd, 1, 1, 1, 1);
    tDispgridlayout->addWidget(m_pTempCurTargetUnitrLab, 1, 2, 1, 1);

    QGridLayout* ttargridlayout = new QGridLayout;
    ttargridlayout->addWidget(m_pTempTargDescrLab, 0, 0, 1, 1);
    ttargridlayout->addWidget(m_pTempTargSetDpbox, 0, 1, 1, 1);
    ttargridlayout->addWidget(m_pTempTargetSetBtn, 0, 2, 1, 1);
    ttargridlayout->addWidget(m_pCaliFactDescrLab, 1, 0, 1, 1);
    ttargridlayout->addWidget(m_pCaliFactDpbox, 1, 1, 1, 1);
    ttargridlayout->addWidget(m_pCaliFactChgeBtn, 1, 2, 1, 1);

    thorlayout->addLayout(tDispgridlayout);
    thorlayout->addSpacing(10);
    thorlayout->addLayout(ttargridlayout);
    thorlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::MinimumExpanding));
    m_pTempCtrlGpbox->setLayout(thorlayout);

    m_pTempCurveGpbox = new QGroupBox(QStringLiteral("Temperatrue Curve"), m_pTempViewWidget);
    InitCurveView();

    QHBoxLayout* tabBtnhorilayout = new QHBoxLayout();
    tabBtnhorilayout->addWidget(m_pTempRecTabClearBtn,0,Qt::AlignRight);


    QHBoxLayout* maxchorlayout = new QHBoxLayout();
    maxchorlayout->addWidget(m_pRecMaxlineCountLab);
    maxchorlayout->addWidget(m_pRecMaxlineCountSpbox);

    QHBoxLayout* temploghorlayout = new QHBoxLayout();
    temploghorlayout->addWidget(m_pTempLogToFileChk);
    temploghorlayout->addWidget(m_pTempLogFilePathEdit);
    temploghorlayout->addWidget(m_pTempLogFileLoacatBtn);
    temploghorlayout->addWidget(m_pTempLogFileAppendChk);

    QVBoxLayout* tabvctlayout = new QVBoxLayout();
    tabvctlayout->addWidget(m_pTempRecTableWdget);
    tabvctlayout->addLayout(maxchorlayout);
    tabvctlayout->addLayout(tabBtnhorilayout);
    tabvctlayout->addLayout(temploghorlayout);
    m_pTempRecTabGpbox->setLayout(tabvctlayout);

    QHBoxLayout* curtabhorlayout = new QHBoxLayout();
    curtabhorlayout->addWidget(m_pTempCurveGpbox);
    curtabhorlayout->addWidget(m_pTempRecTabGpbox);
    curtabhorlayout->setStretch(0, 20);
    curtabhorlayout->setStretch(1, 1);

    QVBoxLayout* tempvlayout = new QVBoxLayout();
    tempvlayout->addLayout(curtabhorlayout);
    tempvlayout->addWidget(m_pTempCtrlGpbox);
    tempvlayout->setStretch(0, 20);
    tempvlayout->setStretch(1, 1);
    m_pTempViewWidget->setLayout(tempvlayout);

    m_pViewStackWidget->addWidget(m_pTempViewWidget);

    connect(m_pTempRecTabClearBtn, SIGNAL(clicked()), this, SLOT(OnClearTempTableBtnSlot()));
    connect(m_pTempTargetSetBtn, SIGNAL(clicked()), this, SLOT(OnTempTargetSetBtnSlot()));
    connect(m_pCaliFactChgeBtn, SIGNAL(clicked()), this, SLOT(OnTempCalibFactorChgeSLot()));
    connect(m_pTempLogFileLoacatBtn, SIGNAL(clicked()), this, SLOT(OnChooseTempLogFileBtnSlot()));
    connect(m_pTempLogToFileChk, SIGNAL(toggled(bool)), SLOT(OnEnableTempLoggingSlot(bool)));
}


/**
 * @brief 初始化曲线UI
 */
void TemperatureMonitor::InitCurveView(void)
{
    m_pTempCustomPlot = new QCustomPlot(m_pTempCurveGpbox);
    QVBoxLayout* vertCurlayout = new QVBoxLayout();

    m_curveXaisTime = 20;
    m_currenTime = QTime::currentTime();
    m_lastPointKey = 0;
    m_timeToClearCnt = 60*60*24; //24小时的秒数

    m_pYaisMinTempLab = new QLabel(tr("Ymin"), m_pTempCurveGpbox);
    m_pYaisMinTempDpbox = new QDoubleSpinBox(m_pTempCurveGpbox);
    m_pYaisMaxTempLab = new QLabel(tr("Ymax"), m_pTempCurveGpbox);
    m_pYaisMaxTempDpbox = new QDoubleSpinBox(m_pTempCurveGpbox);
    m_pXaisTempLab = new QLabel(tr("Time"), m_pTempCurveGpbox);
    m_pXaisTempSpbox = new QSpinBox(m_pTempCurveGpbox);
    m_pXYaisChageApplyBtn = new QPushButton(tr("Apply"), m_pTempCurveGpbox);
    m_pCurveClearBtn = new QPushButton(tr("Clear"), m_pTempCurveGpbox);
    m_pYaisMinTempDpbox->setSuffix(QStringLiteral(" ℃"));
    m_pYaisMinTempDpbox->setMinimum(-20);
    m_pYaisMinTempDpbox->setValue(-5);
    m_pYaisMaxTempDpbox->setSuffix(QStringLiteral(" ℃"));
    m_pYaisMaxTempDpbox->setValue(40);
    m_pXaisTempSpbox->setSuffix(QStringLiteral(" s"));
    m_pXaisTempSpbox->setValue(20);
    m_pXaisTempSpbox->setMinimum(1);

    QHBoxLayout* hyaislayout = new QHBoxLayout();
    hyaislayout->addStretch();
    hyaislayout->addWidget(m_pYaisMinTempLab);
    hyaislayout->addWidget(m_pYaisMinTempDpbox);
    hyaislayout->addSpacing(10);
    hyaislayout->addWidget(m_pYaisMaxTempLab);
    hyaislayout->addWidget(m_pYaisMaxTempDpbox);
    hyaislayout->addSpacing(10);
    hyaislayout->addWidget(m_pXaisTempLab);
    hyaislayout->addWidget(m_pXaisTempSpbox);
    hyaislayout->addSpacing(10);
    hyaislayout->addWidget(m_pXYaisChageApplyBtn);
    hyaislayout->addStretch();
    hyaislayout->addWidget(m_pCurveClearBtn);

    vertCurlayout->addWidget(m_pTempCustomPlot);
    vertCurlayout->addLayout(hyaislayout);
    m_pTempCurveGpbox->setLayout(vertCurlayout);

    m_pTempCustomPlot->addGraph();
    m_pTempCustomPlot->graph(0)->SetLastDataRange(false);
    m_pTempCustomPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));//曲线1蓝色
    m_pTempCustomPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 205, 50)));     // 设置图层画刷颜色
    m_pTempCustomPlot->graph(0)->setAntialiasedFill(false);        // 设置图层反锯齿：关闭


    m_pTempCustomPlot->addGraph();
    m_pTempCustomPlot->graph(1)->setPen(QPen(Qt::blue, 3));//曲线蓝色
    m_pTempCustomPlot->graph(1)->setLineStyle(QCPGraph::lsNone);   // 不画线条
    m_pTempCustomPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);     // 设置点的形状


    //坐标轴使用时间刻度
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    m_pTempCustomPlot->xAxis->setTicker(timeTicker);
    //四边安上坐标轴
    m_pTempCustomPlot->axisRect()->setupFullAxesBox();
    // x轴设置
    m_pTempCustomPlot->xAxis->setLabel(QString::fromLocal8Bit("时间(单位s)"));         // 设置横轴标签
    m_pTempCustomPlot->yAxis->setRange(m_pYaisMinTempDpbox->value(), m_pYaisMaxTempDpbox->value());       //设置温度范围
    m_pTempCustomPlot->yAxis->setLabel(QString::fromLocal8Bit("温度℃"));

    //m_pTempCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);    //设置属性可缩放，移动
    m_pTempCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);    //允许可缩放，移动
    QList < QCPAxis*>  xis;
    xis.append(m_pTempCustomPlot->xAxis);
    m_pTempCustomPlot->axisRect()->setRangeZoomAxes(xis);

    // 使上下轴、左右轴范围同步
    connect(m_pTempCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_pTempCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_pTempCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_pTempCustomPlot->yAxis2, SLOT(setRange(QCPRange)));
    connect(m_pXYaisChageApplyBtn, &QPushButton::clicked, [&]() {\
        m_pTempCustomPlot->yAxis->setRange(m_pYaisMinTempDpbox->value(), m_pYaisMaxTempDpbox->value()); \
        m_curveXaisTime = m_pXaisTempSpbox->value(); });

    connect(m_pCurveClearBtn, &QPushButton::clicked, this, &TemperatureMonitor::OnClearCurveBtnSlot);
    //定时器连接槽函数realtimeDataSlot
//    connect(&m_pTimerUpdate, SIGNAL(timeout()), this, SLOT(RealtimeDataSlot()));
//    m_pTimerUpdate.start(1000); // 间隔时间 0ms表示尽可能快的触发


}


/**
 * @brief 关闭串口
 */
void TemperatureMonitor::ClosePort(void)
{
    try
    {
        if(m_pSerialPort->IsOpen())
        {
            m_pSerialPort->Close();
        }
    } catch (std::exception& error)
    {
        LOGE("{} Standard exception: {} !", __FUNCTION__, error.what());
        QMessageBox::critical(this, "Warning ", tr("close port error!!!"), QMessageBox::Ok);
    }
}

/**
 * @brief 连接串口
 */
void TemperatureMonitor::OnPortConnectBtnSlot(const bool &checked)
{
    if(checked)
    {
        if(OpenPort())
        {
            m_pPortConnectBtn->setText(tr("Close"));
            m_pPortConnectBtn->setIcon(QIcon(":img/img/close.ico"));
            m_pSearchPortComb->setEnabled(!checked); // 禁止更改串口
            m_pPortBaudRateComb->setEnabled(!checked);
            m_pPortSettingBtn->setEnabled(!checked);
            return;
        }
        else
        {
            m_pPortConnectBtn->setChecked(false);
        }
    }
    else
    {
        ClosePort();
        UpLoadStrToSharedMem(QStringLiteral("Off"));
        m_pPortConnectBtn->setText(tr("Open"));
        m_pPortConnectBtn->setIcon(QIcon(":img/img/connect.ico"));
        m_pSearchPortComb->setEnabled(!checked);
        m_pPortBaudRateComb->setEnabled(!checked);
        m_pPortSettingBtn->setEnabled(!checked);

    }
}

/**
 * @brief 设置串口参数
 */
void TemperatureMonitor::OnPortSettingBtnSlot(void)
{
    if (m_pPortSettingDialog)
    {
        m_pPortSettingDialog->show();
    }
}

/**
 * @brief 打开端口
 */
bool TemperatureMonitor::OpenPort(void)
{
    QString com = m_pSearchPortComb->currentText().section(' ', 0, 0);
    QString desc = m_pSearchPortComb->currentText().section(' ', 1, -1);

#if defined(Q_OS_LINUX)
    if (com.indexOf("/dev/") != 0) {
        com = "/dev/" + com;
    }
#endif
    m_pPortSettingDialog->OnUpdateParam();
    SerialSettingDialog::SerialSettings settings = m_pPortSettingDialog->Settings();
    settings.com = com;
    settings.name = desc;
    settings.baudRate = m_pPortBaudRateComb->currentText().toInt();

    if (m_pSerialPort->Open(settings))
    {
        return true;
    }
    QMessageBox err(QMessageBox::Critical,
        tr("Error"),
        tr("Can not open the port!\n"
            "Port may be occupied or configured incorrectly!"),
        QMessageBox::Cancel, this);
    err.exec();
    return false;

}


/**
 * @brief 扫描端口
 */
void TemperatureMonitor::ScanPort()
{
    bool sync = false;
    int index = 0;
    QVector<QSerialPortInfo> vec;

    //查找可用的串口
    foreach(const QSerialPortInfo & info, QSerialPortInfo::availablePorts()) {
        // 检测端口列表变更
        QString str = info.portName() + " (" + info.description() + ")";
        if (m_pSearchPortComb->findText(str) == -1) {
            sync = true;
        }
        vec.append(info);
    }
    // 需要同步或者ui->portNameBox存在无效端口
    if (sync || m_pSearchPortComb->count() != vec.count()) {
        int len = 0;
        QFontMetrics fm(m_pSearchPortComb->font());
        QString text = m_pSearchPortComb->currentText();
        bool edited = m_pSearchPortComb->findText(text) == -1; // only edit enable (linux)
        m_pSearchPortComb->clear();
        for (int i = 0; i < vec.length(); ++i) {
            QString name = vec[i].portName() + " (" + vec[i].description() + ")";
            m_pSearchPortComb->addItem(name);
            int width = fm.boundingRect(name).width(); // 计算字符串的像素宽度
            if (width > len) { // 统计最长字符串
                len = width;
            }
        }
        // 设置当前选中的端口
        if (!text.isEmpty() && (m_pSearchPortComb->findText(text) != -1 || edited)) {
            m_pSearchPortComb->setCurrentText(text);
            qDebug() << text;
        }
        else {
            m_pSearchPortComb->setCurrentIndex(0);
        }
        //        // 自动控制下拉列表宽度
        m_pSearchPortComb->view()->setMinimumWidth(len + 16);
    }

#if defined(Q_OS_WIN32)
    index = m_pSearchPortComb->findText(WINDOWS_CUSTOM_PORT_NAME, Qt::MatchContains);
#endif
#if defined(Q_OS_LINUX)
    index = m_pSearchPortComb->findText(LINUX_CUSTOM_PORT_NAME, Qt::MatchContains);
#endif
    m_pSearchPortComb->setCurrentIndex(index);
}


/**
 * @brief 更新数据到串口接收界面
 * @param rcvTime 接收时间
 * @param rawData  数据
 */
void TemperatureMonitor::UpdateSerialRcvInfoSlot(const QString rcvTime, const QByteArray rawData)
{
    m_pRecvDataTextEdit->moveCursor(QTextCursor::End);
    QString strMsg = "";
    if (m_pRxAsciiRaBtn->isChecked())
    {
        arrayToASCII(strMsg, rawData);
    }
    else
    {
        arrayToHex(strMsg, rawData);
    }

    ShowMessage(rcvTime, strMsg);
}


/**
 * @brief 更新温度数据到界面
 * @param 日期时间
 * @param 当前温度
 * @param 当前目标温度
 */
void TemperatureMonitor::UpdateTemperaInfoSlot(const QString& dateTimeStr, double curTemp, double curTargTemp)
{
    auto factcur = curTemp + m_dFactorParam;

    UpdateLatestTempToCurve(factcur);
    UpdateLcdDisplay(factcur, curTargTemp);
    InsertTempTable(dateTimeStr, factcur);
    //将当前温度更新到内存共享位置
    UpLoadCurTempToSharedMem(factcur);


    if (m_tempLogFile.isOpen())    //输出温度日志
    {
        QString logStr = dateTimeStr + QString(" %1\n").arg(factcur);
        m_tempLogFile.write(logStr.toLocal8Bit());
    }
}


/**
 * @brief 显示数据到串口接受区
 * @param 接收时间
 * @param 数据
 */
void TemperatureMonitor::ShowMessage(const QString& rcvTime, const QString& data)
{

    static int rcvLineCount = 0;

    QString strMsg = "";
    strMsg = rcvTime + " " + data;
    //    if(text.endsWith("\r\n"))
    //    {
    //         m_pRecvDataTextEdit->append(text.section("\r\n",0,0));
    //    }
    m_pRecvDataTextEdit->append(strMsg);
    if (rcvLineCount++ > m_pRxMaxlineCountSpbox->value())
    {
        m_pRecvDataTextEdit->clear();
        rcvLineCount = 0;
    }

}


/**
 * @brief 数据转换为ASCII
 * @param str   转换后字符串
 * @param array  原始数据
 */
void TemperatureMonitor::arrayToASCII(QString& str, const QByteArray& array)
{
    //    str = QString::fromStdString(array.toStdString());
    str = array;    //QBytearry 直接转QString;
}

/**
 * @brief S数据转换为HEX
 * @param str 转换后字符串
 * @param array 原始数据
 */
void TemperatureMonitor::arrayToHex(QString& str, const QByteArray& array)
{
    int len = array.length();
    str.resize(len * 3);
    for (int i = 0, j = 0; i < len; ++i) {
        uint8_t outChar = uint8_t(array[i]), t;   //每字节填充一次，直到结束
        //十六进制的转换
        t = (outChar >> 4) & 0x0F;
        str[j++] = t + (t < 10 ? '0' : 'A' - 10);
        t = outChar & 0x0F;
        str[j++] = t + (t < 10 ? '0' : 'A' - 10);
        str[j++] = ' ';
    }
}

/**
 * @brief 串口界面发送按钮槽函数
 */
void TemperatureMonitor::OnSendButtonClickedSlot(void)
{
    QString str = m_pSendDataTextEdit->toPlainText();
    if (!str.isEmpty()) {
        SendData();

        // 历史记录下拉列表删除多余项
        while (m_pTxHistoryComb->count() >= 20) {
            m_pTxHistoryComb->removeItem(19);
        }
        // 数据写入历史记录下拉列表
        int i = m_pTxHistoryComb->findText(str);
        if (i != -1) { // 存在的项先删除
            m_pTxHistoryComb->removeItem(i);
        }
        m_pTxHistoryComb->insertItem(0, str); // 数据添加到第0个元素
        m_pTxHistoryComb->setCurrentIndex(0);
    }
}


/**
 * @brief 串口发送函数
 */
void TemperatureMonitor::SendData(void)
{
    QByteArray array;
    if (m_pTxAsciiRaBtn->isChecked())
    {
        array = m_pSendDataTextEdit->toPlainText().toLocal8Bit();
    }
    else
    {
        array = QByteArray::fromHex(m_pSendDataTextEdit->toPlainText().toLatin1());
    }
    if (m_pTxFormaTailComb->currentText() == QStringLiteral("CR/LF"))
    {
        array.append(0x0D); //ASCII 码中为 /r
        array.append(0x0A);//ASCII 码中为 /n
    }
    m_pSerialPort->WritePortData(array);
}


/**
 * @brief 循环发送槽函数
 */
void TemperatureMonitor::ResendTimerSlot(void)
{
    QString str = m_pSendDataTextEdit->toPlainText();
    if (!str.isEmpty())
    {
        m_pTxSendBtn->click();
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),
            tr("Send Text is empty,please input and try again."));
        m_pIsTimerToSend->setChecked(false);
    }
}

/**
 * @brief 使能循环发送
 * @param checked  是否使能
 */
void TemperatureMonitor::OnPeriodSendToggleSlot(bool checked)
{
    if (checked)
    {
        m_pResendTimer->start(m_pTimerToSendSpbox->value());
    }
    else
    {
        m_pResendTimer->stop();
    }
}



/**
 * @brief 测试曲线槽函数
 */
void TemperatureMonitor::RealtimeDataSlot()
{
#if(0)
    double key = m_currenTime.elapsed() / 1000.0; // 开始到现在的时间，单位秒
    if (key - m_lastPointKey > 0.002) // 大约2ms添加一次数据
    {
        // 添加数据到graph
//        m_pTempCustomPlot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
//        m_pTempCustomPlot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
        m_pTempCustomPlot->graph(0)->addData(key, 28);
        m_pTempCustomPlot->graph(1)->addData(key, 28);
        //记录当前时刻
        m_lastPointKey = key;
    }
    // 曲线能动起来的关键在这里，设定x轴范围为最近8个时刻
    m_pTempCustomPlot->xAxis->setRange(key, m_curveXaisTime, Qt::AlignRight);
    //绘图
    m_pTempCustomPlot->replot();
#endif
}

/**
 * @brief 更新温度曲线
 * @param temp  最新温度
 */
void TemperatureMonitor::UpdateLatestTempToCurve(const double& temp)
{
    double key = m_currenTime.elapsed() / 1000.0; // 开始到现在的时间，单位秒
    if(key > m_timeToClearCnt)
    {
        OnClearCurveBtnSlot();
        return;
    }
    if (key - m_lastPointKey > 0.002) // 大约2ms添加一次数据
    {
        // 添加数据到graph
        m_pTempCustomPlot->graph(0)->addData(key, temp);
        m_pTempCustomPlot->graph(1)->addData(key, temp);
        //记录当前时刻
        m_lastPointKey = key;
    }
    // 曲线能动起来的关键在这里，设定x轴范围为最近8个时刻
    m_pTempCustomPlot->xAxis->setRange(key, m_curveXaisTime, Qt::AlignRight);
    //绘图
    if(m_pTempCustomPlot->isVisible())
    {
        m_pTempCustomPlot->replot();
    }
}

/**
 * @brief 更新LCD 温度显示
 * @param curTemp   当前温度
 * @param curTargetTemp 当前目标温度
 */
void TemperatureMonitor::UpdateLcdDisplay(const double& curTemp, const double& curTargetTemp)
{
    m_curTempStr.setNum(curTemp,'f',2);
    if(m_pTempCurDispLcd->isVisible())
    {
        m_pTempCurDispLcd->display(m_curTempStr);
    }
    if(m_pTempCurTargetDescrLcd->isVisible())
    {
        m_targetTempStr.setNum(curTargetTemp,'f',2);
        m_pTempCurTargetDescrLcd->display(m_targetTempStr);
    }

    emit SetCurrentTempSignal(m_curTempStr);
}

/**
 * @brief 插入表格数据
 * @param dataTimeStr   日期字符串
 * @param temp  当前温度
 */
void TemperatureMonitor::InsertTempTable(const QString& dataTimeStr, const double& temp)
{
    update();
#if (1)
    QTableWidgetItem *itemTimePtr  = m_pTempRecTableWdget->item(m_tempTableRawCnt,0);;
    QTableWidgetItem *itempTempPtr = m_pTempRecTableWdget->item(m_tempTableRawCnt,1);;

//    QString temperature = QString("%1").arg(temp);

    QString temperature ;
    temperature.setNum(temp,'f',2);
    if(m_tempTableRawCnt >= m_pRecMaxlineCountSpbox->value())
    {
        OnClearTempTableBtnSlot();
        m_tempTableRawCnt = 0;
    }

    if(itemTimePtr == nullptr)
    {
        itemTimePtr =  new QTableWidgetItem();
        itemTimePtr->setData(Qt::DisplayRole,dataTimeStr);
        m_pTempRecTableWdget->setItem(m_tempTableRawCnt, 0, itemTimePtr);
    }
    else
    {
         itemTimePtr->setData(Qt::DisplayRole,dataTimeStr);
    }

    if(itempTempPtr == nullptr)
    {
        itempTempPtr =  new QTableWidgetItem();
        itempTempPtr->setData(Qt::DisplayRole,temperature);
        m_pTempRecTableWdget->setItem(m_tempTableRawCnt, 1, itempTempPtr);
    }
    else
    {
         itempTempPtr->setData(Qt::DisplayRole,temperature);
    }

    if(m_pTempRecTableWdget->isVisible())
    {
        m_pTempRecTableWdget->scrollToItem(itempTempPtr);
    }
    m_tempTableRawCnt++;
#endif

#if(0)
//    QString temperature = QString("%1").arg(temp);
//    int rowCount = m_pTempRecTableWdget->rowCount();   //获取总行数
//    if (rowCount > m_pRecMaxlineCountSpbox->value())     //防止表格数据过多
//    {
//        OnClearTempTableBtnSlot();
//    }
//    m_pTempRecTableWdget->insertRow(rowCount);         //插入行
//    QTableWidgetItem* item0 = new QTableWidgetItem();
//    QTableWidgetItem* item1 = new QTableWidgetItem();
//    item0->setText(dataTimeStr);
//    item1->setText(temperature);
//    m_pTempRecTableWdget->setItem(rowCount, 0, item0);
//    m_pTempRecTableWdget->setItem(rowCount, 1, item1);
//    m_pTempRecTableWdget->verticalScrollBar()->setValue(m_pTempRecTableWdget->rowCount());
#endif
}

/**
 * @brief 保存表格到文件
 */
void TemperatureMonitor::OnClearTempTableBtnSlot(void)
{
#if(0)
//    QTableWidgetItem *itemptr = nullptr;
//    while (m_pTempRecTableWdget->rowCount())
//    {
//        for(int col = 0; col <2 ; ++col)
//        {
//            itemptr = m_pTempRecTableWdget->item(0,col);
//            SAFE_DELETEP(itemptr);
//        }
//        m_pTempRecTableWdget->removeRow(0);
//    }
#endif
#if(1)
    m_pTempRecTableWdget->clearContents();
    m_tempTableRawCnt = 0;
#endif
}

/**
 * @brief 设置温度按钮槽函数
 */
void  TemperatureMonitor::OnTempTargetSetBtnSlot(void)
{
    double target = m_pTempTargSetDpbox->value();
    m_dFactorParam = m_pCaliFactDpbox->value() * ((34 - target) / (34 - 28));//实际目标温度转化公式 T-A*((34-T)/(34-28))
    double transformtartget = target - m_dFactorParam;
    m_pTempTargSetDpbox->setToolTip(QStringLiteral("T-A*((34-T)/(34-28))"));
    QString sendStr = QString("S1 %1").arg(transformtartget);
    m_pSendDataTextEdit->clear();
    m_pSendDataTextEdit->setText(sendStr);
    OnSendButtonClickedSlot();
}


/**
 * @brief 更改温度校准因子槽函数
 */
void TemperatureMonitor::OnTempCalibFactorChgeSLot(void)
{
    bool ok = false;
    QString text = QInputDialog::getText(this, QString::fromLocal8Bit("密码确认"), QString::fromLocal8Bit("请输入密码"), QLineEdit::Password, 0, &ok);
    if (ok)
    {
        if (QString::compare(text,QString::fromStdString( PROGRAME_PASSWORD)) == 0)
        {
            ok = false;
            double factor = QInputDialog::getDouble(this, QString::fromLocal8Bit("输入校准系数"), QString::fromLocal8Bit("系数更改为"), m_pCaliFactDpbox->value(), 0.0, 10, 2, &ok);
            if (ok)
            {
                m_pCaliFactDpbox->setValue(factor);
                double target = m_pTempTargSetDpbox->value();
                m_dFactorParam = factor * ((34 - target) / (34 - 28));//实际目标温度转化公式 T-A*((34-T)/(34-28))
                SaveConfig(m_config);
            }
        }
        else
        {
            QMessageBox::critical(this, "Warning ", tr("wrong password"), QMessageBox::Ok);
        }
    }
}

/**
 * @brief 选择LogFile路径
 */
void TemperatureMonitor::OnChooseLogFileBtnSlot(void)
{
    QString filename = QString("%1_RawData.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    QString logFile = QFileDialog::getSaveFileName(this, tr("Save log file ..."), filename);
    if (!logFile.isEmpty())
    {
        m_pLogFilePathEdit->setText(logFile);
    }
}

/**
 * @brief 输出日志槽函数
 * @param on
 */
void TemperatureMonitor::OnEnableLoggingSlot(const bool& on)
{
    if (m_logFile.isOpen() == on)
    {
        return;
    }

    if (on)
    {
        m_logFile.setFileName(m_pLogFilePathEdit->text());
        QIODevice::OpenMode mode = QIODevice::ReadWrite;
        if (m_pLogFileAppendChk->isChecked())
        {
            mode = mode | QIODevice::Append;
        }
        else
        {
            mode = mode | QIODevice::Truncate;
        }

        if (!m_logFile.open(mode))
        {
            QMessageBox::information(this, tr("Opening file failed"), tr("Could not open file %1 for writing").arg(m_pLogFilePathEdit->text()));
            m_pLogToFileChk->setChecked(false);
        }
        else
        {
            m_pLogFileAppendChk->setEnabled(false);
            m_pLogFilePathEdit->setEnabled(false);
            m_pLogFileLoacatBtn->setEnabled(false);
        }
    }
    else
    {
        m_logFile.close();

        m_pLogFileAppendChk->setEnabled(true);
        m_pLogFilePathEdit->setEnabled(true);
        m_pLogFileLoacatBtn->setEnabled(true);
    }

}

/**
 * @brief 选择LogFile路径
 */
void TemperatureMonitor::OnChooseTempLogFileBtnSlot(void)
{
    QString filename =QString("%1_Templog.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    QString logFile = QFileDialog::getSaveFileName(this, tr("Save log file ..."), filename);
    if (!logFile.isEmpty())
    {
        m_pTempLogFilePathEdit->setText(logFile);
    }
}

/**
 * @brief 输出日志槽函数
 * @param on 是否勾选
 */
void TemperatureMonitor::OnEnableTempLoggingSlot(const bool& on)
{
    if (m_tempLogFile.isOpen() == on)
    {
        return;
    }

    if (on)
    {
        m_tempLogFile.setFileName(m_pTempLogFilePathEdit->text());
        QIODevice::OpenMode mode = QIODevice::ReadWrite;
        if (m_pTempLogFileAppendChk->isChecked())
        {
            mode = mode | QIODevice::Append;
        }
        else
        {
            mode = mode | QIODevice::Truncate;
        }

        if (!m_tempLogFile.open(mode))
        {
            QMessageBox::information(this, tr("Opening file failed"), tr("Could not open file %1 for writing").arg(m_pTempLogFilePathEdit->text()));
            m_pTempLogToFileChk->setChecked(false);
        }
        else
        {
            m_pTempLogFileAppendChk->setEnabled(false);
            m_pTempLogFilePathEdit->setEnabled(false);
            m_pTempLogFileLoacatBtn->setEnabled(false);
        }
    }
    else
    {
        m_tempLogFile.close();
        m_pTempLogFileAppendChk->setEnabled(true);
        m_pTempLogFilePathEdit->setEnabled(true);
        m_pTempLogFileLoacatBtn->setEnabled(true);
    }

}

void TemperatureMonitor::OnSwichFunctViewChange(int index)
{
    if (index == TEMP_VIEW_CONTROL_INDEX)
    {
        bool ok = false;
        QString text = QInputDialog::getText(this, QString::fromLocal8Bit("密码确认"), QString::fromLocal8Bit("请输入密码"), QLineEdit::Password, 0, &ok);
        if (ok)
        {
            if (QString::compare(text, QString::fromStdString(PROGRAME_PASSWORD)) == 0)
            {
                m_pViewStackWidget->setCurrentIndex(TEMP_VIEW_CONTROL_INDEX);
            }
            else
            {
                m_pSwithViewComb->setCurrentIndex(TEMP_VIEW_CURVE_INDEX);
                QMessageBox::critical(this, "Warning ", tr("wrong password"), QMessageBox::Ok);
            }
        }

    }
    else if (index == TEMP_VIEW_CURVE_INDEX)
    {
        m_pViewStackWidget->setCurrentIndex(TEMP_VIEW_CURVE_INDEX);
    }
    else if(index == TEMP_VIEW_LOG_INDEX)
    {
        m_pViewStackWidget->setCurrentIndex(TEMP_VIEW_LOG_INDEX);
    }

}

void TemperatureMonitor::OnClearCurveBtnSlot(void)
{
    m_pTempCustomPlot->graph(0)->data()->clear();
    m_pTempCustomPlot->graph(1)->data()->clear();
    m_currenTime.restart();
    m_lastPointKey = 0;
}


unsigned long TemperatureMonitor::GetCurrentThreadSelf(void)
{
#ifdef Q_OS_WIN32
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
    return 0;
}


void TemperatureMonitor::UpLoadCurTempToSharedMem(const double &curTemp)
{
    if(m_tempSharedMem.isAttached()) {
           m_tempSharedMem.detach();
       }
       QByteArray arr = QString::number(curTemp,'f',2).toLocal8Bit();
       int len = arr.length();
       if(!m_tempSharedMem.create(len)) {
           LOGE("Attach Error : {}",m_tempSharedMem.errorString().toStdString());
           qDebug() << m_tempSharedMem.errorString();
           return ;
       }
       m_tempSharedMem.lock();
       char *dest = reinterpret_cast<char *>(m_tempSharedMem.data());
       for (int i = 0; i < len; i++) {
           dest[i] = arr[i];
       }
       m_tempSharedMem.unlock();
}

void TemperatureMonitor::UpLoadStrToSharedMem(const QString &str)
{
    if(m_tempSharedMem.isAttached()) {
           m_tempSharedMem.detach();
       }
       QByteArray arr = str.toLocal8Bit();
       int len = arr.length();
       if(!m_tempSharedMem.create(len)) {
           LOGE("Attach Error : {}",m_tempSharedMem.errorString().toStdString());
           qDebug() << m_tempSharedMem.errorString();
           return ;
       }
       m_tempSharedMem.lock();
       char *dest = reinterpret_cast<char *>(m_tempSharedMem.data());
       for (int i = 0; i < len; i++) {
           dest[i] = arr[i];
       }
       m_tempSharedMem.unlock();
}
