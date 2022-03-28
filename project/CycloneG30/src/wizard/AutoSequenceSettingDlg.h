#pragma once
#include <QDialog>

class QSettings;
class QTabWidget;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QGroupBox;
class CustomDoubleSpinbox;
class CustomSpinbox;
class QSettings;
class QDialogButtonBox;


class AutoSequenceSettingDlg: public QDialog
{
    Q_OBJECT
public:
    explicit AutoSequenceSettingDlg(QWidget *parent = nullptr);
    virtual ~AutoSequenceSettingDlg();

     void accept() override;

     typedef struct SSeqLockParams
     {
         SSeqLockParams():overScanDcVoL(0.18f),overScanAcVoLt(0.05f),overScanTimes(2){}
         float overScanDcVoL;
         float overScanAcVoLt;
         int overScanTimes;
     }SSeqLockParams;

     typedef struct SSeqValidFiltParams
     {
         SSeqValidFiltParams(): shuotffDcVolt(0.18f),filterCurMin(65.0f),filterCurMax(155.0f),filterStdMin(9.0f),filterStdMax(30.0f),filterWaitSec(6),validFilterTimes(2),\
         bObserveWaitNeed(false),observeWaitTime(5){}
         float shuotffDcVolt;
         float filterCurMin;
         float filterCurMax;
         float filterStdMin;
         float filterStdMax;
         int filterWaitSec;
         int validFilterTimes;
         bool bObserveWaitNeed;
         int observeWaitTime;

     }SSeqValidFiltParams;

     typedef struct SSeqOtherParams
     {
         SSeqOtherParams(): membrSaturatedCurrent(550.0f),autoUnblockPeriod(30.0),oVFScanPeriod(30.0f),oVFScanTimes(2){}
         float membrSaturatedCurrent;
         float autoUnblockPeriod;
         float oVFScanPeriod;
         int oVFScanTimes;
     }SMembQuantityJudgeParams;


     typedef struct SSeqDegateParams
     {
         SSeqDegateParams(): cyclePeriod(2),durationTime(200),allowTimes(10),durationThres(0.1f),gatingSTD(8),gatingSTDMax(18),\
         signalMin(1),signalMax(300),gatingMin(1),gatingMax(180),bAutoseqDegat(true){}
         int cyclePeriod;
         int durationTime;
         int allowTimes;
         float durationThres;
         int gatingSTD;
         int gatingSTDMax;
         int signalMin;
         int signalMax;
         int gatingMin;
         int gatingMax;
         bool bAutoseqDegat;
     }SSeqDegateParams;

private:
    void InitCtrl(void);
    void InitLockCtrl(void);
    void InitValidCtrl(void);
    void InitDegateCtrl(void);
    void InitOtherCtrl(void);

    void SaveParams(void);


private:
    QSettings *m_pSettings = nullptr;
    QTabWidget *m_pTabWidget = nullptr;
    QDialogButtonBox *m_pDialogBtn = nullptr;

    //InitLockCtrl
    QWidget *m_pLockScanWdgt = nullptr;
    CustomDoubleSpinbox *m_pOverScanDcVoLtDpbx= nullptr;
    CustomDoubleSpinbox *m_pOverScanAcVoLtDpbx= nullptr;
    CustomSpinbox *m_pOverScanTimesSpbx=nullptr;

    //InitValidCtrl
    QWidget *m_pValidFiltWdgt = nullptr;
    CustomDoubleSpinbox *m_pShuotffDcVoltDpbx= nullptr;
    CustomDoubleSpinbox* m_pFilterCurMinDpbx= nullptr;
    CustomDoubleSpinbox* m_pFilterCurMaxDpbx= nullptr;
    CustomDoubleSpinbox* m_pFilterStdMinDpbx= nullptr;
    CustomDoubleSpinbox* m_pFilterStdMaxDpbx= nullptr;
    CustomSpinbox* m_PFilterWaitSecSpbx = nullptr;
    CustomSpinbox* m_pValidFilterTimesSpbx = nullptr;
    QCheckBox* m_pFilterNeedObersveChk= nullptr;
    CustomSpinbox* m_pFilterObserveDurationDpbx= nullptr;

    //InitDegate
    QWidget *m_pDegateCtrlWdgt = nullptr;
    //degating base
    QGroupBox* m_pgrpDegatingBase = nullptr;
    QLabel* m_plabCyclePeriod = nullptr;
    CustomSpinbox* m_pispCyclePeriod = nullptr;
    QLabel* m_plabDurationTime = nullptr;
    CustomSpinbox* m_pispDurationTime = nullptr;
    QLabel* m_plabAllowTimes = nullptr;
    CustomSpinbox* m_pispAllowTimes = nullptr;
    QCheckBox* m_pchkAutoseqGating = nullptr;
    //degating parameter
    QGroupBox *m_pDegatParmGrpBx= nullptr;
    QLabel* m_plabDurationThres = nullptr;
    CustomDoubleSpinbox* m_pdspDurationThres = nullptr;
    QLabel* m_plabGatingSTD = nullptr;
    CustomSpinbox* m_pispGatingSTD = nullptr;
    QLabel* m_plabGatingSTDMax = nullptr;
    CustomSpinbox* m_pispGatingSTDMax = nullptr;
    QLabel* m_plabSignalMin = nullptr;
    CustomSpinbox* m_pispSignalMin = nullptr;
    QLabel* m_plabSignalMax = nullptr;
    CustomSpinbox* m_pispSignalMax = nullptr;
    QLabel* m_plabGatingMin = nullptr;
    CustomSpinbox* m_pispGatingMin = nullptr;
    QLabel* m_plabGatingMax = nullptr;
    CustomSpinbox* m_pispGatingMax = nullptr;

    //InitOther
    QWidget *m_pOtherCtrlWdgt = nullptr;
    CustomDoubleSpinbox *m_pMembrSaturatedCurrentDpbx = nullptr;
    CustomDoubleSpinbox *m_pAutoUnblockPeriodDpbx = nullptr;
    CustomDoubleSpinbox *m_pOVFScanPeriodDpbx= nullptr;
    CustomSpinbox *m_pOVFScanTimesSpbx= nullptr;

    SSeqLockParams m_SSeqLockParams;
    SSeqValidFiltParams m_SSeqValidFiltParams;
    SSeqOtherParams m_SSeqOtherParams;
    SSeqDegateParams m_SSeqDegateParams;

public:
    bool LoadConfig(QSettings* pSetting);
    bool SaveConfig(QSettings* pSetting);
    void SetConfigPtr(QSettings* pSetting){ m_pSettings = pSetting;}

    const SSeqLockParams &GetSSeqLockParams(void){return m_SSeqLockParams;}
    const SSeqValidFiltParams &GetSSeqValidFiltParams(void) {return m_SSeqValidFiltParams;}
    const SSeqOtherParams &GetSSeqOtherParams(void) {return m_SSeqOtherParams;}
    const SSeqDegateParams &GetSSeqDegateParams(void) {return m_SSeqDegateParams;}
};

