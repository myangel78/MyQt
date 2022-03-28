#include "logpage.h"
#include "ui_logpage.h"
#include"logger/Log.h"

std::mutex LogPage::m_mutex;
std::queue<std::pair<int, std::string>> LogPage::m_queueLog;

LogPage::LogPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogPage)
{
    ui->setupUi(this);

    startLogShow(this);
    SETLOGCB(LogPage::loggerCallback);

    ui->logText->document()->setMaximumBlockCount(200);

    connect(this,SIGNAL(updateUisignal(QString)),this,SLOT(updateUiText(QString)));
    connect(ui->nClearBtn,SIGNAL(clicked(bool)),this,SLOT(ClearUiText()));
}

LogPage::~LogPage()
{
    m_bQuit = true;
    delete ui;
}

void LogPage::loggerCallback(int nType, const char* pMsg)
{
    m_mutex.lock();
    m_queueLog.push(std::make_pair(nType, pMsg));
    m_mutex.unlock();
}

void LogPage::startLogShow(QWidget *pThis)
{
    std::thread thrd([pThis]()
    {
        if(pThis != NULL)
        {
            LogPage* pLogPage = (LogPage*)pThis;
            pLogPage->logShowThread();
        }
    });
    thrd.detach();
}

void LogPage::logShowThread()
{
    m_bQuit = false;
    while(!m_bQuit)
    {
        m_mutex.lock();
        std::pair<int,std::string> logMsg;

        if(m_queueLog.size() > 0 )
        {
            logMsg = m_queueLog.front();
            m_queueLog.pop();
            m_mutex.unlock();
            emit updateUisignal(QString::fromStdString(logMsg.second));
        }
        else
        {
            m_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            continue;
        }
    }
}

void LogPage::updateUiText(QString msg)
{
    ui->logText->moveCursor(QTextCursor::End);
    ui->logText->insertPlainText(msg);
}

void LogPage::ClearUiText(void)
{
    ui->logText->clear();
}

