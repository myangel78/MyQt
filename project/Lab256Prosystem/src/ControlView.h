#pragma once

#include <QWidget>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
//#include <qlistview.h>
#include <qlistwidget.h>
#include <qtimer.h>
#include <qlcdnumber.h>
#include <qprogressdialog.h>
#include <qsettings.h>
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <concurrentqueue.h>

#include "ConfigServer.h"
#include "ControlListItem.h"
#include "RedisAccess.h"


class ControlView : public QWidget
{
    Q_OBJECT
public:
    friend class PoreSelectDialog;
    friend class RecordSettingDialog;
    friend class ChannelStatusView;
    explicit ControlView(QWidget *parent = nullptr);
    ~ControlView();

private:
    //Flow
    QGroupBox* m_grpFlowset;

    QPushButton* m_pbtnLoadConfig;
    QPushButton* m_pbtnSaveConfig;

    QCheckBox* m_pchkAllVisible1;
    QCheckBox* m_pchkShowVisible1;
    QCheckBox* m_pchkAllSeq1;
    QCheckBox* m_pchkShowSeq1;

    QLabel* m_plabShowCount;


    //Flow tab
    QTabWidget* m_ptabFlow;

    //ChannleQC
    QLabel* m_plabVolt1;
    QDoubleSpinBox* m_pdspVolt1;
    QLabel* m_plabDuration1;
    QDoubleSpinBox* m_pdspDuration1;
    QLabel* m_plabCurrent1;
    QDoubleSpinBox* m_pdspCurrentMin1;
    //QDoubleSpinBox* m_pdspCurrentMax1;
    //QLabel* m_plabStd1;
    //QDoubleSpinBox* m_pdspStdMin1;
    //QDoubleSpinBox* m_pdspStdMax1;
    //QLabel* m_plabDuration12;
    //QDoubleSpinBox* m_pdspDuration12;
    //QPushButton* m_pbtnChannelReset;
    QPushButton* m_pbtnChannelQC;

    //MakeMembrane
    QGroupBox* m_pgrpCapTest;
    QLabel* m_plabInterval2;
    QSpinBox* m_pispInterval2;
    QLabel* m_plabTimerDuration2;
    QDoubleSpinBox* m_pdspTimerDuration2;
    QLCDNumber* m_plcdTimerDuration2;
    QPushButton* m_pbtnCapTestTimer;

    QGroupBox* m_pgrpMembraneQC;
    QLabel* m_plabVolt2;
    QDoubleSpinBox* m_pdspVolt2;
    QLabel* m_plabDuration2;
    QDoubleSpinBox* m_pdspDuration2;
    //QPushButton* m_pbtnWaitingZeroAdjust2;
    //QLabel* m_plabWaitingZeroAdjust2;
    //QCheckBox* m_pchkWaitingZeroAdjust2;
    QLabel* m_plabCurrent2;
    QDoubleSpinBox* m_pdspCurrentMin2;
    QDoubleSpinBox* m_pdspCurrentMax2;
    QLabel* m_plabStd2;
    QDoubleSpinBox* m_pdspStdMin2;
    QDoubleSpinBox* m_pdspStdMax2;
    QPushButton* m_pbtnMakeMembrane;

    QGroupBox* m_pgrpCapFilter;
    QLabel* m_plabCap2;
    QDoubleSpinBox* m_pdspCapMin2;
    QDoubleSpinBox* m_pdspCapMax2;
    QPushButton* m_pbtnMakeMembraneStartCap;

