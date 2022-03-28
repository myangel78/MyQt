#pragma once

#include <QWidget>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qtimer.h>

#include "ConfigServer.h"


class ControlListItem : public QWidget
{
    Q_OBJECT
public:
    explicit ControlListItem(int chan, const QColor& color, QWidget *parent = nullptr);
    ~ControlListItem();

    enum EGatingState
    {
        EGS_NORMAL,
        EGS_GATING,
        EGS_INACTIVE,

        EGS_MAX
    };

private:
    QCheckBox* m_pchkChannel;
    QCheckBox* m_pchkChanSeq;
    QToolButton* m_pbtnChannel;
    QLabel* m_plabCurCurrent;
    QLabel* m_plabAvgCurrent;
    QLabel* m_plabStdCurrent;
    //QCheckBox* m_pchkAutoZero;
    QToolButton* m_pbtnStatus;

    QCheckBox* m_pchkChanVolt;
    //QToolButton* m_pbtnPoreState;
    QPushButton* m_pbtnOpposite;
    QDoubleSpinBox* m_pdspVoltAmpl;
    QPushButton* m_pbtnApply;
    //QCheckBox* m_pchkAutoDegating;
    QToolButton* m_pbtnDegatingColor;
    QLabel* m_plabDegatingCount;
    //QToolButton* m_pbtnDegatingCount;

    int m_iChannel;
    QColor m_clrChannel;
    QColor m_clrDegating;


    bool m_bPorePortect = false;
    bool m_bZeroSetted = false;
    //int m_iPoreStatusCount = 2;
    //double m_dSingleLimit = 100.0;
    //double m_dMultiLimit = 400.0;
    //double m_dSaturated = 1000.0;
    double m_dProtectVolt = 0.05;
    EEmbedPoreStatus m_phsHoleStatus = EEmbedPoreStatus::EPS_NONEPORE;
    EEmbedPoreStatus m_phsCurHoleStatus = EEmbedPoreStatus::EPS_NONEPORE;
    //EEmbedPoreStatus m_phsLastHoleStatus = EEmbedPoreStatus::EPS_NONEPORE;
    //std::map<PoreHoleStatus, int> m_mapHoleStatusCount;
    int m_iHoleStatueCount = 0;


    //bool m_bPoreState = false;
    EGatingState m_eGatingState = EGS_NORMAL;
    int m_iLimitCheckTime = 0;
    int m_iCurTimes = 0;
    int m_iLimitCycle = 14;
    int m_iAllowTimes = 10;
    int m_iSteppingTime = 0;
    int m_iDurationTime = 200;
    int m_iCyclePeriod = 2;
    double m_dDegatingVolt = -0.1;
    double m_dSequencingVolt = 0.18;
    QTimer* m_ptmrPollingDegating;
    QTimer* m_ptmrDurationDegating;
    QTimer* m_ptmrLimitCheckDegating;

    std::atomic<bool> m_bIsDegating = false;
    std::atomic<bool> m_bValidChannel = true;
    std::atomic<bool> m_bPoreFilterChannel = false;

private:
    bool InitCtrl(int chan, const QColor& color);

    void SetChannelDCLevel(double volt);

    void DoClickDegatingColor(void);

    bool CloseChannelWhileReachAllowTime(void);

public:
    bool GetIsDegating(void) const { return m_bIsDegating; }
    void SetIsDegating(bool val) { m_bIsDegating = val; }
    bool GetValidChannel(void) const { return m_bValidChannel; }
    void SetValidChannel(bool val) { m_bValidChannel = val; }
    bool GetPoreFilterChannel(void) const { return m_bPoreFilterChannel; }
    void SetPoreFilterChannel(bool val) { m_bPoreFilterChannel = val; }

    bool GetChecked(void) const { return m_pchkChannel->isChecked(); }
    void SetChecked(bool chk) { m_pchkChannel->setChecked(chk); }

    bool GetCheckedSeq(void) const { return m_pchkChanSeq->isChecked(); }
    void SetCheckedSeq(bool chk) { m_pchkChanSeq->setChecked(chk); }

    QColor GetColor(void) const { return m_clrChannel; }
    void SetColor(const QColor& color);

    QString GetCurCurrent(void) const { return m_plabCurCurrent->text(); }
    void SetCurCurrent(const QString& txt) { m_plabCurCurrent->setText(txt); }
    QString GetAvgCurrent(void) const { return m_plabAvgCurrent->text(); }
    void SetAvgCurrent(const QString& txt) { m_plabAvgCurrent->setText(txt); }
    QString GetStdCurrent(void) const { return m_plabStdCurrent->text(); }
    void SetStdCurrent(const QString& txt) { m_plabStdCurrent->setText(txt); }


    void SetPorePortect(bool val)
    {
        m_bPorePortect = val;
    }
    //void SetPoreLimitVolt(int porecnt, double sing, double mult, double satu)
    //{
    //    m_iPoreStatusCount = porecnt;
    //    m_dSingleLimit = sing;
    //    m_dMultiLimit = mult;
    //    m_dSaturated = satu;
    //}

