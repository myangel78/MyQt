#include "UpdateDialog.h"
#include "mainversion.h"
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextCodec>
#include <QJsonParseError>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QEventLoop>
#include <QTimer>
#include <QProcess>
#include <QProgressBar>
#include <Log.h>
#include <QDesktopServices>
#include <QDir>
#ifdef Q_OS_WIN32
#include <windows.h>
#include <shellapi.h>
#endif
#include <QCheckBox>
#include "ConfigServer.h"




UpdateDialog::UpdateDialog(QWidget* parent) :QDialog(parent)
{
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint;
	this->setWindowFlags(flags);
	InitCtrl();
}
UpdateDialog::~UpdateDialog(void)
{

}

void UpdateDialog::InitCtrl(void)
{

	m_pIconLab = new QLabel(this);
	m_pCurVerDescLab	= new QLabel(QStringLiteral("Current Version:"),this);
	m_pCurVerLab		= new QLabel(SOFTWARE_VERSION, this);
	m_pLastVerDescLab	= new QLabel(QStringLiteral("Latest Version:"), this);
	m_pLastVerLab = new QLabel(tr("..."),this);
	
	SetLabelFontStyle(m_pCurVerDescLab, "Microsoft YaHei", 8, QFont::Normal, "black");
	SetLabelFontStyle(m_pCurVerLab, "Microsoft YaHei", 8, QFont::Normal, "black");
	SetLabelFontStyle(m_pLastVerDescLab, "Microsoft YaHei", 8, QFont::Normal, "black");
	SetLabelFontStyle(m_pLastVerLab, "Microsoft YaHei", 8, QFont::Normal, "black");

	QPixmap icon(":/img/img/BGI_LOGO.ico");
	icon = icon.scaledToWidth(64, Qt::SmoothTransformation);
	m_pIconLab->setPixmap(icon);


	m_pUpdateBtn = new QPushButton(tr("Update"), this);
	m_pLatestVerBrow = new QTextBrowser(this);
	OnEnableUpdateBtn(false);

	m_pLatestVerGpbox = new QGroupBox(QStringLiteral("Latest Version"), this);

	m_pUpdateProgressBar = new QProgressBar(this);
	m_pUpdateProgressBar->setVisible(false);

	QVBoxLayout* vertgplayout = new QVBoxLayout();
	vertgplayout->addWidget(m_pLatestVerBrow);
	m_pLatestVerGpbox->setLayout(vertgplayout);


	QGridLayout* vergrilayout = new QGridLayout();
	vergrilayout->addWidget(m_pIconLab, 0, 0, 4, 1);
	vergrilayout->addWidget(m_pCurVerDescLab,0,1,1,1);
	vergrilayout->addWidget(m_pCurVerLab,0,2, 1, 1);
	vergrilayout->addWidget(m_pLastVerDescLab,1,1, 1, 1);
	vergrilayout->addWidget(m_pLastVerLab,1,2, 1, 1);

	QHBoxLayout* hbrowlayout = new QHBoxLayout();
	hbrowlayout->addWidget(m_pLatestVerGpbox);

	QHBoxLayout* horuplayout = new QHBoxLayout();
	horuplayout->addStretch();
	horuplayout->addWidget(m_pUpdateBtn);

	QVBoxLayout* vertlayout = new QVBoxLayout();
	vertlayout->addLayout(vergrilayout);
	vertlayout->addLayout(hbrowlayout);
	vertlayout->addWidget(m_pUpdateProgressBar);
	vertlayout->addLayout(horuplayout);
	//    vertlayout->addStretch();

	setLayout(vertlayout);


	connect(m_pUpdateBtn, SIGNAL(clicked()), this, SLOT(OnUpdateBtnclickedSlot()));
	this->resize(minimumSizeHint());

	m_updatefilePath.clear();
	m_intallPath.clear();

#ifdef Q_OS_WIN32
	m_intallPath = QString::fromStdString(ConfigServer::GetUserPath());
#else
	m_intallPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/LAB256pro";
#endif

}

/**
 * @brief 检查服务器版本和环境
 */
void UpdateDialog::AutoCheckUpdate(void)
{
	bool bisNeedUpdate = false;
	QUrl url("http://172.16.18.2:9101/version/lab256_pro");

	OnQuerySerVersionInfo(url, bisNeedUpdate);
	m_pUpdateProgressBar->reset();
	m_pUpdateProgressBar->setVisible(false);
	OnEnableUpdateBtn(bisNeedUpdate);


}


/**
 * @brief 请求获取服务器版本信息
 */
