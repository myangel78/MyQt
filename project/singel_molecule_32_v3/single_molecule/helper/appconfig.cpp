#include "appconfig.h"
#include "logger/Log.h"
#include <QString>
#include <QtCore>
#include <QStandardPaths>
#include "./helper/commonhelper.h"
#include<QDir>

#define GET_ARRAY_LEN(array,len) {len = (sizeof(array) / sizeof(array[0]));}
#define FOLD_MOD 0775


//const string g_rootPath = "D:\\QTDebug\\";
//const string g_rootPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).toStdString() + "/QTDebug/";
const string g_rootPath = QDir::currentPath().toStdString() + "/QTDebug/";


enum DFLP_FOLDER
{
    F_CONFIG = 0,
    F_INFO,
    F_LOGS,
    F_DATA,
    F_DUMPS,
};
                         //   0           1      2       3        4
const string g_folder[] = {"Configs/", "Info/", "Logs/", "Data/", "Dumps/"};

const string g_configFile = "config.ini";
const string g_configGrpKey[] = {"DateTimeConfig", "IPConfig", "AutoCalib"};
const string g_configKey[] = {"DateTime", "LanIP", "IPMask"};


void AppConfig::initGlobalFolder()
{
    int nSize = 0;
    GET_ARRAY_LEN(g_folder, nSize);

    makeDir(g_rootPath);

    for (int i = 0; i < nSize; i++)
    {
        string folder = g_rootPath + g_folder[i];
        makeDir(folder);
    }
}

int AppConfig::makeDir(const std::string& path)
{
    int nRet = -1;
    QDir dir(QString::fromStdString(path));

    if (!dir.exists())
    {
        if(!dir.mkdir(QString::fromStdString(path)))
        {
            LOGE("mkdir fold{} error!\n", path.c_str());
        }
    }

    return nRet;
}

string AppConfig::getRootFolder()
{
    return g_rootPath;
}

string AppConfig::getConfigFolder()
{
    return g_rootPath + g_folder[F_CONFIG];
}

string AppConfig::getLogFolder()
{
    return g_rootPath + g_folder[F_LOGS];
}

string AppConfig::getDumpFolder()
{
    return g_rootPath + g_folder[F_DUMPS];
}

string AppConfig::getInfoFolder()
{
    return g_rootPath + g_folder[F_INFO];
}

string AppConfig::getDataFolder()
{
    return g_rootPath + g_folder[F_DATA];
}


string AppConfig::createDataFolder()
{
    std::string curPath = getDataFolder();
    SYSTEMTIME sysTime;
    char dateStr[50] = {0};
    GetLocalTime(&sysTime);
    sprintf(dateStr, "%4d_%02d_%02d_%02d_%02d_%02d_%03d/",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds);
    curPath += dateStr;
    makeDir(curPath);
    return curPath;
}

QString AppConfig::createDataFolder(const QString &current_date,const QString &DeviceInfo,const QString &ChipInfo,const QString& RunNum)
{
    QString curPath = QString::fromStdString(getDataFolder());
    curPath += current_date+ DeviceInfo + ChipInfo + RunNum;
    makeDir(curPath.toStdString());
    return curPath;
}


QString AppConfig::readConfigDateTime()
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[0]));
    QString sValue = set->value(QString::fromStdString(g_configKey[0])).toString();
    set->endGroup();

    return sValue;
}

QString AppConfig::readConfigIP()
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[1]));
    QString sValue = set->value(QString::fromStdString(g_configKey[1])).toString();
    set->endGroup();

    return sValue;
}

QString AppConfig::readConfigIPMask()
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[1]));
    QString sValue = set->value(QString::fromStdString(g_configKey[2])).toString();
    set->endGroup();

    return sValue;
}

void AppConfig::writeConfigDateTime(const QString& sDateTime)
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[0]));
    set->setValue(QString::fromStdString(g_configKey[0]), sDateTime);
    set->endGroup();
}

void AppConfig::writeConfigIP(const QString& sIP)
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[1]));
    set->setValue(QString::fromStdString(g_configKey[1]), sIP);
    set->endGroup();
}

void AppConfig::writeConfigIPMask(const QString& sIP)
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[1]));
    set->setValue(QString::fromStdString(g_configKey[2]), sIP);
    set->endGroup();
}

bool AppConfig::getAutoCalib()
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[2]));
    int nValue = set->value(QString::fromStdString(g_configGrpKey[2])).toInt();
    set->endGroup();

    return nValue==0?false:true;
}

void AppConfig::writeAutoCalib(bool bAuto /*= false*/)
{
    QString fileName = QString("%1%2").arg(QString::fromStdString(getConfigFolder()),QString::fromStdString(g_configFile));
    QSettings *set = new QSettings(fileName, QSettings::IniFormat);
    set->beginGroup(QString::fromStdString(g_configGrpKey[2]));
    set->setValue(QString::fromStdString(g_configGrpKey[2]), bAuto?"1":"0");
    set->endGroup();
}

void AppConfig::initInstrumentTime()
{
    QDateTime datetime = QDateTime::fromString(readConfigDateTime(), "yyyy-MM-dd hh:mm:ss");
    if( QDateTime::currentDateTime() < datetime )
    {
        //set init time:
        std::string strDateTime = datetime.toString("yyyy-MM-dd hh:mm:ss").toStdString();
        CommonHelper::configSysDateTime(strDateTime);
    }
}

bool AppConfig::isPausedIfError()
{
    return false;
}


void AppConfig::syncDefaultConfig(const QString &iniName)
{
    QFile file(iniName);

    if (!file.exists()) { // 文件不存在
        qDebug()<<"file doesn't exist";
        QFileInfo fi(iniName);
        QString fullPath = fi.absolutePath();
        QString base = fi.baseName();

        qDebug()<<fullPath<<base;
        QString default_file =":/config/default_"+base+".ini";

        QDir dir(fullPath);
        if (!dir.exists()) { // 配置文件夹不存在就创建
            dir.mkpath(fullPath); // 创建多级目录
        }

        QFile::copy(default_file, iniName);
        file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
    }
}






