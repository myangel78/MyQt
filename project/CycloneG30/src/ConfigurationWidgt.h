#pragma once

#include <QObject>
#include <QWidget>
#include "Global.h"

class UsbFtdDevice;
class QPushButton;
class QLabel;
class QComboBox;
class QTabWidget;
class QLineEdit;
class QSlider;
class QSpinBox;
class QGroupBox;
class QDoubleSpinBox;
class HexSpinBox;
class QProgressDialog;
class QEventLoop;
class QRadioButton;
class QButtonGroup;
class QSettings;


class ConfigurationWidgt:public QWidget
{
    Q_OBJECT
public:
    ConfigurationWidgt(UsbFtdDevice *pUsbFtdDevice,QWidget *parent =nullptr);
    ~ConfigurationWidgt();

    void SetEnable(const bool &enable);
    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);

private:
    typedef enum{
        ENUM_RAW_DATA = 0,
        ENUM_RAW_ADC_DATA = 1,
    }IS_RAW_ADC_ENUM;

    typedef enum{
        ENUM_NOT_TO_TRANS= 0,
        ENUM_NEED_TO_TRANS = 1,
    }IS_NEED_TRANS_ENUM;


    UsbFtdDevice *m_pUsbFtdDevice = nullptr;

    QPushButton *m_pSysInitBtn = nullptr;
    QPushButton *m_pSysLoadConfBtn = nullptr;

    QLabel *m_pAdcFreqLab = nullptr;
    QComboBox *m_pAdcFreqComb = nullptr;

    QLabel *m_pIsRawDatalab = nullptr;
    QRadioButton *m_pIsUnSignRawDataBtn = nullptr;
    QRadioButton *m_pIsSignAdcDataBtn = nullptr;

    QLabel *m_pbTransDatalab = nullptr;
    QRadioButton *m_pbNeedToTransBtn = nullptr;
    QRadioButton *m_pbNotToTransBtn = nullptr;

    QLabel *m_pInCurrentLab = nullptr;
    QComboBox *m_pInCurrentArrComb[LVDS_SAMPLE_KIINDS_NUM];

    QPushButton *m_pReadConfBtn = nullptr;
    QPushButton *m_pWriteConfBtn = nullptr;


    QProgressDialog *m_pProgressDlg;
    QEventLoop *m_pProgressEventLoop;
    QTimer *m_pProgresssTimer;

    int m_samprate[LVDS_SAMPLE_KIINDS_NUM];

    QGroupBox *m_pOtherSettingGrpbx = nullptr;
    QLabel *m_pbPowerComsumplab = nullptr;
    QRadioButton *m_pLowPowerComsumpBtn = nullptr;
    QRadioButton *m_pHighPowerComsumpBtn = nullptr;
    QPushButton *m_pOtherReadConfBtn = nullptr;
    QPushButton *m_pOtherWriteConfBtn = nullptr;


    //advance setting
    QGroupBox *m_pAdvanceSettingGrpbx = nullptr;
    QLabel *m_pVoltInverterlab = nullptr;
    QRadioButton *m_pVoltUnInverterBtn = nullptr;
    QRadioButton *m_pVoltInverterBtn = nullptr;
    QButtonGroup *m_pVoltInvertBtnGrp= nullptr;
    QPushButton *m_pAllIntalSoureSetBtn = nullptr;
signals:
    void SigSampleRateChanged(void);
    void SigAFEPowerConsumpChange(const POWER_CONSUMPTION_ENUM &flag);
    void SigSetAllChIntalSource(void);
private:
    void InitCtrl(void);
    void InitAdvance(void);

    bool OnOtherReadConf(void);
    bool OnOtherWriteConf(void);
private slots:
    void OnReadConfBtnSlot(void);
    void OnWriteConfBtnSlot(void);
    void OnLoadRegBtnSlot(void);
    void SampleRateChangeSlot(void);
    void InitSysCfgSlot(void);
    void LVDSCurRangeChangeSlot(void);

    void OnOtherReadConfBtnSlot(void);
    void OnOtherWriteConfBtnSlot(void);
    void PermissionToModifyAdvanceSlot(const bool &checked);
};

