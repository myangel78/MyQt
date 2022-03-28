#pragma once

#include <QWidget>
#include "Global.h"
#include "ModelSharedData.h"
#include <QTime>

class QCheckBox;
class QComboBox;
class QTableView;
class PoreStateMapTabModel;
class QPushButton;
class QGroupBox;
class QToolButton;
class QSpinBox;
class QDoubleSpinBox;
class QLabel;
class QSignalMapper;
class UsbFtdDevice;
class CapCaculate;
class QProgressDialog;
class QTabWidget;
class SensorPanel;
class QLCDNumber;
class QTimer;


class PoreStateMapWdgt:public QWidget
{
    Q_OBJECT
public:
    explicit PoreStateMapWdgt(QWidget *parent = nullptr);
    ~PoreStateMapWdgt();

private:
    QTableView *m_pPoreStateMapTabView = nullptr;
    PoreStateMapTabModel * m_pPoreStateMapTabModel = nullptr;

    QComboBox *m_pDispContentComb= nullptr;
    QCheckBox *m_pIsResizeToContent = nullptr;

    //sensor state display
    QGroupBox *m_pGrpSensorbox = nullptr;
    QPushButton *m_pSensorManualRefreshBtn = nullptr;
    int m_SensorStateNum;
    QToolButton* m_pbtnSensorState[SENSOR_STATE_NUM];
    QLabel *m_plabSensorStateCnt[SENSOR_STATE_NUM];
    QColor m_clrSensorState[SENSOR_STATE_NUM];
    QSignalMapper *m_pSensorClrBtnMapper = nullptr;

    //pore state display
    QGroupBox *m_pGrpPorebox = nullptr;
    int m_PoreStateNum;
    QSignalMapper *m_pPoreClrBtnMapper = nullptr;
    QPushButton* m_pbtnPoreSateRest =nullptr;
    QPushButton* m_pbtnPoreSateCount =nullptr;
    QTabWidget *m_pPoreTabWidgt = nullptr;
    QWidget *m_pPoreGrpWdgt[SENSOR_GROUP_NUM];
    QColor m_poreStateColor[SENSOR_GROUP_NUM][PORE_STATE_NUM];
    QToolButton* m_pbtnPoreStateClrChose[SENSOR_GROUP_NUM][PORE_STATE_NUM];
    QLabel *m_plabPoreStateCnt[SENSOR_GROUP_NUM][PORE_STATE_NUM];


    //cap state display
    QGroupBox *m_pGrpCapbox = nullptr;
    int m_CapStateNum;
    QToolButton* m_pbtnCapState[CAP_STATE_NUM];
    QLabel *m_plabCapStateCnt[CAP_STATE_NUM];
    QColor m_clrCapState[CAP_STATE_NUM];
    QSignalMapper *m_pCapClrBtnMapper = nullptr;
    QPushButton *m_pCapCalStartBtn= nullptr;
    QDoubleSpinBox *m_pCaplevelLmtDpbx[CAP_STATE_NUM];
    float m_capLevelLmit[CAP_STATE_NUM];
    QLabel *m_plabCapLevelDescr[CAP_STATE_NUM];
    QPushButton* m_pbtnCapSave =nullptr;
    QPushButton* m_pbtnCapSateRest =nullptr;

    //OVF state display
    QGroupBox *m_pGrpOVFbox = nullptr;
    int m_OVFStateNum;
    QToolButton* m_pbtnOVFState[OVF_STATE_NUM];
    QLabel *m_plabOVFStateCnt[OVF_STATE_NUM];
    QColor m_clrOVFState[OVF_STATE_NUM];
    QSignalMapper *m_pOVFClrBtnMapper = nullptr;
    QPushButton* m_pbtnOVFSateRest =nullptr;
    QPushButton* m_pbtnOVFRefresh =nullptr;
    QPushButton* m_pbtnOVFShutoff =nullptr;
    QPushButton* m_pbtnOVFLock =nullptr;
    QTimer *m_pTimerToAutoOvfScan = nullptr;
    QDoubleSpinBox *m_pTimerOvfScanInptDpbx = nullptr;
    QPushButton *m_pTimerAutoOvfSScanBtn = nullptr;
    QTime m_tmAutoOvfScan;
    QLCDNumber* m_plcdTimerOvfScanPeriod = nullptr;
    int m_ovfScanTimes = 2;

    //Lock operate state display
    typedef enum{
        LOCK_SENSOR_BUTTON_ALLOW = 0,
        LOCK_SENSOR_BUTTON_PROHIBITED = 1,
        LOCK_SENSOR_BUTTON_CLEAR = 3,
    }LOCK_SENSOR_BUTTON_ENUM;

