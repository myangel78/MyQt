#include "SqlDataBase.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QMessageBox>
#include <QFileInfo>
#include <QSqlDriver>


SqlDataBase::SqlDataBase(QObject *parent):QObject(parent)
{

}

SqlDataBase::~SqlDataBase()
{
    QSqlDatabase db = QSqlDatabase::database(m_dataBaseName);
    if(db.isOpen())
    {
        db.close();
    }
}

void SqlDataBase::InitDataBase(const QString &dataPath)
{
    QFileInfo fileInfo(dataPath);
    QString dataBaseName =  fileInfo.baseName();
    m_dataBaseName = dataBaseName;
    m_dataBasePath = dataPath;

    if (QSqlDatabase::contains(dataBaseName)) {

           QSqlDatabase db = QSqlDatabase::database(dataBaseName);
           m_bDbIsOpen = db.open();
       }
       else {

//           QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","test");
           QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//           m_dataBase.setDatabaseName(dataBaseName+".db");
           db.setDatabaseName(m_dataBasePath);
           m_bDbIsOpen = db.open();
       }

    if(!m_bDbIsOpen)
    {
        QMessageBox::critical(0, "Cannot open database",\
                QString("Unable to establish a database %1 connection.").arg(dataBaseName), QMessageBox::Cancel);
    }
}


bool SqlDataBase::DeleteDataBase(const QString &dataBasePath)
{
    {
        QSqlDatabase db = QSqlDatabase::database(m_dataBaseName);
        if(db.isOpen())
        {
            db.commit();
            db.close();
            m_bDbIsOpen = false;
            qDebug()<<"db.isValid"<<db.isValid();
        }
    }

    QString name;
    {
        name = QSqlDatabase::database().connectionName();
    }//超出作用域，隐含对象QSqlDatabase::database()被删除。
    qDebug()<<name;
    {
        //qt_sql_default_connection
        QSqlDatabase::removeDatabase(m_dataBaseName);
        QSqlDatabase::removeDatabase(name);
    }



    QFile fileTemp(dataBasePath);
    if(!fileTemp.exists())
        return true;
    return fileTemp.remove();
}



bool SqlDataBase::InitRegsTable(const QString &tableName)
{
    if (!m_bDbIsOpen)
    {
        return false;
    }

    if (!IsExistTable(tableName)) {
        return CreateRegsTable(tableName);
    }
    return true;
}

bool SqlDataBase::InitRegsFieldTable(const QString &tableName)
{
    if (!m_bDbIsOpen)
    {
        return false;
    }

    if (!IsExistTable(tableName)) {
        return CreateFieldTable(tableName);
    }
    return true;
}


bool SqlDataBase::IsExistTable(const QString &table)
{
    bool bRet = false;
    if (!m_bDbIsOpen)
    {
        return bRet;
    }

    QSqlQuery query;
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(table));    //关键的判断
    if (query.next())
    {
        if (query.value(0).toInt() > 0)
        {
            bRet = true;
        }
    }
    return bRet;
}


bool SqlDataBase::IsExistRecord(const QString &tableName,const QString &filed, const QString &fieldName)
{
    bool bResult = false;
    if (!m_bDbIsOpen)
    {
        return bResult;
    }

    QSqlQuery query;
    bResult = query.exec(QString("select count(*) from %1 where %2 = '%3';").arg(tableName).arg(filed).arg(fieldName));
    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("Check IsExistRecord query fialed name: %s failed,error: %s",qPrintable(tableName),qPrintable(lastError.driverText()));
        return bResult;
    }

    bResult = false;
    while(query.next())
    {
        if(query.isActive())
        {
            if(query.value(0).toInt() > 0)
            {
                qDebug("query reg name %s is exist",fieldName.toStdString().c_str());
                bResult = true;
            }
        }
    }
    return bResult;
}


bool SqlDataBase::CreateRegsTable(const QString &tableName)
{
    bool bResult = false;
    if (!m_bDbIsOpen)
    {
        return bResult;
    }

    QSqlQuery query;
    bResult = query.exec(QString("create table %1 ("
                         "Id INTEGER IDENTITY, "
                         "Reg_Name varchar(40) NOT NULL, "
                         "Address varchar(40) NOT NULL, "
                         "Size int NOT NULL, "
                         "DefaultVue varchar(40) NOT NULL,"
                         "Value varchar(40) NOT NULL,"
                         "Field_Table_Name varchar(40) NOT NULL,"
                                 "PRIMARY KEY (Id))").arg(tableName));

    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("CreateRegsTable name: %s failed,error: %s",qPrintable(tableName),qPrintable(lastError.driverText()));
    }
    return bResult;
}

bool SqlDataBase::CreateFieldTable(const QString &tableName)
{
    bool bResult = false;
    if (!m_bDbIsOpen)
    {
        return bResult;
    }

//    (Fid,FieldName,Bits,Mode,DefaultVue,Value,RegId);

    QSqlQuery query;
    bResult = query.exec(QString("create table %1 ("
                         "Fid int primary key, "
                         "FieldName varchar(40) NOT NULL, "
                         "Bits varchar(40) NOT NULL, "
                         "Mode varchar(40) NOT NULL, "
                         "DefaultVue varchar(40) NOT NULL,"
                         "Value varchar(40) NOT NULL,"
                         "Description varchar(200) NOT NULL,"
                         "RegId int NOT NULL)").arg(tableName));

    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("%s name: %s failed,error: %s",__FUNCTION__,qPrintable(tableName),qPrintable(lastError.driverText()));
    }
    return bResult;

}

