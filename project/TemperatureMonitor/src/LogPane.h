#pragma once

#include <QWidget>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <thread>


class LogPane : public QWidget
{
    Q_OBJECT
public:
    explicit LogPane(QWidget *parent = nullptr);

private:
    QTextEdit* m_ptxtEdit;
    QPushButton* m_pbtnClear;

private:
    bool m_bRunning = false;
    std::shared_ptr<std::thread> m_ThreadPtr = nullptr;
    int m_logCnt;

public:
    bool StartThread(void);
    void ThreadFunc(void);

    bool EndThread(void);
    bool StopThread(void);

signals:
    void UpdateLogSignal(QString msg);

private slots:
    void UpdateLogSlot(QString msg);
    void ClearLogSlot(void);

};

