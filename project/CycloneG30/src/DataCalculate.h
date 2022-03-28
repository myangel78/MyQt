#pragma once

#include <QObject>
#include <mutex>
#include <condition_variable>
#include <concurrentqueue.h>
#include "ConfigServer.h"
class SensorPanel;
class PoreStateMapWdgt;
class QTimer;

class DataCalculate:public QObject
{
    Q_OBJECT
public:
    DataCalculate(QObject *parent = nullptr);
    ~DataCalculate();

private:
    bool m_bRunning = false;
    std::mutex m_mutexTask;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

    std::vector<float> m_vetAverUnitResult;  //average calculate unit result;
    std::vector<std::vector<float>> m_vetAverageUnitData;//avg unit result vector

    std::vector<float> m_vetAverResult;  //average calculate result;
    std::vector<float> m_vetStdResult;  //std calculate result;
    std::vector<std::vector<float>> m_vetAverageData;//avg current
    std::vector<std::vector<float>> m_vetStdData;//std current

    std::mutex m_mutexavgstd;

public:
    std::condition_variable m_cv;
    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataCalc;

signals:
    void SendAverageRultForPoreInsertSig(std::vector<float> &vetAverage);
    void SendStdRultForShutOffChannSig(std::vector<float> &vetStdRuslt);
    void SendAverageRultForMembroProtectSig(std::vector<float> &vetStdRuslt);


public slots:
    void GetCurStdForShutOffSlot(void);
    void GetCurForPoreInsertSlot(void);
    void GetCurForMembrokProtectSlot(void);

    void GetAllChAvgSlot(std::vector<float> &vetAvg);
    void GetAllChStdSlot(std::vector<float> &vetStd);

public:
    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);


    const std::vector<float> & GetCurrentAvgRef(void) {return m_vetAverResult; }
    const std::vector<float> & GetCurrentStdRef(void) {return m_vetStdResult; }

//    void SetSensorPanelPtr(SensorPanel* pSensorPanel) { m_pSensorPanel = pSensorPanel; }
//    void SetPoreStateMapWdgtPtr(PoreStateMapWdgt* pPoreStateMapWdgt) { m_pPoreStateMapWdgt = pPoreStateMapWdgt; }

//private:
//    SensorPanel *m_pSensorPanel = nullptr;
//    PoreStateMapWdgt *m_pPoreStateMapWdgt = nullptr;

};

