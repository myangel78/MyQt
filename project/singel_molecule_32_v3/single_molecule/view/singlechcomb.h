#ifndef SINGLECHCOMB_H
#define SINGLECHCOMB_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class SingleChComb;
}

enum VOLTAGE_WAY
{
    DIRECT_VOLTAGE = 0,
    PULSE_VOLTAGE,
    FREQUENCE_SET,
};

enum DEGATE_STATE_COLOR
{
    NODEGATE_GREEN = false,
    DEGATE_RED = true
};

enum SINGLE_PORE_STATE
{
    NOT_SINGLE_PORE = false,
    SINGLE_PORE = true,
};


class SingleChComb : public QWidget
{
    Q_OBJECT

public:
    explicit SingleChComb(QWidget *parent = 0);
    ~SingleChComb();

public:
    void init(int &index);
    void initDegat(const double &degatVolt,const int &degatDurationTime, const int &degatCheckDutyTime);
    void setDegatParam(const double &degatVolt,const int &degatDurationTime, const int &degatCheckDutyTime);
    int channel() {
        return m_channel;
    }

    void setChannel(int &Channel) {
        m_channel = Channel;
    }

    float getVoltSet(void)const;
    void setVolt(double value);

    bool isAutoChecked(void)const;
    void setAutoCheck(const bool &check);
    bool isSingleVoltChecked(void)const;
    void setSingleVoltCheck(const bool &check);
    void setAutoCheckEnable(const bool &state)const;
    void setDegateColor(bool state);

    bool getPoreState(void)const{ return m_bPoreState;}

public slots:
    void applyBtnClick(void);

signals:
    void SingelChanged(SingleChComb *item);
    void SingelDegateItemChanged(int channel,int mode, double Volt);

private slots:
    void ampPlusBtnClick(void);
    void degatDurationTimeOut(void);
    void degatCheckDutyTimeOut(void);
    void on_degatColorBtn_clicked();
    void on_degatAutoChk_toggled(bool checked);

    void on_nPoreStateBtn_toggled(bool checked);


private:
    Ui::SingleChComb *ui;
    void initConnect(void);
    int m_channel;

    double m_degatVolt;
    int m_degatDurationTime;
    int m_degatCheckDutyTime;

    bool m_bOriColor;
    QTimer *m_degatDurationTimer = nullptr;
    QTimer *m_degatChkDutyTimer = nullptr;
    QString m_sheetStyle = "border:1px solid black;background:";

    bool m_bPoreState;
    const QString SinglePoreStyle = {"min-width: 11px; min-height: 11px;max-width:11px; max-height: 11px;border:1px solid green;background: rgb(0, 255, 255)"};
    const QString notSinglePoreStyle = {"min-width: 11px; min-height: 11px;max-width:11px; max-height: 11px;border:1px solid black;background: white;"};


};

#endif // SINGLECHCOMB_H
