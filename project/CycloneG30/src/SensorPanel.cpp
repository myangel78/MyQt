#include "SensorPanel.h"
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QApplication>
#include <QTime>
#include <iostream>
#include <QDoubleSpinBox>
#include <QLCDNumber>
#include <future>
#include <QTimer>
#include <QFile>

#include <ConfigServer.h>
#include "Log.h"
#include "UsbFtdDevice.h"
#include "PoreStateMapWdgt.h"
#include "PoreStateMapTabModel.h"
#include "SavePannel.h"


SensorPanel::SensorPanel(QWidget *parent):QWidget(parent)
{
    InitCtrl();
}

SensorPanel::~SensorPanel()
{

}

void SensorPanel::InitCtrl(void)
{
    QGroupBox *pSensorGroubox = new QGroupBox(QStringLiteral("All Sensor Mux Select"),this);
    QGroupBox *pSensorCustomGrp = new QGroupBox(QStringLiteral("Custom Sensor Mux Select"),this);
    QGroupBox *pAutoTimerUnblockGrp = new QGroupBox(QStringLiteral("Auto Degating"),this);
    pSensorCustomGrp->setCheckable(true);

    m_pAllChSelBtn = new QPushButton(QStringLiteral("All channel"),this);
    m_pSensorApplyBtn = new QPushButton(QStringLiteral("Apply"),this);
    m_pStartChpSelSpbox = new QSpinBox(this);
    m_pEndChSelSpbox = new QSpinBox(this);
    m_pSensorFuncStateComb = new QComboBox(this);
    QLabel *pStartchlab = new QLabel(QStringLiteral("Start(L)"),this);
    QLabel *pEndchlab = new QLabel(QStringLiteral("End(H)"),this);
    QLabel *pStateComblab = new QLabel(QStringLiteral("State"),this);

    QLabel *pStateQuickGrouplab = new QLabel(QStringLiteral("Quick select normal:"),this);
    m_pSensorASelbtn = new QRadioButton(QString("A"),this);
    m_pSensorBSelbtn = new QRadioButton(QString("B"),this);
    m_pSensorCSelbtn = new QRadioButton(QString("C"),this);
    m_pSensorDSelbtn = new QRadioButton(QString("D"),this);
    m_pSensorOffSelbtn = new QRadioButton(QString("Off"),this);
    m_pSensorCurrSelbtn = new QRadioButton(QString("Internal sources"),this);
    m_pSensorMuxSelbtn = new QRadioButton(QString("Mux"),this); //混合模式

    QLabel *pTimerAutoDegatLab = new QLabel("Auto unblock",this);
    m_pTimerToDegating = new QTimer(this);
    m_pTimerIntalInptDpbx= new QDoubleSpinBox(this);
    m_pTimerAutoDegatinBtn = new QPushButton(QStringLiteral("Auto unblock"),this);
    m_pTimerAutoDegatinBtn->setCheckable(true);
    m_pTimerIntalInptDpbx->setRange(0.1,360);
    m_pTimerIntalInptDpbx->setValue(10);
    m_pTimerIntalInptDpbx->setSuffix(QStringLiteral(" min"));
    m_plcdTimerPeriod = new QLCDNumber(8, this);
    m_plcdTimerPeriod->setSegmentStyle(QLCDNumber::Flat);
    m_plcdTimerPeriod->setFrameShape(QFrame::StyledPanel);

    QButtonGroup *SensorBtnGrp = new QButtonGroup(this);
    SensorBtnGrp->addButton(m_pSensorASelbtn,ALL_SENSOR_STATE_A_SELECT);
    SensorBtnGrp->addButton(m_pSensorBSelbtn,ALL_SENSOR_STATE_B_SELECT);
    SensorBtnGrp->addButton(m_pSensorCSelbtn,ALL_SENSOR_STATE_C_SELECT);
    SensorBtnGrp->addButton(m_pSensorDSelbtn,ALL_SENSOR_STATE_D_SELECT);
    SensorBtnGrp->addButton(m_pSensorOffSelbtn,ALL_SENSOR_STATE_OFF_SELECT);
    SensorBtnGrp->addButton(m_pSensorCurrSelbtn,ALL_SENSOR_STATE_INTERNAL_CURRENT_SELECT);
    SensorBtnGrp->addButton(m_pSensorMuxSelbtn,ALL_SENSOR_STATE_MUX_SELECT);
    SensorBtnGrp->setExclusive(true);
    m_pSensorOffSelbtn->setChecked(true);

    m_pSensorFuncStateComb->addItem(QStringLiteral("Normal"),SENSORGRP_FUNC_STATE_NORMAL);
    m_pSensorFuncStateComb->addItem(QStringLiteral("Unblock"),SENSORGRP_FUNC_STATE_UNBLOCK);
    m_pSensorFuncStateComb->addItem(QStringLiteral("Internal sources"),SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE);
    m_pSensorFuncStateComb->addItem(QStringLiteral("Switch off"),SENSORGRP_FUNC_STATE_OFF);
    m_pSensorFuncStateComb->setEnabled(true);

    m_pStartChpSelSpbox->setRange(1,CHANNEL_NUM);
    m_pEndChSelSpbox->setRange(1,CHANNEL_NUM);
    m_pStartChpSelSpbox->setValue(1);
    m_pEndChSelSpbox->setValue(CHANNEL_NUM);
    m_pStartChpSelSpbox->setPrefix(QStringLiteral("Ch "));
    m_pEndChSelSpbox->setPrefix(QStringLiteral("Ch "));

    QHBoxLayout *pAllGrphorlayout = new QHBoxLayout();
    pAllGrphorlayout->addWidget(pStateQuickGrouplab);
    pAllGrphorlayout->addWidget(m_pSensorASelbtn);
    pAllGrphorlayout->addWidget(m_pSensorBSelbtn);
    pAllGrphorlayout->addWidget(m_pSensorCSelbtn);
    pAllGrphorlayout->addWidget(m_pSensorDSelbtn);
    pAllGrphorlayout->addWidget(m_pSensorOffSelbtn);
    pAllGrphorlayout->addWidget(m_pSensorCurrSelbtn);
    pAllGrphorlayout->addWidget(m_pSensorMuxSelbtn);
    pAllGrphorlayout->addSpacerItem(new QSpacerItem(10, 10));
    pAllGrphorlayout->addStretch();
    pSensorGroubox->setLayout(pAllGrphorlayout);


    QGridLayout *pSengridlayout = new QGridLayout();
    pSengridlayout->addWidget(m_pAllChSelBtn,1,0,1,1);
    pSengridlayout->addWidget(pStartchlab,0,1,1,1);
    pSengridlayout->addWidget(m_pStartChpSelSpbox,1,1,1,1);
    pSengridlayout->addWidget(pEndchlab,0,2,1,1);
    pSengridlayout->addWidget(m_pEndChSelSpbox,1,2,1,1);
    pSengridlayout->addWidget(pStateComblab,0,3,1,1);
    pSengridlayout->addWidget(m_pSensorFuncStateComb,1,3,1,2);
    pSengridlayout->addWidget(m_pSensorApplyBtn,1,6,1,1);
    pSengridlayout->setColumnStretch(7,10);
    pSensorCustomGrp->setLayout(pSengridlayout);

    QHBoxLayout *pAutoTmrDegathorlayout =new QHBoxLayout();
    pAutoTmrDegathorlayout->addWidget(pTimerAutoDegatLab);
    pAutoTmrDegathorlayout->addWidget(m_pTimerIntalInptDpbx);
    pAutoTmrDegathorlayout->addWidget(m_plcdTimerPeriod);
    pAutoTmrDegathorlayout->addWidget(m_pTimerAutoDegatinBtn);
    pAutoTmrDegathorlayout->addStretch();
    pAutoTimerUnblockGrp->setLayout(pAutoTmrDegathorlayout);

    QHBoxLayout *horlyout1 = new QHBoxLayout();
    horlyout1->addWidget(pSensorCustomGrp);
    horlyout1->addWidget(pAutoTimerUnblockGrp);


    QVBoxLayout *vertlout = new QVBoxLayout();
    vertlout->addWidget(pSensorGroubox);
    vertlout->addLayout(horlyout1);
    vertlout->addStretch();

    setLayout(vertlout);
    connect(m_pAllChSelBtn,&QPushButton::clicked,[&](){ m_pStartChpSelSpbox->setValue(1); m_pEndChSelSpbox->setValue(CHANNEL_NUM);});
    connect(SensorBtnGrp, QOverload<int>::of(&QButtonGroup::buttonClicked),this,&SensorPanel::OnAllSensorSelectModeChgeBtnSlot);
    connect(m_pSensorApplyBtn,&QPushButton::clicked,this,&SensorPanel::SensorGrpCustomSelctBtnSlot);
    connect(m_pTimerToDegating,&QTimer::timeout,this,&SensorPanel::OnTimerToAutoDegatSlot);
    connect(m_pTimerAutoDegatinBtn,&QPushButton::clicked,this,&SensorPanel::OnAutoDegateBtnSlot);

    SetTimehms();

}

