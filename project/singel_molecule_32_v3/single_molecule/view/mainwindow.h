#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "view/logpage.h"
#include "view/qplotwidget.h"
#include "view/cappanelview.h"
#include "view/aboutbox.h"
#include "view/currentsingelsettingpage.h"
#include "view/currentdisplayview.h"
#include "view/calibrationdialog.h"
#include "view/savefilepanel.h"
#include "view/controlpanel.h"

#include "serialsettingsdialog.h"
#include "models/DatacCommunicaor.h"
#include "models/SerialPortControl.h"
#include "models/SerialPortOut.h"
#include "models/DataStackHandle.h"
#include "models/DataMonitor.h"
#include "models/DataCollection.h"
#include "models/DataMeansCal.h"
#include "models/DataCapCollection.h"
#include "models/DataCapCaculate.h"
#include "models/DataOutputFormat.h"
#include "models/DataRecord.h"
#include "models/DataFunction.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void stopThread();
    void writeCmd(int cmd);
    void updateStatusSignal(int id, int status);

private slots:
    void on_actionCheck_port_triggered();
    void on_actionOpen_data_folder_D_triggered();
    void on_actionOpen_log_folder_L_triggered();
    void on_actionAbout_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionCalibration_triggered();
    void on_actionClearPort_triggered();
    void recvqMsgPortInfo_Slot(QString info);
    void updataRunTimeDisplay(void);
    void dataFunctionFinished(void);


private:
    void initWidget();
    void LoadConfig(QSettings *config);
    void SaveConfig(QSettings *config);
    void intiSetting();
    void SaveSetting();

    void initRunTimeView(void);
    void startRunTimeDisplay(void);
    void stopRunTimeDisplay(void);
    void StopRunnningAndSaveSlot(void);


private:
    Ui::MainWindow *ui;

    SerialPortControl *m_pSerialPortControl;
    SerialPortOut * m_pSerialPortOut;
    QThread *m_pSerialPortOutThread;
    DataStackHandle * m_DataStackHandle;
    DataMonitor *m_pDataMonitor;
    DataCollection *m_pDataCollection;
    DataMeansCal *m_pDataMeansCal;
    DataCapCollection *m_pDataCapCollection;
    DataCapCaculate *m_pDataCapCaculate;
    DataOutputFormat *m_pDataOutputFormat;
    DataRecord * m_pDataRecord;
    QThread *m_pDataRecordThread;
    DataFunction *m_pDataFunction;
    QThread *m_pDataFunctionThread;


    LogPage *m_pLogPage;
    QPlotwidget *m_pCurrcrnttWidget;
    CapPanelView *m_pCapPanelView;
    CurrentSingelSettingPage *m_pCurrentSingelSettingPage;
    CurrentDisplayView *m_pCurrentDisplayView;
    SerialSettingsDialog * m_pSerialPortsettingsDialog    = new SerialSettingsDialog;
    Calibrationdialog *m_pCalibrationdialog;
    SaveFIlePanel *m_pSaveFIlePanel;
    ControlPanel *m_pControlPanel;


    QSettings *m_calibConfig;
    QSettings *m_nSettingConfig;

    QLCDNumber m_runTimeLCDNumber;
    QLabel m_runTimeLab;
    QTimer m_runSecTimer;
    QTime m_runTimeRecord;

};
#endif // MAINWINDOW_H
