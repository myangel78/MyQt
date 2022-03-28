#include "InputNewRecordDlg.h"
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>

InputNewRecordDlg::InputNewRecordDlg(DLG_TYPE_ENUM type,QWidget *parent):QDialog(parent)
{
    InitCtr();
    if(type == INSERT_DLG)
    {
        this->setWindowTitle("Insert new record");
        m_pConfirmBtn->setText("Insert");
    }
    else if(type == MODIFY_DLG)
    {
         this->setWindowTitle("Modfiy record");
        m_pConfirmBtn->setText("Modify");
        m_pRegFielTabNameEdit->setEnabled(false);
    }

}

void InputNewRecordDlg::InitCtr(void)
{

    m_pRegIdLab = new QLabel(QStringLiteral("Id:"),this);
    m_pRegNameLab = new  QLabel(QStringLiteral("Reg_Name:"),this);
    m_pRegAddrLab= new QLabel(QStringLiteral("Address:"),this);
    m_pRegSizeLab= new QLabel(QStringLiteral("Size:"),this);
    m_pRegDefVueLab= new QLabel(QStringLiteral("DefaultVue:"),this);
    m_pRegVueLab= new QLabel(QStringLiteral("Value:"),this);
    m_pRegFielTabNameLab= new QLabel(QStringLiteral("Field_Table_Name:"),this);


    m_pRegIdSpbox = new QSpinBox(this);
    m_pRegIdSpbox->setMaximum(500);
    m_pRegNameEdit = new QLineEdit(this);
    m_pRegAddrEdit = new QLineEdit(this);
    m_pRegSizeComb = new QComboBox(this);
    m_pRegDefVueEdit = new QLineEdit(this);
    m_pRegVueEdit = new QLineEdit(this);
    m_pRegFielTabNameEdit = new QLineEdit(this);

    m_pConfirmBtn = new QPushButton(tr("Confirm"));
    m_pCancelBtn = new QPushButton(QStringLiteral("Cancel"));

    m_pRegSizeComb->addItem("32",32);

    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->addWidget(m_pRegIdLab,0,0,1,1);
    gridlayout->addWidget(m_pRegIdSpbox,0,1,1,1);

    gridlayout->addWidget(m_pRegNameLab,1,0,1,1);
    gridlayout->addWidget(m_pRegNameEdit,1,1,1,1);

    gridlayout->addWidget(m_pRegAddrLab,2,0,1,1);
    gridlayout->addWidget(m_pRegAddrEdit,2,1,1,1);

    gridlayout->addWidget(m_pRegSizeLab,3,0,1,1);
    gridlayout->addWidget(m_pRegSizeComb,3,1,1,1);

    gridlayout->addWidget(m_pRegDefVueLab,4,0,1,1);
    gridlayout->addWidget(m_pRegDefVueEdit,4,1,1,1);

    gridlayout->addWidget(m_pRegVueLab,5,0,1,1);
    gridlayout->addWidget(m_pRegVueEdit,5,1,1,1);

    gridlayout->addWidget(m_pRegFielTabNameLab,6,0,1,1);
    gridlayout->addWidget(m_pRegFielTabNameEdit,6,1,1,1);

    gridlayout->addWidget(m_pConfirmBtn,7,0,1,1);
    gridlayout->addWidget(m_pCancelBtn,7,1,1,1);


    setLayout(gridlayout);

    connect(m_pConfirmBtn,&QPushButton::clicked,this,&InputNewRecordDlg::ConfirmBtnSlot);
    connect(m_pCancelBtn,&QPushButton::clicked,[=](){this->reject();});

}

void InputNewRecordDlg::SetId(const int &id)
{
    m_pRegIdSpbox->setValue(id);
}

void InputNewRecordDlg::LoaddingData(const RegIdItemSimp &item)
{
    m_pRegIdSpbox->setValue(item.id);
    m_pRegNameEdit->setText(item.name);
    m_pRegAddrEdit->setText(item.addr);
    m_pRegDefVueEdit->setText(item.defalVue);
    m_pRegVueEdit->setText(item.value);
    m_pRegFielTabNameEdit->setText(item.fieldTabStr);
}

void InputNewRecordDlg::GetInRegIdItemStu(RegIdItemSimp &item)
{
    item.id = m_pRegIdSpbox->value();
    item.name= m_pRegNameEdit->text();
    item.addr = m_pRegAddrEdit->text();
    item.size = m_pRegSizeComb->currentData().toInt();
    item.defalVue = m_pRegDefVueEdit->text();
    item.value = m_pRegVueEdit->text();
    item.fieldTabStr = m_pRegFielTabNameEdit->text();
}

bool InputNewRecordDlg::CheckAllValid(void)
{
    return (!m_pRegNameEdit->text().isEmpty() && !m_pRegAddrEdit->text().isEmpty() && !m_pRegDefVueEdit->text().isEmpty()\
             && !m_pRegVueEdit->text().isEmpty()  && !m_pRegFielTabNameEdit->text().isEmpty());
}

void InputNewRecordDlg::ConfirmBtnSlot(void)
{
    if(CheckAllValid())
    {
        this->accept();
    }
    else
    {
        QMessageBox::warning(this,"Notice","Please fill all info before confirm the dialog!!!");
    }
}
