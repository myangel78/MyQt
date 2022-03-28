#pragma once

#include <atomic>
#include <qcolor.h>
#include <qsettings.h>
#include <qdatetime.h>
#include <qcoreapplication.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <regex>

#include <concurrentqueue.h>

//#include <Point.h>

//#pragma execution_character_set("utf-8")


//channel all number //1024;// 
constexpr int CHANNEL_NUM = 256;

constexpr int DEFAULT_SAMPLE_RATE = 5000;
constexpr int SAMPLE_RATE_5K = 5000;
constexpr int SAMPLE_RATE_20K = 20000;


//zero double
constexpr double ZERODOUBLE = 0.00000001;

#define ISDOUBLEZERO(xx) ((xx < ZERODOUBLE) && (xx > -ZERODOUBLE))

#define SAFE_DELETEP(xx) if(xx){delete xx;xx=nullptr;}
#define SAFE_DELETEA(xx) if(xx){delete[] xx;xx=nullptr;}


#define PROGRAME_PASSWORD "lab256"



constexpr int REAL_CHANNEL[CHANNEL_NUM] = {
	13,6,12,7,15,4,14,5,9,2,8,3,11,0,10,1
	,29,22,28,23,31,20,30,21,25,18,24,19,27,16,26,17
	,45,38,44,39,47,36,46,37,41,34,40,35,43,32,42,33
	,61,54,60,55,63,52,62,53,57,50,56,51,59,48,58,49
	,77,70,76,71,79,68,78,69,73,66,72,67,75,64,74,65
	,93,86,92,87,95,84,94,85,89,82,88,83,91,80,90,81
	,109,102,108,103,111,100,110,101,105,98,104,99,107,96,106,97
	,125,118,124,119,127,116,126,117,121,114,120,115,123,112,122,113
	,141,134,140,135,143,132,142,133,137,130,136,131,139,128,138,129
	,157,150,156,151,159,148,158,149,153,146,152,147,155,144,154,145
	,173,166,172,167,175,164,174,165,169,162,168,163,171,160,170,161
	,189,182,188,183,191,180,190,181,185,178,184,179,187,176,186,177
	,205,198,204,199,207,196,206,197,201,194,200,195,203,192,202,193
	,221,214,220,215,223,212,222,213,217,210,216,211,219,208,218,209
	,237,230,236,231,239,228,238,229,233,226,232,227,235,224,234,225
	,253,246,252,247,255,244,254,245,249,242,248,243,251,240,250,241
};

constexpr int REAL_CHANNEL2[CHANNEL_NUM] = {
   12,13,14,15,8,9,10,11,7,6,5,4,3,2,1,0,
   28,29,30,31,24,25,26,27,23,22,21,20,19,18,17,16,
   44,45,46,47,40,41,42,43,39,38,37,36,35,34,33,32,
   60,61,62,63,56,57,58,59,55,54,53,52,51,50,49,48,
   76,77,78,79,72,73,74,75,71,70,69,68,67,66,65,64,
   92,93,94,95,88,89,90,91,87,86,85,84,83,82,81,80,
   108,109,110,111,104,105,106,107,103,102,101,100,99,98,97,96,
   124,125,126,127,120,121,122,123,119,118,117,116,115,114,113,112,
   140,141,142,143,136,137,138,139,135,134,133,132,131,130,129,128,
   156,157,158,159,152,153,154,155,151,150,149,148,147,146,145,144,
   172,173,174,175,168,169,170,171,167,166,165,164,163,162,161,160,
   188,189,190,191,184,185,186,187,183,182,181,180,179,178,177,176,
   204,205,206,207,200,201,202,203,199,198,197,196,195,194,193,192,
   220,221,222,223,216,217,218,219,215,214,213,212,211,210,209,208,
   236,237,238,239,232,233,234,235,231,230,229,228,227,226,225,224,
   252,253,254,255,248,249,250,251,247,246,245,244,243,242,241,240
};