bool SensorPanel::SetTimehms(void)
{
    int itm = m_pTimerIntalInptDpbx->value() * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmAutoDegate.setHMS(ihour, imin, isec, 0);
        m_plcdTimerPeriod->display(m_tmAutoDegate.toString("hh:mm:ss"));

        return true;
    }
    return false;
}
bool SensorPanel::GenerateChannelArray(const int &block,std::vector<int> &vctIncoherentCh)
{
    int channelTotal = 64;
    vctIncoherentCh.resize(channelTotal);
    for(int i =0; i< channelTotal; ++i)
    {
        vctIncoherentCh[i] = block+ i*16;
    }
    return true;
}


void SensorPanel::OnUnblockAllChannelSlot(const int &delayms)
{
    int totalsize = 64;
    std::vector<int> vctIncoherentCh(totalsize,0);
    std::vector<SENSORGRP_FUNC_STATE_ENUM>vctIncoherentState(totalsize,SENSORGRP_FUNC_STATE_UNBLOCK);

    const auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    auto &vctAllchDegateResultState = m_pModelSharedDataPtr->GetAllChDegateStateData();

    for(int unblockGrp = 0; unblockGrp < 16; ++unblockGrp)
    {
        GenerateChannelArray(unblockGrp,vctIncoherentCh);
        for(int i = 0; i < totalsize; ++i)
        {
            int ch  = vctIncoherentCh[i];
            vctAllchDegateResultState[ch] = RECORD_IS_DEGATE_STATE;
            vctIncoherentState[i] = (vctAllchFuncState[ch] == SENSORGRP_FUNC_STATE_OFF ? SENSORGRP_FUNC_STATE_OFF : SENSORGRP_FUNC_STATE_UNBLOCK);
        }
        //record degate state to csv
        RecordDegateLog(vctIncoherentCh,vctIncoherentState);

        IncontinueChDifferFuncStateSet(vctIncoherentCh,vctIncoherentState);
        //unblock延时
        ConfigServer::caliSleep(delayms);
        //设置回normal状态
#ifdef EASY_PROGRAM_DEBUGGING
        for(int i = 0; i < totalsize; ++i)
        {
            int ch  = vctIncoherentCh[i];
            vctIncoherentState[i] = (vctAllchFuncState[ch] == SENSORGRP_FUNC_STATE_OFF ? SENSORGRP_FUNC_STATE_OFF : SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE);
        }
#else
        for(int i = 0; i < totalsize; ++i)
        {
            int ch  = vctIncoherentCh[i];
            vctIncoherentState[i] = (vctAllchFuncState[ch] == SENSORGRP_FUNC_STATE_OFF ? SENSORGRP_FUNC_STATE_OFF : SENSORGRP_FUNC_STATE_NORMAL);
        }
#endif
        IncontinueChDifferFuncStateSet(vctIncoherentCh,vctIncoherentState);
    }

}

