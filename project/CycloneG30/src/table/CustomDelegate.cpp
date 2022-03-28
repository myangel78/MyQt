#include "CustomDelegate.h"
#include <QDebug>
//#include <QSqlRecord>


#include "Global.h"
#include "RegisterTableModel.h"
#include "RegFieldTableModel.h"



bool ParseBitsStrDiff(const QString &bitsStr,char &rDiff)
{
    QStringList bitList = bitsStr.split(":",Qt::SkipEmptyParts);
    int size = bitList.size();
    if(size < 1 || size > 2 )
        return false;

    bool bResult = false;
    if(size == 1)
    {
        rDiff = 0;
        return true;
    }else if(size  == 2)
    {
        bResult = false;
        int hBits = bitList.at(0).toInt(&bResult,10);
        if(!bResult)
        {
            return false;
        }
        bResult = false;
        int lBits = bitList.at(01).toInt(&bResult,10);
        if(!bResult)
        {
            return false;
        }
        rDiff = hBits - lBits;
        return true;
    }
    return false;
}


HexSpinBox::HexSpinBox(QWidget *parent) : QSpinBox(parent), m_maxRange(maximum())
{
    setPrefix("0x");
    setDisplayIntegerBase(16);
    setRange(UINT_MAX);
}

void HexSpinBox::setRange(unsigned int max)
{
    m_maxRange = max;
    if (m_maxRange <= INT_MAX) {
        QSpinBox::setRange(0, int(m_maxRange));
    } else {
        QSpinBox::setRange(INT_MIN, INT_MAX);
    }
}

QString HexSpinBox::textFromValue(int value) const
{
    int fillField = 0;
    if (m_fillField) {
        uint m = m_maxRange;
        while (m) {
            m >>= 4;
            ++fillField;
        }
    }
    return QString("%1").arg(u(value), fillField, 16, QLatin1Char('0')).toUpper();
}

int HexSpinBox::valueFromText(const QString &text) const
{
    return i(text.toUInt(nullptr, 16));
}

QValidator::State HexSpinBox::validate(QString &input, int &pos) const
{
    QString copy(input);
    QString pref = prefix();
    if (copy.startsWith(pref))
        copy.remove(pref);
    pos -= copy.size() - copy.trimmed().size();
    copy = copy.trimmed();
    if (copy.isEmpty())
        return QValidator::Intermediate;
    input = pref + copy.toUpper();
    bool okay;
    unsigned int val = copy.toUInt(&okay, 16);
    if (!okay || val > m_maxRange)
        return QValidator::Invalid;
    return QValidator::Acceptable;
}




CustomDelegate::CustomDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *CustomDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    if (index.column() == RegisterTableModel::COLUMN_REG_VALUE)
    {
        #if 1
        HexSpinBox *editor = new HexSpinBox(parent);
        return editor;
        #endif
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void CustomDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == RegisterTableModel::COLUMN_REG_VALUE)
    {
#if 1
        HexSpinBox * hexSpinbox = qobject_cast<HexSpinBox *>(editor);
        if(hexSpinbox)
        {
            const QString text = index.model()->data(index).toString();
            bool ok = false;
            uint value = text.toUInt(&ok,16);
            if(ok)
            {
                 hexSpinbox->setHexValue(value);
            }
            else
            {
                hexSpinbox->setHexValue(0);
            }
        }
#endif
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void CustomDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == RegisterTableModel::COLUMN_REG_VALUE)
    {
#if 1
        HexSpinBox * hexSpinbox = qobject_cast<HexSpinBox *>(editor);
        if(hexSpinbox)
        {
            uint value = hexSpinbox->hexValue();
            model->setData(index, QString("%1").arg(value, 8, 16, QLatin1Char('0')).toUpper());
        }
#endif
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}


void CustomDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}




RegFiledValueDelegate::RegFiledValueDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *RegFiledValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    if (index.column() == RegFieldTableModel::COLUMN_FIELD_VALUE)
    {
        HexSpinBox *editor = new HexSpinBox(parent);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void RegFiledValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == RegFieldTableModel::COLUMN_FIELD_VALUE)
    {
        HexSpinBox * pHexSpinBox = qobject_cast<HexSpinBox *>(editor);
        if(pHexSpinBox)
        {
            const QString text = index.model()->data(index).toString();
            bool ok = false;
            uint value = text.toUInt(&ok,16);
            if(ok)
            {
                 pHexSpinBox->setHexValue(value);
                 QString bitsStr  = index.model()->index(index.row(),RegFieldTableModel::COLUMN_FIELD_BITS).data().toString();
                 char bitDff = 0;
                 if(ParseBitsStrDiff(bitsStr,bitDff))
                 {
                     pHexSpinBox->setRange(PowOfTwo[bitDff]);
                 }
            }
            else
            {
                pHexSpinBox->setHexValue(0);
            }
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void RegFiledValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == RegFieldTableModel::COLUMN_FIELD_VALUE)
    {
        HexSpinBox * pHexSpinBox = qobject_cast<HexSpinBox *>(editor);
        if(pHexSpinBox)
        {
            uint value = pHexSpinBox->hexValue();
            QString bitsStr  = index.model()->index(index.row(),RegFieldTableModel::COLUMN_FIELD_BITS).data().toString();
            char bitDff = 0;
            if(ParseBitsStrDiff(bitsStr,bitDff))
            {
                QString fieldVueStr = QString("%1").arg(value ,bitDff/4+1,16, QLatin1Char('0')).toUpper();
                model->setData(index,fieldVueStr );
            }
//            model->setData(index, QString("%1").arg(value, 1, 16, QLatin1Char('0')).toUpper());
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}


void RegFiledValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

AFETIADelegate::AFETIADelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *AFETIADelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setRange(0,1);
    return editor;
}

void AFETIADelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index,Qt::EditRole).toInt();
    QSpinBox * pSpinBox = qobject_cast<QSpinBox *>(editor);
    pSpinBox->setValue(value);
}

void AFETIADelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
   QSpinBox *pSpinBox = static_cast<QSpinBox *>(editor);
   pSpinBox ->interpretText();
   int value = pSpinBox->value();
   model->setData(index,value,Qt::EditRole);
}


void AFETIADelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}




