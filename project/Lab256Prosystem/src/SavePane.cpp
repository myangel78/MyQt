#include "SavePane.h"

#include <qlayout.h>
#include <qdatetime.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qprocess.h>

#ifdef Q_OS_WIN32
#include <windows.h>
//#include <shellapi.h>
#endif

#include <filesystem>
#include <Log.h>
#include <msgpack.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "TranslateDialog.h"
#include "PlotReviewDialog.h"
#include "HttpClient.h"


#define PACK_WITH_MSGPACK_NOTJSON


SavePane::SavePane(QWidget *parent) : QWidget(parent)
{
	m_vetSelChannel.resize(CHANNEL_NUM);
	m_vetSaveFile.resize(CHANNEL_NUM);
	m_vetFileLength.resize(CHANNEL_NUM);
	for (int i = 0; i < CHANNEL_NUM; ++i)
	{
		m_vetSelChannel[i] = false;
		m_vetSaveFile[i] = nullptr;
		m_vetFileLength[i].first = 0;
		m_vetFileLength[i].second = 0;
	}
	for (int i = 0; i < 50; ++i)
	{
		m_arMockChannelData[i] = 10.0;
	}

	InitCtrl();
	//std::thread thrd([this]()
	//	{
	//		m_bRedisReady = InitTransfer();
	//		if (m_bRedisReady)
	//		{
	//			m_redisAccess.DelAll("status_code");
	//			m_redisAccess.Disconnect();
	//		}
	//	});
	//thrd.detach();

#ifdef Q_OS_WIN32
	std::string strdatapath = ConfigServer::GetCurrentPath() + "/etc/Data";
#else
	std::string strdatapath = "/data/raw_data";
	//std::thread thrchown([&]()
	//	{
	//		char arcmd[1024] = { 0 };
	//		sprintf(arcmd, "sudo chown -R $SUDO_USER:$SUDO_USER %s", strdatapath.c_str());
	//		system(arcmd);
	//		//ConfigServer::MeSleep(10000);
	//	});
	//thrchown.join();
#endif
	std::filesystem::path pathdatapath(strdatapath);
	if (!std::filesystem::exists(pathdatapath))
	{
		std::filesystem::create_directories(pathdatapath);
	}
	m_peditSavePath->setText(strdatapath.c_str());
	m_pdlgRecordSetting->SetSavePath(strdatapath);


	connect(this, SIGNAL(stopRealtimeSequencingAnalysisSignal(const QString)), this, SLOT(stopRealtimeSequencingAnalysisSlot(const QString)));
	connect(m_pdlgRecordSetting, SIGNAL(onSequencingIdChangedSignal(int)), this, SIGNAL(onSequencingIdChangedSignal(int)));
	connect(m_pdlgRecordSetting, SIGNAL(onEnableStartSaveDataSignal(bool)), this, SLOT(onEnableStartSaveDataSlot(bool)));
	connect(m_pdlgRecordSetting, SIGNAL(checkAlgorithmServerStatusCodeSignal(void)), this, SLOT(checkAlgorithmServerStatusCodeSlot(void)));
	connect(&m_tmrCheckAlgorithmServerState, &QTimer::timeout, this, &SavePane::timerCheckAlgorithmServerStateSlot);
}

SavePane::~SavePane()
{
	EndThread3();
	EndThread2();
	if (m_tmrSequencingStatus.isActive())
	{
		m_tmrSequencingStatus.stop();
	}
	if (m_tmrAutoStopTime.isActive())
	{
		m_tmrAutoStopTime.stop();
	}
	if (m_tmrCheckAlgorithmServerState.isActive())
	{
		m_tmrCheckAlgorithmServerState.stop();
	}
	StopThread2();
	StopThread3();
}

