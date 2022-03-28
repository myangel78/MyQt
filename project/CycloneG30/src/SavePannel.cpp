#include "SavePannel.h"
#include "qgroupbox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>

#ifdef Q_OS_WIN32
#include <windows.h>
//#include <shellapi.h>
#endif
#include <msgpack.hpp>

#include "Log.h"
#include "ConfigServer.h"
#include "TranslateDialog.h"
#include "PlotReviewDialog.h"
#include "RecordSettingDialog.h"
#include "HttpClient.h"


SavePannel::SavePannel(QWidget *parent):QWidget(parent)
{
    m_vetSelChannel.resize(CHANNEL_NUM);
    m_vetSaveFile.resize(CHANNEL_NUM);
    m_vetFileLength.resize(CHANNEL_NUM);
    m_vctFileCurIndex.resize(CHANNEL_NUM);
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetSelChannel[i] = false;
        m_vetSaveFile[i] = nullptr;
        m_vetFileLength[i].first = 0;
        m_vetFileLength[i].second = 0;
        m_vctFileCurIndex[i] = 0;
#if RTSA_DATA_TEST
        m_vetSaveFile2[i] = nullptr;
        m_vetFileLength2[i].first = 0;
        m_vetFileLength2[i].second = 0;
#endif
    }
    for (int i = 0; i < 50; ++i)
    {
        m_arMockChannelData[i] = 10.0;
    }

    InitCtrl();

    connect(this, SIGNAL(stopRealtimeSequencingAnalysisSignal(const QString)), this, SLOT(stopRealtimeSequencingAnalysisSlot(const QString)));
    connect(m_pRecordSettingDialog, SIGNAL(checkAlgorithmServerStatusCodeSignal(void)), this, SLOT(checkAlgorithmServerStatusCodeSlot(void)));
    connect(&m_tmrSequencingStatus, &QTimer::timeout, this, &SavePannel::TimerSequencingStatusSlot);
    connect(&m_tmrCheckAlgorithmServerState, &QTimer::timeout, this, &SavePannel::timerCheckAlgorithmServerStateSlot);

    m_algorithmReads = 0;
    m_algorithmAdapter = 0;
}

SavePannel::~SavePannel()
{
}

void SavePannel::InitCtrl(void)
{

    m_pgrpSavingData = new QGroupBox("SavingData", this);

    m_pbtnConfig = new QPushButton("Config", m_pgrpSavingData);

    m_pbtnConfig->setIcon(QIcon(":/img/img/eject.png"));
    m_pbtnConfig->setIconSize(QSize(30, 30));

    m_pbtnStart = new QPushButton("Start", m_pgrpSavingData);
    m_pbtnStart->setIcon(QIcon(":/img/img/run.png"));
    m_pbtnStart->setIconSize(QSize(30, 30));
    m_pbtnStart->setCheckable(true);

    m_pispFileSize = new QSpinBox(m_pgrpSavingData);
    m_pispFileSize->setMinimum(1);
    m_pispFileSize->setMaximum(128);
    m_pispFileSize->setValue(128);
    m_pispFileSize->setSuffix(" M");

    m_pbtnTranslate = new QPushButton("Translate", m_pgrpSavingData);
    m_pbtnReplot = new QPushButton("Plot Review", m_pgrpSavingData);
    m_pbtnRawReplot = new QPushButton("Raw Review", m_pgrpSavingData);

    m_peditConnectIP = new QLineEdit("127.0.0.1", m_pgrpSavingData);//192.168.0.127
    m_peditConnectIP->setVisible(false);

    m_peditSequencingStatus = new QLineEdit("Realtime Sequencing Analysis Status...", m_pgrpSavingData);
    m_peditSequencingStatus->setReadOnly(true);


    //long long rds = 987654321;
    //long long adp = 123456789;
    //m_peditSequencingStatus->setText(QString("Reads=%1, Adapter=%2").arg(rds).arg(adp));

    m_peditSavePath = new QLineEdit(m_pgrpSavingData);
    m_peditSavePath->setReadOnly(true);

    m_pbtnChooseFolder = new QPushButton("ChooseFloder", m_pgrpSavingData);
    m_pbtnOpenFolder = new QPushButton("OpenFolder", m_pgrpSavingData);


    connect(m_pbtnConfig, &QPushButton::clicked, this, &SavePannel::OnClickConfig);
    connect(m_pbtnStart, &QPushButton::toggled, this, &SavePannel::OnClickStart);
    connect(m_pbtnTranslate, &QPushButton::clicked, this, &SavePannel::OnClickTranslate);
    connect(m_pbtnReplot, &QPushButton::clicked, this, &SavePannel::OnClickReplot);
    connect(m_pbtnRawReplot, &QPushButton::clicked, this, &SavePannel::OnClickRawReplot);

    connect(m_pbtnChooseFolder, &QPushButton::clicked, this, &SavePannel::OnClickChooseFolder);
    connect(m_pbtnOpenFolder, &QPushButton::clicked, this, &SavePannel::OnClickOpenFolder);


    QHBoxLayout* horlayout1 = new QHBoxLayout();
    horlayout1->addWidget(m_pbtnConfig);
    horlayout1->addWidget(m_pbtnStart);
    horlayout1->addWidget(m_pispFileSize);
//    horlayout1->addStretch(1);
    horlayout1->addWidget(m_pbtnTranslate);
    horlayout1->addWidget(m_pbtnReplot);
    horlayout1->addWidget(m_pbtnRawReplot);
    horlayout1->addWidget(m_peditConnectIP);
    horlayout1->addStretch();

    QHBoxLayout* horlayout2 = new QHBoxLayout();
    horlayout2->addWidget(m_peditSavePath);
    horlayout2->addWidget(m_pbtnChooseFolder);
    horlayout2->addWidget(m_pbtnOpenFolder);
    horlayout2->addStretch();

    QHBoxLayout* horlayout3 = new QHBoxLayout();
    horlayout3->addWidget(m_peditSequencingStatus,10);

    QVBoxLayout* verlayout1 = new QVBoxLayout();
    verlayout1->addLayout(horlayout1);
    verlayout1->addLayout(horlayout3);
//    verlayout1->addWidget(m_peditSequencingStatus);
    verlayout1->addLayout(horlayout2);

    m_pgrpSavingData->setLayout(verlayout1);


    //Auto stop saving
    m_pgrpAutoStopAndSave = new QGroupBox("AutoStopAndSave", this);
    //m_pgrpAutoStopAndSave->setContentsMargins(0, 5, 0, 3);

    m_plabAutoStopTime = new QLabel("<font color=red>AutoStop</font>", m_pgrpAutoStopAndSave);
    m_pispAutoStopTime = new QSpinBox(m_pgrpAutoStopAndSave);
    m_pispAutoStopTime->setMinimum(1);
    m_pispAutoStopTime->setMaximum(1439);//23:39:00
    m_pispAutoStopTime->setValue(90);
    m_pispAutoStopTime->setSuffix(" min");

    SetTimehms();

    m_plcdTimer = new QLCDNumber(8, m_pgrpAutoStopAndSave);
    m_plcdTimer->setSegmentStyle(QLCDNumber::Flat);
    m_plcdTimer->setFrameShape(QFrame::Panel);
    m_plcdTimer->display(m_tmAutoStopTime.toString("hh:mm:ss"));

    m_plabTimeUp = new QLabel(g_qlabelStyle[2], m_pgrpAutoStopAndSave);
    m_plabTimeUp->setVisible(false);

    m_pbtnStartAuto = new QPushButton("StartAuto", m_pgrpAutoStopAndSave);
    m_pbtnStartAuto->setCheckable(true);

    m_pbtnResetAuto = new QPushButton("ResetAuto", m_pgrpAutoStopAndSave);

    connect(m_pbtnStartAuto, &QPushButton::toggled, this, &SavePannel::OnClickStartAuto);
    connect(m_pbtnResetAuto, &QPushButton::clicked, this, &SavePannel::OnClickResetAuto);
    connect(&m_tmerAutoStopTime, &QTimer::timeout, this, &SavePannel::TimerUpdateTimeSlot);



    QHBoxLayout* horlayout5 = new QHBoxLayout();
    horlayout5->addWidget(m_plabAutoStopTime);
    horlayout5->addWidget(m_pispAutoStopTime);
    horlayout5->addWidget(m_plcdTimer);
    horlayout5->addWidget(m_plabTimeUp);
    horlayout5->addStretch(1);

    QHBoxLayout* horlayout6 = new QHBoxLayout();
    horlayout6->addWidget(m_pbtnStartAuto);
    horlayout6->addWidget(m_pbtnResetAuto);
    horlayout6->addStretch();

    QVBoxLayout* verlayout2 = new QVBoxLayout();
    verlayout2->addStretch();
    verlayout2->addLayout(horlayout5);
    verlayout2->addLayout(horlayout6);
    verlayout2->addStretch();


    m_pgrpAutoStopAndSave->setLayout(verlayout2);

    InitSimulateCtrl();
    QHBoxLayout* horlayout = new QHBoxLayout();
    horlayout->addWidget(m_pgrpSavingData);
    horlayout->addWidget(m_pgrpAutoStopAndSave);
    horlayout->setStretch(0,4);
    horlayout->setStretch(1,1);
//    setLayout(horlayout);
    QVBoxLayout* vertlayout = new QVBoxLayout();
    vertlayout->addWidget(m_pSimulateGrpbx);
    vertlayout->addLayout(horlayout);
    setLayout(vertlayout);

    m_pRecordSettingDialog = new RecordSettingDialog(this);

#ifdef Q_OS_WIN32
#ifdef EASY_PROGRAM_DEBUGGING
    std::string strdatapath = ConfigServer::GetCurrentPath() + "/etc/Data";
#else
    std::string strdatapath = ""; //应孔老板要求，删除默认路径
#endif
#else
    std::string strdatapath = "/data/raw_data";
    m_peditSavePath->setEnabled(false);
    m_pbtnOpenFolder->setEnabled(false);
#endif
    //std::filesystem::path pathdatapath(strdatapath);
    //if (!std::filesystem::exists(pathdatapath))
    //{
    //    std::filesystem::create_directories(pathdatapath);
    //}
    m_peditSavePath->setText(strdatapath.c_str());
    m_pRecordSettingDialog->SetSavePath(strdatapath);

}
void SavePannel::InitSimulateCtrl(void)
{
    m_vetSimulateFiles.resize(CHANNEL_NUM);
    m_pSimulateGrpbx=  new QGroupBox(QStringLiteral("Loading Simulate Dir"),this);
    m_pSimulateGrpbx->setVisible(false);
    m_pbtnSimulateChooseFolder = new QPushButton("ChooseFloder", this);
    m_peditSimulateSavePath = new QLineEdit(this);
    m_peditSimulateSavePath->setReadOnly(true);

    QHBoxLayout *simulhorlayout = new QHBoxLayout();
    simulhorlayout->addWidget(m_peditSimulateSavePath);
    simulhorlayout->addWidget(m_pbtnSimulateChooseFolder);

    m_pSimulateGrpbx->setLayout(simulhorlayout);

    connect(m_pbtnSimulateChooseFolder,&QPushButton::clicked,[=](){
        QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Save Directory"),
            "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!dir.isEmpty())
        {
            m_peditSimulateSavePath->setText(dir.replace("\\", "/"));
        }
    });

}


