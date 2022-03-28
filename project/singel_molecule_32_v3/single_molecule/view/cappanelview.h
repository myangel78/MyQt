#ifndef CAPPANELVIEW_H
#define CAPPANELVIEW_H

#include <QWidget>
#include "models/DataBase.h"
#include "view/capitem.h"
#include "view/caplistitem.h"


namespace Ui {
class CapPanelView;
}

class CapPanelView : public QWidget,public DataBase
{
    Q_OBJECT

public:
    explicit CapPanelView(QWidget *parent = nullptr);
    ~CapPanelView();
    void appendData(int ch,const QVector<float> &vctValue);
    void appendAllChData(QVector<QVector<float>> &vctValue);
    void updateView(int startCh);
    void clearData(void);
    bool OverMaxCount(int channel);
signals:
    void reDrawSig(void);
    void setTrangularWave(const bool &bIsSet);

protected:
    virtual void threadFunc();

private slots:
    void onChanleViewChange(int page);
    void reDrawSlot(void);
    void on_startBtn_clicked(bool checked);
    void on_SaveCapToFileBtn_clicked();
    void onCapFilterApplyBtnclicked();

private:
    Ui::CapPanelView *ui;
    void initWidget(void);
    void listViewInit(void);
    void updateCapValDisp(void);
    void onCapListViewChange(const int &startIndex);

    std::vector<std::shared_ptr<CapItem>> m_vctCapItem;
    std::vector<std::shared_ptr<CapListItem>> m_vctCapListItem;

    std::mutex m_lock;
    QVector<QVector<QPointF>> m_vctData;
    QVector<int> m_vctCount;
    QVector<float> m_vctCapValue;
    QVector<int> m_vctYmax;
    int m_maxsize;

};

#endif // CAPPANELVIEW_H