bool SavePane::InitCtrl(void)
{
	//saving data
	m_pgrpSavingData = new QGroupBox("SavingData", this);
	//m_pgrpSavingData->setContentsMargins(0, 5, 0, 3);

	m_pbtnConfig = new QPushButton("Config", m_pgrpSavingData);
	m_pbtnConfig->setIcon(QIcon(":/img/img/eject.png"));
	m_pbtnConfig->setIconSize(QSize(30, 30));

	//m_pbtnChannalSel = new QPushButton("ChannelSel", this);
	//m_pbtnChannalSel->setIcon(QIcon("res/version.png"));
	//m_pbtnChannalSel->setIconSize(QSize(30, 30));
	//connect(m_pbtnChannalSel, &QPushButton::clicked, this, &SavePane::onClickChannelSel);

	m_pbtnStart = new QPushButton("Start", m_pgrpSavingData);
	m_pbtnStart->setIcon(QIcon(":/img/img/run.png"));
	m_pbtnStart->setIconSize(QSize(30, 30));
	m_pbtnStart->setCheckable(true);

	m_pispFileSize = new QSpinBox(m_pgrpSavingData);
	m_pispFileSize->setMinimum(1);
	m_pispFileSize->setMaximum(128);
	m_pispFileSize->setValue(128);
	m_pispFileSize->setSuffix(" M");

	m_pbtnUpload = new QPushButton("Upload", m_pgrpSavingData);
	m_pbtnUpload->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>选择要上传的目录，现在为自动上传原始数据,如果自动上传失败请手动上传，上传进度可以上172.16.18.2:9097页面查看，task记录消失表示文件传输完成。文件传输完成前请不要关闭电脑</p><p>注：Windows系统暂不可用</p></body></html>"));
	//m_pbtnUpload->setVisible(false);

	//m_plabSequencingStatus = new QLabel("Realtime Sequencing Analysis Status...", m_pgrpSavingData);
	////m_plabSequencingStatus->setMinimumWidth(80);
	//m_plabSequencingStatus->setFrameShape(QFrame::Box);

	m_peditSequencingStatus = new QLineEdit("Realtime Sequencing Analysis Status...", m_pgrpSavingData);
	m_peditSequencingStatus->setReadOnly(true);

	m_pbtnTranslate = new QPushButton("Translate", m_pgrpSavingData);
	m_pbtnReplot = new QPushButton("Plot Review", m_pgrpSavingData);


	m_peditSavePath = new QLineEdit(m_pgrpSavingData);
	m_peditSavePath->setReadOnly(true);

	m_pbtnChooseFolder = new QPushButton("ChooseFloder", m_pgrpSavingData);
	m_pbtnOpenFolder = new QPushButton("OpenFolder", m_pgrpSavingData);


	m_pbtnUpload->setEnabled(false);
#ifdef Q_OS_WIN32
#else
	m_pbtnChooseFolder->setEnabled(false);
	m_pbtnOpenFolder->setEnabled(false);
#endif


	connect(m_pbtnConfig, &QPushButton::clicked, this, &SavePane::OnClickConfig);
	connect(m_pbtnStart, &QPushButton::toggled, this, &SavePane::OnClickStart);
	connect(m_pbtnUpload, &QPushButton::clicked, this, &SavePane::OnClickChooseUploadFolder);

	connect(&m_tmrSequencingStatus, &QTimer::timeout, this, &SavePane::TimerSequencingStatusSlot);

	connect(m_pbtnTranslate, &QPushButton::clicked, this, &SavePane::OnClickTranslate);
	connect(m_pbtnReplot, &QPushButton::clicked, this, &SavePane::OnClickReplot);

	connect(m_pbtnChooseFolder, &QPushButton::clicked, this, &SavePane::OnClickChooseFolder);
	connect(m_pbtnOpenFolder, &QPushButton::clicked, this, &SavePane::OnClickOpenFolder);


	QHBoxLayout* horlayout1 = new QHBoxLayout();
	horlayout1->addWidget(m_pbtnConfig);
	//horlayout1->addWidget(m_pbtnChannalSel);
	horlayout1->addWidget(m_pbtnStart);
	horlayout1->addWidget(m_pispFileSize);
	horlayout1->addWidget(m_pbtnUpload);
	//horlayout1->addWidget(m_plabSequencingStatus);
	horlayout1->addWidget(m_peditSequencingStatus);
	//horlayout1->addStretch(1);
	horlayout1->addWidget(m_pbtnTranslate);
	horlayout1->addWidget(m_pbtnReplot);

	QHBoxLayout* horlayout2 = new QHBoxLayout();
	horlayout2->addWidget(m_peditSavePath);
	horlayout2->addWidget(m_pbtnChooseFolder);
	horlayout2->addWidget(m_pbtnOpenFolder);

	QVBoxLayout* verlayout1 = new QVBoxLayout();
	verlayout1->addLayout(horlayout1);
	verlayout1->addLayout(horlayout2);

	m_pgrpSavingData->setLayout(verlayout1);


	//Auto stop saving
	m_pgrpAutoStopAndSave = new QGroupBox("AutoStopAndSave", this);
	//m_pgrpAutoStopAndSave->setContentsMargins(0, 5, 0, 3);

	m_plabAutoStopTime = new QLabel("<font color=red>AutoStop</font>", m_pgrpAutoStopAndSave);
	m_pispAutoStopTime = new QSpinBox(m_pgrpAutoStopAndSave);
	m_pispAutoStopTime->setSuffix(" min");
	m_pispAutoStopTime->setMinimum(1);
	m_pispAutoStopTime->setMaximum(1439);//23:39:00
	m_pispAutoStopTime->setSingleStep(10);
	m_pispAutoStopTime->setValue(90);

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

	connect(m_pbtnStartAuto, &QPushButton::toggled, this, &SavePane::OnClickStartAuto);
	connect(m_pbtnResetAuto, &QPushButton::clicked, this, &SavePane::OnClickResetAuto);

	connect(&m_tmrAutoStopTime, &QTimer::timeout, this, &SavePane::TimerUpdateTimeSlot);


	QHBoxLayout* horlayout5 = new QHBoxLayout();
	horlayout5->addWidget(m_plabAutoStopTime);
	horlayout5->addWidget(m_pispAutoStopTime);
	horlayout5->addWidget(m_plcdTimer);
	horlayout5->addWidget(m_plabTimeUp);
	horlayout5->addStretch(1);
	horlayout5->addWidget(m_pbtnStartAuto);
	horlayout5->addWidget(m_pbtnResetAuto);

	m_pgrpAutoStopAndSave->setLayout(horlayout5);


	//
	QVBoxLayout* verlayout9 = new QVBoxLayout();
	verlayout9->setContentsMargins(5, 0, 5, 3);
	verlayout9->addWidget(m_pgrpSavingData);
	verlayout9->addWidget(m_pgrpAutoStopAndSave);

	setLayout(verlayout9);


	m_pdlgRecordSetting = new RecordSettingDialog(this);
	//m_pdlgRecordSelect = new RecordSelectDialog(this);

	//m_strUploadUser = ConfigServer::GetInstance()->GetLoginUserName();

	return false;
}

bool SavePane::InitTransfer(void)
{
	bool bret = false;
	int iport = 6379;
	int idbnum = 0;
	std::string strhost = ConfigServer::GetInstance()->GetTaskRedisParam(iport, idbnum);
	//if (strhost.size() <= 0)
	{
		strhost = "127.0.0.1";//"172.16.16.44";
		iport = 9099;//9096;
		idbnum = 2;//6;

		ConfigServer::GetInstance()->SetTaskRedisParam(strhost, iport, idbnum);
	}

	bret = m_redisAccess.SetAccessParam(strhost.c_str(), iport, nullptr);
	if (!bret)
	{
		LOGE("Redis set param failed!!!");
		return false;
	}
	bret = m_redisAccess.Connecting();
	if (!bret)
	{
		LOGE("Connect redis failed!!!");
		return false;
	}
	bret = m_redisAccess.selectDB(idbnum);
	if (!bret)
	{
		LOGE("Select redis db failed!!!");
		return false;
	}

	return true;
}

