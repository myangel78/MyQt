#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H
#include <QThread>
#include <windows.h>

class DataCollection:public QThread
{
    Q_OBJECT
public:
    DataCollection();
    ~ DataCollection();

    void startThread();
    void stopThread();
signals:
    void dispInstantCurrentSig(QVector<float> dispValue);


protected:
    void run();

private:
    void initTimeCnt();
    void setReStartCnt();
    float getTimeMsDifference();
    float GetMean(QVector<float> &in);

    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startCnt;
    LARGE_INTEGER m_stopCnt;

    std::atomic<bool> m_bRunThread = false;
};

#endif // DATACOLLECTION_H
