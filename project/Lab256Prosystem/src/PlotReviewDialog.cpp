#include "PlotReviewDialog.h"

#include <qlayout.h>


PlotReviewDialog::PlotReviewDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Plot Review");
    InitCtrl();
    resize(600, 400);
}

//PlotReviewDialog::~PlotReviewDialog()
//{
//    delete m_pReplot;
//    delete m_plabDataName;
//    delete m_pbtnLoadData;
//    delete m_pbtnResetData;
//}

bool PlotReviewDialog::InitCtrl()
{
    m_pReplot = new QCustomPlot(this);
    m_pReplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    m_plabDataName = new QLabel("Data Name", this);

    m_pbtnLoadData = new QPushButton("Load Data", this);
    connect(m_pbtnLoadData, &QPushButton::clicked, this, &PlotReviewDialog::OnClickLoadData);

    m_pbtnResetData = new QPushButton("Reset Data", this);
    connect(m_pbtnResetData, &QPushButton::clicked, this, &PlotReviewDialog::OnClickResetData);


    QHBoxLayout* horlayout = new QHBoxLayout();
    horlayout->addWidget(m_plabDataName);
    horlayout->addStretch(1);
    horlayout->addWidget(m_pbtnLoadData);
    horlayout->addWidget(m_pbtnResetData);

    QVBoxLayout* verlayout = new QVBoxLayout();
    verlayout->addWidget(m_pReplot);
    verlayout->addLayout(horlayout);

    setLayout(verlayout);


    m_graph = m_pReplot->addGraph();
    m_graph->SetLastDataRange(false);

    return true;
}

void PlotReviewDialog::OnClickLoadData(void)
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open  plot data File"),
        ".",
        "binary dat files(*.dat)",
        0);

    if (!fileName.isNull())
    {
        QFileInfo fileInfo(fileName);
        QString fileBaseName = fileInfo.baseName();
        m_plabDataName->setText(fileName);

        float recvBuf[1024] = { 0 };
        FILE* pf = fopen(fileName.toStdString().c_str(), "rb");
        float temp = 0;

        m_dXnum = 0.0;
        m_graph->data()->clear();
        m_pReplot->replot(QCustomPlot::rpQueuedReplot);

        while (true)
        {
            size_t  readCount = fread(&recvBuf[0], sizeof(float), sizeof(recvBuf) / sizeof(float), pf);
            if (readCount > 0)
            {
                for (int i = 0; i < readCount; i++)
                {
                    temp = recvBuf[i];
                    if (temp > m_dYmax)
                    {
                        m_dYmax = temp;
                    }
                    if (temp < m_dYmin)
                    {
                        m_dYmin = temp;
                    }
                    m_graph->addData(m_dXnum++, temp);
                }
            }
            else
            {
                //qDebug() << "file has read out";
                break;
            }
        }
        fclose(pf);

        m_pReplot->xAxis->rescale();
        m_graph->rescaleValueAxis(false, true);
        m_pReplot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void PlotReviewDialog::OnClickResetData(void)
{
    std::thread thrd([this]()
        {
            m_pReplot->xAxis->rescale();
            m_graph->rescaleValueAxis(false, true);
            m_pReplot->replot(QCustomPlot::rpQueuedReplot);
        });
    thrd.detach();
}