constexpr int COORDINATE_CHANNEL[CHANNEL_NUM][3] = {
	{89, 1, 0}, {87, 2, 0}, {81, 3, 0}, {100, 4, 0}, {130, 5, 0}, {136, 6, 0}, {138, 7, 0}
	, {93, 1, 1}, {102, 2, 1}, {85, 3, 1}, {83, 4, 1}, {179, 5, 1}, {132, 6, 1}, {134, 7, 1}, {181, 8, 1}, {142, 9, 1}
	, {110, 0, 2}, {112, 1, 2}, {108, 2, 2}, {104, 3, 2}, {98, 4, 2}, {183, 5, 2}, {187, 6, 2}, {191, 7, 2}, {189, 8, 2}
	, {96, 1, 3}, {95, 2, 3}, {111, 3, 3}, {109, 4, 3}, {177, 5, 3}, {190, 6, 3}, {192, 7, 3}, {144, 8, 3}, {143, 9, 3}
	, {92, 0, 4}, {94, 1, 4}, {107, 2, 4}, {105, 3, 4}, {106, 4, 4}, {186, 5, 4}, {188, 6, 4}, {141, 7, 4}, {139, 8, 4}
	, {88, 1, 5}, {90, 2, 5}, {103, 3, 5}, {101, 4, 5}, {185, 5, 5}, {182, 6, 5}, {184, 7, 5}, {137, 8, 5}, {135, 9, 5}
	, {84, 0, 6}, {86, 1, 6}, {99, 2, 6}, {97, 3, 6}, {91, 4, 6}, {178, 5, 6}, {180, 6, 6}, {133, 7, 6}, {131, 8, 6}
	, {65, 1, 7}, {82, 2, 7}, {114, 3, 7}, {116, 4, 7}, {140, 5, 7}, {163, 6, 7}, {161, 7, 7}, {129, 8, 7}, {146, 9, 7}
	, {69, 0, 8}, {67, 1, 8}, {118, 2, 8}, {120, 3, 8}, {36, 4, 8}, {167, 5, 8}, {165, 6, 8}, {148, 7, 8}, {150, 8, 8}
	, {73, 1, 9}, {71, 2, 9}, {122, 3, 9}, {124, 4, 9}, {243, 5, 9}, {171, 6, 9}, {169, 7, 9}, {152, 8, 9}, {154, 9, 9}
	, {77, 0, 10}, {75, 1, 10}, {126, 2, 10}, {128, 3, 10}, {17, 4, 10}, {175, 5, 10}, {173, 6, 10}, {156, 7, 10}, {158, 8, 10}
	, {80, 1, 11}, {79, 2, 11}, {127, 3, 11}, {125, 4, 11}, {194, 5, 11}, {174, 6, 11}, {176, 7, 11}, {160, 8, 11}, {159, 9, 11}
	, {76, 0, 12}, {78, 1, 12}, {123, 2, 12}, {121, 3, 12}, {66, 4, 12}, {170, 5, 12}, {172, 6, 12}, {157, 7, 12}, {155, 8, 12}
	, {72, 1, 13}, {74, 2, 13}, {119, 3, 13}, {117, 4, 13}, {145, 5, 13}, {166, 6, 13}, {168, 7, 13}, {153, 8, 13}, {151, 9, 13}
	, {68, 0, 14}, {70, 1, 14}, {115, 2, 14}, {113, 3, 14}, {19, 4, 14}, {162, 5, 14}, {164, 6, 14}, {149, 7, 14}, {147, 8, 14}
	, {21, 1, 15}, {23, 2, 15}, {38, 3, 15}, {44, 4, 15}, {196, 5, 15}, {251, 6, 15}, {245, 7, 15}, {200, 8, 15}, {198, 9, 15}
	, {25, 0, 16}, {27, 1, 16}, {42, 2, 16}, {48, 3, 16}, {40, 4, 16}, {255, 5, 16}, {249, 6, 16}, {204, 7, 16}, {202, 8, 16}
	, {29, 1, 17}, {31, 2, 17}, {46, 3, 17}, {45, 4, 17}, {247, 5, 17}, {254, 6, 17}, {253, 7, 17}, {208, 8, 17}, {206, 9, 17}
	, {32, 0, 18}, {30, 1, 18}, {47, 2, 18}, {41, 3, 18}, {34, 4, 18}, {250, 5, 18}, {256, 6, 18}, {205, 7, 18}, {207, 8, 18}
	, {28, 1, 19}, {26, 2, 19}, {43, 3, 19}, {37, 4, 19}, {241, 5, 19}, {246, 6, 19}, {252, 7, 19}, {201, 8, 19}, {203, 9, 19}
	, {24, 0, 20}, {22, 1, 20}, {39, 2, 20}, {33, 3, 20}, {62, 4, 20}, {242, 5, 20}, {248, 6, 20}, {197, 7, 20}, {199, 8, 20}
	, {20, 1, 21}, {18, 2, 21}, {35, 3, 21}, {52, 4, 21}, {237, 5, 21}, {227, 6, 21}, {244, 7, 21}, {193, 8, 21}, {195, 9, 21}
	, {1, 0, 22}, {3, 1, 22}, {50, 2, 22}, {56, 3, 22}, {13, 4, 22}, {231, 5, 22}, {225, 6, 22}, {212, 7, 22}, {210, 8, 22}
	, {5, 1, 23}, {7, 2, 23}, {54, 3, 23}, {60, 4, 23}, {222, 5, 23}, {235, 6, 23}, {229, 7, 23}, {216, 8, 23}, {214, 9, 23}
	, {9, 0, 24}, {11, 1, 24}, {58, 2, 24}, {64, 3, 24}, {4, 4, 24}, {239, 5, 24}, {233, 6, 24}, {220, 7, 24}, {218, 8, 24}
	, {61, 1, 25}, {57, 2, 25}, {53, 3, 25}, {49, 4, 25}, {211, 5, 25}, {226, 6, 25}, {230, 7, 25}, {234, 8, 25}, {238, 9, 25}
	, {63, 0, 26}, {59, 1, 26}, {55, 2, 26}, {51, 3, 26}, {2, 4, 26}, {228, 5, 26}, {232, 6, 26}, {236, 7, 26}, {240, 8, 26}
	, {15, 1, 27}, {14, 2, 27}, {16, 3, 27}, {6, 4, 27}, {209, 5, 27}, {213, 6, 27}, {223, 7, 27}, {221, 8, 27}, {224, 9, 27}
	, {10, 2, 28}, {12, 3, 28}, {8, 4, 28}, {215, 5, 28}, {217, 6, 28}, {219, 7, 28}
};