    //PoreInsert
    QCheckBox* m_pchkWaitingZeroAdjust3;
    QPushButton* m_pbtnWaitingZeroAdjust3;
    QLabel* m_plabVolt3;
    QDoubleSpinBox* m_pdspVolt3;
    QLabel* m_plabDuration3;
    QDoubleSpinBox* m_pdspDuration3;
    QLabel* m_plabSingleLimit3;
    QDoubleSpinBox* m_pdspSingleLimit3;
    QLabel* m_plabProtectVolt3;
    QDoubleSpinBox* m_pdspProtectVolt3;
    QLabel* m_plabMultiLimit3;
    QDoubleSpinBox* m_pdspMultiLimit3;
    QLabel* m_plabPoreStatus3;
    QSpinBox* m_pispPoreStatus3;
    QLabel* m_plabSaturatedLimit3;
    QDoubleSpinBox* m_pdspSaturatedLimit3;
    //QLabel* m_plabStartSinglePoreQC3;
    //QCheckBox* m_pchkStartSinglePoreQC3;
    QLabel* m_plabTimerDuration3;
    QDoubleSpinBox* m_pdspTimerDuration3;
    QLCDNumber* m_plcdTimerDuration3;
    //QLabel* m_plabCurrent3;
    //QDoubleSpinBox* m_pdspCurrentMin3;
    //QDoubleSpinBox* m_pdspCurrentMax3;
    QPushButton* m_pbtnPoreInsertTimer;
    QPushButton* m_pbtnPoreInsert;

    //PoreQC
    QCheckBox* m_pchkWaitingZeroAdjust4;
    QPushButton* m_pbtnWaitingZeroAdjust4;
    //QLabel* m_plabWaitingZeroAdjust4;
    //QCheckBox* m_pchkWaitingZeroAdjust4;
    QLabel* m_plabVolt4;
    QDoubleSpinBox* m_pdspVolt4;
    QLabel* m_plabDuration4;
    QDoubleSpinBox* m_pdspDuration4;
    QLabel* m_plabVolt42;
    QDoubleSpinBox* m_pdspVolt42;
    QLabel* m_plabDuration42;
    QDoubleSpinBox* m_pdspDuration42;
    QLabel* m_plabVolt43;
    QDoubleSpinBox* m_pdspVolt43;
    QLabel* m_plabDuration43;
    QDoubleSpinBox* m_pdspDuration43;
    QLabel* m_plabCurrent4;
    QDoubleSpinBox* m_pdspCurrentMin4;
    QDoubleSpinBox* m_pdspCurrentMax4;
    QLabel* m_plabStd4;
    QDoubleSpinBox* m_pdspStdMin4;
    QDoubleSpinBox* m_pdspStdMax4;
    QPushButton* m_pbtnPoreQC;

    //Sequencing
    QLabel* m_plabVolt5;
    QDoubleSpinBox* m_pdspVolt5;
    QLabel* m_plabDuration5;
    QDoubleSpinBox* m_pdspDuration5;
    QPushButton* m_pbtnStartAutoDegating5;
    //QLabel* m_plabStartAutoDegating5;
    QPushButton* m_pbtnStartAutoZeroAdjust5;
    //QLabel* m_plabStartAutoZeroAdjust5;
    //QCheckBox* m_pchkStartAutoDegating5;
    //QCheckBox* m_pchkStartAutoZeroAdjust5;
    //QLabel* m_plabTimerDuration5;
    //QDoubleSpinBox* m_pdspTimerDuration5;
    ////QLCDNumber* m_plcdTimerQC5;
    //QLabel* m_plabCurrent5;
    //QDoubleSpinBox* m_pdspCurrentMin5;
    ////QDoubleSpinBox* m_pdspCurrentMax5;
    QPushButton* m_pbtnSequencing;


    //MuxScan
    //QGroupBox* m_pgrpPoreQC;
    QPushButton* m_pbtnWaitingZeroAdjust6;
    QLabel* m_plabVolt6;
    QDoubleSpinBox* m_pdspVolt6;
    QLabel* m_plabDuration6;
    QDoubleSpinBox* m_pdspDuration6;
    QLabel* m_plabCurrent6;
    QDoubleSpinBox* m_pdspCurrentMin6;
    QDoubleSpinBox* m_pdspCurrentMax6;
    QLabel* m_plabStd6;
    QDoubleSpinBox* m_pdspStdMin6;
    QDoubleSpinBox* m_pdspStdMax6;
    QPushButton* m_pbtnNextStep;

