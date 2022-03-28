#pragma once

#include <QWidget>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qtoolbutton.h>

#include "qcustomplot.h"
#include "ConfigServer.h"


class HistoryListItem : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryListItem(int chan, QWidget* parent = nullptr);

private:
    QCheckBox* m_pchkChannel = nullptr;
    QToolButton* m_pbtnChannel = nullptr;
    QLabel* m_plabCurrentValue = nullptr;

    int m_iChannel = 0;

private:
    bool InitCtrl(void);

public:
    bool GetChecked(void) const { return m_pchkChannel->isChecked(); }
    void SetChecked(bool chk) { m_pchkChannel->setChecked(chk); }

    QString GetValue(void) const { return m_plabCurrentValue->text(); }
    void SetValue(const QString& txt) { m_plabCurrentValue->setText(txt); }

public slots:
    void OnCheckShowChanged(bool chk);

signals:
    void checkShowChangedSignal(int chan, bool chk);

};


class HistoryDataView : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryDataView(QWidget *parent = nullptr);

private:
    QGroupBox* m_pgrpPlot = nullptr;
    QCustomPlot* m_pHistoryPlot = nullptr;
    QCPGraph* m_graph = nullptr;

    QLabel* m_plabYmax;
    QDoubleSpinBox* m_pdspYmax;
    QLabel* m_plabYmin;
    QDoubleSpinBox* m_pdspYmin;
    QLabel* m_plabPtnum;
    QDoubleSpinBox* m_pdspPtnum;
    QPushButton* m_pbtnPlotApply;

    QLineEdit* m_peditFileName = nullptr;
    QPushButton* m_pbtnLoadFile = nullptr;
    QPushButton* m_pbtnReloadFile = nullptr;
    QPushButton* m_pbtnResetData = nullptr;
    QGroupBox* m_pgrpCtrl = nullptr;
    QSpinBox* m_pispTimeNode = nullptr;
    QCheckBox* m_pchkAllCheck = nullptr;
    QCheckBox* m_pchkShowCheck = nullptr;
    QComboBox* m_pcmbSelChannel = nullptr;
    QDoubleSpinBox* m_pdspFilterMin = nullptr;
    QDoubleSpinBox* m_pdspFilterMax = nullptr;
    QPushButton* m_pbtnHistoryFilter = nullptr;
    QLabel* m_plabHistoryFilter = nullptr;
    QListWidget* m_plistHistoryShow = nullptr;


    int m_iStartIndex = 0;
    int m_iEndIndex = CHANNEL_NUM;

    //double m_dYmax = -999.99;
    //double m_dYmin = 999.99;
    //double m_dXnum = 0.0;
    double m_dFirstTimeval = 0.0;

    std::vector<std::shared_ptr<HistoryListItem>> m_vetHistoryListItem;
    std::vector<double> m_vetHistoryValue;

private:
    bool InitCtrl(void);

    void GetActiveChannelIndex(int curIndex);
    void DoLoadDataFile(const QString& fileName);

public slots:
    void OnCheckAllChecked(bool chk);
    void OnCheckShowChecked(bool chk);
    void OnPlotPageViewChange(int index);
    void OnClickFilterValue(void);
    void OnClickLoadFile(void);
    void OnClickReloadFile(void);
    void OnClickResetData(void);
    void OnPlotApply(void);

    void MyMouseMoveEvent(QMouseEvent* event);

    void checkShowChangedSlot(int chan, bool chk);

signals:

};

