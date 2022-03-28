#pragma once

#include <QWidget>
#include <qlabel.h>


class CapListItem : public QWidget
{
    Q_OBJECT
public:
    explicit CapListItem(int chan, QWidget *parent = nullptr);


private:
    QLabel* m_plabChannel;
    QLabel* m_plabCapValue;
    //QLabel* m_plabCapUnit;

    int m_iChannel = 0;

private:
    bool InitCtrl(void);

public:
    void SetCapValue(float val)
    {
        m_plabCapValue->setText(QString::number(val, 'f', 2));
    }

signals:

};

