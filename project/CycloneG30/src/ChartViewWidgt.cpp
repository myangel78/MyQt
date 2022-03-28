#include "ChartViewWidgt.h"

#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QtCharts>
#include <QDateTime>
#include "Global.h"
#include "ConfigServer.h"
#include "Log.h"
#include "PoreStateMapWdgt.h"
#include "UsbProtoMsg.h"
#include "RecordCsvFile.h"
#include "PercentBarChartView.h"



ChartViewWidgt::ChartViewWidgt(PoreStateMapWdgt *pPoreStateMapWdgt,QWidget *parent):m_pPoreStateMapWdgt(pPoreStateMapWdgt),QWidget(parent)
{
    InitCtrl();

}

ChartViewWidgt::~ChartViewWidgt()
{
    for(auto barset: m_vctPoreBarset)
    {
        delete barset;
    }

    for(auto splines: m_vctReadsAdapterSplines)
    {
        delete splines;
    }

}


void ChartViewWidgt::InitCtrl(void)
{
    QGroupBox *pChartAreaGrpbx = new QGroupBox(QStringLiteral("Chart"),this);
    QGroupBox *pAllAreaGrpbx = new QGroupBox(QStringLiteral("All"),this);

    m_pControlStackWgt = new QStackedWidget(this);


    QLabel *pThemeLab = new QLabel(QStringLiteral("Theme"),this);
    m_pThemeComb = new QComboBox(this);
    m_pThemeComb->addItem("Light", QChart::ChartThemeLight);
    m_pThemeComb->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    m_pThemeComb->addItem("Dark", QChart::ChartThemeDark);
    m_pThemeComb->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    m_pThemeComb->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    m_pThemeComb->addItem("High Contrast", QChart::ChartThemeHighContrast);
    m_pThemeComb->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    m_pThemeComb->addItem("Qt", QChart::ChartThemeQt);
//    m_pThemeComb->setCurrentText(QStringLiteral("Blue Cerulean"));
    m_pThemeComb->setCurrentText(QStringLiteral("Blue NCS"));


    QLabel *pAnimationsLab = new QLabel(QStringLiteral("Animations"),this);
    m_pAnimationsComb = new QComboBox(this);
    m_pAnimationsComb->addItem("No Animations", QChart::NoAnimation);
    m_pAnimationsComb->addItem("GridAxis Animations", QChart::GridAxisAnimations);
    m_pAnimationsComb->addItem("Series Animations", QChart::SeriesAnimations);
    m_pAnimationsComb->addItem("All Animations", QChart::AllAnimations);
    m_pAnimationsComb->setCurrentText(QStringLiteral("All Animations"));


    QLabel *pLegendLab = new QLabel(QStringLiteral("Legend"),this);
    m_pLegendComb = new QComboBox(this);
    m_pLegendComb->addItem("No Legend ", 0);
    m_pLegendComb->addItem("Legend Top", Qt::AlignTop);
    m_pLegendComb->addItem("Legend Bottom", Qt::AlignBottom);
    m_pLegendComb->addItem("Legend Left", Qt::AlignLeft);
    m_pLegendComb->addItem("Legend Right", Qt::AlignRight);
    m_pLegendComb->setCurrentText(QStringLiteral("Legend Right"));


    m_pAliasingChk = new QCheckBox(QStringLiteral("Aliasing"));
    m_pAliasingChk->setChecked(true);
    m_pUpdateChartBtn = new QPushButton(QStringLiteral("Update"),this);

    InitPoreChartCtrl();
    InitValidChartCtrl();
    InitReadsAdaptersChartCtrl();
    m_pChartTabWdgt = new QTabWidget(this);
    m_pChartTabWdgt->insertTab(ENUM_CHART_PORE_PANNEL,m_pPoreChartWdgt,QStringLiteral("Pore"));
    m_pChartTabWdgt->insertTab(ENUM_CHART_VALID_PANNEL,m_pValidChartWdgt,QStringLiteral("Valid"));
    m_pChartTabWdgt->insertTab(ENUM_CHART_READS_ADAPTERS_PANNEL,m_pReadsAdaptersChartWdgt,QStringLiteral("Reads"));
    m_pChartTabWdgt->setTabPosition(QTabWidget::East);
//    m_pChartTabWdgt->setDocumentMode(true);

    m_pControlStackWgt->addWidget(m_pReadControlgrpbx);


    QHBoxLayout *pcharhorlayout = new QHBoxLayout();
    pcharhorlayout->addWidget(m_pChartTabWdgt);
    pChartAreaGrpbx->setLayout(pcharhorlayout);

    QGridLayout *pctrlGridlayout = new QGridLayout();
    pctrlGridlayout->addWidget(pThemeLab,0,0,1,1);
    pctrlGridlayout->addWidget(m_pThemeComb,0,1,1,1);
    pctrlGridlayout->addWidget(pAnimationsLab,1,0,1,1);
    pctrlGridlayout->addWidget(m_pAnimationsComb,1,1,1,1);
    pctrlGridlayout->addWidget(pLegendLab,2,0,1,1);
    pctrlGridlayout->addWidget(m_pLegendComb,2,1,1,1);
    pctrlGridlayout->addWidget(m_pAliasingChk,3,0,1,1);
    pctrlGridlayout->addWidget(m_pUpdateChartBtn,4,0,1,1);
    pctrlGridlayout->setRowStretch(5,10);
    pAllAreaGrpbx->setLayout(pctrlGridlayout);


    QVBoxLayout *vertlayout = new QVBoxLayout();
    vertlayout->addWidget(pAllAreaGrpbx);
    vertlayout->addWidget(m_pControlStackWgt);
    vertlayout->setStretch(0,1);
    vertlayout->setStretch(1,10);

    QHBoxLayout *horlayout = new QHBoxLayout();
    horlayout->addWidget(pChartAreaGrpbx);
    horlayout->addLayout(vertlayout);
    horlayout->setStretch(0,10);
    horlayout->setStretch(1,1);
    setLayout(horlayout);

    connect(m_pUpdateChartBtn,&QPushButton::clicked,this,&ChartViewWidgt::UpdateChartBtnSlot);
    connect(m_pAnimationsComb,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ChartViewWidgt::UpdateChartUIStyle);
    connect(m_pThemeComb,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ChartViewWidgt::UpdateChartUIStyle);
    connect(m_pLegendComb,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ChartViewWidgt::UpdateChartUIStyle);
    connect(m_pChartTabWdgt,&QTabWidget::currentChanged,[=](const int &index){
        if (index == ENUM_CHART_READS_ADAPTERS_PANNEL)
        {
            m_pControlStackWgt->setCurrentWidget(m_pReadControlgrpbx);
        }});

#if 0
    m_pThemeComb->setVisible(false);
    pAnimationsLab->setVisible(false);
    m_pAnimationsComb->setVisible(false);
    pLegendLab->setVisible(false);
    m_pLegendComb->setVisible(false);
    m_pAliasingChk->setVisible(false);
#endif
    UpdateChartUIStyle();

}

