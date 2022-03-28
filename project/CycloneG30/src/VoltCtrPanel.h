#pragma once

#include <QWidget>
#include "Global.h"

class QPushButton;
class QSpinBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class UsbFtdDevice;


class VoltCtrPanel:public QWidget
{
    Q_OBJECT
public:
    explicit VoltCtrPanel(QWidget *parent = nullptr);
    ~VoltCtrPanel();

    void SetUsbDevicePtr(UsbFtdDevice* pUsbFtdDevice) { m_pUsbFtdDevice = pUsbFtdDevice; }
    void VoltInital(void);

private:

    QDoubleSpinBox *m_pDac0MinVoltdpbx = nullptr;
    QDoubleSpinBox *m_pDac0MaxVoltdpbx = nullptr;

    QDoubleSpinBox *m_pDac1MinVoltdpbx = nullptr;
    QDoubleSpinBox *m_pDac1MaxVoltdpbx = nullptr;
    QPushButton *m_pDacRangeInpAplyBtn = nullptr;
    QPushButton *m_pDacReadBackBtn =nullptr;

    QComboBox *m_pVoltTypeComb = nullptr;
    QDoubleSpinBox *m_pVoltAmpInpDpbx = nullptr;
    QDoubleSpinBox *m_pVoltTrianSinAmpInpDpbx = nullptr;
    QLabel *m_pVoltFreqInpLab = nullptr;
    QComboBox *m_pVoltFreqInpComb = nullptr;
    QPushButton *m_pDac0VoltApplyBtn = nullptr;
    QPushButton *m_pDac0NegPosiApplyBtn = nullptr;

    QDoubleSpinBox *m_pUnblockVoltInpDpbx = nullptr;
    QPushButton *m_pUnblockVoltApplyBtn = nullptr;


private:
    void InitCtrl(void);

    double m_vcmVolt;
    double m_dac0LSB;
    double m_dac0VoltSet;

    double m_dac1LSB;
    double m_dac1VoltSet;
    double m_dac1MidVolt;
    double m_dac1MaxInput;
    double m_dac1MinInput;
    double m_unblockVolt;

    UsbFtdDevice *m_pUsbFtdDevice = nullptr;
private:
    int16_t CalcDACHexByDiffer(const double &voltSet,const double &midVolt,const double &lsb);
    int16_t CalcDACOneHexByDiffer(const double &voltSet,const double &midVolt,const double &lsb);
    void TriangularWaveGener(const double &trianAmp);
    void SinWaveGener(const double &sinAmp);
    bool DacReadBackByExtnADC(float &dac0MinVolt,float &dac0MaxVolt,float &dac1MinVolt,float &dac1MaxVolt);
signals:
    void AllChanSensorStateSetOffSig(void);

public slots:
    void TrianStartAndGetAmplitudeSlot(const bool bStart,double &amplitude);

    void VoltageSetSlot(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double &freq);
    void VoltSetParameters(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double freq = 7.8125);
    void DegatVoltSetSlot(const float &negativeVolt);

    void EnableSimulationSlot(const bool &enable);
    void SetSimulationVoltSlot(const float &voltage,const MEMBRANE_SIMULATION_ENUM &type);
    void EnablePolymerSimulationSlot(const bool &enable);
    void OnDacReadBackByExtnADCBtnSlot(void);

private slots:
    void CustomVoltRangeSetBtnSlot(void);
    void VoltTypeChangeSlot();
    void VoltApplyBtnSlot(void);
    void UnblockVoltApplyBtnSlot(void);
    void NegPosiVoltApplyBtnSlot(void);
};