    //QGroupBox* m_pgrpSequencing;
    QLabel* m_plabVolt62;
    QDoubleSpinBox* m_pdspVolt62;
    QLabel* m_plabDuration62;
    QDoubleSpinBox* m_pdspDuration62;
    QLabel* m_plabVolt63;
    QDoubleSpinBox* m_pdspVolt63;
    QLabel* m_plabDuration63;
    QDoubleSpinBox* m_pdspDuration63;
    QPushButton* m_pbtnStartAutoDegating6;
    QPushButton* m_pbtnStartAutoZeroAdjust6;
    //QLabel* m_plabTimerDuration6;
    //QDoubleSpinBox* m_pdspTimerDuration6;
    //QLabel* m_plabCurrent62;
    //QDoubleSpinBox* m_pdspCurrentMin62;
    QLabel* m_plabCycle6;
    QSpinBox* m_pispCycle6;
    QLabel* m_plabTimerPeriod6;
    QDoubleSpinBox* m_pdspTimerPeriod6;
    QLCDNumber* m_plcdTimerPeriod6;
    QPushButton* m_pbtnSequencingQC;



    //current
    QGroupBox* m_pgrpCurrent;

    QLabel* m_plabSingleLimit;
    QDoubleSpinBox* m_pdspSingleLimit;
    QPushButton* m_pbtnEnableAuto;
    //QCheckBox* m_pchkAllAuto;
    QLabel* m_plabMultiLimit;
    QDoubleSpinBox* m_pdspMultiLimit;
    QPushButton* m_pbtnEnableProtect;
    //QPushButton* m_pbtnAutoZero;
    QLabel* m_plabSaturatedLimit;
    QDoubleSpinBox* m_pdspSaturatedLimit;
    QPushButton* m_pbtnOnlySinglePore;

    //QLabel* m_plabNonembed;
    //QLabel* m_plabNonembed2;
    //QLabel* m_plabSingle;
    //QLabel* m_plabSingle2;
    //QLabel* m_plabMultiMar;
    //QLabel* m_plabMultiMar2;
    QLabel* m_plabCloseVlot;
    QLabel* m_plabCloseVlot2;
    QLabel* m_plabSingleProtect;
    QLabel* m_plabSingleProtect2;
    QDoubleSpinBox* m_pdspVolt;

    QSpinBox* m_pispPoreStatus;
    QCheckBox* m_pchkKeepShow;

    QCheckBox* m_pchkAllSeq;
    QCheckBox* m_pchkShowSeq;

    QCheckBox* m_pchkAllVisible;
    QCheckBox* m_pchkShowVisible;
    QComboBox* m_pcmbShowChan;

    QCheckBox* m_pchkFilter;
    QDoubleSpinBox* m_pdspFilterMin;
    QDoubleSpinBox* m_pdspFilterMax;
    QLabel* m_plabFilterCount;

    QCheckBox* m_pchkFilterSTD;
    QDoubleSpinBox* m_pdspFilterSTDMin;
    QDoubleSpinBox* m_pdspFilterSTD;
    QPushButton* m_pbtnFilter;


    //voltage
    QGroupBox* m_pgrpVoltage;
    QTabWidget* m_ptabVolt;

    //volt
    QRadioButton* m_pradAllChannel;
    QRadioButton* m_pradSingleChannel;
    QLabel* m_plabVoltType;
    QComboBox* m_pcmbVolyType;
    QLabel* m_pLabVoltAmpl;
    QDoubleSpinBox* m_pdspVoltAmpl;
    QLabel* m_plabVoltFreq;
    QDoubleSpinBox* m_pdspVolyFreq;
    QCheckBox* m_pchkAllCheck;
    QPushButton* m_pbtnOpposite;
    QPushButton* m_pbtnVoltApply;

