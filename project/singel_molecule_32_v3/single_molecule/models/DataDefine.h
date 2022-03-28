#ifndef DATADEFINE_H
#define DATADEFINE_H

#include <QByteArray>
#include <QObject>
#include <QColor>
#include <QDebug>
#include <QThread>
#include <QSettings>
#include<vector>
#include"helper/EnumString.h"
#define g_pCommunicaor DataCommunicaor::Instance()
//#define DEMO_TEST

#define CHANNEL_NUM 32
//#define CHANNEL_NUM 256

#if(CHANNEL_NUM == 256)
    constexpr int FRAME_GROUP_NUM = 4;
#else if(CHANNEL_NUM == 32)
    constexpr int FRAME_GROUP_NUM = 7;
#endif


#define PROGRAME_PASSWORD "123456"

constexpr double DACMAX_TWOPOINTFINE = 2.5;
constexpr double DACMAX_FIVEVOLT = 5;
constexpr int DACMAX_BITRANGE_SIXTEEN = 65536;
constexpr int DACMAX_HALF_BITRANGE_SIXTEEN = 32768;
constexpr int CALIBRAION_STEP_FRAME_COUNT = 10000;
constexpr double ZERO_VOLTAGE = 0.0;
constexpr int MAX_POINTS_FOR_DEGATING = 20000;
constexpr int MAX_POINTS_FOR_CAPPOINTS = 5;



constexpr int XRANGE = 2;
constexpr double XRANGE_S = XRANGE - 0.0000001;


constexpr double REV_fREQ_K = 12.5;
constexpr int PLOT_REFRESH_INTREVAL_MS = 50;
constexpr int PLOT_BUF_BLOCK_CNT = 5;


constexpr int DEGATE_FRAMES_SIZE = CHANNEL_NUM;
constexpr int DEGATE_BUF_BLOCK_CNT = 2;
constexpr int DEGATE_BUF_BLOCK_END_INDEX = DEGATE_BUF_BLOCK_CNT - 1;
constexpr int DEGATE_BUF_BLOCK_LENGTH = DEGATE_FRAMES_SIZE * DEGATE_BUF_BLOCK_CNT;

constexpr int MEANSCAL_FRAMES_SIZE = CHANNEL_NUM;
constexpr int MEANSCAL_BUF_BLOCK_CNT = 2;
constexpr int MEANSCAL_BUF_BLOCK_END_INDEX = MEANSCAL_BUF_BLOCK_CNT - 1;
constexpr int MEANSCAL_BUF_BLOCK_LENGTH = MEANSCAL_FRAMES_SIZE * MEANSCAL_BUF_BLOCK_CNT;

constexpr int MEANSCAP_FRAMES_SIZE = CHANNEL_NUM;
constexpr int MEANSCAP_BUF_BLOCK_CNT = 5;
constexpr int MEANSCAP_BUF_BLOCK_END_INDEX = MEANSCAP_BUF_BLOCK_CNT - 1;
constexpr int MEANSCAP_BUF_BLOCK_LENGTH = MEANSCAP_FRAMES_SIZE * MEANSCAP_BUF_BLOCK_CNT;

constexpr int MEANSCAPRES_BUF_BLOCK_CNT = 5;
constexpr int MEANSCAPRES_BUF_BLOCK_END_INDEX = MEANSCAPRES_BUF_BLOCK_CNT - 1;
constexpr int MEANSCAPRES_BUF_BLOCK_LENGTH = MEANSCAP_FRAMES_SIZE * MEANSCAPRES_BUF_BLOCK_CNT;

#ifndef DEMO_TEST
    constexpr int RAW_BUF_BLOCK_LEN =  REV_fREQ_K * PLOT_REFRESH_INTREVAL_MS * PLOT_BUF_BLOCK_CNT;
#else
    constexpr int RAW_BUF_BLOCK_LEN = 1;
#endif


constexpr int RAW_BUF_BLOCK_END_INDEX = RAW_BUF_BLOCK_LEN - 1;

constexpr int SAVE_FRAMES_SIZE = CHANNEL_NUM;
constexpr int SAVE_BUF_BLOCK_LEN = 50;
constexpr int SAVE_BUF_BLOCK_END_INDEX = SAVE_BUF_BLOCK_LEN - 1;
constexpr int SAVE_BUF_BLOCK_LENGTH = SAVE_FRAMES_SIZE * SAVE_BUF_BLOCK_LEN;

