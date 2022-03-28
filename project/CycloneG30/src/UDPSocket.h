#pragma once

#include <QObject>
#include <mutex>
#include <tuple>
#include <condition_variable>
#include <concurrentqueue.h>
#include "qtimer.h"
#include "ConfigServer.h"

class DataHandle;

class UDPSocket:public QObject
{
    Q_OBJECT
public:
    explicit UDPSocket(QObject *parent = nullptr);
    ~UDPSocket(void);

signals:
    void SpeedDispSig(const float &speed);

private:
     QString m_localIp;
     int m_localPort;

     bool m_bwatting = false;
     bool m_bRunning = false;
     bool m_bSuspend = false;
     std::shared_ptr<std::thread> m_ThreadRecvPtr = nullptr;
     DataHandle *m_pDataHandle = nullptr;

     qulonglong m_rcvBytesCnt;
     QTimer m_rcvBytesTimer;

     uint32_t m_saveLen;

protected:
     void run(void);

public:
    std::condition_variable m_cvRecv;
    std::mutex m_mutexRecv;


public:
     void SetThreadSuspend(bool bsuspend) { m_bSuspend = bsuspend; }
     void SetDataHandlePtr(DataHandle* pDataHandle) { m_pDataHandle = pDataHandle; }
     bool OnSetUdpSocket(const QString localIp,const int localPort);
     QStringList OnSearchLocalIp(void);

    bool StartThread(void);
    void ThreadRecvFunc(void);
    void ThreadHandleFunc(void);
    bool EndThread(void);
    bool StopThread(void);
};

