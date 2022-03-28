#pragma once

#include <QWidget>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtimer.h>


class MyQMessageBox : public QWidget
{
    Q_OBJECT
public:
    explicit MyQMessageBox(QWidget *parent = nullptr);

private:
    QMessageBox* m_pmsgboxMy;
    QPushButton* m_pbtnYes;
    QPushButton* m_pbtnNo;
    QTimer* m_ptmrCountdown;

    static const int m_ciCountdown = 10;
    int m_iCountdown = m_ciCountdown;

private:
    bool InitCtrl(void);

public:
    int exec(void);

public slots:
    void OnClickYes(void);
    void OnClickNo(void);
    void TimerCountdownSlot(void);

signals:

};

