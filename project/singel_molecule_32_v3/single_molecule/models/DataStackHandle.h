#ifndef DATASTACKHANDLE_H
#define DATASTACKHANDLE_H

#include <QThread>
#include "models/DataDefine.h"
#include <windows.h>


class DataStackHandle : public QThread
{
    Q_OBJECT
public:
    explicit DataStackHandle(QObject *parent = 0);
    ~DataStackHandle();
    void startThread();
    void stopThread();

public:
    void LoadConfig(QSettings *nConfig);
signals:
    void WriteCmdSig(int cmd);

public slots:
    void updateFormula(bool bState);
    void zeroOffsetAdjustSlot(QVector<float> vctZeroOffset);

protected:
    void run();

    std::atomic<bool> m_bRunThread = false;

private:
    const float & readZeroOffset(uint16_t &channel);

    float m_nSlopeArray[CHANNEL_NUM];
    float m_nOffsetArray[CHANNEL_NUM] = {0};

    float m_nOffsetZeroArray[CHANNEL_NUM] = {0};
    float m_nTempZeroArray[CHANNEL_NUM] = {0};
    QSettings *m_nConfig;
    std::mutex m_zeroMutex;

    LARGE_INTEGER frequency, startCount, stopCount;
    LARGE_INTEGER startCount1,startCount2,startCount3;
    LARGE_INTEGER stopCount1,stopCount2,stopCount3;
    LONGLONG elapsed,elapsed1,elapsed2,elapsed3;
};


#endif // DATASTACKHANDLE_H
