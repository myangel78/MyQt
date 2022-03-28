#include "PoreStateMapTabModel.h"
#include <QColor>
#include <QBrush>
#include <iostream>
#include <vector>

PoreStateMapTabModel::PoreStateMapTabModel(QObject * parent):QAbstractTableModel(parent),m_rowCount(MAPPING_ROW),m_columnCount(MAPPING_COLUMN)
{
    InitData();

}

void PoreStateMapTabModel::InitData(void)
{
    m_displayMode = SENSOR_STATE_MAP_MODE;
    m_displayContentMode = NO_CONTENT_DISPLAY_MODE;
    m_lockStateGrpSelect = SENSOR_STATE_A_GROUP;
    m_poreStateGrpSlect = SENSOR_STATE_A_GROUP;
    m_pValidStateGrpSlect = SENSOR_STATE_A_GROUP;
}


void PoreStateMapTabModel::RestState(const int &mode,const int &state,const int sensorGrp)
{
    switch (mode) {
    case SENSOR_STATE_MAP_MODE:
    {
        SENSOR_STATE_ENUM sensorState = SENSOR_STATE_ENUM(state);
        m_pModelSharedDataPtr->m_vectAllChSensorState.assign(CHANNEL_NUM,sensorState);
    }
        break;
    case OVF_STATE_MAP_MODE:
    {
        OVF_STATE_ENUM ovfState = OVF_STATE_ENUM(state);
        m_pModelSharedDataPtr->m_vectAllChOVFState.assign(CHANNEL_NUM,ovfState);
    }
        break;
    case CAP_STATE_MAP_MODE:
    {
        CAP_STATE_ENUM capState = CAP_STATE_ENUM(state);
        m_pModelSharedDataPtr->m_vectAllChCapState.assign(CHANNEL_NUM,capState);
    }
        break;
    case LOCK_SENSOR_STATE_MAP_MODE:
    {
        OPERATE_STATE_ENUM lockState = OPERATE_STATE_ENUM(state);
        m_pModelSharedDataPtr->m_vectAllChLockState[sensorGrp].assign(CHANNEL_NUM,lockState);
    }
        break;
    case PORE_STATE_MAP_MODE:
    {
        PORE_STATE_ENUM poreState = PORE_STATE_ENUM(state);
        m_pModelSharedDataPtr->m_vectAllChPoreState[sensorGrp].assign(CHANNEL_NUM,poreState);
    }
        break;
    case VALID_SENSOR_STATE_MAP_MODE:
    {
        CHANNEL_VALID_ENUM validState = CHANNEL_VALID_ENUM(state);
        std::fill_n(m_pModelSharedDataPtr->m_vetChanIsValid[sensorGrp].begin(),CHANNEL_NUM,validState);
    }
        break;
    case MUX_STATE_MAP_MODE:
    {
        SENSOR_STATE_GROUP_ENUM muxSensorGrp = SENSOR_STATE_GROUP_ENUM(state);
        std::fill_n(m_pModelSharedDataPtr->m_vetAllChMuxState.begin(),CHANNEL_NUM,muxSensorGrp);
    }
        break;
    default:
        break;
    }
    UpdateAllChDataDisplayByRole(Qt::BackgroundRole);

}

void PoreStateMapTabModel::ChangeDisplayMode(const int &mode)
{
    m_displayMode = mode;
    UpdateAllChDataDisplayByRole(Qt::BackgroundRole);

}

void PoreStateMapTabModel::ChangeSensorValidGroup(const SENSOR_STATE_GROUP_ENUM &group)
{
    m_pValidStateGrpSlect = group;
    UpdateAllChDataDisplayByRole(Qt::BackgroundRole);
}

void PoreStateMapTabModel::ChangeSensorLockGroup(const SENSOR_STATE_GROUP_ENUM &group)
{
    m_lockStateGrpSelect = group;
    UpdateAllChDataDisplayByRole(Qt::BackgroundRole);
}

void PoreStateMapTabModel::ChangeSensorPoreGroup(const SENSOR_STATE_GROUP_ENUM &group)
{
    m_poreStateGrpSlect = group;
    UpdateAllChDataDisplayByRole(Qt::BackgroundRole);
}


void PoreStateMapTabModel::ChangeDisplayContentMode(const int &mode)
{
    m_displayContentMode = mode;
    UpdateAllChDataDisplayByRole(Qt::DisplayRole);
}

int PoreStateMapTabModel::columnCount(const QModelIndex & /*parent = QModelIndex()*/) const
{
    return m_columnCount;
}


int PoreStateMapTabModel::rowCount(const QModelIndex & /* parent = QModelIndex()*/) const
{
    return m_rowCount;
}


QColor PoreStateMapTabModel::GetBackgroudRoleColor(const int &mode,const int &state) const
{
    switch (mode) {
    case SENSOR_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vctSensorStateColr[state];
    case CAP_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vctCapStateColr[state];
    case OVF_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vctOVFStateColr[state];
    case PORE_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vctPoreStateColor[m_poreStateGrpSlect][state];
    case LOCK_SENSOR_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vctLockStateColr[m_lockStateGrpSelect][state];
    case VALID_SENSOR_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vctValidStateColor[m_pValidStateGrpSlect][state];
    case MUX_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vctMuxStateColor[state];
    default:
        break;
    }
    return Qt::white;
}

