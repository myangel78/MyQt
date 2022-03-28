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


class MembraneQCSettingDlg: public QDialog
{
    Q_OBJECT
public:
    explicit MembraneQCSettingDlg(QWidget *parent = nullptr);
    virtual ~MembraneQCSettingDlg();

     void accept() override;

     typedef struct SOverFlowFiltParams
     {
         SOverFlowFiltParams():overScanVolt(0.18f),overScanTimes(10){}
         float overScanVolt;
         int overScanTimes;
     }SOverFlowFiltParams;

     typedef struct SOverFlowRecheckParams
     {
         SOverFlowRecheckParams(): directVolt(0.18f),wattingTime(5),overRecheckStd(30.0f),overRecheckMean(100.0f){}
         float directVolt;
         int wattingTime;
         float overRecheckStd;
         float overRecheckMean;
     }SOverFlowRecheckParams;

     typedef struct SMembQuantityJudgeParams
     {
         SMembQuantityJudgeParams(): acVolt(0.05f),wattingTime(10),capLevel1(15.0f),capLevel2(40.0f),capLevel3(100.0f),capLevel4(200.0f){}
         float acVolt;
         int wattingTime;
         float capLevel1;
         float capLevel2;
         float capLevel3;
         float capLevel4;
     }SMembQuantityJudgeParams;

private:
    void InitCtrl(void);
    void InitMembOverFlowFilt(void);
    void InitMembOverFlowRecheck(void);
    void InitMembraneQuantityJudge(void);

    void SaveParams(void);

private:
    QSettings *m_pSettings = nullptr;
    QTabWidget *m_pTabWidget = nullptr;
    QDialogButtonBox *m_pDialogBtn = nullptr;

    //MembOverFlowFilt
    QWidget *m_pMembOverFlowFiltWdgt = nullptr;
    CustomDoubleSpinbox *m_pOverScanVoLtDpbx= nullptr;
    CustomSpinbox *m_pOverScanTimesSpbx=nullptr;

    //MembOverFlowRecheck
    QWidget *m_pMembOverFlowRecheckWdgt = nullptr;
    CustomDoubleSpinbox *m_pOverRecheckVoLtDpbx= nullptr;
    CustomSpinbox *m_pOverRecheckTimeSpbx=nullptr;
    CustomDoubleSpinbox *m_pOverRecheckStdDpbx= nullptr;
    CustomDoubleSpinbox *m_pOverRecheckMeanDpbx= nullptr;

    //MembraneQuantityJudge
    QWidget *m_pMembraneQuantityJudgeWdgt = nullptr;
    CustomDoubleSpinbox *m_pQuantityACVoltDpbx= nullptr;
    CustomSpinbox *m_pQuantityACTimeSpbx=nullptr;
    CustomDoubleSpinbox *m_pQuantityCapLevel1Dpbx= nullptr;
    CustomDoubleSpinbox *m_pQuantityCapLevel2Dpbx= nullptr;
    CustomDoubleSpinbox *m_pQuantityCapLevel3Dpbx= nullptr;
    CustomDoubleSpinbox *m_pQuantityCapLevel4Dpbx= nullptr;

    SOverFlowFiltParams m_SOverFlowFiltParams;
    SOverFlowRecheckParams m_SOverFlowRecheckParams;
    SMembQuantityJudgeParams m_SMembQuantityJudgeParams;

public:
    bool LoadConfig(QSettings* pSetting);
    bool SaveConfig(QSettings* pSetting);
    void SetConfigPtr(QSettings* pSetting){ m_pSettings = pSetting;}

    SOverFlowFiltParams &GetMembOverFlowFiltParams(void){return m_SOverFlowFiltParams;}
    SOverFlowRecheckParams &GetMembOverFlowRecheckParams(void){return m_SOverFlowRecheckParams;}
    SMembQuantityJudgeParams &GetMembQuantityJudgeParams(void){return m_SMembQuantityJudgeParams;}

};


