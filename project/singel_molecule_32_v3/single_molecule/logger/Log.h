#ifndef LOG_H_
#define LOG_H_
#include "spdlog/EcoliLog.h"

extern ecoli::CEcoliLog gEcoliLog;

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
#define SETLOGCB(func) gEcoliLog.SetLogCallback(func)

/* //e.g
int i = 10;
float f = 1.5;
double d = 1.8
LOGI("message {:2d}, {}", i, f);
LOGE("message {}, {}", i, d);
LOGW("message {}", (void*)(size_t)(i)); //print hex
*/
#define LOGT(...) gEcoliLog.LogT(__VA_ARGS__)//!<trace
//#define LOGD(...) gEcoliLog.LogD(__VA_ARGS__)//!<debug
#define LOGI(...) gEcoliLog.LogI(__VA_ARGS__)//!<info
#define LOGW(...) gEcoliLog.LogW(__VA_ARGS__)//!<warn
#define LOGE(...) gEcoliLog.LogE(__VA_ARGS__)//!<error
//#define LOGC(...) gEcoliLog.LogC(__VA_ARGS__)//!<critical

//print to console. default:disable
#define LOGCONSOLE(enable) gEcoliLog.LogConsole((bool)enable);
//flush to disk
#define LOGFLUSH() gEcoliLog.Flush()

#endif//LOG_H_
