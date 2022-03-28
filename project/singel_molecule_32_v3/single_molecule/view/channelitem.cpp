#include "channelitem.h"
#include "ui_channelitem.h"
#include <QColorDialog>
#include <QtWidgets/QAction>
#include <QtGui/QPainter>
#include <QDebug>

ChannelItem::ChannelItem(QString string,QColor color,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelItem)
{
    ui->setupUi(this);

    ui->colorBtn->setFixedSize(QSize(16, 16));
    ui->colorBtn->setAutoRaise(true);
    QAction *action = new QAction(this);
    action->setToolTip(tr("Change plot lines color"));
    ui->colorBtn->setDefaultAction(action);
    setColor(color);

    ui->chcheckBox->setText(string);


    connect(ui->colorBtn, &QToolButton::clicked, this, &ChannelItem::changeColor);
    connect(ui->chcheckBox, &QCheckBox::clicked, this, &ChannelItem::itemChange);
}

ChannelItem::~ChannelItem()
{
    delete ui;
}



void ChannelItem::itemChange(bool checked)
{
    emit changelChanged(this);
}

void ChannelItem::setColor(QColor color)
{
    Color = color;
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(QRect(0, 0, 15, 15));
    painter.fillRect(QRect(1, 1, 14, 14), color);

    ui->colorBtn->defaultAction()->setIcon(QIcon(pixmap));

}


void ChannelItem::changeColor()
{
    QColor color = QColorDialog::getColor(QColor(Color), this);
    if (color.isValid()) {
        setColor(color);
        emit changelChanged(this);
    }
}