bool UpdateDialog::OnQuerySerVersionInfo(const QUrl url,bool &bisNeedUpdate)
{
	QTimer timer;
	timer.setInterval(3000);    //设置超时时间
	timer.setSingleShot(true);  // 单次触发

	QNetworkAccessManager manager;
	QNetworkRequest request;
	//request.setUrl(QUrl("http://172.16.18.2:9101/version/lab256_pro"));
	request.setUrl(url);

	QNetworkReply* pReply = manager.get(request);

	QEventLoop loop;
	connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
	connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	timer.start();
	loop.exec();  // 启动事件循环

	if (timer.isActive()) {  // 处理响应
		timer.stop();
		if (pReply->error() != QNetworkReply::NoError) {
			// 错误处理
			pReply->deleteLater();
			qDebug() << "Error String : " << pReply->errorString();
			m_pLastVerLab->setText(tr("Network error"));
			bisNeedUpdate = false;
			return false;
		}
		else {
			QTextCodec* codec = QTextCodec::codecForName("utf8");
			QString jsonStr = codec->toUnicode(pReply->readAll());
			QMap<QString, QString> map;
			if (ParseLatestVerInfo(jsonStr, map))
			{
				if (map.contains("verinfo"))
				{
					m_pLatestVerBrow->setText(tr("%1").arg(map["verinfo"]));
				}
				if (map.contains("version"))
				{
					m_pLastVerLab->setText(QString("V%1").arg(map["version"]));
					m_requireversion = map["version"];
					if (CompareVersion(m_requireversion, SOFTWARE_VERSION) != 0)
					{
						SetLabelFontStyle(m_pLastVerLab, "Microsoft YaHei", 9, QFont::Bold, "green");
						bisNeedUpdate = true;
					}
					else
					{
						SetLabelFontStyle(m_pLastVerLab, "Microsoft YaHei", 8, QFont::Normal, "black");
						bisNeedUpdate = false;
					}
				}
			}
		}
	}
	else {  // 处理超时
		disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		m_pLastVerLab->setText(tr("Network error"));
		SetLabelFontStyle(m_pLastVerLab, "Microsoft YaHei", 8, QFont::Bold, "red");
		bisNeedUpdate = false;
		pReply->abort();
		pReply->deleteLater();
		qDebug() << "Timeout";
		return false;
	}

	pReply->deleteLater();
	return true;
}



/**
 * @brief 解析服务器返回的信息
 * @param jsonStr 原始文本
 * @param reMap 生成键值对
 * @return 是否成功
 */
bool UpdateDialog::ParseLatestVerInfo(const QString& jsonStr, QMap<QString, QString>& reMap)
{
	bool res = false;
	reMap.clear();
	QJsonParseError parseJsonErr;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseJsonErr);
	if (!(parseJsonErr.error == QJsonParseError::NoError))
	{
		qDebug() << tr("解析json文件错误！");
		return res;
	}
	if (jsonDocument.isObject())
	{
		QJsonObject jsonObject = jsonDocument.object();
		if (jsonObject.contains(QStringLiteral("mustupdate")))
		{
			QJsonValue jsonValue = jsonObject.value(QStringLiteral("mustupdate"));
			reMap["mustupdate"] = QString("%1").arg(jsonValue.toInt());
		}
		if (jsonObject.contains(QStringLiteral("remark")))
		{
			QJsonValue jsonValue = jsonObject.value(QStringLiteral("remark"));
			reMap["remark"] = QString("%1").arg(jsonValue.toString());
		}
		if (jsonObject.contains(QStringLiteral("status")))
		{
			QJsonValue jsonValue = jsonObject.value(QStringLiteral("status"));
			reMap["status"] = QString("%1").arg(jsonValue.toString());
		}
		if (jsonObject.contains(QStringLiteral("verinfo")))
		{
			QJsonValue jsonValue = jsonObject.value(QStringLiteral("verinfo"));
			reMap["verinfo"] = QString("%1").arg(jsonValue.toString());
		}
		if (jsonObject.contains(QStringLiteral("version")))
		{
			QJsonValue jsonValue = jsonObject.value(QStringLiteral("version"));
			reMap["version"] = QString("%1").arg(jsonValue.toString());
		}
		res = true;
	}
	//    QMapIterator<QString, QString> i(map);
	//    while (i.hasNext()) {
	//        i.next();
	//        qDebug() << i.key() << ": " << i.value() << Qt::endl;
	//    }
	return res;
}


/**
 * @brief linux 执行升级步骤
 * @param filepath 文件位置
 * @return 是否成功
 */
