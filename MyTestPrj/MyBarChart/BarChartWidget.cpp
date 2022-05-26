#include "BarChartWidget.h"
#include <QVBoxLayout>
#include <QDebug>

BarChartWidget::BarChartWidget(QWidget *parent) : QWidget(parent)
{
    InitChartCtrl();
}


void BarChartWidget::InitChartCtrl(void)
{
    m_pChart = new QChart();

    m_pBarset0 = new QBarSet("Jane");
    m_pBarset1 = new QBarSet("John");
    m_pBarset2 = new QBarSet("Axel");
    m_pBarset3 = new QBarSet("Mary");
    m_pBarset4 = new QBarSet("Samantha");


    m_pStackedBarSeries = new QStackedBarSeries(m_pChart);
    m_plineseries = new QLineSeries(m_pChart);
    m_plineseries->setName("trend");

    m_pStackedBarSeries->append(m_pBarset0);
    m_pStackedBarSeries->append(m_pBarset1);
    m_pStackedBarSeries->append(m_pBarset2);
    m_pStackedBarSeries->append(m_pBarset3);
    m_pStackedBarSeries->append(m_pBarset4);

//    *m_pBarset0 << 1 ;
//    *m_pBarset1 << 3 ;
//    *m_pBarset2 << 4 ;
//    *m_pBarset3 << 2 ;
//    *m_pBarset4 << 4 ;


    m_pChart->addSeries(m_pStackedBarSeries);
    m_pChart->addSeries(m_plineseries);

    m_pChart->setTitle("Simple stackedbarchart example");
    m_pChart->setAnimationOptions(QChart::NoAnimation);

    m_pDateTimeAxisX = new QDateTimeAxis(m_pChart);
    m_pDateTimeAxisX->setFormat("dd-hh:mm:ss");
    m_pDateTimeAxisX->setGridLineVisible(false);
    m_pChart->addAxis(m_pDateTimeAxisX, Qt::AlignBottom);
    m_plineseries->attachAxis(m_pDateTimeAxisX);
//    m_pStackedBarSeries->attachAxis(m_pDateTimeAxisX);

    m_pBarCategoryAxis = new QBarCategoryAxis();
    m_pChart->addAxis(m_pBarCategoryAxis, Qt::AlignBottom);
    m_pStackedBarSeries->attachAxis(m_pBarCategoryAxis);
    //    m_plineseries->attachAxis(m_pValueAxisY);
    m_pBarCategoryAxis->setVisible(false);

    m_pValueAxisY = new QValueAxis(m_pChart);
    m_pChart->addAxis(m_pValueAxisY, Qt::AlignLeft);
    m_pStackedBarSeries->attachAxis(m_pValueAxisY);

    //    m_pDateTimeAxisX->setTickCount(8);
    m_pStackedBarSeries->setBarWidth(1);
    m_pChart->axes(Qt::Vertical,m_pStackedBarSeries).first()->setRange(0,20);
    m_plineseries->setVisible(false);



    m_pChart->legend()->setVisible(true);
    m_pChart->legend()->setAlignment(Qt::AlignBottom);

    m_pChartView = new QChartView(m_pChart);
    m_pChartView->setRenderHint(QPainter::Antialiasing);

    m_pTestBtn = new QPushButton(QStringLiteral("Add"),this);

    QVBoxLayout *pvertlayout =new QVBoxLayout();
    pvertlayout->addWidget(m_pChartView);
    pvertlayout->addWidget(m_pTestBtn);
    setLayout(pvertlayout);

    connect(m_pTestBtn,&QPushButton::clicked,this,&BarChartWidget::AddBtnSlot);

    m_minTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
}


void BarChartWidget::AddBtnSlot(void)
{
    qDebug()<<__FUNCTION__;

    m_pBarCategoryAxis->append(QString::number(m_plineseries->count(),10));

    *m_pBarset0 <<  qrand()%(10);
    *m_pBarset1 <<  qrand()%(10);
    *m_pBarset2 <<  qrand()%(10);
    *m_pBarset3 <<  qrand()%(10);
    *m_pBarset4 <<  qrand()%(10);

    m_maxTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    *m_plineseries << QPointF(m_maxTime,  qrand()%(10));

    m_pDateTimeAxisX->setTickCount( m_plineseries->count() <= 10 ?  m_plineseries->count()+1 : 10);
    m_pDateTimeAxisX->setRange(QDateTime::fromMSecsSinceEpoch(m_minTime),QDateTime::fromMSecsSinceEpoch(m_maxTime));
}
