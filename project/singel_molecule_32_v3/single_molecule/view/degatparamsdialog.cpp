#include "degatparamsdialog.h"
#include "ui_degatparamsdialog.h"

DegatParamsDialog::DegatParamsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DegatParamsDialog)
{
    // 不显示问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    flags &=~Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);

    ui->setupUi(this);
    this->setWindowTitle("De-gating Parameter Adjustment");
}

DegatParamsDialog::~DegatParamsDialog()
{
    delete ui;
}

void DegatParamsDialog::on_degatApplyBtn_clicked()
{
    emit setDegatingParameter(ui->duration_thres_dpbox->value(),ui->gating_std_spbox->value(),ui->gating_signal_min_spbox->value(),ui->gating_signal_max_spbox->value(),ui->gating_max_spbox->value(),ui->gating_min_spbox->value());
}
