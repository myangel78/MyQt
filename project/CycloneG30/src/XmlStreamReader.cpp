#include "XmlStreamReader.h"
#include <QFile>
#include <iostream>
#include <QDebug>
#include <QString>

XmlStreamReader::XmlStreamReader(IXmlTransformCallback *callback):m_pIXmlTransCallback(callback)
{

}


bool XmlStreamReader::ReadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        std::cerr << "Error: Cannot read file " << qPrintable(fileName)
                  << ": " << qPrintable(file.errorString())
                  << std::endl;
        return false;
    }
    reader.setDevice(&file);

    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isStartElement())
        {
            if (reader.name() == "Registers")
            {
                ReadRegisterMembers();
            }
            else
            {
                reader.raiseError(QObject::tr("Not a Registers file"));
            }
        }
        else
        {
            reader.readNext();
        }
    }

    file.close();
    if (reader.hasError())
    {
        std::cerr << "Error: Failed to parse file "
                  << qPrintable(fileName) << ": "
                  << qPrintable(reader.errorString()) << std::endl;
        return false;
    }
    else if (file.error() != QFile::NoError)
    {
        std::cerr << "Error: Cannot read file " << qPrintable(fileName)
                  << ": " << qPrintable(file.errorString())
                  << std::endl;
        return false;
    }
    return true;
}


void XmlStreamReader::PrintAllMembers()
{
    qDebug() << "All Registers: ";
    for (const auto& regStr : m_vctRegs)
    {
        Cyclone::Print(regStr);
    }
}



void XmlStreamReader::ReadRegisterMembers(void)
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if (reader.isStartElement())
        {
            if (reader.name() == "Register")
            {

                ReadRegister(reader.attributes().value("id"));
            }
            else
            {
                SkipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}


void XmlStreamReader::ReadFieldMembers(QVector<Cyclone::RegField> &vctField,const int &regId)
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if (reader.isStartElement())
        {
            if (reader.name() == "Field")
            {
                ReadFieldInfo(reader.attributes().value("id"),vctField,regId);
            }
            else
            {
                SkipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}



void XmlStreamReader::ReadRegister(const QStringRef& regIdStr)
{
    reader.readNext();

    int regId = regIdStr.toString().toInt();

    QString name;
    QString addr;
    QString defalVue;
    int size = 32;
    QString value;
    QString fieldTabName;
    QVector<Cyclone::RegField> vctField;

    while (!reader.atEnd())
    {
       if (reader.isEndElement())
       {
           reader.readNext();
           break;
       }

       if (reader.isStartElement())
       {
           if (reader.name() == "Reg_Name")
           {
               name = reader.readElementText();
           }
           else if (reader.name() == "Address")
           {
               addr = reader.readElementText();
           }
           else if (reader.name() == "Default_Value")
           {
               defalVue = reader.readElementText();
           }
           else if (reader.name() == "Size")
           {
               size = reader.readElementText().toInt();
           }
           else if (reader.name() == "Value")
           {
               value = reader.readElementText();
           }
           else if (reader.name() == "AllField")
           {
               fieldTabName = reader.attributes().value("Table_Name").toString();
               ReadFieldMembers(vctField,regId);
           }
           else
           {
               SkipUnknownElement();
           }
       }
       reader.readNext();
    }

    Cyclone::RegisterStr regStr(regId, name, addr, defalVue, size,value,fieldTabName,vctField);
    m_vctRegs.push_back(std::move(regStr));
}


void XmlStreamReader::ReadFieldInfo(const QStringRef& fieldIdStr,QVector<Cyclone::RegField> &vctField,const int &regId)
{
    reader.readNext();

    int fieldId = fieldIdStr.toString().toInt();
    QString fieldName;
    QString mode;
    QString defalValStr;
    QString valueStr;
    QString bitsStr;
    QString descrStr;


    while (!reader.atEnd())
    {
       if (reader.isEndElement())
       {
           reader.readNext();
           break;
       }

       if (reader.isStartElement())
       {
           if (reader.name() == "Field_name")
           {
               fieldName = reader.readElementText();
           }
           else if (reader.name() == "Bits")
           {
               bitsStr = reader.readElementText();
           }
           else if (reader.name() == "Mode")
           {
               mode = reader.readElementText();
           }
           else if (reader.name() == "Default_Value")
           {
               defalValStr = reader.readElementText();
           }
           else if (reader.name() == "Value")
           {
               valueStr = reader.readElementText();
           }
           else if (reader.name() == "Description")
           {
               descrStr = reader.readElementText();
           }
           else
           {
               SkipUnknownElement();
           }
       }
       reader.readNext();
    }

    Cyclone::RegField filed(fieldId,fieldName,mode,defalValStr,bitsStr,valueStr,descrStr,regId);
    vctField.push_back(std::move(filed));

}


void XmlStreamReader::SkipUnknownElement()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if (reader.isStartElement())
        {
            SkipUnknownElement();
        }
        else
        {
            reader.readNext();
        }
    }
}


bool XmlStreamReader::UpLoadDataToRegsTable(void)
{
    for( auto &&regItem :m_vctRegs)
    {
         m_pIXmlTransCallback->WriteElementRegToDataBase(regItem);
    }
    return true;
}
