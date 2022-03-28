#include "CapacitanceView.h"

#include <qlayout.h>
#include <qheaderview.h>
#include <qaction.h>
#include <qpainter.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <Log.h>


CapacitanceView::CapacitanceView(QWidget *parent) : QWidget(parent)
{
    m_vetCapValue.resize(CHANNEL_NUM);
    m_vetMeanCap.resize(CHANNEL_NUM);
    m_vetCapListItem.resize(CHANNEL_NUM);

    //InitData();
    InitCtrl();

    connect(this, SIGNAL(updateCapListSignal()), this, SLOT(updateCapListSlot()));
    connect(&m_tmrRecordCap, &QTimer::timeout, this, &CapacitanceView::OnTimerRecordCapdata);
}

CapacitanceView::~CapacitanceView()
{
    if (m_pbtnCapStart->isChecked())
    {
        m_pbtnCapStart->click();
    }
    if (ConfigServer::GetInstance()->GetCollectCap())
    {
        ConfigServer::GetInstance()->SetCollectCap(false);
        StopThread();
    }
}

#if 0
bool CapacitanceView::InitData(void)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_mapChannelCoordinate[COORDINATE_CHANNEL[i][0] - 1] = { COORDINATE_CHANNEL[i][1], COORDINATE_CHANNEL[i][2] };
    }

    return false;
}

bool CapacitanceView::FindChannelCoordinate(int ch, int& ix, int& iy)
{
    for (auto&& ite : m_mapChannelCoordinate)
    {
        if (ite.first == ch)
        {
            ix = ite.second.first;
            iy = ite.second.second;
            return true;
        }
    }
    return false;
}

bool CapacitanceView::FindCoordinateChannel(int ix, int iy, int& ch)
{
    for (auto&& ite : m_mapChannelCoordinate)
    {
        if (ite.second.first == ix && ite.second.second == iy)
        {
            ch = ite.first;
            return true;
        }
    }
    return false;
}
#endif