bool SavePane::OpenChannelsFile(void)
{
	m_iSelectChan = 0;
	m_iSplitSize = m_pispFileSize->value();
	m_iSplitSize = (m_iSplitSize << 18);// (isplit << 20) / 4;
	auto& chansel = m_pdlgRecordSetting->GetChannelSelect();
	for (int i = 0; i < CHANNEL_NUM; ++i)
	{
		bool sel = chansel[i];
		m_vetSelChannel[i] = sel;
		if (sel && m_vetSaveFile[i] == nullptr)
		{
			int ch = i + 1;
			char archanpath[512] = { 0 };
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
			sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch, ch, m_vetFileLength[i].second++);
			m_vetSaveFile[i] = fopen(archanpath, "wb");
			if (m_vetSaveFile[i] == nullptr)
			{
				LOGE("Open file: {} failed!!!", archanpath);
			}
			++m_iSelectChan;
		}
	}
	if (m_iSelectChan <= 0)
	{
		LOGW("Save Data no channel select!!!");
		QMessageBox::warning(this, "Save No Data", "Save Data no channel select!!!");
	}

	return true;
}

bool SavePane::CloseChannelsFile(void)
{
	for (int i = 0; i < CHANNEL_NUM; ++i)
	{
		if (/*m_vetSelChannel[i] &&*/ m_vetSaveFile[i] != nullptr)
		{
			fclose(m_vetSaveFile[i]);
			m_vetSaveFile[i] = nullptr;
		}

		m_vetFileLength[i].first = 0;
		m_vetFileLength[i].second = 0;
	}
	//std::shared_ptr<SDataHandle> dtclear;
	//while (m_queDataHandle.try_dequeue(dtclear));

	return false;
}

bool SavePane::SetTimehms(void)
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

void SavePane::SetControlView(ControlView* pview)
{
	m_pdlgRecordSetting->SetControlView(pview);
	m_pdlgRecordSetting->UpdateChannelSelected();
}

bool SavePane::LoadConfig(QSettings* pset)
{
	if (m_pdlgRecordSetting != nullptr)
	{
		m_pdlgRecordSetting->LoadConfig(pset);
		return true;
	}
	return false;
}

bool SavePane::SaveConfig(QSettings* pset)
{
	if (m_pdlgRecordSetting != nullptr)
	{
		m_pdlgRecordSetting->SaveConfig(pset);
		return true;
	}
	return false;
}

bool SavePane::UpdateFolderOwner(const std::string& taskpath)
{
#ifdef Q_OS_WIN32
	return taskpath.empty();
#else
	char arcmd[1024] = { 0 };
	//sprintf(arcmd, "sudo chown -R shanzhu:shanzhu %s", m_strTaskPath.c_str());
	sprintf(arcmd, "sudo chown -R $SUDO_USER:$SUDO_USER %s", taskpath.c_str());
	system(arcmd);
#endif
	return false;
}

bool SavePane::UploadRawDataFolder(const QString& qtaskpath)
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
			auto strUrl = ConfigServer::GetInstance()->GetUploadAddr();
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
				ConfigServer::GetInstance()->SetUploadAddr(strUrl);
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

bool SavePane::StartThread(void)
{
	if (!m_bRunning && m_ThreadPtr == nullptr)
	{
		m_bRunning = true;
		m_ThreadPtr = std::make_shared<std::thread>(&SavePane::ThreadFunc, this);
		return m_ThreadPtr != nullptr;
	}
	return false;
}

void SavePane::ThreadFunc(void)
{
	LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());//__PRETTY_FUNCTION__

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

					if (m_vetFileLength[i].first >= m_iSplitSize)//FILEDATA_BUFSIZE
					{
						fclose(m_vetSaveFile[i]);
						char archanpath[512] = { 0 };
						int ch = i + 1;
						sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch, ch, m_vetFileLength[i].second++);
						m_vetSaveFile[i] = fopen(archanpath, "wb");
						m_vetFileLength[i].first = 0;
					}
				}
			}
		}
	}
	LOGI("ThreadExit={}!Left={}!={}", __FUNCTION__, m_queDataHandle.size_approx(), ConfigServer::GetCurrentThreadSelf());

	std::shared_ptr<ConfigServer::SDataHandle> dtclear;
	while (m_queDataHandle.try_dequeue(dtclear));
}

bool SavePane::EndThread(void)
{
	m_bRunning = false;
	m_cv.notify_one();
	return false;
}

bool SavePane::StopThread(void)
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

bool SavePane::StartThread2(void)
{
	if (!m_bRunning2 && m_ThreadPtr2 == nullptr)
	{
		m_bRunning2 = true;
		m_ThreadPtr2 = std::make_shared<std::thread>(&SavePane::ThreadFunc2Seq, this);
		return m_ThreadPtr2 != nullptr;
	}
	return false;
}

