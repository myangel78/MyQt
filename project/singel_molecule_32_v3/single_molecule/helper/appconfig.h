#ifndef _ECOLI_APPCONFIG_H
#define _ECOLI_APPCONFIG_H
#include <string>
using namespace std;
#include <QString>

class AppConfig
{
public:
    static void initGlobalFolder();
    static string getRootFolder();
    static string getDataFolder();
    static string getConfigFolder();
    static string getInfoFolder();
    static string getDumpFolder();
    static string getLogFolder();
    static string createDataFolder();
    static void syncDefaultConfig(const QString &iniName);
    static QString createDataFolder(const QString &current_date,const QString &DeviceInfo,\
                                    const QString &ChipInfo,const QString& RunNum);


public:
    static bool getAutoCalib();
    static void writeAutoCalib(bool bAuto = false);

public:
    static QString readConfigDateTime();
    static QString readConfigIP();
    static QString readConfigIPMask();

    static void initInstrumentTime();
    static void writeConfigDateTime(const QString& sDateTime);
    static void writeConfigIP(const QString& sIP);
    static void writeConfigIPMask(const QString& sIP);

public:
    static bool isPausedIfError();
    static int makeDir(const std::string& path);

private:
    static bool m_enableSensor;
};

#endif // _ECOLI_APPCONFIG_H