constexpr int CH_BUF_BLOCK_LEN = RAW_BUF_BLOCK_LEN * FRAME_GROUP_NUM;
constexpr int CH_BUF_BLOCK_END_INDEX = CH_BUF_BLOCK_LEN - 1;

constexpr int PLOT_FRAMES_SIZE = CHANNEL_NUM;
constexpr int PLOT_BUF_BLOCK_END_INDEX = PLOT_BUF_BLOCK_CNT - 1;
constexpr int PLOT_BUF_BLOCK_LENGTH = PLOT_FRAMES_SIZE * PLOT_BUF_BLOCK_CNT;

constexpr int CHANNEL_NUM_END_INDEX = CHANNEL_NUM - 1;
constexpr int FRAME_DATA_TOTAL_CNT = CHANNEL_NUM * FRAME_GROUP_NUM;
constexpr int FRAME_BYTE_CNT = FRAME_DATA_TOTAL_CNT * 2;

constexpr int SEM_TRY_TIME = 3;
constexpr int SEM_TIMEOUT = 1000;
constexpr int PLOT_SEM_TIMEOUT = 500;
constexpr int RAW_SEM_TIMEOUT = 100;
constexpr int DISP_DUTY_TIME = 100;
constexpr int MEANS_DUTY_TIME = 1100;

constexpr qint64 RECORDING_FILE_SIZE_MB = 1024*1024;
constexpr qint64 RECORDING_FILE_LIMIT_SIZE = RECORDING_FILE_SIZE_MB * 100;

constexpr double  DAC_REFVOLTAGE =  2.5;

typedef enum __tagStatus
{
    SERIAL_OPEN_SUCCESS = 0,
    SERIAL_OPEN_FAIL,
    SERIAL_OPEN_ALREADY_EXIST,
    SERIAL_WRITE_CMD_ILLEGAL,
    SERIAL_WRITE_CMD_FAIL,
    SERIAL_WRITE_CMD_PASS,
    SERIAL_READ_ALREADY_START,
    SERIAL_READ_TIMEOUT,
    SERIAL_RESOURCE_ERROR,
    LAST_THREAD_NOT_STOP,


    THREAD_NORMAL_EXIT,
    THREAD_RAW_FREE_SEM_ACQ_TIMEOUT,
    THREAD_RAW_USED_SEM_ACQ_TIMEOUT,
    THREAD_CHANNEL_FREE_SEM_ACQ_TIMEOUT,
    THREAD_CHANNEL_USED_SEM_ACQ_TIMEOUT,
    THREAD_SAVE_FREE_SEM_ACQ_TIMEOUT,
    THREAD_SAVE_USED_SEM_ACQ_TIMEOUT,
    THREAD_OUTPUT_FREE_SEM_ACQ_TIMEOUT,
    THREAD_OUTPUT_USED_SEM_ACQ_TIMEOUT,
    THREAD_PLOT_FREE_SEM_ACQ_TIMEOUT,
    THREAD_PLOT_USED_SEM_ACQ_TIMEOUT,


    FILE_WRITE_ERROR,
    FILE_OPEN_ERROR,
    FILE_REOPEN_ERROR,
}ENUM_STATUS;


Begin_Enum_String(ENUM_STATUS)
{
    Enum_String(SERIAL_OPEN_SUCCESS);
    Enum_String(SERIAL_OPEN_FAIL);
    Enum_String(SERIAL_OPEN_ALREADY_EXIST);
    Enum_String(SERIAL_WRITE_CMD_ILLEGAL);
    Enum_String(SERIAL_WRITE_CMD_FAIL);
    Enum_String(SERIAL_WRITE_CMD_PASS);
    Enum_String(SERIAL_READ_ALREADY_START);
    Enum_String(SERIAL_READ_TIMEOUT);
    Enum_String(SERIAL_RESOURCE_ERROR);
    Enum_String(LAST_THREAD_NOT_STOP);


    Enum_String(THREAD_NORMAL_EXIT);       
    Enum_String(THREAD_RAW_FREE_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_RAW_USED_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_CHANNEL_FREE_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_CHANNEL_USED_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_SAVE_FREE_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_SAVE_USED_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_OUTPUT_FREE_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_OUTPUT_USED_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_PLOT_FREE_SEM_ACQ_TIMEOUT);
    Enum_String(THREAD_PLOT_USED_SEM_ACQ_TIMEOUT);


    Enum_String(FILE_WRITE_ERROR);
    Enum_String(FILE_OPEN_ERROR);
    Enum_String(FILE_REOPEN_ERROR);
}
End_Enum_String;

