#ifndef CAPLISTITEM_H
#define CAPLISTITEM_H

#include <QWidget>

namespace Ui {
class CapListItem;
}

class CapListItem : public QWidget
{
    Q_OBJECT

public:
    explicit CapListItem(QWidget *parent = nullptr,int channel = 0);
    ~CapListItem();

    void setNum(float &value);

private:
    Ui::CapListItem *ui;
};

#endif // CAPLISTITEM_H