void SavePane::ThreadFunc2Seq(void)
{
	LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

	long long llFrameNum = 0;
	while (m_bRunning2)
	{
		{
			std::unique_lock<std::mutex> lck(m_mutexTask2);
			m_cv2.wait(lck, [&] {return !m_bRunning2 || m_queDataHandle2.size_approx() > 0; });
		}
		//int quepacksz = m_queDataHandle2.size_approx();
		std::shared_ptr<ConfigServer::SDataHandle> dtpk;
		bool bret = m_queDataHandle2.try_dequeue(dtpk);
		if (bret)
		{
#if 0
			bool isreach = false;
			for (int i = 0; i < CHANNEL_NUM; ++i)
			{
				if (m_vetSelChannel[i])
				{
					m_vetSequencingData[i].insert(m_vetSequencingData[i].end(), dtpk->dataHandle[i].begin(), dtpk->dataHandle[i].end());
					if (m_vetSequencingData[i].size() >= 10000)
					{
						isreach = true;
					}

					if (!m_arSequencedChannal[i])
					{
						m_arSequencedChannal[i] = true;
						++m_iSequencedChannel;
					}
				}
			}
			if (isreach)
			{
				msgpack::sbuffer sbuf;
				msgpack::packer<msgpack::sbuffer> pker(&sbuf);
				pker.pack_map(4);
				//pker.pack("startpoint");
				//pker.pack(ConfigServer::GetCurrentTimestamp());
				pker.pack("data_name");
				pker.pack(m_strSaveName.c_str());
				pker.pack("frame_num");
				pker.pack(++llFrameNum);
				pker.pack("is_last_frame");
				pker.pack(0);
				//pker.pack("file_info");
				//pker.pack("O");

				pker.pack(std::string("data_info"));
				pker.pack_array(m_iSelectChan);
				for (int i = 0; i < CHANNEL_NUM; ++i)
				{
					if (m_vetSelChannel[i])
					{
						pker.pack_map(2);
						pker.pack("chn_num");
						pker.pack(i + 1);
						pker.pack("data");
						//pker.pack_array(1);
						pker.pack(m_vetSequencingData[i]);
						m_vetSequencingData[i].clear();

						if (!m_arSequencedChannal[i])
						{
							m_arSequencedChannal[i] = true;
							++m_iSequencedChannel;
						}
					}
				}

				try
				{
					bool bph = m_redisAccess.Rpushb("cyclone_data", sbuf.data(), sbuf.size());
					if (!bph)
					{
						LOGE("ThreadSeq={} Push data FAILED!!!Will Stop RealTime Sequencing Analysis!", __FUNCTION__);

						StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while push data FAILED1!!!");
						//EndThread2();
						//ConfigServer::GetInstance()->SetRealtimeSequence(false);
						//
						//emit stopRealtimeSequencingAnalysisSignal("Stop RealTime Sequencing Analysis while push data FAILED1!!!");
					}
				}
				catch (const std::exception& exp)
				{
					LOGE("ThreadSeq10000={} Push data excep FAILED!={}!Will Stop RealTime Sequencing Analysis!", __FUNCTION__, exp.what());

					StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while catch an exception2!!!");
					//EndThread2();
					//ConfigServer::GetInstance()->SetRealtimeSequence(false);
					//
					//emit stopRealtimeSequencingAnalysisSignal("Stop RealTime Sequencing Analysis while catch an exception2!!!");
				}
			}
#else

#ifdef PACK_WITH_MSGPACK_NOTJSON

			//data frame
			msgpack::sbuffer sbuf;
			msgpack::packer<msgpack::sbuffer> pker(&sbuf);
			pker.pack_map(4);
			//pker.pack("startpoint");
			//pker.pack(ConfigServer::GetCurrentTimestamp());
			pker.pack("data_name");
			pker.pack(m_strSaveName.c_str());
			pker.pack("frame_num");
			pker.pack(++llFrameNum);
			pker.pack("is_last_frame");
			pker.pack(0);
			//pker.pack("file_info");
			//pker.pack("O");

			pker.pack(std::string("data_info"));
			pker.pack_array(m_iSelectChan);
			for (int i = 0; i < CHANNEL_NUM; ++i)
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
						++m_iSequencedChannel;
					}
				}
			}

			try
			{
				bool bph = m_redisAccess.Rpushb("cyclone_data", sbuf.data(), sbuf.size());
				if (!bph)
				{
					LOGE("ThreadSeq={} Push data FAILED!!!Will Stop RealTime Sequencing Analysis!", __FUNCTION__);

					StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while push data FAILED!!!");
					//EndThread2();
					//ConfigServer::GetInstance()->SetRealtimeSequence(false);
					//
					//emit stopRealtimeSequencingAnalysisSignal("Stop RealTime Sequencing Analysis while push data FAILED!!!");
				}
			}
			catch (const std::exception& exp)
			{
				LOGE("ThreadSeq={} Push data excep FAILED!={}!Will Stop RealTime Sequencing Analysis!", __FUNCTION__, exp.what());

				StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while catch an exception!!!");
				//EndThread2();
				//ConfigServer::GetInstance()->SetRealtimeSequence(false);
				//
				//emit stopRealtimeSequencingAnalysisSignal("Stop RealTime Sequencing Analysis while catch an exception!!!");
			}

#else

#if 1
			//data frame
			rapidjson::StringBuffer strBuf;
			rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
			writer.StartObject();

			writer.Key("task_params");
			writer.String(m_strSaveName.c_str(), m_strSaveName.size());

			writer.Key("data_info");
			writer.StartArray();
			for (int i = 0; i < CHANNEL_NUM; ++i)
			{
				if (m_vetSelChannel[i])
				{
					writer.StartObject();
					writer.Key("chn_num");
					writer.Int(i + 1);
					writer.Key("data");
					writer.StartArray();
					for (int j = 0, n = dtpk->dataHandle[i].size(); j < n; ++j)
					{
						writer.Double(dtpk->dataHandle[i][j]);
					}
					writer.EndArray();
					writer.EndObject();
				}
			}
			writer.EndArray();

			writer.Key("frame_num");
			writer.Int64(++llFrameNum);
			writer.Key("is_last_frame");
			writer.Int(0);
			writer.Key("file_info");
			writer.String("O", 1);
			writer.Key("curtime");
			writer.Uint64(GetTickCount64());

			writer.EndObject();

			m_redisAccess.Rpush((m_strSaveName + "_orig_data").c_str(), strBuf.GetString());

