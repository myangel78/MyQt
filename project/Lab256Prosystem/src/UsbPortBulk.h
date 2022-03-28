#pragma once

#include <qobject.h>
#include <mutex>
#include <tuple>
#include <vector>
#include <condition_variable>
#include <string.h>
//#include <windows.h>
//#include <minwindef.h>

#include <concurrentqueue.h>

#include "UsbProtoMsg.h"


#define MOCKDATA_DEMO_TEST 0


#define USE_EACHTHREADRECVANDHANDLE_DATA

#define     TIMEOUT_PER_TRANSFER_MILLI_SEC      1500
#define     USBD_STATUS_ENDPOINT_HALTED     0xC0000030
//constexpr int RAW_BUF_BLOCK_END_INDEX = 3125 - 1;


#ifdef Q_OS_WIN32
class UsbCyDevice;
#else
class UsbCyclone;
#endif
class CustomPlotView;
class ControlView;
class CalibrationDialog;
class SavePane;
class CapacitanceView;


class UsbPortBulk //: public QObject
{
	//Q_OBJECT

public:
	UsbPortBulk();
	~UsbPortBulk();

private:
#ifdef Q_OS_WIN32
	UsbCyDevice* m_pUsbCyDevice = nullptr;
#else
	UsbCyclone* m_pUsbCyDevice = nullptr;
#endif

	CustomPlotView* m_pCustomPlot = nullptr;
	ControlView* m_pControlView = nullptr;
	CalibrationDialog* m_pCalibrationDlg = nullptr;
	SavePane* m_pSavePane = nullptr;
	CapacitanceView* m_pCollectCap = nullptr;
public:
#ifdef Q_OS_WIN32
	UsbCyDevice* GetUsbCyDevice(void) const { return m_pUsbCyDevice; }
	void SetUsbCyDevice(UsbCyDevice* pusbcydevice) { m_pUsbCyDevice = pusbcydevice; }
#else
	UsbCyclone* GetUsbCyDevice(void) const { return m_pUsbCyDevice; }
	void SetUsbCyDevice(UsbCyclone* pusbcydevice) { m_pUsbCyDevice = pusbcydevice; }
#endif

	void SetCustomPlotView(CustomPlotView* pPlotView) { m_pCustomPlot = pPlotView; }
	void SetCurrentView(ControlView* pView) { m_pControlView = pView; }
	void SetCalibrationDlg(CalibrationDialog* pdlg) { m_pCalibrationDlg = pdlg; }
	void SetSavePane(SavePane* ppane) { m_pSavePane = ppane; }
	void SetCollectCap(CapacitanceView* ppane) { m_pCollectCap = ppane; }


private:
	bool m_bRunning = false;
	bool m_bSuspend = false;
	bool m_bRunningHnadle = false;
	std::shared_ptr<std::thread> m_ThreadPtr = nullptr;
	std::shared_ptr<std::thread> m_ThreadHandlePtr = nullptr;

	int m_iHandleBuf = 0;
	unsigned char* m_pHandleBuf = nullptr;
	unsigned char m_arUnhandleBuf[1024] = { 0 };

	const int* m_pRealChannel = nullptr;

public:
    //volatile int waiter_cnt = 0;
	std::condition_variable m_cvRecv;
	std::mutex m_mutexRecv;
	std::condition_variable m_cvHandle;
	std::mutex m_mutexHandle;

	void SetThreadSuspend(bool bsuspend) { m_bSuspend = bsuspend; }

#ifdef USE_EACHTHREADRECVANDHANDLE_DATA
	struct SDataPack
	{
		std::vector<unsigned char> dataPack;

		SDataPack(unsigned char* org, int len)
		{
			dataPack.resize(len);
			memcpy(&dataPack[0], org, len);
		}
	};
	moodycamel::ConcurrentQueue<std::shared_ptr<SDataPack>> m_queDataPack;
#endif

	std::vector<std::vector<float>> m_dataHandle;


private:
	static int FindStartFlag(unsigned char* psrc, unsigned char a, unsigned char b);
	inline bool SetDataHandle(int i, unsigned char high, unsigned char low, const std::vector<std::tuple<double, double, double>>* ptabdt);
	inline bool SetDataHandleCalibration(int i, unsigned char high, unsigned char low);
	inline bool SetDataHandleZeroAdjust(int i, unsigned char high, unsigned char low, const std::vector<std::tuple<double, double, double>>* ptabdt);

public:
	bool StartThread(void);
	void ThreadFunc(void);
#ifdef USE_EACHTHREADRECVANDHANDLE_DATA
	void ThreadHandleFunc(void);
#endif

	bool EndThread(bool all = false);
	bool StopThread(bool all = false);
};