constexpr int COORDINATE_CHANNEL2[CHANNEL_NUM][3] = {
	{90, 1, 0}, {95, 2, 0}, {94, 3, 0}, {104, 4, 0}, {135, 5, 0}, {134, 6, 0}, {131, 7, 0}
	, {92, 1, 1}, {101, 2, 1}, {96, 3, 1}, {93, 4, 1}, {189, 5, 1}, {136, 6, 1}, {133, 7, 1}, {192, 8, 1}, {129, 9, 1}
	, {97, 0, 2}, {98, 1, 2}, {100, 2, 2}, {102, 3, 2}, {103, 4, 2}, {191, 5, 2}, {185, 6, 2}, {187, 7, 2}, {188, 8, 2}
	, {82, 1, 3}, {91, 2, 3}, {107, 3, 3}, {108, 4, 3}, {190, 5, 3}, {177, 6, 3}, {178, 7, 3}, {130, 8, 3}, {139, 9, 3}
	, {84, 0, 4}, {81, 1, 4}, {105, 2, 4}, {106, 3, 4}, {99, 4, 4}, {179, 5, 4}, {180, 6, 4}, {140, 7, 4}, {137, 8, 4}
	, {86, 1, 5}, {83, 2, 5}, {111, 3, 5}, {112, 4, 5}, {186, 5, 5}, {181, 6, 5}, {182, 7, 5}, {138, 8, 5}, {143, 9, 5}
	, {88, 0, 6}, {85, 1, 6}, {109, 2, 6}, {110, 3, 6}, {89, 4, 6}, {183, 5, 6}, {184, 6, 6}, {144, 7, 6}, {141, 8, 6}
	, {78, 1, 7}, {87, 2, 7}, {119, 3, 7}, {120, 4, 7}, {132, 5, 7}, {173, 6, 7}, {174, 7, 7}, {142, 8, 7}, {151, 9, 7}
	, {80, 0, 8}, {77, 1, 8}, {117, 2, 8}, {118, 3, 8}, {40, 4, 8}, {175, 5, 8}, {176, 6, 8}, {152, 7, 8}, {149, 8, 8}
	, {74, 1, 9}, {79, 2, 9}, {115, 3, 9}, {116, 4, 9}, {253, 5, 9}, {169, 6, 9}, {170, 7, 9}, {150, 8, 9}, {147, 9, 9}
	, {76, 0, 10}, {73, 1, 10}, {113, 2, 10}, {114, 3, 10}, {30, 4, 10}, {171, 5, 10}, {172, 6, 10}, {148, 7, 10}, {145, 8, 10}
	, {66, 1, 11}, {75, 2, 11}, {123, 3, 11}, {124, 4, 11}, {199, 5, 11}, {161, 6, 11}, {162, 7, 11}, {146, 8, 11}, {155, 9, 11}
	, {68, 0, 12}, {65, 1, 12}, {121, 2, 12}, {122, 3, 12}, {71, 4, 12}, {163, 5, 12}, {164, 6, 12}, {156, 7, 12}, {153, 8, 12}
	, {70, 1, 13}, {67, 2, 13}, {127, 3, 13}, {128, 4, 13}, {158, 5, 13}, {165, 6, 13}, {166, 7, 13}, {154, 8, 13}, {159, 9, 13}
	, {72, 0, 14}, {69, 1, 14}, {125, 2, 14}, {126, 3, 14}, {29, 4, 14}, {167, 5, 14}, {168, 6, 14}, {160, 7, 14}, {157, 8, 14}
	, {32, 1, 15}, {31, 2, 15}, {37, 3, 15}, {36, 4, 15}, {200, 5, 15}, {249, 6, 15}, {256, 7, 15}, {198, 8, 15}, {197, 9, 15}
	, {26, 0, 16}, {25, 1, 16}, {35, 2, 16}, {34, 3, 16}, {38, 4, 16}, {251, 5, 16}, {250, 6, 16}, {196, 7, 16}, {195, 8, 16}
	, {28, 1, 17}, {27, 2, 17}, {33, 3, 17}, {44, 4, 17}, {255, 5, 17}, {241, 6, 17}, {252, 7, 17}, {194, 8, 17}, {193, 9, 17}
	, {18, 0, 18}, {17, 1, 18}, {43, 2, 18}, {42, 3, 18}, {39, 4, 18}, {243, 5, 18}, {242, 6, 18}, {204, 7, 18}, {203, 8, 18}
	, {20, 1, 19}, {19, 2, 19}, {41, 3, 19}, {48, 4, 19}, {254, 5, 19}, {245, 6, 19}, {244, 7, 19}, {202, 8, 19}, {201, 9, 19}
	, {22, 0, 20}, {21, 1, 20}, {47, 2, 20}, {46, 3, 20}, {49, 4, 20}, {247, 5, 20}, {246, 6, 20}, {208, 7, 20}, {207, 8, 20}
	, {24, 1, 21}, {23, 2, 21}, {45, 3, 21}, {56, 4, 21}, {236, 5, 21}, {237, 6, 21}, {248, 7, 21}, {206, 8, 21}, {205, 9, 21}
	, {14, 0, 22}, {13, 1, 22}, {55, 2, 22}, {54, 3, 22}, {12, 4, 22}, {239, 5, 22}, {238, 6, 22}, {216, 7, 22}, {215, 8, 22}
	, {16, 1, 23}, {15, 2, 23}, {53, 3, 23}, {52, 4, 23}, {209, 5, 23}, {233, 6, 23}, {240, 7, 23}, {214, 8, 23}, {213, 9, 23}
	, {10, 0, 24}, {9, 1, 24}, {51, 2, 24}, {50, 3, 24}, {8, 4, 24}, {235, 5, 24}, {234, 6, 24}, {212, 7, 24}, {211, 8, 24}
	, {60, 1, 25}, {58, 2, 25}, {64, 3, 25}, {62, 4, 25}, {221, 5, 25}, {231, 6, 25}, {229, 7, 25}, {227, 8, 25}, {225, 9, 25}
	, {59, 0, 26}, {57, 1, 26}, {63, 2, 26}, {61, 3, 26}, {7, 4, 26}, {232, 5, 26}, {230, 6, 26}, {228, 7, 26}, {226, 8, 26}
	, {11, 1, 27}, {1, 2, 27}, {2, 3, 27}, {5, 4, 27}, {222, 5, 27}, {224, 6, 27}, {219, 7, 27}, {220, 8, 27}, {210, 9, 27}
	, {3, 2, 28}, {4, 3, 28}, {6, 4, 28}, {223, 5, 28}, {218, 6, 28}, {217, 7, 28}
};

