#include "ChannelStatusView.h"
#include <qheaderview.h>
#include <qaction.h>
#include <qpainter.h>
#include <qcolordialog.h>
#include <qlayout.h>
#include <qprocess.h>
#include <filesystem>
#include <Log.h>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#include "ControlView.h"
#include "CapUnitItem.h"
#include "CapacitanceView.h"


ChannelStatusView::ChannelStatusView(QWidget *parent) : QWidget(parent)
{
    InitCtrl();
    connect(&m_tmrChannelStatus, &QTimer::timeout, this, &ChannelStatusView::OnTimerChannelStatusSlot);
}

ChannelStatusView::~ChannelStatusView()
{
    if (m_pbtnStartStop->isChecked())
    {
        m_pbtnStartStop->click();
    }
}

bool ChannelStatusView::InitCtrl(void)
{
    m_pgrpChanState = new QGroupBox("Channel Status", this);
    m_ptabChanState = new QTableWidget(m_ciRow, m_ciCol, m_pgrpChanState);
    m_ptabChanState->setMinimumHeight(350);
    auto horheader = m_ptabChanState->horizontalHeader();
    horheader->setSectionResizeMode(QHeaderView::Stretch);
    horheader->setHidden(true);
    auto verheader = m_ptabChanState->verticalHeader();
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
                //QTableWidgetItem* itemChan = new QTableWidgetItem(QString("%1").arg(chan + 1));
                //itemChan->setBackground(QBrush(m_clrLevel[0]));
                //itemChan->setFlags(itemChan->flags() & (~Qt::ItemIsEditable));
                //itemChan->setTextAlignment(Qt::AlignCenter);
                //m_ptabChanState->setItem(i, j, itemChan);

                CapUnitItem* pitem = new CapUnitItem(chan, m_clrLevel[0], m_ptabChanState);
                m_ptabChanState->setCellWidget(i, j, pitem);
            }
        }
    }

    for (int i = 0; i < m_ciStageNum; ++i)
    {
        m_pgrpLevel[i] = new QGroupBox(this);
        m_plabLevel[i] = new QLabel(m_strLevel[i] + "(" + QString::number(i) + ")", m_pgrpLevel[i]);

        m_pbtnLevel[i] = new QToolButton(m_pgrpLevel[i]);
        auto& pbtn = m_pbtnLevel[i];
        QAction* action1 = new QAction(m_pgrpLevel[i]);
        action1->setToolTip(tr("Show Color"));
        pbtn->setDefaultAction(action1);
        SetColor(pbtn, m_clrLevel[i]);
        pbtn->setObjectName(QString::number(i, 10));
        //connect(pbtn, &QToolButton::clicked, this, &ChannelStatusView::OnClickChangeColor);

        m_vetLevelCount.emplace_back(0);

        m_plabLevel2[i] = new QLabel("Count", m_pgrpLevel[i]);
        m_plabLevel3[i] = new QLabel("--", m_pgrpLevel[i]);


        QGridLayout* gdlayout = new QGridLayout();
        gdlayout->addWidget(m_plabLevel[i], 0, 0);
        gdlayout->addWidget(pbtn, 0, 1);
        gdlayout->addWidget(m_plabLevel2[i], 1, 0);
        gdlayout->addWidget(m_plabLevel3[i], 1, 1);

        QHBoxLayout* horlayout3 = new QHBoxLayout();
        horlayout3->addLayout(gdlayout);
        horlayout3->addStretch(1);

        m_pgrpLevel[i]->setLayout(horlayout3);
    }

    m_pgrpLevel[3]->setVisible(false);

    m_pgrpStartStop = new QGroupBox(this);
    m_plabInterval = new QLabel("Intervals", m_pgrpStartStop);
    m_pispInterval = new QSpinBox(m_pgrpStartStop);
    m_pispInterval->setSuffix(" s");
    m_pispInterval->setMaximum(1);
    m_pispInterval->setMaximum(99999);
    m_pispInterval->setValue(600);
    m_pispInterval->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>定时保存通道状态数据的时间间隔。保存目录为/data/Status/</p></body></html>"));

    m_pchkMergeCap = new QCheckBox("MergeCap", m_pgrpStartStop);
    m_pchkMergeCap->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>是否合并电容分布热图</body></html>"));
    connect(m_pchkMergeCap, &QCheckBox::stateChanged, this, &ChannelStatusView::OnCheckMergeCap);

    //m_pbtnOpenFolder = new QPushButton("OpenFolder", m_pgrpStartStop);
    //m_pbtnOpenFolder->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>打开保存数据的文件夹</body></html>"));
    //connect(m_pbtnOpenFolder, &QToolButton::clicked, this, &ChannelStatusView::OnClickOpenFolder);

    m_pbtnStartStop = new QPushButton("Start", m_pgrpStartStop);
    m_pbtnStartStop->setCheckable(true);
    m_pbtnStartStop->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>启用/停用通道状态显示与保存</p>0 未嵌孔<br>1 单孔<br>2 多孔<br>4 破孔<br>5 弃孔<br>6 滑孔</body></html>"));
    connect(m_pbtnStartStop, &QToolButton::clicked, this, &ChannelStatusView::OnClickStartStop);

    QGridLayout* gdlayout2 = new QGridLayout();
    gdlayout2->addWidget(m_plabInterval, 0, 0);
    gdlayout2->addWidget(m_pispInterval, 0, 1);
    gdlayout2->addWidget(m_pchkMergeCap, 1, 0);
    //gdlayout2->addWidget(m_pbtnOpenFolder, 1, 0);
    gdlayout2->addWidget(m_pbtnStartStop, 1, 1);

    m_pgrpStartStop->setLayout(gdlayout2);


    int row = 0;
    int col = 0;
    QGridLayout* gdlayout8 = new QGridLayout();
    for (int i = 0; i < 4; ++i)
    {
        gdlayout8->addWidget(m_pgrpLevel[i], row, col++);
    }
    ++row;
    col = 0;
    for (int i = 4; i < m_ciStageNum; ++i)
    {
        gdlayout8->addWidget(m_pgrpLevel[i], row, col++);
    }
    gdlayout8->addWidget(m_pgrpStartStop, row, col++);

    QVBoxLayout* verlayout = new QVBoxLayout();
    verlayout->addWidget(m_ptabChanState);
    verlayout->addStretch(1);
    verlayout->addLayout(gdlayout8);
    verlayout->addStretch(5);

    m_pgrpChanState->setLayout(verlayout);


    QHBoxLayout* horlayout = new QHBoxLayout();
    horlayout->addWidget(m_pgrpChanState);

    setLayout(horlayout);

    return false;
}

