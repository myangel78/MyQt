#include "ControlListItem.h"
#include <qpainter.h>
#include <qaction.h>
#include <qlayout.h>
#include <qcolordialog.h>
#include <qmessagebox.h>

#include <Log.h>

#include "ConfigServer.h"
#include "UsbProtoMsg.h"


ControlListItem::ControlListItem(int chan, const QColor& color, QWidget *parent) 
	: QWidget(parent)
	, m_iChannel(chan)
{
	InitCtrl(chan, color);

    connect(this, SIGNAL(degatingStateColorSignal(int)), this, SLOT(degatingStateColorSlot(int)));
    connect(this, SIGNAL(degatingStatePollingSignal(int)), this, SLOT(degatingStatePollingSlot(int)));

    m_ptmrPollingDegating = new QTimer(this);
    m_ptmrPollingDegating->setSingleShot(false);
    connect(m_ptmrPollingDegating, &QTimer::timeout, this, &ControlListItem::timerPollingDegating);

    m_ptmrDurationDegating = new QTimer(this);
    m_ptmrDurationDegating->setSingleShot(true);
    connect(m_ptmrDurationDegating, &QTimer::timeout, this, &ControlListItem::timerDurationDegating);

    m_ptmrLimitCheckDegating = new QTimer(this);
    m_ptmrLimitCheckDegating->setSingleShot(false);
    connect(m_ptmrLimitCheckDegating, &QTimer::timeout, this, &ControlListItem::timerLimitCheckDegating);
}

ControlListItem::~ControlListItem()
{
    ResetDegatingState();
}

