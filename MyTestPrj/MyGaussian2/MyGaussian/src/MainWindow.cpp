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

#define PI 3.1415926535898
#define DESIGN_MIU 3.0
#define DESIGN_SIGMA 0.5
#define DESIGN_AMPLITUDE 1.0
//#define DRAW_DESIGN
#define XASIX_POINTS 1000

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
#if 1
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
    SetupPlot(m_pCapHistogramPlot);
}

MainWindow::~MainWindow()
{
}

void MainWindow::InitCtrl(void)
{
    m_pCenterWdgt = new QWidget(this);
    m_pCapHistogramPlot = new QCustomPlot(this);

    QVBoxLayout *pvertlayout = new QVBoxLayout();
    pvertlayout->addWidget(m_pCapHistogramPlot);
    m_pCenterWdgt->setLayout(pvertlayout);
    setCentralWidget(m_pCenterWdgt);

}



void MainWindow::SetupPlot(QCustomPlot *customPlot)
{

    m_vctCapBarXaxis.clear();
    m_vctCapBarYaxis.clear();
    GenneralAsix(m_vctCapBarXaxis,m_vctCapBarYaxis);


    m_pCapBarSet = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    m_pCapBarSet->setWidth(m_binsize);
    m_pCapBarSet->setData(m_vctCapBarXaxis, m_vctCapBarYaxis);
    m_pCapBarSet->setPen(Qt::NoPen);
    m_pCapBarSet->setBrush(QColor(0,191,255));

    //redraw gauss curve according to miu and sigma
    m_vctCapGaussXaxis.clear();
    m_vctCapGaussYaxis.clear();
    PrepareDrawGaussian(m_vctCapGaussXaxis,m_vctCapGaussYaxis,m_miu,m_sigma);

    // create and configure plottables:
    m_pCapGaussGraph = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    m_pCapGaussGraph->setData(m_vctCapGaussXaxis, m_vctCapGaussYaxis);
//    m_pCapGaussGraph->SetLastDataRange(false);
    QPen redDotPen;
    redDotPen.setStyle(Qt::DotLine);
    redDotPen.setColor(QColor(255,0,0));
    redDotPen.setWidthF(2);
    m_pCapGaussGraph->setPen(redDotPen);

    //  customPlot->addLayer("abovemain", customPlot->layer("main"), QCustomPlot::limAbove);
    //  customPlot->addLayer("belowmain", customPlot->layer("main"), QCustomPlot::limBelow);
    //  m_pCapGaussGraph->setLayer("abovemain");
    //  customPlot->layer("abovemain")->setMode(QCPLayer::lmLogical);
    //  customPlot->xAxis->grid()->setLayer("belowmain");
    //  customPlot->yAxis->grid()->setLayer("belowmain");

    //图例设置
    m_pCapGaussGraph->setName(QString("Amplitude: %1\n Mean: %2\n Std: %3").arg(m_amplitude).arg(m_miu).arg(m_sigma));
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

    //  customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//模式：框选放大
    //定时器连接槽函数
    //  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    customPlot->rescaleAxes();
    customPlot->yAxis->setRange(0,1);
    //  customPlot->yAxis->setRange(0, 500);
    //  customPlot->xAxis->setRange(DESIGN_MIU-3 *DESIGN_SIGMA,DESIGN_MIU+ 3*DESIGN_SIGMA);
    customPlot->xAxis->setRange(m_xAsixMin,m_xAsixMax);
    customPlot->replot();
}


void MainWindow::Update(void)
{
     m_pCapHistogramPlot->replot();
}

