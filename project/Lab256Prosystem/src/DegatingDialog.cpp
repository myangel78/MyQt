#include "DegatingDialog.h"

#include <qlayout.h>
#include "ConfigServer.h"


DegatingDialog::DegatingDialog(QWidget *parent) : QDialog(parent)
{
    InitCtrl();
    if (ConfigServer::GetInstance()->GetAutoDegating())
    {
        m_pispCyclePeriod->setEnabled(false);
        m_pispAllowTimes->setEnabled(false);
        m_pchkAutoseqGating->setEnabled(false);
    }
}

bool DegatingDialog::InitCtrl(void)
{
    QGroupBox* pgrpDegatingBase = new QGroupBox("Degating", this);
    //pgrpDegatingBase->setContentsMargins(3, 1, 3, 0);
    m_plabDegatingVolt = new QLabel("Degating Volt:", pgrpDegatingBase);
    m_pdspDegatingVolt = new QDoubleSpinBox(pgrpDegatingBase);
    m_pdspDegatingVolt->setDecimals(2);
    m_pdspDegatingVolt->setSuffix(" V");
    m_pdspDegatingVolt->setMinimum(-99.99);
    m_pdspDegatingVolt->setMaximum(99.99);
    m_pdspDegatingVolt->setSingleStep(0.01);
    m_pdspDegatingVolt->setValue(-0.1);
    m_pdspDegatingVolt->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置Degating电压</p></body></html>"));

    m_plabCyclePeriod = new QLabel("Cycle Period:", pgrpDegatingBase);
    m_pispCyclePeriod = new QSpinBox(pgrpDegatingBase);
    m_pispCyclePeriod->setSuffix(" s");
    m_pispCyclePeriod->setMinimum(1);
    m_pispCyclePeriod->setMaximum(99);
    m_pispCyclePeriod->setSingleStep(1);
    m_pispCyclePeriod->setValue(2);
    m_pispCyclePeriod->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置多久（几秒）检测一次gating状态</p></body></html>"));

    m_plabDurationTime = new QLabel("Duration Time:", pgrpDegatingBase);
    m_pispDurationTime = new QSpinBox(pgrpDegatingBase);
    m_pispDurationTime->setSuffix(" ms");
    m_pispDurationTime->setMinimum(100);
    m_pispDurationTime->setMaximum(10000);
    m_pispDurationTime->setSingleStep(100);
    m_pispDurationTime->setValue(200);
    m_pispDurationTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置施加Degating电压持续时间</p></body></html>"));

    m_plabSteppingTime = new QLabel("Stepping Time:", pgrpDegatingBase);
    m_pispSteppingTime = new QSpinBox(pgrpDegatingBase);
    m_pispSteppingTime->setSuffix(" ms");
    m_pispSteppingTime->setMinimum(0);
    m_pispSteppingTime->setMaximum(10000);
    m_pispSteppingTime->setSingleStep(10);
    m_pispSteppingTime->setValue(0);
    m_pispSteppingTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置gating检测增量时间（下一次比上一次增加的检测时间）</p></body></html>"));

    m_plabAllowTimes = new QLabel("Allow Times:", pgrpDegatingBase);
    m_pispAllowTimes = new QSpinBox(pgrpDegatingBase);
    //m_pispAllowTimes->setSuffix(" num");
    m_pispAllowTimes->setMinimum(0);
    m_pispAllowTimes->setMaximum(100);
    m_pispAllowTimes->setSingleStep(1);
    m_pispAllowTimes->setValue(10);
    m_pispAllowTimes->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间内的Degating次数，规定时间内达到此次数则关断该通道。0为表示此功能不开启</p></body></html>"));

    m_plabLimitCycle = new QLabel("Limit Cycle:", pgrpDegatingBase);
    m_pispLimitCycle = new QSpinBox(pgrpDegatingBase);
    //m_pispLimitCycle->setSuffix(" num");
    m_pispLimitCycle->setMinimum(5);
    m_pispLimitCycle->setMaximum(10000);
    m_pispLimitCycle->setSingleStep(1);
    m_pispLimitCycle->setValue(14);
    m_pispLimitCycle->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置规定时间周期，在此周期内判定Degating次数</p></body></html>"));

    m_plabSaturatedCurrent = new QLabel("Portect Current", pgrpDegatingBase);
    m_plabSaturatedCurrent->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>多孔和破膜检测的阈值设置请参考PoreInsertion页里面对应的多孔限流和膜破限流设置项</p></body></html>"));
    m_pdspSaturatedCurrent = new QDoubleSpinBox(pgrpDegatingBase);
    m_pdspSaturatedCurrent->setDecimals(2);
    m_pdspSaturatedCurrent->setSuffix(" pA");
    m_pdspSaturatedCurrent->setMinimum(-999999.99);
    m_pdspSaturatedCurrent->setMaximum(999999.99);
    m_pdspSaturatedCurrent->setValue(400.0);
    //m_pdspSaturatedCurrent->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置短路膜破检测的电流阈值</p></body></html>"));
    m_pdspSaturatedCurrent->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>多孔和破膜检测的阈值设置请参考PoreInsertion页里面对应的多孔限流和膜破限流设置项</p></body></html>"));
    m_pdspSaturatedCurrent->setEnabled(false);

    m_pchkAutoseqGating = new QCheckBox("Autoseq", pgrpDegatingBase);
    //m_pchkAutoseqGating->setChecked(true);
    m_pchkAutoseqGating->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否启用AutoseqGating算法</p></body></html>"));



    //Advanced
    QGroupBox* pgrpDegatingAdvanced = new QGroupBox("Advanced", this);
    //pgrpDegatingAdvanced->setContentsMargins(3, 1, 3, 0);
    m_plabDurationThres = new QLabel("Durate Thres:", pgrpDegatingAdvanced);
    m_pdspDurationThres = new QDoubleSpinBox(pgrpDegatingAdvanced);
    m_pdspDurationThres->setMinimum(-99.99);
    m_pdspDurationThres->setMaximum(99.99);
    m_pdspDurationThres->setValue(0.5);
    m_pdspDurationThres->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>最大值不能超过1，如果有某段信号0.5s一致维持在同一个水平不变，那就认为是gating状态</p></body></html>"));

    m_plabGatingSTD = new QLabel("Seq STD Min:", pgrpDegatingAdvanced);
    m_pispGatingSTD = new QSpinBox(pgrpDegatingAdvanced);
    m_pispGatingSTD->setMinimum(0);
    m_pispGatingSTD->setMaximum(100);
    m_pispGatingSTD->setValue(5);
    m_pispGatingSTD->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>小于该STD min值，则认为是gating状态</p></body></html>"));

    m_plabGatingSTDMax = new QLabel("Seq STD Max:", pgrpDegatingAdvanced);
    m_pispGatingSTDMax = new QSpinBox(pgrpDegatingAdvanced);
    m_pispGatingSTDMax->setMinimum(0);
    m_pispGatingSTDMax->setMaximum(50000);
    m_pispGatingSTDMax->setValue(105);
    m_pispGatingSTDMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>大于该STD max值，则认为是gating状态</p></body></html>"));

    m_plabSignalMin = new QLabel("Signal Min:", pgrpDegatingAdvanced);
    m_pispSignalMin = new QSpinBox(pgrpDegatingAdvanced);
    m_pispSignalMin->setMinimum(-200);
    m_pispSignalMin->setMaximum(200);
    m_pispSignalMin->setValue(20);
    m_pispSignalMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，小于该值为非gating,大于此值才进一步作gating分析</p></body></html>"));

    m_plabSignalMax = new QLabel("Signal Max:", pgrpDegatingAdvanced);
    m_pispSignalMax = new QSpinBox(pgrpDegatingAdvanced);
    m_pispSignalMax->setMinimum(0);
    m_pispSignalMax->setMaximum(500);
    m_pispSignalMax->setValue(150);
    m_pispSignalMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>初级阈值限制，大于该值为非gating,小于此值才进一步作gating分析</p></body></html>"));

    m_plabGatingMin = new QLabel("Gating Min:", pgrpDegatingAdvanced);
    m_pispGatingMin = new QSpinBox(pgrpDegatingAdvanced);
    m_pispGatingMin->setMinimum(-20);
    m_pispGatingMin->setMaximum(500);
    m_pispGatingMin->setValue(40);
    m_pispGatingMin->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，大于此值作gating分析</p></body></html>"));

    m_plabGatingMax = new QLabel("Gating Max:", pgrpDegatingAdvanced);
    m_pispGatingMax = new QSpinBox(pgrpDegatingAdvanced);
    m_pispGatingMax->setMinimum(0);
    m_pispGatingMax->setMaximum(500);
    m_pispGatingMax->setValue(130);
    m_pispGatingMax->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>二次阈值限制，小于此值作gating分析</p></body></html>"));

    //m_pbtnStartDegating = new QPushButton("Start", this);
    //m_pbtnStartDegating->setCheckable(true);

    m_pbtnbox = new QDialogButtonBox(this);
    m_pbtnbox->addButton("OK", QDialogButtonBox::AcceptRole);
    m_pbtnbox->addButton("Cancel", QDialogButtonBox::RejectRole);
    connect(m_pbtnbox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_pbtnbox, SIGNAL(rejected()), this, SLOT(reject()));


    QGridLayout* gdlayout1 = new QGridLayout();
    //gdlayout1->setContentsMargins(1, 8, 1, 1);
    gdlayout1->addWidget(m_plabDegatingVolt, 0, 0);
    gdlayout1->addWidget(m_plabCyclePeriod, 1, 0);
    gdlayout1->addWidget(m_plabDurationTime, 2, 0);
    gdlayout1->addWidget(m_plabSteppingTime, 3, 0);
    gdlayout1->addWidget(m_plabAllowTimes, 4, 0);
    gdlayout1->addWidget(m_plabLimitCycle, 5, 0);
    gdlayout1->addWidget(m_plabSaturatedCurrent, 6, 0);
    gdlayout1->addWidget(m_pdspDegatingVolt, 0, 1);
    gdlayout1->addWidget(m_pispCyclePeriod, 1, 1);
    gdlayout1->addWidget(m_pispDurationTime, 2, 1);
    gdlayout1->addWidget(m_pispSteppingTime, 3, 1);
    gdlayout1->addWidget(m_pispAllowTimes, 4, 1);
    gdlayout1->addWidget(m_pispLimitCycle, 5, 1);
    gdlayout1->addWidget(m_pdspSaturatedCurrent, 6, 1);

    pgrpDegatingBase->setLayout(gdlayout1);

    QHBoxLayout* horlayout1 = new QHBoxLayout();
    horlayout1->addSpacing(50);
    horlayout1->addWidget(m_pchkAutoseqGating);
    horlayout1->addStretch(1);


    int cnt = 0;
    //Advanced
    QGridLayout* gdlayout2 = new QGridLayout();
    gdlayout2->setContentsMargins(1, 8, 1, 1);
    gdlayout2->addWidget(m_plabDurationThres, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTD, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingSTDMax, cnt++, 0);
    gdlayout2->addWidget(m_plabSignalMin, cnt++, 0);
    gdlayout2->addWidget(m_plabSignalMax, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingMin, cnt++, 0);
    gdlayout2->addWidget(m_plabGatingMax, cnt++, 0);
    cnt = 0;
    gdlayout2->addWidget(m_pdspDurationThres, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTD, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingSTDMax, cnt++, 1);
    gdlayout2->addWidget(m_pispSignalMin, cnt++, 1);
    gdlayout2->addWidget(m_pispSignalMax, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingMin, cnt++, 1);
    gdlayout2->addWidget(m_pispGatingMax, cnt++, 1);

    pgrpDegatingAdvanced->setLayout(gdlayout2);


    //QHBoxLayout* horlayout6 = new QHBoxLayout();
    //horlayout6->addWidget(m_pbtnStartDegating);
    //horlayout6->addWidget(m_pbtnbox);


    QVBoxLayout* verlayout9 = new QVBoxLayout();
    //verlayout9->setContentsMargins(3, 1, 3, 1);
    verlayout9->addWidget(pgrpDegatingBase);
    verlayout9->addLayout(horlayout1);
    verlayout9->addWidget(pgrpDegatingAdvanced);
    verlayout9->addWidget(m_pbtnbox);
    //verlayout9->addLayout(horlayout6);

    setLayout(verlayout9);
    
    return false;
}

