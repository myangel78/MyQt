#pragma once

#include <QDialog>
//#include <QtCharts>
#include <qlabel.h>
#include <qpushbutton.h>

#include "qcustomplot.h"


class PlotReviewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotReviewDialog(QWidget *parent = nullptr);
    //virtual ~PlotReviewDialog();

private:
    //QChart* m_pchaPlot;
    //QLineSeries* m_plsrPlot;
    QCustomPlot* m_pReplot;
    QLabel* m_plabDataName;
    QPushButton* m_pbtnLoadData;
    QPushButton* m_pbtnResetData;

    //const char* m_pLayer = "replotlayer";
    double m_dYmax = -999.99;
    double m_dYmin = 999.99;
    double m_dXnum = 0.0;

    QPointer<QCPGraph> m_graph;
private:
    bool InitCtrl(void);

    void OnClickLoadData(void);
    void OnClickResetData(void);

signals:

};