bool CapacitanceView::InitCtrl(void)
{
    //HeatMap
    {
        m_pgrpHeatMap = new QGroupBox("Heat Map", this);
        m_ptabCapHeat = new QTableWidget(m_ciRow, m_ciCol, m_pgrpHeatMap);
        m_ptabCapHeat->setMinimumHeight(350);
        auto horheader = m_ptabCapHeat->horizontalHeader();
        horheader->setSectionResizeMode(QHeaderView::Stretch);
        horheader->setHidden(true);
        auto verheader = m_ptabCapHeat->verticalHeader();
        verheader->setSectionResizeMode(QHeaderView::Stretch);
        verheader->setHidden(true);
        //m_ptabCapHeat->setAutoScroll(true);
        for (int i = 0; i < m_ciRow; ++i)
        {
            for (int j = 0; j < m_ciCol; ++j)
            {
                int chan = 0;
                if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
                {
                    QTableWidgetItem* itemChan = new QTableWidgetItem(QString("%1").arg(chan + 1));
                    itemChan->setBackground(QBrush(m_clrLevel[0]));
                    itemChan->setFlags(itemChan->flags() & (~Qt::ItemIsEditable));
                    itemChan->setTextAlignment(Qt::AlignCenter);
                    m_ptabCapHeat->setItem(i, j, itemChan);
                }
            }
        }


        for (int i = 0; i < m_ciLevelNum; ++i)
        {
            m_pgrpLevel[i] = new QGroupBox(m_pgrpHeatMap);
            auto pgrp = m_pgrpLevel[i];

            m_plabLevel3[i] = new QLabel("-", pgrp);
            m_plabLevel4[i] = new QLabel("pF  -", pgrp);

            m_pdspLevel[i] = new QDoubleSpinBox(pgrp);
            auto pdsp = m_pdspLevel[i];
            pdsp->setDecimals(2);
            pdsp->setSuffix(" pF");
            pdsp->setMinimum(-9999.99);
            pdsp->setMaximum(9999.99);
            pdsp->setValue(m_fLevel[i]);

            m_pbtnLevel[i] = new QToolButton(pgrp);
            auto pbtn = m_pbtnLevel[i];
            QAction* act = new QAction(pgrp);
            act->setToolTip(tr("Change color"));
            pbtn->setDefaultAction(act);
            SetColor(pbtn, m_clrLevel[i]);
            pbtn->setObjectName(QString::number(i, 10));
            connect(pbtn, &QToolButton::clicked, this, &CapacitanceView::OnClickChangeColor);

            //m_plabLevel[i] = new QLabel("Count:", pgrp);
            m_plabLevel2[i] = new QLabel("--", pgrp);
            m_plabLevel2[i]->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>该分类统计的计数Count</body></html>"));

            m_pchkLevel[i] = new QCheckBox("Merge", pgrp);
            m_pchkLevel[i]->setChecked(true);
            m_pchkLevel[i]->setObjectName(QString::number(i, 10));
            m_pchkLevel[i]->setVisible(false);
            connect(m_pchkLevel[i], &QCheckBox::stateChanged, this, &CapacitanceView::OnCheckMergeStatus);


            QGridLayout* gdlayout5 = new QGridLayout();
            gdlayout5->addWidget(m_plabLevel3[i], 0, 0, Qt::AlignVCenter | Qt::AlignRight);
            gdlayout5->addWidget(m_plabLevel4[i], 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
            gdlayout5->addWidget(pdsp, 0, 2);
            gdlayout5->addWidget(pbtn, 1, 0, Qt::AlignCenter);
            //gdlayout5->addWidget(m_plabLevel[i], 1, 1);
            gdlayout5->addWidget(m_plabLevel2[i], 1, 1);
            gdlayout5->addWidget(m_pchkLevel[i], 1, 2);

            QHBoxLayout* horlayout3 = new QHBoxLayout();
            horlayout3->addLayout(gdlayout5);
            horlayout3->addStretch(1);

            pgrp->setLayout(horlayout3);

            m_vetLevelCount.emplace_back(0);
        }

        m_plabLevel4[0]->setText(" < ");
        m_plabLevel3[1]->setText(QString::number(m_fLevel[0]));
        m_plabLevel3[2]->setText(QString::number(m_fLevel[1]));
        m_plabLevel3[3]->setText(QString::number(m_fLevel[2]));
        m_plabLevel4[4]->setText(" > ");
        m_plabLevel3[0]->setVisible(false);
        m_plabLevel3[4]->setVisible(false);
        m_pdspLevel[4]->setEnabled(false);

        connect(m_pdspLevel[0], SIGNAL(valueChanged(double)), m_plabLevel3[1], SLOT(setNum(double)));
        connect(m_pdspLevel[1], SIGNAL(valueChanged(double)), m_plabLevel3[2], SLOT(setNum(double)));
        connect(m_pdspLevel[2], SIGNAL(valueChanged(double)), m_plabLevel3[3], SLOT(setNum(double)));
        connect(m_pdspLevel[3], SIGNAL(valueChanged(double)), m_pdspLevel[4], SLOT(setValue(double)));


        m_pgrpHeatMapApply = new QGroupBox(m_pgrpHeatMap);
        m_pchkShowChannel = new QCheckBox("Show Channel", m_pgrpHeatMapApply);
        m_pbtnHeatMapApply = new QPushButton("Apply", m_pgrpHeatMapApply);

        QVBoxLayout* verlayout1 = new QVBoxLayout();
        verlayout1->addWidget(m_pchkShowChannel);
        verlayout1->addWidget(m_pbtnHeatMapApply);
        m_pgrpHeatMapApply->setLayout(verlayout1);
        m_pgrpHeatMapApply->setVisible(false);


        int row = 0;
        int col = 0;
        QGridLayout* gdlayout8 = new QGridLayout();
        gdlayout8->addWidget(m_pgrpLevel[0], row, col++);
        gdlayout8->addWidget(m_pgrpLevel[1], row, col++);
        gdlayout8->addWidget(m_pgrpLevel[2], row, col++);
        row = 1;
        col = 0;
        gdlayout8->addWidget(m_pgrpLevel[3], row, col++);
        gdlayout8->addWidget(m_pgrpLevel[4], row, col++);
        gdlayout8->addWidget(m_pgrpHeatMapApply, row, col++);

        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->addWidget(m_ptabCapHeat);
        verlayout9->addStretch(1);
        verlayout9->addLayout(gdlayout8, 1);
        verlayout9->addStretch(5);

        m_pgrpHeatMap->setLayout(verlayout9);
    }

    //Capacitance
    {
        m_pgrpCapacitance = new QGroupBox("Capacitance", this);
        m_pgrpCapacitance->setMaximumWidth(180);

        m_pgrpCapFilter = new QGroupBox("Cap Filter", m_pgrpCapacitance);

        m_pdspFilterMin = new QDoubleSpinBox(m_pgrpCapFilter);
        m_pdspFilterMin->setSuffix(" pF");
        m_pdspFilterMin->setDecimals(2);
        m_pdspFilterMin->setMinimum(0.0);
        m_pdspFilterMin->setMaximum(1000.0);
        m_pdspFilterMin->setValue(0.0);

        m_pdspFilterMax = new QDoubleSpinBox(m_pgrpCapFilter);
        m_pdspFilterMax->setSuffix(" pF");
        m_pdspFilterMax->setDecimals(2);
        m_pdspFilterMax->setMinimum(0.0);
        m_pdspFilterMax->setMaximum(1000.0);
        m_pdspFilterMax->setValue(200.0);

        m_pbtnFilterApply = new QPushButton("Filter", m_pgrpCapFilter);
        connect(m_pbtnFilterApply, &QPushButton::clicked, this, &CapacitanceView::OnClickCapFilter);

        m_plabFilterCount = new QLabel("Count:0", m_pgrpCapFilter);

        QHBoxLayout* horalyout1 = new QHBoxLayout();
        horalyout1->addWidget(m_pdspFilterMin);
        horalyout1->addWidget(m_pdspFilterMax);

        QHBoxLayout* horalyout2 = new QHBoxLayout();
        horalyout2->addWidget(m_pbtnFilterApply);
        horalyout2->addWidget(m_plabFilterCount);

        QVBoxLayout* verlayout1 = new QVBoxLayout();
        verlayout1->setContentsMargins(3, 6, 2, 2);
        verlayout1->addLayout(horalyout1);
        verlayout1->addLayout(horalyout2);

        m_pgrpCapFilter->setLayout(verlayout1);


        m_pgrpCapList = new QGroupBox("Cap List (pF)", m_pgrpCapacitance);
        m_pcmbCapChannel = new QComboBox(m_pgrpCapList);
        m_pcmbCapChannel->addItem(tr("CH 1-32"));
        m_pcmbCapChannel->addItem(tr("CH 33-64"));
        m_pcmbCapChannel->addItem(QStringLiteral("CH 65-96"));
        m_pcmbCapChannel->addItem(QStringLiteral("CH 97-128"));
        m_pcmbCapChannel->addItem(QStringLiteral("CH 129-160"));
        m_pcmbCapChannel->addItem(QStringLiteral("CH 161-192"));
        m_pcmbCapChannel->addItem(QStringLiteral("CH 193-224"));
        m_pcmbCapChannel->addItem(QStringLiteral("CH 225-256"));
        m_pcmbCapChannel->addItem(tr("AllChannel"));
        m_pcmbCapChannel->addItem(tr("First 128"));
        m_pcmbCapChannel->addItem(tr("Last 128"));
        m_pcmbCapChannel->setCurrentIndex(8);

        connect(m_pcmbCapChannel, SIGNAL(activated(int)), this, SLOT(OnCapPageViewChange(int)));

        m_pbtnCapStart = new QPushButton("Start", m_pgrpCapList);
        m_pbtnCapStart->setCheckable(true);
        connect(m_pbtnCapStart, &QPushButton::clicked, this, &CapacitanceView::OnClickCapStart);


        m_plistCapChannel = new QListWidget(m_pgrpCapList);
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            auto widitem = new QListWidgetItem(m_plistCapChannel);
            m_vetCapListItem[i] = std::make_shared<CapListItem>(i, m_plistCapChannel);
            m_plistCapChannel->addItem(widitem);
            m_plistCapChannel->setItemWidget(widitem, m_vetCapListItem[i].get());

            m_clrCapColor[i] = m_clrLevel[0];
            m_bMergeStatus[i] = true;
        }


        m_pbtnCapSave = new QPushButton("Cap Result Save as...", m_pgrpCapList);
        connect(m_pbtnCapSave, &QPushButton::clicked, this, &CapacitanceView::OnClickCapSaveas);


        QHBoxLayout* horlayout6 = new QHBoxLayout();
        horlayout6->addWidget(m_pcmbCapChannel);
        horlayout6->addWidget(m_pbtnCapStart);

        QVBoxLayout* verlayout6 = new QVBoxLayout();
        verlayout6->setContentsMargins(3, 6, 2, 2);
        verlayout6->addLayout(horlayout6);
        verlayout6->addWidget(m_plistCapChannel);
        verlayout6->addWidget(m_pbtnCapSave);

        m_pgrpCapList->setLayout(verlayout6);


        QVBoxLayout* verlayout9 = new QVBoxLayout();
        verlayout9->setContentsMargins(1, 6, 1, 0);
        verlayout9->addWidget(m_pgrpCapFilter);
        verlayout9->addWidget(m_pgrpCapList);

        m_pgrpCapacitance->setLayout(verlayout9);
    }

    QHBoxLayout* horlayout = new QHBoxLayout();
    horlayout->addWidget(m_pgrpHeatMap);
    horlayout->addWidget(m_pgrpCapacitance);

    setLayout(horlayout);

    return false;
}

