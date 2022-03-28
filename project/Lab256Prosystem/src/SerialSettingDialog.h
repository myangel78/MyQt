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

    QGroupBox* m_pSettingsGrpbox;

    QLabel* m_pDataBitLab;
    QLabel* m_pPriorityLab;
    QLabel* m_pStopbitLab;
    QLabel* m_pFlowControlLab;

    QComboBox* m_pDataBitComb;
    QComboBox* m_pPriorityComb;
    QComboBox* m_pStopbitComb;
    QComboBox* m_pFlowControlComb;

    QPushButton* m_pUpdateBtn;

private slots:
    void OnClickOkBtnSlot(void);


private:
    void InitCtrl(void);


};