bool ControlListItem::InitCtrl(int chan, const QColor& color)
{
    int channo = chan + 1;
    m_pchkChannel = new QCheckBox(QString("CH%1").arg(channo), this);
    m_pchkChannel->setChecked(true);
    m_pchkChannel->setMinimumWidth(60);
    m_pchkChannel->setMaximumWidth(65);
    m_pchkChannel->setToolTip(QString("Show Channel %1").arg(channo));

    m_pchkChanSeq = new QCheckBox(QString("CH%1").arg(channo), this);
    m_pchkChanSeq->setChecked(true);
    m_pchkChanSeq->setMinimumWidth(60);
    m_pchkChanSeq->setMaximumWidth(65);
    m_pchkChanSeq->setToolTip(QString("Sequencing Channel %1").arg(channo));

    m_pbtnChannel = new QToolButton(this);
    m_pbtnChannel->setFixedSize(QSize(16, 16));
    m_pbtnChannel->setAutoRaise(true);
    QAction* action = new QAction(this);
    action->setToolTip(tr("Change plot lines color"));
    m_pbtnChannel->setDefaultAction(action);
    SetColor(color);

    m_plabCurCurrent = new QLabel(this);
    m_plabCurCurrent->setMinimumWidth(65);
    m_plabCurCurrent->setFrameShape(QFrame::StyledPanel);
    m_plabCurCurrent->setToolTip(QString("Instantaneous Current for Channel %1").arg(channo));

    m_plabAvgCurrent = new QLabel(this);
    m_plabAvgCurrent->setMinimumWidth(65);
    m_plabAvgCurrent->setFrameShape(QFrame::StyledPanel);
    m_plabAvgCurrent->setToolTip(QString("Average Current for Channel %1").arg(channo));

    m_plabStdCurrent = new QLabel(this);
    m_plabStdCurrent->setMinimumWidth(40);
    m_plabStdCurrent->setFrameShape(QFrame::StyledPanel);
    m_plabStdCurrent->setToolTip(QString("STD of Current for Channel %1").arg(channo));

    //m_pchkAutoZero = new QCheckBox("Auto", this);
    //m_pchkAutoZero->setChecked(true);
    //m_pchkAutoZero->setMaximumWidth(50);
    //m_pchkAutoZero->setEnabled(false);

    m_pbtnStatus = new QToolButton(this);
    m_pbtnStatus->setStyleSheet(g_colorStyle[EEmbedPoreStatus::EPS_NONEPORE]);
    m_pbtnStatus->setFixedSize(QSize(16, 16));
    //m_pbtnStatus->setEnabled(false);
    //m_pbtnStatus->setMaximumSize(QSize(16, 16));
    //m_pbtnStatus->setAutoRaise(true);
    //m_pbtnStatus->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    //m_pbtnStatus->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    //m_pbtnStatus->setPopupMode(QToolButton::DelayedPopup);


    m_pchkChanVolt = new QCheckBox(QString("CH%1").arg(chan + 1), this);
    //m_pchkChanDegating->setChecked(true);
    m_pchkChanVolt->setMinimumWidth(60);
    m_pchkChanVolt->setMaximumWidth(65);

    //m_pbtnPoreState = new QToolButton;
    //m_pbtnPoreState->setFixedSize(QSize(16, 16));
    //m_pbtnPoreState->setAutoRaise(true);
    //m_pbtnPoreState->setStyleSheet(g_SinglePoreStyle[0]);
    //m_pbtnPoreState->setCheckable(true);
    ////QAction* action = new QAction(this);
    ////action->setToolTip(tr("Single Pore State"));
    ////m_pbtnPoreState->setDefaultAction(action);

    m_pbtnOpposite = new QPushButton("+/-", this);
    m_pbtnOpposite->setMaximumWidth(30);

    m_pdspVoltAmpl = new QDoubleSpinBox(this);
    //m_pdspVoltAmpl->setValue(0.0);
    m_pdspVoltAmpl->setMaximum(10.0);
    m_pdspVoltAmpl->setMinimum(-10.0);
    m_pdspVoltAmpl->setSingleStep(0.1);

    m_pbtnApply = new QPushButton("Set", this);
    m_pbtnApply->setMaximumWidth(30);

    //m_pchkAutoDegating = new QCheckBox("Auto", this);
    //m_pchkAutoDegating->setChecked(false);
    //m_pchkAutoDegating->setMaximumWidth(50);
    //m_pchkAutoDegating->setEnabled(false);

    m_pbtnDegatingColor = new QToolButton(this);
    m_pbtnDegatingColor->setFixedSize(QSize(16, 16));
    m_pbtnDegatingColor->setStyleSheet(g_DegatingSheetStyle[0]);

    m_plabDegatingCount = new QLabel(this);
    m_plabDegatingCount->setMinimumWidth(20);
    m_plabDegatingCount->setMaximumWidth(50);
    m_plabDegatingCount->setFrameShape(QFrame::StyledPanel);
    m_plabDegatingCount->setToolTip(QString("The Count of Degating in Time Period for Channel %1").arg(channo));
    m_plabDegatingCount->setText(QString::number(m_iCurTimes));
    //m_pbtnDegatingCount = new QToolButton(this);
    //m_pbtnDegatingCount->setMinimumWidth(20);
    //m_pbtnDegatingCount->setMaximumWidth(30);
    //m_pbtnDegatingCount->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>The Count of Degating in Time Period for Channel %1</p><p>¿Éµã»÷ÖØÖÃGating×´Ì¬</p></body></html>").arg(channo));
    //m_pbtnDegatingCount->setText(QString::number(m_iCurTimes));


    QHBoxLayout* horlayout1 = new QHBoxLayout();
    //horlayout1->setMargin(0);
    horlayout1->setContentsMargins(1, 0, 1, 0);
    horlayout1->setSpacing(0);
    horlayout1->addWidget(m_pchkChannel);
    horlayout1->addWidget(m_pchkChanSeq);
    horlayout1->addWidget(m_pbtnChannel);
    horlayout1->addWidget(m_plabCurCurrent);
    horlayout1->addSpacing(2);
    horlayout1->addWidget(m_plabAvgCurrent);
    horlayout1->addSpacing(2);
    horlayout1->addWidget(m_plabStdCurrent);
    horlayout1->addSpacing(2);
    //horlayout1->addWidget(m_pchkAutoZero);
    horlayout1->addWidget(m_pbtnStatus);

    horlayout1->addSpacing(5);
    horlayout1->addWidget(m_pchkChanVolt);
    //horlayout1->addWidget(m_pbtnPoreState);
    horlayout1->addSpacing(2);
    horlayout1->addWidget(m_pbtnOpposite);
    horlayout1->addWidget(m_pdspVoltAmpl);
    horlayout1->addWidget(m_pbtnApply);
    horlayout1->addSpacing(2);
    //horlayout1->addWidget(m_pchkAutoDegating);
    horlayout1->addWidget(m_pbtnDegatingColor);
    horlayout1->addSpacing(2);
    horlayout1->addWidget(m_plabDegatingCount);
    //horlayout1->addWidget(m_pbtnDegatingCount);

    setLayout(horlayout1);


    connect(m_pchkChannel, &QCheckBox::stateChanged, this, &ControlListItem::onCheckChannelChange);
    connect(m_pbtnChannel, &QToolButton::clicked, this, &ControlListItem::onClickChannelChange);
    connect(m_pbtnStatus, &QToolButton::clicked, this, &ControlListItem::onClickStatusChange);


    //connect(m_pchkChanVolt, &QCheckBox::stateChanged, this, &ControlListItem::onCheckChanVoltChange);
    //connect(m_pbtnPoreState, &QToolButton::clicked, this, &ControlListItem::onClickPoreStateChange);
    connect(m_pbtnOpposite, &QToolButton::clicked, this, &ControlListItem::onClickVoltageOpposite);
    connect(m_pbtnApply, &QToolButton::clicked, this, &ControlListItem::onClickVoltageApply);
    //connect(m_pchkAutoDegating, &QCheckBox::stateChanged, this, &ControlListItem::onCheckAutoDegating);
    connect(m_pbtnDegatingColor, &QToolButton::clicked, this, &ControlListItem::onClickDegatingColor);
    //connect(m_pbtnDegatingCount, &QToolButton::clicked, this, &ControlListItem::onClickResetDegating);

    return false;
}

