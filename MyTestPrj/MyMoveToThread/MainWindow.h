#pragma once

#include <QMainWindow>
class QPushButton;
class QWidget;
class Worker;
class QThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void InitCtrl(void);
private slots:
    void StartTaskBtnSlot(const bool &checked);
    void WorkertResultSlot(void);
private:
    QPushButton *m_pStartTaskBtn = nullptr;
    QWidget *m_pCenterWidget = nullptr;
    Worker *m_pWorker = nullptr;
    QThread *m_pWorkerThread = nullptr;
};