void SensorPanel::OnTimerToAutoDegatSlot(void)
{
    int delayms = 200;
    bool bIsDegateRunning = false;
    bool bIsAutoScanRunning = false;
    m_tmAutoDegate = m_tmAutoDegate.addSecs(-1);
    m_plcdTimerPeriod->display(m_tmAutoDegate.toString("hh:mm:ss"));
    if (m_tmAutoDegate == QTime(0, 0, 0, 0))
    {

        m_pTimerToDegating->stop();
        emit AcquireDegateIsRunningSig(bIsDegateRunning);
        if(bIsDegateRunning)
        {
            emit TempControlDegateSig(false);
        }

        emit AcquireAutoScanIsRunningSig(bIsAutoScanRunning);
        if(bIsAutoScanRunning)
        {
            emit AutoScanControlPauseSig(false); //暂停autoscan 倒计时
        }

        emit AcquireDegateDelaymsSig(delayms);
        OnUnblockAllChannelSlot(delayms);
        if(bIsDegateRunning)
        {
            emit TempControlDegateSig(true);
        }
        if(bIsAutoScanRunning)
        {
            emit AutoScanControlPauseSig(true); //恢复autoscan 倒计时
        }

        SetTimehms();
        m_pTimerToDegating->start(1000);
    }
    return;
}

void SensorPanel::OnAutoDegateBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pTimerAutoDegatinBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pTimerAutoDegatinBtn->setText(QStringLiteral("Stop"));
        if(!m_pTimerToDegating->isActive())
            m_pTimerToDegating->start(1000);
        SetTimehms();
    }
    else
    {
        m_pTimerAutoDegatinBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pTimerAutoDegatinBtn->setText(QStringLiteral("Auto unblock"));
        if(m_pTimerToDegating->isActive())
            m_pTimerToDegating->stop();
    }

}


bool SensorPanel::UpdateRegCacheByChState(std::vector<StateRegUnion> &vctSensorRegAry, const int &channel,const int &state)
{
    int quotient = channel/8;
    int remind = channel%8;

    if(quotient < SENSOR_STATE_REG_NUM && remind < 8)
    {
        switch (remind)
        {
        case 0:
            vctSensorRegAry[quotient].SensorState.ch0 = state &0x0f;
            break;
        case 1:
            vctSensorRegAry[quotient].SensorState.ch1 = state &0x0f;
            break;
        case 2:
            vctSensorRegAry[quotient].SensorState.ch2 = state &0x0f;
            break;
        case 3:
            vctSensorRegAry[quotient].SensorState.ch3 = state &0x0f;
            break;
        case 4:
            vctSensorRegAry[quotient].SensorState.ch4 = state &0x0f;
            break;
        case 5:
            vctSensorRegAry[quotient].SensorState.ch5 = state &0x0f;
            break;
        case 6:
            vctSensorRegAry[quotient].SensorState.ch6 = state &0x0f;
            break;
        case 7:
            vctSensorRegAry[quotient].SensorState.ch7 = state &0x0f;
            break;
        default:
            return false;
        }
    }
    else
    {
        LOGCI("{} cal the channel and location out of range!!!",__FUNCTION__);
        return false;
    }
    return true;
}


