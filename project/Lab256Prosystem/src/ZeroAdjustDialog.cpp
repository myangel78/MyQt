#include "ZeroAdjustDialog.h"
#include <qlayout.h>


ZeroAdjustDialog::ZeroAdjustDialog(QWidget *parent) : QDialog(parent)
{
	InitCtrl();
}

bool ZeroAdjustDialog::InitCtrl(void)
{
    //ZeroAdjust
    m_plabBackwardVoltage = new QLabel("BackwardVolt:", this);
    m_pdspBackwardVoltage = new QDoubleSpinBox(this);
    m_pdspBackwardVoltage->setMinimum(-99.99);
    m_pdspBackwardVoltage->setMaximum(99.99);
    m_pdspBackwardVoltage->setSingleStep(0.01);
    m_pdspBackwardVoltage->setSuffix(" V");
    m_pdspBackwardVoltage->setValue(-0.18);
    m_pdspBackwardVoltage->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置调零前的回调电压值</p></body></html>"));

    m_plabBackwardDuration = new QLabel("BackwardTime:", this);
    m_pdspBackwardDuration = new QDoubleSpinBox(this);
    m_pdspBackwardDuration->setMinimum(1.00);
    m_pdspBackwardDuration->setMaximum(999.99);
    m_pdspBackwardDuration->setSuffix(" s");
    m_pdspBackwardDuration->setValue(1.00);
    m_pdspBackwardDuration->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置调零前的回调电压作用时间</p></body></html>"));

    m_plabZeroVoltDuration = new QLabel("ZeroVoltTime:", this);
    m_pdspZeroVoltDuration = new QDoubleSpinBox(this);
    m_pdspZeroVoltDuration->setMinimum(3.00);
    m_pdspZeroVoltDuration->setMaximum(999.99);
    m_pdspZeroVoltDuration->setSuffix(" s");
    m_pdspZeroVoltDuration->setValue(3.00);
    m_pdspZeroVoltDuration->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置所有通道为0电压的作用时间</p></body></html>"));

    m_plabZeroAdjustTime = new QLabel("AdjustTime:", this);
    m_pdspZeroAdjustTime = new QDoubleSpinBox(this);
    m_pdspZeroAdjustTime->setMinimum(6.00);
    m_pdspZeroAdjustTime->setMaximum(999.99);
    m_pdspZeroAdjustTime->setSuffix(" s");
    m_pdspZeroAdjustTime->setValue(6.00);
    m_pdspZeroAdjustTime->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置调零电压后的作用时间</p></body></html>"));


    //Auto ZeroAdjust
    m_plabIntervalsTimer = new QLabel("Intervals:", this);
    m_pispIntervalsTimer = new QSpinBox(this);
    m_pispIntervalsTimer->setMinimum(15);
    m_pispIntervalsTimer->setMaximum(86399);//23:59:59
    m_pispIntervalsTimer->setSuffix(" s");
    m_pispIntervalsTimer->setValue(900);
    m_pispIntervalsTimer->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>设置自动调零的时间间隔</p></body></html>"));

    m_pbtnbox = new QDialogButtonBox(this);
    m_pbtnbox->addButton("OK", QDialogButtonBox::AcceptRole);
    m_pbtnbox->addButton("Cancel", QDialogButtonBox::RejectRole);
    connect(m_pbtnbox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_pbtnbox, SIGNAL(rejected()), this, SLOT(reject()));


    QGridLayout* gdlayout = new QGridLayout();
    gdlayout->setContentsMargins(1, 8, 1, 1);
    gdlayout->addWidget(m_plabBackwardVoltage, 0, 0);
    gdlayout->addWidget(m_plabBackwardDuration, 1, 0);
    gdlayout->addWidget(m_plabZeroVoltDuration, 2, 0);
    gdlayout->addWidget(m_plabZeroAdjustTime, 3, 0);
    gdlayout->addWidget(m_plabIntervalsTimer, 4, 0);
    gdlayout->addWidget(m_pdspBackwardVoltage, 0, 1);
    gdlayout->addWidget(m_pdspBackwardDuration, 1, 1);
    gdlayout->addWidget(m_pdspZeroVoltDuration, 2, 1);
    gdlayout->addWidget(m_pdspZeroAdjustTime, 3, 1);
    gdlayout->addWidget(m_pispIntervalsTimer, 4, 1);

    QVBoxLayout* verlayout6 = new QVBoxLayout();
    verlayout6->addLayout(gdlayout);
    verlayout6->addWidget(m_pbtnbox);

    setLayout(verlayout6);
    
    return false;
}
