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
    QLabel* m_pSearchPortNameLab =nullptr;
    QComboBox* m_pSearchPortComb=nullptr;
    QLabel* m_pPortBaudRateLab=nullptr;
    QComboBox* m_pPortBaudRateComb=nullptr;
    QPushButton* m_pPortSearchBtn =nullptr;
    QPushButton* m_pPortConnectBtn =nullptr;
    QPushButton* m_pPortSettingBtn =nullptr;
    QGroupBox* m_pSearchPortGpbox =nullptr;
    QComboBox* m_pSwithViewComb =nullptr;

    QGroupBox* m_pInteractViewGpbox =nullptr;
    QStackedWidget* m_pViewStackWidget =nullptr;

    QWidget* m_pSerialTextrrView =nullptr;
    QTextEdit* m_pRecvDataTextEdit =nullptr;
    QPushButton* m_pRxClearBtn =nullptr;

    QCheckBox* m_pLogToFileChk = nullptr;
    QLineEdit* m_pLogFilePathEdit = nullptr;
    QPushButton* m_pLogFileLoacatBtn = nullptr;
    QCheckBox* m_pLogFileAppendChk = nullptr;

    QLabel* m_pRecMaxlineCountLab  = nullptr;
    QSpinBox* m_pRecMaxlineCountSpbox = nullptr;
    QCheckBox* m_pTempLogToFileChk = nullptr;
    QLineEdit* m_pTempLogFilePathEdit = nullptr;
    QPushButton* m_pTempLogFileLoacatBtn = nullptr;
    QCheckBox* m_pTempLogFileAppendChk = nullptr;

    QTextEdit* m_pSendDataTextEdit = nullptr;
    QGroupBox* m_pRxSettingsGpbox = nullptr;
    QGroupBox* m_pTxSettingsGpbox = nullptr;
    QRadioButton* m_pTxAsciiRaBtn = nullptr;
    QRadioButton* m_pRxAsciiRaBtn = nullptr;
    QRadioButton* m_pTxHexRaBtn = nullptr;
    QRadioButton* m_pRxHexRaBtn = nullptr;
    QCheckBox* m_pIsTimerToSend = nullptr;
    QSpinBox* m_pTimerToSendSpbox = nullptr;
    QPushButton* m_pTxSendBtn = nullptr;
    QPushButton* m_pTxClearBtn = nullptr;
    QComboBox* m_pTxHistoryComb = nullptr;
    QComboBox* m_pTxFormaTailComb = nullptr;
    QLabel* m_pRxMaxlineCountLab = nullptr;
    QSpinBox* m_pRxMaxlineCountSpbox = nullptr;
    QTimer* m_pResendTimer = nullptr;


    QCPItemText* m_pCurveCoorItem = nullptr;
    QCustomPlot* m_pTempCustomPlot = nullptr;
    QGroupBox* m_pTempCurveGpbox = nullptr;
    QLabel* m_pYaisMinTempLab = nullptr;
    QDoubleSpinBox* m_pYaisMinTempDpbox = nullptr;
    QLabel* m_pYaisMaxTempLab = nullptr;
    QDoubleSpinBox* m_pYaisMaxTempDpbox = nullptr;
    QLabel* m_pXaisTempLab = nullptr;
    QSpinBox* m_pXaisTempSpbox = nullptr;
    QPushButton* m_pXYaisChageApplyBtn = nullptr;
    QPushButton* m_pCurveClearBtn = nullptr;
    QLabel* m_pCaliFactDescrLab = nullptr;
    QDoubleSpinBox* m_pCaliFactDpbox = nullptr;
    QPushButton* m_pCaliFactChgeBtn = nullptr;

    QGroupBox* m_pTempCtrlGpbox = nullptr;
    QGroupBox* m_pTempRecTabGpbox = nullptr;
    QWidget* m_pTempViewWidget = nullptr;
    QLabel* m_pTempCurDescrLab = nullptr;
    QLCDNumber* m_pTempCurDispLcd = nullptr;
    QLabel* m_pTempCurDescrUnitrLab = nullptr;
    QLabel* m_pTempCurTargetDescrLab = nullptr;
    QLCDNumber* m_pTempCurTargetDescrLcd = nullptr;
    QLabel* m_pTempCurTargetUnitrLab = nullptr;

    QLabel* m_pTempTargDescrLab = nullptr;
    QDoubleSpinBox* m_pTempTargSetDpbox = nullptr;
    QPushButton* m_pTempTargetSetBtn = nullptr;
    QTableWidget* m_pTempRecTableWdget = nullptr;
    QPushButton* m_pTempRecTabClearBtn = nullptr;

    QTimer m_pTimerUpdate;
    int m_curveXaisTime;    //x轴时刻

    QTime m_currenTime;     //记录当前时间
    double m_lastPointKey; //记录最后一次画点的时间
    QSettings* m_config = nullptr;

    QFile m_logFile;        //logfile指针
    QFile m_tempLogFile;        //logfile指针
    int m_tempTableRawCnt;
    int m_timeToClearCnt;

    double m_dFactorParam;
    QString m_curTempStr;
    QString m_targetTempStr;

    SerialPort* m_pSerialPort = nullptr;
    SerialSettingDialog* m_pPortSettingDialog = nullptr;
    QThread* m_pSerialPortThread = nullptr;
public:
    void LoadConfig(QSettings* config);
    void SaveConfig(QSettings* config);

    bool OpenPort(void);
    void ClosePort(void);
signals:
    void transmitData(const QByteArray& data);

    void SetCurrentTempSignal(const QString &tempstr);

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
    void OnPortConnectBtnSlot(const bool &checked);
    void OnPortSettingBtnSlot(void);
    void UpdateSerialRcvInfoSlot(const QString rcvTime, const QByteArray rawData);
    void UpdateTemperaInfoSlot(const QString& dateTimeStr, double curTemp, double curTargTemp);
    void OnSendButtonClickedSlot(void);
    void ResendTimerSlot(void);
    void OnPeriodSendToggleSlot(bool checked);
    void RealtimeDataSlot();

    void OnClearTempTableBtnSlot(void);
    void OnTempTargetSetBtnSlot(void);
    void OnTempCalibFactorChgeSLot(void);
    void OnChooseLogFileBtnSlot(void);
    void OnEnableLoggingSlot(const bool& on);
    void OnChooseTempLogFileBtnSlot(void);
    void OnEnableTempLoggingSlot(const bool& on);
    void OnClearCurveBtnSlot(void);

};

