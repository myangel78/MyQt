#ifndef DATAOUTPUTFORMAT_H
#define DATAOUTPUTFORMAT_H
#include <QThread>



class DataOutputFormat: public QThread
{
    Q_OBJECT
public:
    DataOutputFormat();
    ~DataOutputFormat();

    void startThread();
    void stopThread();

signals:
    void sendSaveDataBufSig(QVector<QVector<float>> vctDataBuf);

protected:
     void run();
private:
     std::atomic<bool> m_bRunThread = false;

};

#endif // TEMPERATURERECORD_H