bool CapacitanceView::SetColor(QToolButton* pbtn, QColor clr)
{
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(QRect(0, 0, 15, 15));
    painter.fillRect(QRect(1, 1, 14, 14), clr);
    pbtn->defaultAction()->setIcon(QIcon(pixmap));

    return false;
}

void CapacitanceView::GetActiveChannelIndex(int curIndex)
{
    if (curIndex >= 0 && curIndex <= 7)
    {
        m_iStartIndex = 32 * curIndex;
        m_iEndIndex = 32 + m_iStartIndex;
    }
    else if (curIndex == 8)
    {
        m_iStartIndex = 0;
        m_iEndIndex = CHANNEL_NUM;
    }
    else if (curIndex == 9)
    {
        m_iStartIndex = 0;
        m_iEndIndex = 128;
    }
    else if (curIndex == 10)
    {
        m_iStartIndex = 128;
        m_iEndIndex = 256;
    }
    else
    {
        m_iStartIndex = 0;
        m_iEndIndex = CHANNEL_NUM;
    }
}

bool CapacitanceView::LoadConfig(QSettings* pset)
{
    if (pset)
    {
        pset->beginGroup("capHeatMap");
        m_pdspFilterMax->setValue(pset->value("capFilteMax").toDouble());
        m_pdspFilterMin->setValue(pset->value("capFilteMin").toDouble());

        m_pdspLevel[0]->setValue(pset->value("capLevel0").toDouble());
        m_pdspLevel[1]->setValue(pset->value("capLevel1").toDouble());
        m_pdspLevel[2]->setValue(pset->value("capLevel2").toDouble());
        m_pdspLevel[3]->setValue(pset->value("capLevel3").toDouble());
        pset->endGroup();
    }
    return false;
}

