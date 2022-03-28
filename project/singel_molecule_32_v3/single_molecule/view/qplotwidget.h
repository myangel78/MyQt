#ifndef QPLOTWIDGET_H
#define QPLOTWIDGET_H
#include "models/DataBase.h"
#include <QWidget>
#include "models/costomplot/qcustomplot.h"
#include <mutex>
#include "models/DataDefine.h"

#include <QWidget>


namespace Ui {
class QPlotwidget;
}

class QPlotwidget : public QWidget,public DataBase
{
    Q_OBJECT

public:
    explicit QPlotwidget(QWidget *parent = nullptr);
    ~QPlotwidget();

    void enableOpengl();
    void LoadConfig(QSettings *nConfig);
    void SaveConfig(QSettings * nConfig);


signals:
    void replotSignal(int layorName);
public slots:
    void GraphVisibleSlot(int channel,bool isVisible);
    void GraphColorChangeSlot(int channel,QColor color);
    void GraphAllVisibleSlot(bool isVisible);  //统一设置，避免32个信号槽


protected:
    virtual void threadFunc();

private slots:
     void replotSlot(int layorNum);
     void plotSetSlot(void);

private:
     void saveRangeParam(double yMax,double yMin,double xRange)\
        { m_nYMax =yMax; m_nYmin = yMin; m_nXRange = xRange;}
     bool isSameRangeSet(double yMax,double yMin,double xRange)\
        { return yMax == m_nYMax && yMin == m_nYmin && xRange == m_nXRange;}

    Ui::QPlotwidget *ui;
    QCustomPlot *m_pPlot;

    double m_nXRange;
    double m_nYMax;
    double m_nYmin;

    std::mutex m_lock;
    QSettings *m_nConfig;
};

#endif // QPLOTWIDGET_H


