#include "RegisterTableModel.h"
#include <QColor>
#include <QBrush>
#include <QSqlQuery>
#include <QDebug>

static const int REGISTERS_NUM =219;

RegisterTableModel::RegisterTableModel(QObject * parent):QSqlTableModel(parent)
{
    m_vctbVueReadResult.resize(REGISTERS_NUM);
    for(auto && item : m_vctbVueReadResult)
    {
        item = true;
    }
}

Qt::ItemFlags RegisterTableModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if (index.column() != COLUMN_REG_VALUE)
        flags &= ~Qt::ItemIsEditable;
    return flags;
}


bool RegisterTableModel::setData( const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    const QVariant oldValue = QSqlTableModel::data(index, role);
        if (value == oldValue
            && value.isNull() == oldValue.isNull())
            return true;
    if(index.column() == COLUMN_REG_VALUE && role ==Qt::EditRole )
    {
        bool bResult =  QSqlTableModel::setData(index, value, Qt::EditRole);
        if(bResult)
        {
            emit RegsValueChangeSig(index,index);
            return bResult;
        }
    }

    return QSqlTableModel::setData(index, value, Qt::EditRole);
}


QVariant RegisterTableModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlTableModel::data(index, role);
//    if (value.isValid() && role == Qt::DisplayRole && \
//            (index.column() == COLUMN_FIELD_VALUE || index.column() == COLUMN_FIELD_DEFAULT_VALUE))
//            return value.toString().toUpper();

    if (role == Qt::ForegroundRole && index.column() == COLUMN_REG_VALUE && isDirty(index))
        return QVariant::fromValue(QColor(Qt::blue));

    if(role == Qt::BackgroundRole  && index.column() == COLUMN_REG_VALUE)
    {
        if(!m_vctbVueReadResult.at(index.row()))
        {
            return QVariant::fromValue(QColor(Qt::red));
        }
        else if(isDirty(index))
        {
            return QVariant::fromValue(QColor(Qt::yellow));
        }
        return QVariant::fromValue(QColor(127,255,170));
    }
    return value;
}

bool RegisterTableModel::UpdateVueReadResult(const bool &bState,const int &row)
{
    if(row >= m_vctbVueReadResult.size())
    {
        return false;
    }
    m_vctbVueReadResult[row] = bState;
    return true;
}
