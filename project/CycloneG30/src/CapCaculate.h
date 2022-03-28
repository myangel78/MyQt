#pragma once

#include <QObject>
#include <mutex>
#include <condition_variable>
#include <concurrentqueue.h>
#include "ConfigServer.h"

class CapCaculate:public QObject
{
    Q_OBJECT
public:
    explicit CapCaculate(QObject *parent = nullptr);
    ~CapCaculate();
private:
    bool m_bRunning = false;
    std::mutex m_mutexTask;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

public:
    std::condition_variable m_cv;
    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queCapDataCalc;

    std::vector<float> m_vetCapValue;
    std::vector<std::vector<float>> m_vetMeanCap;
    std::chrono::steady_clock::time_point m_startCnt;
    std::chrono::steady_clock::time_point m_stopCnt;

signals:
    void SigCapResultFromCalculate(std::vector<float> &vetCapVue);
public:
    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);
};
