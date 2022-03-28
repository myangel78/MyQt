#ifndef LOG_H_
#define LOG_H_
#include "EcoliLog.h"

//extern ecoli::CEcoliLog gEcoliLog;

enum EcoliQTInfoType
{
    ECOLI_MSG,
    ECOLI_LOG,
};

/*func: void (int, const char*)
@param int: logLevel:
    trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    err = 4,
    critical = 5,

@param char*: log msg
 */
#define SETLOGCB(func) ecoli::CEcoliLog::GetInstance()->SetLogCallback(func)

/* //e.g
int i = 10;
float f = 1.5;
double d = 1.8
LOGI("message {:2d}, {}", i, f);
LOGE("message {}, {}", i, d);
LOGW("message {}", (void*)(size_t)(i)); //print hex
*/
#define LOGT(...) ecoli::CEcoliLog::GetInstance()->LogT(__VA_ARGS__)//!<trace
//#define LOGD(...) gEcoliLog.LogD(__VA_ARGS__)//!<debug
#define LOGI(...) ecoli::CEcoliLog::GetInstance()->LogI(__VA_ARGS__)//!<info
#define LOGW(...) ecoli::CEcoliLog::GetInstance()->LogW(__VA_ARGS__)//!<warn
#define LOGE(...) ecoli::CEcoliLog::GetInstance()->LogE(__VA_ARGS__)//!<error
//#define LOGC(...) gEcoliLog.LogC(__VA_ARGS__)//!<critical

#define LOGRUNI(...) ecoli::CEcoliLog::GetInstance()->LogRunI(__VA_ARGS__)//!<info

#define LOGCT(...) ecoli::CEcoliLog::GetInstance()->LogConsT(__VA_ARGS__)//!<trace
#define LOGCD(...) ecoli::CEcoliLog::GetInstance()->LogConsD(__VA_ARGS__)//!<debug
#define LOGCI(...) ecoli::CEcoliLog::GetInstance()->LogConsI(__VA_ARGS__)//!<info
#define LOGCW(...) ecoli::CEcoliLog::GetInstance()->LogConsW(__VA_ARGS__)//!<warn
#define LOGCE(...) ecoli::CEcoliLog::GetInstance()->LogConsE(__VA_ARGS__)//!<error
#define LOGCC(...) ecoli::CEcoliLog::GetInstance()->LogConsC(__VA_ARGS__)//!<critical

#define LOGDEGDATAI(...) ecoli::CEcoliLog::GetInstance()->LogDegDataI(__VA_ARGS__)//!<info

#define LOGDET(...) ecoli::CEcoliLog::GetInstance()->LogDegateT(__VA_ARGS__)//!<trace
#define LOGDED(...) ecoli::CEcoliLog::GetInstance()->LogDegateD(__VA_ARGS__)//!<debug
#define LOGDEI(...) ecoli::CEcoliLog::GetInstance()->LogDegateI(__VA_ARGS__)//!<info
#define LOGDEW(...) ecoli::CEcoliLog::GetInstance()->LogDegateW(__VA_ARGS__)//!<warn
#define LOGDEE(...) ecoli::CEcoliLog::GetInstance()->LogDegateE(__VA_ARGS__)//!<error
#define LOGDEC(...) ecoli::CEcoliLog::GetInstance()->LogDegateC(__VA_ARGS__)//!<critical
//flush to disk
#define LOGDGATEFLUSH() ecoli::CEcoliLog::GetInstance()->LogDegatFlush()

//print to console. default:disable
#define LOGCONLITOCONSOLE(enable) ecoli::CEcoliLog::GetInstance()->EnaLogColiToConsole((bool)enable);
//flush to disk
#define LOGFLUSH() ecoli::CEcoliLog::GetInstance()->Flush()

#endif//LOG_H_
