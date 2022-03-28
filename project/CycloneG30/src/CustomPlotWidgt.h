#pragma once

#include <QWidget>
#include "customplot/qcustomplot.h"

#include <concurrentqueue.h>
#include "ConfigServer.h"
#include <shared_mutex>
#include "ModelSharedData.h"

class CusPlotCurvesCfgDlg;
class PLotCfgTabModel;

class CustomPlotWidgt:public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlotWidgt(QWidget *parent = nullptr);
    ~CustomPlotWidgt();

public:
    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);

public slots:
    void OnEnableOpengl(const bool& check);

private:
    QCustomPlot* m_pCustomPlot = nullptr;

    QLabel* m_plabYmax = nullptr;
    QDoubleSpinBox* m_pdspYmax =nullptr;
    QLabel* m_plabYmin = nullptr;
    QDoubleSpinBox* m_pdspYmin =nullptr;
    QLabel* m_plabXtime = nullptr;
    QDoubleSpinBox* m_pdspXtime = nullptr;

    QLabel* m_pLabChStartIndex = nullptr;
    QLabel* m_pLabChEndIndex = nullptr;
    QSpinBox *m_pSpChStartIndex = nullptr;
    QSpinBox *m_pSpChEndIndex = nullptr;

    QPushButton *m_pAixsSetBtn = nullptr;
    QPushButton *m_pChanelSetBtn = nullptr;

    QToolButton *m_pPlotCfgToolBtn = nullptr;
    QAction *m_pPlotColorVisAct = nullptr;

    CusPlotCurvesCfgDlg *m_pCusPlotCurvesCfgDlg = nullptr;
    PLotCfgTabModel *m_pPLotCfgTabModel = nullptr;

    double m_dSampleRate;
    double m_dPintNum;
    double m_dAxisUnit;
    const double m_cdYmax = 999999.0;
    const double m_cdXmax = 20.0;//99999999.0;

    double m_arrayCurRecod[CHANNEL_NUM];

private:
    void InitCtrl(void);
    bool InitPlot(void);
    bool AddPointsToGrpah(const int &indx, const int &block,std::shared_ptr<ConfigServer::SDataHandle> dtpk);

private:
    bool m_bRunning = false;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;

public:
    volatile int waiter_cnt = 0;
    std::condition_variable m_cv;
    std::mutex m_mutexTask;
    std::mutex m_lock;

    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataHandle;


signals:
    void replotSignal(bool isClear);
    void GetAllChAvgSig(std::vector<float> &vetAvg);
    void GetAllChStdSig(std::vector<float> &vetStd);


private slots:
    void replotSlot(bool isClear);
    void OnPlotApply(void);
    void OnChannelSetBtnSlot(void);

    void CurveVisibleChgeSlot(int ch,bool visible);
    void CurveColorChgeSlot(int ch,QColor color);
    void AllCurvesVisibleSlot(const bool &chk);
public slots:
    void OnCurvesCfgClkSlot(void);

public:
    bool StartThread(void);
    void ThreadFunc(void);
    bool EndThread(void);
    bool StopThread(void);

    void OnSampRateChange(void);
    std::vector<bool> &GetAllChSeqSteRef(void);
    std::vector<bool> &GetAllChVisibleRef(void);
    void UpdateAllChVisibleView(const std::vector<bool> &vctVisible);

    CusPlotCurvesCfgDlg *GetCusPlotCurvesCfgDlgPtr(void)const {return m_pCusPlotCurvesCfgDlg;}

};

class PLotCfgTabModel;
class PoreStateMapTabModel;
class SensorPanel;
class PlotCfgSortFilterModel;
class CusPlotCurvesCfgDlg : public QDialog
{
    Q_OBJECT
public:
    CusPlotCurvesCfgDlg(CustomPlotWidgt *pCutomplotwgt,QWidget *parent);
    ~CusPlotCurvesCfgDlg();

    enum VIVISBLE_TRIGER_ENUM{
        ALL_CHAN_INVIVISBLE_TRIGER_ENUM = 0,
        ALL_CHAN_VIVISBLE_TRIGER_ENUM,
        AVGSTD_VIVISBLE_TRIGER_ENUM,
        SEQ_VIVISBLE_TRIGER_ENUM,
        VALID_VIVISBLE_TRIGER_ENUM};
private:
    PlotCfgSortFilterModel  *m_pSortFilterProxyModel = nullptr;
    QTableView *m_pCurvesCfgTbleView = nullptr;
    PLotCfgTabModel *m_pPlotCfgModel = nullptr;
    QStyledItemDelegate *m_pPlotCfgDelegate= nullptr;

    QComboBox *m_pFiltTypeSelectComb = nullptr;
    QLabel *m_pCurFiltRowsCntLab =nullptr;