void SavePannel::OnClickChooseFolder(void)
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Save Directory"),
        "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        m_peditSavePath->setText(dir.replace("\\", "/"));
        m_pRecordSettingDialog->SetSavePath(dir.toStdString());
    }
}

bool SavePannel::InitTransfer(void)
{
    bool bret = false;
    int iport = 6379;
    int idbnum = 0;
    std::string strhost = "127.0.0.1";//ConfigServer::GetInstance()->GetTaskRedisParam(iport, idbnum);
    //if (strhost.size() <= 0)
    {
        strhost = m_peditConnectIP->text().toStdString();//"127.0.0.1";//"172.16.16.44";//"192.168.0.127";//
        iport = 9099;//9096;
        idbnum = 2;//6;
    }

    for (int i = 0; i < m_ciThread; ++i)
    {
        bret = m_redisAccess[i].SetAccessParam(strhost.c_str(), iport, nullptr);//m_iRedisPort[i]
        if (!bret)
        {
            LOGE("Redis set param failed!!!={}", i);
            return false;
        }
        bret = m_redisAccess[i].Connecting();
        if (!bret)
        {
            LOGE("Connect redis failed!!!={}", i);
            return false;
        }
        bret = m_redisAccess[i].selectDB(idbnum);
        if (!bret)
        {
            LOGE("Select redis db failed!!!={}", i);
            return false;
        }
    }

    return true;
}

void SavePannel::OnClickOpenFolder(void)
{
#ifdef Q_OS_WIN32

    char sparam[1024] = { 0 };
    sprintf(sparam, "/e,/n,%s", m_peditSavePath->text().replace("/", "\\").toStdString().c_str());///e,/select
    ::ShellExecuteA(nullptr, "open", "explorer.exe", sparam, NULL, SW_SHOWNORMAL);

#else

    QString path = m_peditSavePath->text().replace("\\", "/");
    QString cmd = QString("sudo nautilus %1").arg(path);
    QProcess::startDetached(cmd);

#endif
}

void SavePannel::OnClickTranslate(void)
{
#if 0
    m_bRedisReady = InitTransfer();
    return;
#endif
    TranslateDialog trandlg(this);
    trandlg.exec();
}

