#pragma once

#include <QMainWindow>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlistwidget.h>
#include <qstackedwidget.h>
#include <thread>
#include <qtimer.h>
#include <qlcdnumber.h>

#include "DataHandle.h"
#include "UsbFtdDevice.h"
#include "qsharedmemory.h"



class QSettings;
class QToolButton;
class QGroupBox;
class QStateMachine;
class QState;
class LogPane;
class RegisterMapWdgt;
class RegisterWidget;
class ConfigurationWidgt;
class DataAnalysWidgt;
class CustomSplitter;
class UDPSocket;
class QThread;
class QNotify;
class SavePannel;
class SensorPanel;
class VoltCtrPanel;
class DegatingPane;
class PoreStateMapWdgt;
class DataCalculate;
class PoreCtrlPanel;
class MuxScanPanel;
class TemperatureMonitor;
class ChartViewWidgt;
class FlowOpertPanel;

typedef enum{
    STREAM_TYPE_USB = 0,
    STREAM_TYPE_UDP = 1,
}STREAM_TYPE_ENUM;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    LogPane *m_pLogPane;
    UsbFtdDevice m_usbFtdDevice;
    UDPSocket *m_pUDPSocket = nullptr;
    DataHandle m_dataHandle;

    //menu & toolbar &statusBar
    QMenu* m_pMenuFile = nullptr;
    QMenu* m_pMenuTool = nullptr;
    QMenu* m_pMenuHelp = nullptr;
    QToolBar* m_pToolBar = nullptr;
    QStatusBar* m_pStatusBar = nullptr;

    //Toolbar USB device
    QLabel* m_plabDevice = nullptr;
    QComboBox* m_pcmbDevice = nullptr;
    QPushButton* m_pbtnSearch = nullptr;

    //Toolbar UDP device
    QLabel *m_plabUdpIcon = nullptr;
    QToolButton *m_pUdpLocalIpSerchBtn = nullptr;
    QLabel *m_plabUdpLocalPortDesc = nullptr;
    QComboBox *m_pUdpLocalIpInpComb = nullptr;
    QLineEdit *m_pUdpLocalPortInpEdit = nullptr;

    QAction* m_pactlabUdpIcon =nullptr;
    QAction* m_pactUdpLocalIpSerchBtn =nullptr;
    QAction* m_pactlabUdpLocalPortDesc =nullptr;
    QAction* m_pactUdpLocalIpInpCombn =nullptr;
    QAction* m_pactUdpLocalPortInpEdit =nullptr;
    QAction* m_pactUdpTestMode = nullptr;

    QAction* m_pactStreamPrintMode = nullptr;
    //simulated data test
    QAction* m_pactSimulateDataTest =nullptr;
    //ToolBar sensor state
    QLabel *m_pLabCurSensorDisp = nullptr;
    //ToolBar temperature
    QAction* m_pactTemperature = nullptr;
    QLCDNumber* m_plcdTemperature = nullptr;
    QLabel* m_plabTemperature = nullptr;


    //files
    QAction* m_pactEnableOpenGl = nullptr;
    QAction* m_pactExit = nullptr;

    //tools
    QAction* m_pactConsole = nullptr;
    QAction* m_pactOpen = nullptr;
    QAction* m_pactStart = nullptr;
    //tools crc
    QAction* m_pactCrcCheck = nullptr;
    QAction* m_pactCrcNfPrint = nullptr;

    //about
    QAction* m_pactUserGuide = nullptr;
    QAction* m_pactAbout = nullptr;

    //statusBar
    QLabel *m_pStatusPowerConsumpLab = nullptr;
    QLabel *m_pStatusBgiIconLab = nullptr;
    QLabel *m_pStatuConnetLab = nullptr;

    QLabel *m_pRunTimeRecLab = nullptr;
    QTime m_tmRunTimeRec;
    QTimer *m_pTmrRunTimeRec= nullptr;
    QLCDNumber *m_plcdRunTimeRec= nullptr;

    QLabel m_labAutoStopSaving;
    QTime m_tmAutoStopSaving;
    QTimer m_tmrAutoStopSaving;
    QLCDNumber m_lcdAutoStopSaving;

    QLabel m_labSpeedDisp;

    //Spliter
    CustomSplitter *m_pMainSplitter = nullptr;

    //listwidget
    QListWidget *m_pListWdiget = nullptr;
    //stackwidget
    QStackedWidget *m_pFuncViewStackWidget = nullptr;
    //Registers View
    RegisterWidget *m_pRegisterWdgt = nullptr;
    //Collect data View
    DataAnalysWidgt *m_pDataAnalysWidgt = nullptr;
    CustomPlotWidgt *m_pCustomPlotWidgt = nullptr;
    SavePannel *m_pSavePane = nullptr;
    SensorPanel *m_pSensorPanel = nullptr;
    VoltCtrPanel *m_pVoltCtrPanel =nullptr;
    PoreCtrlPanel *m_pPoreCtrlPanel =nullptr;
    DegatingPane* m_pDegatingPane = nullptr;
    MuxScanPanel *m_pMuxScanPanel = nullptr;
    PoreStateMapWdgt *m_pPoreStateMapWdgt= nullptr;
    //temperature view
    TemperatureMonitor *m_pTemperatureMonitorView =nullptr;
    //chart view
    ChartViewWidgt *m_pChartViewWidgt = nullptr;
    //flowoperate panel
    FlowOpertPanel *m_pFlowOpertPanel = nullptr;

    DataCalculate *m_pDataCalculate = nullptr;
    CapCaculate *m_pCapCaculate = nullptr;


    QNotify *m_pNotify = nullptr;
    //Board Configuration View
    ConfigurationWidgt *m_pConfigurationWidgt = nullptr;


    QWidget *m_pCenterWidget = nullptr;
    QSettings* m_pSysConfig = nullptr;

    QTimer m_tempUpdateTimer;
    QSharedMemory m_tempSharedMem;


