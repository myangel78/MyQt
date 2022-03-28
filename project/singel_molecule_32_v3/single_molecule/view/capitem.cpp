#include "capitem.h"
#include "ui_capitem.h"
#include "QTimer"
#include "QTime"
#include "QList"
#include "qmath.h"
#include "QPointF"
#include "QDebug"

QT_CHARTS_USE_NAMESPACE

CapItem::CapItem(QWidget *parent,int channel) :
    QWidget(parent),  m_channel(channel),
    ui(new Ui::CapItem)
{
    ui->setupUi(this);
    m_capValue = 0;
    m_Ymax = 10;
    initChart();

}

CapItem::~CapItem()
{
    delete axisX;
    delete axisY;
    delete ui;
}

void CapItem::initChart(void)
{
    m_chart = new QChart();
    QChartView *chartView = new QChartView(m_chart);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    m_series = new QLineSeries;
    m_chart->addSeries(m_series);

//    m_series->setUseOpenGL(true);//openGl 加速
//    qDebug()<<m_series->useOpenGL();

    axisX = new QValueAxis;
    axisX->setRange(0,300);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Counts");


    axisY = new QValueAxis;
    axisY->setRange(0,m_Ymax);
    axisY->setTitleText("pf");

//    m_chart->setAxisX(axisX,m_series);
//    m_chart->setAxisY(axisY,m_series);
    m_chart->addAxis(axisX,Qt::AlignBottom);
    m_chart->addAxis(axisY,Qt::AlignLeft);
    m_series->attachAxis(axisX);
    m_series->attachAxis(axisY);
    m_chart->legend()->hide();
    m_chart->setTitle("Channel " + QString::number(m_channel+1));

    QVBoxLayout *layout = ui->verticalLayout;
    layout->addWidget(chartView);


}

void CapItem::replace(int &channel,QVector<QPointF> &data)
{
     m_chart->setTitle("Channel " + QString::number(channel+1));
     m_series->replace(data);
}

void CapItem::setYAsix(int max)
{
  if(max > m_Ymax)
  {
     axisY->setRange(0,max+5);
     m_Ymax = max;
  }
}

