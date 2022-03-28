#include "FlowOpertPanel.h"
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QSettings>
#include <QLCDNumber>
#include <QDir>
#include "ConfigServer.h"
#include "PoreStateMapWdgt.h"
#include "CustomPlotWidgt.h"
#include "SensorPanel.h"
#include "AutoScanPanel.h"
#include "CustomComponent.h"
#include "table/PlotCfgTabModel.h"
#include "Log.h"



FlowOpertPanel::FlowOpertPanel(QWidget *parent): QWidget(parent)
{
    m_vetAvg.resize(CHANNEL_NUM);
    m_vetStd.resize(CHANNEL_NUM);
    std::fill_n(m_vetAvg.begin(),CHANNEL_NUM,0);
    std::fill_n(m_vetStd.begin(),CHANNEL_NUM,0);

    InitCtrl();
    InitConfig();


}
FlowOpertPanel::~FlowOpertPanel()
{

}

void FlowOpertPanel::InitCtrl(void)
{

    m_pGroupBox = new QGroupBox(QStringLiteral("Standard Operating Procedure"),this);
    m_pFlowTabWiget = new QTabWidget(this);
    InitMembCtrl();
    InitPoreInsertCtrl();
    InitPoreAutoFiltCtrl();
    InitAutoSequenCtrl();
    m_pFlowTabWiget->insertTab(FLOW_PROCESS_PAGE_MEMBRANEQC,m_pMembraneQCGrpbx,QStringLiteral("MembraneQC"));
    m_pFlowTabWiget->insertTab(FLOW_PROCESS_PAGE_POREINSERT,m_pPoreInsertGrpbx,QStringLiteral("PoreInsert"));
    m_pFlowTabWiget->insertTab(FLOW_PROCESS_PAGE_POREFILT,m_pPoreAutoFiltGrpbx,QStringLiteral("PoreAutoFilt"));
    m_pFlowTabWiget->insertTab(FLOW_PROCESS_PAGE_SEQUENCY,m_pAutoSeqWidget,QStringLiteral("AutoSeq"));

    QHBoxLayout *tabhorlayout =new QHBoxLayout();
    tabhorlayout->addWidget(m_pFlowTabWiget);
    m_pGroupBox->setLayout(tabhorlayout);


    QHBoxLayout *horlayout= new QHBoxLayout();
    horlayout->addWidget(m_pGroupBox);
    setLayout(horlayout);

    m_pFlowTabWiget->setDocumentMode(true);

}


void FlowOpertPanel::InitMembCtrl(void)
{
    m_pMembraneQCGrpbx = new QGroupBox(QStringLiteral("MembraneQC"),this);
    m_pMembraneQCWizardBtn = new QPushButton(QStringLiteral("Setting"),this);
    m_pMembraneQCStartBtn = new QPushButton(QIcon(":/img/img/start.ico"),"Start",this);
    m_pMembraneQCWizardDlg = new MembraneQCSettingDlg(this);
    m_pMembraneQCProgresBar = new QProgressBar(this);
    m_pMembraneQCStartBtn->setCheckable(true);

    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->addWidget(m_pMembraneQCWizardBtn,0,0,1,1);
    gridlayout->addWidget(m_pMembraneQCStartBtn,1,0,1,1);
    gridlayout->addWidget(m_pMembraneQCProgresBar,1,1,1,1);
    gridlayout->setColumnStretch(1,10);
    m_pMembraneQCGrpbx->setLayout(gridlayout);
    connect(m_pMembraneQCWizardBtn,&QPushButton::clicked,[=](){
#if 0
        int Index = QApplication::desktop()->screenNumber(this);
        qDebug()<<QApplication::desktop()->screenGeometry(Index);
        //获取当前屏幕的索引
        m_pMembraneQCWizardDlg->setGeometry(QApplication::desktop()->screenGeometry(Index));
        //设置app所在位置
#endif
        m_pMembraneQCWizardDlg->LoadConfig(m_pMembraneQCSettings);
        m_pMembraneQCWizardDlg->show();
    });

      connect(m_pMembraneQCStartBtn,&QPushButton::toggled,this,&FlowOpertPanel::MembraneQCStartBtnSlot);
}

void FlowOpertPanel::InitPoreInsertCtrl(void)
{
    m_pPoreInsertGrpbx = new QGroupBox(QStringLiteral("Pore insert"),this);

}

