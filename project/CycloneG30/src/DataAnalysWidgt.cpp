#include "DataAnalysWidgt.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QSignalMapper>

#include "CustomPlotWidgt.h"
#include "PoreStateMapWdgt.h"
#include "SavePannel.h"
#include "CustomSplitter.h"
#include "SensorPanel.h"
#include "VoltCtrPanel.h"
#include "PoreCtrlPanel.h"
#include "MuxScanPanel.h"
#include "DegatingPane.h"
#include "MembranePanel.h"
#include "ModelSharedData.h"
#include "PoreStateMapTabModel.h"
#include "AutoScanPanel.h"
#include "PlotCfgTabModel.h"
#include "ChartViewWidgt.h"
#include "FlowOpertPanel.h"



DataAnalysWidgt::DataAnalysWidgt(QWidget *parent):QWidget(parent)
{
    InitCtrl();
}

DataAnalysWidgt::~DataAnalysWidgt()
{
    delete m_pChartViewWidgt;
    delete m_pPoreStateMapWdgt;
    delete m_pCustomPlotWidgt;
    delete m_pTabSwitWidget;
    delete m_pSavePane;
    delete m_pSensorPanel;
    delete m_pVoltCtrPanel;
    delete m_pPoreCtrlPanel;
    delete m_pMembranePanel;
    delete m_pMuxScanPanel;
    delete m_pCtrolStackWidgt;
}


