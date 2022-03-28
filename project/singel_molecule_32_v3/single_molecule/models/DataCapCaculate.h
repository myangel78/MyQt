#ifndef DATACAPCACULATE_H
#define DATACAPCACULATE_H
#include <QThread>
#include <windows.h>


class DataCapCaculate:public QThread
{
      Q_OBJECT
public:
    DataCapCaculate();
    ~ DataCapCaculate();

    void startThread();
    void stopThread();
    float GetMean(const QVector<float> &in);
    float CalCapacitance(const float &meanCurrent);

protected:
    void run();

private:

    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startCnt;
    LARGE_INTEGER m_stopCnt;

    std::atomic<bool> m_bRunThread = false;
};

#endif // DATACAPCACULATE_H