    //degating
    QGroupBox* m_pgrpDegatingBase = nullptr;
    QLabel* m_plabDegatingVolt;
    QDoubleSpinBox* m_pdspDegatingVolt;
    QLabel* m_plabCyclePeriod;
    QSpinBox* m_pispCyclePeriod;
    QLabel* m_plabDurationTime;
    QSpinBox* m_pispDurationTime;
    QLabel* m_plabSteppingTime;
    QSpinBox* m_pispSteppingTime;
    QLabel* m_plabAllowTimes;
    QSpinBox* m_pispAllowTimes;
    QLabel* m_plabLimitCycle;
    QSpinBox* m_pispLimitCycle;

    QCheckBox* m_pchkAutoseqGating;
    QPushButton* m_pbtnDegatingEnable;
    QPushButton* m_pbtnDegatingApply;
    //QCheckBox* m_pchkDegatingAllCheck;
    QPushButton* m_pbtnCalcDegating;
    QLabel* m_plabCalcDegating;

    QGroupBox* m_pgrpDegatingAdvanced = nullptr;
    QLabel* m_plabDurationThres;
    QDoubleSpinBox* m_pdspDurationThres;
    QLabel* m_plabGatingSTD;
    QSpinBox* m_pispGatingSTD;
    QLabel* m_plabGatingSTDMax;
    QSpinBox* m_pispGatingSTDMax;
    QLabel* m_plabSignalMin;
    QSpinBox* m_pispSignalMin;
    QLabel* m_plabSignalMax;
    QSpinBox* m_pispSignalMax;
    QLabel* m_plabGatingMin;
    QSpinBox* m_pispGatingMin;
    QLabel* m_plabGatingMax;
    QSpinBox* m_pispGatingMax;
    QPushButton* m_pbtnDefaultSet;
    QPushButton* m_pbtnAdvancedSet;


    //ZeroAdjust
    QGroupBox* m_pgrpZeroAdjust;

    QLabel* m_plabBackwardVoltage;
    QDoubleSpinBox* m_pdspBackwardVoltage;
    QLabel* m_plabBackwardDuration;
    QDoubleSpinBox* m_pdspBackwardDuration;
    QLabel* m_plabZeroVoltDuration;
    QDoubleSpinBox* m_pdspZeroVoltDuration;
    QLabel* m_plabZeroAdjustTime;
    QDoubleSpinBox* m_pdspZeroAdjustTime;
    QPushButton* m_pbtnZeroAdjustDefault;
    QPushButton* m_pbtnZeroAdjust;

    QProgressDialog* m_pProgressDialog;

    //AutoZeroAdjust
    QGroupBox* m_pgrpAutoAdjust;

    QLabel* m_plabIntervalsTimer;
    QSpinBox* m_pispIntervalsTimer;
    QLCDNumber* m_plcdIntervalsTimer;
    QPushButton* m_pbtnZeroAdjustStart;
    QPushButton* m_pbtnZeroAdjustReset;


    //polymer
    QLabel* m_plabPoreLimit;
    QDoubleSpinBox* m_pdspPoreLimit;
    QLabel* m_plabPoreVolt;
    QDoubleSpinBox* m_pdspPoreVolt;
    QLabel* m_plabVoltStart;
    QDoubleSpinBox* m_pdspVoltStart;
    QLabel* m_plabVoltEnd;
    QDoubleSpinBox* m_pdspVoltEnd;
    QLabel* m_plabVoltStep;
    QDoubleSpinBox* m_pdspVoltStep;
    QLabel* m_plabTimeStep;
    QDoubleSpinBox* m_pdspTimeStep;

    QPushButton* m_pbtnPolymerDefault;
    QPushButton* m_pbtnPolymerStart;


    //rotation seqencing
    QLabel* m_plabRotationTime;
    QSpinBox* m_pispRotationTime;
    QLCDNumber* m_plcdRotationTime;
    QLabel* m_plabSeqencingVolt;
    QDoubleSpinBox* m_pdspSeqencingVolt;
    QLabel* m_plabWaitingVolt;
    QDoubleSpinBox* m_pdspWaitingVolt;
    QLabel* m_plabCurseqChannel;
    QComboBox* m_pcmbCurseqChannel;