void DataAnalysWidgt::InitCtrl(void)
{
    //add pannels btn
    m_pModelSharedDataPtr = std::make_shared<ModelSharedData>();

    int iconWidth =  100;
    m_pCtrolBtnListWgt = new QListWidget(this);
    m_pCtrolBtnListWgt->setObjectName("CtrloBtnListWgt");
    m_pCtrolBtnListWgt->insertItem(ENUM_SAVE_PANNEL, new QListWidgetItem(QIcon(":/img/img/save.png"),tr("save"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->insertItem(ENUM_SENSOR_PANEL, new QListWidgetItem(QIcon(":/img/img/muxctrol.png"),tr("Mux"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->insertItem(ENUM_VOLTCTRL_PANEL, new QListWidgetItem(QIcon(":/img/img/voltage.png"),tr("Volt"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->insertItem(ENUM_POREINSERT_PANEL, new QListWidgetItem(QIcon(":/img/img/poreInsert.png"),tr("Pore"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->insertItem(ENUM_MUXSCAN_PANEL, new QListWidgetItem(QIcon(":/img/img/muxscan.png"),tr("MuxScan"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->insertItem(ENUM_MEMBRANE_ACTIVE_PANEL, new QListWidgetItem(QIcon(":/img/img/membrane.png"),tr("Simulate"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->insertItem(ENUM_AUTOSCAN_PANEL, new QListWidgetItem(QIcon(":/img/img/autoscan.png"),tr("AutoScan"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->insertItem(ENUM_FLOW_PANEL, new QListWidgetItem(QIcon(":/img/img/flow.png"),tr("FlowOper"),m_pCtrolBtnListWgt));
    m_pCtrolBtnListWgt->setIconSize(QSize(iconWidth, 40));
    m_pCtrolBtnListWgt->setFrameShape(QListWidget::NoFrame);
    m_pCtrolBtnListWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pCtrolBtnListWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pCtrolBtnListWgt->setTextElideMode(Qt::ElideMiddle);
    m_pCtrolBtnListWgt->setViewMode(QListView::IconMode);
    m_pCtrolBtnListWgt->setResizeMode(QListView::Adjust);
    m_pCtrolBtnListWgt->setDragEnabled(false);
    m_pCtrolBtnListWgt->setFixedWidth(m_pCtrolBtnListWgt->sizeHintForColumn(0));
    m_pCtrolBtnListWgt->setMaximumHeight(m_pCtrolBtnListWgt->sizeHintForRow(0)*(ENUM_FLOW_PANEL+1));


    //other pannel stackwdgt
    m_pCtrolStackWidgt = new QStackedWidget(this);
    m_pSavePane = new SavePannel(this);
    m_pSensorPanel = new SensorPanel(this);
    m_pVoltCtrPanel = new VoltCtrPanel(this);
    m_pPoreCtrlPanel = new PoreCtrlPanel(this);
    m_pDegatingPane = new DegatingPane(this);
    m_pMuxScanPanel = new MuxScanPanel(m_pDegatingPane,this);
    m_pMembranePanel = new MembranePanel(this);
    m_pAutoScanPanel = new AutoScanPanel(this);
    m_pFlowOpertPanel = new FlowOpertPanel(this);
    m_pCtrolStackWidgt->addWidget(m_pSavePane);
    m_pCtrolStackWidgt->addWidget(m_pSensorPanel);
    m_pCtrolStackWidgt->addWidget(m_pVoltCtrPanel);
    m_pCtrolStackWidgt->addWidget(m_pPoreCtrlPanel);
    m_pCtrolStackWidgt->addWidget(m_pMuxScanPanel);
    m_pCtrolStackWidgt->addWidget(m_pMembranePanel);
    m_pCtrolStackWidgt->addWidget(m_pAutoScanPanel);
    m_pCtrolStackWidgt->addWidget(m_pFlowOpertPanel);

    m_pTabSwitWidget = new QTabWidget(this);
    m_pCustomPlotWidgt= new CustomPlotWidgt(this);
    m_pTabSwitWidget->addTab(m_pCustomPlotWidgt,QIcon(":/img/img/curve.png"),QStringLiteral("Curves"));

    m_pPoreStateMapWdgt = new PoreStateMapWdgt(this);
    m_pTabSwitWidget->addTab(m_pPoreStateMapWdgt,QIcon(":/img/img/matrixmap.png"),QStringLiteral("Mapping"));
    m_pPoreStateMapWdgt->SetSensorPanelPtr(m_pSensorPanel);
    m_pPoreStateMapWdgt->GetPoreStateMapTab()->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pPoreStateMapWdgt->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pDegatingPane->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pSensorPanel->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pMuxScanPanel->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pCustomPlotWidgt->GetCusPlotCurvesCfgDlgPtr()->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pCustomPlotWidgt->GetCusPlotCurvesCfgDlgPtr()->GetPLotCfgTabModelPtr()->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pSensorPanel->SetPoreStateMapWdgtPtr(m_pPoreStateMapWdgt);
    m_pSensorPanel->SetSavePannelPtr(m_pSavePane);
    m_pMuxScanPanel->SetPoreStateMapWdgtPtr(m_pPoreStateMapWdgt);
    m_pMembranePanel->SetPoreStateMapWdgtPtr(m_pPoreStateMapWdgt);
    m_pAutoScanPanel->SetPoreStateMapWdgtPtr(m_pPoreStateMapWdgt);
    m_pFlowOpertPanel->SetPoreStateMapWdgtPtr(m_pPoreStateMapWdgt);
    m_pFlowOpertPanel->SetModelShareDataPtr(m_pModelSharedDataPtr);
    m_pFlowOpertPanel->SetSensorPanelPtr(m_pSensorPanel);
    m_pFlowOpertPanel->SetAutoScanPanelPtr(m_pAutoScanPanel);

    //
    m_pCustomPlotWidgt->GetCusPlotCurvesCfgDlgPtr()->SetPoreStateMapTabModelPtr(m_pPoreStateMapWdgt->GetPoreStateMapTab());
    m_pCustomPlotWidgt->GetCusPlotCurvesCfgDlgPtr()->SetSensorPanelPtr(m_pSensorPanel);
    m_pAutoScanPanel->SetCurvesCfgDlgPtr(m_pCustomPlotWidgt->GetCusPlotCurvesCfgDlgPtr());
    m_pFlowOpertPanel->SetCurvesCfgDlgPtr(m_pCustomPlotWidgt->GetCusPlotCurvesCfgDlgPtr());
    m_pDegatingPane->m_pPoreStateMapTab = m_pPoreStateMapWdgt->GetPoreStateMapTab();
    m_pDegatingPane->m_pSensorPane = m_pSensorPanel;

    m_pChartViewWidgt = new ChartViewWidgt(m_pPoreStateMapWdgt,this);
    m_pTabSwitWidget->addTab(m_pChartViewWidgt,QIcon(":/img/img/chart.png"),QStringLiteral("Chart"));
    m_pRightSplitter = new CustomSplitter(Qt::Vertical, SPLITTER_HIDE_DIRECT_BOTTOM,this);

    m_pRightSplitter->addWidget(m_pTabSwitWidget);
    m_pRightSplitter->addWidget(m_pCtrolStackWidgt);
    m_pRightSplitter->setStretchFactor(0, 20);
    m_pRightSplitter->setStretchFactor(1, 1);

    QVBoxLayout *vertlatout = new QVBoxLayout();
    vertlatout->addWidget(m_pCtrolBtnListWgt);
    vertlatout->addStretch();
    vertlatout->setStretch(0,1);

    QHBoxLayout *horlayout = new QHBoxLayout();
    horlayout->addLayout(vertlatout);
    horlayout->addWidget(m_pRightSplitter);
    horlayout->setStretch(0, 1);
    horlayout->setStretch(1, 20);
    setLayout(horlayout);

    connect(m_pSavePane, &SavePannel::setDegatingDataNameSignal, m_pDegatingPane, &DegatingPane::setDegatingDataNameSlot);
    connect(m_pPoreStateMapWdgt,&PoreStateMapWdgt::SigSpecificChanSetFunctState,m_pSensorPanel,&SensorPanel::SpecificChanSetFunctStateSlot);
    connect(m_pMuxScanPanel,&MuxScanPanel::SigAllChDifferFuncStateSet,m_pSensorPanel,&SensorPanel::AllChDifferFuncStateSetSlot);
    connect(m_pPoreStateMapWdgt,&PoreStateMapWdgt::SigAllChDifferFuncStateSet,m_pSensorPanel,&SensorPanel::AllChDifferFuncStateSetSlot);
    connect(m_pPoreStateMapWdgt,&PoreStateMapWdgt::SigGetPoreLimitCurrInp,m_pPoreCtrlPanel,&PoreCtrlPanel::SlotGetPoreLimitCurrInp);
    connect(m_pPoreStateMapWdgt,&PoreStateMapWdgt::SigGetMembraneimitCurrInp,m_pPoreCtrlPanel,&PoreCtrlPanel::SlotGetMembraneLimitCurrInp);
    connect(m_pPoreStateMapWdgt,&PoreStateMapWdgt::SigTriangularStartAndGetAmp,m_pVoltCtrPanel,&VoltCtrPanel::TrianStartAndGetAmplitudeSlot);
    connect(m_pPoreStateMapWdgt,&PoreStateMapWdgt::SigGetStdLimitInp,m_pPoreCtrlPanel,&PoreCtrlPanel::SlotGetStdLimitInput);
    connect(m_pCtrolBtnListWgt,&QListWidget::clicked,this,&DataAnalysWidgt::SelectPanelIndxSlot);
    connect(m_pSensorPanel,&SensorPanel::SigClearPoreAndCapState,m_pPoreStateMapWdgt,&PoreStateMapWdgt::ClearPoreAndCapStateSlot);
    connect(m_pPoreCtrlPanel,&PoreCtrlPanel::SigUpdateSensorState,m_pPoreStateMapWdgt,&PoreStateMapWdgt::UpdateSensorStateSlot);

    connect(m_pMuxScanPanel,&MuxScanPanel::VoltageSetSig,m_pVoltCtrPanel,&VoltCtrPanel::VoltageSetSlot);
    connect(m_pMuxScanPanel,&MuxScanPanel::SetDataSaveSignal,m_pSavePane,&SavePannel::SetDataSaveSlot);
    connect(m_pMuxScanPanel,&MuxScanPanel::UpdateAllchSaveSig,m_pSavePane,&SavePannel::SetDataIsSaveSlot);
    connect(m_pMembranePanel,&MembranePanel::EnableSimulationSig,m_pSensorPanel,&SensorPanel::EnableSimulationSlot);
    connect(m_pMembranePanel,&MembranePanel::EnableSimulationSig,m_pVoltCtrPanel,&VoltCtrPanel::EnableSimulationSlot);
    connect(m_pMembranePanel,&MembranePanel::SetSimulationVoltSig,m_pVoltCtrPanel,&VoltCtrPanel::SetSimulationVoltSlot);
    connect(m_pMembranePanel,&MembranePanel::EnablePoreProtectSig,m_pPoreCtrlPanel,&PoreCtrlPanel::EnablePoreProtectSlot);
    connect(m_pMembranePanel,&MembranePanel::EnablePolymerSimulationSig,m_pVoltCtrPanel,&VoltCtrPanel::EnablePolymerSimulationSlot);
    connect(m_pMembranePanel,&MembranePanel::SensorGroupAndFuncChangeSig,m_pSensorPanel,&SensorPanel::SensorGroupAndFuncChangeSlot);
    connect(m_pVoltCtrPanel,&VoltCtrPanel::AllChanSensorStateSetOffSig,m_pSensorPanel,&SensorPanel::AllChanSensorStateSetOffSlot);
    connect(m_pPoreStateMapWdgt,&PoreStateMapWdgt::AcquireWhetherPoreLockSig,m_pPoreCtrlPanel,&PoreCtrlPanel::AcquireWhetherPoreLockSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::VoltageSetSig,m_pVoltCtrPanel,&VoltCtrPanel::VoltageSetSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::DegatVoltSetSig,m_pVoltCtrPanel,&VoltCtrPanel::DegatVoltSetSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::EnablePoreProtectSig,m_pPoreCtrlPanel,&PoreCtrlPanel::EnablePoreProtectSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::SensorGroupChangeSig,m_pSensorPanel,&SensorPanel::SensorGroupChangeSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::OnUnblockAllChannelSig,m_pSensorPanel,&SensorPanel::OnUnblockAllChannelSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::TempControlDegateSig,m_pDegatingPane,&DegatingPane::TempControlDegateSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::UpdateDegatingParametersSig,m_pDegatingPane,&DegatingPane::UpdateDegatingParametersSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::UpdateDegateBaseParametersSig,m_pDegatingPane,&DegatingPane::UpdateDegateBaseParametersSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::GenerateMuxModeSig,m_pPoreStateMapWdgt,&PoreStateMapWdgt::GenerateMuxModeSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::AcquireDegateIsRunningSig,m_pDegatingPane,&DegatingPane::AcquireDegateIsRunningSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::SetDataSaveSignal,m_pSavePane,&SavePannel::SetDataSaveSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::GetDataSaveDirIsEmptySig,m_pSavePane,&SavePannel::GetDataSaveDirIsEmptySlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::AcquireAutoTimerDegatIsRunningSig,m_pSensorPanel,&SensorPanel::AcquireAutoTimerDegatIsRunningSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::AutoTimerDegatControlSig,m_pSensorPanel,&SensorPanel::AutoTimerDegatControlSlot);
    connect(m_pSensorPanel,&SensorPanel::AcquireAutoScanIsRunningSig,m_pAutoScanPanel,&AutoScanPanel::AcquireAutoScanIsRunningSlot);
    connect(m_pSensorPanel,&SensorPanel::AutoScanControlPauseSig,m_pAutoScanPanel,&AutoScanPanel::AutoScanControlPauseSlot);
    connect(m_pSensorPanel,&SensorPanel::AcquireDegateDelaymsSig,m_pDegatingPane,&DegatingPane::AcquireDegateDelaymsSlot);
    connect(m_pSensorPanel,&SensorPanel::AcquireDegateIsRunningSig,m_pDegatingPane,&DegatingPane::AcquireDegateIsRunningSlot);
    connect(m_pSensorPanel,&SensorPanel::TempControlDegateSig,m_pDegatingPane,&DegatingPane::TempControlDegateSlot);
    connect(m_pCustomPlotWidgt->GetCusPlotCurvesCfgDlgPtr(),&CusPlotCurvesCfgDlg::UpdateCountValidStateCntSig,m_pPoreStateMapWdgt,&PoreStateMapWdgt::UpdateValidStateCntSlot);
    connect(m_pChartViewWidgt,&ChartViewWidgt::OnRefreshCountSig,m_pPoreStateMapWdgt,&PoreStateMapWdgt::OnRefreshCountSlot);
    connect(m_pChartViewWidgt,&ChartViewWidgt::AcquireCurReadsApatersSig,m_pSavePane,&SavePannel::AcquireCurReadsApatersSlot);
    connect(m_pAutoScanPanel,&AutoScanPanel::NotifyRecordOperateSig,m_pChartViewWidgt,&ChartViewWidgt::NotifyRecordOperateSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::GetDataSaveDirIsEmptySig,m_pSavePane,&SavePannel::GetDataSaveDirIsEmptySlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::NotifyRecordOperateSig,m_pChartViewWidgt,&ChartViewWidgt::NotifyRecordOperateSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::VoltageSetSig,m_pVoltCtrPanel,&VoltCtrPanel::VoltageSetSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::SensorGroupChangeSig,m_pSensorPanel,&SensorPanel::SensorGroupChangeSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::VoltSetParametersSig,m_pVoltCtrPanel,&VoltCtrPanel::VoltSetParameters);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::AcquireAutoTimerDegatIsRunningSig,m_pSensorPanel,&SensorPanel::AcquireAutoTimerDegatIsRunningSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::AutoTimerDegatControlSig,m_pSensorPanel,&SensorPanel::AutoTimerDegatControlSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::SetDataSaveSignal,m_pSavePane,&SavePannel::SetDataSaveSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::EnableMembrokProtectSig,m_pPoreCtrlPanel,&PoreCtrlPanel::EnableMembrokProtectSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::TimerDegatEanbleSig,m_pSensorPanel,&SensorPanel::TimerDegatEanbleSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::TempControlDegateSig,m_pDegatingPane,&DegatingPane::TempControlDegateSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::UpdateDegatingParametersSig,m_pDegatingPane,&DegatingPane::UpdateDegatingParametersSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::UpdateDegateBaseParametersSig,m_pDegatingPane,&DegatingPane::UpdateDegateBaseParametersSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::AcquireDegateIsRunningSig,m_pDegatingPane,&DegatingPane::AcquireDegateIsRunningSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::SendFinalMuxSig,m_pChartViewWidgt,&ChartViewWidgt::SendFinalMuxSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::AllChanSensorStateSetOffSig,m_pSensorPanel,&SensorPanel::AllChanSensorStateSetOffSlot);
    connect(m_pFlowOpertPanel,&FlowOpertPanel::EnableOVFAutoScanSig,m_pPoreStateMapWdgt,&PoreStateMapWdgt::EnableOVFAutoScanSlot);
}

void DataAnalysWidgt::SelectPanelIndxSlot(const QModelIndex &index)
{
    int panIndx = index.row();
    if(panIndx != m_pCtrolStackWidgt->currentIndex())
    {
        if(!m_pRightSplitter->GetWgtIsVisible())
        {
            m_pRightSplitter->ToggleShowHideClick();
        }
        m_pCtrolStackWidgt->setCurrentIndex(panIndx);
    }
    else
    {
        m_pRightSplitter->ToggleShowHideClick();
    }
}