void ChartViewWidgt::InitPoreChartCtrl(void)
{
    m_pPoreChartWdgt = new QWidget(this);

    m_pPoreChart = new QChart();
    m_pPoreChart->setTitle(QStringLiteral("Pore State"));

    m_vctPoreBarset.resize(PORE_STATE_NUM);
    for(int porestate = 0; porestate < PORE_STATE_NUM; ++porestate)
    {
        m_vctPoreBarset[porestate] = new QBarSet(QString::fromStdString(gPoreStateStr[porestate]));
    }

    m_pPoreSeries = new QPercentBarSeries(m_pPoreChart);


    for(int porestate = 0; porestate < PORE_STATE_NUM; ++porestate)
    {
        for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
        {
            *m_vctPoreBarset[porestate]<< (sensorGrp+1);
        }
        m_pPoreSeries->append(m_vctPoreBarset[porestate]);
    }
    m_pPoreChart->addSeries(m_pPoreSeries);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(SENSRO_FULLSTR);
    m_pPoreChart->addAxis(axisX, Qt::AlignBottom);
    m_pPoreSeries->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
//    axisY->setRange(0,100);
    axisY->setLabelFormat("%.1f%  ");
    m_pPoreChart->addAxis(axisY, Qt::AlignLeft);
    m_pPoreSeries->attachAxis(axisY);

    m_pPoreChartView = new QChartView(m_pPoreChart);
    m_charts.push_back(m_pPoreChartView);

    QHBoxLayout *pporelayout = new QHBoxLayout();
    pporelayout->addWidget(m_pPoreChartView);
    m_pPoreChartWdgt->setLayout(pporelayout);
}