bool CapacitanceView::SaveConfig(QSettings* pset)
{
    if (pset)
    {
        pset->beginGroup("capHeatMap");
        pset->setValue("capFilteMax", m_pdspFilterMax->value());
        pset->setValue("capFilteMin", m_pdspFilterMin->value());

        pset->setValue("capLevel0", m_pdspLevel[0]->value());
        pset->setValue("capLevel1", m_pdspLevel[1]->value());
        pset->setValue("capLevel2", m_pdspLevel[2]->value());
        pset->setValue("capLevel3", m_pdspLevel[3]->value());
        pset->endGroup();
    }
    return false;
}

bool CapacitanceView::ReloadCtrl(void)
{
    for (int i = 0; i < m_ciRow; ++i)
    {
        for (int j = 0; j < m_ciCol; ++j)
        {
            int chan = 0;
            if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
            {
                m_ptabCapHeat->item(i, j)->setText(QString("%1").arg(chan + 1));
            }
        }
    }
    return false;
}

bool CapacitanceView::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&CapacitanceView::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void CapacitanceView::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    double triangeFreqence = ConfigServer::GetInstance()->GetTriangFrequence();
    float dutyTime = 20.0 / triangeFreqence;
    float distTime = 0.0;
    m_startCnt = std::chrono::steady_clock::now();
    while (m_bRunning)
    {
        //
        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataHandle.size_approx() > 0; });
        }
        double triangeAmplitude = ConfigServer::GetInstance()->GetTriangAmplitude();
        if (ISDOUBLEZERO(triangeAmplitude))
        {
            triangeAmplitude = 1.0;
        }

        //int quepacksz = m_queDataHandle.size_approx();
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataHandle.try_dequeue(dtpk);
        if (bret)
        {
            m_stopCnt = std::chrono::steady_clock::now();
            distTime = std::chrono::duration<double>(m_stopCnt - m_startCnt).count();
            if (distTime <= dutyTime)
            {
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    for (int j = 0, sz = dtpk->dataHandle[i].size(); j < sz; ++j)
                    {
                        float val = dtpk->dataHandle[i][j];
                        if (val > 0)
                        {
                            m_vetMeanCap[i].emplace_back(val);
                        }
                    }
                }
            }
            else
            {
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    int avgsz = m_vetMeanCap[i].size();
                    if (avgsz > 0)
                    {
                        double sum = std::accumulate(m_vetMeanCap[i].begin(), m_vetMeanCap[i].end(), 0.0);
                        double mean = sum / avgsz;

                        float cap = mean / 4.0 / triangeFreqence / triangeAmplitude;
                        m_vetCapValue[i] = cap;

                        m_vetMeanCap[i].clear();
                    }
                    else
                    {
                        m_vetCapValue[i] = 0.0;
                    }
                }
                emit updateCapListSignal();

                m_startCnt = std::chrono::steady_clock::now();
            }
            //m_vetMeanCap
        }
    }
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool CapacitanceView::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return false;
}

