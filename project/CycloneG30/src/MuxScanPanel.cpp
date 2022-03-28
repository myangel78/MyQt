#include "MuxScanPanel.h"
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QCheckBox>
#include <QMessageBox>
#include <QProgressDialog>
#include "ConfigServer.h"
#include "Log.h"

#include "DegatingPane.h"
#include "DataCalculate.h"
#include "PoreStateMapWdgt.h"
#include "MyQMessageBox.h"
#include "CustomPlotWidgt.h"
#include "PoreStateMapTabModel.h"


MuxScanPanel::MuxScanPanel(DegatingPane * pDegatingPane,QWidget *parent):QWidget(parent),m_pDegatingPane(pDegatingPane)
{
    InitCtrl();
}

MuxScanPanel::~MuxScanPanel()
{

}

void MuxScanPanel::InitCtrl(void)
{
    m_iCycleCount = 0;
    m_bIsNextContinue = true;

    QGroupBox *pStepOneGroupBox = new QGroupBox(QStringLiteral("Pore QC"),this);
    QGroupBox *pStepTwoGroupBox = new QGroupBox(QStringLiteral("Seq"),this);
    QGroupBox *pStepThreeGroupBox = new QGroupBox(QStringLiteral("Period"),this);

    QLabel *plabBackward = new QLabel(QStringLiteral("Backward"),this);
    QLabel *plabPoreQc = new QLabel(QStringLiteral("PoreQC"),this);
    QLabel *plabPoreQcVolt = new QLabel(QStringLiteral("Volt"),this);
    QLabel *plabPoreQcDuration = new QLabel(QStringLiteral("Duration"),this);
    QLabel *plabFilterCur = new QLabel(QStringLiteral("Current"),this);
    QLabel *plabFilterCurMin = new QLabel(QStringLiteral("Min"),this);
    QLabel *plabFilterCurMax = new QLabel(QStringLiteral("Max"),this);
    QLabel *plabFilterStd = new QLabel(QStringLiteral("Std"),this);
    QLabel *plabSeqDesc = new QLabel(QStringLiteral("Seq"),this);
    QLabel *plabZeroVoltDesc = new QLabel(QStringLiteral("ZeroVolt"),this);
    QLabel *plabZeroVolt = new QLabel(QStringLiteral("Volt"),this);
    QLabel *plabZeroDuration = new QLabel(QStringLiteral("Duration"),this);
    QLabel *plabCycleInp = new QLabel(QStringLiteral("Cycle"),this);
    QLabel *plabTimerPeriod = new QLabel(QStringLiteral("Timer Period"),this);

    m_pBackVoltDpbx= new QDoubleSpinBox(this);
    m_pBackVoltDurationDpbx= new QDoubleSpinBox(this);
    m_pPoreQcVoltDpbx= new QDoubleSpinBox(this);
    m_pPoreQcDurationDpbx= new QDoubleSpinBox(this);
    m_pFilterCurMinDpbx= new QDoubleSpinBox(this);
    m_pFilterCurMaxDpbx= new QDoubleSpinBox(this);
    m_pFilterStdMinDpbx= new QDoubleSpinBox(this);
    m_pFilterStdMaxDpbx= new QDoubleSpinBox(this);
    m_pZeroVoltDpbx=  new QDoubleSpinBox(this);
    m_pZeroDurationDpbx=  new QDoubleSpinBox(this);
    m_pSeqVoltDpbx= new QDoubleSpinBox(this);
    m_pSeqDurationDpbx= new QDoubleSpinBox(this);
    m_pScanCycleDpbx= new QDoubleSpinBox(this);
    m_pCyclePeriodDpbx= new QDoubleSpinBox(this);

    m_pDegateEnableChk = new QCheckBox(QStringLiteral("Enable"),this);


    m_plcdTimerPeriod = new QLCDNumber(8, this);
    m_plcdTimerPeriod->setSegmentStyle(QLCDNumber::Flat);
    m_plcdTimerPeriod->setFrameShape(QFrame::StyledPanel);

    m_pStartMuxScanBtn = new QPushButton(QIcon(":/img/img/start.ico"),QStringLiteral("Start"),this);
    m_pNextStepBtn = new QPushButton("Next", this);
    m_pDegatePanlSetBtn = new QPushButton(QIcon(":/img/img/configuration.png"),QStringLiteral("Degate setting..."), this);
    m_pDegateStartBtn = new QPushButton(QIcon(":/img/img/start.ico"),QStringLiteral("Start"),this);
    m_pDegateStartBtn->setCheckable(true);
    m_pStartMuxScanBtn->setCheckable(true);

    m_pBackVoltDpbx->setDecimals(2);
    m_pBackVoltDpbx->setSuffix(" V");
    m_pBackVoltDpbx->setRange(-3.2,0);
    m_pBackVoltDpbx->setValue(-0.1);

    m_pBackVoltDurationDpbx->setDecimals(2);
    m_pBackVoltDurationDpbx->setSuffix(" s");
    m_pBackVoltDurationDpbx->setMaximum(10.0);
    m_pBackVoltDurationDpbx->setValue(1.0);

    m_pPoreQcVoltDpbx->setDecimals(2);
    m_pPoreQcVoltDpbx->setSuffix(" V");
    m_pPoreQcVoltDpbx->setRange(0,3.2);
    m_pPoreQcVoltDpbx->setValue(0.05);

    m_pPoreQcDurationDpbx->setDecimals(2);
    m_pPoreQcDurationDpbx->setSuffix(" s");
    m_pPoreQcDurationDpbx->setMaximum(10.0);
    m_pPoreQcDurationDpbx->setValue(5.0);

    m_pFilterCurMinDpbx->setDecimals(2);
    m_pFilterCurMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterCurMinDpbx->setMinimum(-999999.99);
    m_pFilterCurMinDpbx->setMaximum(999999.99);
    m_pFilterCurMinDpbx->setValue(10.0);

    m_pFilterCurMaxDpbx->setDecimals(2);
    m_pFilterCurMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterCurMaxDpbx->setMinimum(-999999.99);
    m_pFilterCurMaxDpbx->setMaximum(999999.99);
    m_pFilterCurMaxDpbx->setValue(1000.0);

    m_pFilterStdMinDpbx->setDecimals(4);
    m_pFilterStdMinDpbx->setMinimum(-999.9999);
    m_pFilterStdMinDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterStdMinDpbx->setMaximum(999.9999);
    m_pFilterStdMinDpbx->setValue(0.0001);

    m_pFilterStdMaxDpbx->setDecimals(4);
    m_pFilterStdMaxDpbx->setMinimum(-999.9999);
    m_pFilterStdMaxDpbx->setSuffix(QStringLiteral(" pA"));
    m_pFilterStdMaxDpbx->setMaximum(999.9999);
    m_pFilterStdMaxDpbx->setValue(30);

    m_pZeroVoltDpbx->setDecimals(2);
    m_pZeroVoltDpbx->setSuffix(" V");
    m_pZeroVoltDpbx->setMinimum(-3.2);
    m_pZeroVoltDpbx->setMaximum(3.2);
    m_pZeroVoltDpbx->setSingleStep(0.01);
    m_pZeroVoltDpbx->setValue(0.0);
    m_pZeroVoltDpbx->setEnabled(false);

    m_pZeroDurationDpbx->setDecimals(2);
    m_pZeroDurationDpbx->setSuffix(" s");
    m_pZeroDurationDpbx->setMaximum(10.0);
    m_pZeroDurationDpbx->setValue(3.0);

    m_pSeqVoltDpbx->setDecimals(2);
    m_pSeqVoltDpbx->setSuffix(" V");
    m_pSeqVoltDpbx->setRange(-3.2,3.2);
    m_pSeqVoltDpbx->setSingleStep(0.01);
    m_pSeqVoltDpbx->setValue(0.18);

    m_pSeqDurationDpbx->setDecimals(2);
    m_pSeqDurationDpbx->setSuffix(" s");
    m_pSeqDurationDpbx->setMaximum(10.0);
    m_pSeqDurationDpbx->setValue(3.0);

    m_pScanCycleDpbx->setDecimals(0);
    m_pScanCycleDpbx->setRange(0,500);
    m_pScanCycleDpbx->setValue(10);

    m_pCyclePeriodDpbx->setDecimals(2);
    m_pCyclePeriodDpbx->setSuffix(" min");
    m_pCyclePeriodDpbx->setRange(0.1,360);
    m_pCyclePeriodDpbx->setValue(30);

    SetTimehms6();


    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->addWidget(plabPoreQcVolt,0,1,1,1);
    gridlayout->addWidget(plabPoreQcDuration,0,2,1,1);

    gridlayout->addWidget(plabBackward,1,0,1,1);
    gridlayout->addWidget(m_pBackVoltDpbx,1,1,1,1);
    gridlayout->addWidget(m_pBackVoltDurationDpbx,1,2,1,1);
    gridlayout->addWidget(plabPoreQc,2,0,1,1);
    gridlayout->addWidget(m_pPoreQcVoltDpbx,2,1,1,1);
    gridlayout->addWidget(m_pPoreQcDurationDpbx,2,2,1,1);

    gridlayout->addWidget(plabFilterCurMin,3,1,1,1);
    gridlayout->addWidget(plabFilterCurMax,3,2,1,1);

    gridlayout->addWidget(plabFilterCur,4,0,1,1);
    gridlayout->addWidget(m_pFilterCurMinDpbx,4,1,1,1);
    gridlayout->addWidget(m_pFilterCurMaxDpbx,4,2,1,1);
    gridlayout->addWidget(plabFilterStd,5,0,1,1);
    gridlayout->addWidget(m_pFilterStdMinDpbx,5,1,1,1);
    gridlayout->addWidget(m_pFilterStdMaxDpbx,5,2,1,1);
    pStepOneGroupBox->setLayout(gridlayout);

    QGridLayout *gridlayout2 = new QGridLayout();
    gridlayout2->addWidget(plabZeroVolt,0,1,1,1);
    gridlayout2->addWidget(plabZeroDuration,0,2,1,1);
    gridlayout2->addWidget(plabZeroVoltDesc,1,0,1,1);
    gridlayout2->addWidget(m_pZeroVoltDpbx,1,1,1,1);
    gridlayout2->addWidget(m_pZeroDurationDpbx,1,2,1,1);

    gridlayout2->addWidget(plabSeqDesc,2,0,1,1);
    gridlayout2->addWidget(m_pSeqVoltDpbx,2,1,1,1);
    gridlayout2->addWidget(m_pSeqDurationDpbx,2,2,1,1);
    gridlayout2->addWidget(m_pDegateEnableChk,3,0,1,1);
    gridlayout2->addWidget(m_pDegateStartBtn,3,1,1,1);
    gridlayout2->addWidget(m_pDegatePanlSetBtn,3,2,1,2);
    pStepTwoGroupBox->setLayout(gridlayout2);


    QGridLayout *gridlayout3 = new QGridLayout();
    gridlayout3->addWidget(plabTimerPeriod,0,0,1,1);
    gridlayout3->addWidget(m_pCyclePeriodDpbx,0,1,1,1);
    gridlayout3->addWidget(plabCycleInp,1,0,1,1);
    gridlayout3->addWidget(m_pScanCycleDpbx,1,1,1,1);
    gridlayout3->addWidget(m_plcdTimerPeriod,2,0,1,2);
    gridlayout3->addWidget(m_pNextStepBtn,3,0,1,1);
    gridlayout3->addWidget(m_pStartMuxScanBtn,3,1,1,1);
    pStepThreeGroupBox->setLayout(gridlayout3);

    gridlayout->setVerticalSpacing(2);
    gridlayout2->setVerticalSpacing(2);
    gridlayout3->setVerticalSpacing(2);


    QHBoxLayout *horlayout =new QHBoxLayout();
    horlayout->addWidget(pStepOneGroupBox);
    horlayout->addWidget(pStepTwoGroupBox);
    horlayout->addWidget(pStepThreeGroupBox);

    horlayout->addStretch();
    setLayout(horlayout);

    connect(m_pDegatePanlSetBtn,&QPushButton::clicked,this,&MuxScanPanel::OnDegatPanelSettClickSlot);
    connect(m_pDegateStartBtn,&QPushButton::toggled,this,&MuxScanPanel::OnDegatPanelStartClickSlot);
    connect(m_pStartMuxScanBtn,&QPushButton::clicked,this,&MuxScanPanel::OnClickMuxScanStart);
    connect(&m_tmrMuxScanStart,&QTimer::timeout,this,&MuxScanPanel::OnSecondTimerTimeoutSlot);
    connect(m_pNextStepBtn,&QPushButton::clicked,this,&MuxScanPanel::OnClickNextStep);
}


