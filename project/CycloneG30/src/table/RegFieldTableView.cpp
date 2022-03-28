#include "RegFieldTableView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>

#include "Global.h"
#include "RegFieldTableModel.h"

RegFieldTableView::RegFieldTableView(RegFieldTableModel *model,QWidget *parent):QTableView(parent),m_pModel(model)
{
    InitCtr();
}

RegFieldTableView::~RegFieldTableView()
{

}

void RegFieldTableView::InitCtr(void)
{
    horizontalHeader()->setStretchLastSection(true);
//    horizontalHeader()->setMinimumSectionSize(95);
//    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setColumnHidden(RegFieldTableModel::COLUMN_FIELD_REG_ID,true);
    setAlternatingRowColors(true);
    verticalHeader()->setHidden(true);

    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_ID);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_NAME);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_BITS);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_MODE);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_DEFAULT_VALUE);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_DESCRIPTION);
    setColumnWidth(RegFieldTableModel::COLUMN_FIELD_VALUE,100);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setStyleSheet(QTableView_QSS_ALtColor);
}


void RegFieldTableView::ResetView(void)
{
    setColumnHidden(RegFieldTableModel::COLUMN_FIELD_REG_ID,true);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_ID);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_NAME);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_BITS);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_MODE);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_DEFAULT_VALUE);
    resizeColumnToContents(RegFieldTableModel::COLUMN_FIELD_DESCRIPTION);
    setColumnWidth(RegFieldTableModel::COLUMN_FIELD_VALUE,100);
}


bool RegFieldTableView::GetRegId(int &rId)
{
    bool bRet = false;
    rId = m_pModel->data(m_pModel->index(0,RegFieldTableModel::COLUMN_FIELD_REG_ID)).toInt(&bRet);
    return bRet;
}

/**
 * @brief 获得Field table特定行的default value
 * @param row
 * @return
 */
QString RegFieldTableView::GetRegDefVueStrByRow(const int &row)
{
    QModelIndex index = m_pModel->index(row,RegFieldTableModel::COLUMN_FIELD_DEFAULT_VALUE);
    if(index.isValid())
    {
        return m_pModel->data(index).toString();
    }
    return QString();
}

/**
 * @brief 获得Field table特定行的RegVlue
 * @param row
 * @return
 */
bool RegFieldTableView::GetRegVueByRow(const int &row,uint32_t &reVue)
{
    bool bRet = false;
    QString valueStr = GetRegVueStrByRow(row);
    if(!valueStr.isEmpty())
    {
        reVue = valueStr.toUInt(&bRet,16);
    }
    return bRet;
}


/**
 * @brief 获得Field table特定行的RegVlue
 * @param row
 * @return
 */
QString RegFieldTableView::GetRegVueStrByRow(const int &row)
{
    QModelIndex index = m_pModel->index(row,RegFieldTableModel::COLUMN_FIELD_VALUE);
    if(index.isValid())
    {
        return m_pModel->data(index).toString();
    }
    return QString();
}

/**
 * @brief 获得Field table的当前索引 的RegVue
 * @return
 */
QString RegFieldTableView::GetRegVueStrByCurIndex(const QModelIndex &curIndex)
{
    QModelIndex index = curIndex.siblingAtColumn(RegFieldTableModel::COLUMN_FIELD_VALUE);
    if(index.isValid())
    {
        return m_pModel->data(index).toString();
    }
    return QString();
}

/**
 * @brief 获得Field table的当前RegVueIndex;
 * @return
 */
QModelIndex RegFieldTableView::GetRegVueIndexByCurIndex(const QModelIndex &curIndex)
{
   return curIndex.siblingAtColumn(RegFieldTableModel::COLUMN_FIELD_VALUE);
}

/**
 * @brief 获得Field table的特定行的RegVueIndex;
 * @param row
 * @return
 */
QModelIndex RegFieldTableView::GetRegVueIndexByRow(const int &row)
{
   return m_pModel->index(row,RegFieldTableModel::COLUMN_FIELD_VALUE);
}



bool RegFieldTableView::GetBitByRow(const int &row,int &rHigBits,int &rLowBits, int &rDiff)
{
    QString bitStr = m_pModel->index(row,RegFieldTableModel::COLUMN_FIELD_BITS).data().toString();

    return ParseBitStrToNum(bitStr,rHigBits,rLowBits,rDiff);
}

