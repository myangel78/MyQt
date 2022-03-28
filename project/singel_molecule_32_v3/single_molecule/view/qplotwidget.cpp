#include "qplotwidget.h"
#include "ui_qplotwidget.h"
#include<thread>
#include<vector>
#include"models/DatacCommunicaor.h"
#include"logger/Log.h"

#define RESET_CLEAR_LAYER 1
#define NO_CLEAR_LAYER 2
#define LAYER_NAME "layer1"

#define g_pCommunicaor DataCommunicaor::Instance()

QPlotwidget::QPlotwidget(QWidget *parent)
    : QWidget(parent),
      m_pPlot(NULL),
      ui(new Ui::QPlotwidget)
{
    ui->setupUi(this);

    QHBoxLayout *baseLayout = new QHBoxLayout();
    m_pPlot = new QCustomPlot(ui->plotwidget);
    baseLayout->addWidget(m_pPlot);

    m_pPlot->addLayer(LAYER_NAME, 0, QCustomPlot::limAbove);
    m_pPlot->layer(LAYER_NAME)->setMode(QCPLayer::lmBuffered);
    for(int j = 0; j < CHANNEL_NUM; j++)
    {
        m_pPlot->addGraph();
        // m_pPlot->graph(j)->setAntialiased (false);
        m_pPlot->graph(j)->setPen(QPen(vctPenColor.at(j%32)));
        m_pPlot->graph(j)->setLayer(LAYER_NAME);
    }

//          QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
//          timeTicker->setTimeFormat("%s.%z");
//          m_pPlot->xAxis->setTicker(timeTicker);

    //设置刻度颜色
    QPen pen;
    pen.setColor(Qt::red);//主刻度红色
    pen.setWidth(1);//线宽2
    m_pPlot->xAxis->setTickPen(pen);
    m_pPlot->xAxis->setTickLengthIn(6);//主刻度向内延伸30
    m_pPlot->xAxis->setTickLengthOut(2);//主刻度向外延伸10
    pen.setColor(Qt::blue);//子刻度蓝色
    m_pPlot->xAxis->setSubTickPen(pen);
    m_pPlot->xAxis->setSubTickLengthIn(3);//子刻度向内延伸15
    m_pPlot->xAxis->setSubTickLengthOut(1);//子刻度向外延伸5


    m_pPlot->axisRect()->setupFullAxesBox();
    m_pPlot->xAxis->setLabel("second/s");
    m_pPlot->yAxis->setLabel("Current(pA)");
//        m_pPlot->yAxis->setRange(-m_nYRange, m_nYRange);
    m_pPlot->yAxis->setRange(-500, 500);
    m_pPlot->yAxis->ticker()->setTickCount(8);  //设置刻度数量
    m_pPlot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability); ////可读性优于设置
    m_pPlot->xAxis->setRange(0, g_pCommunicaor->g_xTime);
    connect(m_pPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_pPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_pPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_pPlot->yAxis2, SLOT(setRange(QCPRange)));
//    m_pPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    //记录
    saveRangeParam(500,-500,g_pCommunicaor->g_xTime);

    ui->plotwidget->setLayout(baseLayout);

    connect(ui->applyButton,&QPushButton::clicked,this,&QPlotwidget::plotSetSlot);
    connect(this, SIGNAL(replotSignal(int)), this, SLOT(replotSlot(int)));

    ui->plotwidget->setAttribute(Qt::WA_TransparentForMouseEvents,true);

    enableOpengl();
}


QPlotwidget::~QPlotwidget()
{
    stopThread();
    if(m_pPlot != NULL)
    {
        m_pPlot->setOpenGl(false);
        delete m_pPlot;
        m_pPlot=nullptr;
    }
    delete ui;
}

void QPlotwidget::enableOpengl()
{
    m_pPlot->setOpenGl(true);
   LOGI("Opengl status:{}",m_pPlot->openGl());
   qDebug()<<"Opengl status:"<<m_pPlot->openGl();
}

