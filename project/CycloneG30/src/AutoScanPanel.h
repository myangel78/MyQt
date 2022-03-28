#pragma once

#include <QWidget>
#include "Global.h"
#include <QTime>
#include <QTimer>

class QPushButton;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QGroupBox;
class QLCDNumber;
class QProgressBar;
class PoreStateMapWdgt;
class CusPlotCurvesCfgDlg;
class QStackedWidget;
class QProgressDialog;
class ValidOptimized;



class AutoScanPanel:public QWidget
{
    Q_OBJECT
public:
    explicit AutoScanPanel(QWidget *parent = nullptr);
    ~AutoScanPanel();

private:
    enum AUTOSCAN_SETTING_PAGE_ENUM{AUTOSCAN_SETTING_OF_VALID_QC_PAGE = 0,AUTOSCAN_SETTING_OF_SEQ_PAGE =1 };

    QGroupBox *m_pBigGrpBx= nullptr;
    QGroupBox *m_pScanGrpBx= nullptr;
    QGroupBox *m_pValidQcGrpBx= nullptr;
    QGroupBox *m_pDegatParmGrpBx= nullptr;
    QGroupBox *m_pPeriodGrpBx= nullptr;
    QGroupBox *m_pPoreFiletGrpBx= nullptr;

    QPushButton *m_pSettingPreviousBtn = nullptr;
    QPushButton *m_pSettingNextBtn = nullptr;
    QStackedWidget *m_pSettingStackWgt = nullptr;

    QWidget *m_pQCSettingWgt = nullptr;
    QWidget *m_pSeqSettingWgt = nullptr;

    QDoubleSpinBox *m_pScanDcVoltDpbx= nullptr;
    QDoubleSpinBox *m_pScanACVoltDpbx= nullptr;
    QSpinBox *m_pScantimesDpbx= nullptr;
    QLabel *m_pScanCurSebsorGrplab= nullptr;
    QProgressBar *m_pScanProgresBar = nullptr;
    QPushButton *m_pScanStartBtn = nullptr;

    QDoubleSpinBox *m_pShuotffDcVoltDpbx= nullptr;
    QDoubleSpinBox* m_pFilterCurMinDpbx= nullptr;
    QDoubleSpinBox* m_pFilterCurMaxDpbx= nullptr;
    QDoubleSpinBox* m_pFilterStdMinDpbx= nullptr;
    QDoubleSpinBox* m_pFilterStdMaxDpbx= nullptr;
    QSpinBox* m_PFilterWaitSecSpbx = nullptr;
    QSpinBox* m_pValidFilterTimesSpbx = nullptr;
    QProgressBar *m_pFilterProgresBar = nullptr;
    QPushButton *m_pFilterStartBtn = nullptr;

    QDoubleSpinBox *m_pPosNegaDcVoltDpbx = nullptr;
    QDoubleSpinBox *m_pPoreprotCurrentDpbx = nullptr;
    QSpinBox *m_pDegateDelayMsSpbx = nullptr;
    QSpinBox *m_pDegateTimesSpbx = nullptr;
    QProgressBar *m_pPosNegaProgresBar = nullptr;
    QPushButton *m_pPosNegaStartBtn = nullptr;

    //degating base
    QGroupBox* m_pgrpDegatingBase = nullptr;
    QLabel* m_plabCyclePeriod = nullptr;
    QSpinBox* m_pispCyclePeriod = nullptr;
    QLabel* m_plabDurationTime = nullptr;
    QSpinBox* m_pispDurationTime = nullptr;
    QLabel* m_plabAllowTimes = nullptr;
    QSpinBox* m_pispAllowTimes = nullptr;
    QCheckBox* m_pchkAutoseqGating = nullptr;

    //degating parameter
    QLabel* m_plabDurationThres = nullptr;
    QDoubleSpinBox* m_pdspDurationThres = nullptr;
    QLabel* m_plabGatingSTD = nullptr;
    QSpinBox* m_pispGatingSTD = nullptr;
    QLabel* m_plabGatingSTDMax = nullptr;
    QSpinBox* m_pispGatingSTDMax = nullptr;
    QLabel* m_plabSignalMin = nullptr;
    QSpinBox* m_pispSignalMin = nullptr;
    QLabel* m_plabSignalMax = nullptr;
    QSpinBox* m_pispSignalMax = nullptr;
    QLabel* m_plabGatingMin = nullptr;
    QSpinBox* m_pispGatingMin = nullptr;
    QLabel* m_plabGatingMax = nullptr;
    QSpinBox* m_pispGatingMax = nullptr;

