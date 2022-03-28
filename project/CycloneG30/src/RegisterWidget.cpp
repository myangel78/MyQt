#include "RegisterWidget.h"
#include <QCoreApplication>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>

#include "SqlDataBase.h"
#include "RegisterMapWdgt.h"
#include "ComunicateTestWdgt.h"

RegisterWidget::RegisterWidget(UsbFtdDevice *usbFtdDevice,QWidget *parent):m_pUsbFtdDevice(usbFtdDevice),QWidget(parent)
{
    InitCtr();
}


RegisterWidget::~RegisterWidget()
{
    delete m_pRegisterMapWdgt;
    delete m_pComunicateTestWdgt;


}

void RegisterWidget::InitCtr(void)
{

    m_pTabSwitWidget = new QTabWidget(this);
    m_pRegisterMapWdgt= new RegisterMapWdgt(m_pTabSwitWidget);
    m_pTabSwitWidget->addTab(m_pRegisterMapWdgt,QIcon(":/img/img/reg.png"),QStringLiteral("RegisterMap"));


    //registermap widget
    m_pComunicateTestWdgt = new ComunicateTestWdgt(m_pTabSwitWidget);
    m_pComunicateTestWdgt->OnSetUsbDevice(m_pUsbFtdDevice);
    m_pTabSwitWidget->addTab(m_pComunicateTestWdgt,QIcon(":/img/img/test.png"),QStringLiteral("Comunicate"));


    connect(m_pRegisterMapWdgt,SIGNAL(ASICRegisterWiteOneSig(const uchar &,const uint &)) ,this,SIGNAL(ASICRegisterWiteOneSig(const uchar &,const uint &)));
    connect(m_pRegisterMapWdgt,SIGNAL(ASICRegisterReadOneSig(const uchar &,uint &,bool &)) ,this,SIGNAL(ASICRegisterReadOneSig(const uchar &,uint &,bool &)));
    connect(m_pRegisterMapWdgt,SIGNAL(ASICRegisterReadAllSig(std::vector<std::tuple<uchar, uint32_t >> &,bool &)) ,this,SIGNAL(ASICRegisterReadAllSig(std::vector<std::tuple<uchar, uint32_t >> &,bool &)));

    QVBoxLayout *vertlayout = new QVBoxLayout();
    vertlayout->addWidget(m_pTabSwitWidget);
    setLayout(vertlayout);

}