void FlowOpertPanel::InitPoreAutoFiltCtrl(void)
{
    m_pPoreAutoFiltGrpbx = new QGroupBox(QStringLiteral("Automatic filtting pore"),this);


    m_pPoreAutoFiltWizardBtn = new QPushButton(QStringLiteral("Setting"),this);
    m_pPoreAutoFiltStartBtn =new QPushButton(QIcon(":/img/img/start.ico"),"Start",this);
    m_pPoreAutoFiltSettingDlg = new PoreAutoFiltSettingDlg(this);
    m_pPoreAutoFiltProgresBar = new QProgressBar(this);
    m_pPoreAutoFiltStartBtn->setCheckable(true);

    m_pObserveMuxFinalProgresDlg = new QProgressDialog(QString("Now is Observe mux finnal,please waitting.."), "Abort Observe", 0,5 , this);
    m_pObserveMuxFinalProgresDlg->reset();
    m_pObserveMuxFinalProgresDlg->hide();

    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->addWidget(m_pPoreAutoFiltWizardBtn,0,0,1,1);
    gridlayout->addWidget(m_pPoreAutoFiltStartBtn,1,0,1,1);
    gridlayout->addWidget(m_pPoreAutoFiltProgresBar,1,1,1,1);
    gridlayout->setColumnStretch(1,10);
    m_pPoreAutoFiltGrpbx->setLayout(gridlayout);

    connect(m_pPoreAutoFiltWizardBtn,&QPushButton::clicked,[=](){
        m_pPoreAutoFiltSettingDlg->LoadConfig(m_pPoreAutoFiltSettings);
        m_pPoreAutoFiltSettingDlg->show();

    });
    connect(m_pPoreAutoFiltStartBtn,&QPushButton::toggled,this,&FlowOpertPanel::PoreAutoFiltStartBtnSlot);


}

void FlowOpertPanel::InitAutoSequenCtrl(void)
{
    m_pAutoSeqWidget = new QWidget(this);
    m_pAutoSequencyGrpbx = new QGroupBox(QStringLiteral("Auto sequencing"),this);
    m_pAutoSequencySetBtn = new QPushButton(QStringLiteral("Setting"),this);
    m_pAutoSequencySettingDlg = new AutoSequenceSettingDlg(this);
    m_pAutoSequencyProgresBar = new QProgressBar(this);

    m_pPeriodGrpBx = new QGroupBox(QStringLiteral("Period"),this);
    QLabel *plabCycleInp = new QLabel(QStringLiteral("Cycle"),this);
    QLabel *plabTimerPeriod = new QLabel(QStringLiteral("Timer Period"),this);
    m_pAutoSeqCycleDpbx= new CustomSpinbox(1,60,10,1,QStringLiteral(""),QStringLiteral(""),this);
    m_pCyclePeriodDpbx= new CustomDoubleSpinbox(0,600,60,1,QStringLiteral(""),QStringLiteral(" min"),this);
    m_pAutoSequencyStartBtn =new QPushButton(QIcon(":/img/img/start.ico"),QStringLiteral("Start"),this);
    m_plcdTimerPeriod = new QLCDNumber(8, this);
    m_plcdTimerPeriod->setSegmentStyle(QLCDNumber::Flat);
    m_plcdTimerPeriod->setFrameShape(QFrame::StyledPanel);
    m_pCyclePeriodDpbx->setValue(30);
    m_pAutoSequencyStartBtn->setCheckable(true);

    SetTimehms(m_pCyclePeriodDpbx->value());

    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->addWidget(m_pAutoSequencySetBtn,0,0,1,1);
    gridlayout->addWidget(m_pAutoSequencyProgresBar,0,1,1,1);
    gridlayout->setColumnStretch(1,10);
    m_pAutoSequencyGrpbx->setLayout(gridlayout);

    QGridLayout *gridlayout3 = new QGridLayout();
    gridlayout3->addWidget(plabTimerPeriod,0,0,1,1);
    gridlayout3->addWidget(m_pCyclePeriodDpbx,0,1,1,1);
    gridlayout3->addWidget(plabCycleInp,1,0,1,1);
    gridlayout3->addWidget(m_pAutoSeqCycleDpbx,1,1,1,1);
    gridlayout3->addWidget(m_plcdTimerPeriod,2,0,1,2);
    gridlayout3->addWidget(m_pAutoSequencyStartBtn,3,1,1,1);
    m_pPeriodGrpBx->setLayout(gridlayout3);

    QHBoxLayout *horilayout =new QHBoxLayout();
    horilayout->addWidget(m_pPeriodGrpBx);
    horilayout->addWidget(m_pAutoSequencyGrpbx);
    m_pAutoSeqWidget->setLayout(horilayout);


    connect(m_pAutoSequencySetBtn,&QPushButton::clicked,[=](){
        m_pAutoSequencySettingDlg->LoadConfig(m_pAutoSequencySettings);
        m_pAutoSequencySettingDlg->show();

    });
    connect(m_pAutoSequencyStartBtn,&QPushButton::toggled,this,&FlowOpertPanel::AutoSequencyStartBtnSlot);
    connect(&m_tmrAutoSeqStart,&QTimer::timeout,this,&FlowOpertPanel::TimeUpToAutoSequencySlot);

}