private:
    QStateMachine *m_pMoveOpacMachine;
    QState *m_pMoveOpacStatStart;
    QState *m_pMoveOpacStatEnd;
    QPropertyAnimation *m_pOpacityAnimation;
//    QPropertyAnimation *m_pMoveAnimation;

private:
    bool InitConfig(void);
    bool LoadConfig(void);
    bool SaveConfig(void);
    bool InitMenuToolbar(void);
    bool InitViewPane(void);
    void InitAnimation(void);
    bool OnUdpPortSetting(bool enable);
    void CloseDataStream(void);
    void LoadStyleSheet(const QString &style, QWidget* pObject = nullptr);
    void StartOrStopRuntimer(const bool &bStart);



private slots:
    void OnShowAbout(void);
    void OnShowUserGuide(void);
    void OnShowConsole(bool checked);
    bool OnSearchPort(void);
    void OnOpenPort(bool checked);
    void OnStartStop(bool checked);
    void OnExit(void);
    void OnEnableUdpTestMode(const bool enable);
    void OnEnableStreamPrintMode(const bool enable);


    void UpdateAutoStopTimeSlot(void);
    void startAutoStopTimerSlot(int msec);
    void stopAutoStopTimerSlot(void);
    void resetAutoStopTimerSlot(int min);

    void OnSearchLocalIp(void);
    void OnDispDeviceSpeed(const float &speed);

    void OnSampleRateChangeSlot(void);
    void UpdateCurrentSensorGrpStrSlot(const QString &sensorGrpStr);

    void SetCurrentTempSlot(const QString &tempstr);
    void OnShowTempureTool(void);
    void UpdateRuntimeSlot(void);

    void AFEPowerConsumpChangeSlot(const POWER_CONSUMPTION_ENUM &flag);
    void OnTimerUpdateTempSlot(void);

private:
    bool m_bRunning = false;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;


    STREAM_TYPE_ENUM m_StreamType;

public:
    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);

    void EndAllThread(void);
    void StopAllThread(void);

protected:
    void resizeEvent(QResizeEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);


};
