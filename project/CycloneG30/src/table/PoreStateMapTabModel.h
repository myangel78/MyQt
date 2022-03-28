#pragma once
#include <QAbstractTableModel>
#include <QColor>
#include <array>

#include "Global.h"
#include "ModelSharedData.h"

class PoreStateMapTabModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PoreStateMapTabModel(QObject * parent = nullptr);

//    typedef QMap<int, QVariant> UnitData;


    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;

    // Make it editable
    bool IndexIsValid(const QModelIndex & index) const;
//    bool setData(const QModelIndex & index, const QVariant & value,
//        int role = Qt::EditRole) override;
private:
    void InitData(void);
    QVariant DisplayModeData(const int &stateMode,const int &contMode, const int &ch) const;

    int GetChannelByIndx(const QModelIndex &index) const;
    QColor GetBackgroudRoleColor(const int &mode,const int &state) const;

    int GetChStateFromMode(const int &mode,const int &ch) const;
    void UpdateAllChDataDisplayByRole(const Qt::ItemDataRole role = Qt::BackgroundRole );

private:
//    std::vector<std::vector<std::unique_ptr<UnitData>>> m_vectAllChState;


    //sensor state color


    int m_displayMode;
    int m_displayContentMode;
    int m_rowCount;
    int m_columnCount;
    SENSOR_STATE_GROUP_ENUM m_lockStateGrpSelect;
    SENSOR_STATE_GROUP_ENUM m_poreStateGrpSlect;
    SENSOR_STATE_GROUP_ENUM m_pValidStateGrpSlect;
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;


public:
    void RestState(const int &mode,const int &state,const int sensorGrp = SENSOR_STATE_A_GROUP);
    void SetStateColor(const int &mode,const int  &state,const QColor &color,const int sensorGrp = SENSOR_STATE_A_GROUP);

    void ChangeSensorValidGroup(const SENSOR_STATE_GROUP_ENUM &group);
    void ChangeSensorLockGroup(const SENSOR_STATE_GROUP_ENUM &group);
    void ChangeSensorPoreGroup(const SENSOR_STATE_GROUP_ENUM &group);
    void ChangeDisplayMode(const int &mode);
    void ChangeDisplayContentMode(const int &mode);
    bool UpdateCapValueModelData(const std::vector<float> &vctCapVue);

    void UpdateAllChDataDisplay(void);
    void ResetModel(void){beginResetModel(); endResetModel();}
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }


};