const std::vector<QColor> g_vetPenColor =
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


enum EEmbedPoreStatus
{
	EPS_NONEPORE,
	EPS_SINGLEPORE,
	EPS_MULTIPORE,
	EPS_SINGLEPORE_PROTECT,
	EPS_SATURATEDPORE,
	EPS_INACTIVEPORE,
	EPS_SLIPPORE,

	EPS_MAX
};

//enum ECurrentStage
//{
//	ECS_PORE_INSERT,
//	ECS_PORE_QC,
//	ECS_SEQENCING,
//
//	ECS_MAX
//};

const static QString g_borderStyle[]{
	"border:3px solid blue; background: #00B000"//blue,green
	, "border:3px solid black;"
};

const static QString g_colorStyle[]{
	"background: #A8A8A8"//C8C8C8
	, "background: #008000"//green
	, "background: #FF0000"//red
	, "border:3px solid blue; background: #00B000"//green
	, "background: #B24D9B"
	, "background: #8BC2EA"
	, "background: #006d9C"
};

const static QString g_SinglePoreStyle[]{
	"min-width: 11px; min-height: 11px;max-width:11px; max-height: 11px;border:1px solid black;background: white;"
	, "min-width: 11px; min-height: 11px;max-width:11px; max-height: 11px;border:1px solid green;background: rgb(0, 255, 255)"
};

