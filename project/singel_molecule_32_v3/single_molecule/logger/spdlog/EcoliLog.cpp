#include "EcoliLog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <iostream>
#include "helper/appconfig.h"
#include"spdlog/spdlog.h"
//#include "controllers/src/version.h"

ecoli::CEcoliLog gEcoliLog;

ecoli::CEcoliLog::CEcoliLog():
    m_bEnableConsole(false), m_FuncCB(nullptr)
{
}

ecoli::CEcoliLog::~CEcoliLog()
{
}

void ecoli::CEcoliLog::Init()
{
    try
    {
        gEcoliLog.m_Logger = spdlog::rotating_logger_mt("ecoli", AppConfig::getLogFolder()+"LOG", 1024 * 1024 * 50, 20);
        gEcoliLog.m_Logger->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l][%t] %v");
        gEcoliLog.m_Logger->set_level(spdlog::level::trace);
        gEcoliLog.m_Logger->flush_on(spdlog::level::trace);
      //  string version, versionDate;
      //  queryQTVersion(version, versionDate);
     //   gEcoliLog.m_Logger->info("ecoli " + version + " " + versionDate);
    }
    catch (...)
    {
        std::cout << "Unable to save log to disk!" << std::endl;
        gEcoliLog.m_Logger = nullptr;
    }
}

void ecoli::CEcoliLog::LogConsole(bool bEnable)
{
    m_bEnableConsole = bEnable;
    if (!m_Console)
    {
        m_Console = spdlog::stdout_color_mt("console");
        m_Console->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l][%t] %v");
        m_Console->set_level(spdlog::level::trace);
    }
}
