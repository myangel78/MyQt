#include "PercentBarChartView.h"
#include "Global.h"


PercentBarChartView::PercentBarChartView(const QString &title,const QStringList &strBarSetList,QWidget *parent):QWidget(parent)
{
    m_barsetNum =0;
    InitChart(title,strBarSetList);

}


PercentBarChartView::~PercentBarChartView()
{
    m_vctChartBarset.erase (m_vctChartBarset.begin(),m_vctChartBarset.end());
}

bool PercentBarChartView::InitChart(const QString &title,const QStringList &strBarSetList)
{
    if(strBarSetList.size() == 0)
        return false;

    m_barsetNum = strBarSetList.size();

    m_pChart =new QChart();
    m_pChart->setTitle(title);
    m_pChart->setAnimationOptions(QChart::SeriesAnimations);

    m_pPercentBarSeries = new QPercentBarSeries(this);
    m_pChart->addSeries(m_pPercentBarSeries);

    m_vctChartBarset.resize(m_barsetNum);
    for(int i = 0; i < m_barsetNum; ++i)
    {
        m_vctChartBarset[i] = std::make_shared<QBarSet>(strBarSetList[i],m_pPercentBarSeries);
        m_pPercentBarSeries->append(m_vctChartBarset[i].get());
    }

    m_pAxisx = new QBarCategoryAxis(m_pChart);
    m_pChart->addAxis(m_pAxisx, Qt::AlignBottom);
    m_pPercentBarSeries->attachAxis(m_pAxisx);

    m_pAxisY = new QValueAxis(m_pChart);
    m_pAxisY->setLabelFormat("%.1f%  ");
    m_pChart->addAxis(m_pAxisY, Qt::AlignLeft);
    m_pPercentBarSeries->attachAxis(m_pAxisY);

    m_ChartView = new QChartView(m_pChart,this);
    m_ChartView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout *horilayout= new QHBoxLayout();
    horilayout->addWidget(m_ChartView);
    setLayout(horilayout);

    return true;
}


bool PercentBarChartView::AppendData(const QString &strAxisx,const std::vector<int> &vctBarSetValue)
{
    if(vctBarSetValue.size() != m_barsetNum)
        return false;

    for(int i = 0; i < m_barsetNum; ++i)
    {
         *(m_vctChartBarset[i].get())<<  vctBarSetValue[i];
    }

    m_pAxisx->append(strAxisx);
    return true;
}

/**
 * @brief 替换柱状图容器中的某个数值
 * @param axisxIndex 横坐标序号
 * @param barsetIndex 柱状序号
 * @param value 新数值
 * @return
 */
bool PercentBarChartView::ReplaceBarsetVue(const int &axisxIndex,const int &barsetIndex,const int &value)
{
    if(axisxIndex >= m_pAxisx->count())
        return false;

    QList<QBarSet *> listBarset = m_pPercentBarSeries->barSets();
    listBarset.at(barsetIndex)->replace(axisxIndex,value);
    return true;
}


/**
 * @brief 替换柱状图容器中的全部数值
 * @param barsetIndex 柱状序号
 * @param values 替换的数值内容
 * @return 是否成功
 */
bool PercentBarChartView::ReplaceBarsetAllVue(const int &barsetIndex,const QList<qreal> &values)
{
    if(barsetIndex >= m_barsetNum)
        return false;

    QList<QBarSet *> listBarset = m_pPercentBarSeries->barSets();

    auto &pbarset  =  listBarset.at(barsetIndex);
    pbarset->remove(0,pbarset->count());
    pbarset->append(values);

    return true;
}

/**
 * @brief 替代所有柱状数据
 * @param axisxCategories 横坐标字符传
 * @param vctBarsetData 所有柱状数据，二维数组[barset][data];
 * @return 是否成功
 */
bool PercentBarChartView::ReplaceAllBarsetData(const QStringList &axisxCategories, const QVector<QList<int>> &vctBarsetData)
{
    if(vctBarsetData.isEmpty() || vctBarsetData.size() != m_barsetNum )
        return false;

    if(axisxCategories.size() != vctBarsetData[0].size())
        return false;

    int size = vctBarsetData.size();
    for(int i= 0; i < size; ++i)
    {
        QList<qreal> values(vctBarsetData[i].begin(),vctBarsetData[i].end());
        ReplaceBarsetAllVue(i, values);
    }

    m_pAxisx->clear();
    m_pAxisx->append(axisxCategories);
    return true;
}
