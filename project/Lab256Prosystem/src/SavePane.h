#pragma once

#include <QWidget>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlcdnumber.h>
#include <qdatetime.h>
#include <qtimer.h>

#include <mutex>
#include <condition_variable>
#include <concurrentqueue.h>

#include "ConfigServer.h"
#include "RecordSettingDialog.h"
//#include "RecordSelectDialog.h"

#include "RedisAccess.h"


class ControlView;


class SavePane : public QWidget
{
    Q_OBJECT
public:
    explicit SavePane(QWidget *parent = nullptr);
    ~SavePane();

private:
    //saving data
    QGroupBox* m_pgrpSavingData;

    QPushButton* m_pbtnConfig;
    //QPushButton* m_pbtnChannalSel;
    QPushButton* m_pbtnStart;
    QSpinBox* m_pispFileSize;
    QPushButton* m_pbtnUpload;
    //QLabel* m_plabSequencingStatus;
    QLineEdit* m_peditSequencingStatus;

    QPushButton* m_pbtnTranslate;
    QPushButton* m_pbtnReplot;

    QLineEdit* m_peditSavePath;
    QPushButton* m_pbtnChooseFolder;
    QPushButton* m_pbtnOpenFolder;

    RecordSettingDialog* m_pdlgRecordSetting;
    //RecordSelectDialog* m_pdlgRecordSelect;

    QTimer m_tmrSequencingStatus;


    //Auto stop saving
    QGroupBox* m_pgrpAutoStopAndSave;

    QLabel* m_plabAutoStopTime;
    QSpinBox* m_pispAutoStopTime;
    QLCDNumber* m_plcdTimer;
    QLabel* m_plabTimeUp;
    QPushButton* m_pbtnStartAuto;
    QPushButton* m_pbtnResetAuto;

    QTime m_tmAutoStopTime;
    QTimer m_tmrAutoStopTime;

    QTimer m_tmrCheckAlgorithmServerState;


    std::string m_strRawSavePath;
    std::vector<bool> m_vetSelChannel;
    std::vector<FILE*> m_vetSaveFile;
    std::vector<std::pair<size_t, int>> m_vetFileLength;
    size_t m_iSplitSize = 1;

    bool m_bRedisReady = false;
    int m_iSelectChan = 0;
    std::string m_strTaskPath;
    std::string m_strSaveName;
    RedisAccess m_redisAccess;

    std::string m_strUploadUser = ("shanzhu");
    std::string m_strUploadPsw = ("shanzhuji");

private:
    bool InitCtrl(void);
    bool InitTransfer(void);
    bool OpenChannelsFile(void);
    bool CloseChannelsFile(void);

    bool SetTimehms(void);

public:
    void SetControlView(ControlView* pview);

    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);

    bool UpdateFolderOwner(const std::string& taskpath);
    bool UploadRawDataFolder(const QString& qtaskpath);

private:
    bool m_bRunning = false;;
    std::mutex m_mutexTask;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

    bool m_bRunning2 = false;;
    std::mutex m_mutexTask2;
    std::shared_ptr<std::thread> m_ThreadPtr2 = nullptr;
    int m_iSequencedChannel = 0;
    bool m_arSequencedChannal[CHANNEL_NUM] = { false };
    float m_arMockChannelData[50] = { 10.0 };
    std::vector<float> m_vetSequencingData[CHANNEL_NUM];

    bool m_bRunning3 = false;;
    std::mutex m_mutexTask3;
    std::shared_ptr<std::thread> m_ThreadPtr3 = nullptr;

public:
    std::condition_variable m_cv;
    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle;

    std::condition_variable m_cv2;
    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle2;


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


public slots:
    void OnClickConfig(void);
    //void onClickChannelSel(void);
    void OnClickStart(bool chk);
    void OnClickChooseUploadFolder(void);

    void TimerSequencingStatusSlot(void);

    void OnClickTranslate(void);
    void OnClickReplot(void);

    void OnClickChooseFolder(void);
    void OnClickOpenFolder(void);

    void SetSelChannelSlot(const bool* psel);
    void SetCloseChannelSlot(const bool* psel);

    void setSamplingRateSlot(int rate);

    void stopRealtimeSequencingAnalysisSlot(const QString qmsg);


    void OnClickStartAuto(bool chk);
    void OnClickResetAuto(void);

    void TimerUpdateTimeSlot(void);

    void onEnableStartSaveDataSlot(bool val);
    void checkAlgorithmServerStatusCodeSlot(void);
    void timerCheckAlgorithmServerStateSlot(void);

    void SetDataSaveSlot(bool val);

signals:
    void startAutoStopTimerSignal(int msec);
    void stopAutoStopTimerSignal(void);
    void resetAutoStopTimerSignal(int min);

    void stopRealtimeSequencingAnalysisSignal(const QString qmsg);

    void onSequencingIdChangedSignal(int val);

    void startStopRecordVoltSignal(int val, const char* ppath);

    void setDegatingDataNameSignal(const QString& qsname);
};

