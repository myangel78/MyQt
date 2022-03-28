#pragma once

#include <atomic>
#include <qcolor.h>
#include <qsettings.h>
#include <qdatetime.h>
#include <qcoreapplication.h>
#include <thread>
#include <chrono>
#ifdef Q_OS_WIN32
#include <windows.h>
#else
#endif
#include <qsemaphore.h>
#include <algorithm>
#include <regex>
#include "Global.h"
//#include <Point.h>

//#pragma execution_character_set("utf-8")

#include "QNotify.h"

//#define DATASTREAM_TEST


//#define LOCK_PROTECT_DEBUG

//channel all number //1024;// 
constexpr int DEFAULT_SAMPLE_RATE = 5000;
constexpr int SAMPLE_RATE_3_75K = 3750;
constexpr int SAMPLE_RATE_5K = 5000;
constexpr int SAMPLE_RATE_10K = 10000;
constexpr int SAMPLE_RATE_20K = 20000;

constexpr int SEND_PACKAGE_SIZE = 200;
constexpr int HANDLE_PACKAGE_SIZE = 250;

constexpr int PLOT_ADD_THREADS_NUM = 4;

//zero double
constexpr double ZERODOUBLE = 0.00000001;

#define ISDOUBLEZERO(xx) ((xx < ZERODOUBLE) && (xx > -ZERODOUBLE))

#define SAFE_DELETEP(xx) if(xx){delete xx;xx=nullptr;}
#define SAFE_DELETEA(xx) if(xx){delete[] xx;xx=nullptr;}
#define BUF_SIZE 1024
#define USB_CMD_READ_BUF_SIZE 14
#define USB_CMD_VALID_LENGTH 12
#define MULTI_ASYNC_NUM             8
#define WHOLE_FRAME_BUF_SIZE 4128


#define COMPLETE_FRAME_SIZE WHOLE_FRAME_BUF_SIZE
#define COMPLETE_FRAME_64CH_SIZE 258
#define CHANNELS_IN_ONE_FRAME 64

//constexpr char LVDS_FRAMEHEAD_FIRST_2D = 0x2D;
//constexpr char LVDS_FRAMEHEAD_SECOND_5A = 0x5A;

constexpr char LVDS_FRAMEHEAD_FIRST_2D = 0x5A;
constexpr char LVDS_FRAMEHEAD_SECOND_5A = 0x5A;

constexpr unsigned char TAGSTART[16] = {
    0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
};

typedef enum{
    COMMAND_ADDR_ASIC = 1,
    COMMAND_ADDR_FPGA = 2,
}COMMAND_TARGET_ENUM;

typedef struct
{
    QSemaphore freeBuf;
    QSemaphore usedBuf;
}Sem;


const std::vector<QColor> g_vetPenColor =
{
    QColor(0, 0, 0),     QColor(47,79,79),    QColor(128,118,105),
    QColor(0,139,139),   QColor(0,191,255),      QColor(255,0,0),
    QColor(227,23,13),   QColor(255,127,80),     QColor(255,192,203),
    QColor(116,0,0),     QColor(255,255,0),      QColor(227,207,87),
    QColor(255,125,64),  QColor(255,227,132),    QColor(85,102,0),
    QColor(0,255,0),     QColor(128,42,42),      QColor(0,255,255),
    QColor(199,97,20),   QColor(127,255,0),      QColor(210,180,140),
    QColor(8,46,84),     QColor(188,143,143),    QColor(34,139,34),
    QColor(107,142,35),  QColor(199,97,20),      QColor(0,0,255),
    QColor(160,32,240),  QColor(218,112,214),    QColor(25,25,112),
    QColor(138,43,226),  QColor(153,51,250)
};

const static QString g_qlabelStyle[]{
    "<font color=red>WillAutoStopAndSaved</font>"
    , "<font color=red><strong>WillAutoStopAndSaved</strong></font>"
    , "<font color=red><strong><h2>TimeUp</h2></strong></font>"
};



