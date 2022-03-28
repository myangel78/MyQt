#ifndef CHANNELITEM_H
#define CHANNELITEM_H
#include "ui_channelitem.h"

#include <QWidget>

namespace Ui {
class ChannelItem;
}

class ChannelItem : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelItem(QString string,
                         QColor color = QColor(255, 255, 255),QWidget *parent = nullptr);
    ~ChannelItem();


    Qt::CheckState checkState() {
        return ui->chcheckBox->checkState();
    }

    bool isChecked() {
        return ui->chcheckBox->isChecked();
    }

    QColor color() {
        return Color;
    }

    int channel() {
        return Channel;
    }

    void setChecked(bool stats) {
        ui->chcheckBox->setChecked(stats);
    }

    void setColor(QColor color);

    void setChannel(int ch) {
        Channel = ch;
    }

private slots:
    void itemChange(bool checked);
    void changeColor();

signals:
    void changelChanged(ChannelItem *item);
private:
    Ui::ChannelItem *ui;
    QColor Color;
    int Channel;
};

#endif // CHANNELITEM_H
