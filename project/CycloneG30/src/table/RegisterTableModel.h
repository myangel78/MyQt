#pragma once

#include <QSqlTableModel>

class RegisterTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit RegisterTableModel(QObject * parent = nullptr);

    typedef enum
    {
        COLUMN_ID = 0,
        COLUMN_REG_NAME,
        COLUMN_REG_ADDR,
        COLUMN_REG_SIZE,
        COLUMN_REG_DEFAULT_VALUE,
        COLUMN_REG_VALUE,
        COLUMN_REG_FIELD_TAB_NAME,
    } ColumnFieldEnum;

signals:
    void RegsValueChangeSig(const QModelIndex &topLeft, const QModelIndex &bottomRight);


public:
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool UpdateVueReadResult(const bool &bState,const int &row);

private:
    QVector<bool> m_vctbVueReadResult;

};

