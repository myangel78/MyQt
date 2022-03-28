#include "PlotCfgTabModel.h"
#include <QDebug>
#include <QColor>


#include "ConfigServer.h"

PLotCfgTabModel::PLotCfgTabModel(QObject *parent) : QAbstractTableModel(parent)
{
   m_vetIsVisible.resize(CHANNEL_NUM);
   m_vetCurvesColor.resize(CHANNEL_NUM);
   m_vetChannelNme.resize(CHANNEL_NUM);
   m_vetAvg.resize(CHANNEL_NUM);
   m_vetStd.resize(CHANNEL_NUM);
   m_vetIsSeq.resize(CHANNEL_NUM);
   m_vctDisplayRoles.resize(CHANNEL_NUM);

   for(int ch = 0; ch < CHANNEL_NUM; ++ch)
   {
       m_vetIsSeq[ch] = true;
       m_vetIsVisible[ch] = true;
       m_vetCurvesColor[ch] = g_vetPenColor[ch%32];
       m_vetChannelNme[ch] = ch;
       m_vetAvg[ch] = 0.0;
       m_vetStd[ch] = 0.0;
       m_vctDisplayRoles[ch] = Qt::DisplayRole;
   }

   m_vetChanGrpPixMap[SENSOR_STATE_A_GROUP]= QPixmap(":/img/img/SensorA.png").scaledToWidth(20,Qt::SmoothTransformation);
   m_vetChanGrpPixMap[SENSOR_STATE_B_GROUP]= QPixmap(":/img/img/SensorB.png").scaledToWidth(20,Qt::SmoothTransformation);
   m_vetChanGrpPixMap[SENSOR_STATE_C_GROUP]= QPixmap(":/img/img/SensorC.png").scaledToWidth(20,Qt::SmoothTransformation);
   m_vetChanGrpPixMap[SENSOR_STATE_D_GROUP]= QPixmap(":/img/img/SensorD.png").scaledToWidth(20,Qt::SmoothTransformation);


}

Qt::ItemFlags PLotCfgTabModel::flags(const QModelIndex &index) const
{
    if(index.column() == Color)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return QAbstractTableModel::flags(index);
}

int PLotCfgTabModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_vetChannelNme.size();
}

int PLotCfgTabModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : Valid+1;
}

QVariant PLotCfgTabModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_vetChannelNme.size())
    {
        return QVariant();
    }

    int ch = index.row();

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column())
        {
            case Channel:
                if(role == Qt::DisplayRole)
                    return  QString("CH %1").arg(m_vetChannelNme[index.row()] +1);
                else
                    return m_vetChannelNme[index.row()];
            case Visible:
                return m_vetIsVisible[index.row()];
            case Color:
                return m_vetCurvesColor[index.row()];
            case AVG:
                return QString::number(m_vetAvg[index.row()],'f',1);
            case STD:
                return QString::number(m_vetStd[index.row()],'f',1);
            case Sequence:
                return m_vetIsSeq[index.row()];
            case Valid:
            {
                return m_pModelSharedDataPtr->GetCurChanValidState(ch);
            }
            default:
                break;
        }
        break;
    case Qt::DecorationRole:
        switch (index.column())
        {
        case Valid:
        {
            int ch = index.row();
            const auto chSensorGrp = m_pModelSharedDataPtr->GetCurChanSensorGroup(ch);
            return m_vetChanGrpPixMap[chSensorGrp];
        }
        default:
            break;
        }
        break;
    }

    return QVariant();
}


QVariant PLotCfgTabModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case Channel:
            return "Channel";
        case Visible:
            return "Visible";
        case Color:
            return "Color";
        case AVG:
            return "Average";
        case STD:
            return "Std";
        case Sequence:
            return "Sequence";
        case Valid:
            return "Valid";
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

bool PLotCfgTabModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_vetChannelNme.size())
    {
        return false;
    }
    const QVariant oldValue = PLotCfgTabModel::data(index, role);
        if (value == oldValue
            && value.isNull() == oldValue.isNull())
            return true;

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column())
        {
            case Visible:
            {
                m_vetIsVisible[index.row()] = value.toBool();
                emit dataChanged(index, index);
                return true;
            }
            case Color:
            {
                m_vetCurvesColor[index.row()] = value.value<QColor>();
                emit dataChanged(index, index);
                return true;
            }
            case Sequence:
            {
                m_vetIsSeq[index.row()] = value.toBool();
                emit dataChanged(index, index);
                return true;
            }
            case Valid:
            {
                int ch = index.row();
                m_pModelSharedDataPtr->SetCurChanValidState(ch,CHANNEL_VALID_ENUM(value.toBool()));
                emit dataChanged(index, index);
                return true;
            }
            default:
                return false;
        }
     default:
        break;
    }

    return false;
}



