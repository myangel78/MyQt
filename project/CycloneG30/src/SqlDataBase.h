#pragma once

#include <QObject>
#include <QString>
#include "Global.h"

using namespace Cyclone;


class SqlDataBase :public QObject
{
    Q_OBJECT
public:
    SqlDataBase(QObject *parent = nullptr);
    ~SqlDataBase();

public:
    void InitDataBase(const QString &dataBaseName);
    bool InitRegsTable(const QString &tableName);
    bool InitRegsFieldTable(const QString &tableName);
    bool InsertRegsData(const QString &tableName,const RegisterStr &regStr);
    bool InsertRegSqlTable(const QString &tableName,const RegIdItemSimp rowItem);
    bool UpdateRegVueByFiledSubmit(const QString tableName,const uint &value,const int &Rid);
    void UpLoadOneRegsInfoToDb(const QString &tableName,const RegisterStr &regStr);
    bool ModifyOneRegsInfoToDb(const QString &tableName,const RegIdItemSimp &info);
    bool DeleteTable(const QString &tableName);
    bool CreateFieldTable(const QString &tableName);
    bool DeleteDataBase(const QString &dataBasePath);


private:
    bool IsExistTable(const QString &tableName);
    bool CreateRegsTable(const QString &tableName);
    bool IsExistRecord(const QString &tableName,const QString &filed, const QString &fieldName);
    bool InsertFieldInfo(const QString &fieldTabName,const RegField &fieldStr);


private:
    QString m_dataBasePath;
    QString m_dataBaseName;
    bool m_bDbIsOpen;

};

