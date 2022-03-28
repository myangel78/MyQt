#pragma once
#include <QWidget>
#include "Global.h"
#include "ModelSharedData.h"

class QPushButton;
class QSpinBox;
class QComboBox;
class QLabel;
class QDoubleSpinBox;
class QLCDNumber;
class DegatingPane;
class QCheckBox;
class DataCalculate;
class PoreStateMapWdgt;
class CustomPlotWidgt;

#include <QTime>
#include <QTimer>


class MuxScanPanel:public QWidget
{
    Q_OBJECT
public:
    explicit MuxScanPanel(DegatingPane * pDegatingPane,QWidget *parent = nullptr);
    ~MuxScanPanel();
private:
    void InitCtrl(void);
    bool SetTimehms6(void);

private:
    //MuxScan
    QDoubleSpinBox* m_pBackVoltDpbx= nullptr;
    QDoubleSpinBox* m_pBackVoltDurationDpbx= nullptr;
    QDoubleSpinBox* m_pPoreQcVoltDpbx= nullptr;
    QDoubleSpinBox* m_pPoreQcDurationDpbx= nullptr;
    QDoubleSpinBox* m_pFilterCurMinDpbx= nullptr;
    QDoubleSpinBox* m_pFilterCurMaxDpbx= nullptr;
    QDoubleSpinBox* m_pFilterStdMinDpbx= nullptr;
    QDoubleSpinBox* m_pFilterStdMaxDpbx= nullptr;
    QDoubleSpinBox* m_pSeqVoltDpbx= nullptr;
    QDoubleSpinBox* m_pSeqDurationDpbx= nullptr;
    QDoubleSpinBox* m_pZeroVoltDpbx= nullptr;
    QDoubleSpinBox* m_pZeroDurationDpbx= nullptr;

    QDoubleSpinBox* m_pScanCycleDpbx= nullptr;
    QDoubleSpinBox* m_pCyclePeriodDpbx= nullptr;
    QPushButton *m_pStartMuxScanBtn = nullptr;
    QPushButton* m_pNextStepBtn = nullptr;
    QLCDNumber* m_plcdTimerPeriod = nullptr;

    QTime m_tmMuxScanStart;
    QTimer m_tmrMuxScanStart;
    int m_iCycleCount;
    bool m_bIsNextContinue;


    QCheckBox* m_pDegateEnableChk = nullptr;
    QPushButton* m_pDegateStartBtn = nullptr;
    QPushButton* m_pDegatePanlSetBtn = nullptr;
    DegatingPane *m_pDegatingPane = nullptr;
private slots:
    void OnDegatPanelSettClickSlot(void);
    void OnDegatPanelStartClickSlot(const bool &checked);
    void OnClickMuxScanStart(const bool &checked);
    void OnSecondTimerTimeoutSlot(void);

    void OnClickNextStep(void);
private:
    void DoSequencingLoop(void);

signals:
    void VoltageSetSig(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double &freq);
    void SetDataSaveSignal(const bool &val);

    void UpdateAllchSaveSig(const std::vector<bool> &vctIsSave);

    void SigAllChDifferFuncStateSet(const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState);

private:
    DataCalculate *m_pDataCalculate = nullptr;
    PoreStateMapWdgt *m_pPoreStateMapWdgt = nullptr;
    CustomPlotWidgt *m_pCustomPlotWidgt =nullptr;
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;
public:
    void SetDataCalculatePtr(DataCalculate* pDataCalculate) { m_pDataCalculate = pDataCalculate; }
    void SetPoreStateMapWdgtPtr(PoreStateMapWdgt* pPoreStateMapWdgt) { m_pPoreStateMapWdgt = pPoreStateMapWdgt; }
    void SetCustomPlotWdgtPtr(CustomPlotWidgt* pCustomPlotWidgt) { m_pCustomPlotWidgt = pCustomPlotWidgt; }
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }

};