bool CapacitanceView::StopThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetMeanCap[i].clear();
    }
    if (m_ThreadPtr != nullptr)
    {
        if (m_ThreadPtr->joinable())
        {
            m_ThreadPtr->join();
        }
        m_ThreadPtr.reset();
    }
    return false;
}

void CapacitanceView::OnClickChangeColor(void)
{
    QObject* sendobject = QObject::sender();
    QToolButton* pbtn = qobject_cast<QToolButton*>(sendobject);
    int index = pbtn->objectName().toInt();
    QColor color = QColorDialog::getColor(m_clrLevel[index], this);
    if (color.isValid())
    {
        SetColor(pbtn, color);
        m_clrLevel[index] = color;

        //update();
        for (int i = 0; i < m_ciRow; ++i)
        {
            for (int j = 0; j < m_ciCol; ++j)
            {
                int chan = 0;
                if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
                {
                    if (index == 0 && m_vetCapValue[chan] < m_pdspLevel[0]->value()
                        || index == 1 && m_vetCapValue[chan] >= m_pdspLevel[0]->value() && m_vetCapValue[chan] < m_pdspLevel[1]->value()
                        || index == 2 && m_vetCapValue[chan] >= m_pdspLevel[1]->value() && m_vetCapValue[chan] < m_pdspLevel[2]->value()
                        || index == 3 && m_vetCapValue[chan] >= m_pdspLevel[2]->value() && m_vetCapValue[chan] <= m_pdspLevel[3]->value()
                        || index == 4 && m_vetCapValue[chan] > m_pdspLevel[3]->value()
                        )
                    {
                        m_ptabCapHeat->item(i, j)->setBackground(QBrush(color));
                        m_clrCapColor[chan] = color;
                    }
                }
            }
        }
    }
}

