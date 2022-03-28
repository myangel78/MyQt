#include "MembraneQCSettingDlg.h"
#include <QtWidgets>
#include <QGridLayout>
#include <QSettings>
#include <QTabWidget>
#include <QDialogButtonBox>
#include "CustomComponent.h"


MembraneQCSettingDlg::MembraneQCSettingDlg(QWidget *parent):QDialog(parent)
{
    InitCtrl();
}
MembraneQCSettingDlg::~MembraneQCSettingDlg()
{

}

void MembraneQCSettingDlg::accept()
{
    SaveParams();
    SaveConfig(m_pSettings);
    QDialog::accept();
}


void MembraneQCSettingDlg::InitCtrl(void)
{
    InitMembOverFlowFilt();
    InitMembOverFlowRecheck();
    InitMembraneQuantityJudge();

    m_pTabWidget = new QTabWidget(this);
    m_pTabWidget->addTab(m_pMembOverFlowFiltWdgt,QStringLiteral("OverFlowFilt"));
    m_pTabWidget->addTab(m_pMembOverFlowRecheckWdgt,QStringLiteral("OverFlowRecheck"));
    m_pTabWidget->addTab(m_pMembraneQuantityJudgeWdgt,QStringLiteral("MembraneQuantity"));

    m_pDialogBtn= new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    connect(m_pDialogBtn, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pDialogBtn, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *pvertlayout = new QVBoxLayout();
    pvertlayout->addWidget(m_pTabWidget);
    pvertlayout->addWidget(m_pDialogBtn);
    setLayout(pvertlayout);

}


void MembraneQCSettingDlg::InitMembOverFlowFilt(void)
{

    m_pMembOverFlowFiltWdgt = new QWidget(this);
    QLabel *pDescriLab = new QLabel(QString::fromLocal8Bit("1.过载通道筛选,永久关断\n"\
                                                           "a)设置直流电压,allow times,进行lock,完成后off \n"\
                                                           "b)自动切换A/B/C/D"),this);
    QLabel *pOverScanVoLtLab = new QLabel(QStringLiteral("Direct Volt:"),this);
    QLabel *pOverScanTimesLab = new QLabel(QStringLiteral("Allow times:"),this);
    m_pOverScanVoLtDpbx=  new CustomDoubleSpinbox(0.0,2.5,0.18,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_pOverScanTimesSpbx= new CustomSpinbox(0,60,10,1,QStringLiteral(""),QStringLiteral(" times"),this);

    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(pDescriLab,0,0,1,4);
    pgridlayout->addWidget(pOverScanVoLtLab,1,0,1,1);
    pgridlayout->addWidget(m_pOverScanVoLtDpbx,1,1,1,1);
    pgridlayout->addWidget(pOverScanTimesLab,1,2,1,1);
    pgridlayout->addWidget(m_pOverScanTimesSpbx,1,3,1,1);
    pgridlayout->setColumnStretch(4,10);
    pgridlayout->setRowStretch(2,10);
    m_pMembOverFlowFiltWdgt->setLayout(pgridlayout);
}
void MembraneQCSettingDlg::InitMembOverFlowRecheck(void)
{
    m_pMembOverFlowRecheckWdgt = new QWidget(this);
    QLabel *pDescriLab = new QLabel(QString::fromLocal8Bit("2.复测过载/漏电通道\n"\
                                                           "a)关断后自动复测是否有过载通道或者漏电通道;如果有，永久关断Lock，完成后off \n"\
                                                           "b)自动切换A/B/C/D\n"),this);
    QLabel *pOverRecheckVoLtLab = new QLabel(QStringLiteral("Direct Volt:"),this);
    QLabel *pOverRecheckTimeLab = new QLabel(QStringLiteral("Durations:"),this);
    QLabel *pOverRecheckStdLab  = new QLabel(QStringLiteral("Currnet Std: >"),this);
    QLabel *pOverRecheckMeanLab = new QLabel(QStringLiteral("OR Mean: >"),this);
    m_pOverRecheckVoLtDpbx= new CustomDoubleSpinbox(0.0,2.5,0.18,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_pOverRecheckTimeSpbx= new CustomSpinbox(0,60,5,1,QStringLiteral(""),QStringLiteral(" s"),this);
    m_pOverRecheckStdDpbx= new CustomDoubleSpinbox(-50000,50000,30,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pOverRecheckMeanDpbx= new CustomDoubleSpinbox(-50000,50000,100,1,QStringLiteral(""),QStringLiteral(" pA"),this);


    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(pDescriLab,0,0,1,4);
    pgridlayout->addWidget(pOverRecheckVoLtLab,1,0,1,1);
    pgridlayout->addWidget(m_pOverRecheckVoLtDpbx,1,1,1,1);
    pgridlayout->addWidget(pOverRecheckTimeLab,1,2,1,1);
    pgridlayout->addWidget(m_pOverRecheckTimeSpbx,1,3,1,1);
    pgridlayout->addWidget(pOverRecheckStdLab,2,0,1,1);
    pgridlayout->addWidget(m_pOverRecheckStdDpbx,2,1,1,1);
    pgridlayout->addWidget(pOverRecheckMeanLab,2,2,1,1);
    pgridlayout->addWidget(m_pOverRecheckMeanDpbx,2,3,1,1);
    pgridlayout->setColumnStretch(4,10);
    pgridlayout->setRowStretch(3,10);
    m_pMembOverFlowRecheckWdgt->setLayout(pgridlayout);

}

void MembraneQCSettingDlg::InitMembraneQuantityJudge(void)
{
    m_pMembraneQuantityJudgeWdgt = new QWidget(this);
    QLabel *pDescriLab = new QLabel(QString::fromLocal8Bit("3.成膜数量判断\n"\
                                                           "a)交流电压,根据阈值范围划分电容;\n"\
                                                           "b)自动切换A/B/C/D \n"\
                                                           "c)自动计数每个sensor\n"\
                                                           "d)电容大于level3的通道Lock,完成后off"),this);
    QLabel *pQuantityACVoltLab = new QLabel(QStringLiteral("AC Volt:"),this);
    QLabel *pQuantityACTimeLab = new QLabel(QStringLiteral("Durations:"),this);
    QLabel *pQuantityCapLevel1Lab = new QLabel(QStringLiteral("Level1:"),this);
    QLabel *pQuantityCapLevel2Lab = new QLabel(QStringLiteral("Level2:"),this);
    QLabel *pQuantityCapLevel3Lab = new QLabel(QStringLiteral("Level3:"),this);
    QLabel *pQuantityCapLevel4Lab = new QLabel(QStringLiteral("Level4:"),this);

    m_pQuantityACVoltDpbx=  new CustomDoubleSpinbox(0.0,2.5,0.05,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_pQuantityACTimeSpbx= new CustomSpinbox(0,60,5,1,QStringLiteral(""),QStringLiteral(" s"),this);
    m_pQuantityCapLevel1Dpbx=  new CustomDoubleSpinbox(0.0,2000,15,1,QStringLiteral(""),QStringLiteral(" pF"),this);
    m_pQuantityCapLevel2Dpbx=  new CustomDoubleSpinbox(0.0,2000,15,1,QStringLiteral(""),QStringLiteral(" pF"),this);
    m_pQuantityCapLevel3Dpbx=  new CustomDoubleSpinbox(0.0,2000,40,1,QStringLiteral(""),QStringLiteral(" pF"),this);
    m_pQuantityCapLevel4Dpbx=  new CustomDoubleSpinbox(0.0,2000,100,1,QStringLiteral(""),QStringLiteral(" pF"),this);

    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(pDescriLab,0,0,1,4);
    pgridlayout->addWidget(pQuantityACVoltLab,1,0,1,1);
    pgridlayout->addWidget(m_pQuantityACVoltDpbx,1,1,1,1);
    pgridlayout->addWidget(pQuantityACTimeLab,2,0,1,1);
    pgridlayout->addWidget(m_pQuantityACTimeSpbx,2,1,1,1);
    pgridlayout->addWidget(pQuantityCapLevel1Lab,3,0,1,1);
    pgridlayout->addWidget(m_pQuantityCapLevel1Dpbx,3,1,1,1);
    pgridlayout->addWidget(pQuantityCapLevel2Lab,4,0,1,1);
    pgridlayout->addWidget(m_pQuantityCapLevel2Dpbx,4,1,1,1);

    pgridlayout->addWidget(pQuantityCapLevel3Lab,5,0,1,1);
    pgridlayout->addWidget(m_pQuantityCapLevel3Dpbx,5,1,1,1);
    pgridlayout->addWidget(pQuantityCapLevel4Lab,6,0,1,1);
    pgridlayout->addWidget(m_pQuantityCapLevel4Dpbx,6,1,1,1);
    pgridlayout->setColumnStretch(4,10);
    pgridlayout->setRowStretch(7,10);
    m_pMembraneQuantityJudgeWdgt->setLayout(pgridlayout);

}


bool MembraneQCSettingDlg::SaveConfig(QSettings* pSetting)
{
    if(pSetting != nullptr)
    {
        pSetting->beginGroup("OverFlowChannelFilt");
        pSetting->setValue("MembSToneOVFScanVolt",m_pOverScanVoLtDpbx->value());
        pSetting->setValue("MembSToneOVFScanTimes",m_pOverScanTimesSpbx->value());
        pSetting->endGroup();

        pSetting->beginGroup("MembOverFlowRecheck");
        pSetting->setValue("MembSTTwoRecheckVolt",m_pOverRecheckVoLtDpbx->value());
        pSetting->setValue("MembSTTwoRecheckTimes",m_pOverRecheckTimeSpbx->value());
        pSetting->setValue("MembSTTwoRecheckStd",m_pOverRecheckStdDpbx->value());
        pSetting->setValue("MembSTTwoRecheckMeans",m_pOverRecheckMeanDpbx->value());
        pSetting->endGroup();

        pSetting->beginGroup("MembraneQuantityJudge");
        pSetting->setValue("MembSTThdMemJudgACVolt",m_pQuantityACVoltDpbx->value());
        pSetting->setValue("MembSTThdMemJudgACTime",m_pQuantityACTimeSpbx->value());
        pSetting->setValue("MembSTThdMemJudgCapLevel1",m_pQuantityCapLevel1Dpbx->value());
        pSetting->setValue("MembSTThdMemJudgCapLevel2",m_pQuantityCapLevel2Dpbx->value());
        pSetting->setValue("MembSTThdMemJudgCapLevel3",m_pQuantityCapLevel3Dpbx->value());
        pSetting->setValue("MembSTThdMemJudgCapLevel4",m_pQuantityCapLevel4Dpbx->value());
        pSetting->endGroup();
        return true;
    }
    return false;
}

bool MembraneQCSettingDlg::LoadConfig(QSettings* pSetting)
{
    if(pSetting != nullptr)
    {
        m_pSettings = pSetting;
        pSetting->beginGroup("OverFlowChannelFilt");
        m_pOverScanVoLtDpbx->setValue(pSetting->value("MembSToneOVFScanVolt").toDouble());
        m_pOverScanTimesSpbx->setValue(pSetting->value("MembSToneOVFScanTimes").toInt());
        pSetting->endGroup();

        pSetting->beginGroup("MembOverFlowRecheck");
        m_pOverRecheckVoLtDpbx->setValue(pSetting->value("MembSTTwoRecheckVolt").toDouble());
        m_pOverRecheckTimeSpbx->setValue(pSetting->value("MembSTTwoRecheckTimes").toInt());
        m_pOverRecheckStdDpbx->setValue(pSetting->value("MembSTTwoRecheckStd").toDouble());
        m_pOverRecheckMeanDpbx->setValue(pSetting->value("MembSTTwoRecheckMeans").toDouble());
        pSetting->endGroup();

        pSetting->beginGroup("MembraneQuantityJudge");
        m_pQuantityACVoltDpbx->setValue(pSetting->value("MembSTThdMemJudgACVolt").toDouble());
        m_pQuantityACTimeSpbx->setValue(pSetting->value("MembSTThdMemJudgACTime").toInt());
        m_pQuantityCapLevel1Dpbx->setValue(pSetting->value("MembSTThdMemJudgCapLevel1").toDouble());
        m_pQuantityCapLevel2Dpbx->setValue(pSetting->value("MembSTThdMemJudgCapLevel2").toDouble());
        m_pQuantityCapLevel3Dpbx->setValue(pSetting->value("MembSTThdMemJudgCapLevel3").toDouble());
        m_pQuantityCapLevel4Dpbx->setValue(pSetting->value("MembSTThdMemJudgCapLevel4").toDouble());
        pSetting->endGroup();
        SaveParams();
        return true;
    }
    return false;
}

void MembraneQCSettingDlg::SaveParams(void)
{

    m_SOverFlowFiltParams.overScanVolt = m_pOverScanVoLtDpbx->value();
    m_SOverFlowFiltParams.overScanTimes = m_pOverScanTimesSpbx->value();

    m_SOverFlowRecheckParams.directVolt = m_pOverRecheckVoLtDpbx->value();
    m_SOverFlowRecheckParams.wattingTime = m_pOverRecheckTimeSpbx->value();
    m_SOverFlowRecheckParams.overRecheckStd = m_pOverRecheckStdDpbx->value();
    m_SOverFlowRecheckParams.overRecheckMean = m_pOverRecheckMeanDpbx->value();

    m_SMembQuantityJudgeParams.acVolt = m_pQuantityACVoltDpbx->value();
    m_SMembQuantityJudgeParams.wattingTime = m_pQuantityACTimeSpbx->value();
    m_SMembQuantityJudgeParams.capLevel1 = m_pQuantityCapLevel1Dpbx->value();
    m_SMembQuantityJudgeParams.capLevel2 = m_pQuantityCapLevel2Dpbx->value();
    m_SMembQuantityJudgeParams.capLevel3 = m_pQuantityCapLevel3Dpbx->value();
    m_SMembQuantityJudgeParams.capLevel4 = m_pQuantityCapLevel4Dpbx->value();
}