void SavePannel::OnClickReplot(void)
{
#if 0
    msgpack::sbuffer sbuf;
    msgpack::packer<msgpack::sbuffer> pker(&sbuf);
    pker.pack_map(4);
    pker.pack("data_name");
    pker.pack("20211130114445_LAB256V2_5K_PC28_10_B0_HD4_AD1_Ecoli_LeiLeyan");//m_strSaveName.c_str()
    pker.pack("frame_num");
    pker.pack(120);
    pker.pack("is_last_frame");
    pker.pack(0);

    const int cinum = 2;// 1024;
    std::vector<float> vetmock[cinum];
    vetmock[0].resize(250);
    vetmock[1].resize(240);
    pker.pack(std::string("data_info"));
    pker.pack_array(cinum);
    for (int i = 0; i < cinum; ++i)
    {
        pker.pack_map(2);
        pker.pack("chn_num");
        pker.pack(i + 1);
        pker.pack("data");
        pker.pack(vetmock[i]);//m_arMockChannelData
    }
    bool bph = m_redisAccess.Rpushb("testcyclone_data", sbuf.data(), sbuf.size());
    return;

    // 反序列化
    msgpack::unpacked  unpack;
    //std::string str = ("\x84\xa9data_name\xd9<20211130114445_LAB256V2_5K_PC28_10_B0_HD4_AD1_Ecoli_LeiLeyan\xa9frame_num\xcd\x15\x88\xadis_last_frame\x00\xa9data_info\x90\x82\xa7chn_num\x01\xa4data");
    //sbuf.write(str.c_str(), str.size());
    msgpack::unpack(unpack, sbuf.data(), sbuf.size());

    // 直接输出结果
    msgpack::object  obj = unpack.get();
    std::cout << obj << std::endl;
    return;
#endif
#if 0
    PlotReviewDialog replotdlg("Plot review",this);
    replotdlg.exec();
#else
    if(m_pPlotReviewDialog == nullptr)
    {
        m_pPlotReviewDialog = new PlotReviewDialog("Plot review",this);
    }
    m_pPlotReviewDialog->show();
#endif
}
void SavePannel::OnClickRawReplot(void)
{
#if 0
    PlotReviewDialog replotdlg("Raw data",this);
    replotdlg.exec();
#else
    if(m_pRawReviewDialog == nullptr)
    {
        m_pRawReviewDialog = new PlotReviewDialog("Raw data",this);
}
    m_pRawReviewDialog->show();
#endif

}

void SavePannel::OnClickStart(bool chk)
{
#ifdef Q_OS_WIN32
#else
    m_pRecordSettingDialog->EnableNanoporeSequencingCheck(!chk);
#endif
    if (chk)
    {
        ConfigServer::GetRunLog("StartSaveData", "top -b -o %MEM", 5000);
//        m_strRawSavePath = m_peditSavePath->text().toStdString();
        m_strRawSavePath = m_pRecordSettingDialog->CreateDataPath();
        if (!m_strRawSavePath.empty())
        {
#if RTSA_DATA_TEST
            m_strRawSavePath2 = m_strRawSavePath + "2";
            std::filesystem::path pathSave2(m_strRawSavePath2);
            if (!std::filesystem::exists(pathSave2))
            {
                if (std::filesystem::create_directories(pathSave2))
                {
                    LOGE("Create RAW2 FAILED!!!");
                }
            }
#endif
            std::filesystem::path pathSave(m_strRawSavePath);
            pathSave = pathSave.parent_path();
            m_strTaskPath = pathSave.string();
            m_strSaveName = pathSave.filename().string();

            emit setDegatingDataNameSignal(QString::fromStdString(m_strSaveName));

            OpenChannelsFile();
            ecoli::CEcoliLog::GetInstance()->InitDegDataLogger();
            ecoli::CEcoliLog::GetInstance()->InitDegateLogger();
            StartThread();
            ConfigServer::GetInstance()->SetSaveData(true);

            if (m_pRecordSettingDialog->GetNanoporeSequencingCheck())
            {
                if (!ConfigServer::GetInstance()->GetAlgorithmServerStatus())
                {
                    LOGW("Algorithm Server has not been Ready.");
                }
                else if (!m_bRedisReady)
                {
                    LOGW("Redis Server has not been Ready.");
                }
                else
                {
                    UploadRunIdToServer();
                    LOGI("Now Will Start Realtime Sequencing Analysis!");
                    //QMessageBox::StandardButton msg = QMessageBox::Yes;
                    //msg = QMessageBox::question(this, "Realtime Sequencing Analysis", "Do you want to start Realtime Sequencing Analysis?");
                    //if (msg == QMessageBox::Yes)
                    for (int i = 0; i < m_ciThread; ++i)
                    {
                        //register task
                        std::string strkey("waiting_task");
                        std::string strfield = m_strSaveName;
                        std::string strvalue;
                        m_pRecordSettingDialog->Json4RegisterTask(strvalue);
                        m_redisAccess[i].SetHash(strkey.c_str(), strfield.c_str(), strvalue.c_str());

                        //head frame
                        std::string strHeadkey = "cyclone_data";// strfield + "_orig_data";
                        std::string strcontent = m_pRecordSettingDialog->GetExperimentConfigs();

                        msgpack::sbuffer sbuf;
                        msgpack::packer<msgpack::sbuffer> pker(&sbuf);
                        pker.pack_map(3);
                        pker.pack("data_name");
                        pker.pack(strfield.c_str());
                        pker.pack("position");
                        pker.pack("H");
                        pker.pack("content");
                        pker.pack(strcontent.c_str());

                        m_redisAccess[i].Rpushb(strHeadkey.c_str(), sbuf.data(), sbuf.size());
                    }

                    StartThread2();
                    ConfigServer::GetInstance()->SetRealtimeSequence(true);
                    m_tmrSequencingStatus.start(1000);

#ifdef Q_OS_WIN32
#else
                    StartThread3();
#endif
                }
            }

            m_pbtnStart->setIcon(QIcon(":/img/img/stop.png"));
            m_pbtnStart->setText("Stop");
            m_pbtnTranslate->setEnabled(false);
            m_pbtnReplot->setEnabled(false);

            return;
        }
        m_pbtnStart->setChecked(false);
        m_pbtnStart->setIcon(QIcon(":/img/img/run.png"));
        m_pbtnStart->setText("Start");
        QMessageBox::critical(this, "Error", "Creat save Directory falied!!!");
        m_pbtnTranslate->setEnabled(true);
        m_pbtnReplot->setEnabled(true);

    }
    else
    {
        EndThread();
        ConfigServer::GetInstance()->SetSaveData(false);
        m_pRecordSettingDialog->UpdateEndtime4Json();
        LOGI("Endsavedata....");

        if (ConfigServer::GetInstance()->GetRealtimeSequence())
        {
            EndThread2();
            EndThread3();
            ConfigServer::GetInstance()->SetRealtimeSequence(false);
            if (m_tmrSequencingStatus.isActive())
            {
                m_tmrSequencingStatus.stop();
            }
            StopThread2();
            StopThread3();
            //TODO:send last frame...
            std::string strfield = m_strSaveName;
            std::string strHeadkey = "cyclone_data";// strfield + "_orig_data";

            int block = CHANNEL_NUM / m_ciThread;
            for (int j = 0; j < m_ciThread; ++j)
            {
                //last frame
                msgpack::sbuffer sbuf;
                msgpack::packer<msgpack::sbuffer> pker(&sbuf);
                pker.pack_map(4);
                pker.pack("data_name");
                pker.pack(strfield.c_str());
                pker.pack("frame_num");
                pker.pack(0);
                pker.pack("is_last_frame");
                pker.pack(1);
                //pker.pack("file_info");
                //pker.pack("O");

                pker.pack(std::string("data_info"));
                pker.pack_array(m_arSequencedChannel[j]);//m_iSequencedChannel
                for (int i = j * block, n = j * block + block; i < n; ++i)
                {
                    if (m_arSequencedChannal[i])
                    {
                        pker.pack_map(2);
                        pker.pack("chn_num");
                        pker.pack(i + 1);
                        pker.pack("data");
                        //pker.pack_array(1);
                        pker.pack(m_arMockChannelData);

                        m_arSequencedChannal[i] = false;
                    }
                    m_vetSequencingData[i].clear();
                }

                m_redisAccess[j].Rpushb(strHeadkey.c_str(), sbuf.data(), sbuf.size());

                //tail frame
                std::string strcontent = m_pRecordSettingDialog->GetExperimentConfigs();
                msgpack::sbuffer sbuf2;
                msgpack::packer<msgpack::sbuffer> pker2(&sbuf2);
                pker2.pack_map(3);
                pker2.pack("data_name");
                pker2.pack(strfield.c_str());
                pker2.pack("position");
                pker2.pack("T");
                pker2.pack("content");
                pker2.pack(strcontent.c_str());

                m_redisAccess[j].Rpushb(strHeadkey.c_str(), sbuf2.data(), sbuf2.size());

                m_arSequencedChannel[j] = 0;
            }

            m_strSaveName = "nodata";
            //m_iSequencedChannel = 0;
        }
        StopThread();
        CloseChannelsFile();
        ecoli::CEcoliLog::GetInstance()->DeleteDegateLogger();
        ecoli::CEcoliLog::GetInstance()->UninitDegDataLogger();
        //record the reads and adapters count to files

#ifdef Q_OS_WIN32
#else
        //UpdateFolderOwner(m_strTaskPath);
//        UploadRawDataFolder(QString::fromStdString(m_strTaskPath));
#endif

        m_pbtnStart->setIcon(QIcon(":/img/img/run.png"));
        m_pbtnStart->setText("Start");
        m_pbtnTranslate->setEnabled(true);
        m_pbtnReplot->setEnabled(true);
        LOGI("Endsavedata2....");
    }
}




