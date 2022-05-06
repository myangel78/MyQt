#include "MainWindow.h"
#include "qcustomplot.h"
#include <QVBoxLayout>
#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <iomanip>
#include <array>
#include "curve_fit.hpp"

using namespace std;
#define PI 3.1415926535898
#define DESIGN_MIU 3.0
#define DESIGN_SIGMA 0.5
#define DESIGN_AMPLITUDE 1
//#define DRAW_DESIGN
#define XASIX_POINTS 100


template <typename Container>
auto linspace(typename Container::value_type a, typename Container::value_type b, size_t n)
{
    assert(b > a);
    assert(n > 1);

    Container res(n);
    const auto step = (b - a) / (n - 1);
    auto val = a;
    for(auto& e: res)
    {
        e = val;
        val += step;
    }
    return res;
}

double gaussian(double x, double a, double b, double c)
{
    #if 0
    const double z = (x - b) / c;
    return a * std::exp(-0.5 * z * z);
#else
    return a * (1/(sqrt(2*PI)*c)) * std::exp(-pow(x - b, 2) / (2 * pow(c, 2)));
#endif
}



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


void MainWindow::PrepareData(QVector<double> &vctXaisx,QVector<double> &vctXaisY,double &reMiu,double &reSigma)
{
#if 1
    std::random_device device;
    auto gen= std::mt19937(device());

    auto xs = linspace<std::vector<double>>(DESIGN_MIU -3* DESIGN_SIGMA,DESIGN_MIU+ 3*DESIGN_SIGMA, XASIX_POINTS);
    auto ys = std::vector<double>(xs.size());

    for(size_t i = 0; i < xs.size(); i++)
    {
#if 1
        auto y =  gaussian(xs[i],DESIGN_AMPLITUDE,DESIGN_MIU, DESIGN_SIGMA);
        auto dist  = std::normal_distribution(0.0, 0.1 *y);
        ys[i] = y + dist(gen);
#endif
    }

#else
    const int nrolls=10000;  // number of experiments
    const int nstars=100;    // maximum number of stars to distribute

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(DESIGN_MIU,DESIGN_SIGMA);

    auto xs = linspace<std::vector<double>>(DESIGN_MIU-3 *DESIGN_SIGMA,DESIGN_MIU+ 3*DESIGN_SIGMA, XASIX_POINTS);
    auto ys = std::vector<double>(xs.size());
    double unit = 6 * DESIGN_SIGMA*1.0/XASIX_POINTS;

    for (int i=0; i<nrolls; ++i) {
        double number = distribution(generator);
        int index = number/unit;
        if(0 <= index && index < XASIX_POINTS)
        {
             ++ys[index];
        }
    }

    for (int i=0; i<ys.size(); ++i) {
        ys[i] = ys[i]*nstars/nrolls;
    }

#endif

    vctXaisx.resize((int)xs.size());
    std::copy(xs.cbegin(),xs.cend(),vctXaisx.begin());

    vctXaisY.resize(ys.size());
    std::copy(ys.cbegin(),ys.cend(),vctXaisY.begin());

    auto result = curve_fit(gaussian, {DESIGN_AMPLITUDE,DESIGN_MIU, DESIGN_SIGMA}, xs, ys);

#if 1
    qDebug("result  amplitude :%lf ",result[0]);
    qDebug("result: Miu :%lf  Sigma: %lf",result[1],result[2]);
    qDebug("error : Miu error:%lf  Sigma error: %lf",result[1] - DESIGN_MIU,result[2] - DESIGN_SIGMA);
#else
    std::cout << "result: " << "Miu"<< result[1] << " Sigma" << result[2] <<  '\n';
    std::cout << "error : " << result[1] - DESIGN_MIU << ' ' << result[2] - DESIGN_SIGMA << '\n';
#endif

#ifdef DRAW_DESIGN
    m_amplitude = DESIGN_AMPLITUDE;
    reMiu = DESIGN_MIU;
    reSigma = DESIGN_SIGMA;
#else
    m_amplitude = result[0];
    reMiu = result[1];
    reSigma = result[2];
#endif
}