void ChartViewWidgt::InitValidChartCtrl(void)
{

    m_pValidChartWdgt = new QWidget(this);
    QStringList strBarSetList = {"Invalid","Valid" };
    m_validBarChartView = std::make_shared<PercentBarChartView>(QStringLiteral("Valid State"),strBarSetList,this);

    std::vector<int> vctBarsetValue(VALID_STATE_NUM,512);
    for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp )
    {
        m_validBarChartView->AppendData(SENSRO_FULLSTR[sensorGrp],vctBarsetValue);
    }

    m_charts.push_back(m_validBarChartView->GetChartView());
    QHBoxLayout *pvalidlayout = new QHBoxLayout();
    pvalidlayout->addWidget(m_validBarChartView.get());
    m_pValidChartWdgt->setLayout(pvalidlayout);

}

void ChartViewWidgt::InitReadsAdaptersChartCtrl(void)
{

    m_maxReadsPer = 10;
    m_maxReadsSum = 10;
    m_maxAdaptersPer = 20;
    m_maxAdaptersSum = 10;

    m_bReadsOrAdapterDisp = true;

    m_pReadsAdaptersChartWdgt = new QWidget(this);

    m_pReadsAdaptersChart = new QChart();
    m_pReadsAdaptersChart->setTitle(QStringLiteral("Reads and Adapters"));
    m_readsStartPlotDateTIme = QDateTime::currentDateTime();

    QStringList strlistReadsAdapters = { "Reads","Adapters","Reads sum","Adapters sum"};
    int splinestotal = ENUM_ADAPTERS_SUM_SPLINES +1;
    m_vctReadsAdapterSplines.resize(splinestotal);
    for(int index = 0; index < splinestotal; ++index)
    {
        auto &pSplineserials = m_vctReadsAdapterSplines[index];
        pSplineserials = new QSplineSeries(m_pReadsAdaptersChart);
        pSplineserials->setName(strlistReadsAdapters[index]);
        m_pReadsAdaptersChart->addSeries(pSplineserials);
    }


    m_pReadAdaptersAxisX= new QDateTimeAxis(m_pReadsAdaptersChart);
    m_pReadAdaptersAxisX->setTitleText(QString::fromLocal8Bit("time"));                 // x轴显示标题
    m_pReadAdaptersAxisX->setGridLineVisible(true);                                     // 隐藏背景网格X轴框线
    m_pReadAdaptersAxisX->setFormat("dd hh:mm:ss");                                     // x轴格式
    m_pReadAdaptersAxisX->setLabelsAngle(90);                                           // x轴显示的文字倾斜角度
    m_pReadAdaptersAxisX->setTickCount(10);                                             // 轴上点的个数
    m_pReadAdaptersAxisX->setRange(m_readsStartPlotDateTIme, m_readsStartPlotDateTIme.addSecs(100));

    m_pReadAdaptersAxisY = new QValueAxis(m_pReadsAdaptersChart);
    m_pReadAdaptersAxisY->setTitleText(QString::fromLocal8Bit("Count"));                // y轴显示标题
    m_pReadAdaptersAxisY->setRange(0, 500);                                             // 范围
    m_pReadAdaptersAxisY->setTickCount(11);                                             // 轴上点的个数
    m_pReadAdaptersAxisY->setGridLineVisible(true);

    m_pReadsAdaptersChart->addAxis(m_pReadAdaptersAxisX, Qt::AlignBottom);
    m_pReadsAdaptersChart->addAxis(m_pReadAdaptersAxisY, Qt::AlignLeft);

    for(const auto &spline: m_vctReadsAdapterSplines)
    {
        spline->attachAxis(m_pReadAdaptersAxisX);
        spline->attachAxis(m_pReadAdaptersAxisY);
    }


    m_pReadsAdaptersChartView = new QChartView(m_pReadsAdaptersChart);
    m_charts.push_back(m_pReadsAdaptersChartView);

    QHBoxLayout *preadslayout = new QHBoxLayout();
    preadslayout->addWidget(m_pReadsAdaptersChartView);
    m_pReadsAdaptersChartWdgt->setLayout(preadslayout);

    m_pReadControlgrpbx = new QGroupBox(QStringLiteral("Reads"),this);
    m_pReadsFileLoadBtn = new QPushButton(QStringLiteral("Choose"),this);
    m_pReadsLoadPathEdit= new QLineEdit(this);
    m_pReadsLoadPathEdit->setReadOnly(true);

    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(m_pReadsLoadPathEdit,0,0,1,1);
    pgridlayout->addWidget(m_pReadsFileLoadBtn,0,1,1,1);
    pgridlayout->setRowStretch(1,10);
    m_pReadControlgrpbx->setLayout(pgridlayout);

    connect(m_pReadsFileLoadBtn,&QPushButton::clicked,this,&ChartViewWidgt::ReadsFileLoadBtnSlot);
}