const static QString g_DegatingSheetStyle[]{
	"border:1px solid black;background:green"
	, "border:1px solid black;background:red"
	, "border:1px solid black;background:black"
};


const static QString g_qlabelStyle[]{
	"<font color=red>WillAutoStopAndSaved</font>"
	, "<font color=red><strong>WillAutoStopAndSaved</strong></font>"
	, "<font color=red><strong><h2>TimeUp</h2></strong></font>"
};


class ConfigServer
{
private:
	ConfigServer();
	ConfigServer(const ConfigServer& other) = delete;
	ConfigServer& operator=(const ConfigServer& other) = delete;
public:
	~ConfigServer() = default;

private:
	static ConfigServer* m_instance;

	int m_iUseBgiCyclone = 0;
	int m_iUseDemoData = 0;
	int m_iPrintDemoData = 0;
	int m_iWorknet = 0;
	int m_iDeveloperMode = 0;
	std::string m_strMainUserName = "shanzhu";
	std::string m_strLoginUserName;
	std::string m_strMockDataDemo = "mockDataDemo";
	int m_iHardwareAmpBoardVer = 1;

	//ECurrentStage m_eCurrenStage = ECS_PORE_INSERT;

	double m_dPlotYmax = 5000.0;
	double m_dPlotYmin = -5000.0;
	double m_dPlotXnum = 10.0;
	int m_iUseOpenGL = 1;
	int m_iSampleRate = DEFAULT_SAMPLE_RATE;
	double m_iTriangFrequence = 10.0;
	double m_iTriangAmplitude = 0.05;

