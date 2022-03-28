#include "RegFieldTableModel.h"
#include <qlineedit.h>
#include <QStyledItemDelegate>
#include <QDebug>
#include <QSqlRecord>

RegFieldTableModel::RegFieldTableModel(QObject * parent):QSqlTableModel(parent)
{

}

Qt::ItemFlags RegFieldTableModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if (index.column() != COLUMN_FIELD_VALUE)
        flags &= ~Qt::ItemIsEditable;
    return flags;
}

bool RegFieldTableModel::setData( const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    const QVariant oldValue = QSqlTableModel::data(index, role);
        if (value == oldValue
            && value.isNull() == oldValue.isNull())
            return true;

    return QSqlTableModel::setData(index, value, Qt::EditRole);
}

QVariant RegFieldTableModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlTableModel::data(index, role);
//    if (value.isValid() && role == Qt::DisplayRole && \
//            (index.column() == COLUMN_FIELD_VALUE || index.column() == COLUMN_FIELD_DEFAULT_VALUE))
//            return value.toString().toUpper();

    if (role == Qt::ForegroundRole && index.column() == COLUMN_FIELD_VALUE && isDirty(index))
        return QVariant::fromValue(QColor(Qt::blue));

    if(role == Qt::BackgroundRole  && index.column() == COLUMN_FIELD_VALUE)
    {
        if(isDirty(index))
        {
            return QVariant::fromValue(QColor(Qt::yellow));
        }
        return QVariant::fromValue(QColor(127,255,170));
    }
    return value;
}