#else
			//data frame
			QJsonObject jsonObjDataFrame;
			jsonObjDataFrame.insert("task_params", m_strSaveName.c_str());//
			QJsonArray jsonArray;

			for (int i = 0; i < CHANNEL_NUM; ++i)
			{
				if (m_vetSelChannel[i])
				{
					QJsonObject jsonObjChanFrame;
					jsonObjChanFrame.insert("chn_num", i + 1);//

					QJsonArray jsonArrayChan;
					for (int j = 0, n = dtpk->dataHandle[i].size(); j < n; ++j)
					{
						jsonArrayChan.append(dtpk->dataHandle[i][j]);//
					}
					jsonObjChanFrame.insert("data", jsonArrayChan);

					jsonArray.append(jsonObjChanFrame);
				}
			}

			jsonObjDataFrame.insert("data_info", jsonArray);//
			jsonObjDataFrame.insert("frame_num", ++llFrameNum);//
			jsonObjDataFrame.insert("is_last_frame", 0);//
			jsonObjDataFrame.insert("file_info", "O");//

			//jsonObjDataFrame.insert("curtime", (long long)GetTickCount64());

			QJsonDocument jsonDocDataFrame;
			jsonDocDataFrame.setObject(jsonObjDataFrame);
			std::string strDatavalue = jsonDocDataFrame.toJson().toStdString();
			std::string strDatakey = m_strSaveName + "_orig_data";
			m_redisAccess.Rpush(strDatakey.c_str(), strDatavalue.c_str());
#endif
#endif

#endif
		}
	}
	LOGI("ThreadExit={}!Left={}!={}", __FUNCTION__, m_queDataHandle2.size_approx(), ConfigServer::GetCurrentThreadSelf());

	std::shared_ptr<ConfigServer::SDataHandle> dtclear;
	while (m_queDataHandle2.try_dequeue(dtclear));
}

bool SavePane::EndThread2(void)
{
	m_bRunning2 = false;
	m_cv2.notify_one();
	return false;
}

bool SavePane::StopThread2(void)
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

bool SavePane::StartThread3(void)
{
	if (!m_bRunning3 && m_ThreadPtr3 == nullptr)
	{
		m_bRunning3 = true;
		m_ThreadPtr3 = std::make_shared<std::thread>(&SavePane::ThreadFunc3, this);
		return m_ThreadPtr3 != nullptr;
	}
	return false;
}

void SavePane::ThreadFunc3(void)
{
	LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

	while (m_bRunning3)
	{
		ConfigServer::MeSleep(2000);

		char argetswapmem[1024] = { 0 };
		ConfigServer::GetExecuteCMD("free -m", argetswapmem, sizeof(argetswapmem));
		std::vector<std::string> vetswapmem;
		ConfigServer::SplitString2(argetswapmem, " ", vetswapmem);
		int swapmemlen = vetswapmem.size();
		if (swapmemlen > 9)
		{
			auto total = ::atof(vetswapmem[swapmemlen - 3].c_str());//2047
			auto dused = ::atof(vetswapmem[swapmemlen - 2].c_str());//511
			if (dused > (total * 0.75))
			{
				LOGE("ThreadSeq={}!Swap memory in a hurry!={}!Will Stop RealTime Sequencing Analysis!", __FUNCTION__, argetswapmem);

				StopRealtimeSequencingAnalysis("Stop RealTime Sequencing Analysis while out of memory!!!");
				//EndThread2();
				//ConfigServer::GetInstance()->SetRealtimeSequence(false);
				//
				//emit stopRealtimeSequencingAnalysisSignal("Stop RealTime Sequencing Analysis while out of memory!!!");
				//
				//std::string strAlgorithmServerPath = (ConfigServer::GetHomePath() + "/auto_analysis/scripts/");
				//char arcmd[1024] = { 0 };
				//sprintf(arcmd, "cd %s && sudo -u $SUDO_USER bash autonanopore.sh stop", strAlgorithmServerPath.c_str());
				//system(arcmd);

				EndThread3();
			}
		}
		else
		{
			LOGW("Get memory info FAILED!!!For safety, Please Stop Real Time Sequencing Analysis.");
		}
	}
	LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}

bool SavePane::EndThread3(void)
{
	m_bRunning3 = false;
	//m_cv3.notify_one();
	return false;
}

