#pragma once

#include <QObject>
#include <QStyledItemDelegate>
#include <QSpinBox>


static bool ParseBitsStrDiff(const QString &bitsStr,char &rDiff);


class HexSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    HexSpinBox(QWidget *parent = nullptr);
    unsigned int hexValue() const { return u(value()); }
    void setHexValue(unsigned int value) { setValue(i(value)); }
    void setRange(unsigned int max);
    bool fillField() const { return m_fillField; }
    void setFillField(bool fillFieldWidth) { m_fillField = fillFieldWidth; }


protected:
    QString textFromValue(int value) const;
    int valueFromText(const QString &text) const;
    QValidator::State validate(QString &input, int &pos) const;

private:
    unsigned int m_maxRange = UINT_MAX;
    bool m_fillField = true;
    inline unsigned int u(int i) const { return *reinterpret_cast<unsigned int *>(&i); }
    inline int i(unsigned int u) const { return *reinterpret_cast<int *>(&u); }
};


class CustomDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CustomDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class RegFiledValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit RegFiledValueDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

class AFETIADelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit AFETIADelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};



