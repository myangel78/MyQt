#include "MembranePanel.h"

#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QProgressBar>
#include <QDoubleSpinBox>
#include "Global.h"
#include "Log.h"
#include "UsbFtdDevice.h"
#include "PoreStateMapWdgt.h"


MembranePanel::MembranePanel(QWidget *parent):QWidget(parent)
{
    InitCtrl();
}

MembranePanel::~MembranePanel()
{

}

void MembranePanel::InitCtrl(void)
{
    m_fSimVoltStart  = 0.0f;
    m_fSimVoltEnd = 0.0f;
    m_fSimVoltStep = 0.0f;
    m_fSimTimeStep = 0.0f;
    m_iSimulationCycle = 1;
    m_simSensorGroup = SENSOR_GROUP_NUM;

    m_bSimulRise =true;
    m_fSimCurVolt = m_fSimVoltStart;
    m_pTmrSimulation = new QTimer(this);
    m_eRampPattern = SIMULATE_RAMP_PATTERN_MIRROR;


    m_pSimulateGrpbx = new QGroupBox(QStringLiteral("Simulation"),this);
    QLabel *pSimVoltStartLab = new QLabel(QStringLiteral("Volt start"),this);
    QLabel *pSimVoltEndLab = new QLabel(QStringLiteral("Volt end"),this);
    QLabel *pSimVoltStepLab = new QLabel(QStringLiteral("Volt step"),this);
    QLabel *pSimTimeStepLab = new QLabel(QStringLiteral("Time step"),this);
    QLabel *pSimCycleLab = new QLabel(QStringLiteral("Cycle"),this);
    QLabel *pSimRampPatterLab = new QLabel(QStringLiteral("Ramp pattern"),this);
    QLabel *pSimCurVoltDispLab = new QLabel(QStringLiteral("CurVolt"),this);
    QLabel *pSimRemainCycleDispLab = new QLabel(QStringLiteral("Remain"),this);
    m_pSimCurVoltDispLab  = new QLabel(QString("0.0"),this);
    m_pSimRemainCycleDispLab = new QLabel(QString("0"),this);
    m_pSimCurVoltDispLab->setFrameShape(QFrame::StyledPanel);
    m_pSimRemainCycleDispLab->setFrameShape(QFrame::StyledPanel);

    m_plabSimProgressDescr = new QLabel(QStringLiteral("A"),this);
    m_pSimProgressbar = new QProgressBar(this);

    m_plabSimProgressDescr->setVisible(false);
    m_pSimProgressbar->setVisible(false);

    m_pSimVoltStartDpbx = new QDoubleSpinBox(this);
    m_pSimVoltEndDpbx = new QDoubleSpinBox(this);
    m_pSimVoltStepDpbx = new QDoubleSpinBox(this);
    m_pSimTimeStepDpbx = new QDoubleSpinBox(this);
    m_pSimCycleSpbx = new QSpinBox(this);
    m_pSimRampPatternComb = new QComboBox(this);

    m_pSimParamDefltBtn = new QPushButton(QStringLiteral("Default"),this);
    m_pSimStartBtn =new QPushButton(QIcon(":/img/img/start.ico"),QStringLiteral("Start Simulation"),this);
    m_pSimStartBtn->setCheckable(true);

    m_pSimRampPatternComb->addItem(QStringLiteral("Mirror"),SIMULATE_RAMP_PATTERN_MIRROR);
    m_pSimRampPatternComb->addItem(QStringLiteral("Rise"),SIMULATE_RAMP_PATTERN_RISE);
    m_pSimRampPatternComb->addItem(QStringLiteral("Fall"),SIMULATE_RAMP_PATTERN_FALL);


    m_pSimVoltStartDpbx->setSuffix(" V");
    m_pSimVoltStartDpbx->setDecimals(2);
    m_pSimVoltStartDpbx->setMinimum(0);
    m_pSimVoltStartDpbx->setMaximum(1.6);
    m_pSimVoltStartDpbx->setSingleStep(0.1);
    m_pSimVoltStartDpbx->setValue(0.0);

    m_pSimVoltEndDpbx->setSuffix(" V");
    m_pSimVoltEndDpbx->setDecimals(2);
    m_pSimVoltEndDpbx->setMinimum(0);
    m_pSimVoltEndDpbx->setMaximum(1.6);
    m_pSimVoltEndDpbx->setSingleStep(0.1);
    m_pSimVoltEndDpbx->setValue(1.6);

    m_pSimVoltStepDpbx->setSuffix(" V");
    m_pSimVoltStepDpbx->setDecimals(2);
    m_pSimVoltStepDpbx->setMinimum(0.01);
    m_pSimVoltStepDpbx->setMaximum(1.6);
    m_pSimVoltStepDpbx->setSingleStep(0.01);
    m_pSimVoltStepDpbx->setValue(0.1);

    m_pSimTimeStepDpbx->setSuffix(" s");
    m_pSimTimeStepDpbx->setDecimals(2);
    m_pSimTimeStepDpbx->setMinimum(0.01);
    m_pSimTimeStepDpbx->setMaximum(100);
    m_pSimTimeStepDpbx->setSingleStep(0.1);
    m_pSimTimeStepDpbx->setValue(1.0);

    m_pSimCycleSpbx->setSuffix("");
    m_pSimCycleSpbx->setMinimum(1);
    m_pSimCycleSpbx->setMaximum(200);
    m_pSimCycleSpbx->setSingleStep(1);
    m_pSimCycleSpbx->setValue(30);


    QGridLayout *pSimGridlayout = new QGridLayout();
    pSimGridlayout->addWidget(pSimCurVoltDispLab,0,0,1,1);
    pSimGridlayout->addWidget(pSimVoltStartLab,0,1,1,1);
    pSimGridlayout->addWidget(pSimVoltEndLab,0,2,1,1);
    pSimGridlayout->addWidget(pSimVoltStepLab,0,3,1,1);
    pSimGridlayout->addWidget(pSimTimeStepLab,0,4,1,1);
    pSimGridlayout->addWidget(pSimRampPatterLab,0,5,1,1);
    pSimGridlayout->addWidget(pSimCycleLab,0,6,1,1);
    pSimGridlayout->addWidget(pSimRemainCycleDispLab,0,7,1,1);
    pSimGridlayout->addWidget(m_plabSimProgressDescr,0,8,1,1);
    pSimGridlayout->addWidget(m_pSimProgressbar,0,9,1,1);

    pSimGridlayout->addWidget(m_pSimCurVoltDispLab,1,0,1,1);
    pSimGridlayout->addWidget(m_pSimVoltStartDpbx,1,1,1,1);
    pSimGridlayout->addWidget(m_pSimVoltEndDpbx,1,2,1,1);
    pSimGridlayout->addWidget(m_pSimVoltStepDpbx,1,3,1,1);
    pSimGridlayout->addWidget(m_pSimTimeStepDpbx,1,4,1,1);
    pSimGridlayout->addWidget(m_pSimRampPatternComb,1,5,1,1);
    pSimGridlayout->addWidget(m_pSimCycleSpbx,1,6,1,1);
    pSimGridlayout->addWidget(m_pSimRemainCycleDispLab,1,7,1,1);
    pSimGridlayout->addWidget(m_pSimParamDefltBtn,1,8,1,1);
    pSimGridlayout->addWidget(m_pSimStartBtn,1,9,1,1);
    pSimGridlayout->setColumnStretch(10,10);


    m_pSimulateGrpbx->setLayout(pSimGridlayout);

    InitPolymer();

    QVBoxLayout *vertlayout = new QVBoxLayout();
    vertlayout->addWidget(m_pSimulateGrpbx);
    vertlayout->addWidget(m_pPolyGroupBox);
    vertlayout->addStretch();

    setLayout(vertlayout);

    connect(m_pSimParamDefltBtn, &QPushButton::clicked, this, &MembranePanel::OnClickSimulationDefault);
    connect(m_pSimStartBtn, &QPushButton::clicked, this, &MembranePanel::OnClickSimulationStart);
    connect(m_pTmrSimulation, SIGNAL(timeout()), this, SLOT(OnTimerSimulationTimeout()));

    connect(m_pbtnPolymerDefault, &QPushButton::clicked, this, &MembranePanel::OnClickPolymerDefault);
    connect(m_pbtnPolymerStart, &QPushButton::clicked, this, &MembranePanel::OnClickPolymerStart);
    connect(m_ptmrPolymer, SIGNAL(timeout()), this, SLOT(OnTimerPolymerSimulateTimeout()));

}

