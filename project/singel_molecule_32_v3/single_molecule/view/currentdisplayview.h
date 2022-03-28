#ifndef CURRENTDISPLAYVIEW_H
#define CURRENTDISPLAYVIEW_H

#include <QWidget>
#include "view/currentdispitem.h"
#include <QTimer>
#include <QSettings>

namespace Ui {
class CurrentDisplayView;
}

class CurrentDisplayView : public QWidget
{
    Q_OBJECT

public:

    explicit CurrentDisplayView(QWidget *parent = nullptr);
    ~CurrentDisplayView();
    void initView(void);
    void LoadConfig(QSettings *config);
    void SaveConfig(QSettings *config);

signals:
    void setChannelZeroSig(int &channel,float &volt);
    void GraphVisibleSig(int channel,bool isVisible);
    void GraphColorChangeSig(int channel,QColor color);
    void GraphAllVisibleSig(bool isVisible);  //统一设置，避免32个信号槽
    void ListItemVisableSig(QVector<bool> vctItemVisiable);

public slots:
    void dispMeansSlot(QVector<float> dispValue);
    void dispInstantSlot(QVector<float> dispValue);
    void setPoreInsertionWorkSlot(bool bIsOn,const float &singleLimt,const float &singleVolt);
    void getPoreStateBackSlot(const int &channel,bool &bIsSet);


private slots:
    void on_nMeansLimitBtn_clicked(bool checked);
    void on_nAutoEnableBtn_toggled(bool checked);
    void setZeroItemSlot(CurrentDispItem *item);
    void HoleEmbedDutyTimeOut(void);

    void on_allCheck_toggled(bool checked);
    void on_onlyShowCheckChk_toggled(bool checked);

private:
    void ResetAllHoleColor(void);
    bool m_bAutoHole;
    QTimer *m_dutyTimer;

    Ui::CurrentDisplayView *ui;
    std::vector<std::shared_ptr<CurrentDispItem>> m_vctCurrentDisp;

};

#endif // CURRENTDISPLAYVIEW_H