void ChartViewWidgt::UpdatePoreChart(void)
{
    const auto &vctPoreStateCnt = m_pPoreStateMapWdgt->GetPoreStateCntVct();
    for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
    {
        for(int porestate = 0; porestate < PORE_STATE_NUM; ++porestate)
        {
            QBarSet *tmpBarset =  m_vctPoreBarset[porestate];
            tmpBarset->replace(sensorGrp, vctPoreStateCnt[sensorGrp][porestate]);
        }
    }
}

void ChartViewWidgt::UpdateValidChart(void)
{
    const auto &vctValidStateCnt = m_pPoreStateMapWdgt->GetValidStateCntVct();

    for(int validstate = 0; validstate < VALID_STATE_NUM; ++validstate)
    {
        QList<qreal> barSetValueList;
        for(int sensorGrp = 0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp)
        {
            barSetValueList<<vctValidStateCnt[sensorGrp][validstate];
        }
        m_validBarChartView->ReplaceBarsetAllVue(validstate,barSetValueList);
    }
}


void ChartViewWidgt::UpdateChartBtnSlot(void)
{
    ENUM_CHART_VIEW_PANEL panelIndex = ENUM_CHART_VIEW_PANEL(m_pChartTabWdgt->currentIndex());
    switch (panelIndex) {
    case ENUM_CHART_PORE_PANNEL:
        emit OnRefreshCountSig(PORE_STATE_MAP_MODE);
        UpdatePoreChart();
        break;
    case ENUM_CHART_VALID_PANNEL:
        emit OnRefreshCountSig(VALID_SENSOR_STATE_MAP_MODE);
        UpdateValidChart();
        break;
    case ENUM_CHART_READS_ADAPTERS_PANNEL:
        UpdateReadsChart();
        break;
    default:
        break;
    }
}