void MembranePanel::InitPolymer(void)
{
    m_ptmrPolymer = new QTimer(this);

    m_pPolyGroupBox = new QGroupBox(QStringLiteral("Polymer simulate"),this);

    m_plabPoreLimit = new QLabel("Pore Limit:", this);
    QLabel *pPolyCurVoltDispLab = new QLabel("CurVolt", this);
    m_pPolyCurVoltDispLab = new QLabel(QStringLiteral("0.0"), this);
    m_pPolyCurVoltDispLab->setFrameShape(QFrame::StyledPanel);

    m_pdspPoreLimit = new QDoubleSpinBox(this);
    m_pdspPoreLimit->setSuffix(" pA");
    m_pdspPoreLimit->setDecimals(2);
    m_pdspPoreLimit->setMinimum(0.0);
    m_pdspPoreLimit->setMaximum(5000.0);
    m_pdspPoreLimit->setSingleStep(10.0);
    m_pdspPoreLimit->setValue(120.0);

    m_plabPoreProtect = new QLabel("Pore protect:", this);
    m_plabPoreProtState = new QLabel(QStringLiteral("Off"),this);
    m_plabPoreProtState->setFrameShape(QFrame::StyledPanel);

    m_plabVoltStart = new QLabel("Volt Start:", this);
    m_pdspVoltStart = new QDoubleSpinBox(this);
    m_pdspVoltStart->setSuffix(" V");
    m_pdspVoltStart->setDecimals(3);
    m_pdspVoltStart->setMinimum(0.0);
    m_pdspVoltStart->setMaximum(0.8);
    m_pdspVoltStart->setSingleStep(0.1);
    m_pdspVoltStart->setValue(0.0);

    m_plabVoltEnd = new QLabel("Volt End:", this);
    m_pdspVoltEnd = new QDoubleSpinBox(this);
    m_pdspVoltEnd->setSuffix(" V");
    m_pdspVoltEnd->setDecimals(3);
    m_pdspVoltEnd->setMinimum(0.0);
    m_pdspVoltEnd->setMaximum(0.8);
    m_pdspVoltEnd->setSingleStep(0.1);
    m_pdspVoltEnd->setValue(0.5);

    m_plabVoltStep = new QLabel("Volt Step:", this);
    m_pdspVoltStep = new QDoubleSpinBox(this);
    m_pdspVoltStep->setSuffix(" V");
    m_pdspVoltStep->setDecimals(3);
    m_pdspVoltStep->setMinimum(0.0);
    m_pdspVoltStep->setMaximum(0.8);
    m_pdspVoltStep->setSingleStep(0.1);
    m_pdspVoltStep->setValue(0.01);

    m_plabTimeStep = new QLabel("Time Step:", this);
    m_pdspTimeStep = new QDoubleSpinBox(this);
    m_pdspTimeStep->setSuffix(" s");
    m_pdspTimeStep->setDecimals(2);
    m_pdspTimeStep->setMinimum(0.1);
    m_pdspTimeStep->setMaximum(5000.0);
    m_pdspTimeStep->setSingleStep(1.0);
    m_pdspTimeStep->setValue(1.0);

    m_pbtnPolymerDefault = new QPushButton("Default", this);
    m_pbtnPolymerStart = new QPushButton(QIcon(":/img/img/start.ico"),QStringLiteral("Start Polymer"), this);
    m_pbtnPolymerStart->setCheckable(true);


    QGridLayout* gdlayout1 = new QGridLayout();
    gdlayout1->addWidget(m_plabPoreLimit,0,0,1,1);
    gdlayout1->addWidget(m_plabPoreProtect,0,1,1,1);
    gdlayout1->addWidget(m_plabVoltStart,0,2,1,1);
    gdlayout1->addWidget(m_plabVoltEnd,0,3,1,1);
    gdlayout1->addWidget(m_plabVoltStep,0,4,1,1);
    gdlayout1->addWidget(m_plabTimeStep,0,5,1,1);
    gdlayout1->addWidget(pPolyCurVoltDispLab,0,6,1,1);
    gdlayout1->addWidget(m_pdspPoreLimit,1,0,1,1);
    gdlayout1->addWidget(m_plabPoreProtState,1,1,1,1);
    gdlayout1->addWidget(m_pdspVoltStart,1,2,1,1);
    gdlayout1->addWidget(m_pdspVoltEnd,1,3,1,1);
    gdlayout1->addWidget(m_pdspVoltStep,1,4,1,1);
    gdlayout1->addWidget(m_pdspTimeStep,1,5,1,1);
    gdlayout1->addWidget(m_pdspTimeStep,1,5,1,1);
    gdlayout1->addWidget(m_pPolyCurVoltDispLab,1,6,1,1);
    gdlayout1->addWidget(m_pbtnPolymerDefault,1,7,1,1);
    gdlayout1->addWidget(m_pbtnPolymerStart,1,8,1,1);
    gdlayout1->setColumnStretch(9,20);

    m_pPolyGroupBox->setLayout(gdlayout1);
}


