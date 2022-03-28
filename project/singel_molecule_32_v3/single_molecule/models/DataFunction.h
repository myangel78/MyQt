#ifndef DATAFUNCTION_H
#define DATAFUNCTION_H
#include <QObject>
using std::vector;
#include <windows.h>
#include <QThread>
#include <QVariant>

Q_DECLARE_METATYPE(QVector<bool>)

class DataFunction :public QObject
{
    Q_OBJECT
public:
    DataFunction();
    ~DataFunction();
    void startThread();
    void stopThread();
public slots:
    void AnalyDegatingChannel(QVector<QVector<float>> &vctData);
    void setDegatingParameterSlot(double duration_thres, int gating_std, int signal_min, int signal_max, int gating_max, int gating_min);

signals:
    void DegateResultSig(QVariant);

private:   
    bool isGating(vector<int> &label, vector<vector<double>> &cluster_stats,
                  double sampling_rate=20000, double duration_thres=0.5,
                  int signal_min=20, int signal_max=150,
                  int gating_min=40, int gating_max=130, int hard_min=10, int gating_std=6,
                  int openpore_thres=170);

    std::atomic<bool> m_bRunThread = false;

    LARGE_INTEGER m_frequency, m_SemTimeStart, m_SemTimeStop;
    LARGE_INTEGER m_TimeContentStart,m_TimeContentStop;
    LONGLONG m_SemTimeElapsed,m_ContentTimeElapsed;

    double m_duration_thres;
    int m_gating_std;
    int m_gating_min;
    int m_gating_max;
    int m_gating_sigMin;
    int m_gating_sigMax;
};
#endif // DATAFUNCTION_H
