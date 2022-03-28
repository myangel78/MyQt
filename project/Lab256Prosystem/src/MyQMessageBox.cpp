#include "MyQMessageBox.h"

MyQMessageBox::MyQMessageBox(QWidget *parent) : QWidget(parent)
{
    InitCtrl();
}

bool MyQMessageBox::InitCtrl()
{
    m_pmsgboxMy = new QMessageBox(this);
    m_pbtnYes = new QPushButton("Yes", this);
    m_pbtnNo = new QPushButton(QString("No (%1)").arg(m_ciCountdown), this);
    m_pmsgboxMy->addButton(m_pbtnYes, QMessageBox::AcceptRole);
    m_pmsgboxMy->addButton(m_pbtnNo, QMessageBox::RejectRole);
    m_pmsgboxMy->setDefaultButton(m_pbtnNo);
    m_pmsgboxMy->setText("Whether to manually add valid channels?");

    m_ptmrCountdown = new QTimer(this);

    connect(m_pbtnYes, &QPushButton::clicked, this, &MyQMessageBox::OnClickYes);
    connect(m_pbtnNo, &QPushButton::clicked, this, &MyQMessageBox::OnClickNo);
    connect(m_ptmrCountdown, &QTimer::timeout, this, &MyQMessageBox::TimerCountdownSlot);

    return true;
}

int MyQMessageBox::exec(void)
{
    m_iCountdown = m_ciCountdown;
    m_ptmrCountdown->start(1000);
    return m_pmsgboxMy->exec();
}

void MyQMessageBox::OnClickYes()
{
    if (m_ptmrCountdown->isActive())
    {
        m_ptmrCountdown->stop();
    }
}

void MyQMessageBox::OnClickNo()
{
    if (m_ptmrCountdown->isActive())
    {
        m_ptmrCountdown->stop();
    }
}

void MyQMessageBox::TimerCountdownSlot()
{
    --m_iCountdown;
    QString str = QString("No (%1)").arg(m_iCountdown);
    m_pbtnNo->setText(str);
    if (m_iCountdown <= 0)
    {
        m_pbtnNo->click();
    }
}
