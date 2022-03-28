#include "AutoSequenceSettingDlg.h"
#include <QtWidgets>
#include <QGridLayout>
#include <QSettings>
#include <QTabWidget>
#include <QDialogButtonBox>
#include "CustomComponent.h"


AutoSequenceSettingDlg::AutoSequenceSettingDlg(QWidget *parent):QDialog(parent)
{
    InitCtrl();
}
AutoSequenceSettingDlg::~AutoSequenceSettingDlg()
{
}

void AutoSequenceSettingDlg::accept()
{
    SaveConfig(m_pSettings);
    SaveParams();
    QDialog::accept();
}


void AutoSequenceSettingDlg::InitCtrl(void)
{
    InitLockCtrl();
    InitValidCtrl();
    InitDegateCtrl();
    InitOtherCtrl();

    m_pTabWidget = new QTabWidget(this);
    m_pTabWidget->addTab(m_pLockScanWdgt,QStringLiteral("LockScan"));
    m_pTabWidget->addTab(m_pValidFiltWdgt,QStringLiteral("ValidFilt"));
    m_pTabWidget->addTab(m_pOtherCtrlWdgt,QStringLiteral("Other"));
    m_pTabWidget->addTab(m_pDegateCtrlWdgt,QStringLiteral("Degate"));

    m_pDialogBtn= new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    connect(m_pDialogBtn, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_pDialogBtn, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *pvertlayout = new QVBoxLayout();
    pvertlayout->addWidget(m_pTabWidget);
    pvertlayout->addWidget(m_pDialogBtn);
    setLayout(pvertlayout);

}

void AutoSequenceSettingDlg::InitLockCtrl(void)
{
    m_pLockScanWdgt = new QWidget(this);
    QLabel *pDescriLab = new QLabel(QString::fromLocal8Bit("1.Lock 异常通道\n"\
                                                           "a)设置直流电压,allow times,进行lock,完成后off \n"\
                                                           "b)自动切换A/B/C/D"),this);
    QLabel *pOverScanDcVoLtLab = new QLabel(QStringLiteral("Direct Volt:"),this);
    QLabel *pOverScanAcVoLtLab = new QLabel(QStringLiteral("Ac Volt:"),this);
    QLabel *pOverScanTimesLab = new QLabel(QStringLiteral("Allow times:"),this);
    m_pOverScanDcVoLtDpbx=  new CustomDoubleSpinbox(0.0,2.5,0.18,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_pOverScanAcVoLtDpbx=  new CustomDoubleSpinbox(0.0,2.5,0.05,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_pOverScanTimesSpbx= new CustomSpinbox(0,60,10,1,QStringLiteral(""),QStringLiteral(" times"),this);

    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(pDescriLab,0,0,1,4);
    pgridlayout->addWidget(pOverScanDcVoLtLab,1,0,1,1);
    pgridlayout->addWidget(m_pOverScanDcVoLtDpbx,1,1,1,1);
    pgridlayout->addWidget(pOverScanAcVoLtLab,2,0,1,1);
    pgridlayout->addWidget(m_pOverScanAcVoLtDpbx,2,1,1,1);
    pgridlayout->addWidget(pOverScanTimesLab,3,0,1,1);
    pgridlayout->addWidget(m_pOverScanTimesSpbx,3,1,1,1);
    pgridlayout->setColumnStretch(2,10);
    pgridlayout->setRowStretch(4,10);
    m_pLockScanWdgt->setLayout(pgridlayout);

}
void AutoSequenceSettingDlg::InitValidCtrl(void)
{
   m_pValidFiltWdgt = new QWidget(this);
   QLabel *pDescriLab = new QLabel(QString::fromLocal8Bit("2.ValidQC\n"\
                                                          "a)设置直流电压,电流范围以及相关参数,不符合则设置off \n"\
                                                          "b)自动切换A/B/C/D\n"\
                                                          "c)记录每个sensor的valid值\n"\
                                                          "d)自动random,非valid则off,施加测序电压"),this);
    QLabel *pShuotffDcVoltlab = new QLabel(QStringLiteral("Dc Volt:"),this);
    QLabel *m_pFilterCurlab = new QLabel(QStringLiteral("Current:"),this);
    QLabel *m_pFilterStdlab = new QLabel(QStringLiteral("Std:"),this);
    QLabel *m_pFilterWaitSeclab = new QLabel(QStringLiteral("Wait:"),this);
    m_pShuotffDcVoltDpbx=  new CustomDoubleSpinbox(0.0,2.5,0.18,0.1,QStringLiteral(""),QStringLiteral(" V"),this);
    m_pFilterCurMinDpbx=  new CustomDoubleSpinbox(-50000,50000,65,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFilterCurMaxDpbx=  new CustomDoubleSpinbox(-50000,50000,155,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFilterStdMinDpbx=  new CustomDoubleSpinbox(-50000,50000,9,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pFilterStdMaxDpbx=  new CustomDoubleSpinbox(-50000,50000,30,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_PFilterWaitSecSpbx=  new CustomSpinbox(0,60,6,1,QStringLiteral(""),QStringLiteral(" s"),this);
    m_pValidFilterTimesSpbx=  new CustomSpinbox(0,60,2,1,QStringLiteral(""),QStringLiteral(" times"),this);

    m_pFilterNeedObersveChk= new QCheckBox(QStringLiteral("Observe watting:"),this);
    m_pFilterObserveDurationDpbx= new CustomSpinbox(1,60,10,1,QStringLiteral(""),QStringLiteral(" s"),this);
    m_pFilterObserveDurationDpbx->setEnabled(false);


    QGridLayout *pvalidgridlayout  = new QGridLayout();
    pvalidgridlayout->addWidget(pDescriLab,0,0,1,3);
    pvalidgridlayout->addWidget(pShuotffDcVoltlab,1,0,1,1);
    pvalidgridlayout->addWidget(m_pShuotffDcVoltDpbx,1,1,1,1);
    pvalidgridlayout->addWidget(m_pFilterCurlab,3,0,1,1);
    pvalidgridlayout->addWidget(m_pFilterCurMinDpbx,3,1,1,1);
    pvalidgridlayout->addWidget(m_pFilterCurMaxDpbx,3,2,1,1);
    pvalidgridlayout->addWidget(m_pFilterStdlab,4,0,1,1);
    pvalidgridlayout->addWidget(m_pFilterStdMinDpbx,4,1,1,1);
    pvalidgridlayout->addWidget(m_pFilterStdMaxDpbx,4,2,1,1);
    pvalidgridlayout->addWidget(m_pFilterWaitSeclab,5,0,1,1);
    pvalidgridlayout->addWidget(m_PFilterWaitSecSpbx,5,1,1,1);
    pvalidgridlayout->addWidget(m_pValidFilterTimesSpbx,5,2,1,1);
    pvalidgridlayout->addWidget(m_pFilterNeedObersveChk,6,0,1,1);
    pvalidgridlayout->addWidget(m_pFilterObserveDurationDpbx,6,1,1,1);
    pvalidgridlayout->setRowStretch(7,10);
    m_pValidFiltWdgt->setLayout(pvalidgridlayout);

    connect(m_pFilterNeedObersveChk,&QCheckBox::toggled,m_pFilterObserveDurationDpbx,&CustomDoubleSpinbox::setEnabled);
}

void AutoSequenceSettingDlg::InitDegateCtrl(void)
{
    m_pDegateCtrlWdgt = new QWidget(this);
    m_pgrpDegatingBase = new QGroupBox("Degating Base", this);
    m_pDegatParmGrpBx = new QGroupBox(QStringLiteral("Degating Advanced"),this);

    m_pgrpDegatingBase->setContentsMargins(3, 1, 3, 0);
    m_plabCyclePeriod = new QLabel("Cycle Period:", this);
    m_pispCyclePeriod = new CustomSpinbox(0,99,2,1,QStringLiteral(""),QStringLiteral(" s"),this);
    m_pispCyclePeriod->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置多久（几秒）检测一次gating状态</p></body></html>"));

    m_plabDurationTime = new QLabel("Duration Time:", this);
    m_pispDurationTime = new CustomSpinbox(100,10000,200,100,QStringLiteral(""),QStringLiteral(" ms"),this);
    m_pispDurationTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置施加Degating电压持续时间</p></body></html>"));


    m_plabAllowTimes = new QLabel("Allow Times:", this);
    m_pispAllowTimes = new CustomSpinbox(0,100,10,1,QStringLiteral(""),QStringLiteral(" times"),this);
    m_pispAllowTimes->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间内的Degating次数，规定时间内达到此次数则关断该通道。0为表示此功能不开启</p></body></html>"));

    m_pchkAutoseqGating = new QCheckBox("Autoseq", this);
    m_pchkAutoseqGating->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否启用AutoseqGating算法</p></body></html>"));

    m_plabDurationThres = new QLabel("Durate Thres:", this);
    m_pdspDurationThres = new CustomDoubleSpinbox(0.2,1,0.1,0.1,QStringLiteral(""),QStringLiteral(""),this);
    m_pdspDurationThres->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>最大值不能超过1，如果有某段信号0.5s一致维持在同一个水平不变，那就认为是gating状态</p></body></html>"));

    m_plabGatingSTD = new QLabel("Seq STD Min:", this);
    m_pispGatingSTD = new CustomSpinbox(0,100,8,1,QStringLiteral(""),QStringLiteral(""),this);
    m_pispGatingSTD->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>小于该STD min值，则认为是gating状态</p></body></html>"));

    m_plabGatingSTDMax = new QLabel("Seq STD Max:", this);
    m_pispGatingSTDMax = new CustomSpinbox(0,50000,18,1,QStringLiteral(""),QStringLiteral(""),this);
    m_pispGatingSTDMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>大于该STD max值，则认为是gating状态</p></body></html>"));

    m_plabSignalMin = new QLabel("Signal Min:", this);
    m_pispSignalMin = new CustomSpinbox(-200,500,1,1,QStringLiteral(""),QStringLiteral(""),this);
    m_pispSignalMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，小于该值为非gating,大于此值才进一步作gating分析</p></body></html>"));

    m_plabSignalMax = new QLabel("Signal Max:", this);
    m_pispSignalMax = new CustomSpinbox(0,500,300,1,QStringLiteral(""),QStringLiteral(""),this);
    m_pispSignalMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，大于该值为非gating,小于此值才进一步作gating分析</p></body></html>"));

    m_plabGatingMin = new QLabel("Gating Min:", this);
    m_pispGatingMin = new CustomSpinbox(-20,500,1,1,QStringLiteral(""),QStringLiteral(""),this);
    m_pispGatingMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，大于此值作gating分析</p></body></html>"));

    m_plabGatingMax = new QLabel("Gating Max:", this);
    m_pispGatingMax = new CustomSpinbox(0,500,180,1,QStringLiteral(""),QStringLiteral(""),this);
    m_pispGatingMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，小于此值作gating分析</p></body></html>"));

    int row = 0;
    int col = 0;
    QGridLayout* gdlayout1 = new QGridLayout();
    gdlayout1->setContentsMargins(1, 8, 1, 1);
    col = 0;
    gdlayout1->addWidget(m_plabCyclePeriod, row, col++);
    gdlayout1->addWidget(m_pispCyclePeriod, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabDurationTime, row, col++);
    gdlayout1->addWidget(m_pispDurationTime, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_plabAllowTimes, row, col++);
    gdlayout1->addWidget(m_pispAllowTimes, row, col++);
    ++row;
    col = 0;
    gdlayout1->addWidget(m_pchkAutoseqGating, row, col++);
    m_pgrpDegatingBase->setLayout(gdlayout1);

    int cnt = 0;
    QGridLayout* gdlayout2 = new QGridLayout();
    gdlayout2->setContentsMargins(1, 8, 1, 1);
    gdlayout2->addWidget(m_plabDurationThres, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTD, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTDMax, cnt++, 0);
    gdlayout2->addWidget(m_plabSignalMin, cnt++, 0);
    cnt = 0;
    gdlayout2->addWidget(m_pdspDurationThres, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTD, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTDMax, cnt++, 1);
    gdlayout2->addWidget(m_pispSignalMin, cnt++, 1);
    cnt = 0;
    gdlayout2->addWidget(m_plabSignalMax, cnt++, 2);
    gdlayout2->addWidget(m_plabGatingMin, cnt++, 2);
    gdlayout2->addWidget(m_plabGatingMax, cnt++, 2);

    cnt = 0;
    gdlayout2->addWidget(m_pispSignalMax, cnt++, 3);
    gdlayout2->addWidget(m_pispGatingMin, cnt++, 3);
    gdlayout2->addWidget(m_pispGatingMax, cnt++, 3);
    gdlayout2->setRowStretch(5,10);
    m_pDegatParmGrpBx->setLayout(gdlayout2);

    QVBoxLayout *pvertlayout = new QVBoxLayout();
    pvertlayout->addWidget(m_pgrpDegatingBase);
    pvertlayout->addWidget(m_pDegatParmGrpBx);
    m_pDegateCtrlWdgt->setLayout(pvertlayout);

}
void AutoSequenceSettingDlg::InitOtherCtrl(void)
{

    m_pOtherCtrlWdgt = new QWidget(this);
    QLabel *pDescriLab = new QLabel(QString::fromLocal8Bit("3.Sequencing\n"\
                                                           "a)自动开始保存数据,开始倒计时\n"\
                                                           "b)开启破膜保护,破膜通道自动off\n"\
                                                           "c)开启de-gating,开启autounblock,开启定时OVF\n"\
                                                           "d)记录validQC参数,记录10s终点值\n"),this);

    QLabel *pMembrSaturatedCurrentlab = new QLabel(QStringLiteral("Saturated current:"),this);
    QLabel *m_pAutoUnblockPeriodlab = new QLabel(QStringLiteral("Autounblock period:"),this);
    QLabel *m_pOVFScanPeriodlab = new QLabel(QStringLiteral("Ovf period:"),this);
    QLabel *m_pOVFScanTimeslab = new QLabel(QStringLiteral("times:"),this);
    m_pMembrSaturatedCurrentDpbx=  new CustomDoubleSpinbox(-50000,50000,550,1,QStringLiteral(""),QStringLiteral(" pA"),this);
    m_pAutoUnblockPeriodDpbx=  new CustomDoubleSpinbox(0,360,30,1,QStringLiteral(""),QStringLiteral(" min"),this);
    m_pOVFScanPeriodDpbx= new CustomDoubleSpinbox(0,360,30,1,QStringLiteral(""),QStringLiteral(" min"),this);
    m_pOVFScanTimesSpbx= new CustomSpinbox(0,60,2,1,QStringLiteral(""),QStringLiteral(" times"),this);

    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(pDescriLab,0,0,1,4);
    pgridlayout->addWidget(pMembrSaturatedCurrentlab,1,0,1,1);
    pgridlayout->addWidget(m_pMembrSaturatedCurrentDpbx,1,1,1,1);


    pgridlayout->addWidget(m_pAutoUnblockPeriodlab,2,0,1,1);
    pgridlayout->addWidget(m_pAutoUnblockPeriodDpbx,2,1,1,1);

    pgridlayout->addWidget(m_pOVFScanPeriodlab,3,0,1,1);
    pgridlayout->addWidget(m_pOVFScanPeriodDpbx,3,1,1,1);
    pgridlayout->addWidget(m_pOVFScanTimeslab,3,2,1,1);
    pgridlayout->addWidget(m_pOVFScanTimesSpbx,3,3,1,1);

    pgridlayout->setColumnStretch(4,10);
    pgridlayout->setRowStretch(4,10);
    m_pOtherCtrlWdgt->setLayout(pgridlayout);
}

bool AutoSequenceSettingDlg::SaveConfig(QSettings* pSetting)
{
    if(pSetting != nullptr)
    {
        pSetting->beginGroup("SeqLockSetting");
        pSetting->setValue("OverScanDcVoL",m_pOverScanDcVoLtDpbx->value());
        pSetting->setValue("OverScanAcVoLt",m_pOverScanAcVoLtDpbx->value());
        pSetting->setValue("OverScanTimes",m_pOverScanTimesSpbx->value());
        pSetting->endGroup();

        pSetting->beginGroup("SeqValidFiltSetting");
        pSetting->setValue("ShuotffDcVolt",m_pShuotffDcVoltDpbx->value());
        pSetting->setValue("FilterCurMin",m_pFilterCurMinDpbx->value());
        pSetting->setValue("FilterCurMax",m_pFilterCurMaxDpbx->value());
        pSetting->setValue("FilterStdMin",m_pFilterStdMinDpbx->value());
        pSetting->setValue("FilterStdMax",m_pFilterStdMaxDpbx->value());
        pSetting->setValue("FilterWaitSec",m_PFilterWaitSecSpbx->value());
        pSetting->setValue("ValidFilterTimes",m_pValidFilterTimesSpbx->value());
        pSetting->setValue("ObserveNeed",m_pFilterNeedObersveChk->isChecked());
        pSetting->setValue("ObserveWatingTime",m_pFilterObserveDurationDpbx->value());
        pSetting->endGroup();

        pSetting->beginGroup("SeqOtherSetting");
        pSetting->setValue("MembrSaturatedCurrent",m_pMembrSaturatedCurrentDpbx->value());
        pSetting->setValue("AutoUnblockPeriod",m_pAutoUnblockPeriodDpbx->value());
        pSetting->setValue("OVFScanPeriod",m_pOVFScanPeriodDpbx->value());
        pSetting->setValue("OVFScanTimes",m_pOVFScanTimesSpbx->value());
        pSetting->endGroup();

        pSetting->beginGroup("SeqDegateSetting");
        pSetting->setValue("CyclePeriod",m_pispCyclePeriod->value());
        pSetting->setValue("DurationTime",m_pispDurationTime->value());
        pSetting->setValue("AllowTimes",m_pispAllowTimes->value());
        pSetting->setValue("DurationThres",m_pdspDurationThres->value());
        pSetting->setValue("GatingSTD",m_pispGatingSTD->value());
        pSetting->setValue("GatingSTDMax",m_pispGatingSTDMax->value());
        pSetting->setValue("SignalMin",m_pispSignalMin->value());
        pSetting->setValue("SignalMax",m_pispSignalMax->value());
        pSetting->setValue("GatingMin",m_pispGatingMin->value());
        pSetting->setValue("GatingMax",m_pispGatingMax->value());
        pSetting->setValue("bAutoseqDegat",m_pchkAutoseqGating->isChecked());
        pSetting->endGroup();


        return true;
    }
    return false;
}

bool AutoSequenceSettingDlg::LoadConfig(QSettings* pSetting)
{
    if(pSetting != nullptr)
    {

        m_pSettings = pSetting;
        pSetting->beginGroup("SeqLockSetting");
        m_pOverScanDcVoLtDpbx->setValue(pSetting->value("OverScanDcVoL").toDouble());
        m_pOverScanAcVoLtDpbx->setValue(pSetting->value("OverScanAcVoLt").toDouble());
        m_pOverScanTimesSpbx->setValue(pSetting->value("OverScanTimes").toInt());
        pSetting->endGroup();

        pSetting->beginGroup("SeqValidFiltSetting");
        m_pShuotffDcVoltDpbx->setValue(pSetting->value("ShuotffDcVolt").toDouble());
        m_pFilterCurMinDpbx->setValue(pSetting->value("FilterCurMin").toDouble());
        m_pFilterCurMaxDpbx->setValue(pSetting->value("FilterCurMax").toDouble());
        m_pFilterStdMinDpbx->setValue(pSetting->value("FilterStdMin").toDouble());
        m_pFilterStdMaxDpbx->setValue(pSetting->value("FilterStdMax").toDouble());
        m_PFilterWaitSecSpbx->setValue(pSetting->value("FilterWaitSec").toInt());
        m_pValidFilterTimesSpbx->setValue(pSetting->value("ValidFilterTimes").toInt());
        m_pFilterNeedObersveChk->setChecked(pSetting->value("ObserveNeed").toBool());
        m_pFilterObserveDurationDpbx->setValue(pSetting->value("ObserveWatingTime").toInt());
        pSetting->endGroup();

        pSetting->beginGroup("SeqOtherSetting");
        m_pMembrSaturatedCurrentDpbx->setValue(pSetting->value("MembrSaturatedCurrent").toDouble());
        m_pAutoUnblockPeriodDpbx->setValue(pSetting->value("AutoUnblockPeriod").toDouble());
        m_pOVFScanPeriodDpbx->setValue(pSetting->value("OVFScanPeriod").toDouble());
        m_pOVFScanTimesSpbx->setValue(pSetting->value("OVFScanTimes").toInt());
        pSetting->endGroup();

        pSetting->beginGroup("SeqDegateSetting");
        m_pispCyclePeriod->setValue(pSetting->value("CyclePeriod").toInt());
        m_pispDurationTime->setValue(pSetting->value("DurationTime").toInt());
        m_pispAllowTimes->setValue(pSetting->value("AllowTimes").toInt());
        m_pdspDurationThres->setValue(pSetting->value("DurationThres").toDouble());
        m_pispGatingSTD->setValue(pSetting->value("GatingSTD").toInt());
        m_pispGatingSTDMax->setValue(pSetting->value("GatingSTDMax").toInt());
        m_pispSignalMin->setValue(pSetting->value("SignalMin").toInt());
        m_pispSignalMax->setValue(pSetting->value("SignalMax").toInt());
        m_pispGatingMin->setValue(pSetting->value("GatingMin").toInt());
        m_pispGatingMax->setValue(pSetting->value("GatingMax").toInt());
        m_pchkAutoseqGating->setChecked(pSetting->value("bAutoseqDegat").toBool());

        pSetting->endGroup();
        SaveParams();
        return true;
    }
    return false;
}

void AutoSequenceSettingDlg::SaveParams(void)
{

    m_SSeqLockParams.overScanDcVoL = m_pOverScanDcVoLtDpbx->value();
    m_SSeqLockParams.overScanAcVoLt = m_pOverScanAcVoLtDpbx->value();
    m_SSeqLockParams.overScanTimes = m_pOverScanTimesSpbx->value();

    m_SSeqValidFiltParams.shuotffDcVolt = m_pShuotffDcVoltDpbx->value();
    m_SSeqValidFiltParams.filterCurMin = m_pFilterCurMinDpbx->value();
    m_SSeqValidFiltParams.filterCurMax = m_pFilterCurMaxDpbx->value();
    m_SSeqValidFiltParams.filterStdMin = m_pFilterStdMinDpbx->value();
    m_SSeqValidFiltParams.filterStdMax = m_pFilterStdMaxDpbx->value();
    m_SSeqValidFiltParams.filterWaitSec = m_PFilterWaitSecSpbx->value();
    m_SSeqValidFiltParams.validFilterTimes = m_pValidFilterTimesSpbx->value();
    m_SSeqValidFiltParams.bObserveWaitNeed = m_pFilterNeedObersveChk->isChecked();
    m_SSeqValidFiltParams.observeWaitTime = m_pFilterObserveDurationDpbx->value();


    m_SSeqOtherParams.membrSaturatedCurrent = m_pMembrSaturatedCurrentDpbx->value();
    m_SSeqOtherParams.autoUnblockPeriod = m_pAutoUnblockPeriodDpbx->value();
    m_SSeqOtherParams.oVFScanPeriod = m_pOVFScanPeriodDpbx->value();
    m_SSeqOtherParams.oVFScanTimes = m_pOVFScanTimesSpbx->value();

    m_SSeqDegateParams.cyclePeriod = m_pispCyclePeriod->value();
    m_SSeqDegateParams.durationTime = m_pispDurationTime->value();
    m_SSeqDegateParams.allowTimes = m_pispAllowTimes->value();
    m_SSeqDegateParams.durationThres = m_pdspDurationThres->value();
    m_SSeqDegateParams.gatingSTD = m_pispGatingSTD->value();
    m_SSeqDegateParams.gatingSTDMax = m_pispGatingSTDMax->value();
    m_SSeqDegateParams.signalMin = m_pispSignalMin->value();
    m_SSeqDegateParams.signalMax = m_pispSignalMax->value();
    m_SSeqDegateParams.gatingMin = m_pispGatingMin->value();
    m_SSeqDegateParams.gatingMax = m_pispGatingMax->value();
    m_SSeqDegateParams.bAutoseqDegat = m_pchkAutoseqGating->isChecked();

}