void FlowOpertPanel::InitConfig(void)
{
    std::string strMembraneQCCfg = QDir::currentPath().toStdString()+ "/etc/Conf/Flow/MembraneQCCfg.ini";
    const char* pMembQCConfig = strMembraneQCCfg.c_str();
    bool ret = ConfigServer::SyncDefaultConfig(":/etc/etc/Flow/default", pMembQCConfig);
    QString membSettingPath = QString::fromStdString(pMembQCConfig);
    QFile membQCfile(membSettingPath);
    if (membQCfile.exists())
    {
        m_pMembraneQCSettings = new QSettings(membSettingPath, QSettings::IniFormat, this);
        if (ret)
        {
            membQCfile.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        }
        m_pMembraneQCWizardDlg->LoadConfig(m_pMembraneQCSettings);
    }

    std::string strPoreFiltCfg = QDir::currentPath().toStdString()+ "/etc/Conf/Flow/PoreFiltValidCfg.ini";
    const char* pPoreFiltConfig = strPoreFiltCfg.c_str();
    ret = ConfigServer::SyncDefaultConfig(":/etc/etc/Flow/default", pPoreFiltConfig);
    QString poreFiltSettingPath = QString::fromStdString(pPoreFiltConfig);
    QFile poreFiltfile(poreFiltSettingPath);
    if (poreFiltfile.exists())
    {
        m_pPoreAutoFiltSettings = new QSettings(poreFiltSettingPath, QSettings::IniFormat, this);
        if (ret)
        {
            poreFiltfile.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        }
//        m_pPoreAutoFiltSettingDlg->SetConfigPtr(m_pPoreAutoFiltSettings);
        m_pPoreAutoFiltSettingDlg->LoadConfig(m_pPoreAutoFiltSettings);
    }

    std::string strAutoSeqCfg = QDir::currentPath().toStdString()+ "/etc/Conf/Flow/AutoSequencingCfg.ini";
    const char* pAutoSeqConfig = strAutoSeqCfg.c_str();
    ret = ConfigServer::SyncDefaultConfig(":/etc/etc/Flow/default", pAutoSeqConfig);
    QString autoSeqSettingPath = QString::fromStdString(pAutoSeqConfig);
    QFile autoSeqFile(autoSeqSettingPath);
    if (autoSeqFile.exists())
    {
        m_pAutoSequencySettings = new QSettings(autoSeqSettingPath, QSettings::IniFormat, this);
        if (ret)
        {
            autoSeqFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        }
//        m_pAutoSequencySettingDlg->SetConfigPtr(m_pAutoSequencySettings);
        m_pAutoSequencySettingDlg->LoadConfig(m_pAutoSequencySettings);
    }




}


