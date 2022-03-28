#pragma once

#include <QWidget>
#include <QTime>
#include "Global.h"
#include "ModelSharedData.h"

class QPushButton;
class QSpinBox;
class QComboBox;
class QRadioButton;
class QProgressDialog;
class QEventLoop;
class QTimer;
class UsbFtdDevice;
class PoreStateMapWdgt;
class QLabel;
class QDoubleSpinBox;
class QLCDNumber;
class QFile;
class SavePannel;


class SensorPanel:public QWidget
{
    Q_OBJECT
public:
    SensorPanel(QWidget *parent = nullptr);
    ~SensorPanel();

    void SetUsbDevicePtr(UsbFtdDevice* pUsbFtdDevice) { m_pUsbFtdDevice = pUsbFtdDevice; }


private:

    QPushButton *m_pAllChSelBtn =nullptr;
    QSpinBox *m_pStartChpSelSpbox = nullptr;
    QSpinBox *m_pEndChSelSpbox = nullptr;
    QComboBox *m_pSensorFuncStateComb = nullptr;
    QPushButton *m_pSensorApplyBtn =nullptr;

    QRadioButton *m_pSensorASelbtn = nullptr;
    QRadioButton *m_pSensorBSelbtn = nullptr;
    QRadioButton *m_pSensorCSelbtn = nullptr;
    QRadioButton *m_pSensorDSelbtn = nullptr;
    QRadioButton *m_pSensorOffSelbtn = nullptr;
    QRadioButton *m_pSensorCurrSelbtn = nullptr;
    QRadioButton *m_pSensorMuxSelbtn = nullptr;

    QTimer *m_pTimerToDegating = nullptr;
    QDoubleSpinBox *m_pTimerIntalInptDpbx = nullptr;
    QPushButton *m_pTimerAutoDegatinBtn = nullptr;
    QTime m_tmAutoDegate;
    QLCDNumber* m_plcdTimerPeriod = nullptr;


    UsbFtdDevice *m_pUsbFtdDevice = nullptr;

signals:
    void SigClearPoreAndCapState(void);
    void UpdateCurrentSensorGrpStrSig(const QString &sensorGrpStr);
    void TempControlDegateSig(const bool &enable);
    void AcquireDegateDelaymsSig(int &delayms);
    void AcquireDegateIsRunningSig(bool &bIsRuning);

    void AutoScanControlPauseSig(const bool &runOrPause);
    void AcquireAutoScanIsRunningSig(bool &bIsRunning);
    void AcqurieOpenFileTimeSig(QDateTime &datetime);


private slots:
    void OnAllSensorSelectModeChgeBtnSlot(const int &buttonId);
    void SensorGrpCustomSelctBtnSlot(void);
    void OnTimerToAutoDegatSlot(void);
    void OnAutoDegateBtnSlot(const bool &checked);
private:
    void InitCtrl(void);
    bool UpdateRegCacheByChState(std::vector<StateRegUnion> &vctSensorRegAry, const int &channel,const int &state);
    bool SetTimehms(void);

    bool GenerateChannelArray(const int &block,std::vector<int> &vctIncoherentCh);

    //设置channel normal/unblock/internal source/shutoff 接口，只操作接口，不操作sensorstate
    bool AllChSameFuncStateSet(const SENSORGRP_FUNC_STATE_ENUM &funcState);
    bool SingleChSameFuncStateSet(const int &ch, const SENSORGRP_FUNC_STATE_ENUM &funcState);
    bool CustomChSameFuncStateSet(const int &startch,const int &endch,const SENSORGRP_FUNC_STATE_ENUM &funcState);
    bool AllChDifferFuncStateSet(const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState);
    bool CustomChDifferFuncStateSet(const int &startch,const int &endch,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState);
    bool IncontinueChDifferFuncStateSet(const std::vector<int> &vctIncoherentCh,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctIncoherentState);
    bool SensorStateCommandSendToFPGA(const int &startch,const int &endch);
    bool IncontinueSensorStateCommandSendToFPGA(const std::vector<int> &vctChannel);
    SENSOR_STATE_ENUM GetSensorStateByFuncState(const SENSOR_STATE_GROUP_ENUM &sensorGrp,const SENSORGRP_FUNC_STATE_ENUM &funcState);

public slots:
    void OneChanSensorSpeciSetSlot(int chan, int sensorState);
    void SpeChanSensorSpeciSetSlot(int startch, int endch, const std::vector<int> &vctSensorState);
    void AllChanSensorStateSetOffSlot(void);
    void EnableSimulationSlot(const bool &enable);
    void OnUnblockAllChannelSlot(const int &delayms);
    void SensorGroupChangeSlot(const SENSOR_STATE_GROUP_ENUM &sensorGrp);
    void SensorGroupAndFuncChangeSlot(const SENSOR_STATE_GROUP_ENUM &sensorGrp,const SENSORGRP_FUNC_STATE_ENUM &allChFuncState );
    void OnMuxModeSet(void);



    void SpecificChanSetFunctStateSlot(const int &ch,const SENSORGRP_FUNC_STATE_ENUM &funcState);
    void AllChDifferFuncStateSetSlot(const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctfuncState);
    void IncontinueChDifferFuncStateSetSlot(const std::vector<int> &vctIncoherentCh,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctIncoherentState);
    void IncontinueChSameFuncStateSetSlot(const std::vector<int> &vctIncoherentCh,const SENSORGRP_FUNC_STATE_ENUM &funcState);

    void AutoTimerDegatControlSlot(const bool &runOrPause);
    void AcquireAutoTimerDegatIsRunningSlot(bool &bIsRunning);
    void TimerDegatEanbleSlot(const bool &enable);
    bool AllChannelAllInternalSourceSet(void);



public:
    void SetPoreStateMapWdgtPtr(PoreStateMapWdgt* pPoreStateMapWdgt) { m_pPoreStateMapWdgt = pPoreStateMapWdgt; }
    void SetModelShareDataPtr(std::shared_ptr<ModelSharedData> pModelSharedDataPtr){m_pModelSharedDataPtr = pModelSharedDataPtr; }
    void SetSavePannelPtr(SavePannel *pSavepannel){m_pSavePannel = pSavepannel;}
    void RecordDegateLog(const std::vector<int> &vctIncoherentCh,const std::vector<SENSORGRP_FUNC_STATE_ENUM> &vctIncoherentState);
private:
    PoreStateMapWdgt *m_pPoreStateMapWdgt = nullptr;
    std::shared_ptr<ModelSharedData> m_pModelSharedDataPtr;

    SavePannel *m_pSavePannel = nullptr;

};

