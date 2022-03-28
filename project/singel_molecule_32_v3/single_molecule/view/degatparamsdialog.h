#ifndef DEGATPARAMSDIALOG_H
#define DEGATPARAMSDIALOG_H

#include <QDialog>

namespace Ui {
class DegatParamsDialog;
}

class DegatParamsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DegatParamsDialog(QWidget *parent = nullptr);
    ~DegatParamsDialog();
signals:
    void setDegatingParameter(double duration_thres,int gating_std, int signal_min, int signal_max,int gating_max,int gating_min);

private slots:
    void on_degatApplyBtn_clicked();

private:
    Ui::DegatParamsDialog *ui;
};

#endif // DEGATPARAMSDIALOG_H
