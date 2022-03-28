#pragma once
#include <QTableView>

class RegFieldTableModel;
class CustomDelegate;

class RegFieldTableView:public QTableView
{
    Q_OBJECT
public:
    RegFieldTableView(RegFieldTableModel *model,QWidget *parent = nullptr);
    ~RegFieldTableView();

    void SetModelPtr(RegFieldTableModel *model){m_pModel = model;}

    void ResetView(void);

private:
    RegFieldTableModel *m_pModel = nullptr;
private:
    void InitCtr(void);

    bool ParseBitStrToNum(const QString &bitsStr,int &rHigBits,int &rLowBits, int &rDiff);
    uint32_t TruncateVueByBits(const int &value,const int &bitsCnt, const int &bitOffset);

public:
    QModelIndex GetRegVueIndexByCurIndex(const QModelIndex &curIndex);
    QModelIndex GetRegVueIndexByRow(const int &row);
    QString GetRegVueStrByCurIndex(const QModelIndex &curIndex);
    QString GetRegDefVueStrByRow(const int &row);
    QString GetRegVueStrByRow(const int &row);
    bool GetRegVueByRow(const int &row,uint32_t &reVue);
    bool GetBitByRow(const int &row,int &rHigBits,int &rLowBits, int &rDiff);
    bool GetRegId(int &rId);

    bool SetRegVueStrByRow(const QString &strVue,const int &row);

    bool ModifyAllFieldVueByRegVue(const uint &value,const QString &tableName);
    bool GenerRegVueByAllField(uint32_t &rValue,int &rId);

    bool OnSubmitAll(void);
    bool OnResetAllDef(void);

};

