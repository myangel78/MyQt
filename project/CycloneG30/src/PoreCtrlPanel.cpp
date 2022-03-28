#include "PoreCtrlPanel.h"
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
#include <QDoubleSpinBox>
#include "Global.h"
#include "Log.h"
#include "UsbFtdDevice.h"


PoreCtrlPanel::PoreCtrlPanel(QWidget *parent):QWidget(parent)
{
    InitCtrl();
}

PoreCtrlPanel::~PoreCtrlPanel()
{

}

void PoreCtrlPanel::InitCtrl(void)
{
    m_poreInsertGrpbx = new QGroupBox(QStringLiteral("Pore insert"),this);
    QGroupBox *groupStdbox = new QGroupBox(QStringLiteral("Std Filter"),this);

    QLabel *pSinglePoreInpLab = new QLabel(QStringLiteral("Single pore:"),this);
    QLabel *pMultPoreInpLab = new QLabel(QStringLiteral("Multi pore:"),this);
    QLabel *pMembrokInpLab = new QLabel(QStringLiteral("Membrane brokent:"),this);
    m_pSinglePoreInpDpbx = new QDoubleSpinBox(this);
    m_pMultPoreInpDpbx = new QDoubleSpinBox(this);
    m_pMembrokenInpDpbx = new QDoubleSpinBox(this);
    m_pIsPoreLockChkBox  = new QCheckBox(QStringLiteral("PoreLock"),this);
    m_pIsPoreLockChkBox->setChecked(false);
    m_pStartPoreInsertBtn = new QPushButton(QIcon(":/img/img/start.ico"),"Start",this);
    m_pStartPoreInsertBtn->setCheckable(true);

    QLabel *pStdDescrLab = new QLabel(QStringLiteral("Std :"),this);
    QLabel *pStdMinInpLab = new QLabel(QStringLiteral("Min:"),this);
    QLabel *pStdMaxInpLab = new QLabel(QStringLiteral("Max:"),this);
    QLabel *pToopTipsLab = new QLabel(QStringLiteral("Only shut off the std overflow the range!!! "),this);
    m_pStdMinDpbx= new QDoubleSpinBox(this);
    m_pStdMaxDpbx= new QDoubleSpinBox(this);
    m_pStdFiltStartBtn= new QPushButton(QIcon(":/img/img/start.ico"),"Start",this);
    m_pStdFiltStartBtn->setCheckable(true);

    m_pMembrProtectGrpbx= new QGroupBox(QStringLiteral("Membrane broken protect"),this);
    QLabel *pMembrokProtectInpLab = new QLabel(QStringLiteral("Membrane brokent:"),this);
    m_pMembrokeProtectInpDpbx = new QDoubleSpinBox(this);
    m_pStartMembraneProtectBtn = new QPushButton(QIcon(":/img/img/start.ico"),"Start",this);
    m_pStartMembraneProtectBtn->setCheckable(true);
    m_pTmrPoreInsert = new QTimer(this);
    m_pTimerCurStd = new QTimer(this);
    m_pTimerMembrokeProtect = new QTimer(this);

    m_pSinglePoreInpDpbx->setRange(0,10000);
    m_pMultPoreInpDpbx->setRange(0,10000);
    m_pMembrokenInpDpbx->setRange(0,10000);
    m_pMembrokeProtectInpDpbx->setRange(0,10000);
    m_pSinglePoreInpDpbx->setSuffix(" pA");
    m_pMultPoreInpDpbx->setSuffix(" pA");
    m_pMembrokenInpDpbx->setSuffix(" pA");
    m_pMembrokeProtectInpDpbx->setSuffix(" pA");
    m_pSinglePoreInpDpbx->setValue(120);
    m_pMultPoreInpDpbx->setValue(300);
    m_pMembrokenInpDpbx->setValue(600);
    m_pMembrokeProtectInpDpbx->setValue(550);

    m_pStdMinDpbx->setRange(0,10000.0);
    m_pStdMaxDpbx->setRange(0,10000.0);
    m_pStdMinDpbx->setSuffix(" pA");
    m_pStdMaxDpbx->setSuffix(" pA");
    m_pStdMinDpbx->setValue(0);
    m_pStdMaxDpbx->setValue(100);
    m_pStdMinDpbx->setDecimals(3);
    m_pStdMaxDpbx->setDecimals(3);

    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->addWidget(pSinglePoreInpLab,0,0,1,1);
    gridlayout->addWidget(m_pSinglePoreInpDpbx,1,0,1,1);
    gridlayout->addWidget(pMultPoreInpLab,0,1,1,1);
    gridlayout->addWidget(m_pMultPoreInpDpbx,1,1,1,1);
    gridlayout->addWidget(pMembrokInpLab,0,2,1,1);
    gridlayout->addWidget(m_pMembrokenInpDpbx,1,2,1,1);
    gridlayout->addWidget(m_pIsPoreLockChkBox,1,3,1,1);
    gridlayout->addWidget(m_pStartPoreInsertBtn,1,4,1,1);
    gridlayout->setColumnStretch(5,10);
    m_poreInsertGrpbx->setLayout(gridlayout);

    QGridLayout *gridlayou1 = new QGridLayout();
    gridlayou1->addWidget(pStdDescrLab,1,0,1,1);
    gridlayou1->addWidget(pStdMinInpLab,0,1,1,1);
    gridlayou1->addWidget(m_pStdMinDpbx,1,1,1,1);
    gridlayou1->addWidget(pStdMaxInpLab,0,2,1,1);
    gridlayou1->addWidget(m_pStdMaxDpbx,1,2,1,1);
    gridlayou1->addWidget(m_pStdFiltStartBtn,1,3,1,1);
    gridlayou1->addWidget(pToopTipsLab,1,4,1,1);
    gridlayou1->setColumnStretch(5,10);
    groupStdbox->setLayout(gridlayou1);

    QGridLayout *gridlayou2 = new QGridLayout();
    gridlayou2->addWidget(pMembrokProtectInpLab,0,0,1,1);
    gridlayou2->addWidget(m_pMembrokeProtectInpDpbx,1,0,1,1);
    gridlayou2->addWidget(m_pStartMembraneProtectBtn,1,1,1,1);
    m_pMembrProtectGrpbx->setLayout(gridlayou2);

    QHBoxLayout *horilayout = new QHBoxLayout();
    horilayout->addWidget(m_poreInsertGrpbx);
    horilayout->addWidget(m_pMembrProtectGrpbx);
    horilayout->addStretch();
    QVBoxLayout *vertlaout = new QVBoxLayout();
    vertlaout->addLayout(horilayout);
    vertlaout->addWidget(groupStdbox);
    vertlaout->addStretch();
    setLayout(vertlaout);

    connect(m_pStartPoreInsertBtn,&QPushButton::toggled,this,&PoreCtrlPanel::StartPoreInsetBtnSlot);
    connect(m_pStdFiltStartBtn,&QPushButton::toggled,this,&PoreCtrlPanel::StartStdFiltBtnSlot);
    connect(m_pStartMembraneProtectBtn,&QPushButton::toggled,this,&PoreCtrlPanel::StartMembBrkoProtectBtnSlot);

    connect(m_pTmrPoreInsert,&QTimer::timeout,this,&PoreCtrlPanel::TimeupToCalCurForPoreInsert);
    connect(m_pTimerCurStd,&QTimer::timeout,this,&PoreCtrlPanel::CalcStdTimeoutSlot);
    connect(m_pTimerMembrokeProtect,&QTimer::timeout,this,&PoreCtrlPanel::TimeUpCalAvgForMembProtctSlot);


}

