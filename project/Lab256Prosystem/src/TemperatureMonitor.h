#pragma once

#include<QWidget>
#include <QSerialPort>
#include "qcustomplot.h"

class QLabel;
class QComboBox;
class QPushButton;
class QCheckBox;
class QTextEdit;
class QSpinBox;
class QThread;
class QStackedWidget;
class QRadioButton;
class SerialPort;
class SerialSettingDialog;
class QGroupBox;
class QTimer;
class QDoubleSpinBox;
class QLCDNumber;



class TemperatureMonitor : public QDialog
{
    Q_OBJECT
public:
    explicit TemperatureMonitor(QWidget* parent = nullptr);
    ~TemperatureMonitor();

private:
    QLabel* m_pSearchPortNameLab;
    QComboBox* m_pSearchPortComb;
    QLabel* m_pPortBaudRateLab;
    QComboBox* m_pPortBaudRateComb;
    QPushButton* m_pPortSearchBtn;
    QPushButton* m_pPortConnectBtn;
    QPushButton* m_pPortSettingBtn;
    QGroupBox* m_pSearchPortGpbox;
    QComboBox* m_pSwithViewComb;

    QGroupBox* m_pInteractViewGpbox;
    QStackedWidget* m_pViewStackWidget;

    QWidget* m_pSerialTextrrView;
    QTextEdit* m_pRecvDataTextEdit;
    QPushButton* m_pRxClearBtn;

    QCheckBox* m_pLogToFileChk;
    QLineEdit* m_pLogFilePathEdit;
    QPushButton* m_pLogFileLoacatBtn;
    QCheckBox* m_pLogFileAppendChk;

    QLabel* m_pRecMaxlineCountLab;
    QSpinBox* m_pRecMaxlineCountSpbox;
    QCheckBox* m_pTempLogToFileChk;
    QLineEdit* m_pTempLogFilePathEdit;
    QPushButton* m_pTempLogFileLoacatBtn;
    QCheckBox* m_pTempLogFileAppendChk;

    QTextEdit* m_pSendDataTextEdit;
    QGroupBox* m_pRxSettingsGpbox;
    QGroupBox* m_pTxSettingsGpbox;
    QRadioButton* m_pTxAsciiRaBtn;
    QRadioButton* m_pRxAsciiRaBtn;
    QRadioButton* m_pTxHexRaBtn;
    QRadioButton* m_pRxHexRaBtn;
    QCheckBox* m_pIsTimerToSend;
    QSpinBox* m_pTimerToSendSpbox;
    QPushButton* m_pTxSendBtn;
    QPushButton* m_pTxClearBtn;
    QComboBox* m_pTxHistoryComb;
    QComboBox* m_pTxFormaTailComb;
    QLabel* m_pRxMaxlineCountLab;
    QSpinBox* m_pRxMaxlineCountSpbox;
    QTimer* m_pResendTimer;

    SerialPort* m_pSerialPort;
    SerialSettingDialog* m_pPortSettingDialog;
    QThread* m_pSerialPortThread;

    QCPItemText* m_pCurveCoorItem;
    QCustomPlot* m_pTempCustomPlot;
    QGroupBox* m_pTempCurveGpbox;
    QLabel* m_pYaisMinTempLab;
    QDoubleSpinBox* m_pYaisMinTempDpbox;
    QLabel* m_pYaisMaxTempLab;
    QDoubleSpinBox* m_pYaisMaxTempDpbox;
    QLabel* m_pXaisTempLab;
    QSpinBox* m_pXaisTempSpbox;
    QPushButton* m_pXYaisChageApplyBtn;
    QPushButton* m_pCurveClearBtn;
    QLabel* m_pCaliFactDescrLab;
    QDoubleSpinBox* m_pCaliFactDpbox;
    QPushButton* m_pCaliFactChgeBtn;

    QGroupBox* m_pTempCtrlGpbox;
    QGroupBox* m_pTempRecTabGpbox;
    QWidget* m_pTempViewWidget;
    QLabel* m_pTempCurDescrLab;
    QLCDNumber* m_pTempCurDispLcd;
    QLabel* m_pTempCurDescrUnitrLab;
    QLabel* m_pTempCurTargetDescrLab;
    QLCDNumber* m_pTempCurTargetDescrLcd;
    QLabel* m_pTempCurTargetUnitrLab;

    QLabel* m_pTempTargDescrLab;
    QDoubleSpinBox* m_pTempTargSetDpbox;
    QPushButton* m_pTempTargetSetBtn;
    QTableWidget* m_pTempRecTableWdget;
    QPushButton* m_pTempRecTabClearBtn;

    QTimer m_pTimerUpdate;
    QPushButton* m_pTempCurveStartBtn;
    bool m_bNeedUpdate;     //实时显示标志
    int m_curveXaisTime;    //x轴时刻

    QTime m_currenTime;     //记录当前时间
    double m_lastPointKey; //记录最后一次画点的时间
    QSettings* m_config;

    QFile m_logFile;        //logfile指针
    QFile m_tempLogFile;        //logfile指针
    int m_tempTableRawCnt;
    int m_timeToClearCnt;

    double m_dFactorParam;

public:
    void LoadConfig(QSettings* config);
    void SaveConfig(QSettings* config);

    bool OpenPort(void);
    void ClosePort(void);
signals:
    void transmitData(const QByteArray& data);

    void SetCurrentTempSignal(double temp);

private:
    void InitPortCtrl(void);
    void InitInteractView(void);
    void InitTempDispView(void);

    void arrayToASCII(QString& str, const QByteArray& array);
    void arrayToHex(QString& str, const QByteArray& array);

    void SendData(void);
    void ShowMessage(const QString& rcvTime, const QString& data);

    void InitCurveView(void);
    void UpdateLatestTempToCurve(const double& temp);
    void UpdateLcdDisplay(const double& curTemp, const double& curTargetTemp);
    void InsertTempTable(const QString& dataTimeStr, const double& temp);
private slots:
    void ScanPort();
    void OnSwichFunctViewChange(int index);
    void OnPortConnectBtnSlot(void);
    void OnPortSettingBtnSlot(void);
    void UpdateSerialRcvInfoSlot(const QString rcvTime, const QByteArray rawData);
    void UpdateTemperaInfoSlot(const QString& dateTimeStr, double curTemp, double curTargTemp);
    void OnSendButtonClickedSlot(void);
    void ResendTimerSlot(void);
    void OnPeriodSendToggleSlot(bool checked);
    void RealtimeDataSlot();

    void OnClearTempTableBtnSlot(void);
    void OnTempTargetSetBtnSlot(void);
    void OnClickStartUpdateData(bool chk);
    void OnTempCalibFactorChgeSLot(void);
    void OnChooseLogFileBtnSlot(void);
    void OnEnableLoggingSlot(const bool& on);
    void OnChooseTempLogFileBtnSlot(void);
    void OnEnableTempLoggingSlot(const bool& on);
    void OnClearCurveBtnSlot(void);

};