    QGroupBox *m_pGrpLockbox = nullptr;
    QSignalMapper *m_pLockClrBtnMapper = nullptr;
    QTabWidget *m_pLockTabWidgt = nullptr;
    QPushButton* m_pbtnLockSateRest = nullptr;
    QPushButton* m_pbtnLockSateCount = nullptr;
    QPushButton* m_pbtnAllowOperate = nullptr;
    QPushButton* m_pbtnProhibitedOperate = nullptr;
    QPushButton* m_pbtnLockScan = nullptr;
    QSpinBox *m_pScanCntSpbx= nullptr;
    QPushButton* m_pbtnUnblock = nullptr;
    QPushButton* m_pbtnNormal = nullptr;

    QWidget *m_pSensorGrpwidget[SENSOR_GROUP_NUM];
    QColor m_clrLockState[SENSOR_GROUP_NUM][LOCK_OPERATE_STATE_NUM];
    QToolButton* m_pbtnLockState[SENSOR_GROUP_NUM][LOCK_OPERATE_STATE_NUM];
    QLabel *m_plabLockStateCnt[SENSOR_GROUP_NUM][LOCK_OPERATE_STATE_NUM];

    //Valid state display
    QGroupBox *m_pGrpValidbox = nullptr;
    QSignalMapper *m_pValidClrBtnMapper = nullptr;
    QTabWidget *m_pValidTabWidgt = nullptr;
    QPushButton* m_pbtnValidSateRest = nullptr;
    QPushButton* m_pbtnValidOperate = nullptr;
    QPushButton* m_pbtnInValidOperate = nullptr;
    QPushButton* m_pbtnValidCount = nullptr;

    QWidget *m_pValidGrpwidget[SENSOR_GROUP_NUM];
    QColor m_clrValidState[SENSOR_GROUP_NUM][VALID_STATE_NUM];
    QToolButton* m_pbtnValidState[SENSOR_GROUP_NUM][VALID_STATE_NUM];
    QLabel *m_plabValidStateCnt[SENSOR_GROUP_NUM][VALID_STATE_NUM];

    //Mux state display
    QGroupBox *m_pGrpMuxbox = nullptr;
    QSignalMapper *m_pMuxClrBtnMapper = nullptr;
    QPushButton* m_pbtnMuxStateRest = nullptr;
    QPushButton* m_pbtnAllchMuxRandom = nullptr;
    QPushButton* m_pbtnAllchMuxValidState = nullptr;
    QPushButton* m_pbtnAllchMuxValidCount = nullptr;
    QToolButton* m_pbtnMuxGrpColor[SENSOR_GROUP_NUM];
    QColor m_pMuxGrpColor[SENSOR_GROUP_NUM];
    QLabel *m_plabMuxStateCnt[SENSOR_GROUP_NUM];
    QLabel *m_pMuxValidCntDisplay = nullptr;
    QLabel *m_pMuxSinglePoreCntDisplay = nullptr;

    QProgressDialog *m_pProgressDlg =nullptr;
    std::vector<int> m_vctSensorStateCnt;
    std::vector<int> m_vctCapStateCnt;
    std::vector<int> m_vctOVFStateCnt;
    std::vector<int> m_vctMuxStateCnt;
    std::vector<std::vector<int>> m_vctLockStateCnt;
    std::vector<std::vector<int>> m_vctPoreStateCnt;
    std::vector<std::vector<int>> m_vctValidStateCnt;

    std::vector<int> m_vctOVFCountToLock;
    std::vector<PORE_STATE_ENUM> m_vctPoreStatePrev;

    QTabWidget *m_pStateTabWidget = nullptr;


private slots:
    void OnStateModeChangeSlot(const int &index);
    void OnDisplayContentModeChgeSlot(void);
    void OnLockGrpChgeSlot(const int &index);
    void OnPoreSensorGrpChgeSlot(const int &index);
    void OnReizeCotentToViewSlot(bool checked);
    void OnSensorStateRefreshBtnSlot(void);

    void OnClickChangeColor(const int & state);
    void OnClickChangePoreStaeColor(const int & mux);
    void OnClickChangeCapStaeColor(const int & state);
    void OnClickChangeOVFStaeColor(const int & state);
    void OnClickChangeLockStaeColor(const int & state);
    void OnClickChangeValidStaeColor(const int & state);
    void OnClickChangeMuxStaeColor(const int & state);
    void OnLockAllowForbitBtnClickSlot(void);
    void OnAutoOvfScanBtnSlot(const bool &checked);
    void OnTimeUpToAutoOvfScanSlot(void);


    void OnClickCapSaveas(void);

    void OnOVFStateRefreshBtnSlot(void);
    void OnOVFStateShutOffBtnSlot(void);
    void OnOVFStateLockOVFBtnSlot(void);
    void OnAllChValidGenerateMuxBtnSlot(void);
    void OnValidOrNotOperateBtnSlot(void);

    void OnCountValidStateCnt(void);
    void OnCountPoreStateCnt(void);
    void OnCountLockStateCnt(void);
    void OnCountMuxStateCnt(void);

private:
    void InitCtrl(void);