void PLotCfgTabModel::UpdateAllChVisible(const bool &chk)
{
    for(int ch = 0; ch < m_vetIsVisible.size(); ++ch)
    {
        m_vetIsVisible[ch] = chk;
    }
    UpdateAllChDataDisplayByColumn(Visible);
}

void PLotCfgTabModel::UpdateAllChVisible(const std::vector<bool> &vetIsVisible)
{
    std::copy(vetIsVisible.cbegin(),vetIsVisible.cend(), m_vetIsVisible.begin());
    UpdateAllChDataDisplayByColumn(Visible);
}



void PLotCfgTabModel::UpdateAllChSeqFlag(const std::vector<bool> &vetSeqFlag)
{
    std::copy(vetSeqFlag.cbegin(),vetSeqFlag.cend(), m_vetIsSeq.begin());
    UpdateAllChDataDisplayByColumn(Sequence);
}

void PLotCfgTabModel::UpdateAllChDataDisplayByColumn(const PLotCfgTabModel::Columns &startColumn,const PLotCfgTabModel::Columns &endColumn,bool bIsAllRole,const Qt::ItemDataRole role)
{
    QModelIndex topleft =  index(0,startColumn);
    QModelIndex bottomRight = index(CHANNEL_NUM-1,endColumn);
    if(bIsAllRole)
    {
        emit dataChanged(topleft,bottomRight);
    }
    else
    {
         QVector<int> vctDataChangeRole(1,role);
         emit dataChanged(topleft,bottomRight,vctDataChangeRole);
    }

}

void PLotCfgTabModel::UpdateAllChDataDisplayByColumn(const PLotCfgTabModel::Columns &column)
{
    UpdateAllChDataDisplayByColumn(column,column,false,Qt::DisplayRole);
    if(column == Valid)
    {
        UpdateAllChDataDisplayByColumn(column,column,false,Qt::DecorationRole);
    }
}


PlotCfgSortFilterModel::PlotCfgSortFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_curFilterType = FILTER_TYPE_CHANNEL;
    m_vetFitForAvgStd.resize(CHANNEL_NUM);
    for(int ch  = 0; ch < CHANNEL_NUM; ++ch)
    {
        m_vetFitForAvgStd[ch] = false;
    }
    m_avgMin = 0;
    m_avgMax = 0;
    m_stdMin = 0;
    m_stdMax = 0;
}



bool PlotCfgSortFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{

    //通过当前视图中的index位置获取model中实际的数据
    QVariant leftData = sourceModel()->data(source_left);
    QVariant rightData = sourceModel()->data(source_right);


    switch ( source_left.column() )
    {
    case PLotCfgTabModel::Channel:
        return leftData.toString().mid(3).toInt() > rightData.toString().mid(3).toInt() ;
    case PLotCfgTabModel::AVG:
    case PLotCfgTabModel::STD:
//        return false;
        return leftData.toFloat() < rightData.toFloat();
    case PLotCfgTabModel::Visible:
    case PLotCfgTabModel::Sequence:
    case PLotCfgTabModel::Valid:
        return leftData.toBool() < rightData.toBool();
    case PLotCfgTabModel::Color:
        return leftData.value<QColor>().name() > rightData.value<QColor>().name();
    default :
        return false;
        break;
    }
    return false;

}

bool PlotCfgSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    int ch =  sourceModel()->index(source_row, PLotCfgTabModel::Channel, source_parent).data(Qt::EditRole).toInt();
    return IsShowItems(m_curFilterType,ch);
}


bool PlotCfgSortFilterModel::IsShowItems(const FILTER_TYPE_ENUM &type,const int &ch) const
{
    PLotCfgTabModel *pPLotCfgTabModel = dynamic_cast<PLotCfgTabModel *>(sourceModel());
    if(pPLotCfgTabModel)
    {
        switch (type)
        {
        case FILTER_TYPE_CHANNEL:
            return true;
            break;
        case FILTER_TYPE_VISIBLE:
        {
            const auto  &visilbe = pPLotCfgTabModel->GetAllChVisibleRef();
            return visilbe[ch];
        }
        case FILTER_TYPE_SEQUENCY:
        {
            const auto  &vctSeq = pPLotCfgTabModel->GetAllChSeqSteRef();
            return vctSeq[ch];
        }
        case FILTER_TYPE_AVG_STD:
        {
            return m_vetFitForAvgStd[ch];
        }
        case FILTER_TYPE_VALID:
        {
            return pPLotCfgTabModel->GetCurChanValidState(ch);
        }
        default:
            break;
        }
    }
    return false;
}



void PlotCfgSortFilterModel::ChangeFilterType(const FILTER_TYPE_ENUM &type)
{
    m_curFilterType = type;
    invalidateFilter();
}

void PlotCfgSortFilterModel::OnSetAvgStdParamer(const double &avgMin,const double &avgMax, const double &stdMin,const double &stdMax)
{
    m_avgMin = avgMin;
    m_avgMax = avgMax;
    m_stdMin = stdMin;
    m_stdMax = stdMax;
}



