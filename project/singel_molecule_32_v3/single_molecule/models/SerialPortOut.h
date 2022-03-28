#ifndef SERIALPORTOUT_H
#define SERIALPORTOUT_H

#include <QObject>
#include<QSerialPort>
#include "view/serialsettingsdialog.h"
#include"logger/Log.h"

class SerialPortOut:public QObject
{
    Q_OBJECT
public:
    SerialPortOut(QObject *parent = nullptr);
    ~SerialPortOut();
    void closePort();
signals:
    void qMsgPortInfoSig(QString info);

public slots:
    void openPortSolt(SerialSettingsDialog::Settings p);
    void writePortData(const QByteArray &array);
    void ClearPort(void);

private slots:
    void handleError(QSerialPort::SerialPortError error);

private:

    QSerialPort *serial;

};

#endif // SERIALPORTOUT_H
