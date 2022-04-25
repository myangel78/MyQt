#include "MainWindow.h"
#include "qcustomplot.h"
#include <QVBoxLayout>
#include <iostream>
#include <string>
#include <random>
#include <cmath>

using namespace std;

#define MU 5.0
#define SIGMA 2.0
#define PI 3.1415926535898

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(600,400);
    InitCtrl();
    setupStyledDemo(m_pCustomPlot);
}

MainWindow::~MainWindow()
{
}

void MainWindow::InitCtrl(void)
{
    m_pCenterWdgt = new QWidget(this);
    m_pCustomPlot = new QCustomPlot(this);

    QVBoxLayout *pvertlayout = new QVBoxLayout();
    pvertlayout->addWidget(m_pCustomPlot);
    m_pCenterWdgt->setLayout(pvertlayout);

    setCentralWidget(m_pCenterWdgt);

}


void MainWindow::setupStyledDemo(QCustomPlot *customPlot)
{
  QString demoName = "Styled Demo";

  // prepare data:
  QVector<double> vctBarX(10,0), vctBarY(10,0);
  for (int i=0; i<vctBarX.size(); ++i)
  {
      vctBarX[i] = i;
  }
  //generate frequency distribution
  GenerateTestDate(vctBarX,vctBarY);

  CalculateMiuAndSigma(vctBarX,vctBarY,m_miu,m_sigma);
  qDebug()<<"miu"<<m_miu<<"sigma"<<m_sigma;

  //generate gaussian
  int distXmax = 1000;
  QVector<double> vctDistX(distXmax,0), vctDistY(distXmax,0);
  GenerateValueByNomalpdf(vctDistX,vctDistY);

  // create and configure plottables:
  QCPGraph *graph1 = customPlot->addGraph();
  graph1->setData(vctDistX, vctDistY);
//  graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
  graph1->setPen(QPen(QColor(120, 120, 120), 2));

  QCPBars *bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
  bars1->setWidth(1-0.1);
  bars1->setData(vctBarX, vctBarY);
  bars1->setPen(Qt::NoPen);
  bars1->setBrush(QColor(10, 140, 70, 160));

  // move bars above graphs and grid below bars:
  customPlot->addLayer("abovemain", customPlot->layer("main"), QCustomPlot::limAbove);
  customPlot->addLayer("belowmain", customPlot->layer("main"), QCustomPlot::limBelow);
  graph1->setLayer("abovemain");
  customPlot->xAxis->grid()->setLayer("belowmain");
  customPlot->yAxis->grid()->setLayer("belowmain");

  // set some pens, brushes and backgrounds:
  customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
  customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
  customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
  customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
  customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
  customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
  customPlot->xAxis->setTickLabelColor(Qt::white);
  customPlot->yAxis->setTickLabelColor(Qt::white);
  customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
  customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
  customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
  customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
  customPlot->xAxis->grid()->setSubGridVisible(true);
  customPlot->yAxis->grid()->setSubGridVisible(true);
  customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
  customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
  customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  QLinearGradient plotGradient;
  plotGradient.setStart(0, 0);
  plotGradient.setFinalStop(0, 350);
  plotGradient.setColorAt(0, QColor(80, 80, 80));
  plotGradient.setColorAt(1, QColor(50, 50, 50));
  customPlot->setBackground(plotGradient);
  QLinearGradient axisRectGradient;
  axisRectGradient.setStart(0, 0);
  axisRectGradient.setFinalStop(0, 350);
  axisRectGradient.setColorAt(0, QColor(80, 80, 80));
  axisRectGradient.setColorAt(1, QColor(30, 30, 30));
  customPlot->axisRect()->setBackground(axisRectGradient);

  customPlot->rescaleAxes();
  customPlot->yAxis->setRange(0, 1);
//  customPlot->yAxis->setRange(0, 500);
//  customPlot->xAxis->setRange(0,20);
}



void MainWindow::GenerateValueByNomalpdf(QVector<double> &vctXaisx,QVector<double> &vctXaisY)
{
    int maxX = 10;
    double unit = 10*1.0/vctXaisx.size();

    for(int i=0; i< vctXaisx.size(); i++)
    {
        vctXaisx[i] = unit * i;
//        vctXaisY[i] = Normal_pdf(vctXaisx[i],MU,SIGMA);
        vctXaisY[i] = Normal_pdf(vctXaisx[i],m_miu,m_sigma);
    }

}

void MainWindow::GenerateTestDate(QVector<double> &vctXasix,QVector<double> &vctYasix)
{
    const int nrolls=1000;  // number of experiments
    const int nstars=1	;    // maximum number of stars to distribute

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(MU,SIGMA);

    double max = vctYasix.size() *1.0;

    for (int i=0; i<nrolls; ++i) {
      double number = distribution(generator);
      if ((number>=0.0)&&(number<max)) ++vctYasix[int(number)];
    }

    double sum = 0.0;
    for (int i=0; i<vctXasix.size(); ++i)
    {
//        std::cout << i << "-" << (i+1) << ": ";
//        std::cout << std::string(vctYasix[i]*nstars * 100/nrolls,'*') << std::endl;
        vctYasix[i] = vctYasix[i]*1.0*nstars/nrolls;
    }
}


bool MainWindow::CalculateMiuAndSigma(const QVector<double> &vctGrpUnit,const QVector<double> &vctFreqDist,double &reMiu, double &reSigma)
{
    if(vctGrpUnit.size() != vctFreqDist.size())
        return false;

    //1.求期望
    int sizes = vctGrpUnit.size();
    double miu  = 0.0;
    for(int i =0; i < sizes; ++i)
    {
        miu += vctGrpUnit[i] * vctFreqDist[i];
    }

    //2.求方差
    double sigma  = 0.0;
    double sumdiff =0.0;
    int freqDistSize = vctFreqDist.size();
    for(int i = 0; i < freqDistSize; ++i)
    {
        sumdiff += std::pow((vctGrpUnit[i] - miu),2) * vctFreqDist[i];
    }
//    sumdiff = sumdiff/(freqDistSize -1);
    sigma = sqrt(sumdiff);


    reMiu = miu;
    reSigma = sigma;
    return true;
}

double MainWindow::Normal_pdf(const float &x, const float & miu, const float & sigma)
{
#if 1
    static const double inv_sqrt_2pi = 0.3989422804014327;
    double a = (x - miu) / sigma;
    return inv_sqrt_2pi / sigma * std::exp(-0.5f * a * a);
#else
    static const float sqrt_two_pi = sqrt(PI * 2);
    return std::exp(- pow((x - miu), 2) / 2 / pow(sigma,2)) / (sqrt_two_pi * sigma);
#endif
}

