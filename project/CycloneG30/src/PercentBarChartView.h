#pragma once

#include <QWidget>
#include <QtCharts>

class PercentBarChartView :public QWidget
{
    Q_OBJECT
public:
    PercentBarChartView(const QString &title,const QStringList &strBarSetList,QWidget *parent =nullptr);
    ~PercentBarChartView();

    bool AppendData(const QString &strAxisx,const std::vector<int> &vctBarSetValue);
    bool ReplaceBarsetVue(const int &axisxIndex,const int &barsetIndex,const int &value);
    bool ReplaceBarsetAllVue(const int &barsetIndex,const QList<qreal> &values);
    bool ReplaceAllBarsetData(const QStringList &axisxCategories, const QVector<QList<int>> &vctBarsetData);

    QChartView *GetChartView(void){return m_ChartView;}

private:
    bool InitChart(const QString &title,const QStringList &strBarSetList);

private:
    QChart *m_pChart = nullptr;
    QChartView *m_ChartView = nullptr;
    QValueAxis *m_pAxisY = nullptr;
    QBarCategoryAxis *m_pAxisx = nullptr;
    std::vector<std::shared_ptr<QBarSet>> m_vctChartBarset;
    QPercentBarSeries *m_pPercentBarSeries = nullptr;
    int m_barsetNum;
};


