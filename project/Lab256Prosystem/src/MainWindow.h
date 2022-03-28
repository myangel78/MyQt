#pragma once

#include <QMainWindow>

#include <qsettings.h>
#include <qtoolbar.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qaction.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qlcdnumber.h>

#ifdef Q_OS_WIN32
#include "UsbCyDevice.h"
#else
#include "UsbCyclone.h"
#endif

#include "UsbPortBulk.h"


class CustomPlotView;
class ChannelStatusView;
class CapacitanceView;
class HistoryDataView;
class ControlView;
class LogPane;
class SavePane;
//class AdjustmentPane;
class FlowModulePane;
class CalibrationDialog;
class PoreSelectDialog;
class TemperatureMonitor;
class UpdateDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QSettings* m_pSysConfig = nullptr;

    //menu & toolbar
    QMenu* m_pMenuFile = nullptr;
    QMenu* m_pMenuTool = nullptr;
    QMenu* m_pMenuHelp = nullptr;
    QToolBar* m_pToolBar = nullptr;
    QStatusBar* m_pStatusBar = nullptr;

    //toolbar item
    QLabel* m_plabDevice = nullptr;
    QComboBox* m_pcmbDevice = nullptr;
    QPushButton* m_pbtnSearch = nullptr;
    QLabel* m_plabSequencingId = nullptr;

    //file
    QAction* m_pactCalibration = nullptr;
    QAction* m_pactLoadFirmware = nullptr;
    QAction* m_pactEnableExperiment = nullptr;
    QAction* m_pactHardwareVersion1 = nullptr;
    QAction* m_pactHardwareVersion2 = nullptr;
    QAction* m_pactEnableOpenGl = nullptr;
    QAction* m_pactSampleRate = nullptr;

    //tools
    QAction* m_pactOpen = nullptr;
    QAction* m_pactLoad = nullptr;
    QAction* m_pactStart = nullptr;
    QAction* m_pactTemperature = nullptr;
    QAction* m_pactPoreSel = nullptr;
    //QDoubleSpinBox* m_pdspTemperature = nullptr;
    QLCDNumber* m_plcdTemperature = nullptr;
    QLabel* m_plabTemperature = nullptr;
    //QDoubleSpinBox* m_pdspSeqVolt = nullptr;
    //QLabel* m_plabSeqVolt = nullptr;

    //about
    QAction* m_pactUserGuide = nullptr;
    QAction* m_pactReportIssue = nullptr;
    QAction* m_pactSuggest = nullptr;
    QAction* m_pactFeedback = nullptr;
    QAction* m_pactAbout = nullptr;
    QAction* m_pactUpdate = nullptr;

    ////stage
    //QLabel* m_plabCurrentStage = nullptr;
    //QComboBox* m_pcmbCurrentStage = nullptr;


    //view
    CustomPlotView* m_pCustomPlotView = nullptr;
    ChannelStatusView* m_pChannelStatusView = nullptr;
    CapacitanceView* m_pCapacitanceView = nullptr;
    HistoryDataView* m_pHistoryDataView = nullptr;
    ControlView* m_pControlView = nullptr;
    TemperatureMonitor *m_pTemperatureMonitorView =nullptr;

    //pane
    LogPane* m_pLogPane = nullptr;
    SavePane* m_pSavePane = nullptr;
    //AdjustmentPane* m_pAdjustmentPane = nullptr;
    //FlowModulePane* m_pModuleFlowPane = nullptr;

    //calibration
    CalibrationDialog* m_pCalibrationDlg = nullptr;

    //pore filter
    PoreSelectDialog* m_pPoreSelectDlg = nullptr;
    //UpdateDialog *m_pUpdateDialog = nullptr;


    //status bar
    QLabel m_labAutoStopSaving;
    QTime m_tmAutoStopSaving;
    QTimer m_tmrAutoStopSaving;
    QLCDNumber m_lcdAutoStopSaving;

    bool m_bResetRunTimeRec = false;
    QLabel m_labRunTimeRec;
    QTime m_tmRunTimeRec;
    QTimer m_tmrRunTimeRec;
    QLCDNumber m_lcdRunTimeRec;

    QTimer m_tmrCheckVerupdate;


#ifdef Q_OS_WIN32
    UsbCyDevice m_devusb;
#else
    UsbCyclone m_devusb;
#endif
    UsbPortBulk m_usbbulk;
    std::vector<std::tuple<std::string, unsigned short, unsigned short>> m_vetDevice;

    FILE* m_pFile = nullptr;

public:
    enum ESampleRateRegister
    {
        ESR_SAMPLE_20K = 0x00,
        ESR_SAMPLE_5K = 0x02,

        ESR_SAMPLE_MAX
    };

    enum EOverloadRegister
    {
        EOR_OVERLOAD_1X_DEFAULT = 0x00,
        EOR_OVERLOAD_BASE = 0x8460,
        EOR_OVERLOAD_1X = EOR_OVERLOAD_BASE + 0x00,
        EOR_OVERLOAD_2X = EOR_OVERLOAD_BASE + 0x02,
        EOR_OVERLOAD_4X = EOR_OVERLOAD_BASE + 0x04,
        EOR_OVERLOAD_8X = EOR_OVERLOAD_BASE + 0x08,

        EOR_OVERLOAD_MAX
    };


private:
    bool InitMenuToolbar(void);
    bool InitViewPane(void);

    bool InitConfig(void);
    bool LoadConfig(void);
    bool SaveConfig(void);

    bool DoShowPoreSelect(bool val);
    bool DoShowUpdate(bool val);


public slots:
    void OnShowCalibration(void);
    void OnLoadFirmware(void);
    void OnEnableExperiment(bool chk);
    void OnShowSampleRate(void);

    void OnSearchPort(void);
    void OnOpenPort(bool checked);
    void OnLoadReg(void);
    void OnShowTempureTool(void);
    void OnStartStop(bool checked);
    void OnShowPoreSelect(void);

    void OnShowUserGuide(void);
    void OnSendFeedback(void);
    void OnShowUpdate(void);
    void OnShowAbout(void);

    //void OnCurrentStageChanged(int val);

    void SetSampleRateChanged(int val);
    void SetOverMultipleChanged(int val);

    void onSequencingIdChangedSlot(int val);

    //void setSequencingVoltageSlot(double val);
    //void getSequencingVoltageSlot(double& val);
    void ShowPoreSelectSlot(bool val);


    bool ClearBuf(void);
    bool FPGARegisterWrite(unsigned short addr, unsigned int data);
    void FPGARegisterReadBack(unsigned short addr, unsigned int& rValue, bool& result);

private:
    bool m_bRunning = false;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;
public:
    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);

    void EndAllThread(void);
    void StopAllThread(void);


private slots:
    void UpdateRuntimeSlot(void);

    void UpdateAutoStopTimeSlot(void);
    void startAutoStopTimerSlot(int msec);
    void stopAutoStopTimerSlot(void);
    void resetAutoStopTimerSlot(int min);

    void OnChangeHardwareVersionSlot(void);

    void timerCheckVerupdateSlot(void);

    void SetCurrentTempSlot(double temp);

    void startStopRecordVoltSlot(int val, const char* ppath);
};
