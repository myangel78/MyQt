#include "SerialPort.h"

#include <QDebug>
#include <QThread>
#include <QFile>
#include <QTime>


SerialPort::SerialPort(QObject* parent) :QObject(parent)
{
    m_pSerial = new QSerialPort();

    m_logFile = nullptr;

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
            m_pSerial->close();
        delete m_pSerial;
    }
}

/**
 * @brief �����Ƿ��
 * @return ����״̬
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
 * @brief �򿪴���
 * @param portInfo ������Ϣ
 * @return �Ƿ�ɹ�
 */
bool SerialPort::Open(const SerialSettingDialog::SerialSettings& portInfo)
{
    bool result = false;
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
            result = true;
            qDebug() << "Serial port" << portInfo.com << " Connect success!" << portInfo.name;
        }
        else
        {
            result = false;
            qDebug() << "Serial port" << portInfo.com << " Connect failed!" << portInfo.name;
        }
    }
    else
    {
        result = false;
        qDebug() << "Serial port" << portInfo.com << " has already connect" << portInfo.name;
    }
    return result;

}

/**
 * @brief SerialPort::Close �رմ���
 */
void SerialPort::Close(void)
{
    if (IsOpen())
    {
        m_pSerial->close();
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
 * @brief д���ݵ�����
 * @param ����
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
 * @brief �����ڴ���
 * @param ���ڴ���
 */
void SerialPort::HandleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        qDebug() << m_pSerial->errorString() << m_pSerial->portName();

    }
}

/**
 * @brief ���ڽ�������
 */
void SerialPort::ReceiveData(void)
{
    if (m_pSerial)
    {
        //        QByteArray dataByte = m_pSerial->readAll();
        //        cout<<dataByte.toStdString();
#if 0
        static QByteArray sumData;
        QByteArray tempData = m_pSerial->readAll();
        if (!tempData.isEmpty())
        {
            sumData.append(tempData);
            if (sumData.contains("\r\n")) // ��⵽���з�
            {
                HandleRcvData(sumData);
                sumData.clear();
            }
        }
#endif
#if 1
        if( !m_pSerial->canReadLine())
            return;
        QByteArray tempData = m_pSerial->readLine(2000);
        if (!tempData.isEmpty())
        {
            if (tempData.contains("\r\n")) // ��⵽���з�
            {
                HandleRcvData(tempData);
                tempData.clear();
            }
        }
#endif
    }
}

/**
 * @brief ��ʼ����־�ļ�
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
 * @brief ��ʼ����־�ļ�
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
 * @brief  ������պ������
 * @param array ����
 */
void SerialPort::HandleRcvData(const QByteArray& array)
{
    QString strTime = QTime::currentTime().toString(tr("[hh:mm:ss.zzz]"));
    emit UpdateSerialRcvInfoSig(strTime, array);     //��ӡ��������

    double curTemp = 0.0, curTargTemp = 0.0;
    QString rawStr = array;
    if (TryToAcquireTemperature(rawStr, curTemp, curTargTemp)) //�Ƿ�����¶�
    {
        QString dateTimeStr = QDateTime::currentDateTime().toString("[MM.dd hh:mm:ss]"); //������ʾ��ʽ
        emit UpdateTemperaInfoSig(dateTimeStr, curTemp, curTargTemp); //�����¶���Ϣ
        //if (m_logTempFile && m_logTempFile->isOpen())    //����¶���־
        //{
        //    QString logStr = dateTimeStr + QString(" %1\n").arg(curTemp);
        //    m_logTempFile->write(logStr.toLocal8Bit());
        //}
    }

    if (m_logFile && m_logFile->isOpen())        //�������ԭʼ������־
    {
        QByteArray logArray = strTime.toLatin1() + array;
        m_logFile->write(logArray);
    }

}

/**
 * @brief ���Խ����¶����ݰ�
 * @param str   ����
 * @param curTemp   ��ǰ�¶�
 * @param curTargTemp   ��ǰĿ���¶�
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
