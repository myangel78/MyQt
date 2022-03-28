#pragma once

#include <QDialog>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qdialogbuttonbox.h>


class ZeroAdjustDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ZeroAdjustDialog(QWidget *parent = nullptr);

private:
    QLabel* m_plabBackwardVoltage;
    QDoubleSpinBox* m_pdspBackwardVoltage;
    QLabel* m_plabBackwardDuration;
    QDoubleSpinBox* m_pdspBackwardDuration;
    QLabel* m_plabZeroVoltDuration;
    QDoubleSpinBox* m_pdspZeroVoltDuration;
    QLabel* m_plabZeroAdjustTime;
    QDoubleSpinBox* m_pdspZeroAdjustTime;

    QLabel* m_plabIntervalsTimer;
    QSpinBox* m_pispIntervalsTimer;

    QDialogButtonBox* m_pbtnbox;

private:
    bool InitCtrl(void);

public:
    double GetBackwardVolt(void) const { return m_pdspBackwardVoltage->value(); }
    double GetBackwardTime(void) const { return m_pdspBackwardDuration->value(); }
    double GetZeroTime(void) const { return m_pdspZeroVoltDuration->value(); }
    double GetAdjustTime(void) const { return m_pdspZeroAdjustTime->value(); }
    int GetIntervalsTimer(void) const { return m_pispIntervalsTimer->value(); }

    void SetBackwardVolt(double val) { m_pdspBackwardVoltage->setValue(val); }
    void SetBackwardTime(double val) { m_pdspBackwardDuration->setValue(val); }
    void SetZeroTime(double val) { m_pdspZeroVoltDuration->setValue(val); }
    void SetAdjustTime(double val) { m_pdspZeroAdjustTime->setValue(val); }
    void SetIntervalsTimer(int val) { m_pispIntervalsTimer->setValue(val); }

signals:

};