void SensorPanel::OneChanSensorSpeciSetSlot(int chan, int sensorState)
{
    std::future<bool> future = std::async(std::launch::async, [this, &chan, &sensorState]() {
        std::vector<StateRegUnion>& vctSensorRegAry = ConfigServer::GetInstance()->GetSensorStateRegCache();
        bool brawFlag = ConfigServer::GetInstance()->GetIsRawData();
        uint A0DataBefore = 0x00000000;
        uint A0DataAfter = 0x01000000;
        if (brawFlag)
        {
            A0DataBefore = 0x00800000;
            A0DataAfter = 0x01800000;
        }
        m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0, A0DataBefore);

        UpdateRegCacheByChState(vctSensorRegAry, chan, sensorState);

        int i = chan / 8;
            if (!m_pUsbFtdDevice->ASICRegisterWiteOne(0x20 + i, vctSensorRegAry[i].stateReg))
            {
                LOGE("ASIC REG write one FAILED!!!");
                return false;
        }
        m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0, A0DataAfter);
        return true;
        });
    std::future_status status;
    do {
        status = future.wait_for(std::chrono::milliseconds(1));
        QCoreApplication::processEvents();   //处理事件
    } while (status != std::future_status::ready);
//    LOGCI(" {} Async function result {}", __FUNCTION__, future.get());
}
void SensorPanel::SpeChanSensorSpeciSetSlot(int startch, int endch, const std::vector<int>& vctSensorState)
{
    std::future<bool> future = std::async(std::launch::async, [this, &startch, &endch, &vctSensorState]() {
        std::vector<StateRegUnion>& vctSensorRegAry = ConfigServer::GetInstance()->GetSensorStateRegCache();
        bool brawFlag = ConfigServer::GetInstance()->GetIsRawData();
        const uint A0DataBefore = (brawFlag ? 0x00800000: 0x00000000);
        const uint A0DataAfter = (brawFlag  ? 0x01800000: 0x01000000);

        m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0, A0DataBefore);
        for (int k = startch; k < endch; ++k)
        {
            UpdateRegCacheByChState(vctSensorRegAry, k, vctSensorState[k - startch]);
        }

        int regStart = startch / 8;
        if (regStart >= 128)
        {
            regStart = 127;
        }
        int regEnd = endch / 8;
        if (regEnd >= 128)
        {
            regEnd = 127;
        }
        for (int i = regStart; i <= regEnd; ++i)
        {
            if (!m_pUsbFtdDevice->ASICRegisterWiteOne(0x20 + i, vctSensorRegAry[i].stateReg))
            {
                LOGE("ASIC REG write one FAILED!!!");
                return false;
            }
        }
        m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0, A0DataAfter);
        return true;
        });

    std::future_status status;
    do {
        status = future.wait_for(std::chrono::milliseconds(1));
        QCoreApplication::processEvents();   //处理事件
    } while (status != std::future_status::ready);
//    LOGCI(" {} Async function result {}", __FUNCTION__, future.get());
}



void SensorPanel::AllChanSensorStateSetOffSlot(void)
{
    m_pSensorOffSelbtn->click();
}

void SensorPanel::EnableSimulationSlot(const bool &enable)
{
    if(enable)
    {
        if(!AllChSameFuncStateSet(SENSORGRP_FUNC_STATE_UNBLOCK))
        {
            LOGI("{} set function {} failed!!!",__FUNCTION__,CHANNEL_FUNCTION_STATE_STR[SENSORGRP_FUNC_STATE_UNBLOCK]);
        }
    }
    else
    {
        if(!AllChSameFuncStateSet(SENSORGRP_FUNC_STATE_OFF))
        {
            LOGI("{} set function {} failed!!!",__FUNCTION__,CHANNEL_FUNCTION_STATE_STR[SENSORGRP_FUNC_STATE_OFF]);
        }
    }
}


void SensorPanel::SensorGroupChangeSlot(const SENSOR_STATE_GROUP_ENUM &sensorGrp)
{
    switch (sensorGrp)
    {
    case SENSOR_STATE_A_GROUP:
        m_pSensorASelbtn->click();
        break;
    case SENSOR_STATE_B_GROUP:
        m_pSensorBSelbtn->click();
        break;
    case SENSOR_STATE_C_GROUP:
        m_pSensorCSelbtn->click();
        break;
    case SENSOR_STATE_D_GROUP:
        m_pSensorDSelbtn->click();
        break;
    default:
        return;
    }
#ifdef EASY_PROGRAM_DEBUGGING
        m_pSensorCurrSelbtn->click();
#endif
}