void ControlListItem::SetChannelDCLevel(double volt)
{
    emit SetChannelDCLevelSignal(m_iChannel, volt);
    //int arout = 0;
    //size_t len = UsbProtoMsg::SetOneDCLevel(m_iChannel, volt, arout);
    //emit transmitUsbData(arout, len);
    if (ConfigServer::GetInstance()->GetSaveData())
    {
        time_t setvoltmt = time(nullptr);
        char arvoltinfo[1024] = { 0 };
        sprintf(arvoltinfo, "[%f] CH%d Volt2=%.2f\n", difftime(setvoltmt, ConfigServer::GetInstance()->GetStartSaveData()) / 60.0, m_iChannel + 1, volt);
        //sprintf(arvoltinfo, "[%f] CH%d Volt2=%.2f\n", ConfigServer::GetInstance()->GetRawDataTime(), m_iChannel + 1, volt);
        ConfigServer::GetInstance()->EnqueueVoltageInfo(std::string(arvoltinfo));
        //LOGI(arvoltinfo);
    }
}

void ControlListItem::DoClickDegatingColor(void)
{
    if (!m_ptmrDurationDegating->isActive())
    {
        m_bIsDegating = true;
        //SetDegatingState(EGatingState::EGS_NORMAL);
        emit singleChanDegatingSignal(m_iChannel, m_dDegatingVolt);
        //SetChannelDCLevel(m_dDegatingVolt);
        //m_ptmrDurationDegating->start(m_iDurationTime + m_iCurTimes * m_iSteppingTime);

        ++m_iCurTimes;
        m_plabDegatingCount->setText(QString::number(m_iCurTimes));
        //m_pbtnDegatingCount->setText(QString::number(m_iCurTimes));

        bool retclose = false;
        if (m_iAllowTimes > 0 && m_ptmrLimitCheckDegating->isActive())
        {
            retclose = CloseChannelWhileReachAllowTime();
        }

        if (!retclose)
        {
            //m_ptmrDurationDegating->setInterval(m_iDurationTime + m_iCurTimes * m_iSteppingTime);
            m_ptmrDurationDegating->start(m_iDurationTime + m_iCurTimes * m_iSteppingTime);
        }
    }
}

