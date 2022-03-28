#pragma once

#include <QWidget>


class CapUnitItem : public QWidget
{
    Q_OBJECT
public:
    explicit CapUnitItem(int chan, const QColor& clr, QWidget *parent = nullptr);

private:
    int m_iChannel;
    bool m_bShowSecond = false;
    QColor m_clrFirst = QColor(168, 168, 168);
    QColor m_clrSecond = QColor(128, 128, 128);// QColor(0x008000);

private:
    //bool InitCtrl(void);

public:
    virtual void paintEvent(QPaintEvent* event);

public:
    void SetChannel(int chan) { m_iChannel = chan; update(); }
    void SetFirstColor(const QColor& clr) { m_clrFirst = clr; update(); }
    void SetSecondColor(const QColor& clr) { m_clrSecond = clr; update(); }
    void SetShowSecond(bool val) { m_bShowSecond = val; update(); }


signals:

};

