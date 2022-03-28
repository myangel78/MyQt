#pragma once

#include <QWidget>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>

#include <concurrentqueue.h>

#include "qcustomplot.h"
#include "ConfigServer.h"



class CustomPlotView : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlotView(QWidget *parent = nullptr);

private:
    const double m_cdYmax = 999999.0;
    const double m_cdXmax = 20.0;//99999999.0;
    double m_dPintNum = 10.0;
    double m_dSampleRate = DEFAULT_SAMPLE_RATE;

    QCustomPlot* m_pCustomPlot;
    QLabel* m_plabYmax;
    QDoubleSpinBox* m_pdspYmax;
    QLabel* m_plabYmin;
    QDoubleSpinBox* m_pdspYmin;
    QLabel* m_plabPtnum;
    QDoubleSpinBox* m_pdspPtnum;
    QPushButton* m_pbtnPlotApply;

    std::mutex m_lock;
    bool m_bChangedSample = false;

private:
    bool InitCtrl(void);
    bool InitPlot(void);

public:
    void OnPlotApply(void);

public:
    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);


private:
    bool m_bRunning = false;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

public:
    volatile int waiter_cnt = 0;
    std::condition_variable m_cv;
    std::mutex m_mutexTask;

    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle;

public:
    bool StartThread(void);
    void ThreadFunc(void);

    bool EndThread(void);
    bool StopThread(void);


signals:
    void replotSignal(bool isClear);

private slots:
    void replotSlot(bool isClear);

public slots:
    //void channelChangedSlot(int chan, bool chk, const QColor& clr);
    void checkChangedSlot(int chan, bool chk);
    void colorChangedSlot(int chan, const QColor& clr);

    void setSamplingRateSlot(int rate);
    void OnEnableOpengl(const bool& check);

};

