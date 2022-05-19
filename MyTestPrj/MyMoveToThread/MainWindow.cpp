#include "MainWindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QDebug>
#include <QThread>
#include "Worker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    InitCtrl();
    qDebug()<<__FUNCTION__<<"mainwindow thread id"<< QThread::currentThreadId();
}

MainWindow::~MainWindow()
{
    if(m_pStartTaskBtn->isChecked())
    {
        m_pStartTaskBtn->click();
    }
}

void MainWindow::InitCtrl(void)
{
    m_pCenterWidget = new QWidget(this);
    m_pStartTaskBtn = new QPushButton(QStringLiteral("Start"),this);

    QVBoxLayout *pvertlayout = new QVBoxLayout();
    pvertlayout->addWidget(m_pStartTaskBtn);
    m_pCenterWidget->setLayout(pvertlayout);

    setCentralWidget(m_pCenterWidget);

    m_pStartTaskBtn->setCheckable(true);
    connect(m_pStartTaskBtn,&QPushButton::clicked,this,&MainWindow::StartTaskBtnSlot);

}

void MainWindow::StartTaskBtnSlot(const bool &checked)
{
    if(checked)
    {
        m_pStartTaskBtn->setText(QStringLiteral("Close"));

        m_pWorkerThread = new QThread();
        m_pWorker = new Worker();

        // move the task object to the thread BEFORE connecting any signal/slots
        m_pWorker->moveToThread(m_pWorkerThread);

        connect(m_pWorkerThread, SIGNAL(started()), m_pWorker, SLOT(WorkerTaskStartSlot()));
        connect(m_pWorker, SIGNAL(WorkertResultSig()), this, SLOT(WorkertResultSlot()));

        // automatically delete thread and task object when work is done:
        connect(m_pWorkerThread, SIGNAL(finished()), m_pWorker, SLOT(deleteLater()));
        connect(m_pWorkerThread, SIGNAL(finished()), m_pWorkerThread, SLOT(deleteLater()));


        m_pWorkerThread->start();
    }
    else
    {
        m_pStartTaskBtn->setText(QStringLiteral("Start"));
        m_pWorkerThread->quit();
        m_pWorkerThread->wait();
    }
}

void MainWindow::WorkertResultSlot(void)
{
    qDebug()<<__FUNCTION__<<"threadid"<<QThread::currentThreadId();

}


