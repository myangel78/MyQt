#include "MainWindow.h"
#include <QHBoxLayout>


#include "Log.h"
#include "LogPane.h"
#include "TemperatureMonitor.h"
#include "AboutDialog.h"
#include "mainversion.h"

ecoli::CEcoliLog* ecoli::CEcoliLog::m_instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    InitCtrl();

    LoadStyleSheet(":/qss/qss/myStyle.qss");
    QString strTitle = QString("%1 %2").arg(SOFTWARE_NAME).arg(SOFTWARE_VERSION);
    setWindowTitle(strTitle);
}

MainWindow::~MainWindow()
{
}

void MainWindow::InitCtrl(void)
{
    ecoli::CEcoliLog::GetInstance()->InitEcoliLogger(true);
    m_pTemperatureMonitor = new TemperatureMonitor(this);
    setCentralWidget(m_pTemperatureMonitor);

    m_pMenuHelp = menuBar()->addMenu(tr("&Help"));

    //add about
    m_pactAbout = new QAction(QIcon(":/img/img/about.png"), tr("About"), this);
    m_pactAbout->setStatusTip(tr("About CycloneG30"));
    m_pMenuHelp->addAction(m_pactAbout);

    connect(m_pactAbout, &QAction::triggered, this, &MainWindow::OnShowAbout);
}


void MainWindow::LoadStyleSheet(const QString &style, QWidget* pObject)
{
    QFile styleSheet(style);
    if (!styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LOGI("Can't open the style sheet file.");
        return;
    }
    if(pObject != NULL)
        pObject->setStyleSheet(styleSheet.readAll());
    else
        qApp->setStyleSheet(styleSheet.readAll());

    styleSheet.close();
}


void MainWindow::OnShowAbout(void)
{
    //QMessageBox::information(this, tr("Info"), tr("This is LAB256 Pro System."));
    AboutDialog dlgAbout(this);
    dlgAbout.exec();
}