    QPushButton* m_pbtnStartRotation;
    QPushButton* m_pbtnResetRotation;


    //triangular simulation
    QLabel* m_plabVoltStart3;
    QDoubleSpinBox* m_pdspVoltStart3;
    QLabel* m_plabVoltEnd3;
    QDoubleSpinBox* m_pdspVoltEnd3;
    QLabel* m_plabVoltStep3;
    QDoubleSpinBox* m_pdspVoltStep3;
    QLabel* m_plabTimeStep3;
    QDoubleSpinBox* m_pdspTimeStep3;
    QLabel* m_plabSimulationCycle;
    QSpinBox* m_pispSimulationCycle;
    QLabel* m_plabRampPattern;
    QComboBox* m_pcmbRampPattern;
    QLabel* m_plabVoltOffset;
    QDoubleSpinBox* m_pdspVoltOffset;

    QPushButton* m_pbtnSimulationDefault;
    QPushButton* m_pbtnSimulationStart;


    //layout
    QHBoxLayout* m_phlay1 = nullptr;
    QHBoxLayout* m_phlay2 = nullptr;
    QVBoxLayout* m_pvlay = nullptr;


    //Flow2
    //ZeroAdjust
    double m_dBackwardVolt = -0.18;
    double m_dBackwardTime = 1.0;
    double m_dZeroTime = 3.0;
    double m_dAdjustTime = 6.0;
    int m_iIntervalsTimer = 900;
    //Degating
    //base param
    double m_dDegatingVolt = -0.1;
    int m_iCyclePeriod = 2;
    int m_iDurationTime = 200;
    int m_iSteppingTime = 0;
    int m_iAllowTime = 10;
    int m_iLimitCycle = 14;
    double m_dProtectCurrent = 400.0;
    //advance param Look at Ever before....

    //ChannelQC
    //MakeMembrane
    QTime m_tmMembraneForm;
    QTimer m_tmrMembraneForm;
    //PoreInsert
    QTime m_tmPoreInsert;
    QTimer m_tmrPoreInsert;
    //Sequencing
    QTimer m_tmrSeqZeroAdjust;
    //QTimer m_tmrSequencingFilter;
    //MuxScan
    bool m_bIsNextContinue = true;
    int m_iCycleCount = 0;
    QFile* m_pFile = nullptr;
    QTextStream* m_pTextStream = nullptr;
    QTime m_tmMuxScanStart;
    QTimer m_tmrMuxScanStart;
    //QTimer m_tmrMuxScanFilter;
    QTimer m_tmrRecordDegating;


    //Please Looking forward....^_^
    std::map<int, bool> m_mapChannlShow;

    //control list
    QListWidget* m_plistControlShow;
    std::vector<std::shared_ptr<ControlListItem>> m_vetControlListItem;

    QTimer m_tmrCur;
    QTimer m_tmrAvg;
    QTimer m_tmrStd;

    int m_iStartIndex = 0;
    int m_iEndIndex = CHANNEL_NUM;

    //bool m_bEnableAuto = false;
    QTimer m_tmrAutoZero;
    QTimer m_tmrAutoProtect;

    std::mutex m_mutexavg;
    std::mutex m_mutexstd;
    std::mutex m_mutexdegating;


    //Degating
    double m_dSampleRate = DEFAULT_SAMPLE_RATE;
    double m_dDurationThres = 0.5f;
    int m_iGatingSTD = 5;
    int m_iGatingSTDMax = 105;
    int m_iSignalMin = 20;
    int m_iSignalMax = 150;
    int m_iGatingMin = 40;
    int m_iGatingMax = 130;

    bool m_bAutoseqGating = false;

    QTimer m_tmrDegating;
    QTime m_tmCalculation;
    QTimer m_tmrCalculation;


    //Adjust
    QTime m_tmAdjust;
    QTimer m_tmrAdjust;


