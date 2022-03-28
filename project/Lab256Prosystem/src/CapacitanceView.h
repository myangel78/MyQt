#pragma once

#include <QWidget>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qtablewidget.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlistwidget.h>
#include <qcolor.h>
#include <qtimer.h>
#include <qfile.h>
#include <qtextstream.h>
#include <map>
#include <thread>
#include <condition_variable>
#include <concurrentqueue.h>

#include "CapListItem.h"
#include "ConfigServer.h"


class CapacitanceView : public QWidget
{
    Q_OBJECT
public:
    friend class ChannelStatusView;
    explicit CapacitanceView(QWidget *parent = nullptr);
    ~CapacitanceView();

private:
    QGroupBox* m_pgrpHeatMap;
    QTableWidget* m_ptabCapHeat;


    static const int m_ciLevelNum = 5;
    QGroupBox* m_pgrpLevel[m_ciLevelNum];
    QLabel* m_plabLevel3[m_ciLevelNum];
    QLabel* m_plabLevel4[m_ciLevelNum];
    QDoubleSpinBox* m_pdspLevel[m_ciLevelNum];
    QToolButton* m_pbtnLevel[m_ciLevelNum];
    //QLabel* m_plabLevel[m_ciLevelNum];
    QLabel* m_plabLevel2[m_ciLevelNum];
    QCheckBox* m_pchkLevel[m_ciLevelNum];

    QGroupBox* m_pgrpHeatMapApply;
    QCheckBox* m_pchkShowChannel;
    QPushButton* m_pbtnHeatMapApply;


    QGroupBox* m_pgrpCapacitance;

    QGroupBox* m_pgrpCapFilter;
    QDoubleSpinBox* m_pdspFilterMin;
    QDoubleSpinBox* m_pdspFilterMax;
    QPushButton* m_pbtnFilterApply;
    QLabel* m_plabFilterCount;

    QGroupBox* m_pgrpCapList;
    QComboBox* m_pcmbCapChannel;
    QPushButton* m_pbtnCapStart;
    QListWidget* m_plistCapChannel;
    QPushButton* m_pbtnCapSave;

    float m_fLevel[m_ciLevelNum] = { 10.0f, 40.0f, 200.0f, 300.0f, 300.0f };
    QColor m_clrLevel[m_ciLevelNum] = { QColor(128, 128, 128), QColor(135, 206, 250), QColor(0, 128, 0), QColor(255, 165, 0), QColor(255, 0, 0) };

    const int m_ciRow = 10;
    const int m_ciCol = 29;
    std::map<int, std::pair<int, int>> m_mapChannelCoordinate;

    int m_iStartIndex = 0;
    int m_iEndIndex = CHANNEL_NUM;
    std::vector<float> m_vetCapValue;
    std::vector< std::shared_ptr<CapListItem>> m_vetCapListItem;
    std::vector<int> m_vetLevelCount;

    QColor m_clrCapColor[CHANNEL_NUM];
    bool m_bMergeStatus[CHANNEL_NUM] = { false };

    int m_iIntervals = 60;//s
    QTimer m_tmrRecordCap;
    QFile* m_pFile = nullptr;
    QTextStream* m_pTextStream = nullptr;

private:
    //bool InitData(void);
    //bool FindChannelCoordinate(int ch, int& ix, int& iy);
    //bool FindCoordinateChannel(int ix, int iy, int& ch);
    bool InitCtrl(void);
    bool SetColor(QToolButton* pbtn, QColor clr);

    void GetActiveChannelIndex(int curIndex);

public:
    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);

    bool ReloadCtrl(void);

private:
    bool m_bRunning = false;;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

public:
    volatile int waiter_cnt = 0;
    std::condition_variable m_cv;
    std::mutex m_mutexTask;

    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle;

    std::vector<std::vector<float>> m_vetMeanCap;
    std::chrono::steady_clock::time_point m_startCnt;
    std::chrono::steady_clock::time_point m_stopCnt;

public:
    bool StartThread(void);
    void ThreadFunc(void);

    bool EndThread(void);
    bool StopThread(void);


public slots:
    void OnClickChangeColor(void);
    void OnCheckMergeStatus(bool chk);
    void OnClickCapFilter(void);
    void OnCapPageViewChange(int index);
    void OnClickCapStart(bool chk);
    void OnClickCapSaveas(void);

    void updateCapListSlot(void);

    //Flow2
    void setCapCalculationSlot(int val, int val2);
    void getCapCalculationSlot(float* pval);

    void OnTimerRecordCapdata(void);

    ////Status
    //void GetCapacitanceColorSlot(QColor* pclr);
    //void GetCapacitanceMergeSlot(bool* pmerge);


signals:
    void updateCapListSignal(void);
    void setTriangeVoltageSignal(int val);
};