void SavePannel::OnClickConfig(void)
{
    m_pRecordSettingDialog->show();
    ConfigServer::WidgetPosAdjustByParent(this->parentWidget()->parentWidget(), m_pRecordSettingDialog);
}


bool SavePannel::OpenChannelsFile(void)
{
#ifdef Q_OS_WIN32
    _setmaxstdio(4096);
#else
//    system("ulimit -SHn 8192");
    system("ulimit -SHn 4096");
#endif
    //m_iSelectChan = 0;
    for (int i = 0; i < m_ciThread; ++i)
    {
        m_arSelectChan[i] = 0;
    }
    m_iSplitSize = m_pispFileSize->value();
    m_iSplitSize = (m_iSplitSize << 18);// (isplit << 20) / 4;
    auto& chansel = m_pRecordSettingDialog->GetChannelSelect();
#if 1
    int block = CHANNEL_NUM / m_ciThread;
    for (int j = 0; j < m_ciThread; ++j)
    {
        for (int i = j * block, n = j * block + block; i < n; ++i)
        {
            bool sel = chansel[i];
            m_vetSelChannel[i] = sel;
            if (sel && m_vetSaveFile[i] == nullptr)
            {
                int ch = i + 1;
                char archanpath[1024] = { 0 };
                //            sprintf(archanpath, "%s/channel%d_ADC%d_CH%d", m_strRawSavePath.c_str(), ch,i/16,i%16);
                sprintf(archanpath, "%s/channel%d", m_strRawSavePath.c_str(), ch);
                std::filesystem::path pathchan(archanpath);
                if (!std::filesystem::exists(pathchan))
                {
                    if (!std::filesystem::create_directories(pathchan))
                    {
                        LOGE("create_folder: {} failed!!!", archanpath);
                        continue;
                    }
                }
                //            sprintf(archanpath, "%s/channel%d_ADC%d_CH%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch,i/16,i%16 ,ch, m_vetFileLength[i].second++);
                sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch, ch, m_vetFileLength[i].second++);
                m_vetSaveFile[i] = fopen(archanpath, "wb");
                if (m_vetSaveFile[i] == nullptr)
                {
                    LOGE("Open file: {} failed!!!", archanpath);
                }
                ++m_arSelectChan[j];

#if RTSA_DATA_TEST
                sprintf(archanpath, "%s/channel%d", m_strRawSavePath2.c_str(), ch);
                std::filesystem::path pathchan2(archanpath);
                if (!std::filesystem::exists(pathchan2))
                {
                    if (!std::filesystem::create_directories(pathchan2))
                    {
                        LOGE("create_folder: {} failed!!!", archanpath);
                        continue;
                    }
                }
                //            sprintf(archanpath, "%s/channel%d_ADC%d_CH%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch,i/16,i%16 ,ch, m_vetFileLength[i].second++);
                sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath2.c_str(), ch, ch, m_vetFileLength2[i].second++);
                m_vetSaveFile2[i] = fopen(archanpath, "wb");
                if (m_vetSaveFile2[i] == nullptr)
                {
                    LOGE("Open file: {} failed!!!", archanpath);
                }
#endif
            }
        }
    }
#else
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        bool sel = chansel[i];
        m_vetSelChannel[i] = sel;
        if (sel && m_vetSaveFile[i] == nullptr)
        {
            int ch = i + 1;
            char archanpath[600] = { 0 };
//            sprintf(archanpath, "%s/channel%d_ADC%d_CH%d", m_strRawSavePath.c_str(), ch,i/16,i%16);
            sprintf(archanpath, "%s/channel%d", m_strRawSavePath.c_str(), ch);
            std::filesystem::path pathchan(archanpath);
            if (!std::filesystem::exists(pathchan))
            {
                if (!std::filesystem::create_directories(pathchan))
                {
                    LOGE("create_folder: {} failed!!!", archanpath);
                    continue;
                }
            }
//            sprintf(archanpath, "%s/channel%d_ADC%d_CH%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch,i/16,i%16 ,ch, m_vetFileLength[i].second++);
            sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch,ch, m_vetFileLength[i].second++);
            m_vetSaveFile[i] = fopen(archanpath, "wb");
            if (m_vetSaveFile[i] == nullptr)
            {
                LOGE("Open file: {} failed!!!", archanpath);
            }
            //++m_iSelectChan;
        }
    }
#endif

    return true;
}

bool SavePannel::CloseChannelsFile(void)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (/*m_vetSelChannel[i] &&*/ m_vetSaveFile[i] != nullptr)
        {
            fclose(m_vetSaveFile[i]);
            m_vetSaveFile[i] = nullptr;
#if RTSA_DATA_TEST
            fclose(m_vetSaveFile2[i]);
            m_vetSaveFile2[i] = nullptr;
#endif
        }

        m_vetFileLength[i].first = 0;
        m_vetFileLength[i].second = 0;
        m_vctFileCurIndex[i] = m_vetFileLength[i].first;
#if RTSA_DATA_TEST
        m_vetFileLength2[i].first = 0;
        m_vetFileLength2[i].second = 0;
#endif
    }
    //std::shared_ptr<SDataHandle> dtclear;
    //while (m_queDataHandle.try_dequeue(dtclear));

    return false;
}


bool SavePannel::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&SavePannel::ThreadFunc, this);
        return m_ThreadPtr != nullptr;
    }
    return false;
}

void SavePannel::ThreadFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexTask);
            m_cv.wait(lck, [&] {return !m_bRunning || m_queDataHandle.size_approx() > 0; });
        }
        //int quepacksz = m_queDataHandle.size_approx();
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataHandle.try_dequeue(dtpk);
        if (bret)
        {
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                if (m_vetSelChannel[i] && m_vetSaveFile[i] != nullptr)
                {
                    auto len = fwrite(&dtpk->dataHandle[i][0], sizeof(float), dtpk->dataHandle[i].size(), m_vetSaveFile[i]);
                    m_vetFileLength[i].first += len;
                    m_vctFileCurIndex[i] = m_vetFileLength[i].first;

                    if (m_vetFileLength[i].first >= m_iSplitSize)//FILEDATA_BUFSIZE
                    {
                        fclose(m_vetSaveFile[i]);
                        char archanpath[512] = { 0 };
                        int ch = i + 1;
//                        sprintf(archanpath, "%s/channel%d_ADC%d_CH%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch,i/16,i%16 ,ch, m_vetFileLength[i].second++);
                          sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch,ch, m_vetFileLength[i].second++);
                        m_vetSaveFile[i] = fopen(archanpath, "wb");
                        m_vetFileLength[i].first = 0;
                        m_vctFileCurIndex[i] = m_vetFileLength[i].first;
                    }
                }
            }
        }
    }
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle.try_dequeue(dtclear));

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool SavePannel::EndThread(void)
{
    m_bRunning = false;
    m_cv.notify_one();
    return false;
}

