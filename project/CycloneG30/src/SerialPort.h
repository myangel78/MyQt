#pragma once

#include <QObject>
#include "SerialSettingDialog.h"

class QFile;

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject* parent = NULL);
    ~SerialPort();

    bool IsOpen(void);
    void Close(void);
    bool Open(const SerialSettingDialog::SerialSettings& portInfo);
    void Clear(void);

    void InitLogFile(QFile* logfile);
    void InitTempLogFile(QFile* logfile);

public slots:
    void WritePortData(const QByteArray& array);

private slots:
    void HandleError(QSerialPort::SerialPortError error);
    void ReceiveData(void);
signals:
    void UpdateSerialRcvInfoSig(const QString rcvTimeStr, const QByteArray array);
    void UpdateTemperaInfoSig(const QString dataTimeStr, const double curTemp, const double curTargTemp);

private:
    QSerialPort* m_pSerial =nullptr;
    QFile* m_logFile = nullptr;
    QFile* m_logTempFile = nullptr;

private:
    void HandleRcvData(const QByteArray& array);
    bool TryToAcquireTemperature(const QString& str, double& curTemp, double& curTargTemp);

};

