#include "ModelSharedData.h"


ModelSharedData::ModelSharedData(QObject *parent):QObject(parent)
{
    InitData();

}
ModelSharedData::~ModelSharedData()
{

}

void ModelSharedData::InitData(void)
{
    m_vectAllChSensorState.resize(CHANNEL_NUM);
    m_vectAllChOVFState.resize(CHANNEL_NUM);
    m_vectAllChCapState.resize(CHANNEL_NUM);
    m_vctChannelNum.resize(CHANNEL_NUM);
    m_vctCapValue.resize(CHANNEL_NUM);
    m_vetChanCurGroup.resize(CHANNEL_NUM);
    m_vetChanFuncState.resize(CHANNEL_NUM);
    m_vetAllChMuxState.resize(CHANNEL_NUM);
    m_vctAllchDegatState.resize(CHANNEL_NUM);

    for(int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        m_vectAllChSensorState[ch] =SENSOR_STATE_OFF;
        m_vectAllChOVFState[ch] = UN_OVER_FLOW_FLAG;
        m_vectAllChCapState[ch] = CAP_LEVEL1_STATE;
        m_vctChannelNum[ch] = ch+1;
        m_vctCapValue[ch] = 0.0;
        m_vetChanCurGroup[ch] = SENSOR_STATE_A_GROUP;
        m_vetAllChMuxState[ch]= SENSOR_STATE_A_GROUP;
        m_vetChanFuncState[ch]= SENSORGRP_FUNC_STATE_OFF;
        m_vctAllchDegatState[ch] = RECORD_NONE_DEGATE_STATE;
    }

    m_vectAllChLockState.resize(SENSOR_GROUP_NUM);
    m_vectAllChPoreState.resize(SENSOR_GROUP_NUM);
    for(int sensorGrp = 0; sensorGrp< SENSOR_GROUP_NUM; ++sensorGrp)
    {
        m_vectAllChLockState[sensorGrp].resize(CHANNEL_NUM);
        m_vectAllChPoreState[sensorGrp].resize(CHANNEL_NUM);
        for(int ch = 0; ch < CHANNEL_NUM; ++ch)
        {
            m_vectAllChLockState[sensorGrp][ch] = ALLOW_OPERATION_STATE;
            m_vectAllChPoreState[sensorGrp][ch] = NONE_PORE_STATE;
        }
    }

    m_vetChanIsValid.resize(SENSOR_GROUP_NUM);
    for(int sensorGrp = 0; sensorGrp <SENSOR_GROUP_NUM; ++sensorGrp )
    {
        m_vetChanIsValid[sensorGrp].resize(CHANNEL_NUM);
#ifdef EASY_PROGRAM_DEBUGGING
//        std::fill_n(m_vetChanIsValid[sensorGrp].begin(),CHANNEL_NUM,VALID_CHANNEL_ENUM);
        std::fill_n(m_vetChanIsValid[sensorGrp].begin(),CHANNEL_NUM,INVALID_CHANNEL_ENUM);
#else
        std::fill_n(m_vetChanIsValid[sensorGrp].begin(),CHANNEL_NUM,INVALID_CHANNEL_ENUM);
#endif
    }

    //state color vector
    m_vctSensorStateColr.resize(SENSOR_STATE_NUM);
    for(int i = 0; i < SENSOR_STATE_NUM; ++i)
    {
        m_vctSensorStateColr[i] =  gSensorStateColr[i];
    }
    m_vctCapStateColr.resize(CAP_STATE_NUM);
    for(int i = 0; i < CAP_STATE_NUM; ++i)
    {
        m_vctCapStateColr[i] =  gCapStateColr[i];
    }
    m_vctOVFStateColr.resize(OVF_STATE_NUM);
    for(int i = 0; i < OVF_STATE_NUM; ++i)
    {
        m_vctOVFStateColr[i] =  gOVFStateColr[i];
    }
    m_vctMuxStateColor.resize(MUX_STATE_NUM);
    for(int i = 0; i < MUX_STATE_NUM; ++i)
    {
        m_vctMuxStateColor[i] =  gMuxGroupStateColr[i];
    }
    m_vctLockStateColr.resize(SENSOR_GROUP_NUM);
    for(int sensorGrp = 0; sensorGrp< SENSOR_GROUP_NUM; ++sensorGrp)
    {
        m_vctLockStateColr[sensorGrp].resize(LOCK_OPERATE_STATE_NUM);
        for(int i = 0; i < LOCK_OPERATE_STATE_NUM; ++i)
        {
            m_vctLockStateColr[sensorGrp][i] =  gLockStateColr[i];
        }
    }
    m_vctPoreStateColor.resize(SENSOR_GROUP_NUM);
    for(int sensorGrp = 0; sensorGrp< SENSOR_GROUP_NUM; ++sensorGrp)
    {
        m_vctPoreStateColor[sensorGrp].resize(PORE_STATE_NUM);
        for(int i = 0; i < PORE_STATE_NUM; ++i)
        {
            m_vctPoreStateColor[sensorGrp][i] =  gPoreStateColr[i];
        }
    }

    m_vctValidStateColor.resize(SENSOR_GROUP_NUM);
    for(int sensorGrp = 0; sensorGrp< SENSOR_GROUP_NUM; ++sensorGrp)
    {
        m_vctValidStateColor[sensorGrp].resize(VALID_STATE_NUM);
        for(int i = 0; i < VALID_STATE_NUM; ++i)
        {
            m_vctValidStateColor[sensorGrp][i] =  gValidStateColr[i];
        }
    }

}
std::vector<SENSOR_STATE_ENUM> &ModelSharedData::GetSensorStateData(void)
{
    return m_vectAllChSensorState;
}
std::vector<OVF_STATE_ENUM> &ModelSharedData::GetOVFStateData(void)
{
    return m_vectAllChOVFState;
}
std::vector<CAP_STATE_ENUM> &ModelSharedData::GetCapStateData(void)
{
    return m_vectAllChCapState;
}
std::vector<OPERATE_STATE_ENUM> & ModelSharedData::GetLockStateDataBySensorGrp(const SENSOR_STATE_GROUP_ENUM &sensorGrp)
{
   return m_vectAllChLockState[sensorGrp];
}
std::vector<std::vector<OPERATE_STATE_ENUM>>& ModelSharedData::GetFourLockStateData(void)
{
   return m_vectAllChLockState;
}
std::vector<std::vector<PORE_STATE_ENUM>> & ModelSharedData::GetFourPoreStateData(void)
{
    return m_vectAllChPoreState;
}
std::vector<float> & ModelSharedData::GetCapVueData(void)
{
    return m_vctCapValue;
}
std::vector<SENSOR_STATE_GROUP_ENUM> & ModelSharedData::GetAllChSensorGroup(void)
{
    return m_vetChanCurGroup;
}
std::vector<SENSOR_STATE_GROUP_ENUM> & ModelSharedData::GetAllChMuxStateRef(void)
{
    return m_vetAllChMuxState;
}
std::vector<SENSORGRP_FUNC_STATE_ENUM> & ModelSharedData::GetAllChFuncState(void)
{
     return m_vetChanFuncState;
}
std::vector<RECORD_DEGATE_STATE_ENUM> &ModelSharedData::GetAllChDegateStateData(void)
{
    return m_vctAllchDegatState;
}
CHANNEL_VALID_ENUM ModelSharedData::GetCurChanValidState(const int &ch)
{
    const auto &chSensorGrp = m_vetChanCurGroup[ch];
    return m_vetChanIsValid[chSensorGrp][ch];
}
void ModelSharedData::SetCurChanValidState(const int &ch,const CHANNEL_VALID_ENUM &validState)
{
    const auto &chSensorGrp = m_vetChanCurGroup[ch];
    m_vetChanIsValid[chSensorGrp][ch] = validState;
}
void ModelSharedData::UpdateAllChSensorGroupAndFuncStateBySensorState(void)
{
    for(int ch =0; ch < CHANNEL_NUM; ++ch)
    {
        const auto &chSensorState = m_vectAllChSensorState[ch];
        auto &chSensorGrp = m_vetChanCurGroup[ch];
        auto &chFuncState = m_vetChanFuncState[ch];
        UpdateChSensorGroupAndFuncBySensorState(chSensorState,chSensorGrp,chFuncState);
    }
}
bool ModelSharedData::UpdateChSensorGroupAndFuncBySensorState(const SENSOR_STATE_ENUM &chSensorState,SENSOR_STATE_GROUP_ENUM &reChSensorGrp,SENSORGRP_FUNC_STATE_ENUM &reChFuncState)
{
    switch (chSensorState) {
    case SENSOR_STATE_OFF:
        reChFuncState = SENSORGRP_FUNC_STATE_OFF;
        return true;
    case SENSOR_STATE_A_NORMAL:
        reChSensorGrp = SENSOR_STATE_A_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        return true;
    case SENSOR_STATE_B_NORMAL:
        reChSensorGrp = SENSOR_STATE_B_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        return true;
    case SENSOR_STATE_C_NORMAL:
        reChSensorGrp = SENSOR_STATE_C_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        return true;
    case SENSOR_STATE_D_NORMAL:
        reChSensorGrp = SENSOR_STATE_D_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_NORMAL;
        return true;
    case SENSOR_STATE_A_UNBLOCK_VOLT:
        reChSensorGrp = SENSOR_STATE_A_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_UNBLOCK;
        return true;
    case SENSOR_STATE_B_UNBLOCK_VOLT:
        reChSensorGrp = SENSOR_STATE_B_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_UNBLOCK;
        return true;
    case SENSOR_STATE_C_UNBLOCK_VOLT:
        reChSensorGrp = SENSOR_STATE_C_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_UNBLOCK;
        return true;
    case SENSOR_STATE_D_UNBLOCK_VOLT:
        reChSensorGrp = SENSOR_STATE_D_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_UNBLOCK;
        return true;
    case SENSOR_STATE_A_INTERNAL_CURRENT :
        reChSensorGrp = SENSOR_STATE_A_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
        return true;
    case SENSOR_STATE_B_INTERNAL_CURRENT :
        reChSensorGrp = SENSOR_STATE_B_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
        return true;
    case SENSOR_STATE_C_INTERNAL_CURRENT :
        reChSensorGrp = SENSOR_STATE_C_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
        return true;
    case SENSOR_STATE_D_INTERNAL_CURRENT :
        reChSensorGrp = SENSOR_STATE_D_GROUP;
        reChFuncState = SENSORGRP_FUNC_STATE_INTERNAL_CURRENT_SOURCE;
        return true;
    default:
        break;
    }
    return false;
}