    QCheckBox *m_pAllVisChk = nullptr;
    QTimer *m_pTmrUpdateAvgStd = nullptr;
    CustomPlotWidgt *m_pCustomPlotWidgt = nullptr;

    QPushButton *m_pAutoRefreshBtn =nullptr;

    QGroupBox *m_ctrlgroupbox = nullptr;
    QDoubleSpinBox *m_pAutoLockLimtAvgDpbox = nullptr;
    QDoubleSpinBox *m_pAutoLockLimtStdDpbox = nullptr;

    QDoubleSpinBox* m_pFilterCurMinDpbx= nullptr;
    QDoubleSpinBox* m_pFilterCurMaxDpbx= nullptr;
    QDoubleSpinBox* m_pFilterStdMinDpbx= nullptr;
    QDoubleSpinBox* m_pFilterStdMaxDpbx= nullptr;
    QCheckBox *m_pOpenPoreFiltChkbox = nullptr;
    QDoubleSpinBox* m_pOpenPoreFiltCurMinDpbx= nullptr;
    QDoubleSpinBox* m_pOpenPoreFiltCurMaxDpbx= nullptr;
    QDoubleSpinBox* m_pOpenPoreFiltStdMinDpbx= nullptr;
    QDoubleSpinBox* m_pOpenPoreFiltStdMaxDpbx= nullptr;
    QPushButton *m_pFiltShutoffBtn= nullptr;
    QPushButton *m_pFiltLockBtn= nullptr;
    QPushButton *m_pFiltHideOutRangeBtn = nullptr;
    QPushButton *m_pFiltValidBtn = nullptr;
    QPushButton *m_pFiltValidShutOtherBtn = nullptr;
    QMenu *m_pCfgMenu = nullptr;
    QMenu *m_pVisibleMenu = nullptr;
    QAction *m_pAllChanInVisiAction = nullptr;
    QAction *m_pAllChanVisiAction = nullptr;
    QAction *m_pAvgStdVisiAction = nullptr;
    QAction *m_pSeqVisiAction = nullptr;
    QAction *m_pvalidVisiAction = nullptr;
    QSignalMapper *m_pVisibleOrNotMapper = nullptr;

    int m_curFilterType;

signals:
    void CurveVisibleChgeSig(int ch,bool visible);
    void CurveColorChgeSig(int ch,QColor color);
    void AllCurvesVisibleSig(const bool &chk);

    void GetAllChAvgSig(std::vector<float> &vetAvg);
    void GetAllChStdSig(std::vector<float> &vetStd);


private slots:
    void DataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void AllChannelVisiChkSlot(const bool &chk);
    void TimeoutGetAllChAvgStd(void);
    void OnAutoRefreshAvgStdBtnSlot(const bool &checked);
    void OnFiltApplyBtnSlot(void);
    void OnFiltHideOutRangeBtnSlot(void);
    void TabViewCreatMenuSlot(const QPoint & p);
    void OnChgeVisibleActMapSlot(const int & actionId);
    void ChangeFiltTypeSlot(const int &type);

private:
    void InitCtrl(void);
    void StartAcquireAvgStdTimer(const int mils);
    void StopAcquireAvgStdTimer(void);
    void OnLockUnqualifiedChBySTD(const double &limtAvg,const double &limtStd);
    void OnFiltChanByAvgStdInTab(const std::vector<float> &vctAvg,const std::vector<float> &vctStd);
public slots:
    void OnValidApplyBtnSlot(void);
    void OnFiltValidShutOtherBtnSlot(void);
private:
    PoreStateMapTabModel* m_pPoreStateMapTab = nullptr;
    SensorPanel* m_pSensorPanel = nullptr;
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;
signals:
    void UpdateCountValidStateCntSig(void);
public:

    void UpdateVisiModelData(const std::vector<bool> &vetIsVisible);
    void UpdateAvgStdParameterSlot(const float &avgMin,const float &avgMax,const float &stdMin,const float &stdMax);
    void SetCurFiltType(const int &type);

    PLotCfgTabModel *GetPLotCfgTabModelPtr(void)const {return m_pPlotCfgModel;}
    void EnableAvgStdAutoRefresh(const bool &enable);

    void SetPoreStateMapTabModelPtr(PoreStateMapTabModel *pPoreStateMapTabModel){ m_pPoreStateMapTab = pPoreStateMapTabModel;}
    void SetSensorPanelPtr(SensorPanel *pSensorPanel){ m_pSensorPanel = pSensorPanel;}
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }
    std::shared_ptr<ModelSharedData> GetModelShareDataPt(void){return m_pModelSharedDataPtr; }

    void OnLockChGreaterAvgOrStdLimit(const float &avgLimt,const float &stdLimt);
    int GetFinalMuxCount(void);

};

