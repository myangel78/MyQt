#include "DataPackageMode.h"

#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <QList>
#include "models/DataDefine.h"
#include "helper/Point.h"

QByteArray qbyteArrStart("55aa0111ddbb");
QByteArray qbyteArrStop("55aa0117ddbb");
QByteArray qbyteArrEnd("ddbb");

//32通道 采样速率设置： 55 AA 01 16 26 20 00 00 频率高8位 频率低8位 DD BB;
//单通道 采样速率设置： 55 AA 01 16 25 通道数 (8位) 00 频率高 8位 频率 中 8位 频率 低 8位 DD BB;
//直流电平统一设置： 55 AA 01 12 22 00 20 幅值高 8位 幅值低 8位 DD BB;
//直流电平独立设置： 55 AA 01 12 21 通道数高8位 通道数低8位 幅值高8位 幅值低8位 DD BB;
//脉冲电平独立设置： 55 AA 01 14 23 通道数高8位 通道数低8位 幅值高8位 幅值低8位 DD BB;
//脉冲电平统一设置： 55 AA 01 14 24 00 20 幅值高8位 幅值低8位 DD BB;
//方波电平统一设置： 55 AA 01 15 幅值高8位 幅值低8位 频率高8位 频率低8位 DD BB;
//三角波电平统一设置 ：55 AA 01 13 幅值高8位 幅值低8位 频率高8位 频率低8位 DD BB;
QByteArray qbyteArrAllChSampFreqHead("55aa011626200000");
QByteArray qbyteArrChSampFreqHead("55aa011625");
QByteArray qbyteArrAllChDcHead("55aa0112220020");
QByteArray qbyteArrSingelChDcHead("55aa011221");
QByteArray qbyteArrAllChSingelPulseHead("55aa011423");
QByteArray qbyteArrAllChPulseHead("55aa0114240020");
QByteArray qbyteArrAllChSquareHead("55aa0113");
QByteArray qbyteArrAllChTriangularHead("55aa0115");

QByteArray ProtocolModule_SampSet(char channel,uint frequency,bool isAllchannel)
{
    if(isAllchannel)
    {
        QByteArray qBtyefreq = QString("%1").arg(frequency, 4, 16, QChar('0')).toUtf8();
        return qbyteArrAllChSampFreqHead + qBtyefreq + qbyteArrEnd;
    }
    else
    {
        QByteArray qBtyefreq   = QString("%1").arg(frequency, 6, 16, QChar('0')).toUtf8();
        QByteArray qBtyechanel = QString("%1").arg(channel, 2, 16, QChar('0')).toUtf8();
        return qbyteArrChSampFreqHead +qBtyechanel +"00"+ qBtyefreq + qbyteArrEnd;
    }
}



QByteArray ProtocolModule_DiectCurrentSet(ushort channel,ushort amplitude,bool isAllchannel)
{
    QByteArray qBtyeAmpl = QString("%1").arg(amplitude, 4, 16, QChar('0')).toUtf8();
    if(isAllchannel)
        return qbyteArrAllChDcHead + qBtyeAmpl + qbyteArrEnd;
    else
    {
        QByteArray qBytech = QString("%1").arg(channel, 4, 16, QChar('0')).toUtf8();
        return qbyteArrSingelChDcHead + qBytech +qBtyeAmpl + qbyteArrEnd;
    }
}



QByteArray ProtocolModule_PulseSet(ushort channel,ushort amplitude,bool isAllchannel)
{
    QByteArray qBtyeAmpl = QString("%1").arg(amplitude, 4, 16, QChar('0')).toUtf8();
    if(isAllchannel)
        return qbyteArrAllChPulseHead + qBtyeAmpl + qbyteArrEnd;
    else
    {
        QByteArray qBytech = QString("%1").arg(channel, 4, 16, QChar('0')).toUtf8();
        return qbyteArrAllChSingelPulseHead + qBytech +qBtyeAmpl + qbyteArrEnd;
    }
}


QByteArray ProtocolModule_SquareSet(ushort channel,ushort amplitude,ushort frequency)
{
    QByteArray qBtyeAmpl = QString("%1").arg(amplitude, 4, 16, QChar('0')).toUtf8();
    QByteArray qBtyeFreq = QString("%1").arg(frequency, 4, 16, QChar('0')).toUtf8();
    return qbyteArrAllChSquareHead + qBtyeAmpl + qBtyeFreq + qbyteArrEnd;
}


