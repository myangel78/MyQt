#include "Worker.h"
#include <QDebug>
#include <QThread>
#include <QTimer>

Worker::Worker(QObject *parent) : QObject(parent)
{
    qDebug()<<__FUNCTION__<<"threadid"<< QThread::currentThreadId();

}

Worker::~Worker()
{
    TaskFinished();
      qDebug()<<__FUNCTION__<<"threadid"<< QThread::currentThreadId();
}


void Worker::WorkerTaskStartSlot(void)
{
    qDebug()<<__FUNCTION__<<"threadid"<< QThread::currentThreadId();
    emit WorkertResultSig();

    m_pTimer = new QTimer(this);
    connect(m_pTimer,&QTimer::timeout,this,&Worker::TimerOutToDoSomethingSlot);
    m_pTimer->start(1000);
}


void Worker::TaskFinished(void)
{
    qDebug()<<__FUNCTION__<<"threadid"<< QThread::currentThreadId();

    if(m_pTimer)
    {
        if(m_pTimer->isActive())
        {
            m_pTimer->stop();
            qDebug()<<__FUNCTION__<<"stop timer"<< QThread::currentThreadId();
        }

        delete m_pTimer;
        m_pTimer = nullptr;
    }
}


void Worker::TimerOutToDoSomethingSlot(void)
{
    qDebug()<<__FUNCTION__<<"threadid"<< QThread::currentThreadId();
}