bool SavePannel::StopThread(void)
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
    return false;
}

bool SavePannel::StartThread2(void)
{
    if (!m_bRunning2 && m_ThreadPtr2 == nullptr)
    {
        m_bRunning2 = true;
        m_ThreadPtr2 = std::make_shared<std::thread>(&SavePannel::ThreadFunc2Seq, this);
        return m_ThreadPtr2 != nullptr;
    }
    return false;
}

void SavePannel::ThreadFunc2Seq(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    long long llFrameNum = 0;
    int block = CHANNEL_NUM / m_ciThread;
    //auto m_startCnt = std::chrono::steady_clock::now();
    //int pkgcnt = 0;
    while (m_bRunning2)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexTask2);
            m_cv2.wait(lck, [&] {return !m_bRunning2 || m_queDataHandle2.size_approx() > 0; });
        }
        if (!m_bRunning2)
        {
            break;
        }
        //int quepacksz = m_queDataHandle2.size_approx();
        std::shared_ptr<ConfigServer::SDataHandle> dtpk;
        bool bret = m_queDataHandle2.try_dequeue(dtpk);
        if (bret)
        {
#if 1
            bool chreach[CHANNEL_NUM] = { false };
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                if (m_vetSelChannel[i])
                {
                    m_vetSequencingData[i].insert(m_vetSequencingData[i].end(), dtpk->dataHandle[i].begin(), dtpk->dataHandle[i].end());
                    chreach[i] = (m_vetSequencingData[i].size() >= SEND_PACKAGE_SIZE);
                }
            }
            bool isreach = true;
            while (isreach && m_bRunning2)
            {
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    if (m_vetSelChannel[i])
                    {
                        if (!chreach[i])
                        {
                            isreach = false;
                            break;
                        }
                    }
                }
                if (isreach)
                {
                    for (int i = 0; i < CHANNEL_NUM; ++i)
                    {
                        m_vetSequencingData2[i].resize(SEND_PACKAGE_SIZE);
                        if (m_vetSelChannel[i])
                        {
                            memcpy(&m_vetSequencingData2[i][0], &m_vetSequencingData[i][0], SEND_PACKAGE_SIZE * sizeof(float));
                            int rest = m_vetSequencingData[i].size() - SEND_PACKAGE_SIZE;
                            if (rest > 0)
                            {
                                std::vector<float> vettemp;
                                vettemp.resize(rest);
                                int leng = rest * sizeof(float);
                                memcpy(&vettemp[0], &m_vetSequencingData[i][SEND_PACKAGE_SIZE], leng);
                                m_vetSequencingData[i].resize(rest);
                                memcpy(&m_vetSequencingData[i][0], &vettemp[0], leng);
                                chreach[i] = (rest >= SEND_PACKAGE_SIZE);
                            }
                            else
                            {
                                m_vetSequencingData[i].clear();
                                chreach[i] = false;
                            }
                        }
                    }

                    ++llFrameNum;
                    for (int j = 0; j < m_ciThread; ++j)
                    {
                        //data frame
                        msgpack::sbuffer sbuf;
                        msgpack::packer<msgpack::sbuffer> pker(&sbuf);
                        pker.pack_map(4);
                        //pker.pack("startpoint");
                        //pker.pack(ConfigServer::GetCurrentTimestamp());
                        pker.pack("data_name");
                        pker.pack(m_strSaveName.c_str());
                        pker.pack("frame_num");
                        pker.pack(llFrameNum);
                        pker.pack("is_last_frame");
                        pker.pack(0);
                        //pker.pack("file_info");
                        //pker.pack("O");

                        pker.pack(std::string("data_info"));
                        pker.pack_array(m_arSelectChan[j]);//m_iSelectChan
                        for (int i = j * block, n = j * block + block; i < n; ++i)
                        {
                            if (m_vetSelChannel[i])
                            {
                                pker.pack_map(2);
                                pker.pack("chn_num");
                                pker.pack(i + 1);
                                pker.pack("data");
                                //pker.pack_array(1);
                                pker.pack(m_vetSequencingData2[i]);

                                if (!m_arSequencedChannal[i])
                                {
                                    m_arSequencedChannal[i] = true;
                                    ++m_arSequencedChannel[j];//m_iSequencedChannel
                                }
                            }
                        }
#if 0
                        ++pkgcnt;
                        auto m_stopCnt = std::chrono::steady_clock::now();
                        auto distTime = std::chrono::duration<double>(m_stopCnt - m_startCnt).count();
                        if (distTime >= 1.0)
                        {
                            LOGI("Time={}, Count={}", distTime, pkgcnt);
                            pkgcnt = 0;
                            m_startCnt = std::chrono::steady_clock::now();
                        }
#else
                        try
                        {
                            bool bph = m_redisAccess[j].Rpushb("cyclone_data", sbuf.data(), sbuf.size());
                            if (!bph)
                            {
                                LOGE("ThreadSeq={} Push data FAILED!!!Will Stop RealTime Sequencing Analysis!", __FUNCTION__);

                                StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while push data FAILED!!!");
                            }
                        }
                        catch (const std::exception& exp)
                        {
                            LOGE("ThreadSeq={} Push data excep FAILED!={}!Will Stop RealTime Sequencing Analysis!", __FUNCTION__, exp.what());

                            StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while catch an exception!!!");
                        }
#endif
                    }
                }
            }
