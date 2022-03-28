#include "ValidOptimized.h"
#include "Log.h"

ValidOptimized::ValidOptimized(void)
{
    Init();
}

void ValidOptimized::ResetTimes(void)
{
    ClearStore();
}

void ValidOptimized::ClearStore(void)
{
    for(int sensorGrp =0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp )
    {
        std::fill_n(m_vctValidStore[sensorGrp].begin(),CHANNEL_NUM,INVALID_CHANNEL_ENUM);
        m_vctValidStoreCnt[sensorGrp] = 0;
    }
}

void ValidOptimized::Init(void)
{
    m_vctValidStore.resize(SENSOR_GROUP_NUM);
    m_vctValidStoreCnt.resize(SENSOR_GROUP_NUM);

    for(int sensorGrp =0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp )
    {
        m_vctValidStore[sensorGrp].resize(CHANNEL_NUM);
        std::fill_n(m_vctValidStore[sensorGrp].begin(),CHANNEL_NUM,INVALID_CHANNEL_ENUM);
        m_vctValidStoreCnt[sensorGrp] = 0;
    }
}


void ValidOptimized::MultiValidFlitOptimize(std::vector<std::vector<CHANNEL_VALID_ENUM>>& vetValidState)
{

    for(int sensorGrp =0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp )
    {
        const auto &vctState =m_vctValidStore[sensorGrp];
        std::copy(vctState.cbegin(),vctState.cend(),vetValidState[sensorGrp].begin());
    }
}

void ValidOptimized::StoreValidState(const int &curTime,const std::vector<std::vector<CHANNEL_VALID_ENUM>>& vetValidState,const std::vector<std::vector<int>> &vctValidCnt)
{
    for(int sensorGrp =0; sensorGrp < SENSOR_GROUP_NUM; ++sensorGrp )
    {
        LOGI("CurTime {}, Valid {}",curTime,vctValidCnt[sensorGrp][VALID_CHANNEL_ENUM]);
//        qDebug("CurTime %d, Valid %d",curTime,vctValidCnt[sensorGrp][VALID_CHANNEL_ENUM]);
        const int &validCnt = vctValidCnt[sensorGrp][VALID_CHANNEL_ENUM] ;
        if(m_vctValidStoreCnt[sensorGrp] < validCnt)
        {
            std::copy(vetValidState[sensorGrp].cbegin(),vetValidState[sensorGrp].cend(),m_vctValidStore[sensorGrp].begin());
            m_vctValidStoreCnt[sensorGrp] = validCnt;
        }
    }
}

