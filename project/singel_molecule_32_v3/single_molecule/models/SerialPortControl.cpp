#include<QDebug>
#include "SerialPortControl.h"
#include"DatacCommunicaor.h"
#include"logger/Log.h"
#include<QMessageBox>
#include<thread>
#include "models/DataPackageMode.h"


using namespace std;
SerialPortControl::SerialPortControl()
{
   QueryPerformanceFrequency(&frequency);
   serial = new QSerialPort(this);

    #ifndef DEMO_TEST
    connect(serial, &QSerialPort::readyRead, this, &SerialPortControl::startReadData);
    #endif
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
           this, &SerialPortControl::handleError);
}

SerialPortControl::~SerialPortControl()
{
//    closePort();
}

void SerialPortControl::openPortSolt(SerialSettingsDialog::Settings p)
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
                emit qMsgPortInfoSig(tr("AI Port %1 is fail").arg(p.com));
            }
            writeCmdSlot(CMD_STOP);
        }else
        {

            nRet = SERIAL_OPEN_ALREADY_EXIST;
            LOGE("Serial port {} {} is already running",p.com.toStdString(),p.name.toStdString());
        }
    #endif
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
}


void SerialPortControl::startReadData()
{
    #ifndef DEMO_TEST
    int nByteCnt = 0;
    QueryPerformanceCounter(&stopCount1);
    if(g_pCommunicaor->m_sRawSem.freeBuf.available() < 10)
        qDebug()<<"g_pCommunicaor->m_sRawSem.freeBuf.available()"<<g_pCommunicaor->m_sRawSem.freeBuf.available();
    if(g_pCommunicaor->m_sRawSem.freeBuf.tryAcquire(1, SEM_TIMEOUT))
    {
        QueryPerformanceCounter(&stopCount);
        g_pCommunicaor->m_VctRawBuf[m_nRawSemCnt] = serial->readAll();
        nByteCnt = g_pCommunicaor->m_VctRawBuf[m_nRawSemCnt].size();

        g_pCommunicaor->m_sRawSem.usedBuf.release();
        m_nRawSemCnt = (m_nRawSemCnt == RAW_BUF_BLOCK_END_INDEX) ? 0 : (m_nRawSemCnt+1);


        elapsed = (stopCount.QuadPart - startCount.QuadPart) * 1000000 / frequency.QuadPart;
        elapsed1 = (stopCount1.QuadPart - startCount.QuadPart) * 1000000 / frequency.QuadPart;
        QueryPerformanceCounter(&startCount);
//        qDebug()<<"read = "<<elapsed1<<" "<<elapsed<<"us "<<nByteCnt<<" "<<g_pCommunicaor->m_VctRawBuf[m_nRawSemCnt].size();
    }else
    {
         writeCmdSlot(CMD_STOP);
        LOGI("Quit startReadData thread.");
        g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,THREAD_RAW_FREE_SEM_ACQ_TIMEOUT);
    }
    #endif
}


void SerialPortControl::run()
{
    LOGI("Start run serialport receive thread.");
    #ifndef DEMO_TEST
        exec();
    #else
        readDataThread_demoTest();
    #endif
}

void SerialPortControl::closePort()
{
    if (serial->isOpen())
    {
        if(m_bStartRead)
        {
            writeCmdSlot(CMD_STOP);
        }
        LOGI("Close Serial port {}!",serial->portName().toStdString());
        qDebug("Close Serial port {}!",serial->portName().toStdString());
        serial->close();
    }
}


void SerialPortControl::ClearPort(void)
{
    if (serial->isOpen())
    {
       serial->clear();
       LOGI("Serial clearPort {}!");
    }
}


void SerialPortControl::stopThread()
{
    m_bStartRead = false;
    closePort();
    LOGI("Quit SerialPortControl thread.");
    delete serial;
    this->thread()->exit();
}


void SerialPortControl::writeCmdSlot(int cmd)
{
    int nRet = SERIAL_WRITE_CMD_PASS;
    switch(cmd)
    {
        case CMD_START:
            if(!m_bStartRead)
            {
                m_nRawSemCnt = 0;
                m_bStartRead = true;
                #ifndef DEMO_TEST
                    serial->clear();
                #else
                    m_Condition.notify_all();
                #endif
                LOGI("Start read data.");
            }else
            {
                nRet = SERIAL_READ_ALREADY_START;
            }
            break;


        case CMD_STOP:
            m_bStartRead = false;
//            LOGI("Stop read data.");
            break;

         default:
            nRet = SERIAL_WRITE_CMD_ILLEGAL;
            LOGI("Cmd error!");
    }

    if (serial->isOpen())
    {
        int writeNum =0;

        if((writeNum = serial->write(CMD_LIST[cmd])) < -1)
        {
            nRet = SERIAL_WRITE_CMD_FAIL;
        }
        if(!serial->waitForBytesWritten())
        {
            nRet = SERIAL_WRITE_CMD_FAIL;
        }
    }
    if( nRet != SERIAL_WRITE_CMD_PASS)
        g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
}



void SerialPortControl::writePortData(const QByteArray &array)
{
    int nRet = SERIAL_WRITE_CMD_PASS;
    if (serial->isOpen() && !array.isEmpty())
    {
        int writeNum =0;
        if((writeNum = serial->write(QByteArray::fromHex(array))) < -1)
        {
            nRet = SERIAL_WRITE_CMD_FAIL;
        }
        qDebug()<<"serial control writeNum"<<writeNum<<array;
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


void SerialPortControl::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        LOGE("Critical Error: {}.Please reconnect port {}", serial->errorString().toStdString(),serial->portName().toStdString());
        closePort();
        g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,SERIAL_RESOURCE_ERROR);

    }
}


void SerialPortControl::readDataThread_demoTest()
{
    #ifdef DEMO_TEST
    LOGI("Start run readDataThread_demoTest thread.");
    LARGE_INTEGER frequency, startCount, stopCount;
    LONGLONG elapsed;
    QueryPerformanceFrequency(&frequency);

    QByteArray test;
    test.resize(FRAME_BYTE_CNT);
    for(uint16_t i = 0; i < FRAME_DATA_TOTAL_CNT; i++)
    {
        test[2*i] = 0;
        test[2*i + 1] = (i % CHANNEL_NUM);
    }
    std::unique_lock <std::mutex> lock(m_Mutex);
    if(!m_bStartRead)
        m_Condition.wait(lock);

    QueryPerformanceCounter(&startCount);
    while(m_bStartRead)
    {
        if(g_pCommunicaor->m_sRawSem.freeBuf.tryAcquire(1, RAW_SEM_TIMEOUT))
        {
            g_pCommunicaor->m_VctRawBuf[m_nRawSemCnt] = test;
            QueryPerformanceCounter(&stopCount);
            elapsed = (stopCount.QuadPart - startCount.QuadPart) * 1000000 / frequency.QuadPart;
            QueryPerformanceCounter(&startCount);
//            qDebug()<<"readTest--- = "<<elapsed<<"us"<<m_nRawSemCnt;  //5

            m_nRawSemCnt = (m_nRawSemCnt == RAW_BUF_BLOCK_END_INDEX) ? 0 : (m_nRawSemCnt+1);
            g_pCommunicaor->m_sRawSem.usedBuf.release();

        }else
        {
            g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,THREAD_RAW_FREE_SEM_ACQ_TIMEOUT);
            break;
        }
    }
    LOGI("Quit readDataThread_demoTest thread.");
    #endif
}
