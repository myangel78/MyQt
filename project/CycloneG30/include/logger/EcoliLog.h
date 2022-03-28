#ifndef ECOLILOG_H_
#define ECOLILOG_H_
#include <iostream>
#include <filesystem>
#include <tuple>
#include <QDateTime>

#include "spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "logger/spdlog/fmt/bin_to_hex.h"

#include "concurrentqueue.h"
#include "ConfigServer.h"



enum EcoliQTLogLevel
{
    ECOLI_TRACE    = 1,
    ECOLI_DEBUG = ECOLI_TRACE<<1,
    ECOLI_INFO = ECOLI_DEBUG<<1,
    ECOLI_WARN = ECOLI_INFO<<1,
    ECOLI_ERROR = ECOLI_WARN<<1,
    ECOLI_CRITICAL = ECOLI_ERROR<<1,
};

namespace ecoli
{
    class CEcoliLog
    {
        typedef std::function<void(int, const char*)> FUNC_INT_CHARPTR;
    public:
        CEcoliLog()
            : m_bEnColiToConsole(false)
            , m_bEnLogToConsole(false)
            , m_FuncCB(nullptr)
        {
        }
        ~CEcoliLog() = default;
    public:
        template <typename... Args>
        inline void LogT(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->trace(fmt, args...);
            if (m_Logger)
            {
                m_Logger->trace(fmt, args...);
//                if (m_FuncCB)
//                {
//                    m_FuncCB(ECOLI_TRACE, m_Logger->get_msg());
//                }
            }
        }

        template <typename... Args>
        inline void LogD(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->debug(fmt, args...);
            if (m_Logger)
            {
                m_Logger->debug(fmt, args...);
//                if (m_FuncCB)
//                {
//                    m_FuncCB(ECOLI_DEBUG, m_Logger->get_msg());
//                }
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_DEBUG, m_Logger->get_msg() });
                }
            }
        }

        template <typename... Args>
        inline void LogI(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->info(fmt, args...);
            if (m_Logger)
            {
                m_Logger->info(fmt, args...);
//                if (m_FuncCB)
//                {
//                    m_FuncCB(ECOLI_INFO, m_Logger->get_msg());
//                }
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_INFO, m_Logger->get_msg() });
                }
            }
        }

        template <typename... Args>
        inline void LogW(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->warn(fmt, args...);
            if (m_Logger)
            {
                m_Logger->warn(fmt, args...);
//                if (m_FuncCB)
//                {
//                    m_FuncCB(ECOLI_WARN, m_Logger->get_msg());
//                }
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_WARN, m_Logger->get_msg() });
                }
            }
        }

        template <typename... Args>
        inline void LogE(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->error(fmt, args...);
            if (m_Logger)
            {
                m_Logger->error(fmt, args...);
//                if (m_FuncCB)
//                {
//                    m_FuncCB(ECOLI_ERROR, m_Logger->get_msg());
//                }
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_ERROR, m_Logger->get_msg() });
                }
            }
        }

        template <typename... Args>
        inline void LogRunI(const char* fmt, const Args&... args)
        {
            if (m_LoggerRun)
            {
                m_LoggerRun->info(fmt, args...);
            }
        }

        template <typename... Args>
        inline void LogC(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->critical(fmt, args...);
            if (m_Logger)
            {
                m_Logger->critical(fmt, args...);
//                if (m_FuncCB)
//                {
//                    m_FuncCB(ECOLI_CRITICAL, m_Logger->get_msg());
//                }
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_CRITICAL, m_Logger->get_msg() });
                }
            }
        }
        inline void Flush()
        {
            if (m_Logger) m_Logger->flush();
        }


        template <typename... Args>
        inline void LogConsT(const char* fmt, const Args&... args)
        {
            if (m_bEnLogToConsole && m_Console) m_Console->trace(fmt, args...);
        }

        template <typename... Args>
        inline void LogConsD(const char* fmt, const Args&... args)
        {
            if (m_bEnLogToConsole && m_Console) m_Console->debug(fmt, args...);
        }

        template <typename... Args>
        inline void LogConsI(const char* fmt, const Args&... args)
        {
            if (m_bEnLogToConsole && m_Console) m_Console->info(fmt, args...);
        }

        template <typename... Args>
        inline void LogConsW(const char* fmt, const Args&... args)
        {
            if (m_bEnLogToConsole && m_Console) m_Console->warn(fmt, args...);
        }

        template <typename... Args>
        inline void LogConsE(const char* fmt, const Args&... args)
        {
            if (m_bEnLogToConsole && m_Console) m_Console->error(fmt, args...);
        }

        template <typename... Args>
        inline void LogConsC(const char* fmt, const Args&... args)
        {
            if (m_bEnLogToConsole && m_Console) m_Console->critical(fmt, args...);
        }