QByteArray ProtocolModule_TriangularSet(ushort channel,ushort amplitude,ushort frequency)
{
    QByteArray qBtyeAmpl = QString("%1").arg(amplitude, 4, 16, QChar('0')).toUtf8();
    QByteArray qBtyeFreq = QString("%1").arg(frequency, 4, 16, QChar('0')).toUtf8();
    return qbyteArrAllChTriangularHead + qBtyeAmpl + qBtyeFreq + qbyteArrEnd;
}


/**
* @brief                   DAC hex值转换成电压
* @param   inData          输入HEX
* @param   refervolt       参考电压V
* @param   maxDacNumRange  65536
* @return  double         电压十进制数值
*/
double ProtocolModule_DacHexToVolageDouble(const short int inData,const double referVolt,const int maxDacNumRange)
{
    int nHalfDacRange = maxDacNumRange/2;        //    maxDacNumRange 65536 , nHalfDacRange 32768
    double tempValue =(inData*1.0)/(nHalfDacRange*1.0);   //inData是输入ADChex值
    double reslult = tempValue *referVolt;

//    qDebug()<<"nHalfDacRange"<<nHalfDacRange<<"inData"<<inData<<"tempValue"<<tempValue<<"reslult"<<reslult;
    return reslult;
}

/**
* @brief                    电压double转换成DAC hex
* @param  inData            输入doubel
* @param  refervolt         参考电压2.5V
* @param  maxDacNumRange    65536
* @return short int         输出Hex
*/
short int ProtocolModule_DacValueToVoltageHex(const double inData,const double referVolt,const int maxDacNumRange)
{
     double absValue = 2*referVolt;
     return (inData - ( 0 - referVolt))*maxDacNumRange/absValue;
}


/**
* @brief			输入电压转换DAC hex值
* @parameter        输入电压值
* @return			void
*/
ushort doubleTransformToDacHex(const double &value)
{
    double maxValue = 2*DAC_REFVOLTAGE;
    double result = (value - ( 0 - DAC_REFVOLTAGE))*65536/maxValue;
    return result;
}


/**
* @brief                   计算电流
* @param    voltage         电压
* @param    resistence      电阻
* @return	double          电流
*/
double CalculateCurrent(double voltage, double resistence)
{
    return voltage/resistence;
}

/**
* @brief                   解码32全通道原始数据7帧
* @param    qbyteRawData   串口数据
* @param    vctListCh      所有通道数据的容器
* @param    reCnt          帧数
* @return	bool           成功与否
*/
bool Module_DataDecode(const QByteArray *qbyteRawData,QVector<QList <short int>> &vctListCh,int &reCnt)
{
    if(!qbyteRawData)
        return false;
    uint16_t nRawCH = 0, nRealCH = 0, datalen = 0, frameNum = 0;
    datalen = qbyteRawData->size();
    const char *data = qbyteRawData->constData();
    for(uint16_t ch = 0, index = 0; index < datalen; index = index+2, ++ch)
    {
        if (ch == FRAME_DATA_TOTAL_CNT)
        {
            ch = 0;
        }
        nRawCH = (ch & CHANNEL_NUM_END_INDEX);
        #ifndef DEMO_TEST
            nRealCH = CHANNEL_TRANSLATION_TABLE[nRawCH];
        #else
            nRealCH = nRawCH;
        #endif

//        uchar low = (*qbyteRawData).at(index);
//        uchar high = (*qbyteRawData).at(index+1);
//        int temp =  (high<<8)+ low;
//        temp = (temp >= 32768) ? (temp -65536):temp;
//         vctListCh[nRealCH].push_back(temp);

//        vctListCh[nRealCH].push_back(static_cast<short int>(uchar(qbyteRawData->at(index)) + ((uchar(qbyteRawData->at(index+1)) << 8))));
        vctListCh[nRealCH].append(static_cast<short int>(uchar(*(data+index)) + ((uchar(*(data + index+1))) << 8)));


        if(nRawCH == CHANNEL_NUM_END_INDEX)
        {
            frameNum++;
        }
    }
    reCnt = frameNum;
    return true;
}

/**
* @brief                   解码32全通道至1帧数据
* @param    vctListCh      所有通道数据的容器
* @param    oneGroup       32个通道一帧的缓存
* @return	bool           成功与否
*/
bool Module_deCodeToOneGroup(QVector<QList <short int>> &vctListCh, short int *oneGroup)
{
    if(!oneGroup)
        return false;
    for(int ch = 0; ch < vctListCh.size(); ch++)
    {
        if(vctListCh[ch].size() > 0)
        {
            *(oneGroup + ch) = vctListCh[ch].takeFirst();
        }
        else
        {
            return false;
        }
    }
    return true;
}

