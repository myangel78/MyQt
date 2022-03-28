#include "confirmmessagedialog.h"
#include "ui_confirmmessagedialog.h"
#include "../helper/commonhelper.h"

ConfirmMessageDialog::ConfirmMessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmMessageDialog)
{
    ui->setupUi(this);
    this->initStyle();
    CommonHelper::moveFormToCenter(this);
}

ConfirmMessageDialog::~ConfirmMessageDialog()
{
    delete ui;
}

void ConfirmMessageDialog::initStyle()
{
    this->setProperty("Form", true);
    this->setProperty("CanMove", true);
    this->setWindowTitle(ui->lab_Title->text());

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);

    ui->labInfo->setText("");
    ui->btnMenu_Close->setText("X");
    ui->lab_Ico->setStyleSheet("border-image: url(:/images/libpreping.png);");
 //   ui->widget_main->setStyleSheet("border:0px solid #1a1a1a;border-radius:1px;background:#f1efd5;");
    //ui->btnCancel->setStyleSheet("color:#1a1a1a;");
  //  ui->btnCancel->setStyleSheet("border:1px solid #1a1a1a;border-radius:5px;color:#ffffff;background:#5e6b7a;");
 //   ui->btnOk->setStyleSheet("border:1px solid #1a1a1a;border-radius:5px;color:#ffffff;background:#5e6b7a;");

    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void ConfirmMessageDialog::setMessage(QString msg, MSG_DLG eType)
{
    switch(eType)
    {
    case MSG_DLG_INFO:
        ui->labIcoMain->setStyleSheet("border-image: url(:/images/msg_info.png);");
        ui->btnCancel->setVisible(false);
        ui->lab_Title->setText("Info");
        ui->labInfo->setText(msg);
        break;

    case MSG_DLG_WARNING:
        ui->labIcoMain->setStyleSheet("border-image: url(:/images/msg_warn.png);");
        ui->btnCancel->setVisible(false);
        ui->lab_Title->setText("Warn");
        ui->labInfo->setText(msg);
        break;

    case MSG_DLG_ERROR:
        if( m_errMsgs.size() == 0 )
        {
            ui->labIcoMain->setStyleSheet("border-image: url(:/images/msg_error.png);");
            ui->btnCancel->setVisible(false);
            ui->lab_Title->setText("Error");
        }
        updateErrorMsg(msg);
        break;

    case MSG_DLG_QUESTION:
        ui->labIcoMain->setStyleSheet("border-image: url(:/images/msg_question.png);");
        ui->lab_Title->setText("Question");
        ui->labInfo->setText(msg);
        break;
    default:
        break;
    }

    this->setWindowTitle(ui->lab_Title->text());
}

void ConfirmMessageDialog::updateErrorMsg(const QString& sMsg)
{
    if(m_errMsgs.count() > 5 ) m_errMsgs.removeLast();

    m_errMsgs.push_front(sMsg);
    ui->labInfo->setText(m_errMsgs.join("\n"));
}

void ConfirmMessageDialog::on_btnOk_clicked()
{
    m_errMsgs.clear();
    done(1);
}
