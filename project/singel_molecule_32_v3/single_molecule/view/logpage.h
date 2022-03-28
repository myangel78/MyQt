#ifndef LOGPAGE_H
#define LOGPAGE_H

#include <QWidget>
#include<mutex>
#include<queue>

namespace Ui {
class LogPage;
}

class LogPage : public QWidget
{
    Q_OBJECT

public:
    explicit LogPage(QWidget *parent = 0);
    ~LogPage();

signals:
    void updateUisignal(QString msg);

private slots:
    void updateUiText(QString msg);
    void ClearUiText(void);

public:
    static void loggerCallback(int nType, const char* pMsg);

private:
    void startLogShow(QWidget *pThis);
    void logShowThread();

private:
    static std::mutex m_mutex;
    static std::queue<std::pair<int, std::string>> m_queueLog;
    bool m_bQuit;

private:
    Ui::LogPage *ui;
};

#endif // LOGPAGE_H
