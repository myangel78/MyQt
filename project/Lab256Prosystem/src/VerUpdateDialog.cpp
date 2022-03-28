#include "VerUpdateDialog.h"
#include <qlayout.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qthread.h>
#ifdef Q_OS_WIN32
#include <windows.h>
#endif
#include <Log.h>
#include "mainversion.h"
#include "HttpClient.h"
#include "ConfigServer.h"


VerUpdateDialog::VerUpdateDialog(QWidget *parent) : QDialog(parent)
{
	resize(366, 266);
	InitCtrl();

	connect(this, &VerUpdateDialog::failedVersionInfoSignal, this, &VerUpdateDialog::failedVersionInfoSlot);
	connect(this, &VerUpdateDialog::parseVersionInfoSignal, this, &VerUpdateDialog::parseVersionInfoSlot);

#if 0
	RequestVersionInfo();
#else

#if 0

	auto qfunc = std::function<void(void)>([&](void)
		{
			RequestVersionInfo();
		});
	QThread* qthr = QThread::create(qfunc);
	qthr->start();

#else

	std::thread thrd([this]()
		{
			RequestVersionInfo();
		});
	thrd.detach();

#endif

#endif
}

void VerUpdateDialog::accept()
{
	QDialog::accept();
}

void VerUpdateDialog::reject()
{
	if (m_bHasUpdate && m_iMustUpdate == 1)
	{
		auto ret = QMessageBox::question(this, "Not update must version?", "App will exit. Really don't want to update?");
		if (ret != QMessageBox::Yes)
		{
			return;
		}
		m_bExitMain = true;
	}
	else
	{
		//auto ret = QMessageBox::question(this, "Not update?", "Really don't want to update?");
		//if (ret != QMessageBox::Yes)
		//{
		//	return;
		//}
		m_bExitMain = false;
	}

	QDialog::reject();
}

bool VerUpdateDialog::InitCtrl(void)
{
	m_plabLogoImage = new QLabel(this);
	QPixmap pix(":/img/img/BGI_LOGO.ico");
	pix = pix.scaledToWidth(64, Qt::SmoothTransformation);
	m_plabLogoImage->setPixmap(pix);

	m_plabVerInfo = new QLabel("<b>LAB256_PRO_SYSTEM</b>", this);

	m_plabCurVersion = new QLabel("Current Version:", this);
	m_plabCurVersion2 = new QLabel(QString("<b>") + SOFTWARE_VERSION + "</b>", this);

	m_plabLatestVersion = new QLabel("Latest  Version:", this);
	m_plabLatestVersion2 = new QLabel("Checking Latest....", this);

	m_plabHasUpdate = new QLabel("<b>Checking Version Available....</b>", this);

	m_pgrpUpdateInfo = new QGroupBox("Update Info", this);
	m_peditUpdateInfo = new QTextEdit("Getting Update Info....<br>Getting The other info....", m_pgrpUpdateInfo);
	m_peditUpdateInfo->setReadOnly(true);
	//m_pgrpUpdateInfo->setMaximumHeight(120);

	m_pbtnUpdateNow = new QPushButton("Update Now", this);
	m_pbtnUpdateNow->setIcon(QIcon(":/img/img/update.png"));
	m_pbtnUpdateNow->setEnabled(false);
	m_pbtnIgnoreVer = new QPushButton("Ignore This Version", this);
	m_pbtnIgnoreVer->setEnabled(false);


	QGridLayout* gdlayout = new QGridLayout();
	gdlayout->addWidget(m_plabCurVersion, 0, 0);
	gdlayout->addWidget(m_plabCurVersion2, 0, 1);
	gdlayout->addWidget(m_plabLatestVersion, 1, 0);
	gdlayout->addWidget(m_plabLatestVersion2, 1, 1);

	QVBoxLayout* verlayout1 = new QVBoxLayout();
	verlayout1->addWidget(m_plabVerInfo);
	verlayout1->addLayout(gdlayout);
	verlayout1->addStretch(1);
	verlayout1->addWidget(m_plabHasUpdate);

	QHBoxLayout* horlayout1 = new QHBoxLayout();
	horlayout1->addWidget(m_plabLogoImage);
	horlayout1->addLayout(verlayout1);
	horlayout1->addStretch(1);

	QVBoxLayout* verlayout2 = new QVBoxLayout();
	verlayout2->addWidget(m_peditUpdateInfo);
	m_pgrpUpdateInfo->setLayout(verlayout2);

	QHBoxLayout* horlayout5 = new QHBoxLayout();
	horlayout5->addSpacing(20);
	horlayout5->addWidget(m_pbtnUpdateNow);
	horlayout5->addStretch(1);
	horlayout5->addWidget(m_pbtnIgnoreVer);
	horlayout5->addSpacing(20);

	QVBoxLayout* verlayout8 = new QVBoxLayout();
	verlayout8->addLayout(horlayout1);
	verlayout8->addWidget(m_pgrpUpdateInfo);
	verlayout8->addLayout(horlayout5);

	setLayout(verlayout8);

	connect(m_pbtnUpdateNow, &QPushButton::clicked, this, &VerUpdateDialog::OnClickUpdatenow);
	connect(m_pbtnIgnoreVer, &QPushButton::clicked, this, &VerUpdateDialog::OnClickIgnoreVersion);

	return false;
}

