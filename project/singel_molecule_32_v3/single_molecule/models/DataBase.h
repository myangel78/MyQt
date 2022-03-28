#ifndef DATABASE_H
#define DATABASE_H

#include<atomic>
#include <windows.h>

class DataBase
{
public:
    DataBase();
    virtual ~DataBase();

    void startThread();
    void stopThread();

protected:
    virtual void threadFunc();
    std::atomic<bool> m_bRunThread = false;
    LARGE_INTEGER m_frequency, m_SemTimeStart, m_SemTimeStop;
    LARGE_INTEGER m_TimeContentStart,m_TimeContentStop;
    LONGLONG m_SemTimeElapsed,m_ContentTimeElapsed;
};

#endif // DATABASE_H
