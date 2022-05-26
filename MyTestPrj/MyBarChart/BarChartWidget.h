#pragma once

#include <QWidget>
#include <QtCharts>

class QPushButton;

class BarChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BarChartWidget(QWidget *parent = nullptr);

private:
    void InitChartCtrl(void);

private:
    QBarSet *m_pBarset0 = nullptr;
    QBarSet *m_pBarset1 = nullptr;
    QBarSet *m_pBarset2 = nullptr;
    QBarSet *m_pBarset3 = nullptr;
    QBarSet *m_pBarset4 = nullptr;

    QStackedBarSeries *m_pStackedBarSeries = nullptr;

    QChart *m_pChart = nullptr;


    QBarCategoryAxis *m_pBarCategoryAxis = nullptr;

    QValueAxis *m_pValueAxisY = nullptr;

    QChartView *m_pChartView = nullptr;

    QPushButton *m_pTestBtn = nullptr;


    QLineSeries *m_plineseries = nullptr;


    QDateTimeAxis *m_pDateTimeAxisX = nullptr;

    qint64 m_minTime;
    qint64 m_maxTime;



signals:
private slots:
    void AddBtnSlot(void);

};

