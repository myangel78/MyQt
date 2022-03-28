#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QProgressDialog>

namespace Ui {
class ControlPanel;
}

enum TIMERUNIT
{
    SECOND = 0,
    MINUTE = 1,
    HOUR = 2,
};

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel();

signals:
    void transmitData(const QByteArray &data);
    void transmitAIData(const QByteArray &data);
    void setZeroAdjustSig(int mode,const double backVolt = -0.18);
    void SendSampRateSig(const int & sampRate);
    void StopRunningAndSaveSig(void);

private slots:
    void commFreqModeChanged(int index);
    void on_sampApplyBtn_clicked();
    void on_zeroOffsetBtn_clicked();
    void CancelZeroAdjust(void);

    void SecTimerUpdataTimeSlot(void);
    void SecTimerUnitChangeSlot(int index);
    void on_ZeroStartStopBtn_toggled(bool checked);
    void on_ZeroRestartBtn_clicked();

    void StopSaveSecTimerUpdataTimeSlot(void);
    void StopSaveSecTimerUnitChangeSlot(int index);
    void on_StopTimeStartStopBtn_toggled(bool checked);
    void on_StopTimeRestartBtn_clicked();

    void progressBarSet(const int &value);

private:
    void initView(void);
    void caliSleep(int msec);
    void initTimerView(void);
    void initStopSaveTimerView(void);
    Ui::ControlPanel *ui;
    QProgressDialog *m_pQProgressDialog;

    QTimer * SecTimer;   //定时器 每秒更新时间
    QTime * TimeRecord;  //记录时间

    QTimer* StopSaveSecTimer;   //定时器 每秒更新时间
    QTime* StopSaveTimeRecord;  //记录时间
};

#endif // CONTROLPANEL_H