SENSOR_STATE_ENUM SensorPanel::GetSensorStateByFuncState(const SENSOR_STATE_GROUP_ENUM &sensorGrp,const SENSORGRP_FUNC_STATE_ENUM &funcState)
{
    switch (funcState)
    {
        case SENSORGRP_FUNC_STATE_NORMAL:
            return SENSORGRP_FUNCT_STATE_TRUNTO_SENSORSTATE[sensorGrp][SENSORGRP_FUNC_STATE_NORMAL];
        case SENSORGRP_FUNC_STATE_UNBLOCK:
            return SENSORGRP_FUNCT_STATE_TRUNTO_SENSORSTATE[sensorGrp][SENSORGRP_FUNC_STATE_UNBLOCK];
        case SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE:
            return SENSORGRP_FUNCT_STATE_TRUNTO_SENSORSTATE[sensorGrp][SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE];
        case SENSORGRP_FUNC_STATE_OFF:
            return SENSORGRP_FUNCT_STATE_TRUNTO_SENSORSTATE[sensorGrp][SENSORGRP_FUNC_STATE_OFF];
        default:
            break;
    }
    return SENSOR_STATE_OFF;
}



bool SensorPanel::AllChSameFuncStateSet(const SENSORGRP_FUNC_STATE_ENUM &funcState)
{
    return CustomChSameFuncStateSet(0, CHANNEL_NUM - 1, funcState);
}

bool SensorPanel::SingleChSameFuncStateSet(const int &ch, const SENSORGRP_FUNC_STATE_ENUM &funcState)
{
    return CustomChSameFuncStateSet(ch, ch, funcState);
}

bool SensorPanel::CustomChSameFuncStateSet(const int &startch,const int &endch,const SENSORGRP_FUNC_STATE_ENUM &funcState)
{
    if(startch >= CHANNEL_NUM || endch >= CHANNEL_NUM )
    {
        return false;
    }

    auto &vctallchFuncState =  m_pModelSharedDataPtr->GetAllChFuncState();
    int chDiffer = endch - startch + 1;
    std::fill_n(vctallchFuncState.begin()+startch,chDiffer,funcState);
    return CustomChDifferFuncStateSet(startch,endch, vctallchFuncState);
}


bool SensorPanel::AllChDifferFuncStateSet(const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState)
{
    return CustomChDifferFuncStateSet(0,CHANNEL_NUM - 1, vctfuncState);
}

bool SensorPanel::CustomChDifferFuncStateSet(const int &startch,const int &endch,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState)
{
    if(startch >= CHANNEL_NUM || endch >= CHANNEL_NUM || vctfuncState.size() != CHANNEL_NUM )
    {
        return false;
    }

    auto &vctSensorRegAry =  ConfigServer::GetInstance()->GetSensorStateRegCache();
    const auto &vctAllLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    const auto &vctAllChSensorGroup =  m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctSensorState =  m_pModelSharedDataPtr->GetSensorStateData();
    auto &vctallchFuncState =  m_pModelSharedDataPtr->GetAllChFuncState();

    SENSOR_STATE_ENUM sensorState = SENSOR_STATE_OFF;
    SENSORGRP_FUNC_STATE_ENUM funcstate = SENSORGRP_FUNC_STATE_OFF;
    for(int ch = startch; ch <= endch; ++ch)
    {
        const auto &chSensorGrp = vctAllChSensorGroup[ch];
        if(vctAllLockState[chSensorGrp][ch] != PROHIBITED_OPERATION_STATE) //要是没有被锁定，则根据功能判断sensor设置
        {
            sensorState = GetSensorStateByFuncState(chSensorGrp,vctfuncState[ch]);
            funcstate = vctfuncState[ch];
        }
        else //要是被锁定，则功能置为off,sensor置为off
        {
            funcstate = SENSORGRP_FUNC_STATE_OFF;
            sensorState = SENSOR_STATE_OFF;
        }
        UpdateRegCacheByChState(vctSensorRegAry,ch,sensorState);
        vctSensorState[ch] = sensorState;
        vctallchFuncState[ch] = funcstate;
    }
    return SensorStateCommandSendToFPGA(startch,endch);
}

/**
 * @brief SensorPanel::IncontinueChDifferFuncStateSet 不连续通道功能状态设置
 * @param vctIncoherentCh  对应的通道 eg: vctIncoherentCh{1,3,5,9,20,121}
 * @param vctIncoherentState 对应通道的功能状态,与 vctIncoherentCh一一对应
 * @return 是否成功
 */
