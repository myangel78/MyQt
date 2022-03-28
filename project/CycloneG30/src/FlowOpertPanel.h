#pragma once

#include <QWidget>
#include "Global.h"

#include<QTime>
#include<QTimer>

#include "ModelSharedData.h"
#include "PoreAutoFiltSettingDlg.h"
#include "AutoSequenceSettingDlg.h"
#include "MembraneQCSettingDlg.h"

class QPushButton;
class QGroupBox;
class QTabWidget;
class QProgressBar;
class PoreStateMapWdgt;
class CusPlotCurvesCfgDlg;
class SensorPanel;
class AutoScanPanel;

class QLCDNumber;
class QSettings;
class QGroupBox;
class CustomDoubleSpinbox;
class CustomSpinbox;
class QProgressDialog;



class FlowOpertPanel:public QWidget
{
    Q_OBJECT
public:
    explicit FlowOpertPanel(QWidget *parent= nullptr);
    ~FlowOpertPanel();

    void InitConfig(void);

private:
    enum FLOW_PROCESS_PAGE_ENUM
    {
        FLOW_PROCESS_PAGE_MEMBRANEQC = 0,
        FLOW_PROCESS_PAGE_POREINSERT,
        FLOW_PROCESS_PAGE_POREFILT,
        FLOW_PROCESS_PAGE_SEQUENCY,
    };

    QGroupBox *m_pGroupBox = nullptr;
    QTabWidget *m_pFlowTabWiget = nullptr;

    //membraneQC
    QGroupBox *m_pMembraneQCGrpbx = nullptr;
    QPushButton *m_pMembraneQCWizardBtn = nullptr;
    QPushButton *m_pMembraneQCStartBtn = nullptr;
    MembraneQCSettingDlg *m_pMembraneQCWizardDlg = nullptr;
    QProgressBar *m_pMembraneQCProgresBar = nullptr;
    QSettings *m_pMembraneQCSettings = nullptr;

    //poreInsert
    QGroupBox *m_pPoreInsertGrpbx = nullptr;

    //Automatic filtting single pore
    QGroupBox *m_pPoreAutoFiltGrpbx = nullptr;
    QPushButton *m_pPoreAutoFiltWizardBtn = nullptr;
    QPushButton *m_pPoreAutoFiltStartBtn = nullptr;
    PoreAutoFiltSettingDlg *m_pPoreAutoFiltSettingDlg = nullptr;
    QProgressBar *m_pPoreAutoFiltProgresBar = nullptr;
    QSettings *m_pPoreAutoFiltSettings = nullptr;
    QProgressDialog *m_pObserveMuxFinalProgresDlg = nullptr;

    //Auto sequenciing
    QWidget *m_pAutoSeqWidget = nullptr;
    QGroupBox *m_pAutoSequencyGrpbx = nullptr;
    QPushButton *m_pAutoSequencySetBtn = nullptr;
    AutoSequenceSettingDlg *m_pAutoSequencySettingDlg = nullptr;
    QProgressBar *m_pAutoSequencyProgresBar = nullptr;
    QSettings *m_pAutoSequencySettings = nullptr;

    //LCD倒计时
    CustomSpinbox* m_pAutoSeqCycleDpbx= nullptr;
    CustomDoubleSpinbox* m_pCyclePeriodDpbx= nullptr;
    QPushButton *m_pAutoSequencyStartBtn = nullptr;
    QLCDNumber* m_plcdTimerPeriod = nullptr;
    QGroupBox *m_pPeriodGrpBx = nullptr;
    QTime m_tmAutoSeqStart;
    QTimer m_tmrAutoSeqStart;

    bool m_bAutoMembrQCStop =false;
    bool m_bAutoPoreFiltStop =false;
    bool m_bAutoSeqStop =false;

    std::vector<float> m_vetAvg;
    std::vector<float> m_vetStd;

    int m_iCycleCount = 10;

signals:
    void VoltageSetSig(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double &freq);
    void VoltSetParametersSig(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double freq = 7.8125);
    void SensorGroupChangeSig(const SENSOR_STATE_GROUP_ENUM &sensorGrp);

    void GetAllChAvgSig(std::vector<float> &vetAvg);
    void GetAllChStdSig(std::vector<float> &vetStd);

    void NotifyRecordOperateSig(const RECORD_FILE_TYPE_ENUM &type,const RECORD_FILE_OPERT_TYPE_ENUM &operate);
    void GetDataSaveDirIsEmptySig(bool &empty);
    void AcquireAutoTimerDegatIsRunningSig(bool &bIsRunning);
    void AutoTimerDegatControlSig(const bool &runOrPause);
    void SetDataSaveSignal(const bool &val);
    void EnablePoreProtectSig(const bool &enable,const float &poreLimtCurrent);
    void EnableMembrokProtectSig(const bool &enable,const float &membrokLimtCurrent);
    void TimerDegatEanbleSig(const bool &enbale);

    void AcquireDegateIsRunningSig(bool &bIsRuning);
    void UpdateDegateBaseParametersSig(const int &cyclePeriod,const int &durationTime,const int &allowtimes, const bool &bAutoseqgating);
    void TempControlDegateSig(const bool &enable);
    void UpdateDegatingParametersSig(const float &duratethres,const int &seqStdMin,const int &seqStdMax, const int &signalMin, const int &signalMax,const int &gatingMin,const int &gatingMax);

    void SendFinalMuxSig(const bool &NeedObserRecord,const int &finalMuxCnt);
    void AllChanSensorStateSetOffSig(void);

    void EnableOVFAutoScanSig(const float &duration,const int &scantimes,const bool &enable);


private slots:
    void MembraneQCStartBtnSlot(const bool &checked);
    void PoreAutoFiltStartBtnSlot(const bool &checked);
    void AutoSequencyStartBtnSlot(const bool &checked);
    void TimeUpToAutoSequencySlot(void);

private:
    void InitCtrl(void);
    void InitMembCtrl(void);
    void InitPoreInsertCtrl(void);
    void InitPoreAutoFiltCtrl(void);
    void InitAutoSequenCtrl(void);

    void MembraneQCStart(void);
    void PoreAutoFiltStart(void);
    bool AutoSequencyStart(void);

    void PoreFiltJudgeByMeansAndStd(const PoreAutoFiltSettingDlg::SPoreFiltToValidParameter &poreFiltParameters);
    bool SetTimehms(const float &min);
    bool GetIsAutoSeqStop(void){return m_bAutoSeqStop;}
    void StopAllActivity(void);
    bool ObserveWattingMuxFinal(const int &seconds,const bool &bCanel,int &reCnt);

private:
    PoreStateMapWdgt *m_pPoreStateMapWdgt = nullptr;
    CusPlotCurvesCfgDlg *m_pCusPlotCurvesCfgDlg = nullptr;
    SensorPanel *m_pSensorPanel = nullptr;
    AutoScanPanel *m_pAutoScanPanel = nullptr;
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;
public:
    void SetPoreStateMapWdgtPtr(PoreStateMapWdgt* pPoreStateMapWdgt) { m_pPoreStateMapWdgt = pPoreStateMapWdgt; }
    void SetCurvesCfgDlgPtr(CusPlotCurvesCfgDlg* pCusPlotCurvesCfgDlg) { m_pCusPlotCurvesCfgDlg = pCusPlotCurvesCfgDlg; }
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }
    void SetSensorPanelPtr(SensorPanel* pSensorPanel) { m_pSensorPanel = pSensorPanel; }
    void SetAutoScanPanelPtr(AutoScanPanel* pAutoScanPanel) { m_pAutoScanPanel = pAutoScanPanel; }



};

