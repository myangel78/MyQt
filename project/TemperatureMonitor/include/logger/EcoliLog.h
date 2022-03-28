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

#define LOG_BY_THREAD

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

#ifndef LOG_BY_THREAD
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_TRACE, m_Logger->get_msg());
                }
#endif
            }
        }

        template <typename... Args>
        inline void LogD(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->debug(fmt, args...);
            if (m_Logger)
            {
                m_Logger->debug(fmt, args...);
#ifndef LOG_BY_THREAD
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_DEBUG, m_Logger->get_msg());
                }
#else
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_DEBUG, m_Logger->get_msg() });
                }
#endif
            }
        }

        template <typename... Args>
        inline void LogI(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->info(fmt, args...);
            if (m_Logger)
            {
                m_Logger->info(fmt, args...);
#ifndef LOG_BY_THREAD
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_INFO, m_Logger->get_msg());
                }
#else
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_INFO, m_Logger->get_msg() });
                }
#endif
            }
        }

        template <typename... Args>
        inline void LogW(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->warn(fmt, args...);
            if (m_Logger)
            {
                m_Logger->warn(fmt, args...);
#ifndef LOG_BY_THREAD
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_WARN, m_Logger->get_msg());
                }
#else
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_WARN, m_Logger->get_msg() });
                }
#endif
            }
        }

        template <typename... Args>
        inline void LogE(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->error(fmt, args...);
            if (m_Logger)
            {
                m_Logger->error(fmt, args...);
#ifndef LOG_BY_THREAD
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_ERROR, m_Logger->get_msg());
                }
#else
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_ERROR, m_Logger->get_msg() });
                }
#endif
            }
        }

        template <typename... Args>
        inline void LogC(const char* fmt, const Args&... args)
        {
            if (m_bEnColiToConsole && m_Console) m_Console->critical(fmt, args...);
            if (m_Logger)
            {
                m_Logger->critical(fmt, args...);
#ifndef LOG_BY_THREAD
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_CRITICAL, m_Logger->get_msg());
                }
#else
                if (m_bCallback)
                {
                    m_queLog.enqueue({ ECOLI_CRITICAL, m_Logger->get_msg() });
                }
#endif
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



    public:
        void InitEcoliLogger(bool cbk = false)
        {
            m_bCallback = cbk;
            try
            {
                std::string strlogpath = "etc/TemperLogs/";
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
#ifdef LOG_BY_THREAD
                m_queLog.enqueue({ ECOLI_CRITICAL, "Unable to save log to disk!" });
#endif
                m_Logger = nullptr;
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
        std::shared_ptr<spdlog::logger> m_Console = nullptr;

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
