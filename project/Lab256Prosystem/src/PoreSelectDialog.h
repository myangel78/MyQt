#pragma once

#include <QDialog>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qsettings.h>
#include <qtimer.h>


class ControlView;

class PoreSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PoreSelectDialog(QWidget *parent = nullptr);


    static const int m_sciFlow = 10;
private:
    QGroupBox* m_pgrpMakeMembrance = nullptr;
    QCheckBox* m_pchkMakeMembrance[m_sciFlow] = { nullptr };
    QDoubleSpinBox* m_pdspMakeMembrance[m_sciFlow] = { nullptr };
    QSpinBox* m_pispMakeMembrance[m_sciFlow] = { nullptr };
    QLabel* m_plabMakeMembranceNum = nullptr;
    QSpinBox* m_pispMakeMembranceNum = nullptr;

    QGroupBox* m_pgrpPoreInsert = nullptr;
    QCheckBox* m_pchkPoreInsert[m_sciFlow] = { nullptr };
    QDoubleSpinBox* m_pdspPoreInsert[m_sciFlow] = { nullptr };
    QSpinBox* m_pispPoreInsert[m_sciFlow] = { nullptr };
    QLabel* m_plabPoreInsertNum = nullptr;
    QSpinBox* m_pispPoreInsertNum = nullptr;

    QGroupBox* m_pgrpChannelSel = nullptr;
    QCheckBox* m_pchkChannelSel[m_sciFlow] = { nullptr };
    QDoubleSpinBox* m_pdspChannelSel[m_sciFlow] = { nullptr };
    QSpinBox* m_pispChannelSel[m_sciFlow] = { nullptr };
    QDoubleSpinBox* m_pdspCurrentMin = nullptr;
    QDoubleSpinBox* m_pdspCurrentMax = nullptr;

    QGroupBox* m_pgrpFinishedVolt = nullptr;
    QLabel* m_plabFinishedVolt = nullptr;
    QDoubleSpinBox* m_pdspFinishedVolt = nullptr;


    QGroupBox* m_pgrpOperation = nullptr;
    QLineEdit* m_peditModeName = nullptr;
    QComboBox* m_pcmbModeName = nullptr;
    QPushButton* m_pbtnAddMode = nullptr;
    QPushButton* m_pbtnDelMode = nullptr;
    QPushButton* m_pbtnEditMode = nullptr;
    QPushButton* m_pbtnStartMode = nullptr;

    QSettings* m_pConfig = nullptr;

    QTimer m_tmrStartMode;
    QTimer m_tmrPoreFilter;

    bool m_bNeedZeroAdjust = false;
    ControlView* m_pControlView = nullptr;

public:
    struct SVoltTime
    {
        bool selchk = false;
        double selvolt = 0.0;
        int seltime = 0;
    };

    struct SPoreSelectData
    {
        std::string modeName;

        std::vector<SVoltTime> voltTime1;
        int loopCount1 = 0;

        std::vector<SVoltTime> voltTime2;
        int loopCount2 = 0;

        bool filterchk = false;
        std::vector<SVoltTime> voltTime3;
        double minFilter = 0.0;
        double maxFilter = 0.0;

        double finvolt = 0.0;
    };

    std::vector<SPoreSelectData> m_vetPoreSelectData;

    std::atomic<bool> m_bRunning = false;

private:
    bool InitCtrl(void);
    bool InitConfig(const char* pfile);

protected:
    void closeEvent(QCloseEvent* evn);

public:
    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);

    void SetNeedZeroAdjust(bool val) { m_bNeedZeroAdjust = val; }
    void SetControlView(ControlView* pview) { m_pControlView = pview; }


public slots:
    void OnChangeModeName(int val);
    void OnClickAddMode(void);
    void OnClickDelMode(void);
    void OnClickEditMode(void);
    void OnClickStartMode(bool chk);

    void OnTimerStartModeSlot(void);
    void TimerPoreFilterSlot(void);

signals:
    //void SetAllChannelVoltSignal(double ampl);
    void SetSelChannelDCLevelSignal(double volt);
    void SetPoreFilterChannelSignal(double minval, double maxval);
    void SetNotPoreFilterDCLevelSignal(double ampl);
    void SetPoreFilterDCLevelSignal(double volt);

    void EnablePoreInsertSignal(bool val);
};

