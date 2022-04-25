#pragma once

#include <QMainWindow>
class QCustomPlot;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void InitCtrl(void);
    void setupStyledDemo(QCustomPlot *customPlot);

    void GenerateValueByNomalpdf(QVector<double> &vctXaisx,QVector<double> &vctXaisY);
    void GenerateTestDate(QVector<double> &vctXasix,QVector<double> &vctYasix);

    bool CalculateMiuAndSigma(const QVector<double> &vctGrpUnit,const QVector<double> &vctFreqDist,double &reMiu, double &reSigma);
    double Normal_pdf(const float &x, const float & miu, const float & sigma);


private:
     QWidget *m_pCenterWdgt  = nullptr;
     QCustomPlot *m_pCustomPlot = nullptr;

     double m_miu = 0.0;
     double m_sigma = 0.0;
};
