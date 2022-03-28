#include "plotreviewdialog.h"
#include "ui_plotreviewdialog.h"
#include "QFileDialog"
#include "models/DataDefine.h"
#include <QDebug>
#include <QDataStream>

QT_CHARTS_USE_NAMESPACE


PlotReViewDialog::PlotReViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotReViewDialog)
{
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);
    ui->setupUi(this);

    setupPlot();
    this->setWindowTitle("PlotReViewDialog");

    m_Count = 0;
    m_maxY  = -10000;
    m_minY  = 10000;
}

PlotReViewDialog::~PlotReViewDialog()
{
    delete ui;
}

void PlotReViewDialog::setupPlot(void)
{
    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    m_series = new QLineSeries;
    m_chart->addSeries(m_series);

    m_series->setUseOpenGL(true);//openGl 加速
    qDebug()<<m_series->useOpenGL();


    axisX->setRange(0,5000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Counts");

    axisY->setRange(-1.5,1.5);
    axisY->setTitleText("Current/pA");

    m_chart->setAxisX(axisX,m_series);
    m_chart->setAxisY(axisY,m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("Plot Review");

    QVBoxLayout *layout = ui->verticalLayout;
    layout->addWidget(chartView);
}

void PlotReViewDialog::on_LoadPlotBtn_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,
            tr("Open  plot data File"),
            ".",
            "binary dat files(*.dat)",
            0);
    if (!file_name.isNull())
    {
        plotDataSelect(file_name);
    }
    else{
        qDebug()<<"cancle. PlotReViewDialog....";
    }
    resetChart();
}

void PlotReViewDialog::plotDataSelect(const QString &file)
{
    qDebug()<<file;

    QFileInfo fileInfo (file);
    QString fileBaseName = fileInfo.baseName();
    m_chart->setTitle(fileBaseName);

    float reciveBuf[1024];
    FILE* fileread = fopen(file.toStdString().c_str(), "rb");
    float temp = 0;

    while(1)
    {
        size_t  readCount = fread(&reciveBuf[0], sizeof(float), sizeof(reciveBuf)/sizeof(float), fileread);
        if(readCount >0)
        {
            for(int i = 0; i < readCount; i++)
            {
                temp = reciveBuf[i];
                compareMaxValue(temp,m_maxY,m_minY);
                m_vctData.push_back(QPointF(m_Count++,temp));
            }
        }
        else
        {
            qDebug()<<"file has read out";
            break;
        }
    }

    m_series->replace(m_vctData);
    axisX->setRange(0,m_Count);
    axisY->setRange(m_minY,m_maxY);
}

void PlotReViewDialog::compareMaxValue(float &value,float &maxValue,float &minValue)
{
    maxValue = (value > maxValue)? value : maxValue;
    minValue = (value < minValue)? value : minValue;
}

void PlotReViewDialog::resetChart(void)
{
    m_maxY =-10000;
    m_minY =10000;
    m_Count  =0;
    m_vctData.clear();
}