bool RegFieldTableView::SetRegVueStrByRow(const QString &strVue,const int &row)
{
      QModelIndex index =  GetRegVueIndexByRow(row);
      if(index.isValid())
      {
          return m_pModel->setData(index,strVue,Qt::EditRole);
      }
      return false;
}



/**
 * @brief 解析bitt字符串为数字
 * @param bitsStr 字符串
 * @param rHigBits 高位
 * @param rLowBits 低位
 * @param rDiff    高低位差值
 * @return
 */
bool RegFieldTableView::ParseBitStrToNum(const QString &bitsStr,int &rHigBits,int &rLowBits, int &rDiff)
{
    QStringList bitList = bitsStr.split(":",Qt::SkipEmptyParts);
    int size = bitList.size();
    if(size < 1 || size > 2 )
        return false;

    bool bResult = false;
    if(size == 1)
    {
        int bits = bitList.at(0).toInt(&bResult,10);
        if(bResult)
        {
            rHigBits = bits;
            rLowBits = bits;
            rDiff = 0;
            return true;
        }
        return false;
    }else if(size  == 2)
    {
        bResult = false;
        int hBits = bitList.at(0).toInt(&bResult,10);
        if(!bResult)
        {
            return false;
        }
        rHigBits = hBits;
        bResult = false;
        int lBits = bitList.at(01).toInt(&bResult,10);
        if(!bResult)
        {
            return false;
        }
        rLowBits = lBits;
        rDiff = hBits - lBits;
        return true;
    }
    return false;
}


bool RegFieldTableView::ModifyAllFieldVueByRegVue(const uint &value,const QString &tableName)
{
    if(m_pModel->tableName() != tableName)
    {
        return false;
    }
    int higBits = 0,lowBits = 0, diff =0;
    int rowCount  = m_pModel->rowCount();
    uint32_t fieldVue = 0;
    bool bResult  = false;
    QString fieldVueStr;
    for(int row = 0; row < rowCount; ++row)
    {
        if(GetBitByRow(row,higBits,lowBits,diff))
        {
             fieldVue = TruncateVueByBits(value,diff,lowBits);
             fieldVueStr = QString("%1").arg(fieldVue ,diff/4+1,16, QLatin1Char('0')).toUpper();
             QModelIndex vueIndex =  GetRegVueIndexByRow(row);
             bResult = m_pModel->setData(vueIndex,fieldVueStr,Qt::EditRole);
             if(!bResult)
                 return false;
        }
    }
    return true;
}

uint32_t RegFieldTableView::TruncateVueByBits(const int &value,const int &bitsCnt, const int &bitOffset)
{
    return ((value & (PowOfTwo[bitsCnt] << bitOffset)) >> bitOffset);
}


bool RegFieldTableView::GenerRegVueByAllField(uint32_t &rValue,int &rId)
{
    int higBits = 0,lowBits = 0, diff =0;
    int rowCount  = m_pModel->rowCount();
    for(int row = 0; row< rowCount; ++row)
    {
        if(!GetBitByRow(row,higBits,lowBits,diff))
        {
            return false;
        }
        uint32_t fieldVue = 0;
        if(!GetRegVueByRow(row,fieldVue))
        {
            return false;
        }
        rValue |= fieldVue << lowBits;
    }

    if(!GetRegId(rId))
    {
        return false;
    }
    QString valueHexStr = QString("%1").arg(rValue, 8 , 16, QLatin1Char('0'));
    qDebug("Final generate Value is %d, Hex Value is %s",rValue,qPrintable(valueHexStr));
    return true;
}


bool RegFieldTableView::OnSubmitAll(void)
{
    m_pModel->database().transaction();//开始事务操作
    if (m_pModel->submitAll()) {
        return m_pModel->database().commit();//提交
    } else {
        m_pModel->database().rollback();//回滚
        QMessageBox::warning(this,tr("Error"),tr("Model : %1 failed!!! error : %2").arg(m_pModel->tableName()).arg(m_pModel->lastError().text()));
        return false;
    }
}

bool RegFieldTableView::OnResetAllDef(void)
{
    bool bRet =false;
    for(int row = 0; row < m_pModel->rowCount(); ++row)
    {
        QString defVue = GetRegDefVueStrByRow(row);
        bRet = SetRegVueStrByRow(defVue,row);
        if(!bRet)
        {
            return bRet;
        }
    }
    return true;
}