bool SavePane::StopThread3(void)
{
	m_bRunning3 = false;
	//m_cv3.notify_one();
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

bool SavePane::StopRealtimeSequencingAnalysis(const QString qmsg, bool stp)
{
	EndThread2();
	ConfigServer::GetInstance()->SetRealtimeSequence(false);

	emit stopRealtimeSequencingAnalysisSignal(qmsg);

	if (stp)
	{
		//stop RTSA AlgorithmServer process
		std::string strAlgorithmServerPath = (ConfigServer::GetHomePath() + "/auto_analysis/scripts/");
		char arcmd[1024] = { 0 };
		sprintf(arcmd, "cd %s && sudo -u $SUDO_USER bash autonanopore.sh stop", strAlgorithmServerPath.c_str());
		system(arcmd);
	}

	return false;
}

void SavePane::OnClickConfig(void)
{
	m_pdlgRecordSetting->UpdateChannelSelected();
	m_pdlgRecordSetting->show();
	ConfigServer::WidgetPosAdjustByParent(this->parentWidget()->parentWidget(), m_pdlgRecordSetting);
}

//void SavePane::onClickChannelSel(void)
//{
//	//m_pdlgRecordSelect->UpdateChannelSelected();
//	//m_pdlgRecordSelect->show();
//}

void SavePane::OnClickStart(bool chk)
{
#ifdef Q_OS_WIN32
#else
	m_pdlgRecordSetting->EnableNanoporeSequencingCheck(!chk);
#endif

	if (chk)
	{
		m_strRawSavePath = m_pdlgRecordSetting->CreateDataPath();
		if (!m_strRawSavePath.empty())
		{
			std::filesystem::path pathSave(m_strRawSavePath);
			pathSave = pathSave.parent_path();
			m_strTaskPath = pathSave.string();
			m_strSaveName = pathSave.filename().string();

			emit setDegatingDataNameSignal(QString::fromStdString(m_strSaveName));

			m_pdlgRecordSetting->UpdateChannelSelected();
			OpenChannelsFile();
			StartThread();
			ConfigServer::GetInstance()->SetSaveData(true);
			emit startStopRecordVoltSignal(1, m_strTaskPath.c_str());

			if (m_pdlgRecordSetting->GetNanoporeSequencingCheck())
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
					LOGI("Now Will Start Realtime Sequencing Analysis!");
					//QMessageBox::StandardButton msg = QMessageBox::Yes;
					//msg = QMessageBox::question(this, "Realtime Sequencing Analysis", "Do you want to start Realtime Sequencing Analysis?");
					//if (msg == QMessageBox::Yes)
					{
						//register task
						std::string strkey("waiting_task");
						std::string strfield = m_strSaveName;
						std::string strvalue;
						m_pdlgRecordSetting->Json4RegisterTask(strvalue);
						m_redisAccess.SetHash(strkey.c_str(), strfield.c_str(), strvalue.c_str());

						//head frame
						std::string strHeadkey = "cyclone_data";// strfield + "_orig_data";
						std::string strcontent = m_pdlgRecordSetting->GetExperimentConfigs();

#ifdef PACK_WITH_MSGPACK_NOTJSON

						msgpack::sbuffer sbuf;
						msgpack::packer<msgpack::sbuffer> pker(&sbuf);
						pker.pack_map(3);
						pker.pack("data_name");
						pker.pack(strfield.c_str());
						pker.pack("position");
						pker.pack("H");
						pker.pack("content");
						pker.pack(strcontent.c_str());

						m_redisAccess.Rpushb(strHeadkey.c_str(), sbuf.data(), sbuf.size());

#else
						QJsonObject jsonObject;
						jsonObject.insert("params", strfield.c_str());//
						jsonObject.insert("position", "H");//
						jsonObject.insert("content", strcontent.c_str());//

						QJsonDocument jsonDoc;
						jsonDoc.setObject(jsonObject);
						std::string strHeadvalue = jsonDoc.toJson().toStdString();
						m_redisAccess.Rpush(strHeadkey.c_str(), strHeadvalue.c_str());
#endif

						StartThread2();
						ConfigServer::GetInstance()->SetRealtimeSequence(true);
						m_tmrSequencingStatus.start(1000);

#ifdef Q_OS_WIN32
#else
						StartThread3();
#endif
					}
				}
			}

			m_pbtnStart->setIcon(QIcon(":/img/img/stop.png"));
			m_pbtnStart->setText("Stop");

			return;
		}
		m_pbtnStart->setChecked(false);
		m_pbtnStart->setIcon(QIcon(":/img/img/run.png"));
		m_pbtnStart->setText("Start");
		QMessageBox::critical(this, "Error", "Creat save Directory falied!!!");
	}
	else
	{
		EndThread();
		ConfigServer::GetInstance()->SetSaveData(false);

		m_pdlgRecordSetting->UpdateEndtime4Json();
		LOGI("Endsavedata....");

		emit startStopRecordVoltSignal(0, m_strTaskPath.c_str());

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
			
#ifdef PACK_WITH_MSGPACK_NOTJSON

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
			pker.pack_array(m_iSequencedChannel);
			for (int i = 0; i < CHANNEL_NUM; ++i)
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

			m_redisAccess.Rpushb(strHeadkey.c_str(), sbuf.data(), sbuf.size());

			//tail frame
			std::string strcontent = m_pdlgRecordSetting->GetExperimentConfigs();
			msgpack::sbuffer sbuf2;
			msgpack::packer<msgpack::sbuffer> pker2(&sbuf2);
			pker2.pack_map(3);
			pker2.pack("data_name");
			pker2.pack(strfield.c_str());
			pker2.pack("position");
			pker2.pack("T");
			pker2.pack("content");
			pker2.pack(strcontent.c_str());

			m_redisAccess.Rpushb(strHeadkey.c_str(), sbuf2.data(), sbuf2.size());

#else
			//last frame
			//msgpack::pack(sbuf, std::string("data_info"));
			//pker.pack_array(0);

			QJsonObject jsonObjLastFrame;
			jsonObjLastFrame.insert("task_params", strfield.c_str());//
			QJsonArray jsonArray;
			jsonObjLastFrame.insert("data_info", jsonArray);//
			jsonObjLastFrame.insert("frame_num", 0);//
			jsonObjLastFrame.insert("is_last_frame", 1);//
			jsonObjLastFrame.insert("file_info", "O");//

			QJsonDocument jsonDocLastFrame;
			jsonDocLastFrame.setObject(jsonObjLastFrame);
			std::string strHeadvalue = jsonDocLastFrame.toJson().toStdString();
			m_redisAccess.Rpush(strHeadkey.c_str(), strHeadvalue.c_str());

			//tail frame
			std::string strcontent = m_pdlgRecordSetting->GetExperimentConfigs();
			QJsonObject jsonObject;
			jsonObject.insert("data_name", strfield.c_str());//
			jsonObject.insert("position", "T");//
			jsonObject.insert("content", strcontent.c_str());//

			QJsonDocument jsonDoc;
			jsonDoc.setObject(jsonObject);
			strHeadvalue = jsonDoc.toJson().toStdString();
			m_redisAccess.Rpush(strHeadkey.c_str(), strHeadvalue.c_str());
#endif

			m_strSaveName = "nodata";
			m_iSequencedChannel = 0;
		}

		StopThread();
		CloseChannelsFile();