void FlowOpertPanel::AutoSequencyStartBtnSlot(const bool &checked)
{
    bool saveDirIsEmpty = false;
    emit GetDataSaveDirIsEmptySig(saveDirIsEmpty);
    if(saveDirIsEmpty)
    {
         m_pAutoSequencyStartBtn->setChecked(false);
         QMessageBox::warning(this, tr("Warning"), tr("Please set your save directory!!!"));
         return;
    }

    m_pAutoSequencyStartBtn->setEnabled(false);
    if (checked)
    {
        LOGI("Autoscan Start....");
        m_iCycleCount = 0;
        m_pAutoSequencyStartBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pAutoSequencyStartBtn->setText(QStringLiteral("Stop"));
        m_bAutoSeqStop = false;

        m_tmAutoSeqStart.setHMS(0, 0, 1, 0);

//        emit NotifyRecordOperateSig(RECORD_READS_ADAPTERS_FILE,RECORD_FILE_OPEN);
//        emit NotifyRecordOperateSig(RECORD_VALID_FILE,RECORD_FILE_OPEN);

        if (!m_tmrAutoSeqStart.isActive())
        {
            m_tmrAutoSeqStart.start(1000);
        }
    }
    else
    {
        auto ret = QMessageBox::Yes;
        if (m_iCycleCount <= m_pAutoSeqCycleDpbx->value())
        {
            ret = QMessageBox::question(this, "Confirm Stop", "Really want to Stop Autoscan?");
        }
        if (ret == QMessageBox::Yes)
        {
            LOGI("Autoseq Stop....");
            if (m_tmrAutoSeqStart.isActive())
            {
                m_tmrAutoSeqStart.stop();
            }

            m_iCycleCount = 0;

            m_pAutoSequencyStartBtn->setIcon(QIcon(":/img/img/start.ico"));
            m_pAutoSequencyStartBtn->setText(QStringLiteral("Start"));
            m_bAutoSeqStop =true;

            StopAllActivity();
            ConfigServer::GetInstance()->SetNeedGenerateRunId(true);
//            emit NotifyRecordOperateSig(RECORD_READS_ADAPTERS_FILE,RECORD_FILE_CLOSE);
//            emit NotifyRecordOperateSig(RECORD_VALID_FILE,RECORD_FILE_CLOSE);
        }
        else
        {
            m_bAutoSeqStop = false;
            m_pAutoSequencyStartBtn->setChecked(true);
        }
    }
    m_pAutoSequencyStartBtn->setEnabled(true);
}

void FlowOpertPanel::TimeUpToAutoSequencySlot(void)
{
    m_tmAutoSeqStart = m_tmAutoSeqStart.addSecs(-1);
    m_plcdTimerPeriod->display(m_tmAutoSeqStart.toString("hh:mm:ss"));
    if (m_tmAutoSeqStart == QTime(0, 0, 0, 0))
    {
        ++m_iCycleCount;
        if (m_iCycleCount > m_pAutoSeqCycleDpbx->value())
        {
            if (m_pAutoSequencyStartBtn->isChecked())
            {
                m_pAutoSequencyStartBtn->click();
            }
            LOGI("Time Period QC Seqpuencing Reach Total Cycle Auto Stop!");
            return;
        }
        else
        {
            m_pAutoSequencyStartBtn->setText(QString("Stop (%1)").arg(m_pAutoSeqCycleDpbx->value() - m_iCycleCount));
            if(!AutoSequencyStart())
            {
                m_pAutoSequencyStartBtn->setChecked(false);
            }
        }

    }
    return;
}
void FlowOpertPanel::MembraneQCStartBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pMembraneQCStartBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pMembraneQCStartBtn->setText("Stop");
        m_bAutoMembrQCStop = false;
        MembraneQCStart();
    }
    else
    {
        m_pMembraneQCStartBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pMembraneQCStartBtn->setText("Start");
        m_bAutoMembrQCStop = true;
    }
}

void FlowOpertPanel::PoreAutoFiltStartBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pPoreAutoFiltStartBtn->setIcon(QIcon(":/img/img/close.ico"));
        m_pPoreAutoFiltStartBtn->setText("Stop");
        m_bAutoPoreFiltStop = false;
        PoreAutoFiltStart();
    }
    else
    {
        m_pPoreAutoFiltStartBtn->setIcon(QIcon(":/img/img/start.ico"));
        m_pPoreAutoFiltStartBtn->setText("Start");
        m_bAutoPoreFiltStop = true;
    }
}


void FlowOpertPanel::PoreAutoFiltStart(void)
{
    const int totalCnt = 2;
    int cnt = 0;
    int finalMuxCnt =0;
    const auto &LockParameters = m_pPoreAutoFiltSettingDlg->GetPoreFiltLockParameter();
    const auto &PoreFiltParameters = m_pPoreAutoFiltSettingDlg->GetPoreFiltToValidParameter();

    m_pPoreAutoFiltProgresBar->setRange(0,totalCnt);
    m_pPoreAutoFiltProgresBar->setTextVisible(true);
    m_pPoreAutoFiltProgresBar->reset();

    //step1  lock unnormal channels
    m_pPoreAutoFiltProgresBar->setValue(cnt++);
    m_pPoreAutoFiltProgresBar->setFormat("Lock unnormal channels....%p%");
    if(m_bAutoPoreFiltStop){ goto stop; }

#if 1
    emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,LockParameters.OverScanVolt,0);
    for(int sensorgrp = 0; sensorgrp< SENSOR_GROUP_NUM; ++sensorgrp)
    {
        if(m_bAutoPoreFiltStop){ goto stop; }
        SENSOR_STATE_GROUP_ENUM sensorGroup = SENSOR_STATE_GROUP_ENUM(sensorgrp);
        emit SensorGroupChangeSig(sensorGroup);
        m_pPoreStateMapWdgt->SetLockScanGroup(sensorGroup,LockParameters.OverScanTimes);
        if(!m_pPoreStateMapWdgt->OnLockScanForAllCh() || m_bAutoPoreFiltStop) { goto stop;}
    }
