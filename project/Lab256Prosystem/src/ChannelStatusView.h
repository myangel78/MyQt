#pragma once

#include <QWidget>
#include <qgroupbox.h>
#include <qtablewidget.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qfile.h>
#include <qtextstream.h>
#include <mutex>
#include <condition_variable>

#include "ConfigServer.h"


class ControlView;
class CapUnitItem;
class CapacitanceView;

class ChannelStatusView : public QWidget
{
    Q_OBJECT
public:
    explicit ChannelStatusView(QWidget *parent = nullptr);
    virtual ~ChannelStatusView();

private:
    QGroupBox* m_pgrpChanState;
    QTableWidget* m_ptabChanState;

    const int m_ciRow = 10;
    const int m_ciCol = 29;

    static const int m_ciStageNum = 7;
    QGroupBox* m_pgrpLevel[m_ciStageNum];
    QLabel* m_plabLevel[m_ciStageNum];
    QToolButton* m_pbtnLevel[m_ciStageNum];
    QLabel* m_plabLevel2[m_ciStageNum];
    QLabel* m_plabLevel3[m_ciStageNum];

    const QString m_strLevel[m_ciStageNum] = {
        "NonePore"
        , "SinglePore"
        , "MultiPore"
        , "ProtectPore"
        , "Saturated"
        , "Inactive"
        , "Slip"
    };

    QColor m_clrLevel[m_ciStageNum] = {
        QColor(168, 168, 168)
        , QColor(0, 128, 0)
        , QColor(255, 0, 0)
        , QColor(0, 176, 0)
        , QColor(178, 77, 155)
        , QColor(139, 194, 234)
        , QColor(0, 109, 156)
    };


    QGroupBox* m_pgrpStartStop;
    QLabel* m_plabInterval;
    QSpinBox* m_pispInterval;
    QCheckBox* m_pchkMergeCap;
    //QPushButton* m_pbtnOpenFolder;
    QPushButton* m_pbtnStartStop;


    QTimer m_tmrChannelStatus;

    ControlView* m_pControlView = nullptr;
    CapacitanceView* m_pCapacitanceView = nullptr;


    std::vector<int> m_vetLevelCount;
    QFile* m_pFile = nullptr;
    QTextStream* m_pTextStream = nullptr;

private:
    bool InitCtrl(void);
    bool SetColor(QToolButton* pbtn, QColor clr);


public:
    void SetControlView(ControlView* pview) { m_pControlView = pview; }
    void SetCapacitanceView(CapacitanceView* pview) { m_pCapacitanceView = pview; }
    bool ReloadCtrl(void);


private:
    bool m_bRunning = false;;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

public:
    volatile int waiter_cnt = 0;
    std::condition_variable m_cv;
    std::mutex m_mutexTask;

public:
    bool StartThread(void);
    void ThreadFunc(void);

    bool EndThread(void);
    bool StopThread(void);


public slots:
    void OnClickChangeColor(void);
    void OnCheckMergeCap(bool chk);
    void OnClickOpenFolder(void);
    void OnClickStartStop(bool chk);

    void SetChannelStatusSlot(bool val);

private slots:
    void OnTimerChannelStatusSlot(void);

signals:
    //void GetCapacitanceColorSignal(QColor* pclr);
    //void GetCapacitanceMergeSignal(bool* pmerge);
};