	unsigned long long m_ullRawDataLength = 0;

	int m_iSequencingId = 0;
	std::string m_strRecordConfig;
	std::string m_strSequenceConfig;

	std::string m_strUploadAddr;//upload file or folder

	std::string m_strTaskAddr;
	int m_iTaskPort = 6379;
	int m_iTaskDbnum = 6;

	std::atomic<bool> m_bAlgorithmServerStatus = false;

	std::atomic<bool> m_bShowVerupdate = false;
	std::string m_strIgnoreVer;
	std::string m_strVerupdateAddr;
	std::string m_strVerloadAddr;

	QSettings* m_pSysConfig = nullptr;

	std::atomic<bool> m_bAutoDegating = false;
	std::atomic<bool> m_bCalibration = false;
	std::atomic<bool> m_bZeroAdjustProcess = false;
	std::atomic<bool> m_bZeroAdjust = false;
	std::atomic<bool> m_bSaveData = false;
	std::atomic<bool> m_bRealtimeSequence = false;
	std::atomic<bool> m_bCollectCap = false;
	std::atomic<bool> m_bRunSleep = true;

	time_t m_tmtStartSaveData = 0;

	std::map<int, std::pair<int, int>> m_mapChannelCoordinate;

public:
	struct SDataHandle
	{
		std::vector<std::vector<float>> dataHandle;

		SDataHandle(const std::vector<std::vector<float>>& other)
		{
			for (int i = 0; i < CHANNEL_NUM; ++i)
			{
				dataHandle.emplace_back(other[i]);
			}
		}
	};

private:
	moodycamel::ConcurrentQueue<std::string> m_queVoltageInfo;


public:
	static ConfigServer* GetInstance(void);

	bool EnqueueVoltageInfo(const std::string& voltinfo);
	bool DequeueVoltageInfo(std::string& voltinfo);

	bool FindChannelCoordinate(int ch, int& ix, int& iy);
	bool FindCoordinateChannel(int ix, int iy, int& ch);

	bool InitData(void);
	bool InitConfig(QSettings * pset);

	int GetUseBgiCyclone(void) const { return m_iUseBgiCyclone; }
	int GetUseDemoData(void) const { return m_iUseDemoData; }
	int GetPrintDemoData(void) const { return m_iPrintDemoData; }
	int GetWorknet(void) const { return m_iWorknet; }

	int GetDeveloperMode(void) const { return m_iDeveloperMode; }
	void SetDeveloperMode(int val) { m_iDeveloperMode = val; }

	const std::string GetMainUserName(void) const { return m_strMainUserName; }
	void SetMainUserName(const std::string& val) { m_strMainUserName = val; }

	const std::string GetLoginUserName(void) const { return m_strLoginUserName; }
	void SetLoginUserName(const std::string& val) { m_strLoginUserName = val; }

	const std::string GetMockDataDemo(void) const { return m_strMockDataDemo; }
	void SetMockDataDemo(const std::string& val) { m_strMockDataDemo = val; }

	int GetHardwareAmpBoardVer(void) const { return m_iHardwareAmpBoardVer; }
	void SetHardwareAmpBoardVer(int val) { m_iHardwareAmpBoardVer = val; }

