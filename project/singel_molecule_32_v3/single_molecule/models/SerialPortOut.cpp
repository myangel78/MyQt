#include "SerialPortOut.h"
#include"DatacCommunicaor.h"
#include"logger/Log.h"
#include<QMessageBox>
#include "models/DataPackageMode.h"

SerialPortOut::SerialPortOut(QObject *parent)
{
    serial = new QSerialPort(this);

     connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &SerialPortOut::handleError);
}

SerialPortOut::~SerialPortOut()
{
    closePort();
    if(serial == nullptr)
         delete serial;
}

void SerialPortOut::openPortSolt(SerialSettingsDialog::Settings p)
{
    int nRet = SERIAL_OPEN_SUCCESS;
    #ifndef DEMO_TEST
        if(!serial->isOpen())
        {
            serial->setPortName(p.com);
            serial->setBaudRate(p.baudRate);
            serial->setDataBits(p.dataBits);
            serial->setParity(p.parity);
            serial->setStopBits(p.stopBits);
            serial->setFlowControl(p.flowControl);
            if (serial->open(QIODevice::ReadWrite))
            {
                nRet = SERIAL_OPEN_SUCCESS;
                LOGI("Serial port {} {} Connect success!",p.com.toStdString(),p.name.toStdString());
            }else
            {
                nRet = SERIAL_OPEN_FAIL;
                LOGI("Serial port {} {} Connect fail!",p.com.toStdString(),p.name.toStdString());
                emit qMsgPortInfoSig(tr("AO Port %1 is fail").arg(p.com));
            }
        }else
        {
            nRet = SERIAL_OPEN_ALREADY_EXIST;
            LOGE("Serial port {} {} is already running",p.com.toStdString(),p.name.toStdString());
        }
    #endif
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
}

void SerialPortOut::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        LOGE("Critical Error: {}.Please reconnect port {}", serial->errorString().toStdString(),serial->portName().toStdString());
        closePort();
        g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,SERIAL_RESOURCE_ERROR);

    }
}

void SerialPortOut::closePort()
{
    if (serial->isOpen())
    {
        LOGI("Close Serial port {}!",serial->portName().toStdString());
        qDebug("Close Serial port {}!",serial->portName().toStdString());
        serial->close();
    }
}

void SerialPortOut::ClearPort(void)
{
    if (serial->isOpen())
    {
       serial->clear();
       LOGI("Serial clearPort {}!");
    }
}


void SerialPortOut::writePortData(const QByteArray &array)
{
    int nRet = SERIAL_WRITE_CMD_PASS;
    if (serial->isOpen() && !array.isEmpty())
    {
        int writeNum =0;
        if((writeNum = serial->write(QByteArray::fromHex(array))) < -1)
        {
            nRet = SERIAL_WRITE_CMD_FAIL;
        }
//        qDebug()<<"serial writeNum"<<writeNum<<array;
        QThread::msleep(1); //避免同时发送指令下位机无法解析复合帧
        if(!serial->waitForBytesWritten())
        {
            nRet = SERIAL_WRITE_CMD_FAIL;
            qDebug()<<"write failed";
        }
    }
    if( nRet != SERIAL_WRITE_CMD_PASS)
        g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
}


