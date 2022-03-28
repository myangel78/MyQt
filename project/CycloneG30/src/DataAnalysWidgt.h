#pragma once

#include <QWidget>
class QToolButton;
class QGroupBox;
class CustomSplitter;
class QStackedWidget;
class QTabWidget;
class QSignalMapper;
class CustomPlotWidgt;
class SavePannel;
class SensorPanel;
class VoltCtrPanel;
class PoreCtrlPanel;
class DegatingPane;
class PoreStateMapWdgt;
class QListWidget;
class MuxScanPanel;
class MembranePanel;
class ModelSharedData;
class AutoScanPanel;
class ChartViewWidgt;
class FlowOpertPanel;

class DataAnalysWidgt:public QWidget
{
    Q_OBJECT
public:
    explicit DataAnalysWidgt(QWidget *parent = nullptr);
    ~DataAnalysWidgt();


    CustomPlotWidgt* GetCustomPlotWidgt(void) const { return m_pCustomPlotWidgt;}
    SavePannel* GetSavePanelWdgt(void) const { return m_pSavePane;}
    SensorPanel* GetSensorPanelWdgt(void) const { return m_pSensorPanel;}
    VoltCtrPanel* GetVoltCtrlPanelWdgt(void) const { return m_pVoltCtrPanel;}
    PoreCtrlPanel *GetPoreCtrlPanelWgt(void) const {return m_pPoreCtrlPanel;}
    DegatingPane*GetDegatingPanelWgt(void) const {return m_pDegatingPane;}
    PoreStateMapWdgt *GetPoreStateMapWgt(void) const {return m_pPoreStateMapWdgt;}
    MuxScanPanel *GetMuxScanPanelWgt(void) const {return m_pMuxScanPanel;}
    AutoScanPanel *GetAutoScanPanelWgt(void) const {return m_pAutoScanPanel;}
    ChartViewWidgt *GetChartViewWgt(void) const {return m_pChartViewWidgt;}
    FlowOpertPanel *GetFlowOpertWgt(void) const {return m_pFlowOpertPanel;}

private:
    typedef enum
    {
        ENUM_SAVE_PANNEL = 0,
        ENUM_SENSOR_PANEL = 1,
        ENUM_VOLTCTRL_PANEL = 2,
        ENUM_POREINSERT_PANEL = 3,
        ENUM_MUXSCAN_PANEL = 4,
        ENUM_MEMBRANE_ACTIVE_PANEL = 5,
        ENUM_AUTOSCAN_PANEL = 6,
        ENUM_FLOW_PANEL = 7,
    }ENUM_CONTROL_PANNEL;

    //pannel tools btn
    QListWidget *m_pCtrolBtnListWgt = nullptr;
    //other pannel stackwdgt
    QStackedWidget *m_pCtrolStackWidgt = nullptr;
    CustomSplitter *m_pRightSplitter = nullptr;
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;

    QTabWidget *m_pTabSwitWidget = nullptr;
    CustomPlotWidgt *m_pCustomPlotWidgt = nullptr;
    //Pore state mapping widgt
    PoreStateMapWdgt *m_pPoreStateMapWdgt= nullptr;
    //ChartView wdiget
    ChartViewWidgt *m_pChartViewWidgt = nullptr;

    //save pannel
    SavePannel *m_pSavePane = nullptr;
    //sensor control pannel
    SensorPanel *m_pSensorPanel = nullptr;
    //voltage control pannel
    VoltCtrPanel *m_pVoltCtrPanel = nullptr;
    //pore insert control pannel
    PoreCtrlPanel *m_pPoreCtrlPanel =nullptr;
    MuxScanPanel *m_pMuxScanPanel =nullptr;
    //degating pannel
    DegatingPane* m_pDegatingPane = nullptr;
    //Membrane pannel
    MembranePanel *m_pMembranePanel = nullptr;
    //AutoScan pannel
    AutoScanPanel *m_pAutoScanPanel = nullptr;
    //Flow operate pannel
    FlowOpertPanel *m_pFlowOpertPanel = nullptr;

private:
    void InitCtrl(void);
private slots:
    void SelectPanelIndxSlot(const QModelIndex &index);
};