enum ENUM_NOTIFY_ID
{
    MAINWINDOW = 0,
};

enum ENUM_CMD_INDEX
{
    CMD_START = 0,
    CMD_STOP,
    CMD_END,
};

enum ENUM_DISP_CHANGE
{
    DISP_NO_CHANGE = 0,
    DISP_UPDATE,
};

enum ENUM_RECEIVE_MODE
{
    RECEIVE_NORAMAL = 0,
    RECEIVE_CALIBRATION,
};

enum ENUM_IS_ALLCHANNEL
{
    ALLCHANNEL = true,
    SINGELCHANNEL =false,
};

enum ENUM_ZERO_ADJUST
{
    ZERO_ADJUST_CANCEL = 0,
    ZERO_ADJUST_START = 1,
    ZERO_ADJUST_SET_PORE_BACKWARD = 2,
    ZERO_ADJUST_SET_ZERO_VOLT     = 3,

};

enum ENUM_CHANNEL_MODE
{
    SINGEL_CHANNEL_MODE = 0,
    ALL_CAHNNEL_MODE,
};

enum ENUM_VOLTAGE_TYPE
{
    DIRECT_CURRENT = 0,
    IMPULSE_VOLTAGE,
    SQUARE_VOLTAGE,
    TRIANGULAR_VOLTAGE,
};

enum PORE_STATE
{
    UN_HOLE_STATE = 0,
    SINGEL_HOLE_STATE,
    MULTDEM_HOLE_STATE,
};

enum GRAPH_VISIABLE_ENUM
{
    GRAPH_VISIABLE = true,
    GRAPH_NOT_VISIABLE = false,
};

enum DEGAT_VOLT_TYPE
{
    NORMAL = 0,
    DEGATTING,
};

const QString colorStyle[] {
   "background: white","background: green","background: red", "border:3px solid blue;"
};


const std::vector<QByteArray> CMD_LIST =
{
    QByteArray::fromHex("55aa0111ddbb"),
    QByteArray::fromHex("55aa0117ddbb"),
};

struct CalibrationVoltage
{
    CalibrationVoltage() : nBeginVoltVaule(0), nStepVoltValue(0),nMaxVoltValue(0)
    {
        nBoolCalib = false;
    }
    double nBeginVoltVaule;
    double nStepVoltValue;
    double nMaxVoltValue;
    bool nBoolCalib;
};


struct PlotSettingStruct
{
    PlotSettingStruct() : nYmax(0), nYmin(0),nTime(0),nChStart(0),nChEnd(0)
    {
    }
    double nYmax;
    double nYmin;
    double nTime;
    int nChStart;
    int nChEnd;
};

struct CalibrationStr
{
    CalibrationStr() : resistence(0), slope(0), offset(true),formula(""){}
    double resistence;
    double slope;
    double offset;
    QString formula;
};

const std::vector<QColor> vctPenColor =
{
    QColor(0, 0, 0),     QColor(202,235,216),    QColor(128,118,105),
    QColor(255,235,205), QColor(255,245,238),    QColor(255,0,0),
    QColor(227,23,13),   QColor(255,127,80),     QColor(255,192,203),
    QColor(116,0,0),     QColor(255,255,0),      QColor(227,207,87),
    QColor(255,125,64),  QColor(255,227,132),    QColor(85,102,0),
    QColor(0,255,0),     QColor(128,42,42),      QColor(0,255,255),
    QColor(199,97,20),   QColor(127,255,0),      QColor(210,180,140),
    QColor(8,46,84),     QColor(188,143,143),    QColor(34,139,34),
    QColor(107,142,35),  QColor(199,97,20),      QColor(0,0,255),
    QColor(160,32,240),  QColor(218,112,214),    QColor(25,25,112),
    QColor(138,43,226),  QColor(153,51,250)
};

const unsigned short CHANNEL_TRANSLATION_TABLE[CHANNEL_NUM] = {
    0,15,1,14,2,13,3,12,4,11,5,10,6,9,7,8,
    16,31,17,30,18,29,19,28,20,27,21,26,22,25,23,24,
};

#endif // DATADEFINE_H