void MainWindow::GenerateRawData(std::vector<double> &vctRawData)
{
    vctRawData.clear();
    std::random_device rd;
    std::mt19937 gen(rd());

    using normal_dist   = std::normal_distribution<>;
    using discrete_dist = std::discrete_distribution<std::size_t>;

    auto G = std::array<normal_dist, 3>{
        normal_dist{5.0, 0.1}, // mean, stddev of G[0]
        normal_dist{7.0, 0.4}, // mean, stddev of G[1]
        normal_dist{2.0, 0.3}  // mean, stddev of G[2]
    };
    auto w = discrete_dist{
            0.1, // weight of G[0]
            0.8, // weight of G[1]
            0.1  // weight of G[2]
    };

    for(int n = 0; n < 100; ++n) {
        // Create one sample of the Gaussian mixture model
        auto index = w(gen);
        auto sample = G[index](gen);
        vctRawData.emplace_back(sample);
        std::cout << sample << " ";
    }


}


void MainWindow::GetMinAndMax(std::vector<double> &vctRawData,double &reMin,double &reMax)
{
    int size = (int)vctRawData.size();

    double min = 0.0;
    double max = 0.0;

    // using default comparison (operator <):
    std::sort (vctRawData.begin(), vctRawData.end());

    min = vctRawData.front();
    max = vctRawData.back();

    qDebug()<< "min"<<min<< "max"<<max;
    reMin = min;
    reMax = max;
}

void MainWindow::GenneralAsix(QVector<double> &vctXaisx,QVector<double> &vctYaisx)
{
    double min = 0.0,max =0.0;
#if 1
    GenerateRawData(m_vctRawData);
    qDebug()<<"size"<<m_vctRawData.size()<<m_vctRawData;
#else
#if 0
    //bad data
    m_vctRawData = {7.2654, 2.03277, 6.76334, 7.00338, 6.14913, 1.9775, 2.39404, 6.12634, 7.35134, 6.7655, 1.36597, 5.2713, 7.21664, 7.37915, 6.6744, 1.74003, 7.72338, 7.60908, 6.72006, 5.14625, 5.00783, 1.73177, 1.76857, 6.69649, 1.77922, 2.2017, 1.83062, 1.95377, 7.00296, 7.31169, 5.9294, 7.24258, 2.24411, 2.2155, 7.66734, 2.82456, 1.67877, 7.76167, 6.93592, 7.20611, 2.38782, 1.88987, 7.35963, 6.91655, 6.20476, 5.10936, 1.71087, 6.93359, 5.0883, 7.46636, 7.15289, 1.83525, 4.94843, 1.96966, 7.62138, 6.34175, 2.38174, 6.74939, 2.11311, 6.83046, 2.0208, 7.42635, 1.94872, 1.38055, 7.13142, 5.18174, 2.38545, 1.74536, 6.41837, 7.131, 7.52165, 7.42459, 2.30149, 2.24742, 6.9972, 6.44297, 7.33294, 7.04717, 4.85079, 4.93295, 6.01496, 6.26122, 7.4935, 6.85164, 7.2565, 1.9032, 2.069, 1.93437, 4.99958, 6.26219, 5.16512, 6.97014, 7.05683, 7.19481, 2.06861, 6.70399, 6.58828, 6.44693, 2.32096, 4.97223};
#else
    //good data
    m_vctRawData = {6.33965, 2.55212, 6.51941, 7.31259, 6.951, 7.28915, 5.81703, 6.76177, 7.17638, 7.11856, 1.87, 7.16429, 6.75712, 7.41274, 2.74875, 1.78699, 7.49564, 2.04678, 7.36245, 6.91319, 7.68733, 4.95401, 6.70903, 6.49051, 6.4956, 1.74239, 6.67, 7.71036, 5.10516, 7.43492, 6.89023, 7.10158, 6.93651, 7.4033, 4.97209, 7.90861, 7.12402, 7.03959, 5.03158, 7.26962, 7.72467, 6.4842, 6.98065, 7.29316, 7.79847, 5.09871, 7.30079, 6.87452, 4.95666, 5.09111, 7.6573, 6.55885, 6.01661, 6.68607, 7.01758, 6.16136, 7.62088, 6.71225, 7.02648, 8.38502, 6.96683, 7.01398, 6.3056, 6.85018, 6.99697, 5.02085, 6.32971, 6.44303, 6.58673, 7.11636, 5.24504, 5.10223, 6.6765, 7.4667, 1.58002, 1.20294, 6.63096, 7.19103, 6.73679, 7.5736, 6.76727, 6.7336, 6.81139, 6.65099, 7.08824, 6.53932, 7.43189, 5.2183, 6.60102, 5.19081, 6.78822, 7.6721, 6.45881, 6.79589, 6.83989, 1.94275, 6.98623, 6.89738, 7.38303, 5.01202};
#endif
#endif

    GetMinAndMax(m_vctRawData,min,max);
    m_xAsixMin = min;
    m_xAsixMax = max;

    double miu = 0.0;
    double sigma = 1.0;
    CaculateMeanAndStd(m_vctRawData,miu,sigma);

#if 0
    const double first= (int)min * 1.0;
    const double end= (int)(max + 1) *1.0;
#else
    const double first= std::floor(min);
    const double end= std::ceil(max);
#endif
    qDebug()<<"first"<<first<<"max"<<end;
    const double span = m_binsize;
    const int nintervals=(end - first) / span +1;

    auto xs = linspace<std::vector<double>>(first,end, nintervals);
    auto ys = std::vector<double>(xs.size());

    int samplecnt = m_vctRawData.size();
    for (int i=0; i<samplecnt; ++i) {
      const auto &value = m_vctRawData[i];
      if ((value>=first)&&(value<end))
        ++ys[int((value-first)/span)];
    }

    for (int i=0; i<ys.size(); ++i) {
        ys[i] = ys[i]*1.0/samplecnt;
    }


    vctXaisx = QVector<double>::fromStdVector(xs);
    vctYaisx = QVector<double>::fromStdVector(ys);

    qDebug()<<"size"<<vctXaisx.size()<<"vctXaisx"<<vctXaisx;
    qDebug()<<"size"<<vctYaisx.size()<<"vctYaisx"<<vctYaisx;

    qDebug()<<"calculate miu"<<miu<<"calculate sigma"<<sigma;

    auto result = curve_fit(gaussian, {1.0,miu, sigma}, xs, ys);


    qDebug("result  amplitude :%lf ",result[0]);
    qDebug("result: Miu :%lf  Sigma: %lf",result[1],result[2]);
    qDebug("error : Miu error:%lf  Sigma error: %lf",result[1] - miu,result[2] - sigma);

#if 0
    m_amplitude = DESIGN_AMPLITUDE;
    m_miu = miu;
    m_sigma = sigma;
#else
    m_amplitude = result[0];
    m_miu = result[1];
    m_sigma = result[2];
#endif
}

