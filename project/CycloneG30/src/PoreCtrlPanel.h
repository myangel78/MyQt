#pragma once

#include <QWidget>


class QPushButton;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class UsbFtdDevice;
class QTimer;
class QGroupBox;

class PoreCtrlPanel:public QWidget
{
    Q_OBJECT
public:
    explicit PoreCtrlPanel(QWidget *parent = nullptr);
    ~PoreCtrlPanel();

    void SetUsbDevicePtr(UsbFtdDevice* pUsbFtdDevice) { m_pUsbFtdDevice = pUsbFtdDevice; }
private:
    //average
    QGroupBox *m_poreInsertGrpbx =nullptr;
    QDoubleSpinBox *m_pSinglePoreInpDpbx = nullptr;
    QDoubleSpinBox *m_pMultPoreInpDpbx= nullptr;
    QDoubleSpinBox *m_pMembrokenInpDpbx= nullptr;
    QPushButton *m_pStartPoreInsertBtn= nullptr;

    QCheckBox *m_pIsPoreLockChkBox = nullptr;
    //std
    QDoubleSpinBox *m_pStdMinDpbx= nullptr;
    QDoubleSpinBox *m_pStdMaxDpbx= nullptr;
    QPushButton *m_pStdFiltStartBtn= nullptr;

    //membrane protect
    QGroupBox *m_pMembrProtectGrpbx =nullptr;
    QDoubleSpinBox *m_pMembrokeProtectInpDpbx= nullptr;
    QPushButton *m_pStartMembraneProtectBtn= nullptr;


    QTimer *m_pTmrPoreInsert = nullptr;
    QTimer *m_pTimerCurStd = nullptr;
    QTimer *m_pTimerMembrokeProtect = nullptr;
signals:
    void SigUpdateSensorState(void);
    void SigCalcCurStdForShut(void);
    void SigCalCurForPoreInsert(void);
    void SigCalCurForMembrokProtect(void);

private slots:
    void StartPoreInsetBtnSlot(bool checked);
    void StartStdFiltBtnSlot(bool checked);
    void StartMembBrkoProtectBtnSlot(bool checked);

    void TimeupToCalCurForPoreInsert(void);
    void CalcStdTimeoutSlot(void);
    void TimeUpCalAvgForMembProtctSlot(void);

private:
    void InitCtrl(void);

    UsbFtdDevice *m_pUsbFtdDevice = nullptr;

public slots:
    void SlotGetPoreLimitCurrInp(float &single,float &mulit,float &membrk);
    void SlotGetMembraneLimitCurrInp(float &membrk);
    void SlotGetStdLimitInput(float &stdMin,float &stdMax);
    void AcquireWhetherPoreLockSlot(bool &bPoreLock);
    void EnablePoreProtectSlot(const bool &enable,const float &poreLimtCurrent);
    void EnableMembrokProtectSlot(const bool &enable,const float &membrokLimtCurrent);
};