bool SensorPanel::IncontinueChDifferFuncStateSet(const std::vector<int> &vctIncoherentCh,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctIncoherentState)
{
    if(vctIncoherentCh.size() != vctIncoherentState.size())
    {
        return false;
    }

    auto &vctSensorRegAry =  ConfigServer::GetInstance()->GetSensorStateRegCache();
    const auto &vctAllLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    const auto &vctAllChSensorGroup =  m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctSensorState =  m_pModelSharedDataPtr->GetSensorStateData();
    auto &vctallchFuncState =  m_pModelSharedDataPtr->GetAllChFuncState();
    int size = (int)vctIncoherentCh.size();

    SENSOR_STATE_ENUM sensorState = SENSOR_STATE_OFF;
    SENSORGRP_FUNC_STATE_ENUM funcstate = SENSORGRP_FUNC_STATE_OFF;
    for(int i = 0; i < size; ++i)
    {
        const int &ch  = vctIncoherentCh[i];
        const auto &chSensorGrp = vctAllChSensorGroup[ch];
        if(vctAllLockState[chSensorGrp][ch] != PROHIBITED_OPERATION_STATE)
        {
            funcstate = vctIncoherentState[i];
            sensorState = GetSensorStateByFuncState(chSensorGrp,funcstate);
        }
        else
        {
            funcstate = SENSORGRP_FUNC_STATE_OFF;
            sensorState = SENSOR_STATE_OFF;
        }
        UpdateRegCacheByChState(vctSensorRegAry,ch,sensorState);
        vctSensorState[ch] = sensorState;
        vctallchFuncState[ch] =  funcstate;
    }
    return IncontinueSensorStateCommandSendToFPGA(vctIncoherentCh);
}


/**
 * @brief SensorPanel::SensorStateCommandSendToFPGA 连续通道发送命令
 * @param startch 起始通道
 * @param endch 终止通道
 * @return  是否成功
 */
bool SensorPanel::SensorStateCommandSendToFPGA(const int &startch,const int &endch)
{
    std::future<bool> future = std::async(std::launch::async, [this,&startch,&endch]()
    {
        const auto &vctSensorRegAry =  ConfigServer::GetInstance()->GetSensorStateRegCache();
        bool brawFlag = ConfigServer::GetInstance()->GetIsRawData();
        const uint A0DataBefore = (brawFlag ? 0x00800000: 0x00000000);
        const uint A0DataAfter = (brawFlag  ? 0x01800000: 0x01000000);
        int regStart = startch/8;
        int regEnd = endch/8;

        m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0,A0DataBefore);
        for(int i = regStart; i<= regEnd; ++i)
        {
            if(!m_pUsbFtdDevice->ASICRegisterWiteOne(0x20+i,vctSensorRegAry[i].stateReg))
            {
                return false;
            }
        }
        return m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0,A0DataAfter);
    });

    std::future_status status;
    do {
        status = future.wait_for(std::chrono::milliseconds(1));
        QCoreApplication::processEvents();   //处理事件
    } while (status != std::future_status::ready);

    return future.get();
}

/**
 * @brief SensorPanel::IncontinueSensorStateCommandSendToFPGA  不连续通道发送命令
 * @param vctIncoherentCh 具体通道值 eg: vctIncoherentCh{1,3,5,9,20,121}
 * @return 是否成功
 */
bool SensorPanel::IncontinueSensorStateCommandSendToFPGA(const std::vector<int> &vctIncoherentCh)
{
    std::future<bool> future = std::async(std::launch::async, [this,&vctIncoherentCh]()
    {
        const auto &vctSensorRegAry =  ConfigServer::GetInstance()->GetSensorStateRegCache();
        bool brawFlag = ConfigServer::GetInstance()->GetIsRawData();
        const uint A0DataBefore = (brawFlag ? 0x00800000: 0x00000000);
        const uint A0DataAfter = (brawFlag  ? 0x01800000: 0x01000000);

        int regOffset  = 0;
        int ch =0;
        int size  = (int)vctIncoherentCh.size();

        m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0,A0DataBefore);
        for(int i =0; i < size ; ++i)
        {
            ch = vctIncoherentCh[i];
            regOffset = ch/8;
            if(!m_pUsbFtdDevice->ASICRegisterWiteOne(0x20+regOffset,vctSensorRegAry[regOffset].stateReg))
            {
                return false;
            }
        }
        return m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0,A0DataAfter);
    });

    std::future_status status;
    do {
        status = future.wait_for(std::chrono::milliseconds(1));
        QCoreApplication::processEvents();
    } while (status != std::future_status::ready);
    return future.get();
}


