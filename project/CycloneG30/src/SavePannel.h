#pragma once

#include <QWidget>
#include <mutex>
#include <condition_variable>
#include <concurrentqueue.h>
#include <qtimer.h>

#include "ConfigServer.h"
#include "RedisAccess.h"

class QCheckBox;
class QLabel;
class QLCDNumber;
class QGroupBox;
class QPushButton;
class QSpinBox;
class QLineEdit;
class RecordSettingDialog;

class PlotReviewDialog;



#define RTSA_DATA_TEST 0


class SavePannel:public QWidget
{
    Q_OBJECT
public:
    explicit SavePannel(QWidget *parent = nullptr);
    ~SavePannel();
private:
    //saving data
    QGroupBox* m_pgrpSavingData;

    QPushButton* m_pbtnConfig;
    //QPushButton* m_pbtnChannalSel;
    QPushButton* m_pbtnStart;
    QSpinBox* m_pispFileSize;
    QPushButton* m_pbtnTranslate;
    QPushButton* m_pbtnReplot;
    QPushButton* m_pbtnRawReplot = nullptr;
    QLineEdit* m_peditConnectIP;

    QLineEdit* m_peditSequencingStatus;

    QLineEdit* m_peditSavePath;
    QPushButton* m_pbtnChooseFolder;
    QPushButton* m_pbtnOpenFolder;


    //Auto stop saving
    QGroupBox* m_pgrpAutoStopAndSave;

    QLabel* m_plabAutoStopTime;
    QSpinBox* m_pispAutoStopTime;
    QLCDNumber* m_plcdTimer;
    QLabel* m_plabTimeUp;
    QPushButton* m_pbtnStartAuto;
    QPushButton* m_pbtnResetAuto;

    QTime m_tmAutoStopTime;
    QTimer m_tmerAutoStopTime;

    QTimer m_tmrCheckAlgorithmServerState;
    QTimer m_tmrSequencingStatus;

    bool m_bRedisReady = false;
    //int m_iSelectChan = 0;
    std::string m_strTaskPath;
    std::string m_strSaveName;
    //RedisAccess m_redisAccess;


    std::string m_strRawSavePath;
    std::vector<bool> m_vetSelChannel;
    std::vector<FILE*> m_vetSaveFile;
    std::vector<std::pair<size_t, int>> m_vetFileLength;
#if RTSA_DATA_TEST
    std::string m_strRawSavePath2;
    FILE* m_vetSaveFile2[CHANNEL_NUM];
    std::pair<size_t, int> m_vetFileLength2[CHANNEL_NUM];
#endif

    size_t m_iSplitSize = 1;
    RecordSettingDialog* m_pRecordSettingDialog = nullptr;

    std::string m_strUploadUser = ("wt_shanzhu");//shanzhu
    std::string m_strUploadPsw = ("shanzhuji");

    //demo simulate
    QGroupBox* m_pSimulateGrpbx= nullptr;
    QLineEdit* m_peditSimulateSavePath= nullptr;
    QPushButton* m_pbtnSimulateChooseFolder= nullptr;
    std::vector<FILE*> m_vetSimulateFiles;
    std::string m_strSimulateRawPath;

    //record reads/adaptor
    long long m_algorithmReads;
    long long m_algorithmAdapter;
    std::vector<size_t> m_vctFileCurIndex;

    PlotReviewDialog *m_pPlotReviewDialog = nullptr;
    PlotReviewDialog *m_pRawReviewDialog = nullptr;

private:
    void InitCtrl(void);
    void InitSimulateCtrl(void);
    bool InitTransfer(void);
    bool OpenChannelsFile(void);
    bool CloseChannelsFile(void);
    bool SetTimehms(void);

    bool OpenSimulateFiles(const std::string &simulateRawDir);
    bool CheckSimulateFilesExist(const std::string &loadpath);

public:
    bool UploadRawDataFolder(const QString& qtaskpath);
    bool UploadRunIdToServer(void);
    void EnableShowSimulateGrpbx(const bool &visible);
    void EnableChooseSimulateDir(const bool &enable);
    bool OpenLoadSimulateFiles(const bool &enabled);
    void CloseSimulateFiles(void);
    const std::vector<FILE*> &GetVctSimulateFile(void){return m_vetSimulateFiles;}

private slots:
    void OnClickChooseFolder(void);
    void OnClickOpenFolder(void);
    void OnClickConfig(void);
    void OnClickStart(bool chk);
    void OnClickTranslate(void);
    void OnClickReplot(void);
    void OnClickRawReplot(void);

    void OnClickStartAuto(bool chk);
    void OnClickResetAuto(void);
    void TimerUpdateTimeSlot(void);

    void TimerSequencingStatusSlot(void);

    void stopRealtimeSequencingAnalysisSlot(const QString qmsg);



private:
    bool m_bRunning = false;
    std::mutex m_mutexTask;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

    bool m_bRunning2 = false;;
    std::mutex m_mutexTask2;
    std::shared_ptr<std::thread> m_ThreadPtr2 = nullptr;
    //int m_iSequencedChannel = 0;
    bool m_arSequencedChannal[CHANNEL_NUM] = { false };
    float m_arMockChannelData[50] = { 10.0 };
    std::vector<float> m_vetSequencingData[CHANNEL_NUM];
    std::vector<float> m_vetSequencingData2[CHANNEL_NUM];

    bool m_bRunning3 = false;;
    std::mutex m_mutexTask3;
    std::shared_ptr<std::thread> m_ThreadPtr3 = nullptr;

    //multi Algorithm Server
    static const int m_ciThread = 1;
    std::shared_ptr<std::thread> m_pThreadSeqPtr[m_ciThread] = { nullptr };
    //int m_iRedisPort[m_ciThread] = { 9088,9089,9090,9091 };
    RedisAccess m_redisAccess[m_ciThread];
    int m_arSelectChan[m_ciThread] = { 0 };
    int m_arSequencedChannel[m_ciThread] = { 0 };


public:
    std::condition_variable m_cv;
    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle;

    std::condition_variable m_cv2;
    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle2;

    std::vector<size_t> &GetCurSaveDataIndex(void){return m_vctFileCurIndex;}

    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);

    bool StartThread2(void);
    void ThreadFunc2Seq(void);
    bool EndThread2(void);
    bool StopThread2(void);

    bool StartThread3(void);
    void ThreadFunc3(void);
    bool EndThread3(void);
    bool StopThread3(void);

    bool StopRealtimeSequencingAnalysis(const QString qmsg, bool stp = true);

signals:
    void startAutoStopTimerSignal(int msec);
    void stopAutoStopTimerSignal(void);
    void resetAutoStopTimerSignal(int min);

    void setDegatingDataNameSignal(const QString& qsname);

    void stopRealtimeSequencingAnalysisSignal(const QString qmsg);

public slots:
    void SetDataSaveSlot(const bool &val);
    void SetDataIsSaveSlot(const std::vector<bool> &vctIsSave);
    void GetDataSaveDirIsEmptySlot(bool &empty);

    void checkAlgorithmServerStatusCodeSlot(void);
    void timerCheckAlgorithmServerStateSlot(void);

    void AcquireCurReadsApatersSlot(long long &curReads, long long & curAdapters);
};