void MuxScanPanel::OnDegatPanelSettClickSlot(void)
{
    if(m_pDegatingPane != nullptr)
    {
        m_pDegatingPane->show();
        ConfigServer::WidgetPosAdjustByParent(nativeParentWidget(), m_pDegatingPane);
    }
}

void MuxScanPanel::OnDegatPanelStartClickSlot(const bool &checked)
{
    if(checked)
    {
        m_pDegateStartBtn->setText(QStringLiteral("Stop"));
        m_pDegateStartBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pDegatingPane->onClickEnableDegating(true);
    }
    else
    {
        m_pDegateStartBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pDegateStartBtn->setText(QStringLiteral("Start"));
        m_pDegatingPane->onClickEnableDegating(false);
    }

}

void MuxScanPanel::OnClickMuxScanStart(const bool &checked)
{
    m_pStartMuxScanBtn->setEnabled(false);
    if (checked)
    {
        LOGI("MuxScan Start....");
        m_iCycleCount = 0;
        m_pStartMuxScanBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pStartMuxScanBtn->setText(QStringLiteral("Stop"));

        m_tmMuxScanStart.setHMS(0, 0, 1, 0);
//        SetTimehms6();
        if (!m_tmrMuxScanStart.isActive())
        {
            m_tmrMuxScanStart.start(1000);
        }
    }
    else
    {
        auto ret = QMessageBox::Yes;
        if (m_iCycleCount <= m_pScanCycleDpbx->value())
        {
            ret = QMessageBox::question(this, "Confirm Stop", "Really want to Stop MuxScan?");
        }
        if (ret == QMessageBox::Yes)
        {
            LOGI("MuxScan Stop....");
            m_bIsNextContinue = true;
            if (m_tmrMuxScanStart.isActive())
            {
                m_tmrMuxScanStart.stop();
            }

            emit SetDataSaveSignal(false);
//            emit SetChannelStatusSignal(false);
            m_pDegateStartBtn->setChecked(false);


            m_iCycleCount = 0;
//            SetShowChannelDCLevelSlot(0.0);

            m_pStartMuxScanBtn->setIcon(QIcon(":/img/img/start.ico"));
            m_pStartMuxScanBtn->setText(QStringLiteral("Start"));
        }
        else
        {
            m_pStartMuxScanBtn->setChecked(true);
        }
    }
    m_pStartMuxScanBtn->setEnabled(true);
}


