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
	* AD��ѹת��
	* dc ΪҪ����ĵ�ѹ
	* rangeΪ��ѹ��Χ��
	  5��ʾ�ķ�Χ��[-2.5, 2.5]
	  10��ʾ�ķ�Χ��[-5, 5]
	  15��ʾ�ķ�Χ��[-7.5, 7.5]
	  20��ʾ�ķ�Χ��[-10, 10]
	* totΪ�ܼ��㷶Χ��һ��Ϊ65536
	* ����ֵΪת�������ֵ
	*/
	static float GetHexCurrent(short dc, int range = 5, int tot = 65536);

	/*
	* DA��ѹת��
	* dc ΪҪ����ĵ�ѹ
	* rangeΪ��ѹ��Χ��
	  5��ʾ�ķ�Χ��[-2.5, 2.5]
	  10��ʾ�ķ�Χ��[-5, 5]
	  15��ʾ�ķ�Χ��[-7.5, 7.5]
	  20��ʾ�ķ�Χ��[-10, 10]
	* totΪ�ܼ��㷶Χ��һ��Ϊ65536
	* ����ֵΪת�������ֵ
	*/
	static int GetCurrentHex(double dc, int range = 5, int tot = 65536);

	/*
	* ���ص�ǰ�汾��Ϣ
	*/
	static std::string GetCurrentVer(void);

	/*
	* ��ʼ�ɼ����ݻش�
	* ���������볤��
	*/
	static size_t StartCollect(unsigned char* pout);
	static size_t StartCollect(int& pout);

	/*
	* ֹͣ�ɼ����ݻش�
	* ���������볤��
	*/
	static size_t StopCollect(unsigned char* pout);
	static size_t StopCollect(int& pout);

	/*
	* ��ͨ�������������ã���ʱ���ã�
	* ���������볤��
	*/
	static size_t SetOneChannelRate(int chan, int rate, unsigned char* pout) = delete;

	/*
	* ͨ����������ͳһ����
	* rateҪ���õ����ʣ�Hz��
	* ���������볤��
	*/
	static size_t SetAllChannelRate(int rate, unsigned char* pout);
	static size_t SetAllChannelRate(int rate, int& pout);

	/*
	* ��������ͳһ����
	* multipleҪ���õĹ���������
	* ���������볤��
	*/
	static size_t SetOverSampMultiple(int multiple, int& pout);

	/*
	* ֱ����ƽ��������
	* chanҪ���õ�ͨ����
	* lvlҪ���õĵ�ƽֵ��V��
	* ���������볤��
	*/
	static size_t SetOneDCLevel(int chan, double lvl, unsigned char* pout);
	static size_t SetOneDCLevel(int chan, double lvl, int& pout);

	/*
	* ֱ����ƽͳһ����
	* lvlҪ���õĵ�ƽֵ��V��
	* ���������볤��
	*/
	static size_t SetAllDCLevel(double lvl, unsigned char* pout);
	static size_t SetAllDCLevel(double lvl, int& pout);

	/*
	* �����ƽ��������
	* chanҪ���õ�ͨ����
	* lvlҪ���õĵ�ƽֵ��V��
	* ���������볤��
	*/
	static size_t SetOnePulseLevel(int chan, double lvl, unsigned char* pout);
	static size_t SetOnePulseLevel(int chan, double lvl, int& pout);

	/*
	* �����ƽͳһ����
	* lvlҪ���õĵ�ƽֵ��V��
	* ���������볤��
	*/
	static size_t SetAllPulseLevel(double lvl, unsigned char* pout);
	static size_t SetAllPulseLevel(double lvl, int& pout);

	/*
	* ���ǲ���ƽͳһ����
	* lvlҪ���õķ�ֵ��V��
	* rateҪ���õ�Ƶ�ʣ�Hz��
	* ���������볤��
	*/
	static size_t SetAllTrianglewaveLevel(double lvl, int rate, unsigned char* pout);
	static size_t SetAllTrianglewaveLevel(double lvl, int rate, int& pout);

	/*
	* ������ƽͳһ����
	* lvlҪ���õķ�ֵ��V��
	* rateҪ���õ�Ƶ�ʣ�Hz��
	* ���������볤��
	*/
	static size_t SetAllRectanglewaveLevel(double lvl, int rate, unsigned char* pout);
	static size_t SetAllRectanglewaveLevel(double lvl, int rate, int& pout);
};

