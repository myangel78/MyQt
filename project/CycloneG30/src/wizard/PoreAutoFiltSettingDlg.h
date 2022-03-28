#pragma once
#include <QDialog>

class QSettings;
class QTabWidget;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QGroupBox;
class CustomDoubleSpinbox;
class CustomSpinbox;
class QSettings;
class QDialogButtonBox;



class PoreAutoFiltSettingDlg: public QDialog
{
    Q_OBJECT
public:
    explicit PoreAutoFiltSettingDlg(QWidget *parent = nullptr);
    virtual ~PoreAutoFiltSettingDlg();

    void accept() override;

    typedef struct SLockUnqualifedParameter
    {
        SLockUnqualifedParameter():OverScanVolt(0.18f),OverScanTimes(10){}
        float OverScanVolt;
        int OverScanTimes;
    }SLockUnqualifedParameter;
    typedef struct SPoreFiltToValidParameter
    {
        SPoreFiltToValidParameter(): DirectVolt(0.18f),WattingTime(8),SinglePoreMeanMin(175.0f),SinglePoreMeanMax(320.0f),\
            SinglePoreStdMin(0.8f),SinglePoreStdMax(10.0f),MultiPoreMeanMin(350.0f),MultiPoreMeanMax(1000.0f),MultiPoreStdMin(0.8f),\
            MultiPoreStdMax(10.0f){}
        float DirectVolt;
        int WattingTime;
        float SinglePoreMeanMin;
        float SinglePoreMeanMax;
        float SinglePoreStdMin;
        float SinglePoreStdMax;
        float MultiPoreMeanMin;
        float MultiPoreMeanMax;
        float MultiPoreStdMin;
        float MultiPoreStdMax;
    }SPoreFiltToValidParameter;


private:
    void InitCtrl(void);
    void InitPoreFiltLockUnqualifed(void);
    void InitPoreFiltToValid(void);

    void SaveParameter(void);

private:
    QSettings *m_pSettings = nullptr;
    QTabWidget *m_pTabWidget = nullptr;
    QDialogButtonBox *m_pDialogBtn = nullptr;

    //PoreFiltLockUnqualifedWdgt
    QWidget *m_pStepOneGrpBx = nullptr;
    QLabel *m_pDescribeStepOneTitle = nullptr;
    CustomDoubleSpinbox *m_pPoreFiltLockVoLtDpbx= nullptr;
    CustomSpinbox *m_pPoreFiltLockScanTimesSpbx=nullptr;
    SLockUnqualifedParameter m_SLockUnqualifedParameter;

    //PoreFiltToValidWdgt
    QWidget *m_pStepTwoGrpBx = nullptr;
    QLabel *m_pDescribeStepTwoTitle = nullptr;
    CustomDoubleSpinbox *m_pFiltDirectVoLtDpbx= nullptr;
    CustomSpinbox *m_FiltWatiTimeSpbx=nullptr;
    CustomDoubleSpinbox *m_pFiltSinglePoreStdMinDpbx= nullptr;
    CustomDoubleSpinbox *m_pFiltSinglePoreStdMaxDpbx= nullptr;
    CustomDoubleSpinbox *m_pFiltSinglePoreAvgMinDpbx= nullptr;
    CustomDoubleSpinbox *m_pFiltSinglePoreAvgMaxDpbx= nullptr;
    CustomDoubleSpinbox *m_pFiltMultPoreStdMinDpbx= nullptr;
    CustomDoubleSpinbox *m_pFiltMultPoreStdMaxDpbx= nullptr;
    CustomDoubleSpinbox *m_pFiltMultPoreAvgMinDpbx= nullptr;
    CustomDoubleSpinbox *m_pFiltMultPoreAvgMaxDpbx= nullptr;
    SPoreFiltToValidParameter m_SPoreFiltToValidParameter;

public:
    bool LoadConfig(QSettings* pSetting);
    bool SaveConfig(QSettings* pSetting);
    void SetConfigPtr(QSettings* pSetting){ m_pSettings = pSetting;}
    SLockUnqualifedParameter &GetPoreFiltLockParameter(void){return m_SLockUnqualifedParameter;}
    SPoreFiltToValidParameter &GetPoreFiltToValidParameter(void){return m_SPoreFiltToValidParameter;}
};

