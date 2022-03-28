#pragma once

//#include <map>
#include <tuple>
#include <vector>
#include <QDialog>
#include <qaction.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qsettings.h>

//#include <Point.h>
#include "ConfigServer.h"



class CalibrationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CalibrationDialog(QAction* pactStart, QWidget *parent = nullptr);
    virtual ~CalibrationDialog();

private:
    QAction* m_pactStart;
    QLabel* m_plabMaxVolt;
    QLabel* m_plabStepVolt;
    QLabel* m_plabCurVolt;
    QDoubleSpinBox* m_pdspMaxVolt;
    QDoubleSpinBox* m_pdspStepVolt;
    QDoubleSpinBox* m_pdspCurVolt;
    QPushButton* m_pbtnAutoCalib;

    QTableWidget* m_ptawCalib;

    QPushButton* m_pbtnLoad;

    QLabel* m_plabAllResistance;
    QDoubleSpinBox* m_pdspAllResistance;
    QPushButton* m_pbtnAllResistance;

    QLabel* m_plabAllSlope;
    QDoubleSpinBox* m_pdspAllSlope;
    QPushButton* m_pbtnAllSlope;

    QLabel* m_plabAllOffset;
    QDoubleSpinBox* m_pdspAllOffset;
    QPushButton* m_pbtnAllOffset;

    QPushButton* m_pbtnSave;
    QPushButton* m_pbtnClear;

    QSettings* m_pSettingConfig = nullptr;

    bool m_bChanged = false;
    double m_dMaxVolt = 0.0;
    double m_dStepVolt = 0.0;
    double m_dCurVolt = 0.0;
    //std::map<int, std::tuple<double, double, double>> m_mapCalib;
    std::vector<std::tuple<double, double, double>> m_vetCalib;

    std::vector<std::vector<std::pair<double, double>>> m_vetPoints;

private:
    bool InitCtrl(void);
    bool InitTable(void);
    bool LoadConfig(const char* pconfig, QSettings** psets);
    bool SaveConfig(QSettings* pset);
    bool SetConfig2UI(void);
    bool SetUI2Config(void);

    void ClearPointBuf(void);
    void GetAllLineLeastSquare(void);

protected:
    void closeEvent(QCloseEvent* evn);

public:
    void OnClickAutoCalibration(void);
    void OnClickLoadConfig(void);
    void OnClickAllResistance(void);
    void OnClickAllSlope(void);
    void OnClickAllOffset(void);
    void OnClickSaveChange(void);
    void OnClickClearChange(void);

    double GetMaxVolt(void) const { return m_dMaxVolt; }
    double GetStepVolt(void) const { return m_dStepVolt; }
    double GetCurVolt(void) const { return m_dCurVolt; }
    //const std::map<int, std::tuple<double, double, double>>& GetTableData(void) const { return m_mapCalib; }
    const std::vector<std::tuple<double, double, double>>& GetTableData(void) const { return m_vetCalib; }

signals:
    void SetAllChannelVoltSignal(double ampl);

private slots:
    void resistenceValueChangedSlot(QString str);
    void slopeValueChangedSlot(QString str);
    void offsetValueChangedSlot(QString str);
    void calibValueChangedSlot(void);

public slots:
    void setCalibrationSlot(const float* pavg);

};