    void InitSensorStateCtrl(void);
    void InitPoreStateCtrl(void);
    void InitCapStateCtrl(void);
    void InitOVFStateCtrl(void);
    void InitLockOperateStateCtrl(void);
    void InitValidStateCtrl(void);
    void InitMuxStateCtrl(void);

    void OnCountSensorStateCnt(void);
    void OnCountOVFStateCnt(void);
    void OnCountCapStateCnt(void);
    void OnCountValidPoreStateCntInMux(void);
    void OnCountValidStateCntForSensorGroup(const SENSOR_STATE_GROUP_ENUM &sensorGrp);
    bool ComaprePreviousPoreState(const PORE_STATE_ENUM &judgeState, PORE_STATE_ENUM &prevPoreState);
    bool SetOVFTimehms(void);



    bool SetBtnColor(QToolButton* pbtn, QColor clr);

    template <typename T>
    bool CountStateNum(const std::vector<T> &vctState,std::vector<int32_t> &vctStateCntRe);


    void UpdateCapLevelLimtInpt(void);
    int AdjudgeCapLevelByValue(const float &val);

    bool LockScanMappingByCycle(const int &cycle,std::array<std::array<int,LVDS_HARD_MAP_COLS>,LVDS_HARD_MAP_ROWS> & matrix);
    std::vector<SENSORGRP_FUNC_STATE_ENUM> &TransScanMatrixToFuncStat(const std::array<std::array<int,LVDS_HARD_MAP_COLS>,LVDS_HARD_MAP_ROWS> & matrix);


signals:
    void SigGetMembraneimitCurrInp(float &membrk);
    void SigGetPoreLimitCurrInp(float &single,float &mulit,float &membrk);
    void SigGetStdLimitInp(float &stdmin,float &stdmax);
    void SigTriangularStartAndGetAmp(const bool bStart,double &amplitude);

    void SigSpecificChanSetFunctState(const int &ch,const SENSORGRP_FUNC_STATE_ENUM &funcState);
    void SigAllChDifferFuncStateSet(const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState);

    void AcquireWhetherPoreLockSig(bool &bPoreLock);
public slots:
    void PoreProtectByMontorAvegeSlot(const std::vector<float> &average);
    void MembrokeProtectByMontorAvegeSlot(const std::vector<float> &average);
    void CapResultFromCalculateSlot(const std::vector<float> &capVue);
    void ShutOffChanByMontorStdSlot(const std::vector<float> &vetStd);
    void ClearPoreAndCapStateSlot(void);
    void UpdateSensorStateSlot(void);
    void OnLockScanClickSlot(void);
    void UpdateValidStateCntSlot(void){ OnCountValidStateCnt();}
    void GenerateMuxModeSlot(void);
    void OnRefreshCountSlot(const STATE_MAP_MODE &state);
    void CapCalStartBtnSlot(const bool &checked);
    void OnAllChValidGenerateMuxRandomBtnSlot(void);
    void OnLockClearClickSlot(void);
    void EnableOVFAutoScanSlot(const float &duration,const int &scantimes,const bool &enable);

public:
    void SetUsbDevicePtr(UsbFtdDevice* pUsbFtdDevice) { m_pUsbFtdDevice = pUsbFtdDevice; }
    void SetCapCalculate(CapCaculate* pCapCaculate) { m_pCapCaculate = pCapCaculate; }
    void SetSensorPanelPtr(SensorPanel* pSensorPanel) { m_pSensorPanel = pSensorPanel; }
    void SetLockScanGroup(const SENSOR_STATE_GROUP_ENUM &sensorGrp,const int &scantimes);
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }
    int GetValidStateCntForSensorGroup(const SENSOR_STATE_GROUP_ENUM &sensorGrp){return m_vctValidStateCnt[sensorGrp][1];}

    bool OnLockScanForAllCh(void);
    void OnCancelScanProgress(void);

    PoreStateMapTabModel* GetPoreStateMapTab(void) { return m_pPoreStateMapTabModel; }

    const std::vector<int> &GetSensorStateCntVct(void)const {return m_vctSensorStateCnt; }
    const std::vector<int> &GetCapStateCntVct(void)const {return m_vctCapStateCnt; }
    const std::vector<std::vector<int>> &GetPoreStateCntVct(void)const {return m_vctPoreStateCnt; }
    const std::vector<std::vector<int>> &GetValidStateCntVct(void)const {return m_vctValidStateCnt; }
    bool UpdateCapLevelLimtInpt(const std::array<float,CAP_STATE_NUM> &levelParams);
    bool OnLockChGreaterCapLevel(const CAP_STATE_ENUM &capLevel);
private:
    UsbFtdDevice *m_pUsbFtdDevice = nullptr;
    CapCaculate *m_pCapCaculate = nullptr;
    SensorPanel *m_pSensorPanel = nullptr;
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;


};
