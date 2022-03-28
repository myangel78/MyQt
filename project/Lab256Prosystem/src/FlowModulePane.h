#pragma once

#include <QWidget>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
//#include <qcheckbox.h>
#include <qtimer.h>
#include <qlcdnumber.h>
#include <qdatetime.h>
#include <qgroupbox.h>


class FlowModulePane : public QWidget
{
    Q_OBJECT
public:
    explicit FlowModulePane(QWidget *parent = nullptr);
    virtual ~FlowModulePane();

private:
    QTabWidget* m_ptabFlow;

    //ChannleQC
    QLabel* m_plabVolt1;
    QDoubleSpinBox* m_pdspVolt1;
    QLabel* m_plabDuration1;
    QDoubleSpinBox* m_pdspDuration1;
    QLabel* m_plabCurrent1;
    QDoubleSpinBox* m_pdspCurrentMin1;
    QDoubleSpinBox* m_pdspCurrentMax1;
    QLabel* m_plabStd1;
    QDoubleSpinBox* m_pdspStdMin1;
    QDoubleSpinBox* m_pdspStdMax1;
    QLabel* m_plabDuration12;
    QDoubleSpinBox* m_pdspDuration12;
    QPushButton* m_pbtnChannelQC;

    //MakeMembrane
    QLabel* m_plabVolt2;
    QDoubleSpinBox* m_pdspVolt2;
    QLabel* m_plabDuration2;
    QDoubleSpinBox* m_pdspDuration2;
    QLabel* m_plabWaitingZeroAdjust2;
    //QCheckBox* m_pchkWaitingZeroAdjust2;
    QLabel* m_plabCurrent2;
    QDoubleSpinBox* m_pdspCurrentMin2;
    QDoubleSpinBox* m_pdspCurrentMax2;
    QLabel* m_plabStd2;
    QDoubleSpinBox* m_pdspStdMin2;
    QDoubleSpinBox* m_pdspStdMax2;
    QPushButton* m_pbtnMakeMembrane;

    //PoreInsert
    QLabel* m_plabVolt3;
    QDoubleSpinBox* m_pdspVolt3;
    QLabel* m_plabDuration3;
    QDoubleSpinBox* m_pdspDuration3;
    QLabel* m_plabStartSinglePoreQC3;
    //QCheckBox* m_pchkStartSinglePoreQC3;
    QLabel* m_plabDuration32;
    QDoubleSpinBox* m_pdspDuration32;
    QLabel* m_plabCurrent3;
    QDoubleSpinBox* m_pdspCurrentMin3;
    QDoubleSpinBox* m_pdspCurrentMax3;
    QPushButton* m_pbtnPoreInsert;

    //PoreQC
    //QLabel* m_plabStopSinglePore;
    QLabel* m_plabWaitingZeroAdjust4;
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
    QLabel* m_plabStartAutoDegating5;
    QLabel* m_plabStartAutoZeroAdjust5;
    //QCheckBox* m_pchkStartAutoDegating5;
    //QCheckBox* m_pchkStartAutoZeroAdjust5;
    QLabel* m_plabTimerQC5;
    QDoubleSpinBox* m_pdspTimerQC5;
    QLCDNumber* m_plcdTimerQC5;
    QLabel* m_plabCurrent5;
    QDoubleSpinBox* m_pdspCurrentMin5;
    QDoubleSpinBox* m_pdspCurrentMax5;
    QPushButton* m_pbtnSequencing;


    //MuxScan
    QGroupBox* m_pgrpQC;
    QLabel* m_plabWaitingZeroAdjust6;
    //QCheckBox* m_pchkWaitingZeroAdjust6;
    //QCheckBox* m_pchkSaveDataStatus6;
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

    QGroupBox* m_pgrpSequencing;
    QLabel* m_plabVolt62;
    QDoubleSpinBox* m_pdspVolt62;
    QLabel* m_plabDuration62;
    QDoubleSpinBox* m_pdspDuration62;
    QLabel* m_plabVolt63;
    QDoubleSpinBox* m_pdspVolt63;
    QLabel* m_plabDuration63;
    QDoubleSpinBox* m_pdspDuration63;
    QLabel* m_plabAutoProtectDegatingZeroAdjust6;
    //QCheckBox* m_pchkStartAutoProtectDegatingZeroAdjust6;
    QLabel* m_plabCycle6;
    QSpinBox* m_pispCycle6;
    QLabel* m_plabTimerPeriod6;
    QDoubleSpinBox* m_pdspTimerPeriod6;
    QLCDNumber* m_plcdTimerPeriod6;
    QPushButton* m_pbtnSequencingQC;


    QTimer m_tmrChannleQC;
    QTimer m_tmrMakeMembrane;
    QTimer m_tmrPoreInsert;
    QTimer m_tmrPoreQC;
    QTimer m_tmrSeqencing;
    QTimer m_tmrSeqencingSelect;
    QTime m_tmSeqencingSelect;
    QTimer m_tmrSeqencingQC;
    QTimer m_tmrSeqencingQCSelect;
    QTime m_tmSeqencingQC;

    bool m_bIsNextContinue = true;
    int m_iCycleCount = 0;

private:
    bool InitCtrl(void);

    bool SetTimehms5(void);
    bool SetTimehms6(void);

    void DoSequencingFirst(void);
    void DoSequencingLoop(void);

public:
    void OnEnableAllCtrl(bool chk);


private slots:
    void timerChannleQCSlot(void);
    void timerMakeMembraneSlot(void);
    void timerPoreInsertSlot(void);
    void timerPoreQCSlot(void);
    void timerSeqencingSlot(void);
    void timerSeqencingSelectSlot(void);
    void timerSeqencingQCSlot(void);
    void timerSeqencingQCSelectSlot(void);

public slots:
    void OnClickChannelQC(void);
    void OnClickMakeMembrane(void);
    void OnClickPoreInsert(void);
    void OnClickPoreQC(void);
    void OnClickNextStep(void);
    void OnClickSeqencing(bool chk);
    void OnClickSeqencingQC(bool chk);

    //void GetNextContinueSlot(bool& val);


signals:
    void SetAllChannelDCLevelSignal(double volt);
    void SetShowChannelDCLevelSignal(double volt);
    void SetSelChannelDCLevelSignal(double volt);
    void SetDCLevelSelectedSignal(double minval, double maxval);
    void SetDCLevelStdSelectedSignal(double minval, double maxval, double minstd, double maxstd);
    void SetEnableSingleHoleSignal(bool val, bool& ret);
    //void SetAutoSingleHoleSignal(bool val, bool& ret);
    void SetClickZeroAdjustSignal(void);
    void SetEnableZeroAdjustSignal(bool val, bool& ret);
    void SetEnableAutoDegatingSignal(bool val, bool& ret);
    void SetCheckAutoDegatingSignal(bool val, bool& ret);
    void SetSeqencingSelectSignal(double val, double minval, double maxval);

    void SetDCLevelStdShowSelectedSignal(double minval, double maxval, double minstd, double maxstd);
    //void SetChannelSaveDataSelectedSignal(void);
    void SetChannelStatusSignal(bool val);
    void SetDataSaveSignal(bool val);
    void SetCancelZeroAdjustSignal(void);
};