void MainWindow::PrepareDrawPlot(QVector<double> &vctXaisx,QVector<double> &vctXaisY,const double &miu, const double &sigma)
{
    auto xs = linspace<std::vector<double>>(miu -3* sigma,miu+ 3*sigma, XASIX_POINTS);
    auto ys = std::vector<double>(xs.size());

    for(int i=0; i< xs.size(); i++)
    {
#if 1
        ys[i] = gaussian(xs[i],m_amplitude,miu,sigma);
#else
        ys[i] = gaussian(xs[i],DESIGN_AMPLITUDE,miu,sigma);
#endif
    }

    vctXaisx.resize((int)xs.size());
    std::copy(xs.cbegin(),xs.cend(),vctXaisx.begin());

    vctXaisY.resize(ys.size());
    std::copy(ys.cbegin(),ys.cend(),vctXaisY.begin());
}




void MainWindow::setupStyledDemo(QCustomPlot *customPlot)
{
  QString demoName = "Styled Demo";

  // prepare data:
  PrepareData(m_vctBarX,m_vctBarY,m_miu,m_sigma);

  //redraw gauss curve according to miu and sigma
  PrepareDrawPlot(m_vctGaussX,m_vctGaussY,m_miu,m_sigma);


  // create and configure plottables:
  QCPGraph *graph1 = customPlot->addGraph();
  graph1->setData(m_vctGaussX, m_vctGaussY);
  QPen redDotPen;
  redDotPen.setStyle(Qt::DotLine);
  redDotPen.setColor(QColor(255,0,0));
  redDotPen.setWidthF(2);
//  graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
  graph1->setPen(redDotPen);

  QCPBars *bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
  bars1->setWidth(1.0/XASIX_POINTS);
  bars1->setData(m_vctBarX, m_vctBarY);
  bars1->setPen(Qt::NoPen);
  bars1->setBrush(QColor(0,191,255));

  // move bars above graphs and grid below bars:
  customPlot->addLayer("abovemain", customPlot->layer("main"), QCustomPlot::limAbove);
  customPlot->addLayer("belowmain", customPlot->layer("main"), QCustomPlot::limBelow);
  graph1->setLayer("abovemain");
  customPlot->xAxis->grid()->setLayer("belowmain");
  customPlot->yAxis->grid()->setLayer("belowmain");

  //图例设置
  graph1->setName(QString("Amplitude: %1\n Mean: %2\n Std: %3").arg(m_amplitude).arg(m_miu).arg(m_sigma));
  customPlot->legend->setIconSize(20,20);
  customPlot->legend->setTextColor(Qt::blue);//设置图例文字颜色
  customPlot->legend->setVisible(true); //设置图例是否可用
  customPlot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignRight);
  customPlot->legend->setBrush(QColor(255,255,255,0));//设置图例背景
  customPlot->legend->setBrush(QColor(255, 255, 255, 150));
  customPlot->legend->setFont(QFont("Helvetica", 9,3));

  // add title layout element:
  customPlot->plotLayout()->insertRow(0);
  customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "Gaussian plot", QFont("sans", 12, QFont::Bold)));
  // set some pens, brushes and backgrounds:
  customPlot->xAxis->setLabel("Capacitance Value/pF");
  customPlot->yAxis->setLabel("Probability");
  customPlot->xAxis->setBasePen(QPen(Qt::black, 1));
  customPlot->yAxis->setBasePen(QPen(Qt::black, 1));
  customPlot->xAxis->setTickPen(QPen(Qt::black, 1));
  customPlot->yAxis->setTickPen(QPen(Qt::black, 1));
  customPlot->xAxis->setSubTickPen(QPen(Qt::black, 1));
  customPlot->yAxis->setSubTickPen(QPen(Qt::black, 1));
  customPlot->xAxis->setTickLabelColor(Qt::black);
  customPlot->yAxis->setTickLabelColor(Qt::black);
  customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
  customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//  customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//  customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
  customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  customPlot->axisRect()->setBackground(QBrush(Qt::white));

  customPlot->rescaleAxes();
  customPlot->yAxis->setRange(0,1);
//  customPlot->yAxis->setRange(0, 500);
//  customPlot->xAxis->setRange(DESIGN_MIU-3 *DESIGN_SIGMA,DESIGN_MIU+ 3*DESIGN_SIGMA);
  customPlot->xAxis->setRange(m_miu-3 *m_sigma,m_miu+ 3*m_sigma);

}








