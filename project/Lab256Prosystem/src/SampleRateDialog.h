#pragma once

#include <QDialog>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>

#include "ConfigServer.h"


class MainWindow;


class SampleRateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SampleRateDialog(QWidget *parent = nullptr);
    //virtual ~SampleRateDialog();

private:
    QGroupBox* m_pgrpUpdateSample = nullptr;
    QLabel* m_plabUpdateSample = nullptr;
    QPushButton* m_pbtnUpdateSample = nullptr;

    QGroupBox* m_pgrpSetSample = nullptr;
    QLabel* m_plabSampleRate = nullptr;
    QComboBox* m_pcmbSampleRate = nullptr;
    QLabel* m_plabOverload = nullptr;
    QComboBox* m_pcmbOverload = nullptr;

    MainWindow* m_pMainWindow = nullptr;

public:
    enum ESampleRate
    {
        ESR_5K = SAMPLE_RATE_5K,
        ESR_20K = SAMPLE_RATE_20K
    };

    enum EOverMultiple
    {
        EOM_1X = 0,
        EOM_2X = 2,
        EOM_4X = 4,
        EOM_8X = 8,

        EOM_MAX
    };

private:
    bool InitCtrl(void);

public:


public slots:
    void OnClickUpdateSample(void);
    void OnCheckSampleRate(bool chk);
    void OnChangeSampleFrequency(int val);
    void OnChangeOverMultiple(int val);

signals:

};

