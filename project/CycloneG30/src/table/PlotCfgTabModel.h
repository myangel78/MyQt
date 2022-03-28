#pragma once

#include <QAbstractTableModel>
#include <QColor>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QPixmap>
#include "Global.h"
#include "ModelSharedData.h"


class PLotCfgTabModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns { Channel = 0, Visible, Color,AVG,STD,Sequence,Valid};

    explicit PLotCfgTabModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);

    bool GetSpecChbVisible(const int &ch){return m_vetIsVisible[ch];}
    void SetSpecChbVisible(const int &ch,const bool &visible){m_vetIsVisible[ch] = visible;}

    QColor GetSpecChColor(const int &ch){return m_vetCurvesColor[ch];}

    void UpdateAllChVisible(const bool &chk);
    void UpdateAllChVisible(const std::vector<bool> &vetIsVisible);
    void UpdateAllChSeqFlag(const std::vector<bool> &vetSeqFlag);


    std::vector<float> &GetAllChAvgRef(void){ return m_vetAvg;}
    std::vector<float> &GetAllChStdRef(void){ return m_vetStd;}

    std::vector<bool> &GetAllChSeqSteRef(void){ return m_vetIsSeq;}
    std::vector<bool> &GetAllChVisibleRef(void){ return m_vetIsVisible;}


    bool GetCurChanValidState(const int &ch){ return m_pModelSharedDataPtr->GetCurChanValidState(ch) ;}

    void UpdateAllChDataDisplayByColumn(const PLotCfgTabModel::Columns &startColumn,const PLotCfgTabModel::Columns &endColumn,bool bIsAllRole = false,const Qt::ItemDataRole role = Qt::DisplayRole);
    void UpdateAllChDataDisplayByColumn(const PLotCfgTabModel::Columns &column);


private:
    std::vector<bool> m_vetIsVisible;
    std::vector<QColor> m_vetCurvesColor;
    std::vector<int> m_vetChannelNme;
    std::vector<float> m_vetAvg;
    std::vector<float> m_vetStd;
    std::vector<bool> m_vetIsSeq;
    QVector<int> m_vctDisplayRoles;

    QPixmap m_vetChanGrpPixMap[SENSOR_GROUP_NUM];

    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;
public:
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }

};


class PlotCfgSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum FILTER_TYPE_ENUM
    {
        FILTER_TYPE_CHANNEL =0,
        FILTER_TYPE_VISIBLE =1,
        FILTER_TYPE_AVG_STD =2,
        FILTER_TYPE_SEQUENCY =3,
        FILTER_TYPE_VALID =4,
    };
    explicit PlotCfgSortFilterModel(QObject *parent = nullptr);
    void ChangeFilterType(const FILTER_TYPE_ENUM &type) ;
    void OnSetAvgStdParamer(const double &avgMin,const double &avgMax, const double &stdMin,const double &stdMax);

    std::vector<bool> &GetAllChAvgStdFitRef(void){ return m_vetFitForAvgStd;}

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
//    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;

private:
    bool IsShowItems(const FILTER_TYPE_ENUM &type,const int &ch) const;
private:

    FILTER_TYPE_ENUM m_curFilterType;

    double m_avgMin;
    double m_avgMax;
    double m_stdMin;
    double m_stdMax;
    std::vector<bool> m_vetFitForAvgStd;
};