#ifdef Q_OS_WIN32
#else
		//UpdateFolderOwner(m_strTaskPath);
		UploadRawDataFolder(QString::fromStdString(m_strTaskPath));
#endif

		m_pbtnStart->setIcon(QIcon(":/img/img/run.png"));
		m_pbtnStart->setText("Start");
		LOGI("Endsavedata2....");
	}
}

void SavePane::OnClickChooseUploadFolder(void)
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Upload Directory"),
		"/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
	{
		//TODO:Upload dir to server
#if 0
		QJsonObject jsonObject;
		jsonObject.insert("data_name", "20210416115457_LAB256V2_5K_LeiLeyan");//
		jsonObject.insert("file_type", "raw_data");//
		jsonObject.insert("src_ip", "172.16.15.19");//
		jsonObject.insert("src_user", "cyclone_public");//
		jsonObject.insert("src_passwd", "BGIsms123");//
		jsonObject.insert("src_path", "/home/cyclone_public/Documents/20210416115457_LAB256V2_5K_LeiLeyan");//
		jsonObject.insert("local_path", "/home/cyclone_public/Documents");//

		QJsonDocument jsonDoc;
		jsonDoc.setObject(jsonObject);
		std::string strData = jsonDoc.toJson().toStdString();

		HttpClient httpcl;
		httpcl.SetIngorSSL(true);
		httpcl.GetReqHeader()["Content-Type"] = "application/json";
		bool ret = httpcl.RequestPost("http://172.16.18.2:9101/copy_files", strData.c_str());
		size_t retsz = 0;
		auto retstr = httpcl.GetReadString(retsz);
		if (ret)
		{
			//
		}

#else

		if (m_strUploadUser.empty() || m_strUploadPsw.empty())
		{
			bool ok = false;
			QString text = QInputDialog::getText(this, QString::fromLocal8Bit("Input User Name"), QString::fromLocal8Bit("Please Enter the User Name."), QLineEdit::Normal, 0, &ok);
			if (ok)
			{
				m_strUploadUser = text.toStdString();

				text = QInputDialog::getText(this, QString::fromLocal8Bit("Input %1's Password").arg(m_strUploadUser.c_str()), QString::fromLocal8Bit("Please Enter %1's Password.").arg(m_strUploadUser.c_str()), QLineEdit::Password, 0, &ok);
				if (ok)
				{
					m_strUploadPsw = text.toStdString();
				}
				else
				{
					return;
				}
			}
			else
			{
				return;
			}
		}

		UploadRawDataFolder(dir);

#endif
	}
}

void SavePane::TimerSequencingStatusSlot(void)
{
	try
	{
		static int siempcnt = 0;
		std::string strValue = m_redisAccess.GetHash("waiting_task", m_strSaveName.c_str());
		if (strValue.empty())
		{
			LOGW("Sequencing waiting_task Not FOUND in waiting_task!!!");
			//if (m_tmrSequencingStatus.isActive())
			//{
			//	m_tmrSequencingStatus.stop();
			//}
			QString qstr = QString("Sequencing waiting_task Not FOUND in waiting_task!!!Something May FAILED! Please Check!=%1").arg(++siempcnt);
			m_peditSequencingStatus->setText(qstr);
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
			LOGE("Wrong Sequencing waiting_task json content!!!={}", strValue.c_str());
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
				//m_plabSequencingStatus->setText(state);
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
		//StopThread2();
		m_peditSequencingStatus->setText("Stop Update RealTime Sequencing Analysis Status!");
		m_pdlgRecordSetting->SetNanoporeSequencingCheck(false);
	}
}

void SavePane::OnClickTranslate(void)
{
#if 0
	std::thread thrd([=]()
		{
			HttpClient httpcl;
			httpcl.SetIngorSSL(true);
			httpcl.GetReqHeader()["Content-Type"] = "multipart/form-data; boundary=<calculated when request is sent>";
			auto strUrl = std::string("http://192.168.0.106:9106/cyclonemail/send_file_email/");
			if (ConfigServer::GetInstance()->GetWorknet() == 1)
			{
				strUrl = std::string("http://172.16.18.2:9106/cyclonemail/send_file_email/");
			}
			std::string strbody = "{\"subject\":\"email server test\",\"body\":\"this is subject content\",\"comefrom\":\"wangwu\",\"to\":[\"lvyongjun@genomics.cn\"],\"cc\":[]}";
			std::string strfile = "E:\\release\\winver\\20210825162038.csv";
			bool ret = httpcl.RequestPostFiles(strUrl, strbody, strfile);
			if (!ret)
			{
				LOGW("Send email Request FAILED!!!url={}", strUrl.c_str());
			}
			size_t retsz = 0;
			auto retstr = httpcl.GetReadString(retsz);
			LOGI("Send email Request folder={}, status={}", strbody.c_str(), retstr.c_str());
		});
	thrd.detach();

	return;
#endif
	TranslateDialog trandlg(this);
	trandlg.exec();
}

void SavePane::OnClickReplot(void)
{
    PlotReviewDialog replotdlg(this);
    replotdlg.exec();
}

void SavePane::OnClickChooseFolder(void)
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Save Directory"),
		"/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
	{
		m_peditSavePath->setText(dir.replace("\\", "/"));
		m_pdlgRecordSetting->SetSavePath(dir.toStdString());
	}
}

