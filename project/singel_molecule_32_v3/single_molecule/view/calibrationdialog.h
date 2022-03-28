#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H
#include "models/DataDefine.h"

#include <QDialog>
class QSettings;
class LeastSquare;
class QMessageBox;
class Point;

namespace Ui {
class Calibrationdialog;
}


class Calibrationdialog : public QDialog
{
    Q_OBJECT

public:
    explicit Calibrationdialog(QAction *actionStart,QAction *actionStop,QAction *actionClear, QWidget *parent = nullptr);
    ~Calibrationdialog();

    void loadConfig(QSettings *config);
    void saveConfig(QSettings *config);
    void SaveCaliToConfig(QSettings * nConfig);

signals:
    void caliDirectVoltSetSig(float &volt);
    void CalibrationFinished(bool nstate);

public slots:
    void meansResultSlot(QVector<float> result);


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void somethingChanged();
    void onSaveChangeBtnClicked();
    void onCancelBtnClicked();
    void onClearAllDataBtnClicked();
    void autoCaliburationBtnClicked();

    void on_loadConfigBtn_clicked();

    void on_resistAllSetBtn_clicked();

    void on_slopeOffsetResetBtn_clicked();

private:
    void initViewer(void);
    void initTable(void);
    void initConnect(void);
    bool okToContinue();
    void clearAllValue(void);
    inline void clearSomeChangeState(void){ m_isItemStateChange = false;}
    void caliSleep(int msec);
    void meansResultBufClear(void);
    void GetAllLineLeastSquare(void);


private:
    Ui::Calibrationdialog *ui;
    QSettings *m_config;
    bool m_isItemStateChange;
    QAction *m_StartActon;
    QAction *m_StopActon;
    QAction *m_ClearAction;
    float m_currentVolt;
    struct CalibrationStr m_nCalibraionStrArrayp[CHANNEL_NUM];
    QVector<QVector<Point>> m_nVctPoints;

    QMessageBox *m_messageBox;
};

#endif // CALIBRATIONDIALOG_H
