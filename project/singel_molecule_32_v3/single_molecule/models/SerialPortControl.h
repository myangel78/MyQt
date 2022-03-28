#ifndef SERIALPORTCONTROL_H
#define SERIALPORTCONTROL_H
#include<QThread>
#include<QSerialPort>
#include "view/serialsettingsdialog.h"
#include"logger/Log.h"
#include<mutex>

class SerialPortControl:public QThread
{
    Q_OBJECT
public:
    SerialPortControl();
    ~SerialPortControl();
signals:
    void qMsgPortInfoSig(QString info);

public slots:
    void openPortSolt(SerialSettingsDialog::Settings p);
    void stopThread();
    void writeCmdSlot(int cmd);
    void writePortData(const QByteArray &array);
    void ClearPort(void);
    void closePort();

private:
    void run();
    void readDataThread_demoTest();

private slots: 
    void handleError(QSerialPort::SerialPortError error);
    void startReadData();

private:
    QSerialPort *serial;
    bool m_bRunReadThread = false;
    std::atomic<bool> m_bStartRead = false;
    std::mutex m_Mutex;
    std::condition_variable m_Condition;
    uint16_t m_nRawSemCnt = 0;
    LARGE_INTEGER frequency, startCount, stopCount,stopCount1;
    LONGLONG elapsed,elapsed1;

    std::vector<LONGLONG> m_recordTime;
};

#endif // SERIALPORTTHREAD_H