/**
* @brief                   1点公式化
* @param    rawData        1点原始数据
* @param    ReData         返回1点公式化
* @return	bool           成功与否
*/
bool Module_takeOnePointFormula(const short int *rawData, double *reData)
{
    // y = kx + b;
    if(!rawData || !reData)
        return false;
     *reData = (double)*rawData;
//       *reData = ProtocolModule_DacHexToVolageDouble(*rawData,DACMAX_FIVEVOLT,DACMAX_BITRANGE_SIXTEEN)*1000;
//     *reData = (*rawData* ADC_LSB)* 3.01 * 0.0005 + (rand()%500);
      return true;
}

/**
* @brief                   1点公式化
* @param    rawData        1点原始数据
* @param    ReData         返回1点公式化
* @return	bool           成功与否
*/
bool Module_takeOnePointFormula(const short int *rawData,const double &nSlope,const double &nOffset,double *reData)
{
    // y = kx + b;
    if(!rawData || !reData)
        return false;
//     *reData = *rawData;
       double nAdcVol = ProtocolModule_DacHexToVolageDouble(*rawData,DACMAX_FIVEVOLT,DACMAX_BITRANGE_SIXTEEN)*1000;
       *reData = (nAdcVol - nOffset)/nSlope;
//        *reData = nAdcVol;

//       *reData = ProtocolModule_DacHexToVolageDouble(*rawData,DACMAX_FIVEVOLT,DACMAX_BITRANGE_SIXTEEN)*1000;
      return true;
}

/**
* @brief                   1帧数据公式转化
* @param    rawData        1帧原始数据
* @param    ReData         返回1帧数据公式后
* @return	bool           成功与否
*/
bool Module_takeOneGroupFormula(const short int *rawData,double *ReData)
{
    if(!rawData || !ReData)
        return false;
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        if(!Module_takeOnePointFormula(rawData +ch, ReData + ch))
            return false;
    }
    return true;
}

/**
* @brief                   1帧数据拷贝至double数组
* @param    rawData        1帧原始数据
* @param    ReData         1帧缓冲数组
* @return	bool           成功与否
*/
bool Module_TakeOneGroupToBuffer(const double *rawData, double *ReData)
{
    if(!rawData || !ReData)
        return false;
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        *(ReData + ch) = *(rawData + ch);
    }
    return true;
}


/**
* @brief                   最小二乘法拟合直线
* @param    vctPoints      离散数据点
* @param    reSlop         返回斜率
* @param    reOffset       返回偏移值
* @param    reString       返回公式字符串
* @return	bool           返回成功
*/
bool Module_GetLineFit(const QVector<Point> &vctPoints,double *reSlop, double *reOffset, QString *reString)
{
    if(!reSlop || !reOffset || !reString)
        return false;
    double m_xx = 0;
    double m_xSum = 0;
    double m_xy = 0;
    double m_ySum = 0;
    int pointSize = vctPoints.size();
    if(!vctPoints.isEmpty())
    {
        for(int i = 0; i < vctPoints.size(); i++)
        {
           m_xx   += vctPoints[i].GetX() * vctPoints[i].GetX();
           m_xSum += vctPoints[i].GetX();
           m_xy   += vctPoints[i].GetX() * vctPoints[i].GetY();
           m_ySum += vctPoints[i].GetY();
        }
        *reSlop = (m_xy *pointSize - m_xSum * m_ySum) / (m_xx * pointSize - m_xSum * m_xSum);
        *reOffset= (m_xx * m_ySum - m_xSum * m_xy) /(m_xx * pointSize - m_xSum * m_xSum);
        *reString = QString("y = %1x + %2").arg(*reSlop,0,'g',6).arg(*reOffset,0,'g',6);
        return true;
    }
    return false;
}


bool Module_GetRMS(const QVector<double> &vctVolt,double *result)
{
    if(!result)
        return false;
    double sum =0;
    for(int i = 0; i <vctVolt.size(); i++)
    {
        sum += (vctVolt.at(i) * vctVolt.at(i));
    }
    *result = sqrt(sum/vctVolt.size());
    return true;
}

bool Module_GetAvarge(const QVector<double> &vctVolt,double *result)
{
    if(!result)
        return false;
    double sum =0;
    for(int i = 0; i <vctVolt.size(); i++)
    {
        sum += vctVolt.at(i);
    }
    *result = sum/vctVolt.size();
    return true;
}




