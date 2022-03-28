#include "SerialPort.h"

#include <QDebug>
#include <QThread>
#include <QFile>
#include <QTime>
#include "Log.h"


SerialPort::SerialPort(QObject* parent) :QObject(parent)
{
    m_pSerial = new QSerialPort();

    qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
    connect(m_pSerial, &QSerialPort::readyRead, this, &SerialPort::ReceiveData);
    connect(m_pSerial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        this, &SerialPort::HandleError);
}

SerialPort::~SerialPort()
{
    if (m_pSerial)
    {
        if (m_pSerial->isOpen())
        {
            m_pSerial->close();
        }
        delete m_pSerial;
        m_pSerial = nullptr;
    }
}

/**
 * @brief 串口是否打开
 * @return 返回状态
 */
bool SerialPort::IsOpen(void)
{
    if (m_pSerial != nullptr)
    {
        return m_pSerial->isOpen();
    }
    return false;
}

/**
 * @brief 打开串口
 * @param portInfo 串口信息
 * @return 是否成功
 */
bool SerialPort::Open(const SerialSettingDialog::SerialSettings& portInfo)
{
    if (!m_pSerial->isOpen())
    {
        m_pSerial->setPortName(portInfo.com);
        m_pSerial->setBaudRate(portInfo.baudRate);
        m_pSerial->setDataBits(portInfo.dataBits);
        m_pSerial->setParity(portInfo.parity);
        m_pSerial->setStopBits(portInfo.stopBits);
        m_pSerial->setFlowControl(portInfo.flowControl);
        if (m_pSerial->open(QIODevice::ReadWrite))
        {
            LOGI("{} {} Connect success!  ",portInfo.name.toStdString(),portInfo.com.toStdString());
            return true;
        }
        else
        {
            LOGI("{} {} Connect failed!  ",portInfo.name.toStdString(),portInfo.com.toStdString());
            return false;
        }
    }
    else
    {
        LOGI("{} {} has already connect!  ",portInfo.name.toStdString(),portInfo.com.toStdString());
        return true;
    }
    return false;
}

/**
 * @brief SerialPort::Close 关闭串口
 */
void SerialPort::Close(void)
{
    if (IsOpen())
    {
        try {
            m_pSerial->close();
        } catch (std::exception& error)
        {
           LOGE("m_pSerial->close() wrong: {}",error.what());
           return;
        }
    }

}
void SerialPort::Clear(void)
{
    if (m_pSerial->isOpen())
    {
        m_pSerial->clear();
    }
}

/**
 * @brief 写数据到串口
 * @param 数据
 */
void SerialPort::WritePortData(const QByteArray& array)
{
    if (m_pSerial->isOpen() && !array.isEmpty())
    {
        int writeNum = 0;
        //        if((writeNum = m_pSerial->write(QByteArray::fromHex(array))) < -1)
        if ((writeNum = m_pSerial->write(array)) < -1)
        {
            qDebug() << "write failed";
        }
        //        qDebug()<<"serial writeNum"<<writeNum<<array<<"size"<<array.size();
        if (!m_pSerial->waitForBytesWritten())
        {
            qDebug() << "write failed";
        }
    }

}

/**
 * @brief 处理串口错误
 * @param 串口错误
 */
void SerialPort::HandleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
       LOGW("QSerialPort::SerialPortError code : {} ",error);

    }
}

/**
 * @brief 串口接收数据
 */
void SerialPort::ReceiveData(void)
{
    if (m_pSerial && m_pSerial->isOpen())
    {
        //        QByteArray dataByte = m_pSerial->readAll();
        //        cout<<dataByte.toStdString();
#if 0
        static QByteArray sumData;
        QByteArray tempData = m_pSerial->readAll();
        if (!tempData.isEmpty())
        {
            sumData.append(tempData);
            if (sumData.contains("\r\n")) // 检测到换行符
            {
                HandleRcvData(sumData);
                sumData.clear();
            }
        }
#endif
#if 1
        if( !m_pSerial->canReadLine())
            return;

        QByteArray tempData;
        tempData.clear();
        tempData = m_pSerial->readLine(2000);
        if (!tempData.isEmpty() && tempData.size() > 2)
        {
            if (tempData.contains("\r\n")) // 检测到换行符
            {
                HandleRcvData(tempData);
                tempData.clear();
            }
        }
#endif
    }
}

/**
 * @brief 初始化日志文件
 *
*/
void SerialPort::InitLogFile(QFile* logfile)
{
    if (logfile)
    {
        m_logFile = logfile;
    }
}

/**
 * @brief 初始化日志文件
 *
*/
void SerialPort::InitTempLogFile(QFile* logfile)
{
    if (logfile)
    {
        m_logTempFile = logfile;
    }
}

/**
 * @brief  处理接收后的数据
 * @param array 数据
 */
void SerialPort::HandleRcvData(const QByteArray& array)
{
    QString strTime = QTime::currentTime().toString(tr("[hh:mm:ss.zzz]"));
    emit UpdateSerialRcvInfoSig(strTime, array);     //打印接收数据

    double curTemp = 0.0, curTargTemp = 0.0;
    QString rawStr = array;
    if (TryToAcquireTemperature(rawStr, curTemp, curTargTemp)) //是否包含温度
    {
        QString dateTimeStr = QDateTime::currentDateTime().toString("[MM.dd hh:mm:ss]"); //设置显示格式
        emit UpdateTemperaInfoSig(dateTimeStr, curTemp, curTargTemp); //更新温度信息
        if (m_logTempFile && m_logTempFile->isOpen())    //输出温度日志
        {
            QString logStr = dateTimeStr + QString(" %1\n").arg(curTemp);
            m_logTempFile->write(logStr.toLocal8Bit());
        }
    }

    if (m_logFile && m_logFile->isOpen())        //输出串口原始数据日志
    {
        QByteArray logArray = strTime.toLatin1() + array;
        m_logFile->write(logArray);
    }

}

/**
 * @brief 尝试解析温度数据包
 * @param str   数据
 * @param curTemp   当前温度
 * @param curTargTemp   当前目标温度
 * @return
 */
bool SerialPort::TryToAcquireTemperature(const QString& str, double& curTemp, double& curTargTemp)
{
    bool res = false;
    if (str.contains("PV") && str.contains("SV"))
    {
        int indexOfPV = str.indexOf("PV");
        int indexOfSV = str.indexOf("SV");
        if ((indexOfSV - indexOfPV == 8) && (str.size() >= indexOfSV + 5))
        {
            QString tempStr = str.mid(indexOfPV + 2, indexOfSV - indexOfPV - 2).trimmed();
            curTemp = tempStr.toDouble(&res);
            tempStr = str.mid(indexOfSV + 2, 5).trimmed();
            curTargTemp = tempStr.toDouble(&res);
        }
    }
    return res;

}