bool SqlDataBase::DeleteTable(const QString &tableName)
{
    bool bResult = false;
    if (!m_bDbIsOpen)
    {
        return bResult;
    }

    QSqlQuery query;
    bResult = query.exec(QString("DROP TABLE %1 ").arg(tableName));

    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("%s name: %s failed,error: %s",__FUNCTION__,qPrintable(tableName),qPrintable(lastError.driverText()));
    }
    return bResult;
}


bool SqlDataBase::InsertRegsData(const QString &tableName,const RegisterStr &regStr)
{
    if (!m_bDbIsOpen) {
        return false;
    }

    QSqlQuery query;
    bool bResult = false;
    bResult = query.exec(QString("INSERT INTO %1 (Id,Reg_Name,Address,Size,DefaultVue,Value,Field_Table_Name) "
        "VALUES (%2, '%3', '%4', %5 ,'%6', '%7', '%8')").arg(tableName)
                              .arg(regStr.id)
                              .arg(regStr.name)
                              .arg(regStr.addr)
                              .arg(regStr.size)
                              .arg(regStr.defalVue)
                              .arg(regStr.value)
                              .arg(regStr.fieldTabStr));
    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("InsertData name: %s failed,error: %s",qPrintable(tableName),qPrintable(lastError.driverText()));
    }
    return bResult;
}


bool SqlDataBase::InsertRegSqlTable(const QString &tableName,const RegIdItemSimp rowItem)
{
    if (!m_bDbIsOpen) {
        return false;
    }

    QSqlQuery query;
    query.prepare(QString("insert into %1 (Id,Reg_Name,Address,Size,DefaultVue,Value,Field_Table_Name) values(?,?,?,?,?,?,?)").arg(tableName));
    query.addBindValue(rowItem.id);
    query.addBindValue(rowItem.name);
    query.addBindValue(rowItem.addr);
    query.addBindValue(rowItem.size);
    query.addBindValue(rowItem.defalVue);
    query.addBindValue(rowItem.value);
    query.addBindValue(rowItem.fieldTabStr);

    bool bResult = query.exec();
    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("InsertData name: %s failed,error: %s",qPrintable(tableName),qPrintable(lastError.driverText()));
    }
    return bResult;
}



bool SqlDataBase::InsertFieldInfo(const QString &fieldTabName,const RegField &fieldStr)
{
    if (!m_bDbIsOpen) {
        return false;
    }

    QSqlQuery query;
    bool bResult = false;
    bResult = query.exec(QString("INSERT INTO %1 (Fid,FieldName,Bits,Mode,DefaultVue,Value,Description,RegId) "
        "VALUES (%2, '%3', '%4', '%5' ,'%6', '%7', '%8',%9)").arg(fieldTabName)
                              .arg(fieldStr.fieldId)
                              .arg(fieldStr.fieldName)
                              .arg(fieldStr.bitsStr)
                              .arg(fieldStr.mode)
                              .arg(fieldStr.defalValStr)
                              .arg(fieldStr.valueStr)
                              .arg(fieldStr.descrStr)
                              .arg(fieldStr.regId));
    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("%s name: %s failed,error: %s",__FUNCTION__,qPrintable(fieldTabName),qPrintable(lastError.driverText()));
    }
    return bResult;
}

void SqlDataBase::UpLoadOneRegsInfoToDb(const QString &tableName,const RegisterStr &regStr)
{
    if (!m_bDbIsOpen) {
        return ;
    }
    InsertRegsData(tableName,regStr);

    bool bResult = true;
    QString fieldTabName = regStr.fieldTabStr;
    if(!IsExistTable(fieldTabName))
    {
        bResult = CreateFieldTable(fieldTabName);
    }
    if(bResult)
    {
        for( auto &&filedItem :regStr.vctField)
        {
            InsertFieldInfo(fieldTabName,filedItem);
        }
    }
    else
    {
        qDebug()<<__FUNCTION__<<"CreatFieldTable failed"<<fieldTabName;
    }

}



bool SqlDataBase::ModifyOneRegsInfoToDb(const QString &tableName,const RegIdItemSimp &info)
{
    if (!m_bDbIsOpen) {
        return false;
    }

    QSqlQuery query;
    query.prepare(QString("update %1 set Reg_Name = ?,Address = ?,DefaultVue = ?,Value = ?,Field_Table_Name = ?  where Id = ? ").arg(tableName));
    query.addBindValue(info.name);
    query.addBindValue(info.addr);
    query.addBindValue(info.defalVue);
    query.addBindValue(info.value);
    query.addBindValue(info.fieldTabStr);
    query.addBindValue(info.id);
    bool bResult = query.exec();
    if(!bResult)
    {
        QSqlError lastError = query.lastError();
        qDebug("%s name: %s failed,error: %s",__FUNCTION__,qPrintable("Registers"),qPrintable(lastError.driverText()));
    }
    return bResult;
}


bool SqlDataBase::UpdateRegVueByFiledSubmit(const QString tableName,const uint &value,const int &Rid)
{
    if (!m_bDbIsOpen) {
        return false;
    }
    QSqlQuery query;
    QString valueHexStr = QString("%1").arg(value, 8 , 16, QLatin1Char('0')).toUpper();
    query.prepare(QString("update %1 set Value = ? where Id = ?").arg(tableName));
    query.addBindValue(valueHexStr);
    query.addBindValue(Rid);
    return query.exec();
}