void MembranePanel::OnClickSimulationDefault(void)
{
    m_pSimVoltStartDpbx->setValue(0.0);
    m_pSimVoltEndDpbx->setValue(1.6);
    m_pSimVoltStepDpbx->setValue(0.1);
    m_pSimTimeStepDpbx->setValue(1.0);
    m_pSimCycleSpbx->setValue(30);
    m_pSimCurVoltDispLab->setText(QString::number(0.0,'f',2));
    m_pSimRemainCycleDispLab->setText(QString::number(m_pSimCycleSpbx->value()));
}

void MembranePanel::OnClickSimulationStart(const bool &checked)
{
    bool enb = !checked;
    m_pSimVoltStartDpbx->setEnabled(enb);
    m_pSimVoltEndDpbx->setEnabled(enb);
    m_pSimVoltStepDpbx->setEnabled(enb);
    m_pSimTimeStepDpbx->setEnabled(enb);
    m_pSimCycleSpbx->setEnabled(enb);
    m_pSimRampPatternComb->setEnabled(enb);
    m_pPolyGroupBox->setEnabled(enb);

    m_plabSimProgressDescr->setVisible(true);
    m_pSimProgressbar->setVisible(true);

    if (checked)
    {
         m_pSimStartBtn->setText("Stop");
         m_pSimStartBtn->setIcon(QIcon(":/img/img/close.ico"));

         m_fSimVoltStart = m_pSimVoltStartDpbx->value();
         m_fSimVoltEnd = m_pSimVoltEndDpbx->value();
         m_fSimVoltStep = m_pSimVoltStepDpbx->value();
         m_fSimTimeStep = m_pSimTimeStepDpbx->value();
         m_iSimulationCycle = m_pSimCycleSpbx->value();
         m_eRampPattern = (SIMULATE_RAMP_PATTERN_ENUM)m_pSimRampPatternComb->currentData().toInt();
         m_simSensorGroup = SENSOR_GROUP_NUM;

         m_fSimCurVolt = m_fSimVoltStart;
         emit SensorGroupAndFuncChangeSig(SENSOR_STATE_A_GROUP,SENSORGRP_FUNC_STATE_UNBLOCK);
         emit EnableSimulationSig(true);
         emit SetSimulationVoltSig(m_fSimCurVolt,MEMBRANE_SIMULATION_ELECTROCHEMISTRY);

         m_pSimCurVoltDispLab->setText(QString::number(m_fSimCurVolt,'f',2));
         m_pSimRemainCycleDispLab->setText(QString::number(m_iSimulationCycle));

         int totalCnt = (m_fSimVoltEnd - m_fSimVoltStart)/m_fSimVoltStep * m_iSimulationCycle * m_simSensorGroup;
         totalCnt =(m_eRampPattern == SIMULATE_RAMP_PATTERN_MIRROR ? totalCnt *2 : totalCnt);
         m_pSimProgressbar->setRange(0,totalCnt);
         m_pSimProgressbar->reset();

         m_pTmrSimulation->start(m_fSimTimeStep * 1000);

    }
    else
    {
        emit EnableSimulationSig(false);
        m_pSimStartBtn->setText("Start Simulation");
        m_pSimStartBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pSimProgressbar->reset();


        if (m_pTmrSimulation->isActive())
        {
            m_pTmrSimulation->stop();
        }
    }
}