bool MuxScanPanel::SetTimehms6(void)
{
    int itm = m_pCyclePeriodDpbx->value() * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmMuxScanStart.setHMS(ihour, imin, isec, 0);
        m_plcdTimerPeriod->display(m_tmMuxScanStart.toString("hh:mm:ss"));

        return true;
    }
    return false;
}


void MuxScanPanel::OnSecondTimerTimeoutSlot(void)
{
    m_tmMuxScanStart = m_tmMuxScanStart.addSecs(-1);
    m_plcdTimerPeriod->display(m_tmMuxScanStart.toString("hh:mm:ss"));
    if (m_tmMuxScanStart == QTime(0, 0, 0, 0))
    {
        ++m_iCycleCount;

        if (m_iCycleCount > m_pScanCycleDpbx->value())
        {
            if (m_pStartMuxScanBtn->isChecked())
            {
                m_pStartMuxScanBtn->click();
            }
            LOGI("Time Period QC Seqpuencing Reach Total Cycle Auto Stop!");
            return;
        }
        m_pStartMuxScanBtn->setText(QString("Stop (%1)").arg(m_pScanCycleDpbx->value() - m_iCycleCount));


        emit SetDataSaveSignal(false);

        m_pDegateStartBtn->setChecked(false);

        SetTimehms6();

        QProgressDialog progressDlg(QString("Now is PoreQc Procedure,please waitting.."), "Abort PoreQc", 0,4 , this);
        progressDlg.setWindowModality(Qt::NonModal);
        progressDlg.reset();
        progressDlg.setValue(0);
        progressDlg.show();
        progressDlg.setAutoClose(true);

        progressDlg.setValue(1);
        progressDlg.setLabelText("Setting backward voltage...");
        if (progressDlg.wasCanceled()) { progressDlg.reset(); goto stop; }

        emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,m_pBackVoltDpbx->value(),0.0);
        ConfigServer::caliSleep(m_pBackVoltDurationDpbx->value() * 1000);

        progressDlg.setValue(2);
        progressDlg.setLabelText("2");
        progressDlg.setLabelText("Setting PoreQc voltage...and filt the channel");
        if (progressDlg.wasCanceled()) { progressDlg.rect();goto stop; }

        emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,m_pPoreQcVoltDpbx->value(),0.0);
        ConfigServer::caliSleep(m_pPoreQcDurationDpbx->value() * 1000);


        progressDlg.setValue(3);
        progressDlg.setLabelText("Filter the channel out of average and std range...");
        if (progressDlg.wasCanceled()) { progressDlg.rect(); goto stop; }


        const auto &allchAvg = m_pDataCalculate->GetCurrentAvgRef();
        const auto &allchStd = m_pDataCalculate->GetCurrentStdRef();
        auto &vctSeqState = m_pCustomPlotWidgt->GetAllChSeqSteRef();
        auto &allChVisible = m_pCustomPlotWidgt->GetAllChVisibleRef();

        float curMin = m_pFilterCurMinDpbx->value();
        float curMax = m_pFilterCurMaxDpbx->value();
        float stdMin = m_pFilterStdMinDpbx->value();
        float stdMax = m_pFilterStdMaxDpbx->value();
        bool bIsOn = false;

        for(int ch  = 0 ; ch< CHANNEL_NUM; ++ch )
        {
            const auto &avg = allchAvg[ch];
            const auto &std = allchStd[ch];
            bIsOn =  (avg >= curMin && avg <= curMax && std >= stdMin && std <= stdMax);
            vctSeqState[ch] = bIsOn == true ? MUXSCAN_SEQUENCING : MUXSCAN_UNSEQUENCING;
            allChVisible[ch] = bIsOn;
//            std::cout<<"ch "<< ch<<" avg "<<avg<<"std "<<std<<std::endl;
        }

        //更新曲线是否可见
        m_pCustomPlotWidgt->UpdateAllChVisibleView(allChVisible);
        //同步测序通道到保存文件
        emit UpdateAllchSaveSig(vctSeqState);

        progressDlg.setMaximum(progressDlg.maximum());
        progressDlg.reset();


        if (m_pStartMuxScanBtn->isChecked())
        {
            MyQMessageBox mymsgbox(this);
            int iret = mymsgbox.exec();
            m_bIsNextContinue = (iret == QMessageBox::RejectRole);

            if (!m_bIsNextContinue)
            {
                if (m_tmrMuxScanStart.isActive())
                {
                    m_tmrMuxScanStart.stop();
                }
                m_pCustomPlotWidgt->OnCurvesCfgClkSlot();
                return;
            }
        }
        else
        {
            return;
        }

        //sequencing....
        DoSequencingLoop();

    }
    return;