bool VerUpdateDialog::RequestVersionInfo(void)
{
	HttpClient httpcl;
	httpcl.SetIngorSSL(true);
	auto strUrl = ConfigServer::GetInstance()->GetVerupdateAddr();
	//if (strUrl.size() <= 0)
	{
		if (ConfigServer::GetInstance()->GetWorknet() == 1)
		{
			strUrl = "http://172.16.18.2:9101/version/lab256_pro";
		}
		else
		{
			strUrl = "http://192.168.0.77:9106/cycloneconfig/lab256_pro/";//"http://192.168.0.106:9101/version/lab256_pro";
		}
		ConfigServer::GetInstance()->SetVerupdateAddr(strUrl);
	}
	bool ret = httpcl.RequestGet(strUrl);
	if (!ret)
	{
		emit failedVersionInfoSignal();
		LOGW("Get Version Update Info FAILED!!!url={}", strUrl.c_str());
		return false;
	}

	size_t retsz = 0;
	auto retstr = httpcl.GetReadString(retsz);

	emit parseVersionInfoSignal(QString::fromStdString(retstr));
	return true;

#if 0
	QJsonParseError jsonError;
	QJsonDocument jsonDocument(QJsonDocument::fromJson(retstr.c_str(), &jsonError));

	if (jsonError.error != QJsonParseError::NoError)
	{
		LOGE("Wrong json content!!!={}", retstr.c_str());
		return false;
	}

	if (jsonDocument.isObject())
	{
		QJsonObject jsonObject = jsonDocument.object();
		auto ite = jsonObject.find("status");
		if (ite != jsonObject.end())
		{
			auto stats = ite->toString();
			if (stats != "ok")
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		ite = jsonObject.find("version");
		if (ite != jsonObject.end())
		{
			auto stats = ite->toString();
			m_strVersion = stats.toStdString();
			stats = "V" + stats;
			if (stats != SOFTWARE_VERSION)
			{
				m_bHasUpdate = true;
				auto ver = "<b><font color=red>" + stats + "</font></b>";
				m_plabLatestVersion2->setText(ver);
				m_plabHasUpdate->setText("<b><font color=blue>There is a version available to update.</font></b>");
				m_pbtnUpdateNow->setEnabled(true);
			}
			else
			{
				m_bHasUpdate = false;
				auto ver = "<b><font color=green>" + stats + "</font></b>";
				m_plabLatestVersion2->setText(ver);
				m_plabHasUpdate->setText("<b>This is the Latest Version.</b>");
				m_pbtnUpdateNow->setEnabled(false);
			}
		}
		else
		{
			return false;
		}

		ite = jsonObject.find("mustupdate");
		if (ite != jsonObject.end())
		{
			m_iMustUpdate = ite->toInt();
			m_pbtnIgnoreVer->setEnabled(m_iMustUpdate == 0);
		}

		ite = jsonObject.find("verinfo");
		if (ite != jsonObject.end())
		{
			auto stats = ite->toString().toStdString();
			//stats = "1.增加MotorID字段。<br>2.更改保存数据的文件夹命名。<br>3.增加数据文件夹上传功能。<br>4.暂时取消定时测序QC时的通道数据动态保存。<br>5.电容计算停止，电压回到开始前的值。<br>6.增加软件自动检测更新。";
			//m_peditUpdateInfo->setText(QString::fromLocal8Bit(stats.c_str()));
			m_peditUpdateInfo->setText(QString(stats.c_str()));
		}
	}
	return true;
#endif
}

void VerUpdateDialog::OnClickUpdatenow(void)
{
#if 0
	{
		HttpClient httpcl;
		httpcl.SetIngorSSL(true);
		auto strUrl = std::string("http://192.168.0.106:9106/cyclonemail/send_file_email/");//ConfigServer::GetInstance()->GetVerupdateAddr();
		std::string strData;
		bool ret = httpcl.RequestPost(strUrl, strData);
		if (!ret)
		{
			LOGW("Send email FAILED!!!url={}", strUrl.c_str());
			return;
		}
	}
	return;
#endif

	auto strUserPath(ConfigServer::GetUserPath());
	std::filesystem::path userPath(strUserPath);
	if (!std::filesystem::exists(userPath))
	{
		std::filesystem::create_directories(userPath);
	}
	auto strUrl = ConfigServer::GetInstance()->GetVerloadAddr();
	//if (strUrl.size() <= 0)
	{
		if (ConfigServer::GetInstance()->GetWorknet() == 1)
		{
			strUrl = "http://172.16.18.2:9103/lab256pro.git";
		}
		else
		{
			strUrl = "http://192.168.0.106:9103/lab256pro.git";
		}
		ConfigServer::GetInstance()->SetVerloadAddr(strUrl);
	}

	//std::thread thrd([&]()
	//	{
#ifdef Q_OS_WIN32

	char arcmd[512] = { 0 };
	std::string strLoadPath = strUserPath + "lab256pro";
	std::filesystem::path loadPath(strLoadPath);
	if (std::filesystem::exists(loadPath))
	{
		sprintf(arcmd, "cd /d %s && rd /s/q lab256pro && git clone %s", strUserPath.c_str(), strUrl.c_str());
	}
	else
	{
		sprintf(arcmd, "cd /d %s && git clone %s", strUserPath.c_str(), strUrl.c_str());
	}
	system(arcmd);

	std::string strAppFullname = strLoadPath + "/Lab256ProsystemV" + m_strVersion + ".exe";
	std::filesystem::path appPath(strAppFullname);
	if (std::filesystem::exists(appPath))
	{
		ShellExecute(NULL, "open", strAppFullname.c_str(), NULL, NULL, SW_SHOWMAXIMIZED);
		//QMessageBox::information(this, "Update SUCCESS", "Update SUCCESS! Please restart to take effect!");
	}
	else
	{
		QMessageBox::warning(this, "Update FALIED", "Version Files not exist, Update FAILED!!!");
	}

#else

	char arcmd[512] = { 0 };
	std::string strLoadPath = strUserPath + "lab256pro/";
	std::filesystem::path loadPath(strLoadPath);
	if (std::filesystem::exists(loadPath))
	{
		sprintf(arcmd, "cd %s ; rm -rf lab256pro ; git clone %s", strUserPath.c_str(), strUrl.c_str());
	}
	else
	{
		sprintf(arcmd, "cd %s ; git clone %s", strUserPath.c_str(), strUrl.c_str());
	}
	system(arcmd);

	//std::string mainUserName = ConfigServer::GetInstance()->GetMainUserName();
	//const char* puser = mainUserName.c_str();//$USER//me=${SUDO_USER:-$LOGNAME}//$me
	std::string strAppFullname = strLoadPath + m_strVersion + ".zip";
	std::filesystem::path appPath(strAppFullname);
	if (std::filesystem::exists(appPath))
	{
		char* homepath = getenv("HOME");
		if (homepath != nullptr)
		{
			std::string strExePath(homepath);
			strExePath += "/LAB256Pro/" + m_strVersion;
			sprintf(arcmd, "sudo -u $SUDO_USER unzip -o -d %s %s", strExePath.c_str(), strAppFullname.c_str());
			system(arcmd);
			sprintf(arcmd, "cd %s && sudo -u $SUDO_USER bash GenerateShortcut.sh", strExePath.c_str());
			system(arcmd);
			//sprintf(arcmd, "secUser=`whoami | awk '{print $1}'` ; #echo $secUser ; su $secUser -c <unzip -o -d %s %s ; cd %s ; chmod 755 GenerateShortcut.sh ; sh GenerateShortcut.sh>", strExePath.c_str(), strAppFullname.c_str(), strExePath.c_str());//
			//sprintf(arcmd, "unzip -o -d %s %s ; cd %s ; chmod 755 GenerateShortcut.sh ; sh GenerateShortcut.sh", strExePath.c_str(), strAppFullname.c_str(), strExePath.c_str());//
			//system(arcmd);
			//sprintf(arcmd, "secUser=`whoami | awk '{print $1}'` ; echo $secUser ; su $secUser -c \"unzip -o -d %s %s\"", strExePath.c_str(), strAppFullname.c_str());
			//system(arcmd);
			//sprintf(arcmd, "secUser=`whoami | awk '{print $1}'` ; echo $secUser ; su $secUser -c \"cd %s ; chmod 755 GenerateShortcut.sh ; sh GenerateShortcut.sh\"", strExePath.c_str());
			//system(arcmd);
			QMessageBox::information(this, "Update SUCCESS", "Update SUCCESS! Please restart to take effect!");
		}
		else
		{
			QMessageBox::warning(this, "No Home Folder", "Get Home Folder FAILED!!!");
		}
	}
	else
	{
		QMessageBox::warning(this, "Update FALIED", "Version Files not exist, Update FAILED!!!");
	}

#endif
	//	});
	//thrd.detach();

	//updatever report
	std::string strTitle = "LAB256Pro Update:From=";
	strTitle += SOFTWARE_VERSION;
	strTitle += " To=V";
	strTitle += m_strVersion;

	std::string datetime = QDateTime::currentDateTime().toString("[yyyyMMddhhmmss] ").toStdString();
	std::string strDetail = datetime;
	strDetail += SOFTWARE_VERSION;
	strDetail += " To=V";
	strDetail += m_strVersion;

	std::string strComefrom = "[LAB256Pro] [";
	strComefrom += ConfigServer::GetLocalIPAddress();
	strComefrom += "] ";
	strComefrom += std::to_string(ConfigServer::GetInstance()->GetSequencingId() + 1);

	QJsonObject jsonObject;
	jsonObject.insert("subject", strTitle.c_str());//
	jsonObject.insert("body", strDetail.c_str());//
	jsonObject.insert("comefrom", strComefrom.c_str());//
	QJsonArray jsonArrayRecv;
	jsonArrayRecv.append("ANYBODY");//
	jsonObject.insert("to", jsonArrayRecv);//
	QJsonArray jsonArraycc;
	jsonArraycc.append("lvyongjun@genomics.cn");//
	jsonObject.insert("cc", jsonArraycc);//

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);

	std::string strbody = jsonDoc.toJson().toStdString();
	std::string strfile;

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
			bool ret = httpcl.RequestPostFiles(strUrl, strbody, strfile);
			if (!ret)
			{
				LOGW("Send email Request FAILED!!!url={}", strUrl.c_str());
				return;
			}
			size_t retsz = 0;
			auto retstr = httpcl.GetReadString(retsz);
			LOGI("Update={}", retstr.c_str());
		});
	thrd.detach();


	accept();
}