bool ChannelStatusView::SetColor(QToolButton* pbtn, QColor clr)
{
    QPixmap pixmap(16, 16);
    QPainter painter(&pixmap);
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(QRect(0, 0, 15, 15));
    painter.fillRect(QRect(1, 1, 14, 14), clr);
    pbtn->defaultAction()->setIcon(QIcon(pixmap));

    return false;
}

bool ChannelStatusView::ReloadCtrl(void)
{
    for (int i = 0; i < m_ciRow; ++i)
    {
        for (int j = 0; j < m_ciCol; ++j)
        {
            int chan = 0;
            if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
            {
                //m_ptabChanState->item(i, j)->setText(QString("%1").arg(chan + 1));
                static_cast<CapUnitItem*>(m_ptabChanState->cellWidget(i, j))->SetChannel(chan);
            }
        }
    }
    return false;
}

bool ChannelStatusView::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&ChannelStatusView::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void ChannelStatusView::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    while (m_bRunning)
    {
        ConfigServer::MeSleep(1000);
    }

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool ChannelStatusView::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return true;
}

bool ChannelStatusView::StopThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    if (m_ThreadPtr != nullptr)
    {
        if (m_ThreadPtr->joinable())
        {
            m_ThreadPtr->join();
        }
        m_ThreadPtr.reset();
    }
    return true;
}

