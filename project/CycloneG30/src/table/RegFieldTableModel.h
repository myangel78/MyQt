#pragma once
#include <QSqlTableModel>

class QStyleOptionViewItem;

class RegFieldTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit RegFieldTableModel(QObject * parent = nullptr);

    typedef enum
    {
        COLUMN_FIELD_ID = 0,
        COLUMN_FIELD_NAME,
        COLUMN_FIELD_BITS,
        COLUMN_FIELD_MODE,
        COLUMN_FIELD_DEFAULT_VALUE,
        COLUMN_FIELD_VALUE,
        COLUMN_FIELD_DESCRIPTION,
        COLUMN_FIELD_REG_ID,
    } ColumnALLFieldEnum;


public:
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


};


