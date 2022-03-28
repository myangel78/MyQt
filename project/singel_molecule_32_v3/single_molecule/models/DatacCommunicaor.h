#ifndef DATACOMMUNICAOR_H
#define DATACOMMUNICAOR_H

#include"helper/Singleton.h"
#include<QSemaphore>
#include "models/costomplot/qcustomplot.h"
#include"helper/ExecuteListener.h"
#include"DataDefine.h"

typedef struct
{
    QSemaphore freeBuf;
    QSemaphore usedBuf;
}Sem;


class DataCommunicaor: public CSingleton<DataCommunicaor>
{
public:
    DataCommunicaor();
    ~DataCommunicaor();

public:
    void OnExecuteStatusNotify(int id,int status);
    void AddExecuteStatusListener(ExecuteListener* pListener,const char* name);
    void RemoveExecuteStatusListener(ExecuteListener* pListener,const char* name);    
    void reset();

public:
   std::vector<QByteArray> m_VctRawBuf;
   float m_VctChannelBuf[CH_BUF_BLOCK_LEN][CHANNEL_NUM];
   QVector<QVector<float>> m_VctSaveBuf;
   QVector<QVector<QCPGraphData>> m_VctPlotBuf;
   float m_VctDispBuf[CHANNEL_NUM];
   QVector<QVector<float>> m_vctDegatBuffer;
   QVector<QVector<float>> m_vctMeansCalBuf;
   QVector<QVector<float>> m_vctMeansCapBuf;
   QVector<QVector<float>> m_vctCapResBuf;



   std::list<ExecuteListener*> m_ExecutorListener;
   std::mutex m_ExecuteMutex;

   Sem m_sChannelSem;
   Sem m_sRawSem;
   Sem m_sSaveSem;
   Sem m_sOutputFormatSem;
   Sem m_sPlotSem;
   Sem m_sDegateSem;
   Sem m_sCollectSem;
   Sem m_sCapCollectSem;
   Sem m_sMeansCalSem;
   Sem m_sCapCatchSem;
   Sem m_sCapResSem;



   ENUM_DISP_CHANGE dispStatus;

   float g_xTime;
   float g_xTimeUs;

   std::atomic<bool> g_IsSaveToFiles = false;

   bool g_bCalibration;

   bool g_bDegattingRunning;

   bool g_bZeroAdjust;
   bool g_bZeroAdjustProcess;


   struct {
       QReadWriteLock CapMutex;
       double triangleAmp = 0.1;
       int triangleFreq = 10;
       bool IsTrangleWave = false;
   }g_capParameter;


private:
   void resetSem();
   void resetBuf();


private:
    template<typename T>
    struct ListenerRemoveIf
    {
    public:
        bool operator( ) ( T* ptr )
        {
            return m_ptr == ptr;
        }

        ListenerRemoveIf(T* ptr){m_ptr = ptr;}
    private:
        T* m_ptr;
    };
};



#endif // DATACOMMUNICAOR_H