    QDoubleSpinBox* m_pScanCycleDpbx= nullptr;
    QDoubleSpinBox* m_pCyclePeriodDpbx= nullptr;
    QPushButton *m_pStartAutoScanBtn = nullptr;
    QPushButton* m_pNextStepBtn = nullptr;
    QLCDNumber* m_plcdTimerPeriod = nullptr;
    int m_iCycleCount;
    bool m_bIsNextContinue;
    bool m_bIsAutoScanStop;

    QProgressDialog *m_pAutoStartProgresDlg = nullptr;

    QTime m_tmAutoScanStart;
    QTimer m_tmrAutoScanStart;

    ValidOptimized *m_pValidOptimized = nullptr;

    bool m_bScanLockStop  = true;
    bool m_bFiterValidStop = true;



private:
    void InitCtrl(void);
    void InitDegateCtrl(void);
    bool SetTimehms(void);
    void OnResetProgressBar(void);
    void SetIsAutoScanStop(const bool &bIsStop){m_bIsAutoScanStop = bIsStop;}
    bool GetIsAutoScanStop(void){return m_bIsAutoScanStop;}


private slots:
    void OnClickAutoScanStart(const bool &checked);
    void OnSecondTimerTimeoutSlot(void);
    void OnPosNegaStartBtnSlot(const bool &checked);
    void OnScanStartBtnSlot(const bool &checked);
    void OnFilterStartBtnSlot(const bool &checked);

signals:
    void VoltageSetSig(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double &freq);
    void DegatVoltSetSig(const float &negativeVolt);
    void EnablePoreProtectSig(const bool &enable,const float &poreLimtCurrent);
    void SensorGroupChangeSig(const SENSOR_STATE_GROUP_ENUM &sensorGrp);
    void OnUnblockAllChannelSig(const int &delayms);

    void TempControlDegateSig(const bool &enable);
    void UpdateDegatingParametersSig(const float &duratethres,const int &seqStdMin,const int &seqStdMax, const int &signalMin, const int &signalMax,const int &gatingMin,const int &gatingMax);

    void SetDataSaveSignal(const bool &val);
    void GetDataSaveDirIsEmptySig(bool &empty);
    void GenerateMuxModeSig(void);
    void AcquireDegateIsRunningSig(bool &bIsRuning);
    void UpdateDegateBaseParametersSig(const int &cyclePeriod,const int &durationTime,const int &allowtimes, const bool &bAutoseqgating);

    void AutoTimerDegatControlSig(const bool &runOrPause);
    void AcquireAutoTimerDegatIsRunningSig(bool &bIsRunning);

    void NotifyRecordOperateSig(const RECORD_FILE_TYPE_ENUM &type,const RECORD_FILE_OPERT_TYPE_ENUM &operate);


public slots:
    void AutoScanControlPauseSlot(const bool &runOrPause);
    void AcquireAutoScanIsRunningSlot(bool &bIsRunning);
    void OnStopAllActivity(void);
public:
    bool StartScanChannelProcedure(const bool &bStop);
    bool StartFilterValidProcedure(const bool &bStop);
    void SetScanLockParams(const float &dcVolt,const float &acVolt,const int &times);
    void SetFilterValidParams(const float &dcVolt,const float &curMin,const float &curMax,\
                              const float &stdMin,const float &stdMax,const int &wait,const int times);

public:
    void SetPoreStateMapWdgtPtr(PoreStateMapWdgt* pPoreStateMapWdgt) { m_pPoreStateMapWdgt = pPoreStateMapWdgt; }
    void SetCurvesCfgDlgPtr(CusPlotCurvesCfgDlg* pCusPlotCurvesCfgDlg) { m_pCusPlotCurvesCfgDlg = pCusPlotCurvesCfgDlg; }

private:
    PoreStateMapWdgt *m_pPoreStateMapWdgt = nullptr;
    CusPlotCurvesCfgDlg *m_pCusPlotCurvesCfgDlg = nullptr;


};
