#ifndef CURRENTSINGELSETTINGPAGE_H
#define CURRENTSINGELSETTINGPAGE_H

#include <QWidget>
#include <QVariant>
#include <QTimer>

namespace Ui {
class CurrentSingelSettingPage;
}

class SingleChComb;
class DegatParamsDialog;
class QSettings;

typedef struct PolymerParam
{
    float nPoreCurrent;
    float nPoreVolt;
    float nPoreVoltStart;
    float nPoreVoltEnd;
    float nPoreVoltStep;
    float nPoreVoltCur;
    float nPoreTimeStep;
}PolymerParam;

enum TRIANGULAR_DIRECTION
{
      TRIANGULAR_RISE = true,
      TRIANGULAR_FALLING = false,
};

typedef struct TriangularPulseSimulate
{
    float nVoltStart;
    float nVoltEnd;
    float nVoltStep;
    float nVoltCur;
    float nVoltTimeStep;
}TriangularPulseSimulateParam;


class CurrentSingelSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentSingelSettingPage(QWidget *parent = nullptr);
    ~CurrentSingelSettingPage();

    void onTriangularTestStop(void);
    void LoadConfig(QSettings *config);
    void SaveConfig(QSettings *config);

signals:
    void transmitData(const QByteArray &);
    void setDegatingParameter(double duration_thres, int gating_std, int signal_min, int signal_max, int gating_max, int gating_min);
    void setPoreInsertionWorkSig(bool bIsOn,const float &singleLimt,const float &singleVolt);
    void getPoreStateBackSig(const int &channel,bool &bIsSet);

public slots:
    void SetZeroAdjustSlot(int mode,const double backVolt = -0.18);
    void setSpecChanelDirect(int &channel,const float &volt);
    void setAllChannelDirect(float &volt);
    void setDegatCurrentDirectVolt(int &channel);
    void setListItemVisiableSlot(QVector<bool> listVisiable);
    void recDegatResultSlot(QVariant rawInfo);
    void setCapTrangularWaveSlot(const bool &bIsSet);

private slots:
    void SingelItemSlot(SingleChComb *item);
    void onPosOrNegBtnClicked(void);
    void commVoltTypeChanged(int index);
    void onVoltAPpplyBtnClicked();

    void SingelDegatItemSlot(int channel,int mode, double Volt);
    void on_degatSetApplyBtn_clicked();
    void on_degatAllChk_toggled(bool checked);
    void on_degatEnableBtn_clicked(bool checked);
    void on_degatAdvancedBtn_clicked();

    void on_nSingleVoltAPpplyBtn_clicked();
    void on_nSinglePosOrNegBtn_clicked();
    void on_nSingleAllChk_toggled(bool checked);
    void setAllChannelOriVoltBySingle(void);


    void on_polyTestStartBtn_toggled(bool checked);
    void PolymerVoltCtrolTimeoutSlot(void);

    void TrianPulseVoltCtrolTimeoutSlot(void);
    void on_TrianPuseStartBtn_toggled(bool checked);

private:
    void initViewer(void);
    void initConnect(void);
    QByteArray EnCode(int channelMode,ushort channel,ushort amplitude,\
                                            int voltageMode,int voltfreq);
    void caliSleep(int msec);

    void UpdatePolymerParam(void);
    void PolymerAllChVoltSet(const float &volt);

    void UpdateTrianPulseParam(void);

    Ui::CurrentSingelSettingPage *ui;
    std::vector<std::shared_ptr<SingleChComb>> m_vctSingleChComb;
    DegatParamsDialog *m_pDegatParamsDialog;

    PolymerParam m_polymerVoltParam;
    QTimer m_polyVoltCtrTimer;

    TriangularPulseSimulateParam m_triangularPulseSimulateParam;
    QTimer m_triangularPulseSimulateTimer;
    bool m_bTriangularDirection;
};

#endif // CURRENTSINGELSETTINGPAGE_H