    //Polymer
    float m_dPoreLimit = 100.0f;
    float m_dPoreVolt = 0.0f;
    float m_dVoltStart = 0.15f;
    float m_dVoltEnd = 0.45f;
    float m_dVoltStep = 0.01f;
    float m_dTimeStep = 1.0f;

    float m_dCurVolt = m_dVoltStart;
    QTimer m_tmrPolymer;


    //Rotation
    QTime m_tmRotation;
    QTimer m_tmrRotation;


    //Simulation
    enum ESimulationRampPattern
    {
        SRP_MIRROR,
        SRP_RISE,
        SRP_FALL,

        SRP_MAX
    };

    float m_fVoltStart3 = 0.0f;
    float m_fVoltEnd3 = 1.0f;
    float m_fVoltStep3 = 0.1f;
    float m_fTimeStep3 = 1.0f;
    int m_iSimulationCycle = 30;
    ESimulationRampPattern m_eRampPattern = SRP_MIRROR;
    float m_fVoltOffset = 0.0f;

    bool m_bSimulRise = true;
    float m_fCurVolt3 = m_fVoltStart3;
    QTimer m_tmrSimulation;

public:
    bool m_bCheckAutoSeqGating = false;


private:
    bool InitFlowCtrl(void);
    bool InitCurrentCtrl(void);
    bool InitVoltageCtrl(void);
    bool InitControlList(void);
    bool InitLayout(void);

    bool SetTimehms2(void);
    bool SetTimehms3(void);
    //bool SetTimehms5(void);
    bool SetTimehms6(void);

    void GetActiveChannelIndex(int curIndex);

    void SetGatingParam(double durthres, int gatingstd, int signalmin, int signalmax, int gatingmin, int gatingmax)
    {
        m_dDurationThres = durthres;
        m_iGatingSTD = gatingstd;
        m_iSignalMin = signalmin;
        m_iSignalMax = signalmax;
        m_iGatingMin = gatingmin;
        m_iGatingMax = gatingmax;
    }

    //bool GetTimehms(int val, int& h, int& m, int& s);
    bool SetTimehms(void);

    bool SetTimehmsMin(void);


private:
    std::atomic<bool> m_bStdCalculating = false;
    bool m_bRunning = false;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;
    std::mutex m_mutexTask;

    std::mutex m_mutexZeroAdjust;

public:
    volatile int waiter_cnt = 0;
    std::condition_variable m_cv;

    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle;

    std::vector<std::vector<float>> m_vetAverageData;//avg current
    std::vector<std::vector<float>> m_vetStdData;//std current
    std::vector<std::vector<float>> m_vetDegatingData;//degating current
    float m_arCurrentInstant[CHANNEL_NUM] = { 0.0 };
    float m_arCurrentAverage[CHANNEL_NUM] = { 0.0 };
    float m_arCurrentStd[CHANNEL_NUM] = { 0.0 };

    float m_arZeroAdjustOffset[CHANNEL_NUM] = { 0.0 };

    double m_dLastVoltage = 0.0;

public:
    bool LayoutCtrl(void);

    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);

    float GetZeroAdjustOffset(int val)
    {
        std::lock_guard<std::mutex> lock(m_mutexZeroAdjust);
        return m_arZeroAdjustOffset[val];
    }

public:
    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);

    void StartSetTimer(int elapse);
    void StopKillTimer(void);

private:
    bool m_bRunning2 = false;
    static const int m_ciThread = 4;
    std::mutex m_mutexGating;
    std::shared_ptr<std::thread> m_pthrGating[m_ciThread] = { nullptr };

    bool m_bRunning3 = false;
    std::shared_ptr<std::thread> m_ThreadPtr3 = nullptr;
    bool m_bRedisReady = false;
    std::string m_strDataName;
    RedisAccess m_redisAccess;
    int m_arGatingStatus[CHANNEL_NUM] = { 0 };
    QTimer m_tmrAutoseqGating;

    std::atomic<bool> m_bDegatingSample[m_ciThread] = { false };
    int m_iDegatingDataPoint = 10000;

