#pragma once
#include <QString>
#include <QXmlStreamReader>
#include "Global.h"

class RegisterItem;
class FieldItem;


class IXmlTransformCallback
{
public:
    virtual bool WriteElementRegToDataBase(const Cyclone::RegisterStr &elment) =  0;
};

class XmlStreamReader
{
public:
    XmlStreamReader(IXmlTransformCallback *callback);
    bool ReadFile(const QString& fileName);
    void PrintAllMembers();
    bool UpLoadDataToRegsTable(void);

private:
    void ReadRegisterMembers(void);
    void ReadRegister(const QStringRef& regIdStr);
    void ReadFieldMembers(QVector<Cyclone::RegField> &vctField,const int &regId);
    void ReadFieldInfo(const QStringRef& fieldIdStr,QVector<Cyclone::RegField> &vctField,const int &regId);
    void SkipUnknownElement();

private:
     QXmlStreamReader reader;
     QVector<Cyclone::RegisterStr> m_vctRegs;

     IXmlTransformCallback *m_pIXmlTransCallback = nullptr;
};