void SensorPanel::OnAllSensorSelectModeChgeBtnSlot(const int &buttonId)
{
    ALL_SENSOR_STATE_SELECT_ENUM sensorSelectMode = (ALL_SENSOR_STATE_SELECT_ENUM)buttonId;

    auto &vctAllChSensorGroup =  m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFuncState =  m_pModelSharedDataPtr->GetAllChFuncState();
    static SENSOR_STATE_GROUP_ENUM lastSensorGroup = SENSOR_STATE_A_GROUP;
    SENSORGRP_FUNC_STATE_ENUM allChFuncState = SENSORGRP_FUNC_STATE_NORMAL;

    switch (sensorSelectMode) {
    case ALL_SENSOR_STATE_A_SELECT:
    {
        allChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,allChFuncState);
        std::fill_n(vctAllChSensorGroup.begin(),CHANNEL_NUM,SENSOR_STATE_A_GROUP);
        lastSensorGroup = SENSOR_STATE_A_GROUP;
        break;
    }
    case ALL_SENSOR_STATE_B_SELECT:
    {
        allChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,allChFuncState);
        std::fill_n(vctAllChSensorGroup.begin(),CHANNEL_NUM,SENSOR_STATE_B_GROUP);
        lastSensorGroup = SENSOR_STATE_B_GROUP;
        break;
    }
    case ALL_SENSOR_STATE_C_SELECT:
    {
        allChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,allChFuncState);
        std::fill_n(vctAllChSensorGroup.begin(),CHANNEL_NUM,SENSOR_STATE_C_GROUP);
        lastSensorGroup = SENSOR_STATE_C_GROUP;
        break;
    }
    case ALL_SENSOR_STATE_D_SELECT:
    {
        allChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,allChFuncState);
        std::fill_n(vctAllChSensorGroup.begin(),CHANNEL_NUM,SENSOR_STATE_D_GROUP);
        lastSensorGroup = SENSOR_STATE_D_GROUP;
        break;
    }
    case ALL_SENSOR_STATE_OFF_SELECT:
    {
        allChFuncState = SENSORGRP_FUNC_STATE_OFF;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,allChFuncState);
        break;
    }
    case ALL_SENSOR_STATE_INTERNAL_CURRENT_SELECT:
    {
        allChFuncState = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,allChFuncState);
        break;
    }
    case ALL_SENSOR_STATE_MUX_SELECT:
    {
        const auto &vctAllChMuxGrpState  = m_pModelSharedDataPtr->GetAllChMuxStateRef();
        auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
        std::copy(vctAllChMuxGrpState.cbegin(),vctAllChMuxGrpState.cend(),vctAllChSensorGrp.begin());
#ifdef EASY_PROGRAM_DEBUGGING
        allChFuncState = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE);
#else
        allChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,SENSORGRP_FUNC_STATE_NORMAL);
#endif
        break;
    }
    default:
        return;
    }

    emit UpdateCurrentSensorGrpStrSig(QString("Current group: <font color=blue><b> %1 </b></font>").arg(SENSRO_SELECT_MODE_STR[sensorSelectMode]));
    if(!AllChSameFuncStateSet(allChFuncState))
    {
        LOGI("{} Set all channel to {} failed!!!!",__FUNCTION__,CHANNEL_FUNCTION_STATE_STR[allChFuncState]);
    }
    emit SigClearPoreAndCapState();
}


void SensorPanel::SensorGrpCustomSelctBtnSlot(void)
{
    int startch = m_pStartChpSelSpbox->value() -1;
    int endch = m_pEndChSelSpbox->value() -1 ;
    SENSORGRP_FUNC_STATE_ENUM funcState = SENSORGRP_FUNC_STATE_ENUM(m_pSensorFuncStateComb->currentData().toInt());

    if(!CustomChSameFuncStateSet(startch,endch,funcState))
    {
        LOGI("{} Set custom channel to {} failed!!!!",__FUNCTION__,CHANNEL_FUNCTION_STATE_STR[funcState]);
    }
}

void SensorPanel::SpecificChanSetFunctStateSlot(const int &ch,const SENSORGRP_FUNC_STATE_ENUM &funcState)
{
     if(!SingleChSameFuncStateSet(ch, funcState))
     {
         LOGI("Chanel {} set function {} failed!!!",ch+1,CHANNEL_FUNCTION_STATE_STR[funcState]);
     }
}

void SensorPanel::AllChDifferFuncStateSetSlot(const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState)
{
    if(!AllChDifferFuncStateSet(vctfuncState))
    {
        LOGI("{} Set all channel different funcstate failed!!!!",__FUNCTION__);
    }
}

void SensorPanel::IncontinueChDifferFuncStateSetSlot(const std::vector<int> &vctIncoherentCh,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctIncoherentState)
{
    if(!IncontinueChDifferFuncStateSet(vctIncoherentCh,vctIncoherentState))
    {
        LOGI("{} Set incontinue channel different funcstate failed!!!!",__FUNCTION__);
    }
}

