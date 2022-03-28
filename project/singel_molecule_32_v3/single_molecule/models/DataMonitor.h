#ifndef DATAMONITOR_H
#define DATAMONITOR_H
#include "DataBase.h"
#include"logger/Log.h"
#include <QObject>


class DataMonitor : public DataBase
{
public:
    DataMonitor();
    ~DataMonitor();


protected:
    virtual void threadFunc();

private:
    void initTimeCnt();
    void setReStartCnt();
    void setStopCnt();
    double getTimeDifference();


private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startCnt;
    LARGE_INTEGER m_stopCnt;
};

#endif // DataMonitor_H
