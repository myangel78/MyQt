#pragma once

#include <QWidget>
#include "Global.h"

#include <QtCharts>
class QPushButton;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLabel;
class QGroupBox;
class QTabWidget;
class PoreStateMapWdgt;
class RecordFile;

class PercentBarChartView;




class ChartViewWidgt:public QWidget
{
    Q_OBJECT
public:
    ChartViewWidgt(PoreStateMapWdgt *pPoreStateMapWdgt,QWidget *parent);
    ~ChartViewWidgt();


    enum ENUM_CHART_VIEW_PANEL
    {
        ENUM_CHART_PORE_PANNEL = 0,
        ENUM_CHART_VALID_PANNEL = 1,
        ENUM_CHART_READS_ADAPTERS_PANNEL = 2,
    };
private:
    void InitCtrl(void);
    void InitPoreChartCtrl(void);
    void InitValidChartCtrl(void);
    void InitReadsAdaptersChartCtrl(void);

    void ReAdjustReasChart(void);
    void AddPointsToReadsAdapterChart(const QVector<QPointF> &vctReadsPoints,const QVector<QPointF> &vctAdaptersPoints);

private:
    QGroupBox *m_pPoreChartGrpbx = nullptr;
    QGroupBox *m_pValidChartGrpbx = nullptr;
    QGroupBox *m_pReadsAdaptersChartGrpbx = nullptr;


     //pore chart
    QWidget *m_pPoreChartWdgt = nullptr;
    QChart *m_pPoreChart = nullptr;
    QChartView *m_pPoreChartView = nullptr;
    QVector<QBarSet *> m_vctPoreBarset;
    QPercentBarSeries *m_pPoreSeries = nullptr;

     //valid chart
    QWidget *m_pValidChartWdgt = nullptr;
    std::shared_ptr<PercentBarChartView> m_validBarChartView;

    //Reads adapters line Chart
    enum ENUM_READS_ADAPTERS_SPLINES
    {
        ENUM_READS_PER_SPLINES = 0,
        ENUM_ADAPTERS_PER_SPLINES = 1,
        ENUM_READS_SUM_SPLINES = 2,
        ENUM_ADAPTERS_SUM_SPLINES = 3,
    };
    QWidget *m_pReadsAdaptersChartWdgt = nullptr;
    QChart *m_pReadsAdaptersChart = nullptr;
    QChartView *m_pReadsAdaptersChartView = nullptr;
    QVector<QSplineSeries *>m_vctReadsAdapterSplines;
    QDateTime m_readsStartPlotDateTIme;
    QDateTime m_readsLastPlotDateTIme;
    QDateTimeAxis *m_pReadAdaptersAxisX = nullptr;
    QValueAxis *m_pReadAdaptersAxisY = nullptr;
    long long m_maxReadsPer;
    long long m_maxReadsSum;
    long long m_maxAdaptersPer;
    long long m_maxAdaptersSum;

    QPushButton *m_pReadsFileLoadBtn =nullptr;
    QLineEdit *m_pReadsLoadPathEdit= nullptr;
    QGroupBox *m_pReadControlgrpbx = nullptr;
    bool m_bReadsOrAdapterDisp;

    QComboBox *m_pThemeComb = nullptr;
    QComboBox *m_pAnimationsComb = nullptr;
    QComboBox *m_pLegendComb = nullptr;
    QCheckBox *m_pAliasingChk = nullptr;

    QTabWidget *m_pChartTabWdgt = nullptr;
    QPushButton *m_pUpdateChartBtn =nullptr;
    QList<QChartView *> m_charts;
    QStackedWidget *m_pControlStackWgt = nullptr;

    RecordFile *m_pReadsAdapterRecord = nullptr;
    RecordFile *m_pValidRecord = nullptr;
    RecordFile *m_pPoreRecord = nullptr;

    bool m_bNeedObserMuxFinalRecord =false;
    int m_observeMuxFinal = 0;



private:
    PoreStateMapWdgt *m_pPoreStateMapWdgt = nullptr;
private:
    void ReadsAdaptersRecord(void);
    void ValidCountRecord(void);
    void PoreCountRecord(void);
    void OpenRecordFileByType(const RECORD_FILE_TYPE_ENUM &type);
    void CloseRecordFileByType(const RECORD_FILE_TYPE_ENUM &type);
    void RecordFileByType(const RECORD_FILE_TYPE_ENUM &type);

private slots:
    void ReadsFileLoadBtnSlot(void);

public slots:
    void UpdatePoreChart(void);
    void UpdateValidChart(void);
    void UpdateReadsChart(void);

    void UpdateChartBtnSlot(void);
    void UpdateChartUIStyle(void);

    void NotifyRecordOperateSlot(const RECORD_FILE_TYPE_ENUM &type,const RECORD_FILE_OPERT_TYPE_ENUM &operate);
    void SendFinalMuxSlot(const bool &NeedObserRecord,const int &finalMuxCnt);
signals:
    void OnRefreshCountSig(const STATE_MAP_MODE &state);
    void AcquireCurReadsApatersSig(long long &reads,long long &adapters);
public:
    void OpenRecordFile(const bool &enable);
};