void MembranePanel::OnTimerSimulationTimeout(void)
{
    if (m_eRampPattern == SIMULATE_RAMP_PATTERN_MIRROR)
    {
        if (m_bSimulRise)
        {
            if (m_fSimCurVolt < m_fSimVoltEnd)
            {
                m_fSimCurVolt += m_fSimVoltStep;
            }
            else
            {
                m_bSimulRise = false;
                m_fSimCurVolt -= m_fSimVoltStep;
            }
        }
        else
        {
            auto dist = m_fSimCurVolt - m_fSimVoltStep;
            if (dist > m_fSimVoltStart || ISDOUBLEZERO((dist - m_fSimVoltStart)))
            {
                m_fSimCurVolt -= m_fSimVoltStep;
            }
            else
            {
                m_bSimulRise = true;
                m_fSimCurVolt += m_fSimVoltStep;
                --m_iSimulationCycle;
                m_pSimRemainCycleDispLab->setText(QString::number(m_iSimulationCycle));
            }
        }
    }
    else if (m_eRampPattern == SIMULATE_RAMP_PATTERN_RISE)
    {
        if (m_fSimCurVolt < m_fSimVoltEnd)
        {
            m_fSimCurVolt += m_fSimVoltStep;
        }
        else
        {
            m_fSimCurVolt = m_fSimVoltStart;
            --m_iSimulationCycle;
            m_pSimRemainCycleDispLab->setText(QString::number(m_iSimulationCycle));
        }
    }
    else if (m_eRampPattern == SIMULATE_RAMP_PATTERN_FALL)
    {
        if (m_fSimCurVolt > m_fSimVoltStart)
        {
            m_fSimCurVolt -= m_fSimVoltStep;
        }
        else
        {
            m_fSimCurVolt = m_fSimVoltEnd;
            --m_iSimulationCycle;
            m_pSimRemainCycleDispLab->setText(QString::number(m_iSimulationCycle));
        }
    }
    else
    {
        //wrong....
        if (m_pTmrSimulation->isActive())
        {
            m_pTmrSimulation->stop();
        }
        return;
    }

    emit SetSimulationVoltSig(m_fSimCurVolt,MEMBRANE_SIMULATION_ELECTROCHEMISTRY);
    m_pSimCurVoltDispLab->setText(QString::number(m_fSimCurVolt,'f',2));

//    int progresscnt = m_pSimProgressbar->value() >= m_pSimProgressbar->maximum() : m_pSimProgressbar->value()
    if(m_pSimProgressbar->value() < m_pSimProgressbar->maximum())
        m_pSimProgressbar->setValue(m_pSimProgressbar->value()+1);
    if (m_iSimulationCycle <= 0)
    {
        if(--m_simSensorGroup <= 0)
        {
            if (m_pSimStartBtn->isChecked())
            {
                m_pSimStartBtn->click();
            }
            m_pSimProgressbar->setValue(m_pSimProgressbar->maximum());
            m_plabSimProgressDescr->setText(QString("ABCD").left(SENSOR_GROUP_NUM- m_simSensorGroup));
            //if (m_timerSimulation.isActive())
            //{
            //    m_timerSimulation.stop();
            //}
        }
        else
        {
            m_fSimCurVolt = m_fSimVoltStart;
            m_iSimulationCycle = m_pSimCycleSpbx->value();
            m_plabSimProgressDescr->setText(QString("ABCD..").left(SENSOR_GROUP_NUM- m_simSensorGroup + 1));
            SENSOR_STATE_GROUP_ENUM sensorGrp =(SENSOR_STATE_GROUP_ENUM) (SENSOR_GROUP_NUM- m_simSensorGroup);
            emit SensorGroupAndFuncChangeSig(sensorGrp,SENSORGRP_FUNC_STATE_UNBLOCK);
            emit EnableSimulationSig(true);     //对应sensor组设置为unblock
        }



    }
}