void SensorPanel::IncontinueChSameFuncStateSetSlot(const std::vector<int> &vctIncoherentCh,const SENSORGRP_FUNC_STATE_ENUM &funcState)
{
    std::vector<SENSORGRP_FUNC_STATE_ENUM> vctIncoherentState(vctIncoherentCh.size(),funcState);
    if(!IncontinueChDifferFuncStateSet(vctIncoherentCh,vctIncoherentState))
    {
        LOGI("{} Set incontinue channel different funcstate failed!!!!",__FUNCTION__);
    }
}

void SensorPanel::SensorGroupAndFuncChangeSlot(const SENSOR_STATE_GROUP_ENUM &sensorGrp,const SENSORGRP_FUNC_STATE_ENUM &allChFuncState)
{
    auto &vctAllChSensorGroup =  m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFuncState =  m_pModelSharedDataPtr->GetAllChFuncState();

    std::fill_n(vctAllChFuncState.begin(),CHANNEL_NUM,allChFuncState);
    std::fill_n(vctAllChSensorGroup.begin(),CHANNEL_NUM,sensorGrp);

    emit UpdateCurrentSensorGrpStrSig(QString("Current group: <font color=blue><b> %1 </b></font>").arg(SENSRO_SELECT_MODE_STR[sensorGrp]));
    if(!AllChSameFuncStateSet(allChFuncState))
    {
        LOGI("{} Set all channel to {} failed!!!!",__FUNCTION__,CHANNEL_FUNCTION_STATE_STR[allChFuncState]);
    }
}

void SensorPanel::OnMuxModeSet(void)
{
    m_pSensorMuxSelbtn->click();
}


void SensorPanel::AutoTimerDegatControlSlot(const bool &runOrPause)
{
    if(runOrPause) //true  control run
    {
        if(m_pTimerAutoDegatinBtn->isChecked() && !m_pTimerToDegating->isActive())
        {
            m_pTimerToDegating->start(1000);  //continue
        }
    }
    else
    {
        if(m_pTimerAutoDegatinBtn->isChecked() && m_pTimerToDegating->isActive())
        {
            m_pTimerToDegating->stop();  //continue
        }

    }
}
void SensorPanel::TimerDegatEanbleSlot(const bool &enable)
{
    if(enable)
    {
        if(!m_pTimerAutoDegatinBtn->isChecked())
        {
            m_pTimerAutoDegatinBtn->click();
        }
    }
    else
    {
        if(m_pTimerAutoDegatinBtn->isChecked())
        {
            m_pTimerAutoDegatinBtn->click();
        }
    }
}
void SensorPanel::AcquireAutoTimerDegatIsRunningSlot(bool &bIsRunning)
{
    bIsRunning = (m_pTimerAutoDegatinBtn->isChecked() && m_pTimerToDegating->isActive());
}
void SensorPanel::RecordDegateLog(const std::vector<int> &vctIncoherentCh,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctIncoherentState)
{
    if(vctIncoherentCh.size() != vctIncoherentState.size())
        return;
    const auto &vctSaveFileIndex = m_pSavePannel->GetCurSaveDataIndex();
    const auto &vctSensorState =  m_pModelSharedDataPtr->GetSensorStateData();
    auto &vctDegateResult =  m_pModelSharedDataPtr->GetAllChDegateStateData();
    int size = (int)vctIncoherentCh.size();
    for(int i = 0; i < size; i++)
    {
        int ch = vctIncoherentCh[i];
        if(vctDegateResult[ch] == RECORD_IS_DEGATE_STATE)
        {
            vctDegateResult[ch] = RECORD_NONE_DEGATE_STATE;
            LOGDEI("Ch{},{},{},{}",ch+1,vctSaveFileIndex[ch],gSensorStateStr[vctSensorState[ch]],CHANNEL_FUNCTION_STATE_STR[vctIncoherentState[i]]);
        }
    }
    LOGDGATEFLUSH();
}

/**
 * @brief 所有通道设置为All internal source
 * @return
 */
bool SensorPanel::AllChannelAllInternalSourceSet(void)
{
    auto &vctSensorRegAry =  ConfigServer::GetInstance()->GetSensorStateRegCache();
    const auto &vctAllLockState = m_pModelSharedDataPtr->GetFourLockStateData();
    const auto &vctAllChSensorGroup =  m_pModelSharedDataPtr->GetAllChSensorGroup();

    SENSOR_STATE_ENUM sensorState = SENSOR_STATE_OFF;
    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        const auto &chSensorGrp = vctAllChSensorGroup[ch];
        if(vctAllLockState[chSensorGrp][ch] != PROHIBITED_OPERATION_STATE) //要是没有被锁定，则根据功能判断sensor设置
        {
            sensorState = SENSOR_STATE_ALL_INTERNAL_CURRENT;
        }
        else //要是被锁定，则功能置为off,sensor置为off
        {
            sensorState = SENSOR_STATE_OFF;
        }
        UpdateRegCacheByChState(vctSensorRegAry,ch,sensorState);
    }
    return SensorStateCommandSendToFPGA(0,1023);
}