    //void SetAutoZeroCheck(bool chk) { m_pchkAutoZero->setChecked(chk); }
 
    void SetEnableAutoZero(bool chk)
    {
        //m_pchkAutoZero->setEnabled(chk);
        m_pbtnStatus->setEnabled(chk);
    }

    bool GetProtectPore(void) const { return m_bZeroSetted; }

    void SetStausStyleSheet(const QString& style)
    {
        m_pbtnStatus->setStyleSheet(style);
    }

    double GetProtectVolt(void) const { return m_dProtectVolt; }
    void SetProtectVolt(double val) { m_dProtectVolt = val; }

    void SetSteadyPoreStatus(EEmbedPoreStatus porestatus)
    {
        m_bZeroSetted = true;
        m_phsHoleStatus = porestatus;
        m_pbtnStatus->setStyleSheet(g_colorStyle[porestatus]);//g_borderStyle[1] +
        m_pchkChannel->setChecked(false);
        m_pchkChanSeq->setChecked(false);
        m_pdspVoltAmpl->setValue(0.0);
        SetChannelDCLevel(0.0);
    }

    EEmbedPoreStatus GetHoleStatus(void) const { return m_phsHoleStatus; }

    //void SetHoleLastStatus(void)
    //{
    //    m_bZeroSetted = true;
    //    if (m_phsLastHoleStatus != EEmbedPoreStatus::EPS_NONEPORE)
    //    {
    //        m_phsHoleStatus = m_phsLastHoleStatus;
    //        m_pbtnStatus->setStyleSheet(g_colorStyle[m_phsHoleStatus]);
    //    }
    //}

    void SetHoleStatus(int porecnt, double single, double multi, double saturate, double val)
    //void SetHoleStatus(double val)
    {
        if (m_bZeroSetted || (m_eGatingState == EGatingState::EGS_INACTIVE))
        {
            return;
        }
        if (val > saturate)//m_dSaturated
        {
            if (m_phsCurHoleStatus != EEmbedPoreStatus::EPS_SATURATEDPORE)
            {
                m_phsCurHoleStatus = EEmbedPoreStatus::EPS_SATURATEDPORE;
                m_iHoleStatueCount = 1;
            }
            else
            {
                ++m_iHoleStatueCount;
            }
        }
        else if (val > multi)//m_dMultiLimit
        {
            if (m_phsCurHoleStatus != EEmbedPoreStatus::EPS_MULTIPORE)
            {
                m_phsCurHoleStatus = EEmbedPoreStatus::EPS_MULTIPORE;
                m_iHoleStatueCount = 1;
            }
            else
            {
                ++m_iHoleStatueCount;
            }
        }
        else if (val > single)//m_dSingleLimit
        {
            if (m_phsCurHoleStatus != EEmbedPoreStatus::EPS_SINGLEPORE)
            {
                m_phsCurHoleStatus = EEmbedPoreStatus::EPS_SINGLEPORE;
                m_iHoleStatueCount = 1;
            }
            else
            {
                ++m_iHoleStatueCount;
            }
        }
        else
        {
            if (!m_bPorePortect)
            {
                if (m_phsCurHoleStatus != EEmbedPoreStatus::EPS_NONEPORE)
                {
                    m_phsCurHoleStatus = EEmbedPoreStatus::EPS_NONEPORE;
                    m_iHoleStatueCount = 1;
                }
                else
                {
                    ++m_iHoleStatueCount;
                }
            }
        }

        if (m_iHoleStatueCount >= porecnt)//m_iPoreStatusCount
        {
            m_iHoleStatueCount = 0;
            m_phsHoleStatus = m_phsCurHoleStatus;
            m_pbtnStatus->setStyleSheet(g_colorStyle[m_phsHoleStatus]);
        }
    }

    void ResetHoleStatus(void)
    {
        m_bZeroSetted = false;
        m_phsHoleStatus = EEmbedPoreStatus::EPS_NONEPORE;
        m_pbtnStatus->setStyleSheet(g_colorStyle[EEmbedPoreStatus::EPS_NONEPORE]);
    }

    void AutoSetStatusChange(void)
    {
        if (/*m_pchkAutoZero->isChecked() &&*/ !m_bZeroSetted && (m_phsHoleStatus != EEmbedPoreStatus::EPS_NONEPORE))
        {
            onClickStatusChange();
        }
    }

    bool GetChecked2(void) const { return m_pchkChanVolt->isChecked(); }
    void SetChecked2(bool chk) { m_pchkChanVolt->setChecked(chk); }

    void SetCheck2Enable(bool enab) { m_pchkChanVolt->setEnabled(enab); }

    void SetVoltEnable(bool enab) { m_pdspVoltAmpl->setEnabled(enab); }

    //void SetDegatingEnable(bool enable) { m_pchkAutoDegating->setEnabled(enable); }

    //bool GetDegatingChecked(void) const { return m_pchkAutoDegating->isChecked(); }
    //void SetDegatingChecked(bool chk) { m_pchkAutoDegating->setChecked(chk); }

