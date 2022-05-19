#pragma once

#include <QObject>
class QTimer;

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

public slots:
    void WorkerTaskStartSlot(void);
    void TaskFinished(void);
private slots:
    void TimerOutToDoSomethingSlot(void);
signals:
    void WorkertResultSig(void);

private:
    QTimer *m_pTimer = nullptr;
};

