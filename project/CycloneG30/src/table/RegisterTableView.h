#pragma once
#include <QTableView>

class RegisterTableModel;
class CustomDelegate;

class RegisterTableView:public QTableView
{
    Q_OBJECT
public:
    RegisterTableView(RegisterTableModel *model,QWidget *parent = nullptr);
    ~RegisterTableView();

    void SetModelPtr(RegisterTableModel *model){m_pModel = model;}

private:
    RegisterTableModel *m_pModel = nullptr;
private:
    void InitCtr(void);

public:
    void ResetView(void);

    QString GetFieldTableNameByIndex(const QModelIndex &index);
    QString GetFieldTableNameByRow(const int &row);

    QModelIndex GetRegVueIndexByCurIndex(const QModelIndex &curIndex);
    QModelIndex GetRegVueIndexByRow(const int &row);

    QString GetRegVueStrByCurIndex(const QModelIndex &curIndex);
    QString GetRegVueStrByRow(const int &row);


    bool SetRegVueByRow(const int &row,const uint32_t &value);
    bool GetRegVueByRow(const int &row,uint32_t &reVue);
    bool GetRegAddrByRow(const int &row,unsigned char &reAddr);
    bool GetRegAddrByIndex(const QModelIndex &index,unsigned char &reAddr);

    unsigned char GetRegAddrByRow(const int &row);

    QString GetRegDefVueStrByRow(const int &row);
    bool SetRegVueStrByRow(const QString &strVue,const int &row);
    bool OnResetAllDef(void);
    bool OnSubmitAll(void);


};