void QPlotwidget::threadFunc()
{
    int nRet = THREAD_NORMAL_EXIT;
    m_nXRange = XRANGE;
    uint16_t nPlotSemCnt = 0;
    bool bReset = false;
    LARGE_INTEGER frequency, startCount, stopCount;
    LONGLONG elapsed;
    QueryPerformanceFrequency(&frequency);

    int nSemTryTime =SEM_TRY_TIME;

    while (m_bRunThread)
    {
        try
        {

            if(g_pCommunicaor->m_sPlotSem.usedBuf.tryAcquire(1,PLOT_SEM_TIMEOUT))
            {

                if(g_pCommunicaor->m_VctPlotBuf[0 + CHANNEL_NUM*nPlotSemCnt].size() > 0)
                {
                    std::lock_guard<std::mutex> lock(m_lock);
                    if(g_pCommunicaor->m_VctPlotBuf[0 + CHANNEL_NUM*nPlotSemCnt][0].key < 0.0000001)
                    {
                        for(int i = 0; i < CHANNEL_NUM; i++)
                        {
                            m_pPlot->graph(i)->data()->clear();
                        }
                        bReset = true;
                    }

                    QueryPerformanceCounter(&startCount);

                    int plotBufIndex = CHANNEL_NUM * nPlotSemCnt;
                    for(int i = 0; i<CHANNEL_NUM;i++)
                    {
                        m_pPlot->graph(i)->addData(g_pCommunicaor->m_VctPlotBuf[plotBufIndex+i],true);
                    }
                }

                QueryPerformanceCounter(&stopCount);
                elapsed = (stopCount.QuadPart - startCount.QuadPart) * 1000000 / frequency.QuadPart;
    //            qDebug()<<"Add dot num = "<<g_pCommunicaor->m_VctPlotBuf[ CHANNEL_NUM * nPlotSemCnt].size()<<"time "<<elapsed<<" nPlotSemCnt = "<<nPlotSemCnt<<"plot used sem"<<g_pCommunicaor->m_sPlotSem.usedBuf.available();

                nPlotSemCnt = (nPlotSemCnt == PLOT_BUF_BLOCK_END_INDEX) ? 0 : (nPlotSemCnt+1);
                g_pCommunicaor->m_sPlotSem.freeBuf.release();
                if(bReset)
                {
                    bReset = false;
                    emit replotSignal(RESET_CLEAR_LAYER);
    //                 m_pPlot->layer(LAYER_NAME)->replot(true);

                }
                else
                {
                    emit replotSignal(NO_CLEAR_LAYER);
    //                m_pPlot->layer(LAYER_NAME)->replot(false);
                }

            }else
            {
                nRet = THREAD_PLOT_USED_SEM_ACQ_TIMEOUT;
//                stopThread();
                if(nSemTryTime-- == 0)
                {
                    LOGI("m_sPlotSem.usedBuf.tryAcquire failed");
                    stopThread();
                    break;
                }
            }
        } catch (std::exception &e)
        {
             LOGI("QPlotwidget thread {}",e.what());
             stopThread();
        }
    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit QPlotwidget thread.");
}


void QPlotwidget::replotSlot(int layorNum)
{
    {
        std::lock_guard<std::mutex> lock(m_lock);
        if(isVisible())
        {
            if(layorNum == RESET_CLEAR_LAYER)
            {
                m_pPlot->layer(LAYER_NAME)->replot(true);
            }else if(layorNum == NO_CLEAR_LAYER)
            {
                m_pPlot->layer(LAYER_NAME)->replot(false);
            }
        }
    }
}



 void QPlotwidget::plotSetSlot(void)
 {
    double nYmax =  ui->YMaxdoubleSpinBox->value();
    double nYmin =  ui->YMindoubleSpinBox->value();
    double ntime  =  ui->XtimedoubleSpinBox->value();

     g_pCommunicaor->g_xTime = ntime;
     g_pCommunicaor->g_xTimeUs = g_pCommunicaor->g_xTime - 0.000001;
     m_pPlot->yAxis->setRange(nYmin, nYmax);
     m_pPlot->xAxis->setRange(0, ntime);


     if(!isSameRangeSet(nYmax,nYmin,ntime))
     {
         saveRangeParam(nYmax,nYmin,ntime);
         m_pPlot->replot();
     }
 }


 void QPlotwidget::LoadConfig(QSettings *nConfig)
 {
     if(!nConfig)
     {
         qDebug()<<"PlotSetting nConfig is nullptr"<<nConfig;
         return;
     }
     m_nConfig = nConfig;

     nConfig->beginGroup("PlotSetting");
     ui->YMaxdoubleSpinBox->setValue(nConfig->value("yMax").toDouble());
     ui->YMindoubleSpinBox->setValue(nConfig->value("yMin").toDouble());
     ui->XtimedoubleSpinBox->setValue(nConfig->value("xTime").toDouble());
     nConfig->endGroup();

     plotSetSlot(); //更新plot设置参数；
 }

 void QPlotwidget::SaveConfig(QSettings * nConfig)
 {
     if(!nConfig)
     {
         qDebug()<<"PlotSetting nConfig is nullptr"<<nConfig;
         return;
     }
     nConfig->beginGroup("PlotSetting");

     nConfig->setValue("yMax",  ui->YMaxdoubleSpinBox->value());
     nConfig->setValue("yMin",   ui->YMindoubleSpinBox->value());
     nConfig->setValue("xTime",  ui->XtimedoubleSpinBox->value());
     nConfig->endGroup();
 }



void QPlotwidget::GraphVisibleSlot(int channel,bool isVisible)
{
     m_pPlot->graph(channel)->setVisible(isVisible);
}
void QPlotwidget::GraphColorChangeSlot(int channel,QColor color)
{
    m_pPlot->graph(channel)->setPen(QPen(color));
}

void QPlotwidget::GraphAllVisibleSlot(bool isVisible)
{
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        m_pPlot->graph(ch)->setVisible(isVisible);
    }
}