bool UpdateDialog::OnUnzipLuxupFile(const QString& filepath)
{
	QFileInfo fileInfo(filepath);
	QString fileName = fileInfo.fileName().replace(QRegExp("\\.?[a-zA-Z]"), "");
	QDir dir(fileInfo.path());
	if (!dir.exists() || !dir.cdUp())
	{
		qWarning() << "cd dir" << dir.path() << "is failed";
		return false;
	}

	QString cmd = QString("unzip -o -d %1/%2 %3").arg(dir.path()).arg(fileName).arg(filepath);
	if (!ProcessUpdateCommand(cmd))
	{
		return false;
	}

	if (dir.cd(fileName))
	{
		QString scriptfile(QString("%1/%2").arg(dir.path()).arg("GenerateShortcut.sh"));
		if (QFile(scriptfile).exists())
		{
			QString cmd = QString("chmod +x %1").arg(scriptfile);
			if (!ProcessUpdateCommand(cmd, dir.path()))
			{
				return false;
			}

			QString cmd2 = QString("sh %1").arg(scriptfile);
			if (!ProcessUpdateCommand(cmd2, dir.path()))
			{
				return false;
			}
		}
	}
	return true;
}



/**
 * @brief 版本比较
 * @param verQuery 服务器版本
 * @param verLocal 本地版本
 * @return  0  版本相同
 *          -1 本地版本偏高
 *          1  本地版本偏低
 */
int UpdateDialog::CompareVersion(QString verQuery, QString verLocal)
{
	QStringList localist = verLocal.replace(QRegExp("[a-zA-Z]"), "").trimmed().split(".");
	QStringList querylist = verQuery.replace(QRegExp("[a-zA-Z]*256[a-zA-Z]*"), "").trimmed().split(".");

	int comparelength = qMin(localist.size(), querylist.size());
	for (int i = 0; i < comparelength; i++) {
		qint32 subLocal = localist.at(i).toInt();
		qint32 subQuery = querylist.at(i).toInt();
		if (subQuery < subLocal) {
			return -1;
		}
		else if (subQuery > subLocal) {
			return 1;
		}
	}
	if (localist.length() == querylist.length()) {
		return 0;
	}
	else {
		return (localist.length() < querylist.length() ? 1 : -1);
	}
}


/**
 * @brief 搜索升级包
 * @param searchPath 搜索路径
 * @param upFile 返回的升级包路径
 * @return 是否成功
 */
bool UpdateDialog::SearchUpdatePackage(const QString searchPath, const QString& version, QString& upFile)
{
	QDir searchDir(searchPath);
	searchDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);
	searchDir.setSorting(QDir::Size | QDir::Reversed);

#ifdef Q_OS_WIN32
	QStringList filters = { "*.exe" };
	QString filename = QString("Lab256ProsystemV%1.exe").arg(version);
#else
	QStringList filters = { "*.zip" };
	QString filename = QString("%1.zip").arg(version);
#endif
	searchDir.setNameFilters(filters);
	QFileInfoList list = searchDir.entryInfoList();
	QFileInfoList::const_iterator cit = list.begin();
	while (cit != list.end())
	{
		if ((*cit).fileName().contains(filename, Qt::CaseInsensitive))
		{
			upFile = (*cit).absoluteFilePath();
			qDebug() << "fileName" << upFile<< "absolute filepath"<<(*cit).absoluteFilePath()<< (*cit).absolutePath();
			return true;
		}
		cit++;
	}
	return false;
}

/**
 * @brief 执行外部指令模块
 * @param command 外部指令
 * @param workDir 工作目录
 * @return 是否成功
 */
bool UpdateDialog::ProcessUpdateCommand(const QString& command, const QString workDir)
{
	bool result = true;
	QProcess* pProcees = new QProcess;
	QStringList env = QProcess::systemEnvironment();
#ifdef Q_OS_WIN32
	env << "C:\\Program Files\\Git\\cmd";
#endif
	if (!workDir.isEmpty())
	{
		pProcees->setWorkingDirectory(workDir);
	}
	pProcees->setEnvironment(env);
	pProcees->start(command);
	if (!pProcees->waitForStarted(5000))
	{
		qWarning("Wait for QProcess started failed: %s in Dir %s", command.toStdString().data(), workDir.toStdString().data());
		result = false;
		return result;
	}
	if (!pProcees->waitForFinished(10000))
	{
		qWarning("Wait for QProcess finished failed: %s in Dir %s", command.toStdString().data(), workDir.toStdString().data());
		result = false;
	}
	//    qDebug()<<pProcees->readAllStandardOutput();
	pProcees->deleteLater();
	return result;
}