class ConfigServer
{
private:
	ConfigServer();
	ConfigServer(const ConfigServer& other) = delete;
	ConfigServer& operator=(const ConfigServer& other) = delete;
public:
	~ConfigServer() = default;

private:
	static ConfigServer* m_instance;
    int m_iSampleRate = DEFAULT_SAMPLE_RATE;
    double m_dPlotYmax = 5000.0;
    double m_dPlotYmin = -5000.0;
    double m_dPlotXnum = 10.0;
    int m_iUseOpenGL = 0;

    QSettings* m_pSysConfig = nullptr;

    QNotify *m_pNotify = nullptr;

    bool m_bConnect =false;

    double m_iTriangFrequence = 7.8125;
    double m_iTriangAmplitude = 0.05;

    std::atomic<bool> m_bSaveData = false;
    std::atomic<bool> m_bAutoDegating = false;
    std::atomic<bool> m_bEnCrcCalc = false;
    std::atomic<bool> m_bEnCrcCalcPrint = false;
    std::atomic<bool> m_bStreamPrint = false;
    std::atomic<bool> m_bStreamRunning = false;
    std::atomic<bool> m_bSimulateRunning = false;

    std::atomic<bool> m_bIsRawData = false;
    std::atomic<bool> m_bIsNeedToTrans = true;

    std::atomic<bool> m_bIsCalculate = false;
    std::atomic<bool> m_bAverageCalc = false;
    std::atomic<bool> m_bStdCurrCalc = false;
    std::atomic<bool> m_bCapCollect = false;
    std::atomic<bool> m_bRealtimeSequence = false;
    std::atomic<bool> m_bIsVoltInvert = false;
    std::atomic<bool> m_bGenerateRunId = true;

    std::atomic<int> m_sensorGrp = SENSOR_STATE_A_GROUP;
    std::atomic<SENSOR_GROUP_MODE> m_sensorGrpMode = SENSOR_A_GROUP;

    std::vector<std::vector<std::pair<float,float>>> m_vctLvdsConvert;
    std::pair<float,float> m_lvdsCurUnitPair;

    std::vector<StateRegUnion> m_vctSensorSteRegVal;

    int m_iWorknet = 1;
    int m_iSequencingId = 0;
    std::string m_strRecordConfig;
    std::string m_strSequenceConfig;
    std::atomic<bool> m_bAlgorithmServerStatus = false;

    std::string m_strLoginUserName;
public:
	struct SDataHandle
	{
		std::vector<std::vector<float>> dataHandle;
#if 0
        SDataHandle(const std::vector<std::vector<float>>& other)
        {
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                dataHandle.emplace_back(other[i]);
            }
        }
#else
        SDataHandle(const std::vector<std::vector<float>>& other)
        {
            dataHandle.resize(other.size());
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                dataHandle[i].resize(other[i].size());
                std::copy(other[i].cbegin(),other[i].cend(), dataHandle[i].begin());
            }
        }

        SDataHandle(std::vector<std::vector<float>>&& other)
        {
            int size =other.size();
            dataHandle.resize(size);
            for(int i =0;i < size; ++i)
            {
                dataHandle[i].swap(other[i]);
            }
//            dataHandle =  std::move(other);
        }
        SDataHandle(const int &channel,const int &reserveSize)
        {
            dataHandle.resize(channel);
            for (int i = 0; i < channel; ++i)
            {
                dataHandle[i].reserve(reserveSize);
            }
        }
#endif
	};

private:

