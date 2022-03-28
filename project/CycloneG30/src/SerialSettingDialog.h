#pragma once

#include <QDialog>
#include <QtSerialPort/QSerialPort>

class QLabel;
class QComboBox;
class QPushButton;
class QGroupBox;


class SerialSettingDialog : public QDialog
{
    Q_OBJECT

public:
    typedef struct Settings {
        QString com;
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    }SerialSettings;


    explicit SerialSettingDialog(QWidget* parent = Q_NULLPTR);
    ~SerialSettingDialog();

    void OnUpdateParam(void);
    SerialSettings Settings() const;

private:
    SerialSettings m_pCurrentSettings;

    QGroupBox* m_pSettingsGrpbox = nullptr;

    QLabel* m_pDataBitLab = nullptr;
    QLabel* m_pPriorityLab = nullptr;
    QLabel* m_pStopbitLab = nullptr;
    QLabel* m_pFlowControlLab = nullptr;

    QComboBox* m_pDataBitComb = nullptr;
    QComboBox* m_pPriorityComb = nullptr;
    QComboBox* m_pStopbitComb = nullptr;
    QComboBox* m_pFlowControlComb = nullptr;

    QPushButton* m_pUpdateBtn = nullptr;

private slots:
    void OnClickOkBtnSlot(void);


private:
    void InitCtrl(void);


};

