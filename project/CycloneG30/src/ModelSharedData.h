#pragma once

#include <QObject>
#include "Global.h"

class ModelSharedData:public QObject
{
    Q_OBJECT
public:
    ModelSharedData(QObject *parent = nullptr);
    ~ModelSharedData();
private:
    void InitData(void);
public:
    //all kind state vector
    std::vector<SENSOR_STATE_ENUM> m_vectAllChSensorState;
    std::vector<OVF_STATE_ENUM> m_vectAllChOVFState;
    std::vector<CAP_STATE_ENUM> m_vectAllChCapState;
    std::vector<std::vector<OPERATE_STATE_ENUM>> m_vectAllChLockState; // four sensor lock state
    std::vector<std::vector<PORE_STATE_ENUM>> m_vectAllChPoreState; // four sensor pore state
    std::vector<std::vector<CHANNEL_VALID_ENUM>> m_vetChanIsValid; //four sensorgrp valid state

    std::vector<SENSOR_STATE_GROUP_ENUM> m_vetAllChMuxState;     //Mux混合模式下保存的group
    std::vector<SENSOR_STATE_GROUP_ENUM> m_vetChanCurGroup;     //通道当前处于的sensor组
    std::vector<SENSORGRP_FUNC_STATE_ENUM> m_vetChanFuncState;     //通道当前在组中处于的功能状态
    //sensor state color
    std::vector<QColor> m_vctSensorStateColr;
    std::vector<QColor> m_vctCapStateColr;
    std::vector<QColor> m_vctOVFStateColr;
    std::vector<QColor> m_vctMuxStateColor;
    std::vector<std::vector<QColor>> m_vctLockStateColr;
    std::vector<std::vector<QColor>> m_vctPoreStateColor;
    std::vector<std::vector<QColor>> m_vctValidStateColor;

    //record degate state
    std::vector<RECORD_DEGATE_STATE_ENUM> m_vctAllchDegatState;
    std::vector<int> m_vctChannelNum;
    std::vector<float> m_vctCapValue;
private:
    bool UpdateChSensorGroupAndFuncBySensorState(const SENSOR_STATE_ENUM &chSensorState,SENSOR_STATE_GROUP_ENUM &reChSensorGrp,SENSORGRP_FUNC_STATE_ENUM &reChFuncState);
public:
    std::vector<SENSOR_STATE_ENUM> &GetSensorStateData(void);
    std::vector<OVF_STATE_ENUM> &GetOVFStateData(void);
    std::vector<CAP_STATE_ENUM> &GetCapStateData(void);
    std::vector<OPERATE_STATE_ENUM> & GetLockStateDataBySensorGrp(const SENSOR_STATE_GROUP_ENUM &sensorGrp);
    std::vector<float> & GetCapVueData(void);
    std::vector<SENSOR_STATE_GROUP_ENUM> & GetAllChSensorGroup(void);
    std::vector<SENSOR_STATE_GROUP_ENUM> & GetAllChMuxStateRef(void);
    std::vector<SENSORGRP_FUNC_STATE_ENUM> & GetAllChFuncState(void);
    std::vector<std::vector<OPERATE_STATE_ENUM>>& GetFourLockStateData(void);
    std::vector<std::vector<PORE_STATE_ENUM>> & GetFourPoreStateData(void);
    std::vector<std::vector<CHANNEL_VALID_ENUM>> &GetAllChFourValidStateRef(void){ return m_vetChanIsValid;}
    std::vector<RECORD_DEGATE_STATE_ENUM> &GetAllChDegateStateData(void);
    CHANNEL_VALID_ENUM GetCurChanValidState(const int &ch);
    SENSOR_STATE_GROUP_ENUM GetCurChanSensorGroup(const int &ch){ return m_vetChanCurGroup[ch];}
    void SetCurChanValidState(const int &ch,const CHANNEL_VALID_ENUM &validState);
    void UpdateAllChSensorGroupAndFuncStateBySensorState(void);
};