void PoreCtrlPanel::StartMembBrkoProtectBtnSlot(bool checked)
{
    if(checked)
    {
        if(m_pStartPoreInsertBtn->isChecked())
        {
            m_pStartPoreInsertBtn->setChecked(false);
        }
        m_pStartMembraneProtectBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pStartMembraneProtectBtn->setText("Stop");
        emit SigUpdateSensorState();
        if(!m_pTimerMembrokeProtect->isActive())
        {
            m_pTimerMembrokeProtect->start(1000);
        }
    }
    else
    {
        m_pStartMembraneProtectBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pStartMembraneProtectBtn->setText("Start");
        if(m_pTimerMembrokeProtect->isActive())
        {
            m_pTimerMembrokeProtect->stop();
        }
    }
    m_pMembrokeProtectInpDpbx->setEnabled(!checked);
}
void PoreCtrlPanel::StartPoreInsetBtnSlot(bool checked)
{
    if(checked)
    {
        if(m_pStartMembraneProtectBtn->isChecked())
        {
            m_pStartMembraneProtectBtn->setChecked(false);
        }
        m_pStartPoreInsertBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pStartPoreInsertBtn->setText("Stop");
        ConfigServer::GetInstance()->SetAverCurrentCalc(true);
        emit SigUpdateSensorState();
        if(!m_pTmrPoreInsert->isActive())
        {
            m_pTmrPoreInsert->start(250);
        }
    }
    else
    {
        m_pStartPoreInsertBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pStartPoreInsertBtn->setText("Start");
        ConfigServer::GetInstance()->SetAverCurrentCalc(false);
        if(m_pTmrPoreInsert->isActive())
        {
            m_pTmrPoreInsert->stop();
        }
    }
    m_pSinglePoreInpDpbx->setEnabled(!checked);
    m_pMultPoreInpDpbx->setEnabled(!checked);
    m_pMembrokenInpDpbx->setEnabled(!checked);
}