#endif

    //step2 filt valid for channels by means and std
    if(m_bAutoPoreFiltStop){ goto stop; }
    m_pPoreAutoFiltProgresBar->setValue(cnt++);
    m_pPoreAutoFiltProgresBar->setFormat("Filt valid for channels by means and std....%p%");
    emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,PoreFiltParameters.DirectVolt,0);
    for(int sensorgrp = 0; sensorgrp< SENSOR_GROUP_NUM; ++sensorgrp)
    {
        if(m_bAutoPoreFiltStop){ goto stop; }
        SENSOR_STATE_GROUP_ENUM sensorGroup = SENSOR_STATE_GROUP_ENUM(sensorgrp);
        emit SensorGroupChangeSig(sensorGroup);
        ConfigServer::SleepForTimeoutOrStop(PoreFiltParameters.WattingTime * 1000,m_bAutoPoreFiltStop);
        PoreFiltJudgeByMeansAndStd(PoreFiltParameters);
    }

    m_pPoreStateMapWdgt->OnRefreshCountSlot(VALID_SENSOR_STATE_MAP_MODE);
    m_pPoreStateMapWdgt->OnRefreshCountSlot(PORE_STATE_MAP_MODE);
    m_pPoreStateMapWdgt->OnAllChValidGenerateMuxRandomBtnSlot();
    m_pSensorPanel->OnMuxModeSet();
    m_pCusPlotCurvesCfgDlg->OnFiltValidShutOtherBtnSlot();    //只留下valid 其他off;

    m_pCusPlotCurvesCfgDlg->SetCurFiltType(PlotCfgSortFilterModel::FILTER_TYPE_AVG_STD);
    m_pCusPlotCurvesCfgDlg->UpdateAvgStdParameterSlot(PoreFiltParameters.SinglePoreMeanMin,PoreFiltParameters.SinglePoreMeanMax,PoreFiltParameters.SinglePoreStdMin,PoreFiltParameters.SinglePoreStdMax);

    if(!ObserveWattingMuxFinal(10,m_bAutoPoreFiltStop,finalMuxCnt)){goto stop;}

    emit SendFinalMuxSig(true,finalMuxCnt);
    //这里要记录Mux中单孔的数量
    emit NotifyRecordOperateSig(RECORD_VALID_FILE,RECORD_FILE_RECORD);
    //这里要记录单孔多孔的数量
    emit NotifyRecordOperateSig(RECORD_PORE_FILE,RECORD_FILE_RECORD);

    m_pPoreAutoFiltStartBtn->setChecked(false);
    m_pPoreAutoFiltProgresBar->setValue(m_pPoreAutoFiltProgresBar->maximum());
    emit AllChanSensorStateSetOffSig();
    m_pPoreAutoFiltProgresBar->setFormat("Single pore auto filtting has finished");
    return;
stop:
    m_pPoreAutoFiltStartBtn->setChecked(false);
    LOGI("PoreAutoFiltStart Stop!!!");
    return;
}