public:
    std::condition_variable m_cvGating;

    std::vector<std::vector<float>> m_vetAvgCur;

    bool StartThread2(void);
    void ThreadFunc2Deg(int index, int block);
    bool EndThread2(void);
    bool StopThread2(void);

    bool InitGatingRedis(void);
    bool StartThread3(void);
    void ThreadFunc3Deg(int index, int block);
    bool EndThread3(void);
    bool StopThread3(void);


private:
    void SetAllChannelDCLevel(double volt);
    void SetSepChannelDCLevel(int chan, double volt);

    void SetPolymerVoltage(double volt);
    void SetSimulationVoltage(double volt);
    void SetOneChannelDCLevel(int chan, double volt);
    void SetOneChannelPulseLevel(int chan, double volt);


//public:
//    bool SetZeroAdjustInterval(int val);
//    bool SetEnableZeroAdjust(bool val);


private slots:
    //void onCheckRelateVolt(bool chk);
    //Flow2
    void DoLoadConfig(const QString& qsconf);
    void DoSaveConfig(const QString& qsconf);
    void DoZeroAdjust(double bkwvolt, double bkwtime, double zerotime, double adjusttime);
    void onClickShowZeroAdjust(void);
    void onClickShowDegating(void);
    void onWaitingZeroAdjust(void);
    void onStartAutoDegating(bool chk);
    void onClickLoadConfig(void);
    void onClickSaveConfig(void);
    void setVoltchkshowEnable(bool chk);
    //ChannelQC
    void onClickChannelReset(void);
    void onClickChannelQC(bool chk);
    //MakeMembrane
    void onClickMakeMembraneStartCapTest(bool chk);
    void onTimerMakeMembraneStartCapTest(void);
    void onClickMakeMembraneStart(bool chk);
    void onClickMakeMembraneStartCapFilter(bool chk);
    //PoreInsert
    void onCheckWaitingZeroAdjust3(bool chk);
    void onClickPoreInsertTimer(bool chk);
    void onTimerPoreInsertStart(void);
    void onClickPoreInsert(void);
    //InitPoreQC
    void onCheckWaitingZeroAdjust4(bool chk);
    void onClickInitPoreQC(bool chk);
    //Sequencing
    void onClickSequencingStart(bool chk);
    void onTimerSequencingZeroAdjust(void);
    //void onTimerSequencingFilter(void);
    //MuxScan
    void DoSequencingLoop(void);
    void onClickNextStep(void);
    void onClickMuxScanStart(bool chk);
    void onTimerMuxScanStart(void);
    //void onTimerMuxScanFilter(void);
    void onTimerRecordDegating(void);


    //comment
    void onCheckAllSeq(bool chk);
    void onCheckShowSeq(bool chk);
    void onCheckAllVisible(bool chk);
    void onCheckShowVisible(bool chk);
    void onPlotPageViewChange(int index);


    //Current
    void onClickEnableAuto(bool chk);
    //void onClickAutoSetZero(bool chk);

    void onClickEnableProtect(bool chk);
    //void onCheckAllAuto(bool chk);

    void onClickShowSinglePore(void);

    void onCheckCurrentFilter(bool chk);
    void onCheckStdFilter(bool chk);
    void onClickFilter(void);

    void timerCurCurrentSlot(void);
    void timerDegatingSlot(void);
    void timerAvgCurrentSlot(void);
    void timerStdCurrentSlot(void);

    void timerAutoZeroSlot(void);
    void timerAutoProtectSlot(void);
    //void setZeroVoltSlot(int chan, int stat);


    void onCheckAllChannel(bool chk);

    void onClickVoltageOpposite(void);
    void onClickVoltageApply(void);

    void onClickEnableDegating(bool chk);
    void onClickDegatingApply(void);
    void onTimerAutoseqGating(void);
    //void onCheckDegatingAllCheck(bool chk);
    void onClickCalculation(bool chk);
    void onTimerCalculationStart(void);

    void onClickDefaultApply(void);
    void onClickAdvanceApply(void);

    void singleChanDegatingSlot(int chan, double volt);
    void singleChanDegatingRecoverSlot(int chan);


    void onClickZeroAdjustDefault(void);
    void onClickZeroAdjust(void);
    void onClickZeroAdjustStart(bool chk);
    void onClickZeroAdjustReset(void);
    void onTimerZeroAdjustStart(void);


    void onClickPolymerDefault(void);
    void onClickPolymerStart(bool chk);
    void onTimerPolymerStart(void);


    void onClickRotationDefault(void);
    void onClickRotationStart(bool chk);
    void onTimerRotationStart(void);


    void onClickSimulationDefault(void);
    void onClickSimulationStart(bool chk);
    void onTimerSimulationStart(void);

