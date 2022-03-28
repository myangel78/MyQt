#ifndef CURRENTDISPITEM_H
#define CURRENTDISPITEM_H
#include "ui_currentdispitem.h"
#include "models/DataDefine.h"

#include <QWidget>

namespace Ui {
class CurrentDispItem;
}

class CurrentDispItem : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentDispItem(QWidget *parent = nullptr);
    ~CurrentDispItem();
    void init(int &chanel,const QColor &color);
    void setInstantValue(const double &Value);
    void setMeanValue(const double &Value);
    void setHoleEmbedEnable(bool &Checked);
    void JudgeHoleType(const double &single,const double &mult,const double &value);
    void resetHoleType(void);
    int getChannel(void){return m_channel;}
    void AutoSetZeroAction(void);
    enum PORE_STATE  getPoreState () const {return m_holeType;}
    bool GetSpecChannelIsShutoff()const {return m_bZeroSet;}



    bool isChecked();
    QColor color();
    void setChecked(bool stats);
    void setColor(QColor color);
    void GraphChangeColor();
    void GraphChangeVisuable(bool checked);

signals:
    void setZeroSig(CurrentDispItem *item);
    void GraphVisibleSig(int channel,bool isVisible);
    void GraphColorChangeSig(int channel,QColor color);

private slots:
    void on_holeEmbStateBtn_clicked();

private:
    void setChannel(int &channel){m_channel = channel;}
    void setCLickHoleColor(void);
    void setHoleColor(const enum PORE_STATE &type);
    void caliSleep(int msec);


    int m_channel;
    bool m_bZeroSet;
    enum PORE_STATE  m_holeType;
    Ui::CurrentDispItem *ui;
    QColor Color;
};

#endif // CURRENTDISPITEM_H