#else
            ++llFrameNum;
            for (int j = 0; j < m_ciThread; ++j)
            {
                //data frame
                msgpack::sbuffer sbuf;
                msgpack::packer<msgpack::sbuffer> pker(&sbuf);
                pker.pack_map(4);
                //pker.pack("startpoint");
                //pker.pack(ConfigServer::GetCurrentTimestamp());
                pker.pack("data_name");
                pker.pack(m_strSaveName.c_str());
                pker.pack("frame_num");
                pker.pack(llFrameNum);
                pker.pack("is_last_frame");
                pker.pack(0);
                //pker.pack("file_info");
                //pker.pack("O");

                pker.pack(std::string("data_info"));
                pker.pack_array(m_arSelectChan[j]);//m_iSelectChan
                for (int i = j * block, n = j * block + block; i < n; ++i)
                {
                    if (m_vetSelChannel[i])
                    {
                        pker.pack_map(2);
                        pker.pack("chn_num");
                        pker.pack(i + 1);
                        pker.pack("data");
                        //pker.pack_array(1);
                        pker.pack(dtpk->dataHandle[i]);

                        if (!m_arSequencedChannal[i])
                        {
                            m_arSequencedChannal[i] = true;
                            ++m_arSequencedChannel[j];//m_iSequencedChannel
                        }
                    }
#if RTSA_DATA_TEST
                    //if (m_vetSelChannel[i] && m_vetSaveFile2[i] != nullptr)
                    //{
                    //    auto len = fwrite(&dtpk->dataHandle[i][0], sizeof(float), dtpk->dataHandle[i].size(), m_vetSaveFile2[i]);
                    //    m_vetFileLength2[i].first += len;
                    //
                    //    if (m_vetFileLength2[i].first >= m_iSplitSize)//FILEDATA_BUFSIZE
                    //    {
                    //        fclose(m_vetSaveFile2[i]);
                    //        char archanpath[512] = { 0 };
                    //        int ch = i + 1;
                    //        sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath2.c_str(), ch, ch, m_vetFileLength2[i].second++);
                    //        m_vetSaveFile2[i] = fopen(archanpath, "wb");
                    //        m_vetFileLength2[i].first = 0;
                    //    }
                    //}
#endif
                }

                try
                {
                    bool bph = m_redisAccess[j].Rpushb("cyclone_data", sbuf.data(), sbuf.size());
                    if (!bph)
                    {
                        LOGE("ThreadSeq={} Push data FAILED!!!Will Stop RealTime Sequencing Analysis!", __FUNCTION__);

                        StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while push data FAILED!!!");
                    }
                }
                catch (const std::exception& exp)
                {
                    LOGE("ThreadSeq={} Push data excep FAILED!={}!Will Stop RealTime Sequencing Analysis!", __FUNCTION__, exp.what());

                    StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while catch an exception!!!");
                }
            }
#if RTSA_DATA_TEST
            for (int i = 0; i < CHANNEL_NUM; ++i)
            {
                if (m_vetSelChannel[i] && m_vetSaveFile2[i] != nullptr)
                {
                    auto len = fwrite(&dtpk->dataHandle[i][0], sizeof(float), dtpk->dataHandle[i].size(), m_vetSaveFile2[i]);
                    m_vetFileLength2[i].first += len;

                    if (m_vetFileLength2[i].first >= m_iSplitSize)//FILEDATA_BUFSIZE
                    {
                        fclose(m_vetSaveFile2[i]);
                        char archanpath[512] = { 0 };
                        int ch = i + 1;
                        sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath2.c_str(), ch, ch, m_vetFileLength2[i].second++);
                        m_vetSaveFile2[i] = fopen(archanpath, "wb");
                        m_vetFileLength2[i].first = 0;
                    }
                }
            }
#endif

#endif
        }
    }
    LOGI("ThreadExit={}!Left={}={}!={}", __FUNCTION__, m_queDataHandle2.size_approx(), m_vetSequencingData[12].size(), ConfigServer::GetCurrentThreadSelf());
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        m_vetSequencingData[i].clear();
    }
    std::shared_ptr<ConfigServer::SDataHandle> dtclear;
    while (m_queDataHandle2.try_dequeue(dtclear));
}

bool SavePannel::EndThread2(void)
{
    m_bRunning2 = false;
    m_cv2.notify_one();
    return false;
}

bool SavePannel::StopThread2(void)
{
    m_bRunning2 = false;
    m_cv2.notify_one();
    if (m_ThreadPtr2 != nullptr)
    {
        if (m_ThreadPtr2->joinable())
        {
            m_ThreadPtr2->join();
        }
        m_ThreadPtr2.reset();
    }
    return false;
}

bool SavePannel::StartThread3(void)
{
    if (!m_bRunning3 && m_ThreadPtr3 == nullptr)
    {
        m_bRunning3 = true;
        m_ThreadPtr3 = std::make_shared<std::thread>(&SavePannel::ThreadFunc3, this);
        return m_ThreadPtr3 != nullptr;
    }
    return false;
}

void SavePannel::ThreadFunc3(void)
{
#if 1
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    int cntop = 0;
    while (m_bRunning3)
    {
        ConfigServer::InterruptSleep(2000, m_bRunning3);
        if (++cntop > 900)
        {
            cntop = 0;
            ConfigServer::GetRunLog("TimerRunlog", "top -b -o %MEM", 5000);
        }

        char argetswapmem[1024] = { 0 };
        ConfigServer::GetExecuteCMD("free -m", argetswapmem, sizeof(argetswapmem));
        std::vector<std::string> vetswapmem;
        ConfigServer::SplitString2(argetswapmem, " ", vetswapmem);
        int swapmemlen = vetswapmem.size();
        if (swapmemlen > 9)
        {
            auto total = ::atof(vetswapmem[swapmemlen - 3].c_str());//2047
            auto dused = ::atof(vetswapmem[swapmemlen - 2].c_str());//511
            if (dused > (total * 0.60))
            {
                LOGE("ThreadSeq={}!Swap memory in a hurry!={}!Will Stop RealTime Sequencing Analysis!", __FUNCTION__, argetswapmem);

                StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while out of memory!!!");

                EndThread3();
            }
        }
        else
        {
            LOGW("Get memory info FAILED!!!For safety, Please Stop Real Time Sequencing Analysis.");
        }
    }
    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
#endif
}

bool SavePannel::EndThread3(void)
{
    m_bRunning3 = false;
    return false;
}

bool SavePannel::StopThread3(void)
{
    m_bRunning3 = false;
    if (m_ThreadPtr3 != nullptr)
    {
        if (m_ThreadPtr3->joinable())
        {
            m_ThreadPtr3->join();
        }
        m_ThreadPtr3.reset();
    }
    return false;
}

bool SavePannel::StopRealtimeSequencingAnalysis(const QString qmsg, bool stp)
{
    EndThread2();
    ConfigServer::GetInstance()->SetRealtimeSequence(false);
    ConfigServer::GetInstance()->SetAlgorithmServerStatus(false);
    if (m_bRedisReady)
    {
        m_bRedisReady = false;
        //for (int i = 0; i < m_ciThread; ++i)
        //{
        //    m_redisAccess[i].Disconnect();
        //}
    }

    emit stopRealtimeSequencingAnalysisSignal(qmsg);

#ifdef Q_OS_WIN32
#else
    if (stp)
    {
#if 0
        //stop RTSA AlgorithmServer process
        //std::string strAlgorithmServerPath = (ConfigServer::GetHomePath() + "/auto_analysis/scripts/");
        char arcmd[1024] = { 0 };
        //sprintf(arcmd, "cd %s && sudo -u $SUDO_USER bash autonanopore.sh stop", strAlgorithmServerPath.c_str());
        sprintf(arcmd, "sudo killall -v supervisord ; sudo killall -v gunicorn ; sudo killall -v celery ; sudo killall -v python3.7 ; sudo killall -v redis-server");
        system(arcmd);
#endif

        ConfigServer::GetRunLog("StopRTSA", "top -b -o %MEM", 5000, true);
        //std::thread thrd([this]()
        //    {
        //        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //        char arcmd[1024] = { 0 };
        //        sprintf(arcmd, "sudo killall -v supervisord ; sudo killall -v gunicorn ; sudo killall -v celery ; sudo killall -v python3.7 ; sudo killall -v redis-server");
        //        system(arcmd);
        //    });
        //thrd.detach();
    }
#endif

    return false;
}

bool SavePannel::SetTimehms(void)
{
    int itm = m_pispAutoStopTime->value();
    if (itm > 0)
    {
        //int iday = itm / 1440;
        int iiday = itm % 1440;
        int ihour = iiday / 60;
        int imin = iiday % 60;

        m_tmAutoStopTime.setHMS(ihour, imin, 0, 0);

        return true;
    }

    return false;
}

