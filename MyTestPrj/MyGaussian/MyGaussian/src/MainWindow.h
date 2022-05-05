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

    void PrepareData(QVector<double> &vctXaisx,QVector<double> &vctXaisY,double &reMiu,double &reSigma);
    void PrepareDrawPlot(QVector<double> &vctXaisx,QVector<double> &vctXaisY,const double &miu, const double &sigma);


private:
     QWidget *m_pCenterWdgt  = nullptr;
     QCustomPlot *m_pCustomPlot = nullptr;
     QVector<double> vctRawData;

     QVector<double> m_vctBarX, m_vctBarY;
     QVector<double> m_vctGaussX, m_vctGaussY;

     double m_miu = 0.0;
     double m_sigma = 0.0;
     double m_amplitude = 1.0;
};
