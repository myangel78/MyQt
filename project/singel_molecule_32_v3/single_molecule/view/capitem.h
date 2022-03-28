#ifndef CAPITEM_H
#define CAPITEM_H
#include <QWidget>
#include <QtCharts>

namespace Ui {
class CapItem;
}


class CapItem : public QWidget
{
    Q_OBJECT

public:
    explicit CapItem(QWidget *parent = nullptr,int channel = 0);
    ~CapItem();
public:
    void replace(int &channel,QVector<QPointF> &data);
    double getCapValue(void){return m_capValue;}
    void setYAsix(int max);

private:
    void initChart(void);

    QChart *m_chart;
    QLineSeries *m_series;
    Ui::CapItem *ui;
    int m_timeId;
    int m_channel;
    int m_Ymax;
    double m_capValue;
    QValueAxis *axisX;
    QValueAxis *axisY;

};


#endif // CAPITEM_H