void ChartViewWidgt::UpdateChartUIStyle(void)
{
    //![6]
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                m_pThemeComb->itemData(m_pThemeComb->currentIndex()).toInt());
    //![6]
    const auto charts = m_charts;
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->theme() != theme) {
        for (QChartView *chartView : charts) {
            //![7]
            chartView->chart()->setTheme(theme);
            //![7]
        }

        // Set palette colors based on selected theme
        //![8]
        QPalette pal = palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        //![8]
        } else if (theme == QChart::ChartThemeDark) {
            pal.setColor(QPalette::Window, QRgb(0x121218));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBlueCerulean) {
            pal.setColor(QPalette::Window, QRgb(0x40434a));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBrownSand) {
            pal.setColor(QPalette::Window, QRgb(0x9e8965));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeBlueNcs) {
            pal.setColor(QPalette::Window, QRgb(0x018bba));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeHighContrast) {
            pal.setColor(QPalette::Window, QRgb(0xffab03));
            pal.setColor(QPalette::WindowText, QRgb(0x181818));
        } else if (theme == QChart::ChartThemeBlueIcy) {
            pal.setColor(QPalette::Window, QRgb(0xcee7f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        }
        setPalette(pal);
    }

    // Update antialiasing
    //![11]
    bool checked = m_pAliasingChk->isChecked();
    for (QChartView *chart : charts)
        chart->setRenderHint(QPainter::Antialiasing, checked);
    //![11]

    // Update animation options
    //![9]
    QChart::AnimationOptions options(
                m_pAnimationsComb->itemData(m_pAnimationsComb->currentIndex()).toInt());
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->animationOptions() != options) {
        for (QChartView *chartView : charts)
            chartView->chart()->setAnimationOptions(options);
    }
    //![9]

    // Update legend alignment
    //![10]
    Qt::Alignment alignment(
                m_pLegendComb->itemData(m_pLegendComb->currentIndex()).toInt());

    if (!alignment) {
        for (QChartView *chartView : charts)
            chartView->chart()->legend()->hide();
    } else {
        for (QChartView *chartView : charts) {
            chartView->chart()->legend()->setAlignment(alignment);
            chartView->chart()->legend()->show();
        }
    }
    //![10]
}


void ChartViewWidgt::ReadsFileLoadBtnSlot(void)
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Load csv file which record reads and adapters"));
    fileDialog->setNameFilter(tr("csv files(*.csv)"));
    fileDialog->setViewMode(QFileDialog::Detail);

    if (fileDialog->exec())
    {
        QString loadFileNamePath = fileDialog->selectedFiles().at(0);
        LOGI(loadFileNamePath.toStdString().c_str());
        if (loadFileNamePath != "")
        {
            m_pReadsLoadPathEdit->setText(loadFileNamePath);
             FILE* pf = ::fopen(loadFileNamePath.toStdString().c_str(), "r");
             if (pf != nullptr)
             {
                 ::fseek(pf, 0L, SEEK_END);
                 uint64_t len = ::ftell(pf);

                 char* pbuf = new char[len];
                 memset(pbuf, 0, len);

                 ::fseek(pf, 0L, SEEK_SET);
                 int buflen = ::fread(pbuf, 1, len, pf);

                 std::vector<std::vector<std::string>> vetvetdt;
                 std::vector<std::string> vetrow;
                 UsbProtoMsg::SplitString(pbuf, "\n", vetrow);

                 delete[] pbuf;
                 pbuf = nullptr;
                 fclose(pf);
                 pf = nullptr;


                 if (vetrow.size() > 0)
                 {
                     for (auto&& ite : vetrow)
                     {
                         std::vector<std::string> vetdt;
                         UsbProtoMsg::SplitString(ite.c_str(), ",", vetdt);
                         vetvetdt.emplace_back(vetdt);
                     }


                     long long reads = 0;
                     long long adapters = 0;
                     qint64 datetimemescs = 0;
                     QVector<QPointF> vctReadsPointf;
                     QVector<QPointF> vctAdaptersPointf;

                     for (int i = 1;  i < vetvetdt.size(); ++i)
                     {
                         auto& ite = vetvetdt.at(i);
                         if (ite.size() >= 2)
                         {
                             if (!ite[0].empty() && !ite[1].empty() && !ite[2].empty())
                             {
                                 QString datestr = QString::fromStdString(ite[0]);
                                 QDateTime datetime = QDateTime::fromString(datestr,"yyyyMMdd-hh:mm:ss");
                                 if(datetime.isValid())
                                 {
                                     datetimemescs = datetime.toMSecsSinceEpoch();
                                 reads = ::atoll(ite[1].c_str());
                                 adapters = ::atoll(ite[2].c_str());

                                 vctReadsPointf.push_back(QPointF(datetimemescs,reads));
                                 vctAdaptersPointf.push_back(QPointF(datetimemescs,adapters));
                                 }
                                 else
                                 {
                                     QMessageBox::critical(this, tr("Load reads csv failed!!!"), tr(" Load reads Lines  %1 failed!!!").arg(i));
                                     break;
                                 }
                             }
                         }
                     }

                     if(vctAdaptersPointf.size() == vctReadsPointf.size() &&  vctAdaptersPointf.size() >= 2)
                     {
                         AddPointsToReadsAdapterChart(vctReadsPointf,vctAdaptersPointf);
                     }

                     return;
                 }
             }

        }
    }
    if(fileDialog)
    {
        delete fileDialog;
        fileDialog = nullptr;
    }
}



