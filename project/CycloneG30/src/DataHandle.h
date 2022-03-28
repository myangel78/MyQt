#pragma once

#include <qobject.h>
#include <mutex>
#include <tuple>
#include <vector>
#include <condition_variable>
#include <string.h>
#include <concurrentqueue.h>
#include "UsbProtoMsg.h"
#include "qtimer.h"
#include "ConfigServer.h"

class DataCalculate;
class UsbFtdDevice;
class CustomPlotWidgt;
class SavePannel;
class DegatingPane;
class CapCaculate;



class DataHandle
{
public:
    DataHandle();
    ~DataHandle();

private:
     UsbFtdDevice *m_pUsbFtdDevice = nullptr;
     CustomPlotWidgt* m_pCustomPlot = nullptr;
     SavePannel* m_pSavePane = nullptr;
     DegatingPane* m_pDegatingPane = nullptr;
     DataCalculate *m_pDataCalculate = nullptr;
     CapCaculate *m_pCapCaculate = nullptr;

     bool m_bRunning = false;
     bool m_bSuspend = false;
     bool m_bRunningHnadle = false;
     bool m_bRunningDecode = false;

     bool m_bCrcCheck = false;
     bool m_bCrcCheckPrint =false;
     bool m_bRawData =false;
     bool m_bNeedTrans =false;

     std::atomic<bool> m_bSimulateData = false;
     std::shared_ptr<std::thread> m_ThreadHandlePtr = nullptr;
     std::shared_ptr<std::thread> m_ThreadDecodePtr = nullptr;
     std::shared_ptr<std::thread> m_ThreadPtr = nullptr;
     std::vector<std::vector<float>> m_vctRawData;

public:
     std::condition_variable m_cvDcode;
     std::mutex m_mutexDecode;
     std::condition_variable m_cvHandle;
     std::mutex m_mutexHandle;
     std::condition_variable m_cvRecv;
     std::mutex m_mutexRecv;

    struct SDataPack
    {
        std::vector<unsigned char> dataPack;

        SDataPack(unsigned char* org, int len)
        {
            dataPack.resize(len);
            memcpy(&dataPack[0], org, len);
        }
        SDataPack(const int &len)
        {
            dataPack.resize(len);
            memset(&dataPack[0], 0, len);
        }

        SDataPack(std::vector<uint8_t> &&vctData)
        {
            dataPack = std::move(vctData);
//            dataPack.swap(vctData);
        }

    };

    moodycamel::ConcurrentQueue<std::shared_ptr<ConfigServer::SDataHandle>> m_queDataPack;
    moodycamel::ConcurrentQueue<std::shared_ptr<SDataPack>> m_queDecodePack;


public:
    void SetCapCalculate(CapCaculate* pCapCaculate) { m_pCapCaculate = pCapCaculate; }
    void SetDataCalculate(DataCalculate* pDataCalculate) { m_pDataCalculate = pDataCalculate; }
    void SetCustomPlotView(CustomPlotWidgt* pPlotView) { m_pCustomPlot = pPlotView; }
    void SetSavePane(SavePannel* ppane) { m_pSavePane = ppane; }
    void SetDegatingPane(DegatingPane* ppane) { m_pDegatingPane = ppane; }
    void SetTestThreadSuspend(bool bsuspend) { m_bSuspend = bsuspend; }
    void SetIsSimulateRunning(const bool &enable){ m_bSimulateData = enable;}
    bool GetIsSimulateRunning(void){return m_bSimulateData;}

    bool StartThread(void);
    void ThreadRecvFunc(void);
    void ThreadHandleFunc(void);
    void ThreadDecodeFunc(void);
    bool EndThread(void);
    bool StopThread(void);

private:
    bool VertifyCompleteFrames(const unsigned char * psrc) ;
    bool DecodeOneFrameRawData(const unsigned char* psrc,const char &frame,const bool bRawData = false);
    bool TryToAcquireFrameNum(const unsigned char * psrc,uchar & reframe);

    bool IsRightFrameHeaderFmt(const unsigned char *ptrSrc);

    void LVDSCurrentConvert(std::vector<std::vector<float>> &vctRawData,const bool &bIsRaw);
};


