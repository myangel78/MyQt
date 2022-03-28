#include "MainWindow.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include "ComunicateTestWdgt.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    InitCtrl();
}

MainWindow::~MainWindow()
{
}

void MainWindow::InitCtrl(void)
{
    m_pCenterWdgt = new QWidget(this);
    m_pComunicateTestWdgt = new ComunicateTestWdgt(this);
    m_pComunicateTestWdgt->OnSetUsbDevice(&m_usbFtdDevice);

    m_pSearchUsbDeviceBtn = new QPushButton(QStringLiteral("Seartch"),this);
    m_pOpenUsbDeviceBtn = new QPushButton(QStringLiteral("Open"),this);
    m_pUsbDeviceItemLab = new QLabel(QStringLiteral("Device:"),this);
    m_pUsbDeviceItemComb= new QComboBox(this);
    m_pUsbDeviceItemComb->setMinimumWidth(300);
    m_pOpenUsbDeviceBtn->setCheckable(true);

    QHBoxLayout *searchHorlayout =new QHBoxLayout();
    searchHorlayout->addWidget(m_pUsbDeviceItemLab);
    searchHorlayout->addWidget(m_pUsbDeviceItemComb);
    searchHorlayout->addWidget(m_pSearchUsbDeviceBtn);
    searchHorlayout->addWidget(m_pOpenUsbDeviceBtn);
    searchHorlayout->addStretch();

    QVBoxLayout *vertlayout = new QVBoxLayout();
    vertlayout->addLayout(searchHorlayout);
    vertlayout->addWidget(m_pComunicateTestWdgt);
    vertlayout->addStretch();

    m_pCenterWdgt->setLayout(vertlayout);
    setCentralWidget(m_pCenterWdgt);

    connect(m_pSearchUsbDeviceBtn,&QPushButton::clicked,this,&MainWindow::OnSearchPort);
    connect(m_pOpenUsbDeviceBtn,&QPushButton::clicked,this,&MainWindow::OnOpenPort);
}


bool MainWindow::OnSearchPort(void)
{
    m_pUsbDeviceItemComb->clear();
    QStringList devDecrList;
    m_usbFtdDevice.OnSearchUsbDevice(devDecrList);
    if(!devDecrList.isEmpty())
    {
        m_pUsbDeviceItemComb->addItems(devDecrList);
    }
    return m_pUsbDeviceItemComb->count() != 0;
}

void MainWindow::OnOpenPort(bool checked)
{
    if (checked)
    {
        int cursel = m_pUsbDeviceItemComb->currentIndex();
        if (cursel >= 0 )
        {
            bool bret = m_usbFtdDevice.OpenDeviceByDescr(m_pUsbDeviceItemComb->currentText());
            if (bret)
            {
                m_pOpenUsbDeviceBtn->setIcon(QIcon(":/img/img/close.ico"));
                m_pOpenUsbDeviceBtn->setText(QString("Close"));
                return;
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"), tr("Open device failed!!!"));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"), tr("No available device!!!"));
        }
        m_pOpenUsbDeviceBtn->setChecked(false);
    }
    else
    {
        m_usbFtdDevice.CloseDevice();
        m_pOpenUsbDeviceBtn->setIcon(QIcon(":/img/img/connect.ico"));
        m_pOpenUsbDeviceBtn->setText(QString("Open"));
    }
}
