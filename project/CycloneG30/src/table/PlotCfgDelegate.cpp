#include "PlotCfgDelegate.h"

#include <QPainter>
#include <QColorDialog>
#include <QDebug>
#include <QCheckBox>
#include <QApplication>
#include <QMouseEvent>

#include "PlotCfgTabModel.h"

PlotCfgDelegate::PlotCfgDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_visiblePixmap = QPixmap(":/img/img/check.png").scaledToWidth(20,Qt::SmoothTransformation);
    m_inVisiblePixmap= QPixmap(":/img/img/checkun.png").scaledToWidth(20,Qt::SmoothTransformation);

    m_SeqChkPixmap = QPixmap(":/img/img/sequencechk.png").scaledToWidth(20,Qt::SmoothTransformation);
    m_unSeqChkPixmap= QPixmap(":/img/img/sequenceunchk.png").scaledToWidth(20,Qt::SmoothTransformation);
    m_validPixmap[0] =  QPixmap(":/img/img/chanelinvalid.png").scaledToWidth(20,Qt::SmoothTransformation);
    m_validPixmap[1] =  QPixmap(":/img/img/chanelvalid.png").scaledToWidth(20,Qt::SmoothTransformation);
}

QWidget *PlotCfgDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    if (index.column() == PLotCfgTabModel::Color)
    {
        QColorDialog *editor = new QColorDialog(parent);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void PlotCfgDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == PLotCfgTabModel::Color)
    {
         QColorDialog *dlg =  qobject_cast<QColorDialog *>(editor);
        if (dlg)
        {
            dlg->move(editor->parentWidget()->mapToGlobal(editor->pos())+ QPoint(editor->rect().width() + 20,0));
            const QVariant color = index.model()->data(index);
            dlg->setCurrentColor(color.value<QColor>());
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void PlotCfgDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == PLotCfgTabModel::Color)
    {
        QColorDialog *dlg =  qobject_cast<QColorDialog *>(editor);
       if (dlg)
       {
           model->setData(index,dlg->currentColor());
       }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void PlotCfgDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);
       initStyleOption(&viewOption, index);
       if (option.state.testFlag(QStyle::State_HasFocus))
           viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

    int column = index.column();
    switch (column)
    {
        case  PLotCfgTabModel::Color:
    {
        painter->save();
        const QVariant value = index.model()->data(index);
        QColor color = value.value<QColor>();
        QRect rect(option.rect);
    #if 1
        painter->fillRect(rect, color);
    #else
            QPen pen;
            pen.setColor(color);
            pen.setWidth(10);
            pen.setStyle(Qt::SolidLine);
            painter->setPen(pen);
            painter->drawLine(QPoint(rect.x() + 10, rect.y() + rect.height()/2), QPoint(rect.x() + rect.width() -10,rect.y()+ rect.height()/2));
    #endif
        painter->restore();
            return;
    }
        case PLotCfgTabModel::Visible:
      {
//          bool data = index.model()->data(index, Qt::EditRole).toBool();
//          QStyleOptionButton checkBoxStyle;
//          checkBoxStyle.state = data ? QStyle::State_On : QStyle::State_Off;
//          checkBoxStyle.state |= QStyle::State_Enabled;
//          checkBoxStyle.iconSize = QSize(20, 20);
//          checkBoxStyle.rect = option.rect;
//          QCheckBox checkBox;
//          checkBoxStyle.iconSize = QSize(20, 20);
//          checkBoxStyle.rect = option.rect;
//          QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter, &checkBox);

        QVariant var = index.model()->data(index,Qt::EditRole);
        if(var.isNull())
        {
            var = false;
        }

        const QPixmap &star = var.toBool() ? m_visiblePixmap : m_inVisiblePixmap;
        const QRect &rect = option.rect;
        int width = star.width();
        int height = star.height();
        int x = rect.x() + rect.width()/2 - width/2;
        int y = rect.y() + rect.height()/2 - height/2;

        painter->drawPixmap(x,y,star);
            return;
    }
        case PLotCfgTabModel::Sequence:
    {
        QVariant var = index.model()->data(index,Qt::EditRole);
        if(var.isNull())
        {
            var = false;
        }
        const QPixmap &star = var.toBool() ? m_SeqChkPixmap : m_unSeqChkPixmap;
        const QRect &rect = option.rect;
        int width = star.width();
        int height = star.height();
        int x = rect.x() + rect.width()/2 - width/2;
        int y = rect.y() + rect.height()/2 - height/2;

            painter->drawPixmap(QPointF(x,y),star);
            return;
    }
        case PLotCfgTabModel::Valid:
    {
            QPixmap decorateimage = qvariant_cast<QPixmap>(index.model()->data(index,Qt::DecorationRole));
            int decorwidth = decorateimage.width();
            const QRect &rect = option.rect;
            int x = rect.x() + decorwidth;
            int y = rect.y() + rect.height()/2 - decorateimage.height()/2;
            painter->drawPixmap(QPointF(x,y),decorateimage);
            QVariant var = index.model()->data(index,Qt::EditRole);
            if(var.isNull())
            {
                var = false;
            }
            const QPixmap &star = var.toBool() ? m_validPixmap[1] : m_validPixmap[0];
            x = rect.x() + rect.width()/2 - star.width()/2 +decorwidth;
            y = rect.y() + rect.height()/2 - star.height()/2;
            painter->drawPixmap(QPointF(x,y),star);
            return;
        }
        default:
            break;
    }
    QStyledItemDelegate::paint(painter, option, index);
}



bool PlotCfgDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    int column = index.column();
    switch (column)
    {
        case PLotCfgTabModel::Visible:
        case PLotCfgTabModel::Sequence:
        case PLotCfgTabModel::Valid:
    {
        QRect decorationRect = option.rect;
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (event->type() == QEvent::MouseButtonPress && decorationRect.contains(mouseEvent->pos()))
        {
            bool data = model->data(index, Qt::EditRole).toBool();
            model->setData(index, !data, Qt::EditRole);
            return true;
        }
    }
        default:
            break;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void PlotCfgDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}
