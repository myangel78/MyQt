#pragma once

#include <QObject>
#include <QWidget>

class QLineEdit;
class QPushButton;
class QTextEdit;
class UsbFtdDevice;

class ComunicateTestWdgt :public QWidget
{
    Q_OBJECT
public:
    explicit ComunicateTestWdgt(QWidget *parent = nullptr);
    ~ComunicateTestWdgt();

    void OnSetUsbDevice(UsbFtdDevice *usbftddevice){ m_pUsbFtdDevice = usbftddevice;}

private:
    QLineEdit *m_pSendEdit;
    QPushButton *m_pSendButton;
    QPushButton *m_pOneKeyTestButton;
    QTextEdit *m_pRcvTextEdit;
    QPushButton *m_pReadButton;
    QPushButton *m_pRcvClearBtn;
    UsbFtdDevice *m_pUsbFtdDevice = nullptr;

private:
    void InitCtrl(void);

private slots:
    void OnSendBtnSlot(void);
    void OnReadBtnSlot(void);

};