bool ControlListItem::CloseChannelWhileReachAllowTime(void)
{
    if (m_iCurTimes >= m_iAllowTimes)
    {
        m_ptmrLimitCheckDegating->stop();

        //m_phsLastHoleStatus = m_phsHoleStatus;
        SetDegatingState(EGatingState::EGS_INACTIVE);
        auto cur = m_plabCurCurrent->text().toFloat();
        if (cur > 20)
        {
            m_phsHoleStatus = EEmbedPoreStatus::EPS_INACTIVEPORE;
        }
        else
        {
            m_phsHoleStatus = EEmbedPoreStatus::EPS_SLIPPORE;
        }
        m_pbtnStatus->setStyleSheet(g_colorStyle[m_phsHoleStatus]);

        m_pchkChannel->setChecked(false);
        m_pchkChanSeq->setChecked(false);
        m_pdspVoltAmpl->setValue(0.0);
        SetChannelDCLevel(0.0);
        //emit UpdateListItemSignal();//hidden

        m_bIsDegating = false;

        return true;
    }
    //else
    //{
    //    m_pbtnStatus->setStyleSheet(g_colorStyle[m_phsHoleStatus]);
    //}

    return false;
}

void ControlListItem::SetColor(const QColor& color)
{
    m_clrChannel = color;
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(QRect(0, 0, 15, 15));
    painter.fillRect(QRect(1, 1, 14, 14), color);

    m_pbtnChannel->defaultAction()->setIcon(QIcon(pixmap));
}

void ControlListItem::onCheckChannelChange(bool chk)
{
    emit checkChangedSignal(m_iChannel, chk);
}

void ControlListItem::onClickChannelChange(void)
{
    QColor color = QColorDialog::getColor(QColor(m_clrChannel), this);

    if (color.isValid())
    {
        SetColor(color);
        emit colorChangedSignal(m_iChannel, m_clrChannel);
    }
}

void ControlListItem::onClickStatusChange(void)
{
    if (!m_bZeroSetted)
    {
        m_bZeroSetted = true;
        //emit setZeroVoltSignal(m_iChannel, m_phsHoleStatus);
        double volt = 0.0;
        if (m_phsHoleStatus == EEmbedPoreStatus::EPS_SINGLEPORE)
        {
            volt = m_dProtectVolt;
            m_pbtnStatus->setStyleSheet(g_colorStyle[EEmbedPoreStatus::EPS_SINGLEPORE_PROTECT]);
        }
        else
        {
            m_pbtnStatus->setStyleSheet(g_borderStyle[1] + g_colorStyle[m_phsHoleStatus]);
            //if (m_phsHoleStatus != EEmbedPoreStatus::EPS_NONEPORE)
            {
                m_pchkChannel->setChecked(false);
                m_pchkChanSeq->setChecked(false);
            }
        }

        m_pdspVoltAmpl->setValue(volt);
        SetChannelDCLevel(volt);

    }
    //QMessageBox::information(this, tr("Information"), QString("clickStatusChange = CH%1").arg(m_iChannel + 1));
}

//void ControlListItem::onCheckChanVoltChange(bool chk)
//{
//    emit checkDegatingChangedSignal(m_iChannel, chk);// == Qt::CheckState::Checked
//}

//void ControlListItem::onClickPoreStateChange(bool chk)
//{
//    if (chk)
//    {
//        m_pbtnPoreState->setStyleSheet(g_SinglePoreStyle[1]);
//        m_bPoreState = true;
//        m_pchkChanVolt->setChecked(true);
//    }
//    else
//    {
//        m_pbtnPoreState->setStyleSheet(g_SinglePoreStyle[0]);
//        m_bPoreState = false;
//        m_pchkChanVolt->setChecked(false);
//    }
//}

