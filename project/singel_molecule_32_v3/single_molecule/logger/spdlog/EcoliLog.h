#ifndef ECOLILOG_H_
#define ECOLILOG_H_
#include "spdlog.h"

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
        CEcoliLog();
        ~CEcoliLog();
    public:
        template <typename... Args>
        inline void LogT(const char* fmt, const Args&... args)
        {
            if (m_bEnableConsole && m_Console) m_Console->trace(fmt, args...);
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
            if (m_bEnableConsole && m_Console) m_Console->debug(fmt, args...);
            if (m_Logger)
            {
                m_Logger->debug(fmt, args...);
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_DEBUG, m_Logger->get_msg());
                }
            }
        }

        template <typename... Args>
        inline void LogI(const char* fmt, const Args&... args)
        {
            if (m_bEnableConsole && m_Console) m_Console->info(fmt, args...);
            if (m_Logger)
            {
                m_Logger->info(fmt, args...);
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_INFO, m_Logger->get_msg());
                }
            }
        }

        template <typename... Args>
        inline void LogW(const char* fmt, const Args&... args)
        {
            if (m_bEnableConsole && m_Console) m_Console->warn(fmt, args...);
            if (m_Logger)
            {
                m_Logger->warn(fmt, args...);
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_WARN, m_Logger->get_msg());
                }
            }
        }

        template <typename... Args>
        inline void LogE(const char* fmt, const Args&... args)
        {
            if (m_bEnableConsole && m_Console) m_Console->error(fmt, args...);
            if (m_Logger)
            {
                m_Logger->error(fmt, args...);
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_ERROR, m_Logger->get_msg());
                }
            }
        }

        template <typename... Args>
        inline void LogC(const char* fmt, const Args&... args)
        {
            if (m_bEnableConsole && m_Console) m_Console->critical(fmt, args...);
            if (m_Logger)
            {
                m_Logger->critical(fmt, args...);
                if (m_FuncCB)
                {
                    m_FuncCB(ECOLI_CRITICAL, m_Logger->get_msg());
                }
            }
        }

        inline void Flush()
        {
            if (m_Logger) m_Logger->flush();
        }

        void LogConsole(bool bEnable);

    public:
        static void Init();
        inline void SetLogCallback(FUNC_INT_CHARPTR func)
        {
            //rotating_file_sink
            m_FuncCB = func;
        }

    private:
        std::shared_ptr<spdlog::logger> m_Logger;
        std::shared_ptr<spdlog::logger> m_Console;
        bool m_bEnableConsole;

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
