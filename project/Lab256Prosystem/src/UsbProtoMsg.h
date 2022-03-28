#pragma once

#include <string>
#include <vector>
#include <atomic>

//#include <windows.h>
//#include <minwindef.h>
//
//#ifndef USE_METYPEDEF
//#define USE_METYPEDEF
//
//typedef unsigned char uchar;
//typedef unsigned short ushort;
//typedef unsigned int uint;
//
//#endif


#define USE_OLD_CTRLPROTOCOL 



class UsbProtoMsg
{
public:
	UsbProtoMsg() = default;
	~UsbProtoMsg() = default;

private:
	static std::atomic<long long> m_iSeqNum;
public:
	static bool SplitString(const char* pSrc, const char* pde, std::vector<std::string>& vetOut);

	static long long String2Hex(const char* pSrc);

	static int Hex2Dec(char hex);

	/*
	* AD电压转换
	* dc 为要计算的电压
	* range为电压范围：
	  5表示的范围是[-2.5, 2.5]
	  10表示的范围是[-5, 5]
	  15表示的范围是[-7.5, 7.5]
	  20表示的范围是[-10, 10]
	* tot为总计算范围，一般为65536
	* 返回值为转换后的数值
	*/
	static float GetHexCurrent(short dc, int range = 5, int tot = 65536);

	/*
	* DA电压转换
	* dc 为要计算的电压
	* range为电压范围：
	  5表示的范围是[-2.5, 2.5]
	  10表示的范围是[-5, 5]
	  15表示的范围是[-7.5, 7.5]
	  20表示的范围是[-10, 10]
	* tot为总计算范围，一般为65536
	* 返回值为转换后的数值
	*/
	static int GetCurrentHex(double dc, int range = 5, int tot = 65536);

	/*
	* 返回当前版本信息
	*/
	static std::string GetCurrentVer(void);

	/*
	* 开始采集数据回传
	* 返回命令码长度
	*/
	static size_t StartCollect(unsigned char* pout);
	static size_t StartCollect(int& pout);

	/*
	* 停止采集数据回传
	* 返回命令码长度
	*/
	static size_t StopCollect(unsigned char* pout);
	static size_t StopCollect(int& pout);

	/*
	* 单通道采样速率设置（暂时不用）
	* 返回命令码长度
	*/
	static size_t SetOneChannelRate(int chan, int rate, unsigned char* pout) = delete;

	/*
	* 通道采样速率统一设置
	* rate要设置的速率（Hz）
	* 返回命令码长度
	*/
	static size_t SetAllChannelRate(int rate, unsigned char* pout);
	static size_t SetAllChannelRate(int rate, int& pout);

	/*
	* 过采样率统一设置
	* multiple要设置的过采样倍数
	* 返回命令码长度
	*/
	static size_t SetOverSampMultiple(int multiple, int& pout);

	/*
	* 直流电平独立设置
	* chan要设置的通道号
	* lvl要设置的电平值（V）
	* 返回命令码长度
	*/
	static size_t SetOneDCLevel(int chan, double lvl, unsigned char* pout);
	static size_t SetOneDCLevel(int chan, double lvl, int& pout);

	/*
	* 直流电平统一设置
	* lvl要设置的电平值（V）
	* 返回命令码长度
	*/
	static size_t SetAllDCLevel(double lvl, unsigned char* pout);
	static size_t SetAllDCLevel(double lvl, int& pout);

	/*
	* 脉冲电平独立设置
	* chan要设置的通道号
	* lvl要设置的电平值（V）
	* 返回命令码长度
	*/
	static size_t SetOnePulseLevel(int chan, double lvl, unsigned char* pout);
	static size_t SetOnePulseLevel(int chan, double lvl, int& pout);

	/*
	* 脉冲电平统一设置
	* lvl要设置的电平值（V）
	* 返回命令码长度
	*/
	static size_t SetAllPulseLevel(double lvl, unsigned char* pout);
	static size_t SetAllPulseLevel(double lvl, int& pout);

	/*
	* 三角波电平统一设置
	* lvl要设置的幅值（V）
	* rate要设置的频率（Hz）
	* 返回命令码长度
	*/
	static size_t SetAllTrianglewaveLevel(double lvl, int rate, unsigned char* pout);
	static size_t SetAllTrianglewaveLevel(double lvl, int rate, int& pout);

	/*
	* 方波电平统一设置
	* lvl要设置的幅值（V）
	* rate要设置的频率（Hz）
	* 返回命令码长度
	*/
	static size_t SetAllRectanglewaveLevel(double lvl, int rate, unsigned char* pout);
	static size_t SetAllRectanglewaveLevel(double lvl, int rate, int& pout);
};