void UpdateDialog::OnUpdateBtnclickedSlot(void)
{
	int step = 0;
	OnEnableUpdateBtn(false);
	m_pUpdateProgressBar->setStyleSheet("color: black; text-align:right;");
	m_pUpdateProgressBar->setVisible(true);
	m_pUpdateProgressBar->reset();
	m_pUpdateProgressBar->setMaximum(5);
	m_pUpdateProgressBar->setValue(++step);
	if (!HasGitToolEnv())
	{
		if (QMessageBox::question(this, tr("Update"),
			tr("You don't have git environment,please download git first?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			QDesktopServices::openUrl(QUrl("https://git-scm.com/downloads"));
		}
		LOGI("[Update] You don't have git environment,please download git first ");
		UpdateFailedOpera();
		return;
	}

	m_pUpdateProgressBar->setValue(++step);
	if (!CreatProDir(m_intallPath))
	{
		LOGI("[Update] CreatProDir: {} failed!!!", m_intallPath.toStdString().data());
		UpdateFailedOpera();
		return;
	}

	m_pUpdateProgressBar->setValue(++step);
	bool bPrjExist = false;
	QString prjPath = GetGitProjectPath(m_intallPath,bPrjExist);
	if (!UpdateGitProjet(prjPath, bPrjExist))
	{
		LOGI("[Update] Update git lab256pro project: {} failed!!! ", prjPath.toStdString().data());
		UpdateFailedOpera();
		return;
	}

	m_pUpdateProgressBar->setValue(++step);
	QString gitPjtPath = PlatformPathTrans(m_intallPath + "/lab256pro");
	QString updateFilePath = "";
	if (!SearchUpdatePackage(gitPjtPath, m_requireversion, updateFilePath))
	{
		LOGI("[Update] Search upadate package: {} failed!!! ", gitPjtPath.toStdString().data());
		UpdateFailedOpera();
		return;
	}

    m_pUpdateProgressBar->setValue(++step);
    OnEnableUpdateBtn(true);
	if (!updateFilePath.isEmpty())
	{
#ifdef Q_OS_WIN32
		ShellExecute(NULL, "open", updateFilePath.toStdString().c_str(), NULL, NULL, SW_SHOWMAXIMIZED);
		//                exit(0);
#else
        if (OnUnzipLuxupFile(updateFilePath))
		{
			if (QMessageBox::question(this, tr("Request"), tr("It had already update successfully,close the programe yes or not"), \
				QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				exit(0);
			}
        }
        else
        {
            LOGI("[Update] Unzip Linux package：{} failed!!! ", updateFilePath.toStdString().data());
            UpdateFailedOpera();
        }

#endif
	}
	else
	{
		m_pUpdateProgressBar->setVisible(false);
		LOGI("[Update] upadate package：{} failed!!! ", updateFilePath.toStdString().data());
		UpdateFailedOpera();
		return;
	}

	return;
}


void UpdateDialog::UpdateFailedOpera(void)
{
	m_pUpdateProgressBar->setStyleSheet("color: red; text-align:right;");
	QMessageBox::information(this, tr("Update error"), tr("Update failed!!! pls see the log detail"));
	OnEnableUpdateBtn(true);
}


bool UpdateDialog::UpdateGitProjet(const QString &path,const bool & bIsExist)
{
	if (bIsExist)
	{
		return ProcessUpdateCommand("git pull ", path);
	}
	else
	{
		return ProcessUpdateCommand("git clone http://172.16.18.2:9103/lab256pro.git ", path);
	}

}


QString UpdateDialog::GetGitProjectPath(const QString &path, bool & bIsExist)
{
	QDir dirGit(path);
	if (dirGit.cd("lab256pro"))
	{
		QString dotGigPth = PlatformPathTrans(dirGit.absolutePath() + "/.git");
		if (QDir(dotGigPth).exists())
		{
			bIsExist = true;
			return dirGit.path();
		}
		else
		{
			bIsExist = false;
			return path;
		}
	}
	else
	{
		bIsExist = false;
		return path;
	}

}


/**
 * @brief 是否具有Git环境
 * @return 是否成功
 */
bool UpdateDialog::HasGitToolEnv(void)
{
#ifdef Q_OS_WIN32
	QString gitPath = "C:\\Program Files\\Git\\cmd\\git.exe";
#else
	QString gitPath = "/usr/bin/git";
#endif
	if (!QFile::exists(gitPath))
	{
		return false;
	}
	return QFile::exists(gitPath);
}


/**
 * @brief 创建目录
 * @param dirPath 目录路径
 * @return 是否成功
 */
bool UpdateDialog::CreatProDir(const QString& dirPath)
{
	QDir dir(dirPath);
	if (!dir.exists())
	{
		return dir.mkpath(dirPath);
	}
	return true;
}

/**
 * @brief 路径名称平台转换
 * @param str 路径字符串
 * @return 转换后的路径
 */

QString UpdateDialog::PlatformPathTrans(QString str)
{
#ifdef Q_OS_WIN32
	return str.replace("/", "\\");
#else
	return str.replace("\\", "/");
#endif
}


void UpdateDialog::SetLabelFontStyle(QLabel* pLabel, const QString& family, int size, QFont::Weight weight, const QString& color)
{
	if (pLabel)
	{
		QFont font(family, size, weight);
		pLabel->setFont(font);
		pLabel->setStyleSheet(QString("color:%1;").arg(color));
	}
}