public:
    static ConfigServer* GetInstance(void);

    bool InitConfig(QSettings * pset);
    bool OnLoadLVDSTransCsv(const QString &filePath);

    double GetIsUsbConnect(void) const { return m_bConnect; }
    void SetUSBConnect(bool state) { m_bConnect = state; }

    double GetPlotYmax(void) const { return m_dPlotYmax; }
    void SetPlotYmax(double val) { m_dPlotYmax = val; }

    double GetPlotYmin(void) const { return m_dPlotYmin; }
    void SetPlotYmin(double val) { m_dPlotYmin = val; }

    double GetPlotXnum(void) const { return m_dPlotXnum; }
    void SetPlotXnum(double val) { m_dPlotXnum = val; }

    int GetUseOpenGL(void) const { return m_iUseOpenGL; }
    void SetUseOpenGL(int val)
    {
        m_iUseOpenGL = val;
        m_pSysConfig->beginGroup("plotCtrl");
        m_pSysConfig->setValue("isOpenGL", val);
        m_pSysConfig->endGroup();
    }

    bool GetSaveData(void) const { return m_bSaveData; }
    void SetSaveData(bool val) { m_bSaveData = val; }
    bool GetAutoDegating(void) const { return m_bAutoDegating; }
    void SetAutoDegating(bool val) { m_bAutoDegating = val; }

    int GetSampleRate(void) const { return m_iSampleRate; }
    void SetSampleRate(int val) { m_iSampleRate = val; }

    void SetNotificationPtr(QNotify *ptr){m_pNotify = ptr;}
    QNotify *GetNotifyPtr(void) const{return m_pNotify;}

    bool GetIsSimulateRunning(void) const { return m_bSimulateRunning; }
    void SetSimulateRunning(bool val) { m_bSimulateRunning = val; }
    bool GetIsStreamRunning(void) const { return m_bStreamRunning; }
    void SetStreamRunning(bool val) { m_bStreamRunning = val; }
    bool GetIsCrcCaculate(void) const { return m_bEnCrcCalc; }
    void SetCrcCaculate(bool val) { m_bEnCrcCalc = val; }

    bool GetIsCrcCalPrint(void) const { return m_bEnCrcCalcPrint; }
    void SetCrcCalPrint(bool val) { m_bEnCrcCalcPrint = val; }

    bool GetIsStreamPrint(void) const { return m_bStreamPrint; }
    void SetIsStreamPrint(bool val) { m_bStreamPrint = val; }

    bool GetIsRawData(void) const { return m_bIsRawData; }
    void SetIsRawData(bool val) { m_bIsRawData = val; }

    bool GetIsNeedToTrans(void) const { return m_bIsNeedToTrans; }
    void SetIsNeedToTrans(bool val) { m_bIsNeedToTrans = val; }

    bool GetIsCalculate(void) const { return m_bIsCalculate; }
    void SetIsCalculate(bool val) { m_bIsCalculate = val; }

    bool GetIsAverCurrentCalc(void) const { return m_bAverageCalc; }
    void SetAverCurrentCalc(bool val) { m_bAverageCalc = val; m_bIsCalculate = m_bAverageCalc | m_bStdCurrCalc; }

    bool GetIsStdCurrentCalc(void) const { return m_bStdCurrCalc; }
    void SetStdCurrentCalc(bool val) { m_bStdCurrCalc = val; m_bIsCalculate = m_bAverageCalc | m_bStdCurrCalc; }

    bool GetIsCapCollect(void) const { return m_bCapCollect; }
    void SetCapCollect(bool val) { m_bCapCollect = val; }

    bool GetIsVoltagteInvert(void) const { return m_bIsVoltInvert; }
    void SetVoltagteInvert(const bool &val) { m_bIsVoltInvert = val; }
    bool GetNeedGenerateRunId(void) const { return m_bGenerateRunId; }
    void SetNeedGenerateRunId(const bool &val) { m_bGenerateRunId = val; }
    double GetTriangFrequence(void) const { return m_iTriangFrequence; }
    void SetTriangFrequence(double val) { m_iTriangFrequence = val; }

    double GetTriangAmplitude(void) const { return m_iTriangAmplitude; }
    void SetTriangAmplitude(double val) { m_iTriangAmplitude = val; }

    int GetCurSensorGroup(void) const { return m_sensorGrp; }
    void SetCurSensorGroup(const int &sensorGrp) { m_sensorGrp = sensorGrp; }

    SENSOR_GROUP_MODE GetCurSensorMode(void) const { return m_sensorGrpMode; }
    void SetCurSensorMode(const SENSOR_GROUP_MODE &sensorGrpMode) { m_sensorGrpMode = sensorGrpMode; }
    bool GetRealtimeSequence(void) const { return m_bRealtimeSequence; }
    void SetRealtimeSequence(bool val) { m_bRealtimeSequence = val; }

    const std::string GetLoginUserName(void) const { return m_strLoginUserName; }
    void SetLoginUserName(const std::string& val) { m_strLoginUserName = val; }


    const std::string GetRecordConfig(void) const { return m_strRecordConfig; }
    void SetRecordConfig(const std::string& val) { m_strRecordConfig = val; }
    int GetWorknet(void) const { return m_iWorknet; }
    const std::string GetSequenceConfig(void) const { return m_strSequenceConfig; }
    void SetSequenceConfig(const std::string& val) { m_strSequenceConfig = val; }
    bool GetAlgorithmServerStatus(void) const { return m_bAlgorithmServerStatus; }
    void SetAlgorithmServerStatus(bool val) { m_bAlgorithmServerStatus = val; }
    int GetSequencingId(void) const { return m_iSequencingId; }
    void SetSequencingId(int val) { m_iSequencingId = val; }
    static void caliSleep(int msec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(msec);
        while (QTime::currentTime() < dieTime)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
    static void SleepForTimeoutOrStop(int msec, const bool& bStop)
    {
        QTime dieTime = QTime::currentTime().addMSecs(msec);
        while (QTime::currentTime() < dieTime && !bStop)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }

    static void InterruptSleep(int msec, const bool& inrup)
    {
        int cnt = msec / 100;
        for (int i = 0; (i < cnt) && inrup; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }


    void SetLvdsCvtUnitArry(const int rowIndx,const int colIndx);
    std::pair<float,float> GetLvdsCurConvt(void) const{return m_lvdsCurUnitPair;}

    std::vector<StateRegUnion> & GetSensorStateRegCache(void) {return m_vctSensorSteRegVal;}

    static bool SyncDefaultConfig(const char* pprefix, const char* pconf);
    static int GetExecuteCMD(const char* cmd, char* result, int len);
    static bool GetRunLog(const char* txt, const char* cmd, int len, bool stp = false);
    static bool SplitString2(const char* pSrc, const char* pde, std::vector<std::string>& vetOut);
    static unsigned long GetCurrentThreadSelf(void);
    static std::string GetLocalIPAddress(void);
    static std::string GetCurrentPath(void);
    static std::string GetHomePath(void);
    static std::string GetRecordPath(const RECORD_FILE_TYPE_ENUM &type);
    static std::string GetLogPath(const LOG_FILE_TYPE_ENUM &type);
    static void WidgetPosAdjustByParent(QWidget* parent, QWidget* widget);
};

#ifdef Q_OS_WIN32

class CalTimeElapse
{
public:
    CalTimeElapse(void){
      InitTimeCnt();
    }
    ~CalTimeElapse(){}

public:
    void InitTimeCnt(void);
    void StartCnt();
    void StopCnt();
    void ReStartCnt();
    float GetTimeMsDifference();
    float GetTimeUsDifference();


private:
    LARGE_INTEGER m_timeStart;
    LARGE_INTEGER m_timeEnd;
    LARGE_INTEGER m_frequency;


};

inline void CalTimeElapse::InitTimeCnt(void)
{
      QueryPerformanceFrequency(&m_frequency);
}

inline void CalTimeElapse::StartCnt()
{
    QueryPerformanceCounter(&m_timeStart);
}

inline void CalTimeElapse::StopCnt()
{
    QueryPerformanceCounter(&m_timeEnd);
}

inline void CalTimeElapse::ReStartCnt()
{
    StartCnt();
}

inline float CalTimeElapse::GetTimeMsDifference()
{
    StopCnt();
    return (((m_timeEnd.QuadPart - m_timeStart.QuadPart) * 1000000/ m_frequency.QuadPart) * 0.001);  //mS
}

inline float CalTimeElapse::GetTimeUsDifference()
{
    StopCnt();
    return ((m_timeEnd.QuadPart - m_timeStart.QuadPart) * 1000000 / m_frequency.QuadPart);  //uS
}
#else
#endif