void SavePane::OnClickOpenFolder(void)
{
#ifdef Q_OS_WIN32

	char sparam[1024] = { 0 };
	sprintf(sparam, "/e,/n,%s", m_peditSavePath->text().replace("/", "\\").toStdString().c_str());///e,/select
	::ShellExecuteA(nullptr, "open", "explorer.exe", sparam, NULL, SW_SHOWNORMAL);

#else

	QString openpath = m_peditSavePath->text().replace("\\", "/");
	QString cmd = QString("sudo -u %1 nautilus %2").arg(QString::fromStdString(ConfigServer::GetInstance()->GetLoginUserName())).arg(openpath);//sudo -u $SUDO_USER
	QProcess::startDetached(cmd);//system(cmd.toStdString().c_str());//

#endif
}

void SavePane::SetSelChannelSlot(const bool* psel)
{
	if (psel != nullptr)
	{
		for (int i = 0; i < CHANNEL_NUM; ++i)
		{
			if (m_vetSaveFile[i] != nullptr)
			{
				m_vetSelChannel[i] = psel[i];
			}
		}
	}
}

void SavePane::SetCloseChannelSlot(const bool* psel)
{
	if (psel != nullptr)
	{
		m_iSelectChan = 0;
		for (int i = 0; i < CHANNEL_NUM; ++i)
		{
			if (!psel[i])
			{
				m_vetSelChannel[i] = false;
				if (m_vetSaveFile[i] != nullptr)
				{
					fclose(m_vetSaveFile[i]);
					m_vetSaveFile[i] = nullptr;
					m_vetFileLength[i].first = 0;
				}
			}
			else
			{
				++m_iSelectChan;
				m_vetSelChannel[i] = true;
				if (m_vetSaveFile[i] == nullptr)
				{
					int ch = i + 1;
					char archanpath[512] = { 0 };
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
					sprintf(archanpath, "%s/channel%d/channel%d_%d.dat", m_strRawSavePath.c_str(), ch, ch, m_vetFileLength[i].second++);
					m_vetSaveFile[i] = fopen(archanpath, "wb");
					m_vetFileLength[i].first = 0;
				}
			}
		}
	}
}

void SavePane::setSamplingRateSlot(int rate)
{
	if (rate != 0)
	{
		m_pdlgRecordSetting->SetSamplingRate(rate / 1000);
	}
}

void SavePane::stopRealtimeSequencingAnalysisSlot(const QString qmsg)
{
	m_peditSequencingStatus->setText(qmsg);
	m_pdlgRecordSetting->SetNanoporeSequencingCheck(false);
	if (m_tmrSequencingStatus.isActive())
	{
		m_tmrSequencingStatus.stop();
	}
}

void SavePane::OnClickStartAuto(bool chk)
{
	if (chk)
	{
		const int tmermsec = 1000;
		emit startAutoStopTimerSignal(tmermsec);

		m_pbtnStartAuto->setText("StopAuto");
		m_tmrAutoStopTime.start(tmermsec);
	}
	else
	{
		if (m_tmAutoStopTime > QTime(0, 0, 0, 0))
		{
			emit stopAutoStopTimerSignal();
		}

		if (m_tmrAutoStopTime.isActive())
		{
			m_tmrAutoStopTime.stop();
		}
		m_pbtnStartAuto->setText("StartAuto");
	}
}

void SavePane::OnClickResetAuto(void)
{
	emit resetAutoStopTimerSignal(m_pispAutoStopTime->value());

	SetTimehms();
	m_plcdTimer->display(m_tmAutoStopTime.toString("hh:mm:ss"));
	m_plabTimeUp->setVisible(false);
}

void SavePane::TimerUpdateTimeSlot(void)
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

void SavePane::onEnableStartSaveDataSlot(bool val)
{
	m_pbtnStart->setEnabled(val);
}

void SavePane::checkAlgorithmServerStatusCodeSlot(void)
{
	m_bRedisReady = InitTransfer();
	if (m_bRedisReady)
	{
		auto ret = m_redisAccess.Get("status_code");
		int val = ::atoi(ret.c_str());
		if (val == 200)
		{
			ConfigServer::GetInstance()->SetAlgorithmServerStatus(true);
			m_pbtnStart->setEnabled(true);
			LOGI("Algorithm Server Start immediately SUCCESS.");
			m_redisAccess.DelAll("status_code");
		}
		else
		{
			m_tmrCheckAlgorithmServerState.start(5000);//Every 5s check the algorithm server state until server start well or timeout.
		}
	}
	else
	{
		ConfigServer::GetInstance()->SetAlgorithmServerStatus(false);
		m_pdlgRecordSetting->SetNanoporeSequencingCheck(false);
		m_pbtnStart->setEnabled(true);
		LOGW("Init redis FAILED!!!");
	}
}

void SavePane::timerCheckAlgorithmServerStateSlot(void)
{
	static int sicount = 0;
	auto ret = m_redisAccess.Get("status_code");
	int val = ::atoi(ret.c_str());
	if (val == 200)
	{
		ConfigServer::GetInstance()->SetAlgorithmServerStatus(true);
		sicount = 30;
		LOGI("Algorithm Server Start SUCCESS.");
		m_redisAccess.DelAll("status_code");
	}

	if (++sicount >= 30)
	{
		if (m_tmrCheckAlgorithmServerState.isActive())
		{
			m_tmrCheckAlgorithmServerState.stop();
		}
		if (!ConfigServer::GetInstance()->GetAlgorithmServerStatus())
		{
			m_pdlgRecordSetting->SetNanoporeSequencingCheck(false);
		}
		m_pbtnStart->setEnabled(true);
		LOGI("Algorithm Server Start Finished.");
	}
}

void SavePane::SetDataSaveSlot(bool val)
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