void CapacitanceView::OnCheckMergeStatus(bool chk)
{
    QObject* sendobject = QObject::sender();
    QCheckBox* pbtn = qobject_cast<QCheckBox*>(sendobject);
    int index = pbtn->objectName().toInt();
    {
        for (int i = 0; i < m_ciRow; ++i)
        {
            for (int j = 0; j < m_ciCol; ++j)
            {
                int chan = 0;
                if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
                {
                    if (index == 0 && m_vetCapValue[chan] < m_pdspLevel[0]->value()
                        || index == 1 && m_vetCapValue[chan] >= m_pdspLevel[0]->value() && m_vetCapValue[chan] < m_pdspLevel[1]->value()
                        || index == 2 && m_vetCapValue[chan] >= m_pdspLevel[1]->value() && m_vetCapValue[chan] < m_pdspLevel[2]->value()
                        || index == 3 && m_vetCapValue[chan] >= m_pdspLevel[2]->value() && m_vetCapValue[chan] <= m_pdspLevel[3]->value()
                        || index == 4 && m_vetCapValue[chan] > m_pdspLevel[3]->value()
                        )
                    {
                        m_bMergeStatus[chan] = chk;
                    }
                }
            }
        }
    }
}

void CapacitanceView::OnClickCapFilter(void)
{
    double minval = m_pdspFilterMin->value();
    double maxval = m_pdspFilterMax->value();
    int cnt = 0;
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        const auto& val = m_vetCapValue[i];
        if (val >= minval && val <= maxval)
        {
            if (i >= m_iStartIndex && i < m_iEndIndex)
            {
                m_plistCapChannel->item(i)->setHidden(false);
            }
            else
            {
                m_plistCapChannel->item(i)->setHidden(true);
            }
            ++cnt;
        }
        else
        {
            m_plistCapChannel->item(i)->setHidden(true);
        }
    }
    m_plabFilterCount->setText(QString::number(cnt));
}

void CapacitanceView::OnCapPageViewChange(int index)
{
    GetActiveChannelIndex(index);

    for (int i = 0; i < CHANNEL_NUM; i++)
    {
        bool show = (i >= m_iStartIndex && i < m_iEndIndex);
        m_plistCapChannel->item(i)->setHidden(!show);
    }
}

void CapacitanceView::OnClickCapStart(bool chk)
{
    if (chk)
    {
        ConfigServer::GetInstance()->SetCollectCap(true);
        m_pbtnCapSave->setEnabled(false);
        emit setTriangeVoltageSignal(1);
        StartThread();

#ifdef Q_OS_WIN32
        std::string strstatus = ConfigServer::GetCurrentPath() + "/etc/Capacitance/";
#else
        std::string strstatus = "/data/Capacitance/";
#endif
        std::filesystem::path pathstatus(strstatus);
        if (!std::filesystem::exists(pathstatus))
        {
            std::filesystem::create_directories(pathstatus);
        }
        std::string datetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toStdString();
        m_pFile = new QFile((strstatus + datetime + ".csv").c_str());
        if (!m_pFile->open(QIODevice::WriteOnly | QIODevice::Text))
        {
            LOGE("Save Capacitance file FAILED!!!");
        }
        else
        {
            m_pTextStream = new QTextStream(m_pFile);
            QString strhead("Time,");
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                strhead += QString("CH%1,").arg(i + 1);
            }
            strhead += "\n";
            *m_pTextStream << strhead;

            //OnTimerRecordCapdata();
            m_tmrRecordCap.start(m_iIntervals * 1000);
        }

        m_pbtnCapStart->setText("Stop");
    }
    else
    {
        EndThread();
        OnTimerRecordCapdata();
        if (m_tmrRecordCap.isActive())
        {
            m_tmrRecordCap.stop();
        }
        if (m_pTextStream != nullptr)
        {
            delete m_pTextStream;
            m_pTextStream = nullptr;
        }
        if (m_pFile != nullptr)
        {
            m_pFile->close();
            delete m_pFile;
            m_pFile = nullptr;
        }
        m_pbtnCapStart->setText("Start");
        m_pbtnCapSave->setEnabled(true);
        ConfigServer::GetInstance()->SetCollectCap(false);
        StopThread();
        emit setTriangeVoltageSignal(0);
    }
}