    QString GetDegatingCount(void) const { return m_plabDegatingCount->text(); }
    void SetDegatingCount(const QString& txt) { m_plabDegatingCount->setText(txt); }
    //QString GetDegatingCount(void) const { return m_pbtnDegatingCount->text(); }
    //void SetDegatingCount(const QString& txt) { m_pbtnDegatingCount->setText(txt); }

    //bool GetPoreState(void) const { return m_bPoreState; }

    EGatingState GetDegatingSate(void) const { return m_eGatingState; }
    void SetDegatingState(EGatingState state)
    {
        emit degatingStateColorSignal((int)state);
    }
    void SetPollingDegatingState(EGatingState state)
    {
#if 1
        emit degatingStatePollingSignal((int)state);
#else
        emit degatingStateColorSignal((int)state);

        if (m_pchkChanSeq->isChecked() && (state == EGatingState::EGS_GATING))
        {
            if (!ConfigServer::GetInstance()->GetZeroAdjustProcess())
            {
                DoClickDegatingColor();
            }
        }
#endif
    }


    double GetVoltAmpl(void) const { return m_pdspVoltAmpl->value(); }
    void SetVoltAmpl(double ampl) { m_pdspVoltAmpl->setValue(ampl); }


    void SetDegatingParam(double degatingVolt, int cyclePeriod, int durTime, int stepTime, int allowTimes, int limitCycle, double seqvolt = 0.18)
    {
        m_iLimitCycle = limitCycle;
        m_iAllowTimes = allowTimes;
        m_iSteppingTime = stepTime;
        m_iDurationTime = durTime;
        m_iCyclePeriod = cyclePeriod * 1000;
        m_dDegatingVolt = degatingVolt;
        m_dSequencingVolt = seqvolt;
        m_iLimitCheckTime = limitCycle * m_iCyclePeriod;
    }

    void SetDegatingLimitTimer(void)
    {
        if (m_iAllowTimes > 0)
        {
            m_ptmrLimitCheckDegating->start(m_iLimitCheckTime);
        }
    }

    void SetDegatingTimer(void)
    {
        m_ptmrPollingDegating->start(m_iCyclePeriod);
        if (m_iAllowTimes > 0)
        {
            m_ptmrLimitCheckDegating->start(m_iLimitCheckTime);
        }
    }

    void ResetDegatingState(void)
    {
        if (m_ptmrPollingDegating->isActive())
        {
            m_ptmrPollingDegating->stop();
        }
        if (m_ptmrLimitCheckDegating->isActive())
        {
            m_ptmrLimitCheckDegating->stop();
        }
        if (m_ptmrDurationDegating->isActive())
        {
            m_ptmrDurationDegating->stop();
        }
        //if (m_eGatingState != EGatingState::EGS_INACTIVE)
        {
            SetDegatingState(EGatingState::EGS_NORMAL);
            m_iCurTimes = 0;
            m_plabDegatingCount->setText("0");
        }
    }

    void ResetMuxScanDegatingState(void)
    {
        if (m_ptmrPollingDegating->isActive())
        {
            m_ptmrPollingDegating->stop();
        }
        if (m_ptmrLimitCheckDegating->isActive())
        {
            m_ptmrLimitCheckDegating->stop();
        }
        if (m_ptmrDurationDegating->isActive())
        {
            m_ptmrDurationDegating->stop();
        }
        SetDegatingState(EGatingState::EGS_NORMAL);
        m_iCurTimes = 0;
        m_plabDegatingCount->setText("0");
    }

    void ResetDegatingTimer(void)
    {
        if (m_ptmrPollingDegating->isActive())
        {
            m_ptmrPollingDegating->stop();
        }
        if (m_ptmrLimitCheckDegating->isActive())
        {
            m_ptmrLimitCheckDegating->stop();
        }
    }

public slots:
    void onCheckChannelChange(bool chk);
    void onClickChannelChange(void);
    void onClickStatusChange(void);


    //void onClickPoreStateChange(bool chk);
    void onClickVoltageOpposite(void);
    void onClickVoltageApply(void);

    //void onCheckAutoDegating(bool chk);
    void onClickDegatingColor(void);

    void onClickResetDegating(void);

    void degatingStateColorSlot(int state);
    void degatingStatePollingSlot(int state);

    void timerPollingDegating(void);
    void timerDurationDegating(void);
    void timerLimitCheckDegating(void);

signals:
    void checkChangedSignal(int chan, bool chk);
    void colorChangedSignal(int chan, const QColor& clr);


    //void setZeroVoltSignal(int chan, int stat);


    void degatingStateColorSignal(int state);
    void degatingStatePollingSignal(int state);


    void UpdateListItemSignal(void);
    void SetChannelDCLevelSignal(int chan, double volt);
    //void transmitUsbData(ushort addr, uint data);
    void singleChanDegatingSignal(int chan, double volt);
    void singleChanDegatingRecoverSignal(int chan);
    //void getSequencingVoltageSignal(double& val);
};