bool SavePannel::UploadRunIdToServer(void)
{
    QJsonObject jsonObject;
    jsonObject.insert("run_id",m_pRecordSettingDialog->GetUniqueRunID());//
    jsonObject.insert("folder_name", m_strSaveName.c_str());//
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);
    std::string strbody = jsonDoc.toJson().toStdString();
    HttpClient httpcl;
    httpcl.SetIngorSSL(true);
    httpcl.GetReqHeader()["Content-Type"] = "application/json";
    auto strUrl = std::string("http");//ConfigServer::GetInstance()->GetUploadAddr();
    {
        if (ConfigServer::GetInstance()->GetWorknet() == 1)
        {
            strUrl = "http://172.16.15.16:9000/filesystem/run_recorder";
        }
        else
        {
            strUrl = "http://192.168.0.74:9000/filesystem/run_recorder";
        }
    }
    bool ret = httpcl.RequestPost(strUrl, strbody);
    if (!ret)
    {
        LOGW("Upload RunId Post FAILED!!!url={}", strUrl.c_str());
		return false;
    }
    size_t retsz = 0;
    auto retstr = httpcl.GetReadString(retsz);
    LOGI("Upload RunId post,status={}",retstr.c_str());
    return true;
}
bool SavePannel::UploadRawDataFolder(const QString& qtaskpath)
{
    std::thread thrd([=]()
        {
            QJsonObject jsonObject;
            jsonObject.insert("data_name", qtaskpath.mid(qtaskpath.lastIndexOf('/') + 1));//
            jsonObject.insert("file_type", "raw_data");//
            jsonObject.insert("src_ip", ConfigServer::GetLocalIPAddress().c_str());//"172.16.15.19"
            jsonObject.insert("src_user", m_strUploadUser.c_str());//"cyclone_public"
            jsonObject.insert("src_passwd", m_strUploadPsw.c_str());//"BGIsms123"
            jsonObject.insert("src_path", qtaskpath);//
            jsonObject.insert("local_path", "/mnt/seqdata/raw_data");///home/cyclone_public/Documents

            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            std::string strbody = jsonDoc.toJson().toStdString();

            HttpClient httpcl;
            httpcl.SetIngorSSL(true);
            httpcl.GetReqHeader()["Content-Type"] = "application/json";
            auto strUrl = std::string("http");//ConfigServer::GetInstance()->GetUploadAddr();
            //if (strUrl.size() <= 0)
            {
                if (ConfigServer::GetInstance()->GetWorknet() == 1)
                {
                    strUrl = "http://172.16.18.2:9101/copy_files";
                }
                else
                {
                    strUrl = "http://192.168.0.106:9101/copy_files";
                }
                //ConfigServer::GetInstance()->SetUploadAddr(strUrl);
            }
            bool ret = httpcl.RequestPost(strUrl, strbody);
            if (!ret)
            {
                LOGW("Upload Request FAILED!!!url={}", strUrl.c_str());
            }
            size_t retsz = 0;
            auto retstr = httpcl.GetReadString(retsz);
            LOGI("Upload Request folder={}, status={}", qtaskpath.toStdString().c_str(), retstr.c_str());
        });
    thrd.detach();

    return false;
}

void SavePannel::OnClickStartAuto(bool chk)
{
    if (chk)
    {
        const int tmermsec = 1000;
        emit startAutoStopTimerSignal(tmermsec);

        m_pbtnStartAuto->setText("StopAuto");
        m_tmerAutoStopTime.start(tmermsec);
    }
    else
    {
        if (m_tmAutoStopTime > QTime(0, 0, 0, 0))
        {
            emit stopAutoStopTimerSignal();
        }

        if (m_tmerAutoStopTime.isActive())
        {
            m_tmerAutoStopTime.stop();
        }
        m_pbtnStartAuto->setText("StartAuto");
    }
}

void SavePannel::OnClickResetAuto(void)
{
    emit resetAutoStopTimerSignal(m_pispAutoStopTime->value());

    SetTimehms();
    m_plcdTimer->display(m_tmAutoStopTime.toString("hh:mm:ss"));
    m_plabTimeUp->setVisible(false);
}

void SavePannel::TimerUpdateTimeSlot(void)
{
    if (m_tmAutoStopTime <= QTime(0, 0, 0, 0))
    {
        m_pbtnStartAuto->toggled(false);
        m_pbtnStartAuto->setChecked(false);

        //stop saving data....
        if (m_pbtnStart->isChecked())
        {
            m_pbtnStart->toggled(false);
            m_pbtnStart->setChecked(false);
        }

        //onClickResetAuto();
        m_plabTimeUp->setVisible(true);
    }
    else
    {
        m_tmAutoStopTime = m_tmAutoStopTime.addSecs(-1);
    }
    m_plcdTimer->display(m_tmAutoStopTime.toString("hh:mm:ss"));
}

void SavePannel::TimerSequencingStatusSlot(void)
{
#if 1
    long long reads = 0;
    long long adapter = 0;
    std::string strreads = m_strSaveName + "_reads_counter";
    std::string stradapter = m_strSaveName + "_adaptors_counter";
    for (int i = 0; i < m_ciThread; ++i)
    {
        std::string sreads = m_redisAccess[i].Get(strreads.c_str());
        reads += ::atoll(sreads.c_str());
        std::string sadapter = m_redisAccess[i].Get(stradapter.c_str());
        adapter += ::atoll(sadapter.c_str());
    }

    m_algorithmReads = reads;
    m_algorithmAdapter = adapter;
    m_peditSequencingStatus->setText(QString("Reads=%1, Adapter=%2").arg(reads).arg(adapter));
    if (!ConfigServer::GetInstance()->GetRealtimeSequence())
    {
        if (m_tmrSequencingStatus.isActive())
        {
            m_tmrSequencingStatus.stop();
        }
    }
#else
    try
    {
        static int siempcnt = 0;
        std::string strValue = m_redisAccess.GetHash("waiting_task", m_strSaveName.c_str());
        if (strValue.empty())
        {
            QString qstr = QString("Sequencing waiting_task Not FOUND in waiting_task!!!Something May FAILED! Please Check!=%1").arg(++siempcnt);
            m_peditSequencingStatus->setText(qstr);
            LOGW("Sequencing waiting_task Not FOUND in waiting_task!!!={}={}", m_strSaveName.c_str(), siempcnt);
            if (siempcnt > 20)
            {
                if (m_tmrSequencingStatus.isActive())
                {
                    m_tmrSequencingStatus.stop();
                }
            }
            return;
        }

        QJsonParseError jsonError;
        QJsonDocument jsonDocument(QJsonDocument::fromJson(strValue.c_str(), &jsonError));

        if (jsonError.error != QJsonParseError::NoError)
        {
            LOGE("Wrong Sequencing waiting_task json content!!!={}={}", m_strSaveName.c_str(), strValue.c_str());
            ++siempcnt;
            if (siempcnt > 20)
            {
                if (m_tmrSequencingStatus.isActive())
                {
                    m_tmrSequencingStatus.stop();
                }
            }
            return;
        }
        siempcnt = 0;

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            auto ite = jsonObject.find("status");
            if (ite != jsonObject.end())
            {
                auto state = ite->toString();
                m_peditSequencingStatus->setText(state);
            }
        }
    }
    catch (const std::exception& exp)
    {
        LOGE("Get RealTime Sequencing Analysis Status FAILED!={}!Will Stop Get Status!", exp.what());
        EndThread2();
        ConfigServer::GetInstance()->SetRealtimeSequence(false);
        if (m_tmrSequencingStatus.isActive())
        {
            m_tmrSequencingStatus.stop();
        }
        m_peditSequencingStatus->setText("Stop Update RealTime Sequencing Analysis Status!");
        m_pRecordSettingDialog->SetNanoporeSequencingCheck(false);
    }
