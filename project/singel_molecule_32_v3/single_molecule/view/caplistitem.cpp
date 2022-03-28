#include "caplistitem.h"
#include "ui_caplistitem.h"

CapListItem::CapListItem(QWidget *parent,int channel) :
    QWidget(parent),
    ui(new Ui::CapListItem)
{
    ui->setupUi(this);
    ui->capValLab->setText(tr("CH") + QString::number(channel));
}

CapListItem::~CapListItem()
{
    delete ui;
}

void CapListItem::setNum(float &value){
    ui->capDispLab->setText(QString::number(value,'f',2));
}
