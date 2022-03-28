#include "LogPane.h"

#include <qboxlayout.h>
#include <Log.h>


LogPane::LogPane(QWidget *parent) : QDialog(parent)
{
    m_logCnt = 0;
    m_ptxtEdit = new QTextEdit(this);
    m_ptxtEdit->setReadOnly(true);

    m_pbtnClear = new QPushButton("Clear", this);

    QVBoxLayout* verlayout1 = new QVBoxLayout();
    verlayout1->addStretch(5);
    verlayout1->addWidget(m_pbtnClear);

    QHBoxLayout* horlayout1 = new QHBoxLayout();
    horlayout1->addWidget(m_ptxtEdit);
    horlayout1->addLayout(verlayout1);
    setLayout(horlayout1);

    m_ptxtEdit->document()->setMaximumBlockCount(200);

    //m_timerLog = new QTimer(this);

    connect(this, &LogPane::UpdateLogSignal, this, &LogPane::UpdateLogSlot);
    connect(m_pbtnClear, &QPushButton::clicked, this, &LogPane::ClearLogSlot);

}

bool LogPane::StartThread(void)
{
    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&LogPane::ThreadFunc, this);
    }

    return false;
}

void LogPane::ThreadFunc(void)
{
    //LOGI("ThreadStart = {}!!!id = {}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    while (m_bRunning)
    {
        if (ecoli::CEcoliLog::GetInstance()->GetLogLength() > 0)
        {
            std::tuple<int, std::string> tplog;
            bool ret = ecoli::CEcoliLog::GetInstance()->GetLogMsg(tplog);
            if (ret)
            {
                emit UpdateLogSignal(std::get<1>(tplog).c_str());
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    //LOGI("ThreadExit = {}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());
}


bool LogPane::EndThread(void)
{
    m_bRunning = false;
    return false;
}

bool LogPane::StopThread(void)
{
    m_bRunning = false;

    if (m_ThreadPtr.get() != nullptr)
    {
        if (m_ThreadPtr->joinable())
        {
            m_ThreadPtr->join();
        }
        m_ThreadPtr.reset();
    }
    return false;
}


void LogPane::UpdateLogSlot(QString msg)
{
    m_ptxtEdit->moveCursor(QTextCursor::End);
    m_ptxtEdit->insertPlainText(msg);
    if(m_logCnt++ > 2000)
    {
        m_ptxtEdit->moveCursor(QTextCursor::End);
        m_ptxtEdit->insertPlainText(QString("The number of logs is greater than %1,now clear.....").arg(2000));
        ClearLogSlot();
    }
}

void LogPane::ClearLogSlot(void)
{
    m_logCnt = 0;
    m_ptxtEdit->clear();
}