void MembranePanel::OnClickPolymerDefault(void)
{
    m_pdspPoreLimit->setValue(100.0);
    m_pdspVoltStart->setValue(0.15);
    m_pdspVoltEnd->setValue(0.45);
    m_pdspVoltStep->setValue(0.01);
    m_pdspTimeStep->setValue(1.0);
    m_pPolyCurVoltDispLab->setText(QString::number(0.0,'f',3));
}

void MembranePanel::OnClickPolymerStart(const bool &chk)
{
    bool enb = !chk;
    m_pdspPoreLimit->setEnabled(enb);
    m_plabPoreProtState->setEnabled(enb);
    m_pdspVoltStart->setEnabled(enb);
    m_pdspVoltEnd->setEnabled(enb);
    m_pdspVoltStep->setEnabled(enb);
    m_pdspTimeStep->setEnabled(enb);
    m_pSimulateGrpbx->setEnabled(enb);
    if (chk)
    {
        m_pbtnPolymerStart->setText("Stop");
        m_pbtnPolymerStart->setIcon(QIcon(":/img/img/close.ico"));

        m_dPoreLimit = m_pdspPoreLimit->value();
        m_dVoltStart = m_pdspVoltStart->value();
        m_dVoltEnd = m_pdspVoltEnd->value();
        m_dVoltStep = m_pdspVoltStep->value();
        m_dTimeStep = m_pdspTimeStep->value();

        m_dCurVolt = m_dVoltStart;

        emit EnablePolymerSimulationSig(true);
        emit EnablePoreProtectSig(true,m_dPoreLimit);
        emit SetSimulationVoltSig(m_dCurVolt,MEMBRANE_SIMULATION_POLYMER);
        m_pPolyCurVoltDispLab->setText(QString::number(m_dCurVolt,'f',3));
        m_ptmrPolymer->start(m_dTimeStep * 1000);
    }
    else
    {
        m_pbtnPolymerStart->setText("Start Simulation");
        m_pbtnPolymerStart->setIcon(QIcon(":/img/img/start.ico"));
//        emit EnablePoreProtectSig(false,m_dPoreLimit);//盛文要求无需关闭单孔保护模块
        emit EnablePolymerSimulationSig(false);
        if (m_ptmrPolymer->isActive())
        {
            m_ptmrPolymer->stop();
        }
    }
}

void MembranePanel::OnTimerPolymerSimulateTimeout(void)
{
    if (m_dCurVolt < m_dVoltEnd)
    {
        m_dCurVolt += m_dVoltStep;
    }
    else
    {
        m_dCurVolt = m_dVoltStart;
    }
    emit SetSimulationVoltSig(m_dCurVolt,MEMBRANE_SIMULATION_POLYMER);
    m_pPolyCurVoltDispLab->setText(QString::number(m_dCurVolt,'f',3));
}
