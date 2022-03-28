#ifndef     __DATAPACKAGE_MODE_H          
#define     __DATAPACKAGE_MODE_H

#include "models/DataDefine.h"
class Point;
typedef unsigned short ushort;


QByteArray ProtocolModule_SampSet(char channel,unsigned int frequency,bool isAllchannel);
QByteArray ProtocolModule_DiectCurrentSet(ushort channel,ushort amplitude,bool isAllchannel);
QByteArray ProtocolModule_PulseSet(ushort channel,ushort amplitude,bool isAllchannel);
QByteArray ProtocolModule_SquareSet(ushort channel,ushort amplitude,ushort frequency);
QByteArray ProtocolModule_TriangularSet(ushort channel,ushort amplitude,ushort frequency);



double ProtocolModule_DacHexToVolageDouble(const short int inData,const double referVolt,\
                                           const int maxDacNumRange);
short int ProtocolModule_DacValueToVoltageHex(const double inData,const double referVolt,\
                                              const int maxDacNumRange);
ushort doubleTransformToDacHex(const double &value);

double CalculateCurrent(double voltage, double resistence);
bool Module_DataDecode(const QByteArray *qbyteRawData,QVector<QList <short int>> &vctListCh,int &reCnt);
bool Module_deCodeToOneGroup(QVector<QList <short int>> &vctListCh, short int *oneGroup);
bool Module_takeOnePointFormula(const short int *rawData, double *reData);
bool Module_takeOnePointFormula(const short int *rawData,const double &nSlope,const double &nOffset,double *reData);
bool Module_takeOneGroupFormula(const short int *rawData,double *ReData);
bool Module_TakeOneGroupToBuffer(const double *rawData, double *ReData);


bool Module_GetLineFit(const QVector<Point>&vctPoints,double *reSlop, double *reOffset, QString *reString);
bool Module_GetRMS(const QVector<double> &vctVolt,double *result);
bool Module_GetAvarge(const QVector<double> &vctVolt,double *result);


#endif