void FlowOpertPanel::MembraneQCStart(void)
{
    const int totalCnt = 3;
    int cnt = 0;

    const auto &SOverFlowFiltParams = m_pMembraneQCWizardDlg->GetMembOverFlowFiltParams();
    const auto &SOverFlowRecheckParams = m_pMembraneQCWizardDlg->GetMembOverFlowRecheckParams();
    const auto &SMembQuantityJudgeParams = m_pMembraneQCWizardDlg->GetMembQuantityJudgeParams();
    const auto &capParams = SMembQuantityJudgeParams;
    std::array<float,CAP_STATE_NUM> arryCapLevleParams = {capParams.capLevel1,capParams.capLevel2,capParams.capLevel3,capParams.capLevel4,capParams.capLevel4};

    m_pMembraneQCProgresBar->setRange(0,totalCnt);
    m_pMembraneQCProgresBar->setTextVisible(true);
    m_pMembraneQCProgresBar->reset();

//    m_pPoreStateMapWdgt->OnLockClearClickSlot();
#if 1
    //step1 membrane over flow QC
    m_pMembraneQCProgresBar->setValue(cnt++);
    m_pMembraneQCProgresBar->setFormat("membrane over flow QC....%p%");
    if(m_bAutoMembrQCStop){ goto stop; }
    emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,SOverFlowFiltParams.overScanVolt,0);
    for(int sensorgrp = 0; sensorgrp< SENSOR_GROUP_NUM; ++sensorgrp)
    {
        if(m_bAutoMembrQCStop){ goto stop; }
        SENSOR_STATE_GROUP_ENUM sensorGroup = SENSOR_STATE_GROUP_ENUM(sensorgrp);
        emit SensorGroupChangeSig(sensorGroup);
        m_pPoreStateMapWdgt->SetLockScanGroup(sensorGroup,SOverFlowFiltParams.overScanTimes);
        if(!m_pPoreStateMapWdgt->OnLockScanForAllCh() || m_bAutoMembrQCStop)
        {
            goto stop;
        }
    }


    //step2 recheck by std or mean
    m_pMembraneQCProgresBar->setValue(cnt++);
    m_pMembraneQCProgresBar->setFormat("recheck by std or mean....%p%");
    emit VoltageSetSig(VOLTAGE_DIRECT_ENUM,SOverFlowRecheckParams.directVolt,0);
    for(int sensorgrp = 0; sensorgrp< SENSOR_GROUP_NUM; ++sensorgrp)
    {
        if(m_bAutoMembrQCStop){ goto stop; }
        SENSOR_STATE_GROUP_ENUM sensorGroup = SENSOR_STATE_GROUP_ENUM(sensorgrp);
        emit SensorGroupChangeSig(sensorGroup);
        ConfigServer::SleepForTimeoutOrStop(SOverFlowRecheckParams.wattingTime * 1000,m_bAutoMembrQCStop);
        m_pCusPlotCurvesCfgDlg->OnLockChGreaterAvgOrStdLimit(SOverFlowRecheckParams.overRecheckMean,SOverFlowRecheckParams.overRecheckStd);
    }
#endif
    //step3 membrane quality judgement
    m_pMembraneQCProgresBar->setValue(cnt++);
    m_pMembraneQCProgresBar->setFormat("membrane quality judgement");

    emit VoltSetParametersSig(VOLTAGE_TRIANGULAR_ENUM,SMembQuantityJudgeParams.acVolt);
    m_pPoreStateMapWdgt->UpdateCapLevelLimtInpt(arryCapLevleParams);
    m_pPoreStateMapWdgt->CapCalStartBtnSlot(true);
    for(int sensorgrp = 0; sensorgrp< SENSOR_GROUP_NUM; ++sensorgrp)
    {
        if(m_bAutoMembrQCStop){ goto stop; }
        SENSOR_STATE_GROUP_ENUM sensorGroup = SENSOR_STATE_GROUP_ENUM(sensorgrp);
        emit SensorGroupChangeSig(sensorGroup);
        ConfigServer::SleepForTimeoutOrStop(SMembQuantityJudgeParams.wattingTime * 1000,m_bAutoMembrQCStop);
        m_pPoreStateMapWdgt->OnLockChGreaterCapLevel(CAP_LEVEL3_STATE);
    }
    m_pPoreStateMapWdgt->CapCalStartBtnSlot(false);

    m_pMembraneQCStartBtn->setChecked(false);
    m_pMembraneQCProgresBar->setValue(m_pMembraneQCProgresBar->maximum());
    emit AllChanSensorStateSetOffSig();
    m_pMembraneQCProgresBar->setFormat("Membrane QC has finished");

    return;
stop:
    m_pMembraneQCStartBtn->setChecked(false);
    m_pPoreStateMapWdgt->OnCancelScanProgress();
    LOGI("Membrane QC Stop!!!");
    return;
}


