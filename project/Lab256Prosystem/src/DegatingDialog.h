#pragma once

#include <QDialog>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qdialogbuttonbox.h>


class DegatingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DegatingDialog(QWidget *parent = nullptr);

private:
    QLabel* m_plabDegatingVolt;
    QDoubleSpinBox* m_pdspDegatingVolt;
    QLabel* m_plabCyclePeriod;
    QSpinBox* m_pispCyclePeriod;
    QLabel* m_plabDurationTime;
    QSpinBox* m_pispDurationTime;
    QLabel* m_plabSteppingTime;
    QSpinBox* m_pispSteppingTime;
    QLabel* m_plabAllowTimes;
    QSpinBox* m_pispAllowTimes;
    QLabel* m_plabLimitCycle;
    QSpinBox* m_pispLimitCycle;
    QLabel* m_plabSaturatedCurrent;
    QDoubleSpinBox* m_pdspSaturatedCurrent;

    QCheckBox* m_pchkAutoseqGating;

    QLabel* m_plabDurationThres;
    QDoubleSpinBox* m_pdspDurationThres;
    QLabel* m_plabGatingSTD;
    QSpinBox* m_pispGatingSTD;
    QLabel* m_plabGatingSTDMax;
    QSpinBox* m_pispGatingSTDMax;
    QLabel* m_plabSignalMin;
    QSpinBox* m_pispSignalMin;
    QLabel* m_plabSignalMax;
    QSpinBox* m_pispSignalMax;
    QLabel* m_plabGatingMin;
    QSpinBox* m_pispGatingMin;
    QLabel* m_plabGatingMax;
    QSpinBox* m_pispGatingMax;

    //QPushButton* m_pbtnStartDegating;
    QDialogButtonBox* m_pbtnbox;

private:
    bool InitCtrl(void);

public:
    //double GetDegatingVolt(void) { return m_pdspDegatingVolt->value(); }
    //void SetDegatingVolt(double val) { m_pdspDegatingVolt->setValue(val); }
    //int GetCyclePeriod(void) { return m_pispCyclePeriod->value(); }
    //void SetCyclePeriod(int val) { m_pispCyclePeriod->setValue(val); }
    //int GetDurationTime(void) { return m_pispDurationTime->value(); }
    //void SetDurationTime(int val) { m_pispDurationTime->setValue(val); }
    //int GetSteppingTime(void) { return m_pispSteppingTime->value(); }
    //void SetSteppingTime(int val) { m_pispSteppingTime->setValue(val); }
    //int GetAllowTimes(void) { return m_pispAllowTimes->value(); }
    //void SetAllowTimes(int val) { m_pispAllowTimes->setValue(val); }
    //int GetLimitCycle(void) { return m_pispLimitCycle->value(); }
    //void SetLimitCycle(int val) { m_pispLimitCycle->setValue(val); }

    bool GetAutoSeqGating(void) const { return m_pchkAutoseqGating->isChecked(); }
    void SetAutoSeqGating(bool auoseq) { m_pchkAutoseqGating->setChecked(auoseq); }

    bool GetDegatingBaseParam(double& degatingVolt, int& cyclePeriod, int& durTime, int& stepTime, int& allowTimes, int& limitCycle, double& satcur) const;
    bool GetDegatingAdvanceParam(double& durThres, int& gatingstd, int& gatingstdmax, int& signalmin, int& signalmax, int& gatingmin, int& gatingmax) const;

    void SetDegatingBaseParam(double degatingVolt, int cyclePeriod, int durTime, int stepTime, int allowTimes, int limitCycle, double satcur);
    void SetDegatingAdvanceParam(double durThres, int gatingstd, int gatingstdmax, int signalmin, int signalmax, int gatingmin, int gatingmax);

signals:

};

