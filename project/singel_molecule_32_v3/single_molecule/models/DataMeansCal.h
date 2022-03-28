#ifndef DATAMEANSCAL_H
#define DATAMEANSCAL_H
#include <QThread>
#include <windows.h>
#include <QWidget>

class DataMeansCal:public QThread
{
    Q_OBJECT
public:
    DataMeansCal(QWidget *parent = nullptr);
    ~ DataMeansCal();

    float GetMean(QVector<float> &in);
    void startThread();
    void stopThread();
signals:
    void MeansResultSig(QVector<float> meanResult);
    void CaliMeanstResSig(QVector<float> meanResult);
    void DegatingBufSig(QVector<QVector<float>> &vctData);
    void ZeroOffsetResSig(QVector<float> vctData);

protected:
    void run();

private:
    std::atomic<bool> m_bRunThread = false;

    LARGE_INTEGER frequency, startCount, stopCount;
    LONGLONG elapsed;
};


#endif // DATAMEANSCAL_H
