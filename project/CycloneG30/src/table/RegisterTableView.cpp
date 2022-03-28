#include "RegisterTableView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>

#include "Global.h"
#include "RegisterTableModel.h"

RegisterTableView::RegisterTableView(RegisterTableModel *model,QWidget *parent):QTableView(parent),m_pModel(model)
{
    InitCtr();
}

RegisterTableView::~RegisterTableView()
{

}

void RegisterTableView::InitCtr(void)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
//    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    ResetView();
    setAlternatingRowColors(true);
    verticalHeader()->setHidden(true);
//    setStyleSheet("selection-background-color:LightSkyBlue;");
    setStyleSheet(QTableView_QSS_ALtColor);

}


/**
 * @brief 重新设定table的界面
 */
void RegisterTableView::ResetView(void)
{
//    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setColumnHidden(RegisterTableModel::COLUMN_ID,true);
    setColumnHidden(RegisterTableModel::COLUMN_REG_SIZE,true);
    setColumnHidden(RegisterTableModel::COLUMN_REG_FIELD_TAB_NAME,true);
    resizeColumnToContents(RegisterTableModel::COLUMN_ID);
    resizeColumnToContents(RegisterTableModel::COLUMN_REG_ADDR);
    resizeColumnToContents(RegisterTableModel::COLUMN_REG_NAME);
    resizeColumnToContents(RegisterTableModel::COLUMN_REG_SIZE);
    resizeColumnToContents(RegisterTableModel::COLUMN_REG_DEFAULT_VALUE);
    setColumnWidth(RegisterTableModel::COLUMN_REG_VALUE,100);
}


/**
 * @brief 获得当前索引行的FieldTable名称
 * @param index
 * @return
 */
QString RegisterTableView::GetFieldTableNameByIndex(const QModelIndex &index)
{
    QModelIndex filedIndex =  index.siblingAtColumn(RegisterTableModel::COLUMN_REG_FIELD_TAB_NAME);
    if(filedIndex.isValid())
    {
        return m_pModel->data(filedIndex).toString();
    }
    return QString();
}



QString RegisterTableView::GetFieldTableNameByRow(const int &row)
{
    QModelIndex index = m_pModel->index(row,RegisterTableModel::COLUMN_REG_FIELD_TAB_NAME);
    if(index.isValid())
    {
        return m_pModel->data(index).toString();
    }
    return QString();
}

bool RegisterTableView::SetRegVueByRow(const int &row,const uint32_t &value)
{
    QModelIndex index = GetRegVueIndexByRow(row);
    QString valueHexStr = QString("%1").arg(value, 8 , 16, QLatin1Char('0')).toUpper();
   return  m_pModel->setData(index,valueHexStr,Qt::EditRole);
}


/**
 * @brief 获得Registers table特定行的RegVlue
 * @param row
 * @return
 */
QString RegisterTableView::GetRegVueStrByRow(const int &row)
{
    QModelIndex index = m_pModel->index(row,RegisterTableModel::COLUMN_REG_VALUE);
    if(index.isValid())
    {
        return m_pModel->data(index).toString();
    }
    return QString();
}

bool RegisterTableView::GetRegVueByRow(const int &row,uint32_t &reVue)
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
 * @brief 获得Registers table的当前索引 的RegVue
 * @return
 */
QString RegisterTableView::GetRegVueStrByCurIndex(const QModelIndex &curIndex)
{
    QModelIndex index = curIndex.siblingAtColumn(RegisterTableModel::COLUMN_REG_VALUE);
    if(index.isValid())
    {
        return m_pModel->data(index).toString();
    }
    return QString();
}

/**
 * @brief 获得Registers table的当前RegVueIndex;
 * @return
 */
QModelIndex RegisterTableView::GetRegVueIndexByCurIndex(const QModelIndex &curIndex)
{
   return curIndex.siblingAtColumn(RegisterTableModel::COLUMN_REG_VALUE);
}

/**
 * @brief 获得Registers table的特定行的RegVueIndex;
 * @param row
 * @return
 */
QModelIndex RegisterTableView::GetRegVueIndexByRow(const int &row)
{
   return m_pModel->index(row,RegisterTableModel::COLUMN_REG_VALUE);
}


bool RegisterTableView::GetRegAddrByRow(const int &row,unsigned char &reAddr)
{
    bool bRet = false;
    QModelIndex addrIndex =  m_pModel->index(row,RegisterTableModel::COLUMN_REG_ADDR);
    if(addrIndex.isValid())
    {
        reAddr = m_pModel->data(addrIndex).toString().toUInt(&bRet,16) &0xff;
    }
    return bRet;
}

bool RegisterTableView::GetRegAddrByIndex(const QModelIndex &index,unsigned char &reAddr)
{
    QModelIndex addrIndex = index.siblingAtColumn(RegisterTableModel::COLUMN_REG_ADDR);
    bool bRet = false;
    if(addrIndex.isValid())
    {
        reAddr = m_pModel->data(addrIndex).toString().toUInt(&bRet,16) &0xff;
    }
    return bRet;
}

QString RegisterTableView::GetRegDefVueStrByRow(const int &row)
{
    QModelIndex index = m_pModel->index(row,RegisterTableModel::COLUMN_REG_DEFAULT_VALUE);
    if(index.isValid())
    {
        return m_pModel->data(index).toString();
    }
    return QString();
}

bool RegisterTableView::SetRegVueStrByRow(const QString &strVue,const int &row)
{
      QModelIndex index =  GetRegVueIndexByRow(row);
      if(index.isValid())
      {
          return m_pModel->setData(index,strVue,Qt::EditRole);
      }
      return false;
}


bool RegisterTableView::OnResetAllDef(void)
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


bool RegisterTableView::OnSubmitAll(void)
{
    m_pModel->database().transaction();//开始事务操作
    if (m_pModel->submitAll()) {
         m_pModel->database().commit();//提交
         return true;
    } else {
        m_pModel->database().rollback();//回滚
        QMessageBox::warning(this,tr("Error"),tr("Model : %1 failed!!! error : %2").arg(m_pModel->tableName()).arg(m_pModel->lastError().text()));
        return false;
    }
}

