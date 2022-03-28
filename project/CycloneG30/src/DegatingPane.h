#pragma once

#include <QWidget>
#include <QDialog>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <concurrentqueue.h>

#include "ConfigServer.h"
#include "ModelSharedData.h"

class PoreStateMapTabModel;
class SensorPanel;


class DegatingPane : public QDialog
{
    Q_OBJECT
public:
    explicit DegatingPane(QWidget *parent = nullptr);

private:
    //degating base
    QGroupBox* m_pgrpDegatingBase = nullptr;
    QLabel* m_plabDegatingVolt = nullptr;
    QDoubleSpinBox* m_pdspDegatingVolt = nullptr;
    QLabel* m_plabCyclePeriod = nullptr;
    QSpinBox* m_pispCyclePeriod = nullptr;
    QLabel* m_plabDurationTime = nullptr;
    QSpinBox* m_pispDurationTime = nullptr;
    QLabel* m_plabSteppingTime = nullptr;
    QSpinBox* m_pispSteppingTime = nullptr;
    QLabel* m_plabAllowTimes = nullptr;
    QSpinBox* m_pispAllowTimes = nullptr;
    QLabel* m_plabLimitCycle = nullptr;
    QSpinBox* m_pispLimitCycle = nullptr;

    QCheckBox* m_pchkAutoseqGating = nullptr;
    QSpinBox* m_pispGatingTime = nullptr;
    QSpinBox* m_pispGatingChannel = nullptr;
    QPushButton* m_pbtnDegatingEnable = nullptr;
    QPushButton* m_pbtnDegatingApply = nullptr;

    //degating advanced
    QGroupBox* m_pgrpDegatingAdvanced = nullptr;
    QLabel* m_plabDurationThres = nullptr;
    QDoubleSpinBox* m_pdspDurationThres = nullptr;
    QLabel* m_plabGatingSTD = nullptr;
    QSpinBox* m_pispGatingSTD = nullptr;
    QLabel* m_plabGatingSTDMax = nullptr;
    QSpinBox* m_pispGatingSTDMax = nullptr;
    QLabel* m_plabSignalMin = nullptr;
    QSpinBox* m_pispSignalMin = nullptr;
    QLabel* m_plabSignalMax = nullptr;
    QSpinBox* m_pispSignalMax = nullptr;
    QLabel* m_plabGatingMin = nullptr;
    QSpinBox* m_pispGatingMin = nullptr;
    QLabel* m_plabGatingMax = nullptr;
    QSpinBox* m_pispGatingMax = nullptr;

    QPushButton* m_pbtnDefaultSet = nullptr;
    QPushButton* m_pbtnAdvancedSet = nullptr;

private:
    //Degating base
    double m_dDegatingVolt = -0.1;
    int m_iCyclePeriod = 2;
    int m_iDurationTime = 200;
    int m_iSteppingTime = 0;
    int m_iAllowTime = 10;
    int m_iLimitCycle = 14;
    //Advance
    double m_dSampleRate = DEFAULT_SAMPLE_RATE;
    double m_dDurationThres = 0.5f;
    int m_iGatingSTD = 5;
    int m_iGatingSTDMax = 105;
    int m_iSignalMin = 20;
    int m_iSignalMax = 150;
    int m_iGatingMin = 40;
    int m_iGatingMax = 130;

public:
    PoreStateMapTabModel* m_pPoreStateMapTab = nullptr;
    SensorPanel* m_pSensorPane = nullptr;
    int m_iDegatingDataTime = 2;
    int m_iDegatingDataPoint = 10000;
    std::string m_strDataName;

private:
    bool InitCtrl(void);

private:
    bool m_bRunning = false;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;
    std::mutex m_mutexTask;

    bool m_bRunning2 = false;
    static const int m_ciThread = 16;
    std::mutex m_mutexGating;
    std::shared_ptr<std::thread> m_pthrGating[m_ciThread] = { nullptr };

    bool m_bRunning3 = false;
    std::mutex m_mutexDegating;
    std::shared_ptr<std::thread> m_pthrDegating = nullptr;

    std::atomic<bool> m_bDegatingSample[m_ciThread] = { false };
    std::vector<int> m_arPoreStateData[m_ciThread];
    std::vector<int> m_arPoreStateData2[m_ciThread];
    int m_arPoreState[CHANNEL_NUM] = { 0 };
    int m_arPoreState2[CHANNEL_NUM] = { 0 };
    std::vector<SENSORGRP_FUNC_STATE_ENUM> m_vctAllchFuncState;
    std::vector<SENSORGRP_FUNC_STATE_ENUM> m_arrBlockChFuncState[m_ciThread];
    std::vector<SENSORGRP_FUNC_STATE_ENUM> m_arrBlockChFuncStateBackup[m_ciThread];
    std::vector<int> m_arryBlockCh[m_ciThread];
    std::vector<int> m_vctUnorderChannel;
    std::vector<SENSORGRP_FUNC_STATE_ENUM> m_vctUnorderChFucState;
    std::atomic<bool> m_bDegatingCalc[m_ciThread] = { false };
    int m_ariCurTimes[CHANNEL_NUM] = { 0 };

public:
    std::condition_variable m_cv;
    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle;
    std::vector<std::vector<float>> m_vetAverageData;//avg current
    std::vector<std::vector<float>> m_vetStdData;//std current
    std::vector<std::vector<float>> m_vetDegatingData;//degating current
    float m_arCurrentInstant[CHANNEL_NUM] = { 0.0 };
    float m_arCurrentAverage[CHANNEL_NUM] = { 0.0 };
    float m_arCurrentStd[CHANNEL_NUM] = { 0.0 };
    bool m_arIsGating[CHANNEL_NUM] = { false };
    std::mutex m_mutexavg;
    std::mutex m_mutexstd;
    std::mutex m_mutexdegating;

    std::condition_variable m_cvGating;
    std::vector<std::vector<float>> m_vetAvgCur;

    std::condition_variable m_cvDegating;

    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);

    bool StartThread2(void);
    void ThreadFunc2Deg(int index, int block);
    void ThreadFunc2Deg2(int index, int block);
    bool EndThread2(void);
    bool StopThread2(void);

    bool StartThread3(void);
    void ThreadFunc3Deg(int block);
    bool EndThread3(void);
    bool StopThread3(void);

    bool EndAndStopAllThread(void);

//private slots:
public slots:
    void onClickEnableDegating(bool chk);
    void onClickDegatingApply(void);
    //void onTimerAutoseqGating(void);

    void onClickDefaultApply(void);
    void onClickAdvanceApply(void);

    void setDegatingDataNameSlot(const QString& qsname);

    void TempControlDegateSlot(const bool &enable);
    void AcquireDegateDelaymsSlot(int &delayms);
    void AcquireDegateIsRunningSlot(bool &bIsRuning);
    void UpdateDegatingParametersSlot(const float &duratethres,const int &seqStdMin,const int &seqStdMax, const int &signalMin, const int &signalMax,const int &gatingMin,const int &gatingMax);
    void UpdateDegateBaseParametersSlot(const int &cyclePeriod,const int &durationTime,const int &allowtimes, const bool &bAutoseqgating);

signals:
     void IncontinueChDifferFuncStateSetSig(const std::vector<int> &vctIncoherentCh,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctIncoherentState);
public:
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }
private:
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;

};

