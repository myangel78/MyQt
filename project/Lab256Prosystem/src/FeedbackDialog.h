#pragma once

#include <QDialog>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qpushbutton.h>


class FeedbackDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FeedbackDialog(QWidget *parent = nullptr);
    //virtual ~FeedbackDialog();

private:
    QLabel* m_plabType;
    QComboBox* m_pcmbType;
    QLabel* m_plabTitle;
    QLineEdit* m_peditTitle;
    QLabel* m_plabReceiver;
    QLineEdit* m_peditReceiver;
    QLabel* m_plabCCer;
    QLineEdit* m_peditCCer;
    QLabel* m_plabAttach;
    QLineEdit* m_peditAttach;
    QPushButton* m_pbtnAttach;
    QLabel* m_plabDetail;
    QTextEdit* m_ptxtDetail;
    QPushButton* m_pbtnSendReport;


    std::vector<std::string> m_vetFeedbackType;

private:
    bool InitCtrl(void);

public:
    void OnClickInsertAttach(void);
    void OnClickSendReport(void);

    void ThreadSendReport(const std::string& strb, const std::string& strf);


private slots:
    void sendReportResultSlot(void);

signals:
    void sendReportResultSignal(void);
};

