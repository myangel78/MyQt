#pragma once

#include <QWidget>
#include "Global.h"

class QPushButton;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QTimer;
class QGroupBox;
class QProgressBar;
class PoreStateMapWdgt;


class MembranePanel:public QWidget
{
    Q_OBJECT
public:
    explicit MembranePanel(QWidget *parent = nullptr);
    ~MembranePanel();

    enum SIMULATE_RAMP_PATTERN_ENUM{
        SIMULATE_RAMP_PATTERN_MIRROR =0,
        SIMULATE_RAMP_PATTERN_RISE =1,
        SIMULATE_RAMP_PATTERN_FALL =2,
    };
private:
    QDoubleSpinBox *m_pSimVoltStartDpbx;
    QDoubleSpinBox *m_pSimVoltEndDpbx;
    QDoubleSpinBox *m_pSimVoltStepDpbx;
    QDoubleSpinBox *m_pSimTimeStepDpbx;
    QSpinBox *m_pSimCycleSpbx;
    QComboBox *m_pSimRampPatternComb;
    QLabel *m_pSimCurVoltDispLab;
    QLabel *m_pSimRemainCycleDispLab;

    QPushButton *m_pSimParamDefltBtn;
    QPushButton *m_pSimStartBtn;
    QGroupBox *m_pSimulateGrpbx;

    QLabel *m_plabSimProgressDescr;
    QProgressBar *m_pSimProgressbar;

    float m_fSimVoltStart ;
    float m_fSimVoltEnd ;
    float m_fSimVoltStep;
    float m_fSimTimeStep ;
    int m_iSimulationCycle;
    int m_simSensorGroup;
    SIMULATE_RAMP_PATTERN_ENUM m_eRampPattern;

    bool m_bSimulRise;
    float m_fSimCurVolt ;
    QTimer *m_pTmrSimulation;


    //Polymer
    QLabel* m_plabPoreLimit;
    QDoubleSpinBox* m_pdspPoreLimit;
    QLabel* m_plabPoreProtect;
    QLabel* m_plabPoreProtState;
    QLabel* m_plabVoltStart;
    QDoubleSpinBox* m_pdspVoltStart;
    QLabel* m_plabVoltEnd;
    QDoubleSpinBox* m_pdspVoltEnd;
    QLabel* m_plabVoltStep;
    QDoubleSpinBox* m_pdspVoltStep;
    QLabel* m_plabTimeStep;
    QDoubleSpinBox* m_pdspTimeStep;
    QLabel *m_pPolyCurVoltDispLab;

    QPushButton* m_pbtnPolymerDefault;
    QPushButton* m_pbtnPolymerStart;
    QGroupBox* m_pPolyGroupBox;

    float m_dPoreLimit = 100.0f;
    float m_dVoltStart = 0.15f;
    float m_dVoltEnd = 0.45f;
    float m_dVoltStep = 0.01f;
    float m_dTimeStep = 1.0f;

    float m_dCurVolt = m_dVoltStart;
    QTimer *m_ptmrPolymer;


private slots:
    void OnClickSimulationDefault(void);
    void OnClickSimulationStart(const bool &checked);
    void OnTimerSimulationTimeout(void);

    void OnClickPolymerDefault(void);
    void OnClickPolymerStart(const bool &chk);
    void OnTimerPolymerSimulateTimeout(void);

signals:
    void SetSimulationVoltSig(const float &voltage,const MEMBRANE_SIMULATION_ENUM &type);
    void SensorGroupAndFuncChangeSig(const SENSOR_STATE_GROUP_ENUM &sensorGrp,const SENSORGRP_FUNC_STATE_ENUM &allChFuncState );
    void EnableSimulationSig(const bool &enable);
    void EnablePolymerSimulationSig(const bool &enable);
    void EnablePoreProtectSig(const bool &enable,const float &poreLimtCurrent);
private:
    void InitCtrl(void);
    void InitPolymer(void);

public:
    void SetPoreStateMapWdgtPtr(PoreStateMapWdgt* pPoreStateMapWdgt) { m_pPoreStateMapWdgt = pPoreStateMapWdgt; }

private:
    PoreStateMapWdgt *m_pPoreStateMapWdgt = nullptr;


};