stop:
    if (m_pStartMuxScanBtn->isChecked())
    {
        m_pStartMuxScanBtn->click();
    }
}


void MuxScanPanel::DoSequencingLoop(void)
{
    QProgressDialog progressDlg(QString("Now is Sequencing Procedure,please waitting.."), "Abort Sequencing", 0,5 , this);
    progressDlg.setWindowModality(Qt::ApplicationModal);
    progressDlg.reset();
    progressDlg.setValue(0);
    progressDlg.show();
    progressDlg.setAutoClose(true);

    progressDlg.setValue(1);
    progressDlg.setLabelText("Shut off unsequencing channel and sync save item...");
    if (progressDlg.wasCanceled())
    {
        if (m_pStartMuxScanBtn->isChecked())
        {
            m_pStartMuxScanBtn->click();
        }
        progressDlg.reset();
    }

    //根据Seq將标志为非测序通道shutoff
    const auto &vctSeqState = m_pCustomPlotWidgt->GetAllChSeqSteRef();
    auto &vctAllchFuncState = m_pModelSharedDataPtr->GetAllChFuncState();
    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        if(vctSeqState[ch] ==  MUXSCAN_UNSEQUENCING && vctAllchFuncState[ch] != SENSORGRP_FUNC_STATE_OFF)
            vctAllchFuncState[ch] = SENSORGRP_FUNC_STATE_OFF;
    }
    //发送sensor控制
    emit SigAllChDifferFuncStateSet(vctAllchFuncState);

    progressDlg.setValue(2);
    progressDlg.setLabelText("Seting zeroVolt...");
    if (progressDlg.wasCanceled())
    {
        if (m_pStartMuxScanBtn->isChecked())
        {
            m_pStartMuxScanBtn->click();
        }
        progressDlg.reset();
    }

    emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,0.0,0.0);
    ConfigServer::caliSleep(m_pZeroDurationDpbx->value() * 1000);

    //保存数据
    emit SetDataSaveSignal(true);

    progressDlg.setValue(3);
    progressDlg.setLabelText("Start enable saving data...");
    ConfigServer::caliSleep(m_pZeroDurationDpbx->value() * 1000);

    progressDlg.setValue(4);
    progressDlg.setLabelText("Seting seqVolt...");
    if (progressDlg.wasCanceled())
    {
        if (m_pStartMuxScanBtn->isChecked())
        {
            m_pStartMuxScanBtn->click();
        }
        progressDlg.reset();
    }

    emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,m_pSeqVoltDpbx->value(),0.0);
    ConfigServer::caliSleep(m_pSeqDurationDpbx->value() * 1000);

    if(m_pStartMuxScanBtn->isChecked() && m_pDegateEnableChk->isChecked())
    {
        progressDlg.setValue(5);
        progressDlg.setLabelText("Enable degating...");
        if (progressDlg.wasCanceled())
        {
            if (m_pStartMuxScanBtn->isChecked())
            {
                m_pStartMuxScanBtn->click();
            }
            progressDlg.reset();
        }

        if(!m_pDegateStartBtn->isChecked())
        {
            m_pDegateStartBtn->click();
        }
        ConfigServer::caliSleep(1000);
    }
    progressDlg.setMaximum(progressDlg.maximum());
    progressDlg.reset();
    return;
//stop:
    if (m_pStartMuxScanBtn->isChecked())
    {
        m_pStartMuxScanBtn->click();
    }
    progressDlg.reset();
}


void MuxScanPanel::OnClickNextStep(void)
{
    if (m_bIsNextContinue)
    {
        return;
    }
    m_bIsNextContinue = true;

    const auto &vctSeqState = m_pCustomPlotWidgt->GetAllChSeqSteRef();
    //同步测序通道到保存文件
    emit UpdateAllchSaveSig(vctSeqState);


    DoSequencingLoop();
    if (m_pStartMuxScanBtn->isChecked())
    {
        if (!m_tmrMuxScanStart.isActive())
        {
            m_tmrMuxScanStart.start(1000);
        }
    }
}