void FlowOpertPanel::PoreFiltJudgeByMeansAndStd(const PoreAutoFiltSettingDlg::SPoreFiltToValidParameter &poreFiltParameters)
{
    const double singleAvgMin = poreFiltParameters.SinglePoreMeanMin;
    const double singleAvgMax = poreFiltParameters.SinglePoreMeanMax;
    const double singleStdMin = poreFiltParameters.SinglePoreStdMin;
    const double singleStdMax = poreFiltParameters.SinglePoreStdMax;

    const double multiAvgMin = poreFiltParameters.MultiPoreMeanMin;
    const double multiAvgMax = poreFiltParameters.MultiPoreMeanMax;
    const double multiStdMin = poreFiltParameters.MultiPoreStdMin;
    const double multiStdMax = poreFiltParameters.MultiPoreStdMax;

    const auto &vctAllChSensorGrp = m_pModelSharedDataPtr->GetAllChSensorGroup();
    auto &vctAllChFourPoreState = m_pModelSharedDataPtr->GetFourPoreStateData();
    auto &vctAllChFourValid = m_pModelSharedDataPtr->GetAllChFourValidStateRef();

    emit GetAllChAvgSig(m_vetAvg);
    emit GetAllChStdSig(m_vetStd);

    for(int ch = 0; ch< CHANNEL_NUM; ++ch)
    {
        const auto &curAvg = m_vetAvg[ch];
        const auto &curStd = m_vetStd[ch];
        const auto &chSensorGrp = vctAllChSensorGrp[ch];
        auto &poreSta = vctAllChFourPoreState[chSensorGrp][ch];
        auto &validState = vctAllChFourValid[chSensorGrp][ch];
        if(curAvg >singleAvgMin && curAvg  <= singleAvgMax)
        {
            if(curStd > singleStdMin && curStd <= singleStdMax)
            {
                poreSta = SINGLE_PORE_STATE;
                validState = VALID_CHANNEL_ENUM;
            }
        }
        else
        {
            if(curAvg  > multiAvgMin && curAvg <= multiAvgMax)
            {
                if(curStd > multiStdMin && curStd <= multiStdMax)
                {
                    poreSta = MULTI_PORE_STATE;
                }
            }
            validState = INVALID_CHANNEL_ENUM;
        }
    }
}


bool FlowOpertPanel::SetTimehms(const float &min)
{
    int itm = min * 60;
    if (itm > 0)
    {
        int ihour = itm / 3600;
        int irest = itm % 3600;
        int imin = irest / 60;
        int isec = irest % 60;

        m_tmAutoSeqStart.setHMS(ihour, imin, isec, 0);
        m_plcdTimerPeriod->display(m_tmAutoSeqStart.toString("hh:mm:ss"));

        return true;
    }
    return false;
}

