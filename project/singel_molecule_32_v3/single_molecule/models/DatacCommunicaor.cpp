#include"DatacCommunicaor.h"
#include"logger/Log.h"

DataCommunicaor::DataCommunicaor()
{
    resetSem();
    m_VctRawBuf.resize(RAW_BUF_BLOCK_LEN);

    m_VctSaveBuf.resize(SAVE_BUF_BLOCK_LENGTH);
    m_VctPlotBuf.resize(PLOT_BUF_BLOCK_LENGTH);

    m_vctDegatBuffer.resize(DEGATE_BUF_BLOCK_LENGTH);
    m_vctMeansCalBuf.resize(MEANSCAL_BUF_BLOCK_LENGTH);

    m_vctMeansCapBuf.resize(MEANSCAP_BUF_BLOCK_LENGTH);
    m_vctCapResBuf.resize(MEANSCAPRES_BUF_BLOCK_LENGTH);

    g_xTime = 2;
    g_xTimeUs = g_xTime - 0.000001;

    g_bZeroAdjust = false;
    g_bZeroAdjustProcess = false;

    g_bCalibration = false;
    g_bDegattingRunning = false;


}

DataCommunicaor::~DataCommunicaor()
{
    m_ExecutorListener.clear();
}

void DataCommunicaor::reset()
{
    resetSem();
    resetBuf();
}

void DataCommunicaor::resetSem()
{
    m_sRawSem.usedBuf.acquire(m_sRawSem.usedBuf.available());
    m_sChannelSem.usedBuf.acquire(m_sChannelSem.usedBuf.available());
    m_sOutputFormatSem.usedBuf.acquire(m_sOutputFormatSem.usedBuf.available());
    m_sSaveSem.usedBuf.acquire(m_sSaveSem.usedBuf.available());
    m_sPlotSem.usedBuf.acquire(m_sPlotSem.usedBuf.available());
    m_sDegateSem.usedBuf.acquire(m_sDegateSem.usedBuf.available());
    m_sCollectSem.usedBuf.acquire(m_sCollectSem.usedBuf.available());
    m_sMeansCalSem.usedBuf.acquire(m_sMeansCalSem.usedBuf.available());
    m_sCapCollectSem.usedBuf.acquire(m_sCapCollectSem.usedBuf.available());
    m_sCapCatchSem.usedBuf.acquire(m_sCapCatchSem.usedBuf.available());
    m_sCapResSem.usedBuf.acquire(m_sCapResSem.usedBuf.available());

    m_sRawSem.freeBuf.release(RAW_BUF_BLOCK_LEN - m_sRawSem.freeBuf.available());
    m_sChannelSem.freeBuf.release(CH_BUF_BLOCK_LEN - m_sChannelSem.freeBuf.available());
    m_sOutputFormatSem.freeBuf.release(CH_BUF_BLOCK_LEN - m_sOutputFormatSem.freeBuf.available());
    m_sSaveSem.freeBuf.release(SAVE_BUF_BLOCK_LEN - m_sSaveSem.freeBuf.available());
    m_sPlotSem.freeBuf.release(PLOT_BUF_BLOCK_CNT - m_sPlotSem.freeBuf.available());
    m_sDegateSem.freeBuf.release(DEGATE_BUF_BLOCK_CNT - m_sDegateSem.freeBuf.available());
    m_sCollectSem.freeBuf.release(CH_BUF_BLOCK_LEN - m_sCollectSem.freeBuf.available());
    m_sMeansCalSem.freeBuf.release(MEANSCAL_BUF_BLOCK_CNT - m_sMeansCalSem.freeBuf.available());
    m_sCapCollectSem.freeBuf.release(CH_BUF_BLOCK_LEN - m_sCapCollectSem.freeBuf.available());
    m_sCapCatchSem.freeBuf.release(MEANSCAP_BUF_BLOCK_CNT - m_sCapCatchSem.freeBuf.available());
    m_sCapResSem.freeBuf.release(MEANSCAPRES_BUF_BLOCK_CNT - m_sCapResSem.freeBuf.available());
}

void DataCommunicaor::resetBuf()
{
    for(int i=0; i<m_VctRawBuf.size(); i++)
    {
        m_VctRawBuf[i].clear();
    }

    for(int i=0; i<m_VctPlotBuf.size(); i++)
    {
        m_VctPlotBuf[i].clear();
    }

    for(int i=0; i<m_VctSaveBuf.size(); i++)
    {
        m_VctSaveBuf[i].clear();
    }

    for(int i= 0; i < m_vctDegatBuffer.size(); i++)
    {
        m_vctDegatBuffer[i].clear();
    }

    for(int i= 0; i < m_vctMeansCalBuf.size(); i++)
    {
        m_vctMeansCalBuf[i].clear();
    }
    for(int i= 0; i < m_vctMeansCapBuf.size(); i++)
    {
        m_vctMeansCapBuf[i].clear();
    }
    for(int i= 0; i < m_vctCapResBuf.size(); i++)
    {
        m_vctCapResBuf[i].clear();
    }

}


void DataCommunicaor::OnExecuteStatusNotify(int id, int status)
{
    std::lock_guard<std::mutex> lock(m_ExecuteMutex);
    LOGT("{} : id={}, value={}, status={}", __func__, (void*)(size_t)id,  (void*)(size_t)status, EcoliEnumString<ENUM_STATUS>::From(status));
    for (auto it : m_ExecutorListener)
    {
        it->OnExecuteStatusNotify(id, status);
    }
}

void DataCommunicaor::AddExecuteStatusListener(ExecuteListener* pListener,const char* name)
{
    std::lock_guard<std::mutex> lock(m_ExecuteMutex);
    LOGT("add ExecuteStatus {} Lsn:{}", name,(void*)(size_t)pListener);
    m_ExecutorListener.push_back(pListener);

}

void DataCommunicaor::RemoveExecuteStatusListener(ExecuteListener* pListener,const char* name)
{
    std::lock_guard<std::mutex> lock(m_ExecuteMutex);
    LOGT("remove ExecuteResult {} Lsn:{}", name,(void*)(size_t)pListener);
    m_ExecutorListener.remove_if(ListenerRemoveIf<ExecuteListener>(pListener));
}