void ChartViewWidgt::AddPointsToReadsAdapterChart(const QVector<QPointF> &vctReadsPoints,const QVector<QPointF> &vctAdaptersPoints)
{
    if(vctReadsPoints.size() != vctAdaptersPoints.size())
        return;

    int pointsize = vctReadsPoints.size();
    long long maxReadsY = 0;
    long long maxReadsYSum = 0;
    long long maxAdaptersY = 0;
    long long maxAdaptersYSum = 0;

    const auto &readsSpline = m_vctReadsAdapterSplines[ENUM_READS_PER_SPLINES];
    const auto &readsSumSpline = m_vctReadsAdapterSplines[ENUM_READS_SUM_SPLINES];
    const auto &adapterSpline = m_vctReadsAdapterSplines[ENUM_ADAPTERS_PER_SPLINES];
    const auto &adapterSumSpline = m_vctReadsAdapterSplines[ENUM_ADAPTERS_SUM_SPLINES];

    if(pointsize >= 2)
    {
        readsSpline->clear();
        readsSumSpline->clear();
        adapterSpline->clear();
        adapterSumSpline->clear();

        for(int i = 0; i < pointsize; ++i)
        {
            QPointF points =  vctReadsPoints[i];
            const auto &yVue = points.y();
            if(yVue > maxReadsY)
            {
                maxReadsY = yVue;
            }
            maxReadsYSum += yVue;
            points.setY(maxReadsYSum);
            readsSumSpline->append(points);

            QPointF adpaterpoints =  vctAdaptersPoints[i];
            const auto &adapterYVue = adpaterpoints.y();
            if(adapterYVue > maxAdaptersY)
            {
                maxAdaptersY = adapterYVue;
            }
            maxAdaptersYSum += adapterYVue;
            adpaterpoints.setY(maxAdaptersYSum);
            adapterSumSpline->append(adpaterpoints);
        }
        readsSpline->replace(vctReadsPoints);
        if(maxReadsY > 0 && maxReadsYSum > 0)
        {
            m_maxReadsPer = maxReadsY;
            m_maxReadsSum = maxReadsYSum;
        }

        adapterSpline->replace(vctAdaptersPoints);
        if(maxAdaptersY > 0 && maxAdaptersYSum > 0)
        {
            m_maxAdaptersPer = maxAdaptersY;
            m_maxAdaptersSum = maxAdaptersYSum;
        }
        UpdateReadsChart();
    }



}