int PoreStateMapTabModel::GetChStateFromMode(const int &mode,const int &ch) const
{
    switch (mode) {
    case SENSOR_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vectAllChSensorState[ch];
    case CAP_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vectAllChCapState[ch];
    case OVF_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vectAllChOVFState[ch];
    case PORE_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vectAllChPoreState[m_poreStateGrpSlect][ch];
    case LOCK_SENSOR_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vectAllChLockState[m_lockStateGrpSelect][ch];
    case VALID_SENSOR_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vetChanIsValid[m_pValidStateGrpSlect][ch];
    case MUX_STATE_MAP_MODE:
        return m_pModelSharedDataPtr->m_vetAllChMuxState[ch];
    default:
        break;
    }
    return 0;
}



QVariant PoreStateMapTabModel::data(const QModelIndex & index, int role) const
{
    if (!IndexIsValid(index))
        return QVariant();

    int ch  = GetChannelByIndx(index);

    switch (role) {
    case Qt::DisplayRole:
       return DisplayModeData(m_displayMode,m_displayContentMode,ch);
    case Qt::BackgroundRole:
        return GetBackgroudRoleColor(m_displayMode,GetChStateFromMode(m_displayMode,ch));
    case Qt::TextAlignmentRole:
         return Qt::AlignCenter;
    default:
        break;
    }
    return QVariant();
}

QVariant PoreStateMapTabModel::DisplayModeData(const int &stateMode,const int &contMode, const int &ch) const
{
    switch (contMode) {
    case NO_CONTENT_DISPLAY_MODE:
        return QVariant();
    case CHANNEL_NUM_DISPLAY_MODE:
        return m_pModelSharedDataPtr->m_vctChannelNum[ch];
    case CAP_VALUE_DISPLAY_MODE:
        if(m_displayMode == CAP_STATE_MAP_MODE)
        {
            return QString::number(m_pModelSharedDataPtr->m_vctCapValue[ch],'f',1);
            return m_pModelSharedDataPtr->m_vctCapValue[ch];
        }
        break;
    case STATE_VALUE_DISPLAY_MODE:
         return GetChStateFromMode(stateMode,ch);
    default:
        break;
    }
    return QVariant();
}



QVariant PoreStateMapTabModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal)
        return QString("%1").arg(section);
    else
        return QString("%1").arg(section);
}

bool PoreStateMapTabModel::IndexIsValid(const QModelIndex & index) const
{
    return index.isValid() && index.row() < rowCount() && index.column() < columnCount();
}

int PoreStateMapTabModel::GetChannelByIndx(const QModelIndex &index)const
{
//    return CHANNEL_GET_BY_INDEX_ARRAY[index.row()][index.column()];
    return UNORDERCHANNEL_GET_BY_INDEX_ARRAY[index.row()][index.column()];
}


void PoreStateMapTabModel::SetStateColor(const int &mode,const int  &state,const QColor &color,const int sensorGrp)
{
    switch (mode) {
    case SENSOR_STATE_MAP_MODE:
        m_pModelSharedDataPtr->m_vctSensorStateColr[state] = color;
        break;
    case CAP_STATE_MAP_MODE:
        m_pModelSharedDataPtr->m_vctCapStateColr[state] = color;
        break;
    case OVF_STATE_MAP_MODE:
        m_pModelSharedDataPtr->m_vctOVFStateColr[state] = color;
        break;
    case PORE_STATE_MAP_MODE:
        m_pModelSharedDataPtr->m_vctPoreStateColor[sensorGrp][state] = color;
        break;
    case LOCK_SENSOR_STATE_MAP_MODE:
        m_pModelSharedDataPtr->m_vctLockStateColr[sensorGrp][state] = color;
        break;
    case VALID_SENSOR_STATE_MAP_MODE:
        m_pModelSharedDataPtr->m_vctValidStateColor[sensorGrp][state] = color;
        break;
    case MUX_STATE_MAP_MODE:
        m_pModelSharedDataPtr->m_vctMuxStateColor[state] = color;
        break;
    default:
        break;
    }
}

bool PoreStateMapTabModel::UpdateCapValueModelData(const std::vector<float> &vctCapVue)
{
    std::copy(vctCapVue.cbegin(),vctCapVue.cend(),  m_pModelSharedDataPtr->m_vctCapValue.begin());
    UpdateAllChDataDisplayByRole(Qt::DisplayRole);
    return true;
}

void PoreStateMapTabModel::UpdateAllChDataDisplay(void)
{
    UpdateAllChDataDisplayByRole(Qt::BackgroundRole);
    if(m_displayMode == CAP_STATE_MAP_MODE && m_displayContentMode == CAP_VALUE_DISPLAY_MODE)
    {
        UpdateAllChDataDisplayByRole(Qt::DisplayRole);
    }
}

void PoreStateMapTabModel::UpdateAllChDataDisplayByRole(const Qt::ItemDataRole role)
{
    QModelIndex topleft =  index(0,0);
    QModelIndex bottomRight = index(rowCount()-1,columnCount()-1);
    QVector<int> vctDataChangeRole(1,role);
    emit dataChanged(topleft,bottomRight,vctDataChangeRole);
}