#endif
}

void SavePannel::stopRealtimeSequencingAnalysisSlot(const QString qmsg)
{
    if (m_tmrSequencingStatus.isActive())
    {
        m_tmrSequencingStatus.stop();
    }
    m_peditSequencingStatus->setText(qmsg);
    m_pRecordSettingDialog->SetNanoporeSequencingCheck(false);
}

void SavePannel::SetDataSaveSlot(const bool &val)
{
    if (m_pbtnStart->isChecked())
    {
        m_pbtnStart->click();
    }
    if (val)
    {
        //m_pdlgRecordSetting->UpdateChannelSelected();
        m_pbtnStart->click();
    }
}

void SavePannel::SetDataIsSaveSlot(const std::vector<bool> &vctIsSave)
{
    m_pRecordSettingDialog->SetChannelSelect(vctIsSave);
}

void SavePannel::checkAlgorithmServerStatusCodeSlot(void)
{
    m_bRedisReady = InitTransfer();
    if (m_bRedisReady)
    {
        bool allready = false;
        for (int i = 0; i < m_ciThread; ++i)
        {
            auto ret = m_redisAccess[i].Get("status_code");
            int val = ::atoi(ret.c_str());
            if (val == 200)
            {
                ConfigServer::GetInstance()->SetAlgorithmServerStatus(true);
                m_pbtnStart->setEnabled(true);
                LOGI("Algorithm Server Start immediately SUCCESS.");
                m_redisAccess[i].DelAll("status_code");

                allready = true;
                break;
            }
        }
        if (!allready)
        {
            m_tmrCheckAlgorithmServerState.start(5000);//Every 5s check the algorithm server state until server start well or timeout.
        }
    }
    else
    {
        ConfigServer::GetInstance()->SetAlgorithmServerStatus(false);
        m_pRecordSettingDialog->SetNanoporeSequencingCheck(false);
        m_pbtnStart->setEnabled(true);
        LOGW("Init redis FAILED!!!");
    }
}

void SavePannel::timerCheckAlgorithmServerStateSlot(void)
{
    static int sicount = 0;
    for (int i = 0; i < m_ciThread; ++i)
    {
        auto ret = m_redisAccess[i].Get("status_code");
        int val = ::atoi(ret.c_str());
        if (val == 200)
        {
            ConfigServer::GetInstance()->SetAlgorithmServerStatus(true);
            sicount = 30;
            LOGI("Algorithm Server Start SUCCESS.");
            m_redisAccess[i].DelAll("status_code");
        }
    }

    if (++sicount >= 30)
    {
        if (m_tmrCheckAlgorithmServerState.isActive())
        {
            m_tmrCheckAlgorithmServerState.stop();
        }
        if (!ConfigServer::GetInstance()->GetAlgorithmServerStatus())
        {
            m_pRecordSettingDialog->SetNanoporeSequencingCheck(false);
        }
        m_pbtnStart->setEnabled(true);
        LOGI("Algorithm Server Start Finished.");
    }
}
void SavePannel::GetDataSaveDirIsEmptySlot(bool &empty)
{
    empty = m_peditSavePath->text().isEmpty();
}
void SavePannel::EnableShowSimulateGrpbx(const bool &visible)
{
    m_pSimulateGrpbx->setVisible(visible);
}

void SavePannel::EnableChooseSimulateDir(const bool &enable)
{
    m_peditSimulateSavePath->setEnabled(enable);
    m_pbtnSimulateChooseFolder->setEnabled(enable);
}

bool SavePannel::OpenSimulateFiles(const std::string &simulateRawDir)
{
    #ifdef Q_OS_WIN32
        _setmaxstdio(4096);
    #else
        //system("ulimit -SHn 8192");
    #endif

    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        if (m_vetSimulateFiles[i] == nullptr)
        {
            int ch = i + 1;
            char archanpath[600] = { 0 };
            sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", simulateRawDir.c_str(), ch,ch, 0);
            std::filesystem::path pathchan(archanpath);
            if (!std::filesystem::exists(pathchan))
            {
                LOGE("Channel {} the file name: {} is not exist!!!",ch,archanpath);
                return false;;
            }

            m_vetSimulateFiles[i] = fopen(archanpath, "rb+");
            if (m_vetSimulateFiles[i] == nullptr)
            {
                LOGE("Open file: {} failed!!!", archanpath);
                return false;
            }
        }
    }
    return true;
}

bool SavePannel::CheckSimulateFilesExist(const std::string &loadpath)
{
    if(!loadpath.empty())
    {
        std::string loadpathStr = loadpath;
        std::filesystem::path pathload(loadpathStr);
        if(pathload.filename().string() != "Raw")
        {
            loadpathStr += "/Raw";
            std::filesystem::path subpath(loadpathStr);
            if (!std::filesystem::exists(subpath))
            {
                LOGE("Folder :{} couldn't find", loadpathStr);
                return false;
            }
        }
        m_strSimulateRawPath = loadpathStr;

        for(int i = 0; i< CHANNEL_NUM; ++i)
        {
            int ch = i + 1;
            char archanpath[1024] = { 0 };
            char archanfilepath[1024] = { 0 };
            sprintf(archanpath, "%s/channel%d", loadpathStr.c_str(), ch);
            std::filesystem::path pathchan(archanpath);
            if (!std::filesystem::exists(pathchan))
            {
                LOGE("create_folder: {} failed!!!", archanpath);
                return false;
            }
            sprintf(archanfilepath, "%s/channel%d/channel%d_%d.dat", loadpathStr.c_str(), ch, ch, 0);
            std::filesystem::path pathchanfile(archanfilepath);
            if (!std::filesystem::exists(pathchanfile))
            {
                LOGE("File: {} couldn't find!!!", archanfilepath);
                return false;
            }
        }
        return true;
    }
    return false;
}

bool SavePannel::OpenLoadSimulateFiles(const bool &enabled)
{
    if(enabled)
    {
        if(m_peditSimulateSavePath->text().isEmpty())
        {
           return false;
        }
        if(!CheckSimulateFilesExist(m_peditSimulateSavePath->text().toStdString()))
        {
            return false;
        }

        if(!OpenSimulateFiles(m_strSimulateRawPath))
        {
            CloseSimulateFiles();
            return false;
        }
    }
    else
    {
        CloseSimulateFiles();
    }

    return true;
}

void SavePannel::CloseSimulateFiles(void)
{

    for(auto &simulatefile: m_vetSimulateFiles)
    {
        if(simulatefile !=  nullptr)
        {
            fclose(simulatefile);
            simulatefile = nullptr;
        }
    }
}

void SavePannel::AcquireCurReadsApatersSlot(long long &curReads, long long & curAdapters)
{
#if 0
    m_algorithmReads += 100000;
    m_algorithmAdapter += 1600000;
#endif
    curReads = m_algorithmReads;
    curAdapters = m_algorithmAdapter;
}


