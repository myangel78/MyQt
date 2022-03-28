#include "ComunicateTestWdgt.h"
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qgridlayout.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qgroupbox.h>
#include <qdebug.h>


#include "UsbFtdDevice.h"
#include "ConfigServer.h"


ComunicateTestWdgt::ComunicateTestWdgt(QWidget *parent):QWidget(parent)
{
    InitCtrl();
}

ComunicateTestWdgt::~ComunicateTestWdgt()
{

}

void ComunicateTestWdgt::InitCtrl(void)
{
    m_pSendEdit = new QLineEdit(this);
    m_pSendButton = new QPushButton(QStringLiteral("Write"),this);
    m_pRcvTextEdit = new QTextEdit(this);
    m_pOneKeyTestButton = new QPushButton(QStringLiteral("OneKeyTest"),this);
    m_pReadButton = new QPushButton(QStringLiteral("Read"),this);
    m_pRcvClearBtn = new QPushButton(QStringLiteral("Clear"),this);

    QGroupBox *pSendGrpbox = new QGroupBox(QStringLiteral("Write"),this);
    QHBoxLayout *horsendlayout = new QHBoxLayout();
    horsendlayout->addWidget(m_pSendEdit);
    horsendlayout->addWidget(m_pSendButton);
    pSendGrpbox->setLayout(horsendlayout);

    QGroupBox *pRcvGrpbox = new QGroupBox(QStringLiteral("Read"),this);
    QGridLayout *gridrcvlayout = new QGridLayout();
    gridrcvlayout->addWidget(m_pRcvTextEdit,0,0,3,1);
    gridrcvlayout->addWidget(m_pOneKeyTestButton,1,1,1,1);
    gridrcvlayout->addWidget(m_pRcvClearBtn,2,1,1,1);
    gridrcvlayout->addWidget(m_pReadButton,3,1,1,1);
    gridrcvlayout->setColumnStretch(0,10);
    gridrcvlayout->setColumnStretch(1,1);
    pRcvGrpbox->setLayout(gridrcvlayout);

    QVBoxLayout *vertlayout = new QVBoxLayout();
    vertlayout->addWidget(pSendGrpbox);
    vertlayout->addWidget(pRcvGrpbox);
    vertlayout->addStretch();
    setLayout(vertlayout);

    m_pSendEdit->setText(tr("55 AA FF 02 00 00 00 02 FA FA"));



    connect(m_pSendButton,SIGNAL(clicked()),this,SLOT(OnSendBtnSlot()));
    connect(m_pReadButton,SIGNAL(clicked()),this,SLOT(OnReadBtnSlot()));
    connect(m_pRcvClearBtn,&QPushButton::clicked,[&](){m_pRcvTextEdit->clear();});
    connect(m_pOneKeyTestButton,&QPushButton::clicked,[&](){m_pSendButton->click(); m_pReadButton->click();});

}


void ComunicateTestWdgt::OnSendBtnSlot(void)
{
#if 0
    QString content = m_pSendEdit->text();
    UCHAR acWriteBuf[BUFFER_SIZE] = {0};
    ULONG ulBytesToWrite = sizeof(acWriteBuf);
    memcpy(acWriteBuf,content.toStdString().c_str(),content.size());
    ULONG ulActualBytesToWrite = 0;


    if(m_pUsbFtdDevice->WriteData(acWriteBuf,ulBytesToWrite,&ulActualBytesToWrite))
    {
         qDebug(">>>>>>>>>>>>>>>>>>>%d senddata actual %d %s",ulBytesToWrite,ulActualBytesToWrite,acWriteBuf);
    }
#endif



#if 1
    UCHAR acWriteBuf[BUFFER_SIZE] = {0};
    QByteArray content = QByteArray::fromHex(m_pSendEdit->text().toLatin1());
    ULONG ulBytesToWrite = content.size() > BUFFER_SIZE ? BUFFER_SIZE : content.size() ;
    ULONG ulActualBytesToWrite = 0;
    memcpy(acWriteBuf,content.data(),content.size());

    if(m_pUsbFtdDevice->WriteData(acWriteBuf,ulBytesToWrite,&ulActualBytesToWrite))
    {
         qDebug(">>>>>>>>>>>>>>>>>>>%d senddata actual %d ",ulBytesToWrite,ulActualBytesToWrite);
         for(int i = 0; i<content.size(); i++)
         {
             printf(" 0x%02X",acWriteBuf[i]);

         }
    }

#endif


}

void ComunicateTestWdgt::OnReadBtnSlot(void)
{
    UCHAR acReadBuf[BUFFER_SIZE] = {0};
    memset(acReadBuf,0x46,BUFFER_SIZE);
    ULONG ulActualBytesToRead = 0;
    ULONG ulBytesToRead = sizeof(acReadBuf);

#ifndef Q_OS_WIN32
    if(m_pUsbFtdDevice->ReadData(acReadBuf,ulBytesToRead,&ulActualBytesToRead))
#else
    if(m_pUsbFtdDevice->ReadDataASync(acReadBuf,ulBytesToRead,&ulActualBytesToRead))
#endif
    {
        qDebug("<<<<<<<<<<<<<<<<<<<<<<<<<<<Plan to read bytes: %d; recvdata actual bytes: %d ",ulBytesToRead,ulActualBytesToRead);

        QByteArray recvByte((const char *)acReadBuf,(int)ulActualBytesToRead);
        QString recvStr= recvByte.toHex(' ').toUpper();
        qDebug()<<recvStr;
        m_pRcvTextEdit->append(recvStr);
//        for(uint i = 0 ; i < ulActualBytesToRead;++i)
//        {
//            if(i%20 == 0)
//            {
//                printf("\n");
//            }
//            printf(" 0x%02X",acReadBuf[i]);
//        }
    }


}