void ChartViewWidgt::UpdateReadsChart(void)
{
    bool bIsReadsDisp = m_bReadsOrAdapterDisp;
        m_vctReadsAdapterSplines[ENUM_READS_PER_SPLINES]->setVisible(bIsReadsDisp);
        m_vctReadsAdapterSplines[ENUM_ADAPTERS_PER_SPLINES]->setVisible(bIsReadsDisp);
        m_vctReadsAdapterSplines[ENUM_READS_SUM_SPLINES]->setVisible(!bIsReadsDisp);
        m_vctReadsAdapterSplines[ENUM_ADAPTERS_SUM_SPLINES]->setVisible(!bIsReadsDisp);
    if(m_bReadsOrAdapterDisp)
    {
        m_pReadAdaptersAxisY->setRange(0, m_maxReadsPer > m_maxAdaptersPer ? m_maxReadsPer : m_maxAdaptersPer);
        ReAdjustReasChart();
    }
    else
    {
        m_pReadAdaptersAxisY->setRange(0, m_maxReadsSum > m_maxAdaptersSum ? m_maxReadsSum : m_maxAdaptersSum);
        ReAdjustReasChart();
    }

    m_bReadsOrAdapterDisp = !m_bReadsOrAdapterDisp;
}

void ChartViewWidgt::ReAdjustReasChart(void)
{
    const auto& splineseries = m_vctReadsAdapterSplines[ENUM_READS_PER_SPLINES];
    int pointsize =splineseries->count();
    if(pointsize >= 2)
    {
        QDateTime readsStartPlotDateTIme = QDateTime::fromMSecsSinceEpoch(splineseries->at(0).x());
        QDateTime readsLastPlotDateTIme = QDateTime::fromMSecsSinceEpoch(splineseries->at(pointsize -1).x());
        m_pReadAdaptersAxisX->setTickCount( pointsize >= 10 ? 10 : pointsize );
        m_pReadAdaptersAxisX->setRange(readsStartPlotDateTIme, readsLastPlotDateTIme);
    }
}


void ChartViewWidgt::ReadsAdaptersRecord(void)
{
    if(m_pReadsAdapterRecord != nullptr)
    {
        long long curReads = 0;
        long long curAdapters = 0;

        emit AcquireCurReadsApatersSig(curReads,curAdapters);
//        qDebug()<<"reads "<<curReads<< "Adapters "<<curAdapters;

        QDateTime currentdataTime = QDateTime::currentDateTime();
        auto hms = currentdataTime.toString("yyyyMMdd-hh:mm:ss");
        hms += "," + QString::number(curReads) + "," + QString::number(curAdapters) + "\n";
        m_pReadsAdapterRecord->RecordInfo(hms.toStdString());
    }
}


void ChartViewWidgt::PoreCountRecord(void)
{
    if(m_pPoreRecord != nullptr)
    {
        const auto &vctPoreStateCnt = m_pPoreStateMapWdgt->GetPoreStateCntVct();

        QDateTime currentdataTime = QDateTime::currentDateTime();
        auto hms = currentdataTime.toString("yyyyMMdd-hh:mm:ss");
        for(int i = 0; i < vctPoreStateCnt.size(); ++i)
        {
             hms += "," + QString::number(vctPoreStateCnt[i][SINGLE_PORE_STATE]);
        }
        for(int i = 0; i < vctPoreStateCnt.size(); ++i)
        {
             hms += "," + QString::number(vctPoreStateCnt[i][MULTI_PORE_STATE]);
        }
        hms +=",\n";
        m_pPoreRecord->RecordInfo(hms.toStdString());
    }
}

