#pragma once
#include <QStyledItemDelegate>

class PlotCfgDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PlotCfgDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    QPixmap m_visiblePixmap;
    QPixmap m_inVisiblePixmap;
    QPixmap m_SeqChkPixmap;
    QPixmap m_unSeqChkPixmap;
    QPixmap m_validPixmap[2];
};