bool FlowOpertPanel::AutoSequencyStart(void)
{

    const int totalCnt = 3;
    int cnt = 0;
    int finalMuxCnt =0;
    bool bIsAutoTimerDegatRunning = false;
    const auto &scanParams = m_pAutoSequencySettingDlg->GetSSeqLockParams();
    const auto &degateParams = m_pAutoSequencySettingDlg->GetSSeqDegateParams();
    const auto &otherParams = m_pAutoSequencySettingDlg->GetSSeqOtherParams();
    const auto &validParams = m_pAutoSequencySettingDlg->GetSSeqValidFiltParams();

#if 1
    emit AcquireAutoTimerDegatIsRunningSig(bIsAutoTimerDegatRunning);
    if(bIsAutoTimerDegatRunning)
    {
        emit AutoTimerDegatControlSig(false); //先暂停定时degating的计时
    }

    //停止保存文件
    emit SetDataSaveSignal(false);
    emit NotifyRecordOperateSig(RECORD_READS_ADAPTERS_FILE,RECORD_FILE_RECORD);

    //关闭正在进行的活动
    m_pAutoScanPanel->OnStopAllActivity();
    //关闭破膜保护
    emit EnableMembrokProtectSig(false,otherParams.membrSaturatedCurrent);

    //关闭OVF Scan模块
    emit EnableOVFAutoScanSig(otherParams.oVFScanPeriod,otherParams.oVFScanTimes,false);

    //关闭degating
    emit TempControlDegateSig(false);
#endif

#ifdef EASY_PROGRAM_DEBUGGING
     SetTimehms(m_pCyclePeriodDpbx->value());
#else
    SetTimehms(m_iCycleCount == 1 ? 30 : m_pCyclePeriodDpbx->value());//第一次一定要设定30min
#endif

    m_pAutoSequencyProgresBar->setRange(0,totalCnt);
    m_pAutoSequencyProgresBar->setTextVisible(true);
    m_pAutoSequencyProgresBar->reset();

    if(m_iCycleCount == 1) //第一次才需要Scan
    {
        //step1  lock unnormal channels
        m_pAutoSequencyProgresBar->setValue(cnt++);
        m_pAutoSequencyProgresBar->setFormat("Lock unnormal channels....%p%");
        m_pAutoScanPanel->SetScanLockParams(scanParams.overScanDcVoL,scanParams.overScanAcVoLt,scanParams.overScanTimes);
        if(!m_pAutoScanPanel->StartScanChannelProcedure(m_bAutoSeqStop)){ goto stop;}
    }

    //step2 valid qc channel
    m_pAutoSequencyProgresBar->setValue(cnt++);
    m_pAutoSequencyProgresBar->setFormat("Valid QC....%p%");
    m_pAutoScanPanel->SetFilterValidParams(validParams.shuotffDcVolt,validParams.filterCurMin,validParams.filterCurMax,validParams.filterStdMin,\
                                           validParams.filterStdMax,validParams.filterWaitSec,validParams.validFilterTimes);
    if(!m_pAutoScanPanel->StartFilterValidProcedure(m_bAutoSeqStop)){ goto stop;}

    if(validParams.bObserveWaitNeed)
    {
        if(!ObserveWattingMuxFinal(validParams.observeWaitTime,m_bAutoSeqStop,finalMuxCnt)){goto stop;}
    }
    emit SendFinalMuxSig(validParams.bObserveWaitNeed,finalMuxCnt);
    emit NotifyRecordOperateSig(RECORD_VALID_FILE,RECORD_FILE_RECORD);


    emit UpdateDegateBaseParametersSig(degateParams.cyclePeriod,degateParams.durationTime,degateParams.allowTimes,degateParams.bAutoseqDegat);
    emit UpdateDegatingParametersSig(degateParams.durationThres,degateParams.gatingSTD,degateParams.gatingSTDMax,degateParams.signalMin,\
                                     degateParams.signalMax,degateParams.gatingMin,degateParams.gatingMax);
    //开启破膜保护
    emit EnableMembrokProtectSig(true,otherParams.membrSaturatedCurrent);
    //开启定时autounblock
    emit TempControlDegateSig(true);
    //开启 OVF Scan
    emit EnableOVFAutoScanSig(otherParams.oVFScanPeriod,otherParams.oVFScanTimes,true);
    //开启保存文件
    ConfigServer::GetInstance()->SetNeedGenerateRunId( m_iCycleCount > 1 ? false : true);
    emit SetDataSaveSignal(true);

    m_pAutoSequencyProgresBar->setValue(m_pAutoSequencyProgresBar->maximum());
    m_pAutoSequencyProgresBar->setFormat("finished Sequencing QC....%p%");

    if(bIsAutoTimerDegatRunning)
    {
        emit AutoTimerDegatControlSig(true); //恢复定时degating的计时
    }
    return true;
stop:
    LOGI("Auto Sequencing Stop!!!");
    return false;

}


void FlowOpertPanel::StopAllActivity(void)
{
    //关闭正在进行的活动
    m_pAutoScanPanel->OnStopAllActivity();
    emit TimerDegatEanbleSig(false);
    //停止保存文件
    emit SetDataSaveSignal(false);
    //关闭破膜保护
    emit EnableMembrokProtectSig(false,0);
    //关闭degating
    emit TempControlDegateSig(false);
    //关闭OVF Scan模块
    emit EnableOVFAutoScanSig(30,2,false);
}

bool FlowOpertPanel::ObserveWattingMuxFinal(const int &seconds,const bool &bCanel,int &reCnt)
{

    m_pObserveMuxFinalProgresDlg->setWindowModality(Qt::ApplicationModal);
    m_pObserveMuxFinalProgresDlg->reset();
    m_pObserveMuxFinalProgresDlg->setRange(0,seconds);
    m_pObserveMuxFinalProgresDlg->setValue(0);
    m_pObserveMuxFinalProgresDlg->show();

    m_pCusPlotCurvesCfgDlg->SetCurFiltType(PlotCfgSortFilterModel::FILTER_TYPE_AVG_STD);
    m_pCusPlotCurvesCfgDlg->show();
    for(int sec = 0; sec < seconds; ++sec)
    {
        if (m_pObserveMuxFinalProgresDlg->wasCanceled() || bCanel)
        {
            m_pObserveMuxFinalProgresDlg->cancel();
            return false;
        }
        ConfigServer::SleepForTimeoutOrStop(1000,bCanel);
        m_pObserveMuxFinalProgresDlg->setValue(sec+1);
    }

    m_pObserveMuxFinalProgresDlg->setValue(m_pObserveMuxFinalProgresDlg->maximum());
    reCnt = m_pCusPlotCurvesCfgDlg->GetFinalMuxCount();
    m_pCusPlotCurvesCfgDlg->hide();
    return true;
}

