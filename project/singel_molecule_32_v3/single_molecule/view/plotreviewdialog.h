#ifndef PLOTREVIEWDIALOG_H
#define PLOTREVIEWDIALOG_H

#include <QDialog>
#include <QtCharts>

namespace Ui {
class PlotReViewDialog;
}

class PlotReViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotReViewDialog(QWidget *parent = nullptr);
    ~PlotReViewDialog();

private slots:
    void on_LoadPlotBtn_clicked();
    void plotDataSelect(const QString &file);

private:
    void setupPlot(void);
    void compareMaxValue(float &value,float &maxValue,float &minValue);
    void resetChart(void);

    Ui::PlotReViewDialog *ui;
    QChart *m_chart;
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;
    QLineSeries *m_series;
    QVector<QPointF> m_vctData;
    int m_Count;
    float m_maxY;
    float m_minY;

};

#endif // PLOTREVIEWDIALOG_H

