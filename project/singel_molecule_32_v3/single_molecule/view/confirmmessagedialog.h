#ifndef CONFIRMMESSAGEDIALOG_H
#define CONFIRMMESSAGEDIALOG_H

#include <QDialog>
#include "../helper/messagedialogdefine.h"

namespace Ui {
class ConfirmMessageDialog;
}

class ConfirmMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfirmMessageDialog(QWidget *parent = 0);
    ~ConfirmMessageDialog();

    void setMessage(const QString msg, MSG_DLG eType);

private:
    void initStyle();
    void updateErrorMsg(const QString& sMsg);

private slots:
    void on_btnOk_clicked();

private:
    Ui::ConfirmMessageDialog *ui;
    QStringList m_errMsgs;
};

#endif // CONFIRMMESSAGEDIALOG_H
