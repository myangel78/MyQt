#include "inputmessagedialog.h"
#include "ui_inputmessagedialog.h"
#include "helper/commonhelper.h"

InputMessageDialog::InputMessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputMessageDialog)
{
    ui->setupUi(this);
    this->initStyle();
    CommonHelper::moveFormToCenter(this);
}


InputMessageDialog::~InputMessageDialog()
{
    delete ui;
}

void InputMessageDialog::initStyle()
{
    this->setProperty("Form", true);
    this->setProperty("CanMove", true);
    this->setWindowTitle(ui->lab_Title->text());
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}


void InputMessageDialog::setPasswordMessage(const QString& content)
{
    ui->labInfo->setText(content);
    ui->txtValue->setPlaceholderText("Password");
    ui->txtValue->setEchoMode(QLineEdit::Password);
}


void InputMessageDialog::setMessage(const QString& title)
{
    ui->labInfo->setText(title);
}

void InputMessageDialog::on_btnOk_clicked()
{
    m_strValue = ui->txtValue->text().trimmed();
    done(1);
    this->close();
}