public slots:
    void SetAllChannelVoltSlot(double ampl);
    void SetChannelDCLevelSlot(int chan, double volt);

    void setZeroAdjustSpeVoltSlot(double val);
    void setZeroAdjustAllzeroVoltSlot(void);
    void setZeroAdjustStartSlot(void);
    void setZeroAdjustEndSlot(void);
    //void setZeroAdjustEndAvgSlot(void);

    void setTriangeVoltageSlot(int val);

    void setSamplingRateSlot(int rate);

    //Flow
    void SetAllChannelDCLevelSlot(double volt);
    void SetShowChannelDCLevelSlot(double volt);
    void SetSelChannelDCLevelSlot(double volt);
    void SetDCLevelSelectedSlot(double minval, double maxval);
    void SetDCLevelBiggerCloseSlot(double val, double val2);
    void SetSinglePoreSelectedSlot(void);
    void SetDCLevelCapSelectedSlot(double mincap, double maxcap);
    void SetDCLevelAllSlot(double minval, double maxval);
    void SetDCLevelStdAllSlot(double minval, double maxval, double minstd, double maxstd);
    void SetDCLevelStdSelectedSlot(double minval, double maxval, double minstd, double maxstd);
    void SetOnlyShowSelectedSlot(bool val);
    void SetClickZeroAdjustSlot(void);
    void SetEnableZeroAdjustSlot(bool val, bool& ret);
    void SetEnableSingleHoleSlot(bool val, bool& ret);
    //void SetAutoSingleHoleSlot(bool val, bool& ret);
    void SetEnableAutoDegatingSlot(bool val, bool& ret);
    //void SetCheckAutoDegatingSlot(bool val, bool& ret);
    void SetSeqencingSelectSlot(double val, double minval, double maxval);

    void SetCancelZeroAdjustSlot(void);
    void SetDCLevelStdShowSelectedSlot(double minval, double maxval, double minstd, double maxstd);
    //void SetChannelSaveDataSelectedSlot(void);
    void checkChangedSlot(int chan, bool chk);

    //PoreFilter
    void SetPoreFilterChannelSlot(double minval, double maxval);
    void SetNotPoreFilterDCLevelSlot(double volt);
    void SetPoreFilterDCLevelSlot(double volt);

    //Flow2
    void UpdateListItemSlot(void);
    void EnablePoreInsertSlot(bool val);

    void setDegatingDataNameSlot(const QString& qsname);

signals:
    void checkChangedSignal(int chan, bool chk);
    void colorChangedSignal(int chan, const QColor& clr);
    void updateCurCurrentSignal(const std::vector<double>& vetcur);

    void setCalibrationSignal(const float* pavg);

    void SetSelChannelSignal(const bool* psel);
    void SetCloseChannelSignal(const bool* psel);

    void transmitUsbData(ushort addr, uint data);

    //void GetNextContinueSignal(bool& val);

    //Flow2 
    //void setSequencingVoltageSignal(double val);
    //void getSequencingVoltageSignal(double& val);
    void setCapCalculationSignal(int val, int val2);
    void getCapCalculationSignal(float* pval);
    void ShowPoreSelectSignal(bool val);
    void SetChannelStatusSignal(bool val);
    void SetDataSaveSignal(bool val);
};