//====================================================================================
        template <typename... Args>
        inline void LogDegDataI(const char* fmt, const Args&... args)
        {
            if (m_DegDataLogger) m_DegDataLogger->info(fmt, args...);
        }

//====================================================================================
        template <typename... Args>
        inline void LogDegateT(const char* fmt, const Args&... args)
        {
            if (m_DegateLogger) m_DegateLogger->trace(fmt, args...);
        }

        template <typename... Args>
        inline void LogDegateD(const char* fmt, const Args&... args)
        {
            if (m_DegateLogger) m_DegateLogger->debug(fmt, args...);
        }

        template <typename... Args>
        inline void LogDegateI(const char* fmt, const Args&... args)
        {
            if (m_DegateLogger) m_DegateLogger->info(fmt, args...);
        }

        template <typename... Args>
        inline void LogDegateW(const char* fmt, const Args&... args)
        {
            if (m_DegateLogger) m_DegateLogger->warn(fmt, args...);
        }

        template <typename... Args>
        inline void LogDegateE(const char* fmt, const Args&... args)
        {
            if (m_DegateLogger) m_DegateLogger->error(fmt, args...);
        }

        template <typename... Args>
        inline void LogDegateC(const char* fmt, const Args&... args)
        {
            if (m_DegateLogger) m_DegateLogger->critical(fmt, args...);
        }

        inline void LogDegatFlush()
        {
            if (m_DegateLogger) m_DegateLogger->flush();
        }

    public:
        void InitEcoliLogger(bool cbk = false)
        {
            m_bCallback = cbk;
            try
            {
                std::string strlogpath = "etc/Logs/";
                auto logpah = std::filesystem::path(strlogpath);
                if (!std::filesystem::exists(logpah))
                {
                    std::filesystem::create_directories(logpah);
                }

                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("yyyyMMddhhmmss");
                m_Logger = spdlog::rotating_logger_mt("ecoli", strlogpath + "LOG_"+current_date.toStdString() + ".log", 1024 * 1024 * 50, 20);
                m_Logger->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%n][%^%l%$] %v");
//                m_Logger->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l][%t] %v");
                m_Logger->set_level(spdlog::level::trace);
                m_Logger->flush_on(spdlog::level::trace);
            }
            catch (...)
            {
                //std::cout << "Unable to save log to disk!" << std::endl;
                m_queLog.enqueue({ ECOLI_CRITICAL, "Unable to save log to disk!" });
                m_Logger = nullptr;
            }
        }

        void InitRunLogger(void)
        {
            try
            {
                std::string strlogpath = "etc/Runlogs/";
                auto logpah = std::filesystem::path(strlogpath);
                if (!std::filesystem::exists(logpah))
                {
                    std::filesystem::create_directories(logpah);
                }
                QDateTime current_date_time = QDateTime::currentDateTime();
                QString current_date = current_date_time.toString("yyyyMMddhhmmss");
                m_LoggerRun = spdlog::rotating_logger_mt("Rundeb", strlogpath + "LOG_" + current_date.toStdString() + ".log", 1024 * 1024 * 50, 20);
                m_LoggerRun->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l][%t] %v");
                m_LoggerRun->set_level(spdlog::level::trace);
                m_LoggerRun->flush_on(spdlog::level::trace);
            }
            catch (...)
            {
                m_LoggerRun = nullptr;
            }
        }

        void InitConsoleLogger(bool cbk = false)
        {
           if(cbk)
           {
               if (!m_Console)
               {
                   m_Console = spdlog::stdout_color_mt("console");
                   m_Console->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%^%l%$] %v");
                   m_Console->set_level(spdlog::level::trace);
               }
           }
           else
           {
               spdlog::drop("console");
               m_Console = nullptr;
           }
        }

        inline void EnaLogToConsole(const bool &chk){m_bEnLogToConsole = chk;}
        inline void EnaLogColiToConsole(const bool &chk){m_bEnColiToConsole = chk;}

        void InitDegDataLogger(void)
        {
            try
            {
                QDateTime current_date_time =QDateTime::currentDateTime();
                std::string current_date =current_date_time.toString("yyyyMMddhhmmss").toStdString();
                std::string strlogpath = "etc/DegDataLogs/";
                auto logpah = std::filesystem::path(strlogpath);
                if (!std::filesystem::exists(logpah))
                {
                    std::filesystem::create_directories(logpah);
                }
                m_DegDataLogger = spdlog::rotating_logger_mt("DegData", strlogpath + "LOG_"+current_date + ".log", 1024 * 1024 * 50, 20);
                m_DegDataLogger->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%n][%^%l%$] %v");
                m_DegDataLogger->set_level(spdlog::level::trace);
                m_DegDataLogger->flush_on(spdlog::level::trace);
            }
            catch (...)
            {
                m_DegDataLogger = nullptr;
            }
        }

        void UninitDegDataLogger(void)
        {
            spdlog::drop("DegData");
            m_DegDataLogger = nullptr;
        }

        void InitDegateLogger(void)
        {
            try
            {
                QDateTime current_date_time =QDateTime::currentDateTime();
                std::string current_date =current_date_time.toString("yyyyMMddhhmmss").toStdString();
                std::string strlogpath = ConfigServer::GetLogPath(LOG_DEGATE_FILE) + current_date+ "/";
                auto logpah = std::filesystem::path(strlogpath);
                if (!std::filesystem::exists(logpah))
                {
                    std::filesystem::create_directories(logpah);
                }

                m_DegateLogger = spdlog::rotating_logger_mt("DegateLog", strlogpath + "LOG_"+current_date + ".log", 1024 * 1024 * 50, 20);
                m_DegateLogger->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%n][%^%l%$] %v");
//                m_DegateLogger->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l][%t] %v");
                m_DegateLogger->set_level(spdlog::level::trace);
                m_DegateLogger->flush_on(spdlog::level::trace);
            }
            catch (...)
            {
                //std::cout << "Unable to save log to disk!" << std::endl;
                m_queLog.enqueue({ ECOLI_CRITICAL, "Unable to save degate log to disk!" });
                m_DegateLogger = nullptr;
            }
        }

        void DeleteDegateLogger(void)
        {
            spdlog::drop("DegateLog");
            m_DegateLogger = nullptr;
        }

        inline void SetLogCallback(FUNC_INT_CHARPTR func)
        {
            //rotating_file_sink
            m_FuncCB = func;
        }

        void SetCallback(bool cbk) { m_bCallback = cbk; }

        size_t GetLogLength(void) const { return m_queLog.size_approx(); }
        bool GetLogMsg(std::tuple<int, std::string>& msg)
        {
            return  m_queLog.try_dequeue(msg);
        }

        static CEcoliLog* GetInstance(void)
        {
            static std::once_flag of;
            std::call_once(of, [&]() {
                m_instance = new CEcoliLog;
            });
            return m_instance;
        }

    private:
        static CEcoliLog* m_instance;

        bool m_bEnColiToConsole;
        bool m_bEnLogToConsole;
        bool m_bCallback = false;
        moodycamel::ConcurrentQueue<std::tuple<int, std::string>> m_queLog;

        std::shared_ptr<spdlog::logger> m_Logger = nullptr;
        std::shared_ptr<spdlog::logger> m_LoggerRun = nullptr;
        std::shared_ptr<spdlog::logger> m_Console = nullptr;
        std::shared_ptr<spdlog::logger> m_DegDataLogger = nullptr;
        std::shared_ptr<spdlog::logger> m_DegateLogger = nullptr;

        /*m_FuncCB: void (int, char*)
        @param int: logLevel:
            trace = 0,
            debug = 1,
            info = 2,
            warn = 3,
            err = 4,
            critical = 5,

        @param char*: log msg
         */
        FUNC_INT_CHARPTR m_FuncCB;
    };
}
#endif//ECOLILOG_H_
