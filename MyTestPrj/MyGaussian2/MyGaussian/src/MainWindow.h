#pragma once

#include <QMainWindow>
class QCustomPlot;
class QCPGraph;
class QCPBars;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void Update(void);

private:
    void InitCtrl(void);
    void SetupPlot(QCustomPlot *customPlot);

    void GenerateRawData(std::vector<double> &vctRawData);
    void GetMinAndMax(std::vector<double> &vctRawData,double &reMin,double &reMax);
    void GenneralAsix(QVector<double> &vctXaisx,QVector<double> &vctYaisx);
    void CaculateMeanAndStd(const std::vector<double> &vctData,double &reMiu, double &reStd);

    void PrepareDrawGaussian(QVector<double> &vctXaisx,QVector<double> &vctXaisY,const double &miu, const double &sigma);


private:
     QWidget *m_pCenterWdgt  = nullptr;
     QCustomPlot *m_pCapHistogramPlot = nullptr;
     QVector<double> m_vctCapBarXaxis, m_vctCapBarYaxis;
     QVector<double> m_vctCapGaussXaxis, m_vctCapGaussYaxis;

     std::vector<double> m_vctRawData;

     QCPGraph *m_pCapGaussGraph = nullptr;
     QCPBars *m_pCapBarSet = nullptr;

     double m_xAsixMax = 10;
     double m_xAsixMin = 0.0;
     double m_yAsixMax = 1.0;
     double m_yAsixMin = 0.0;

     double m_binsize = 0.5;
     double m_miu = 0.0;
     double m_sigma = 0.0;
     double m_amplitude = 1.0;
};