void MainWindow::CaculateMeanAndStd(const std::vector<double> &vctData,double &reMiu, double &reStd)
{
    //计算均值
    double sum = std::accumulate(std::cbegin(vctData), std::cend(vctData), 0.0);
    double mean =  sum / vctData.size();

    //计算方差和标准差
    double accum  = 0.0;
    std::for_each (std::cbegin(vctData), std::cend(vctData), [&](const double d) {
        accum  += (d-mean)*(d-mean);
    });
    double stdev = sqrt(accum/(vctData.size()-1));

    reMiu = mean;
    reStd = stdev;
}



void MainWindow::PrepareDrawGaussian(QVector<double> &vctXaisx,QVector<double> &vctXaisY,const double &miu, const double &sigma)
{

    auto xs = linspace<std::vector<double>>(m_xAsixMin,m_xAsixMax, XASIX_POINTS);
    auto ys = std::vector<double>(xs.size());

    for(int i=0; i< xs.size(); i++)
    {
#if 0
//        ys[i] = gaussian(xs[i],m_amplitude,miu,sigma);
        ys[i] = gaussian(xs[i],DESIGN_AMPLITUDE,DESIGN_MIU,DESIGN_SIGMA);
#else
        ys[i] = gaussian(xs[i],m_amplitude,miu,sigma);
#endif
    }

    vctXaisx.resize((int)xs.size());
    std::copy(xs.cbegin(),xs.cend(),vctXaisx.begin());

    vctXaisY.resize(ys.size());
    std::copy(ys.cbegin(),ys.cend(),vctXaisY.begin());
}