void ControlListItem::onClickVoltageOpposite(void)
{
    double voltampl = 0.0 - m_pdspVoltAmpl->value();
    m_pdspVoltAmpl->setValue(voltampl);
    SetChannelDCLevel(voltampl);
}

void ControlListItem::onClickVoltageApply(void)
{
    double voltampl = m_pdspVoltAmpl->value();
    SetChannelDCLevel(voltampl);
}

//void ControlListItem::onCheckAutoDegating(bool chk)
//{
//    if (chk)
//    {
//        //m_ptimerPollingDegating->setInterval(m_iCyclePeriod);
//        m_ptimerPollingDegating->start(m_iCyclePeriod);
//        m_ptimerLimitCheckDegating->start(m_iLimitCheckTime);
//    }
//    else
//    {
//        if (m_ptimerPollingDegating->isActive())
//        {
//            m_ptimerPollingDegating->stop();
//        }
//        if (m_ptimerLimitCheckDegating->isActive())
//        {
//            m_ptimerLimitCheckDegating->stop();
//        }
//    }
//}

void ControlListItem::onClickDegatingColor(void)
{
    if (m_eGatingState == EGatingState::EGS_INACTIVE)
    {
        SetDegatingState(EGatingState::EGS_NORMAL);
        m_iCurTimes = 0;
        m_plabDegatingCount->setText("0");
        if (!m_ptmrLimitCheckDegating->isActive())
        {
            m_ptmrLimitCheckDegating->start(m_iLimitCheckTime);
        }
    }
    DoClickDegatingColor();
}

void ControlListItem::onClickResetDegating(void)
{
    if (m_eGatingState == EGatingState::EGS_INACTIVE)
    {
        SetDegatingState(EGatingState::EGS_NORMAL);
        m_iCurTimes = 0;
        m_plabDegatingCount->setText("0");
        //m_pbtnDegatingCount->setText("0");
    }
}

void ControlListItem::degatingStateColorSlot(int state)
{
    if (m_eGatingState != state)
    {
        m_eGatingState = (EGatingState)state;
        m_pbtnDegatingColor->setStyleSheet(g_DegatingSheetStyle[state]);
    }
    //m_pbtnDegatingColor->setStyleSheet(g_DegatingSheetStyle[state]);
}

void ControlListItem::degatingStatePollingSlot(int state)
{
    if (m_eGatingState != state)
    {
        m_eGatingState = (EGatingState)state;
        m_pbtnDegatingColor->setStyleSheet(g_DegatingSheetStyle[state]);
    }

    if (m_pchkChanSeq->isChecked() && (m_eGatingState == EGatingState::EGS_GATING))
    {
        if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
        {
            DoClickDegatingColor();
        }
    }
}

void ControlListItem::timerPollingDegating(void)
{
    //if (isVisible() && m_pchkAutoDegating->isChecked() && (m_eGatingState == EGatingState::EGS_GATING))
    if (isVisible() && m_pchkChanSeq->isChecked() && (m_eGatingState == EGatingState::EGS_GATING))
    {
        if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
        {
            DoClickDegatingColor();
        }
    }
}

void ControlListItem::timerDurationDegating(void)
{
    if (m_eGatingState != EGatingState::EGS_INACTIVE)
    {
        if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
        {
            emit singleChanDegatingRecoverSignal(m_iChannel);
            //SetChannelDCLevel(m_pdspVoltAmpl->value());//m_dSequencingVolt
        }
    }

    m_bIsDegating = false;
}

void ControlListItem::timerLimitCheckDegating(void)
{
    //if (m_pchkAutoDegating->isChecked())
    if (m_pchkChanSeq->isChecked())
    {
        CloseChannelWhileReachAllowTime();
        m_iCurTimes = 0;
    }
}