void VerUpdateDialog::OnClickIgnoreVersion(void)
{
	ConfigServer::GetInstance()->SetIgnoreVer(m_strVersion);
	accept();
}

void VerUpdateDialog::failedVersionInfoSlot(void)
{
	m_plabLatestVersion2->setText("<b><font color=red>Get Version FAILED!!!</font></b>");
	m_plabHasUpdate->setText("<b><font color=red>Can't Get Version Update Info!!!</font></b>");
	m_peditUpdateInfo->setText("<font color=red><b>Request Version Update Info FAILED!!!</b><br><br><b>See Log for detail FAILED info.</b></font>");
}

void VerUpdateDialog::parseVersionInfoSlot(const QString& qres)
{
	QJsonParseError jsonError;
	QJsonDocument jsonDocument(QJsonDocument::fromJson(qres.toUtf8(), &jsonError));

	if (jsonError.error != QJsonParseError::NoError)
	{
		LOGE("Wrong Updateinfo json content!!!");
		return;
	}

	if (jsonDocument.isObject())
	{
		QJsonObject jsonObject = jsonDocument.object();
		auto ite = jsonObject.find("status");
		if (ite != jsonObject.end())
		{
			auto stats = ite->toString();
			if (stats != "ok")
			{
				return;
			}
		}
		else
		{
			return;
		}

		ite = jsonObject.find("version");
		if (ite != jsonObject.end())
		{
			auto stats = ite->toString();
			m_strVersion = stats.toStdString();
			stats = "V" + stats;
			if (stats != SOFTWARE_VERSION)
			{
				m_bHasUpdate = true;
				auto ver = "<b><font color=red>" + stats + "</font></b>";
				m_plabLatestVersion2->setText(ver);
				m_plabHasUpdate->setText("<b><font color=blue>There is a version available to update.</font></b>");
				m_pbtnUpdateNow->setEnabled(true);
			}
			else
			{
				m_bHasUpdate = false;
				auto ver = "<b><font color=green>" + stats + "</font></b>";
				m_plabLatestVersion2->setText(ver);
				m_plabHasUpdate->setText("<b>This is the Latest Version.</b>");
				m_pbtnUpdateNow->setEnabled(false);
			}
		}
		else
		{
			return;
		}

		ite = jsonObject.find("mustupdate");
		if (ite != jsonObject.end())
		{
			m_iMustUpdate = ite->toInt();
			m_pbtnIgnoreVer->setEnabled(m_iMustUpdate == 0);
		}

		ite = jsonObject.find("verinfo");
		if (ite != jsonObject.end())
		{
			auto stats = ite->toString().toStdString();
			//stats = "1.增加MotorID字段。<br>2.更改保存数据的文件夹命名。<br>3.增加数据文件夹上传功能。<br>4.暂时取消定时测序QC时的通道数据动态保存。<br>5.电容计算停止，电压回到开始前的值。<br>6.增加软件自动检测更新。";
			//m_peditUpdateInfo->setText(QString::fromLocal8Bit(stats.c_str()));
			m_peditUpdateInfo->setText(QString(stats.c_str()));
		}
	}
}