	//ECurrentStage GetCurrentStage(void) const { return m_eCurrenStage; }
	//void SetCurrentStage(ECurrentStage ecs) { m_eCurrenStage = ecs; }

	double GetPlotYmax(void) const { return m_dPlotYmax; }
	void SetPlotYmax(double val) { m_dPlotYmax = val; }

	double GetPlotYmin(void) const { return m_dPlotYmin; }
	void SetPlotYmin(double val) { m_dPlotYmin = val; }

	double GetPlotXnum(void) const { return m_dPlotXnum; }
	void SetPlotXnum(double val) { m_dPlotXnum = val; }

	int GetUseOpenGL(void) const { return m_iUseOpenGL; }
	void SetUseOpenGL(int val)
	{
		m_iUseOpenGL = val;

		m_pSysConfig->beginGroup("plotCtrl");
		m_pSysConfig->setValue("isOpenGL", val);
		m_pSysConfig->endGroup();
	}

	int GetSampleRate(void) const { return m_iSampleRate; }
	void SetSampleRate(int val) { m_iSampleRate = val; }

	double GetTriangFrequence(void) const { return m_iTriangFrequence; }
	void SetTriangFrequence(double val) { m_iTriangFrequence = val; }

	double GetTriangAmplitude(void) const { return m_iTriangAmplitude; }
	void SetTriangAmplitude(double val) { m_iTriangAmplitude = val; }

	unsigned long long GetRawDataLength(void) const { return m_ullRawDataLength; }
	void SetRawDataLength(size_t val) { m_ullRawDataLength = val; }
	void AddRawDataLength(size_t step) { m_ullRawDataLength += step; }
	double GetRawDataTime(void) const { return m_ullRawDataLength / 60.0 / m_iSampleRate; }

	int GetSequencingId(void) const { return m_iSequencingId; }
	void SetSequencingId(int val) { m_iSequencingId = val; }

	const std::string GetRecordConfig(void) const { return m_strRecordConfig; }
	void SetRecordConfig(const std::string& val) { m_strRecordConfig = val; }

	const std::string GetSequenceConfig(void) const { return m_strSequenceConfig; }
	void SetSequenceConfig(const std::string& val) { m_strSequenceConfig = val; }

	const std::string GetUploadAddr(void) const { return m_strUploadAddr; }
	void SetUploadAddr(const std::string& val)
	{
		m_strUploadAddr = val;

		m_pSysConfig->beginGroup("WithAlgorithm");
		m_pSysConfig->setValue("uploadaddr", val.c_str());
		m_pSysConfig->endGroup();
	}

	const std::string GetTaskRedisParam(int& port, int& dbnum) const
	{
		port = m_iTaskPort;
		dbnum = m_iTaskDbnum;
		return m_strTaskAddr;
	}
	void SetTaskRedisParam(const std::string& host, int port, int dbnum)
	{
		m_strTaskAddr = host;
		m_iTaskPort = port;
		m_iTaskDbnum = dbnum;

		m_pSysConfig->beginGroup("WithAlgorithm");
		m_pSysConfig->setValue("taskaddr", host.c_str());
		m_pSysConfig->setValue("taskport", port);
		m_pSysConfig->setValue("taskdb", dbnum);
		m_pSysConfig->endGroup();
	}

	bool GetAlgorithmServerStatus(void) const { return m_bAlgorithmServerStatus; }
	void SetAlgorithmServerStatus(bool val) { m_bAlgorithmServerStatus = val; }

	bool GetShowVerupdate(void) const { return m_bShowVerupdate; }
	void SetShowVerupdate(bool val) { m_bShowVerupdate = val; }

	const std::string GetIgnoreVer(void) const { return m_strIgnoreVer; }
	void SetIgnoreVer(const std::string& val)
	{
		m_strIgnoreVer = val;
		m_pSysConfig->beginGroup("VerUpdate");
		m_pSysConfig->setValue("ignore", m_strIgnoreVer.c_str());
		m_pSysConfig->endGroup();
	}