void CapacitanceView::OnClickCapSaveas(void)
{
    QFileDialog fileDialog;
    QString fileName = fileDialog.getSaveFileName(this, tr("Open File"), "CapacitanceValue", tr("Text File(*.txt);;csv File(*.csv)"));
    if (fileName == "")
    {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("error"), tr("open file failed"));
        return;
    }
    else
    {
        QTextStream textStream(&file);
        QString str = "Ch,Capacitance Value,Unit\n";
        textStream << str;
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            textStream << "CH" << i + 1 << "," << m_vetCapValue[i] << ",pf\n";
        }
        QMessageBox::information(this, QString::fromLocal8Bit("Tips"), QString::fromLocal8Bit("Saved File Success!"));
        file.close();
    }
}

void CapacitanceView::updateCapListSlot(void)
{
    for (int k = 0; k < 5; ++k)
    {
        m_vetLevelCount[k] = 0;
    }
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        auto& val = m_vetCapValue[i];
        m_vetCapListItem[i]->SetCapValue(val);
        int level = 0;
        if (val < m_pdspLevel[0]->value())
        {
            level = 0;
        }
        else if (val < m_pdspLevel[1]->value())
        {
            level = 1;
        }
        else if (val < m_pdspLevel[2]->value())
        {
            level = 2;
        }
        else if (val <= m_pdspLevel[3]->value())
        {
            level = 3;
        }
        else
        {
            level = 4;
        }
        ++m_vetLevelCount[level];

        int x = 0;
        int y = 0;
        if (ConfigServer::GetInstance()->FindChannelCoordinate(i, x, y))
        {
            m_ptabCapHeat->item(x, y)->setBackgroundColor(m_clrLevel[level]);
            m_clrCapColor[i] = m_clrLevel[level];
            m_bMergeStatus[i] = m_pchkLevel[level]->isChecked();
        }
    }

    for (int j = 0; j < m_ciLevelNum; ++j)
    {
        m_plabLevel2[j]->setText(QString::number(m_vetLevelCount[j]));
    }
}

void CapacitanceView::setCapCalculationSlot(int val, int val2)
{
    m_iIntervals = val2;
    m_pbtnCapStart->click();
    if (val == 1)
    {
        if (!m_pbtnCapStart->isChecked())
        {
            m_pbtnCapStart->click();
        }
    }
    else
    {
        if (m_pbtnCapStart->isChecked())
        {
            m_pbtnCapStart->click();
        }
    }
}

void CapacitanceView::getCapCalculationSlot(float* pval)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        pval[i] = m_vetCapValue[i];
    }
}

void CapacitanceView::OnTimerRecordCapdata(void)
{
    if (m_pFile != nullptr && m_pTextStream != nullptr)
    {
        auto hms = QDateTime::currentDateTime().toString("hh:mm:ss");
        *m_pTextStream << hms << ",";
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            //*m_pTextStream << m_strLevel[m_pControlView->m_vetControlListItem[i]->GetHoleState()] << ",";
            *m_pTextStream << m_vetCapValue[i] << ",";
        }
        *m_pTextStream << "\n";
    }
}

//void CapacitanceView::GetCapacitanceColorSlot(QColor* pclr)
//{
//    for (int i = 0; i < CHANNEL_NUM; ++i)
//    {
//        pclr[i] = m_clrCapColor[i];
//    }
//}
//
//void CapacitanceView::GetCapacitanceMergeSlot(bool* pmerge)
//{
//    for (int i = 0; i < CHANNEL_NUM; ++i)
//    {
//        pmerge[i] = m_bMergeStatus[i];
//    }
//}