void ChannelStatusView::OnClickChangeColor(void)
{
    QObject* sendobject = QObject::sender();
    QToolButton* pbtn = qobject_cast<QToolButton*>(sendobject);
    int index = pbtn->objectName().toInt();
    QColor color = QColorDialog::getColor(m_clrLevel[index], this);
    if (color.isValid())
    {
        SetColor(pbtn, color);
        m_clrLevel[index] = color;

        for (int i = 0; i < m_ciRow; ++i)
        {
            for (int j = 0; j < m_ciCol; ++j)
            {
                int chan = 0;
                if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
                {
                    EEmbedPoreStatus stat = m_pControlView->m_vetControlListItem[chan]->GetHoleStatus();
                    //m_ptabChanState->item(i, j)->setBackground(m_clrLevel[stat]);
                    static_cast<CapUnitItem*>(m_ptabChanState->cellWidget(i, j))->SetFirstColor(m_clrLevel[stat]);
                }
            }
        }
    }
}

void ChannelStatusView::OnCheckMergeCap(bool chk)
{
    if (chk)
    {
        //QColor clrcap[CHANNEL_NUM] = { QColor(128, 128, 128) };
        //emit GetCapacitanceColorSignal(clrcap);
        //bool bmerge[CHANNEL_NUM] = { false };
        //emit GetCapacitanceMergeSignal(bmerge);
        for (int i = 0; i < m_ciRow; ++i)
        {
            for (int j = 0; j < m_ciCol; ++j)
            {
                int chan = 0;
                if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
                {
                    auto pitem = static_cast<CapUnitItem*>(m_ptabChanState->cellWidget(i, j));
                    pitem->SetSecondColor(m_pCapacitanceView->m_clrCapColor[chan]);//clrcap[chan]
                    pitem->SetShowSecond(m_pCapacitanceView->m_bMergeStatus[chan]);//true//bmerge[chan]
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < m_ciRow; ++i)
        {
            for (int j = 0; j < m_ciCol; ++j)
            {
                int chan = 0;
                if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
                {
                    auto pitem = static_cast<CapUnitItem*>(m_ptabChanState->cellWidget(i, j));
                    pitem->SetShowSecond(false);
                }
            }
        }
    }
}

void ChannelStatusView::OnClickOpenFolder(void)
{
#if 0
    int idx = 0;
    m_clrLevel[idx++] = QColor(200, 200, 200);
    m_clrLevel[idx++] = QColor(0, 128, 0);
    m_clrLevel[idx++] = QColor(255, 0, 0);
    m_clrLevel[idx++] = QColor(0, 176, 0);
    m_clrLevel[idx++] = QColor(178, 77, 155);
    m_clrLevel[idx++] = QColor(139, 194, 234);
    m_clrLevel[idx++] = QColor(0, 109, 156);

    for (int i = 0; i < m_ciStageNum; ++i)
    {
        SetColor(m_pbtnLevel[i], m_clrLevel[i]);
    }

    for (int i = 0; i < m_ciRow; ++i)
    {
        for (int j = 0; j < m_ciCol; ++j)
        {
            int chan = 0;
            if (ConfigServer::GetInstance()->FindCoordinateChannel(i, j, chan))
            {
                PoreHoleStatus stat = m_pControlView->m_vetControlListItem[chan]->GetHoleState();
                m_ptabChanState->item(i, j)->setBackground(m_clrLevel[stat]);
            }
        }
    }
#else

#if 0
#ifdef Q_OS_WIN32

    std::string strstatus = ConfigServer::GetCurrentPath() + "/etc/Status/";
    std::replace(strstatus.begin(), strstatus.end(), '/', '\\');
    char sparam[1024] = { 0 };
    sprintf(sparam, "/e,/n,%s", strstatus.c_str());///e,/select
    ::ShellExecuteA(nullptr, "open", "explorer.exe", sparam, NULL, SW_SHOWNORMAL);

#else

    std::string strstatus = "/data/Status/";
    QString openpath(strstatus.c_str());
    QString cmd = QString("sudo -u %1 nautilus %2").arg(QString::fromStdString(ConfigServer::GetInstance()->GetLoginUserName())).arg(openpath);//sudo -u $SUDO_USER
    QProcess::startDetached(cmd);//system(cmd.toStdString().c_str());//

#endif

#endif

#endif
}

void ChannelStatusView::OnClickStartStop(bool chk)
{
    if (chk)
    {
        //int porecntlmt = m_pControlView->m_pispPoreStatus->value();
        //double singlmt = m_pControlView->m_pdspSingleLimit->value();
        //double multlmt = m_pControlView->m_pdspMultiLimit->value();
        //double satulmt = m_pControlView->m_pdspSaturatedLimit->value();
        //for (int i = 0; i < CHANNEL_NUM; ++i)
        //{
        //    m_pControlView->m_vetControlListItem[i]->SetPoreLimitVolt(porecntlmt, singlmt, multlmt, satulmt);
        //}

#ifdef Q_OS_WIN32
        std::string strstatus = ConfigServer::GetCurrentPath() + "/etc/Status/";
#else
        std::string strstatus = "/data/Status/";
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
            LOGE("Save Status file FAILED!!!");
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

            OnTimerChannelStatusSlot();
            m_tmrChannelStatus.start(m_pispInterval->value() * 1000);
        }
        m_pbtnStartStop->setText("Stop");
    }
    else
    {
        OnTimerChannelStatusSlot();
        if (m_tmrChannelStatus.isActive())
        {
            m_tmrChannelStatus.stop();
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
        m_pbtnStartStop->setText("Start");
    }
}

void ChannelStatusView::SetChannelStatusSlot(bool val)
{
    if (val)
    {
        if (!m_pbtnStartStop->isChecked())
        {
            m_pbtnStartStop->click();
        }
    }
    else
    {
        if (m_pbtnStartStop->isChecked())
        {
            m_pbtnStartStop->click();
        }
    }
}

void ChannelStatusView::OnTimerChannelStatusSlot(void)
{
    for (int k = 0; k < m_ciStageNum; ++k)
    {
        m_vetLevelCount[k] = 0;
    }
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        int x = 0;
        int y = 0;
        if (ConfigServer::GetInstance()->FindChannelCoordinate(i, x, y))
        {
            //auto cur = m_pControlView->m_vetControlListItem[i]->GetCurCurrent().toFloat();
            //m_pControlView->m_vetControlListItem[i]->SetHoleStatus(cur);
            EEmbedPoreStatus stat = m_pControlView->m_vetControlListItem[i]->GetHoleStatus();

            //m_ptabChanState->item(x, y)->setBackgroundColor(m_clrLevel[stat]);
            static_cast<CapUnitItem*>(m_ptabChanState->cellWidget(x, y))->SetFirstColor(m_clrLevel[stat]);
            ++m_vetLevelCount[stat];
        }
    }

    for (int j = 0; j < m_ciStageNum; ++j)
    {
        m_plabLevel3[j]->setText(QString::number(m_vetLevelCount[j]));
    }

    if (m_pFile != nullptr && m_pTextStream != nullptr)
    {
        auto hms = QDateTime::currentDateTime().toString("hh:mm:ss");
        *m_pTextStream << hms << ",";
        for (int i = 0; i < CHANNEL_NUM; ++i)
        {
            //*m_pTextStream << m_strLevel[m_pControlView->m_vetControlListItem[i]->GetHoleState()] << ",";
            *m_pTextStream << m_pControlView->m_vetControlListItem[i]->GetHoleStatus() << ",";
        }
        *m_pTextStream << "\n";
    }

    //static int siflag = 0;
    //if (siflag == 0)
    //{
    //    if (ConfigServer::GetInstance()->GetCurrentStage() > ECurrentStage::ECS_PORE_INSERT)
    //    {
    //        siflag = 1;
    //        if (!m_pControlView->m_pbtnDegatingEnable->isChecked())
    //        {
    //            m_pControlView->m_pbtnDegatingEnable->click();
    //        }
    //        if (!m_pControlView->m_pchkDegatingAllCheck->isChecked())
    //        {
    //            m_pControlView->m_pchkDegatingAllCheck->click();
    //        }
    //    }
    //}
}