void PoreCtrlPanel::SlotGetPoreLimitCurrInp(float &single,float &mulit,float &membrk)
{
    single = m_pSinglePoreInpDpbx->value();
    mulit = m_pMultPoreInpDpbx->value();
    membrk = m_pMembrokenInpDpbx->value();
}

void PoreCtrlPanel::SlotGetMembraneLimitCurrInp(float &membrk)
{
    membrk = m_pMembrokeProtectInpDpbx->value();
}

void PoreCtrlPanel::StartStdFiltBtnSlot(bool checked)
{
    if(checked)
    {
        m_pStdFiltStartBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pStdFiltStartBtn->setText("Stop");
        ConfigServer::GetInstance()->SetStdCurrentCalc(true);
        emit SigUpdateSensorState();
        if(!m_pTimerCurStd->isActive())
        {
            m_pTimerCurStd->start(1000);
        }
    }
    else
    {
        m_pStdFiltStartBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pStdFiltStartBtn->setText("Start");
        ConfigServer::GetInstance()->SetStdCurrentCalc(false);
        if(m_pTimerCurStd->isActive())
        {
            m_pTimerCurStd->stop();
        }
    }
    m_pStdMinDpbx->setEnabled(!checked);
    m_pStdMaxDpbx->setEnabled(!checked);
}

void PoreCtrlPanel::SlotGetStdLimitInput(float &stdMin,float &stdMax)
{
    stdMin = m_pStdMinDpbx->value();
    stdMax = m_pStdMaxDpbx->value();
}

void PoreCtrlPanel::TimeupToCalCurForPoreInsert(void)
{
    emit SigCalCurForPoreInsert();
}

void PoreCtrlPanel::TimeUpCalAvgForMembProtctSlot(void)
{
    emit SigCalCurForMembrokProtect();
}


void PoreCtrlPanel::CalcStdTimeoutSlot(void)
{
     emit SigCalcCurStdForShut();
}


void PoreCtrlPanel::EnablePoreProtectSlot(const bool &enable,const float &poreLimtCurrent)
{
    if(enable)
    {
        m_pSinglePoreInpDpbx->setValue(poreLimtCurrent);
        if(!m_pStartPoreInsertBtn->isChecked())
        {
            m_pStartPoreInsertBtn->click();
        }
    }
    else
    {
        if(m_pStartPoreInsertBtn->isChecked())
        {
            m_pStartPoreInsertBtn->click();
        }
    }
}
void PoreCtrlPanel::EnableMembrokProtectSlot(const bool &enable,const float &membrokLimtCurrent)
{
    if(enable)
    {
        m_pMembrokeProtectInpDpbx->setValue(membrokLimtCurrent);
        if(!m_pStartMembraneProtectBtn->isChecked())
        {
            m_pStartMembraneProtectBtn->click();
        }
    }
    else
    {
        if(m_pStartMembraneProtectBtn->isChecked())
        {
            m_pStartMembraneProtectBtn->click();
        }
    }
}
void PoreCtrlPanel::AcquireWhetherPoreLockSlot(bool &bPoreLock)
{
    bPoreLock = m_pIsPoreLockChkBox->isChecked();
}
