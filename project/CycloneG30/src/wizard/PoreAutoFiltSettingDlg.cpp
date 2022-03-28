#include "PoreAutoFiltSettingDlg.h"
#include <QtWidgets>
#include <QGridLayout>
#include <QSettings>
#include <QTabWidget>
#include <QDialogButtonBox>
#include "CustomComponent.h"



PoreAutoFiltSettingDlg::PoreAutoFiltSettingDlg(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QStringLiteral("PoreAutoFiltSettingDlg"));
    InitCtrl();


}

PoreAutoFiltSettingDlg::~PoreAutoFiltSettingDlg()
{

}


void PoreAutoFiltSettingDlg::InitCtrl(void)
{
    m_pTabWidget = new QTabWidget(this);
    InitPoreFiltLockUnqualifed();
    InitPoreFiltToValid();
    m_pTabWidget->addTab(m_pStepOneGrpBx,QStringLiteral("PoreFiltLockUnqualifed"));
    m_pTabWidget->addTab(m_pStepTwoGrpBx,QStringLiteral("PoreFiltToValid"));

    m_pDialogBtn= new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    connect(m_pDialogBtn, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pDialogBtn, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *pvertlayout = new QVBoxLayout();
    pvertlayout->addWidget(m_pTabWidget);
    pvertlayout->addWidget(m_pDialogBtn);
    setLayout(pvertlayout);
}

void PoreAutoFiltSettingDlg::InitPoreFiltLockUnqualifed(void)
{
    m_pStepOneGrpBx = new QWidget(this);
    m_pDescribeStepOneTitle  = new QLabel(QString::fromLocal8Bit("1.Lock异常通道:\n"\
                                                                 "a)设置直流电压,allow times,进行lock,完成后off \n"\
                                                                 "b)自动切换A/B/C/D"),this);
    QLabel *pOverScanVoLtLab = new QLabel(QStringLiteral("Direct Volt:"),this);
    QLabel *pOverScanTimesLab = new QLabel(QStringLiteral("Allow times:"),this);
    m_pPoreFiltLockVoLtDpbx=  new CustomDoubleSpinbox(0.0,2.5,0.18,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_pPoreFiltLockScanTimesSpbx= new CustomSpinbox(0,60,10,1,QStringLiteral(""),QStringLiteral(" times"),this);


    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(m_pDescribeStepOneTitle,0,0,1,3);
    pgridlayout->addWidget(pOverScanVoLtLab,1,0,1,1);
    pgridlayout->addWidget(m_pPoreFiltLockVoLtDpbx,1,1,1,1);
    pgridlayout->addWidget(pOverScanTimesLab,1,2,1,1);
    pgridlayout->addWidget(m_pPoreFiltLockScanTimesSpbx,1,3,1,1);
    pgridlayout->setColumnStretch(4,10);
    pgridlayout->setRowStretch(2,10);
    m_pStepOneGrpBx->setLayout(pgridlayout);
}

void PoreAutoFiltSettingDlg::InitPoreFiltToValid(void)
{
    m_pStepTwoGrpBx = new QWidget(this);

    m_pDescribeStepTwoTitle = new QLabel(QString::fromLocal8Bit("a)直流电压:0.18V wait : secconds\n"\
                                                                "b)自动切换A/B/C/D\n"\
                                                                "c)自动记录每个sensor的单孔/多孔个数，将单孔transform to valid\n"\
                                                                "d)Mux 自动random\n"\
                                                                "e)Sensor选择mux,记录mux后单孔数，记录10s中点值，期间软件锁定；\n"\
                                                                "记录在mux面板上和csv文件中，完成后off"));

    QLabel *pFiltDirectVoLtLab = new QLabel(QStringLiteral("Direct Volt:"),this);
    QLabel *piltWatiTimeLab = new QLabel(QStringLiteral("Waitting :"),this);
    QLabel *ppFiltSinglePoreStdLab = new QLabel(QStringLiteral("Std:"),this);
    QLabel *ppFiltSinglePoreAvgLab = new QLabel(QStringLiteral("Means:"),this);

    QLabel *ppFiltMultPoreStdLab = new QLabel(QStringLiteral("Std:"),this);
    QLabel *ppFiltMultPoreAvgLab = new QLabel(QStringLiteral("Means:"),this);

    m_pFiltDirectVoLtDpbx= new CustomDoubleSpinbox(0.0,2.5,0.18,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_FiltWatiTimeSpbx= new CustomSpinbox(0,60,8,1,QStringLiteral(""),QStringLiteral(" s"),this);
    m_pFiltSinglePoreStdMinDpbx= new CustomDoubleSpinbox(-50000,50000,0.8,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFiltSinglePoreStdMaxDpbx= new CustomDoubleSpinbox(-50000,50000,10,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFiltSinglePoreAvgMinDpbx= new CustomDoubleSpinbox(-50000,50000,175,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFiltSinglePoreAvgMaxDpbx= new CustomDoubleSpinbox(-50000,50000,320,1,QStringLiteral(""),QStringLiteral(" pA"),this);

    m_pFiltMultPoreStdMinDpbx= new CustomDoubleSpinbox(-50000,50000,0.8,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFiltMultPoreStdMaxDpbx= new CustomDoubleSpinbox(-50000,50000,10,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFiltMultPoreAvgMinDpbx= new CustomDoubleSpinbox(-50000,50000,350,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFiltMultPoreAvgMaxDpbx= new CustomDoubleSpinbox(-50000,50000,1000,1,QStringLiteral(""),QStringLiteral(" pA"),this);


    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(m_pDescribeStepTwoTitle,0,0,1,4);
    pgridlayout->addWidget(pFiltDirectVoLtLab,1,0,1,2);
    pgridlayout->addWidget(m_pFiltDirectVoLtDpbx,1,2,1,1);
    pgridlayout->addWidget(piltWatiTimeLab,2,0,1,2);
    pgridlayout->addWidget(m_FiltWatiTimeSpbx,2,2,1,1);

    pgridlayout->addWidget( new QLabel(QStringLiteral("Single pore:"),this),3,0,1,1);
    pgridlayout->addWidget(ppFiltSinglePoreAvgLab,3,1,1,1);
    pgridlayout->addWidget(m_pFiltSinglePoreAvgMinDpbx,3,2,1,1);
    pgridlayout->addWidget(m_pFiltSinglePoreAvgMaxDpbx,3,3,1,1);
    pgridlayout->addWidget(ppFiltSinglePoreStdLab,4,1,1,1);
    pgridlayout->addWidget(m_pFiltSinglePoreStdMinDpbx,4,2,1,1);
    pgridlayout->addWidget(m_pFiltSinglePoreStdMaxDpbx,4,3,1,1);

    pgridlayout->addWidget( new QLabel(QStringLiteral("Mult pore:"),this),5,0,1,1);
    pgridlayout->addWidget(ppFiltMultPoreAvgLab,5,1,1,1);
    pgridlayout->addWidget(m_pFiltMultPoreAvgMinDpbx,5,2,1,1);
    pgridlayout->addWidget(m_pFiltMultPoreAvgMaxDpbx,5,3,1,1);
    pgridlayout->addWidget(ppFiltMultPoreStdLab,6,1,1,1);
    pgridlayout->addWidget(m_pFiltMultPoreStdMinDpbx,6,2,1,1);
    pgridlayout->addWidget(m_pFiltMultPoreStdMaxDpbx,6,3,1,1);
    pgridlayout->setRowStretch(7,10);
    pgridlayout->setColumnStretch(4,10);
    m_pStepTwoGrpBx->setLayout(pgridlayout);

}


void PoreAutoFiltSettingDlg::accept()
{
    SaveParameter();
    SaveConfig(m_pSettings);
    QDialog::accept();
}




bool PoreAutoFiltSettingDlg::LoadConfig(QSettings* pSetting)
{
    if(pSetting != nullptr)
    {
        m_pSettings = pSetting;
        pSetting->beginGroup("PoreFiltLockUnqualified");
        m_pPoreFiltLockVoLtDpbx->setValue(pSetting->value("PoreFiltLockOverScanVolt").toDouble());
        m_pPoreFiltLockScanTimesSpbx->setValue(pSetting->value("PoreFiltLockOverScanTimes").toInt());
        pSetting->endGroup();

        pSetting->beginGroup("PoreFiltToValid");
        m_pFiltDirectVoLtDpbx->setValue(pSetting->value("PoreFiltToValidVolt").toDouble());
        m_FiltWatiTimeSpbx->setValue(pSetting->value("PoreFiltToValidWattingTime").toInt());
        m_pFiltSinglePoreStdMinDpbx->setValue(pSetting->value("PoreFiltToValidSinglePoreStdMin").toDouble());
        m_pFiltSinglePoreStdMaxDpbx->setValue(pSetting->value("PoreFiltToValidSinglePoreStdMax").toDouble());
        m_pFiltSinglePoreAvgMinDpbx->setValue(pSetting->value("PoreFiltToValidSinglePoreMeanMin").toDouble());
        m_pFiltSinglePoreAvgMaxDpbx->setValue(pSetting->value("PoreFiltToValidSinglePoreMeanMax").toDouble());
        m_pFiltMultPoreStdMinDpbx->setValue(pSetting->value("PoreFiltToValidMultiPoreStdMin").toDouble());
        m_pFiltMultPoreStdMaxDpbx->setValue(pSetting->value("PoreFiltToValidMultiPoreStdMax").toDouble());
        m_pFiltMultPoreAvgMinDpbx->setValue(pSetting->value("PoreFiltToValidMultiPoreMeanMin").toDouble());
        m_pFiltMultPoreAvgMaxDpbx->setValue(pSetting->value("PoreFiltToValidMultiPoreMeanMax").toDouble());
        pSetting->endGroup();
        SaveParameter();
        return true;
    }
    return false;
}
bool PoreAutoFiltSettingDlg::SaveConfig(QSettings* pSetting)
{
    if(pSetting != nullptr)
    {
        pSetting->beginGroup("PoreFiltLockUnqualified");
        pSetting->setValue("PoreFiltLockOverScanVolt",m_pPoreFiltLockVoLtDpbx->value());
        pSetting->setValue("PoreFiltLockOverScanTimes",m_pPoreFiltLockScanTimesSpbx->value());
        pSetting->endGroup();

        pSetting->beginGroup("PoreFiltToValid");
        pSetting->setValue("PoreFiltToValidVolt",m_pFiltDirectVoLtDpbx->value());
        pSetting->setValue("PoreFiltToValidWattingTime",m_FiltWatiTimeSpbx->value());
        pSetting->setValue("PoreFiltToValidSinglePoreStdMin",m_pFiltSinglePoreStdMinDpbx->value());
        pSetting->setValue("PoreFiltToValidSinglePoreStdMax",m_pFiltSinglePoreStdMaxDpbx->value());
        pSetting->setValue("PoreFiltToValidSinglePoreMeanMin",m_pFiltSinglePoreAvgMinDpbx->value());
        pSetting->setValue("PoreFiltToValidSinglePoreMeanMax",m_pFiltSinglePoreAvgMaxDpbx->value());
        pSetting->setValue("PoreFiltToValidMultiPoreStdMin",m_pFiltMultPoreStdMinDpbx->value());
        pSetting->setValue("PoreFiltToValidMultiPoreStdMax",m_pFiltMultPoreStdMaxDpbx->value());
        pSetting->setValue("PoreFiltToValidMultiPoreMeanMin",m_pFiltMultPoreAvgMinDpbx->value());
        pSetting->setValue("PoreFiltToValidMultiPoreMeanMax",m_pFiltMultPoreAvgMaxDpbx->value());
        pSetting->endGroup();
        return true;
    }
    return false;
}

void PoreAutoFiltSettingDlg::SaveParameter(void)
{
    m_SLockUnqualifedParameter.OverScanVolt = m_pPoreFiltLockVoLtDpbx->value();
    m_SLockUnqualifedParameter.OverScanTimes = m_pPoreFiltLockScanTimesSpbx->value();

    m_SPoreFiltToValidParameter.DirectVolt = m_pFiltDirectVoLtDpbx->value();
    m_SPoreFiltToValidParameter.WattingTime = m_FiltWatiTimeSpbx->value();
    m_SPoreFiltToValidParameter.SinglePoreMeanMin = m_pFiltSinglePoreAvgMinDpbx->value();
    m_SPoreFiltToValidParameter.SinglePoreMeanMax = m_pFiltSinglePoreAvgMaxDpbx->value();
    m_SPoreFiltToValidParameter.SinglePoreStdMin = m_pFiltSinglePoreStdMinDpbx->value();
    m_SPoreFiltToValidParameter.SinglePoreStdMax = m_pFiltSinglePoreStdMaxDpbx->value();
    m_SPoreFiltToValidParameter.MultiPoreMeanMin = m_pFiltMultPoreAvgMinDpbx->value();
    m_SPoreFiltToValidParameter.MultiPoreMeanMax = m_pFiltMultPoreAvgMaxDpbx->value();
    m_SPoreFiltToValidParameter.MultiPoreStdMin = m_pFiltMultPoreStdMinDpbx->value();
    m_SPoreFiltToValidParameter.MultiPoreStdMax = m_pFiltMultPoreStdMaxDpbx->value();

}

