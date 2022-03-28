#ifndef DATACAPCOLLECTION_H
#define DATACAPCOLLECTION_H
#include <QThread>
#include <windows.h>

class DataCapCollection:public QThread
{
    Q_OBJECT
public:
    DataCapCollection();
    ~ DataCapCollection();

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

    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startCnt;
    LARGE_INTEGER m_stopCnt;

    std::atomic<bool> m_bRunThread = false;
};

#endif // DATACAPCOLLECTION_H
