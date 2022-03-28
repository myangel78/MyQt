#ifndef INPUTMESSAGEDIALOG_H
#define INPUTMESSAGEDIALOG_H

#include <QDialog>

namespace Ui {
class InputMessageDialog;
}

class InputMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputMessageDialog(QWidget *parent = 0);
    ~InputMessageDialog();


    void setMessage(const QString& title);
    void setPasswordMessage(const QString& content);

    QString getValue() const { return m_strValue; }

private slots:
    void on_btnOk_clicked();

private:
    Ui::InputMessageDialog *ui;
    void initStyle();

    QString m_strValue;
};

#endif // INPUTMESSAGEDIALOG_H