void ChartViewWidgt::ValidCountRecord(void)
{
    if(m_pValidRecord != nullptr)
    {
        const auto &vctValidStateCnt = m_pPoreStateMapWdgt->GetValidStateCntVct();

        QDateTime currentdataTime = QDateTime::currentDateTime();
        auto hms = currentdataTime.toString("yyyyMMdd-hh:mm:ss");
        for(int i = 0; i < vctValidStateCnt.size(); ++i)
        {
             hms += "," + QString::number(vctValidStateCnt[i][VALID_CHANNEL_ENUM]);
        }
        if(m_bNeedObserMuxFinalRecord)
        {
            hms +=","+ QString::number(m_observeMuxFinal);
        }
        hms +=",\n";
        m_pValidRecord->RecordInfo(hms.toStdString());
    }
}
void ChartViewWidgt::NotifyRecordOperateSlot(const RECORD_FILE_TYPE_ENUM &type,const RECORD_FILE_OPERT_TYPE_ENUM &operate)
{
    switch (operate) {
    case RECORD_FILE_OPEN:
        OpenRecordFileByType(type);
        break;
    case RECORD_FILE_CLOSE:
        CloseRecordFileByType(type);
        break;
    case RECORD_FILE_RECORD:
        RecordFileByType(type);
        break;
    default:
        break;
    }
}

void ChartViewWidgt::OpenRecordFileByType(const RECORD_FILE_TYPE_ENUM &type)
{
    std::string strFilePath = ConfigServer::GetInstance()->GetRecordPath(type);
    switch (type) {
    case RECORD_READS_ADAPTERS_FILE:
    {
        if(m_pReadsAdapterRecord == nullptr)
        {
            m_pReadsAdapterRecord = new RecordFile(strFilePath);
            std::string strhead("Time,Reads,Adapters,\n");
            m_pReadsAdapterRecord->RecordInfo(strhead);
        }
    }
        break;
    case RECORD_VALID_FILE:
    {
        if(m_pValidRecord == nullptr)
        {
            m_pValidRecord = new RecordFile(strFilePath);
            std::string strhead("Time,SensorA,SensorB,SensorC,SensorD,ObserveMux\n");
            m_pValidRecord->RecordInfo(strhead);
        }
    }
        break;
    case RECORD_MUX_FILE:
        break;
    case RECORD_PORE_FILE:
        if(m_pPoreRecord == nullptr)
        {
            m_pPoreRecord = new RecordFile(strFilePath);
            std::string strhead("Time,ASingel,BSingel,CSingel,DSingel,AMult,BMult,CMult,DMult,\n");
            m_pPoreRecord->RecordInfo(strhead);
        }
        break;
    default:
        return;
    }
}

void ChartViewWidgt::CloseRecordFileByType(const RECORD_FILE_TYPE_ENUM &type)
{
    switch (type) {
    case RECORD_READS_ADAPTERS_FILE:
        SAFE_DELETEP(m_pReadsAdapterRecord);
        break;
    case RECORD_VALID_FILE:
        SAFE_DELETEP(m_pValidRecord);
        break;
    case RECORD_MUX_FILE:
        break;
    case RECORD_PORE_FILE:
        SAFE_DELETEP(m_pPoreRecord);
        break;
    default:
        return;
    }
}

void ChartViewWidgt::RecordFileByType(const RECORD_FILE_TYPE_ENUM &type)
{
    switch (type) {
    case RECORD_READS_ADAPTERS_FILE:
        ReadsAdaptersRecord();
        break;
    case RECORD_VALID_FILE:
        ValidCountRecord();
        break;
    case RECORD_MUX_FILE:
        break;
    case RECORD_PORE_FILE:
        PoreCountRecord();
        break;
    default:
        return;
    }

}

void ChartViewWidgt::SendFinalMuxSlot(const bool &NeedObserRecord,const int &finalMuxCnt)
{
    m_bNeedObserMuxFinalRecord = NeedObserRecord;
    m_observeMuxFinal = finalMuxCnt;
}

void ChartViewWidgt::OpenRecordFile(const bool &enable)
{
    if(enable)
    {
        OpenRecordFileByType(RECORD_READS_ADAPTERS_FILE);
        OpenRecordFileByType(RECORD_VALID_FILE);
        OpenRecordFileByType(RECORD_PORE_FILE);
    }
    else
    {
        CloseRecordFileByType(RECORD_READS_ADAPTERS_FILE);
        CloseRecordFileByType(RECORD_VALID_FILE);
        CloseRecordFileByType(RECORD_PORE_FILE);
    }
}