	const std::string GetVerupdateAddr(void) const { return m_strVerupdateAddr; }
	void SetVerupdateAddr(const std::string& val)
	{
		m_strVerupdateAddr = val;
		m_pSysConfig->beginGroup("VerUpdate");
		m_pSysConfig->setValue("verurl", m_strVerupdateAddr.c_str());
		m_pSysConfig->endGroup();
	}

	const std::string GetVerloadAddr(void) const { return m_strVerloadAddr; }
	void SetVerloadAddr(const std::string& val)
	{
		m_strVerloadAddr = val;
		m_pSysConfig->beginGroup("VerUpdate");
		m_pSysConfig->setValue("updurl", m_strVerloadAddr.c_str());
		m_pSysConfig->endGroup();
	}


	bool GetAutoDegating(void) const { return m_bAutoDegating; }
	void SetAutoDegating(bool val) { m_bAutoDegating = val; }

	bool GetCalibration(void) const { return m_bCalibration; }
	void SetCalibration(bool val) { m_bCalibration = val; }

	//Part ZeroAdjust
	bool GetZeroAdjust(void) const { return m_bZeroAdjust; }
	//Part ZeroAdjust
	void SetZeroAdjust(bool val) { m_bZeroAdjust = val; }

	//All ZeroAdjust
	bool GetZeroAdjustProcess(void) const { return m_bZeroAdjustProcess; }
	//All ZeroAdjust
	void SetZeroAdjustProcess(bool val) { m_bZeroAdjustProcess = val; }

	bool GetSaveData(void) const { return m_bSaveData; }
	void SetSaveData(bool val) { m_bSaveData = val; }

	bool GetRealtimeSequence(void) const { return m_bRealtimeSequence; }
	void SetRealtimeSequence(bool val) { m_bRealtimeSequence = val; }

	bool GetCollectCap(void) const { return m_bCollectCap; }
	void SetCollectCap(bool val) { m_bCollectCap = val; }

	bool GetRunSleep(void) const { return m_bRunSleep; }
	void SetRunSleep(bool val) { m_bRunSleep = val; }

	time_t GetStartSaveData(void) const { return m_tmtStartSaveData; }
	void SetStartSaveData(time_t tmt) { m_tmtStartSaveData = tmt; }


	QSettings* GetSysSetting(void) { return m_pSysConfig; }

	void PoreSleep(int msec)
	{
		QTime dieTime = QTime::currentTime().addMSecs(msec);
		while (m_bRunSleep && QTime::currentTime() < dieTime)
		{
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		}
	}

	static void caliSleep(int msec)
	{
		QTime dieTime = QTime::currentTime().addMSecs(msec);
		while (QTime::currentTime() < dieTime)
		{
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
		}
	}

	static void MeSleep(int msec)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(msec));
	}

	static bool SyncDefaultConfig(const char* pprefix, const char* pconf);

	static bool GetCalibrationLineFit(const std::vector<std::pair<double, double>>& vctPoints, double& dSlope, double& dOffset);

	static int GetFileMD5Sum(const char* filefullpath, char* MD5key);
	static int GetExecuteCMD(const char* cmd, char* result, int len);
	static bool SplitString2(const char* pSrc, const char* pde, std::vector<std::string>& vetOut);

	static std::string tostring(double val, int bit = -1);
	static std::string GetCurrentPath(void);
	static std::string GetCurrentDateTime(void);
	static std::string GetCurrentTimeMore(void);
	static long long GetCurrentTimestamp(void);
	static unsigned long GetCurrentThreadSelf(void);
	static std::string GetLocalIPAddress(void);
	static std::string GetHomePath(void);
	static std::string GetUserPath(void);
	static std::string GetDumpPath(void);

	static void WidgetPosAdjustByParent(QWidget* parent, QWidget* widget);
};