bool DegatingDialog::GetDegatingBaseParam(double& degatingVolt, int& cyclePeriod, int& durTime, int& stepTime, int& allowTimes, int& limitCycle, double& satcur) const
{
    degatingVolt = m_pdspDegatingVolt->value();
    cyclePeriod = m_pispCyclePeriod->value();
    durTime = m_pispDurationTime->value();
    stepTime = m_pispSteppingTime->value();
    allowTimes = m_pispAllowTimes->value();
    limitCycle = m_pispLimitCycle->value();
    satcur = m_pdspSaturatedCurrent->value();

    return false;
}

bool DegatingDialog::GetDegatingAdvanceParam(double& durThres, int& gatingstd, int& gatingstdmax, int& signalmin, int& signalmax, int& gatingmin, int& gatingmax) const
{
    durThres = m_pdspDurationThres->value();
    gatingstd = m_pispGatingSTD->value();
    gatingstdmax = m_pispGatingSTDMax->value();
    signalmin = m_pispSignalMin->value();
    signalmax = m_pispSignalMax->value();
    gatingmin = m_pispGatingMin->value();
    gatingmax = m_pispGatingMax->value();

    return false;
}

void DegatingDialog::SetDegatingBaseParam(double degatingVolt, int cyclePeriod, int durTime, int stepTime, int allowTimes, int limitCycle, double satcur)
{
    m_pdspDegatingVolt->setValue(degatingVolt);
    m_pispCyclePeriod->setValue(cyclePeriod);
    m_pispDurationTime->setValue(durTime);
    m_pispSteppingTime->setValue(stepTime);
    m_pispAllowTimes->setValue(allowTimes);
    m_pispLimitCycle->setValue(limitCycle);
    m_pdspSaturatedCurrent->setValue(satcur);
}

void DegatingDialog::SetDegatingAdvanceParam(double durThres, int gatingstd, int gatingstdmax, int signalmin, int signalmax, int gatingmin, int gatingmax)
{
    m_pdspDurationThres->setValue(durThres);
    m_pispGatingSTD->setValue(gatingstd);
    m_pispGatingSTDMax->setValue(gatingstdmax);
    m_pispSignalMin->setValue(signalmin);
    m_pispSignalMax->setValue(signalmax);
    m_pispGatingMin->setValue(gatingmin);
    m_pispGatingMax->setValue(gatingmax);
}
