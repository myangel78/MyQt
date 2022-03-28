#include "savefilepanel.h"
#include "ui_savefilepanel.h"
#include "helper/appconfig.h"
#include "models/DatacCommunicaor.h"
#include "view/plotreviewdialog.h"
#include "QFileDialog"
#include <QMessageBox>

#define g_pCommunicaor DataCommunicaor::Instance()


SaveFIlePanel::SaveFIlePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SaveFIlePanel)
{
    ui->setupUi(this);

    m_saveFileDir =QString::fromStdString(AppConfig::getDataFolder());
    ui->filePathLineEdit->setText(m_saveFileDir);
    m_pFileRecordSettingDialog->UpdateDirPath(m_saveFileDir);

    initWidget();


}

SaveFIlePanel::~SaveFIlePanel()
{
    delete m_pFileRecordSettingDialog;
    delete ui;
}

void SaveFIlePanel::loadConfig(QSettings *config)
{

}
void SaveFIlePanel::saveConfig(QSettings *config)
{

}

void SaveFIlePanel::initWidget(void)
{
    ui->openToolBtn->setText("Open Directory");
    ui->openToolBtn->setIcon(QIcon(":/images/open.ico"));
//    ui->openToolBtn->setIconSize(QSize(30,30));
    ui->openToolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->openToolBtn->setAutoRaise(false);
    ui->filePathLineEdit->setReadOnly(true);


    ui->startSaveBtn->setIcon(QIcon(":/images/run.png"));
    ui->startSaveBtn->setIconSize(QSize(30,30));
    ui->stopSaveBtn->setIcon(QIcon(":/images/stop.png"));
    ui->stopSaveBtn->setIconSize(QSize(30,30));


    ui->saveSettingBtn->setIcon(QIcon(":/images/version.png"));
    ui->saveSettingBtn->setIconSize(QSize(30,30));

    ui->FileChannelSelectBtn->setIcon(QIcon(":/images/config.png"));
    ui->FileChannelSelectBtn->setIconSize(QSize(30,30));

    connect(ui->saveSettingBtn,SIGNAL(clicked()),this,SLOT(fileSettingShow()));
    connect(ui->retrieveBtn,SIGNAL(clicked(bool)),this,SLOT(onRetrievePlotClick()));
}


void SaveFIlePanel::onFileSettingEnable(bool state)
{
    ui->saveSettingBtn->setEnabled(state);
}

void SaveFIlePanel::on_startSaveBtn_clicked()
{
    QString savePath =  m_pFileRecordSettingDialog->creatDataFolder();
//    qDebug()<<savePath;
    if(savePath == "")
    {
         QMessageBox::warning(this,"error","Can't not creat save dir");
    }

    ui->stopSaveBtn->setEnabled(true);
    ui->startSaveBtn->setEnabled(false);
    ui->filePathLineEdit->setEnabled(false);
    ui->filePathChoseBtn->setEnabled(false);
    ui->saveSettingBtn->setEnabled(false);
    emit saveFilePathSig(savePath);
//    g_pCommunicaor->g_IsSaveToFiles = true;

}

void SaveFIlePanel::UpdateSampRateSlot(const int &rate)
{
    if(rate != 0)
    {
        int dpRate = rate/1000;
        m_pFileRecordSettingDialog->UpdateSampRate(dpRate);
    }
}

void SaveFIlePanel::on_stopSaveBtn_clicked()
{
    ui->stopSaveBtn->setEnabled(false);
    ui->startSaveBtn->setEnabled(true);
    ui->filePathLineEdit->setEnabled(true);
    ui->filePathChoseBtn->setEnabled(true);
    ui->saveSettingBtn->setEnabled(true);

    m_pFileRecordSettingDialog->updateEndTime();
    emit saveFileStopSig();

    g_pCommunicaor->g_IsSaveToFiles = false;
}

QString SaveFIlePanel::creatDataFolder(void)
{
    return m_pFileRecordSettingDialog->creatDataFolder();
}


void SaveFIlePanel::fileSettingShow(void)
{
    m_pFileRecordSettingDialog->show();
}


void SaveFIlePanel::on_openToolBtn_clicked()
{
    QString path, cmd;
    path = ui->filePathLineEdit->text().replace("/", "\\");
    cmd = QString("explorer.exe /select,%1").arg(path);

    QProcess process;
    process.startDetached(cmd);
}


void SaveFIlePanel::onRetrievePlotClick(void)
{
    PlotReViewDialog plotreviewdialog(this);
    plotreviewdialog.exec();
}


void SaveFIlePanel::on_transformBtn_clicked()
{
    PlotTransToTextDialog plottranstotextdialog(this);
    plottranstotextdialog.exec();
}

void SaveFIlePanel::on_filePathChoseBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Save Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(dir == "")
    {
        return;
    }
    else
    {
        ui->filePathLineEdit->setText(dir);
        m_pFileRecordSettingDialog->UpdateDirPath(ui->filePathLineEdit->text());
    }
}

void SaveFIlePanel::on_FileChannelSelectBtn_clicked()
{
    m_pFileSelectDialog->show();
}
