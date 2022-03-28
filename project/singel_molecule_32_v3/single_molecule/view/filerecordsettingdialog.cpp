#include "filerecordsettingdialog.h"
#include "ui_filerecordsettingdialog.h"
#include "helper/appconfig.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>


FileRecordSettingDialog::FileRecordSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileRecordSettingDialog)
{

    // 不显示问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);
    setFixedSize(400, 650); // 不能伸缩的对话框

    ui->setupUi(this);

    InitViewer();
    InitConnect();

    m_curChooseDir= "";
    m_topDirPath = "";
    m_jsonFilePath = "";


}


FileRecordSettingDialog::~FileRecordSettingDialog()
{
    delete ui;
}

void FileRecordSettingDialog::InitViewer(void)
{
    QRegExpValidator *pPersonneLReg = new QRegExpValidator(QRegExp("^[A-Za-z]+$"));
    ui->npersonneLiineEdit->setValidator(pPersonneLReg);

    QRegExpValidator *pRunNumReg = new QRegExpValidator(QRegExp("^[0-9][0-9][0-9]?$"));
    ui->nRunNumLineEdit->setValidator(pRunNumReg);

    ui->nDataTypeComb->addItem(QStringLiteral("multi32_V3"),DATA_TYPE::MULTI32);
    ui->nDataTypeComb->setCurrentIndex(0);

}

void FileRecordSettingDialog::InitConnect(void)
{
    connect(ui->nLoadTemplateBtn,&QPushButton::clicked,this,&FileRecordSettingDialog::LoadFileClickSlot);
}


bool FileRecordSettingDialog::parseJson(const QString &fileName)
{

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         qDebug()<<"json Opened Failed";
         return false;
    }
    QByteArray value = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument parse_document(QJsonDocument::fromJson(value,&jsonError));

    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug()<<"json error...";
        return false;
    }

    if(parse_document.isObject())
    {
        QJsonObject jsonObject = parse_document.object();
        if(jsonObject.contains("Personnel"))
        {
            ui->npersonneLiineEdit->setText(jsonObject.value("Personnel").toString());
        }
        if(jsonObject.contains("DeviceInfo"))
        {
            ui->nDeviceInfoLineEdit->setText(jsonObject.value("DeviceInfo").toString());
        }
        if(jsonObject.contains("ChipInfo"))
        {
            ui->nChipInfoLineEdit->setText(jsonObject.value("ChipInfo").toString());
        }
        if(jsonObject.contains("Run_num"))
        {
            ui->nRunNumLineEdit->setText(jsonObject.value("Run_num").toString());
        }
        if(jsonObject.contains("StartTime"))
        {
            ui->nStartTimeLineEdit->setText(jsonObject.value("StartTime").toString());
        }
        if(jsonObject.contains("EndTime"))
        {
            ui->nEndTimeLineEdit->setText(jsonObject.value("EndTime").toString());
        }
        if(jsonObject.contains("Location"))
        {
            ui->nLocationlineEdit->setText(jsonObject.value("Location").toString());
        }
        if(jsonObject.contains("DataType"))
        {
            ui->nDataTypeComb->setCurrentText(jsonObject.value("DataType").toString());
        }
        if(jsonObject.contains("GenomeSize(bp)"))
        {
            ui->nGenomeSizeLineEdit->setText(jsonObject.value("GenomeSize(bp)").toString());
        }
        if(jsonObject.contains("Plasmid(bp)"))
        {
            ui->nPlasmidLineEdit->setText(jsonObject.value("Plasmid(bp)").toString());
        }
        if(jsonObject.contains("LibraryDNA"))
        {
            ui->nDnaLineEdit->setText(jsonObject.value("LibraryDNA").toString());
        }
        if(jsonObject.contains("Temperature"))
        {
            ui->nTemperLineEdit->setText(jsonObject.value("Temperature").toString());
        }
        if(jsonObject.contains("BufferID"))
        {
            ui->nBufferIdLineEdit->setText(jsonObject.value("BufferID").toString());
        }
        if(jsonObject.contains("PorinID"))
        {
            ui->nPorinIdLineEdit->setText(jsonObject.value("PorinID").toString());
        }
        if(jsonObject.contains("MotorID"))
        {
            ui->nMotorLineEdit->setText(jsonObject.value("MotorID").toString());
        }
        if(jsonObject.contains("LibraryAdaptor"))
        {
            ui->nAdaptorLineEdit->setText(jsonObject.value("LibraryAdaptor").toString());
        }
        if(jsonObject.contains("OriginalSamplingRate(Hz)"))
        {
            QString value = jsonObject.value("OriginalSamplingRate(Hz)").toString().toUpper().split("K").first();
            ui->nSampRatespbox->setValue(value.toInt());
        }
        if(jsonObject.contains("SequencingVoltage(V)"))
        {
            ui->nSequenceLineEdit->setText(jsonObject.value("SequencingVoltage(V)").toString());
        }
        if(jsonObject.contains("LowPassFilter(Hz)"))
        {
            ui->nLowFilterLineEdit->setText(jsonObject.value("LowPassFilter(Hz)").toString());
        }
        if(jsonObject.contains("Features"))
        {
            ui->nFeatureLineEdit->setText(jsonObject.value("Features").toString());
        }
        if(jsonObject.contains("OtherInfo"))
        {
            ui->OtherInfoLineEdit->setText(jsonObject.value("OtherInfo").toString());
        }
    }
    return true;
}

void FileRecordSettingDialog::SaveSettingAsJson(void)
{
    QDateTime current_time = QDateTime::currentDateTime();
    //显示时间，格式为：年-月-日 时：分：秒 周几
    QString StrCurrentTime = current_time.toString("yyyyMMddhhmmss");
    QString nJsonName = StrCurrentTime;
    if(ui->npersonneLiineEdit->text() != "")
        nJsonName += "_" + ui->npersonneLiineEdit->text();
    if(ui->nPorinIdLineEdit->text() != "")
        nJsonName += "_" + ui->nPorinIdLineEdit->text();
    if(ui->nDnaLineEdit->text() != "")
        nJsonName += "_" + ui->nDnaLineEdit->text();
    nJsonName += ".json";

    ui->nStartTimeLineEdit->setText(current_time.toString("yyyy-MM-dd hh:mm"));
    ui->nFolderPathLineEdit->setText(QDir(m_topDirPath).dirName());

    m_jsonFilePath = m_topDirPath + "/" + nJsonName ;
    QFile file(m_jsonFilePath);
    if(!file.open(QIODevice::ReadWrite))
    {
        qDebug()<< m_jsonFilePath<<"File open error";
        return;
    }

    QJsonObject jsonObject;
    jsonObject.insert("Personnel",ui->npersonneLiineEdit->text());
    jsonObject.insert("DeviceInfo",ui->nDeviceInfoLineEdit->text());
    jsonObject.insert("ChipInfo",ui->nChipInfoLineEdit->text());
    jsonObject.insert("Run_num",ui->nRunNumLineEdit->text());
    jsonObject.insert("StartTime",ui->nStartTimeLineEdit->text());
    jsonObject.insert("EndTime",ui->nEndTimeLineEdit->text());
    jsonObject.insert("Location",ui->nLocationlineEdit->text());
    jsonObject.insert("DataType",ui->nDataTypeComb->currentText());
    jsonObject.insert("GenomeSize(bp)",ui->nGenomeSizeLineEdit->text());
    jsonObject.insert("Plasmid(bp)",ui->nPlasmidLineEdit->text());
    jsonObject.insert("Temperature",ui->nTemperLineEdit->text());
    jsonObject.insert("BufferID",ui->nBufferIdLineEdit->text());
    jsonObject.insert("PorinID",ui->nPorinIdLineEdit->text());
    jsonObject.insert("MotorID",ui->nMotorLineEdit->text());
    jsonObject.insert("LibraryAdaptor",ui->nAdaptorLineEdit->text());
    jsonObject.insert("LibraryDNA",ui->nDnaLineEdit->text());
    jsonObject.insert("OriginalSamplingRate(Hz)",ui->nSampRatespbox->text());
    jsonObject.insert("SequencingVoltage(V)",ui->nSequenceLineEdit->text());
    jsonObject.insert("LowPassFilter(Hz)",ui->nLowFilterLineEdit->text());
    jsonObject.insert("Features",ui->nFeatureLineEdit->text());
    jsonObject.insert("OtherInfo",ui->OtherInfoLineEdit->text());
    jsonObject.insert("FolderPath",ui->nFolderPathLineEdit->text());


    // 使用QJsonDocument设置该json对象
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);

    // 将json以文本形式写入文件并关闭文件。
    file.write(jsonDoc.toJson());
    file.close();

    qDebug() << "SaveSettingAsJson to file Success";

}

void FileRecordSettingDialog::updateEndTime(void)
{
    QDateTime current_time = QDateTime::currentDateTime();
    QString StrCurrentTime = current_time.toString("yyyy-MM-dd hh:mm");
    ui->nEndTimeLineEdit->setText(StrCurrentTime);

    QFile file(m_jsonFilePath);
    if(!file.open(QIODevice::ReadWrite))
    {
        qDebug()<< m_jsonFilePath<<"File open error";
        return;
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    // 修改 age
    obj["EndTime"] = ui->nEndTimeLineEdit->text();
    jdc.setObject(obj);
    file.seek(0);
    file.write(jdc.toJson());
    file.flush();
}

void FileRecordSettingDialog::UpdateSampRate(const int &value)
{
    ui->nSampRatespbox->setValue(value);
}

QString FileRecordSettingDialog::creatDataFolder(void)
{
    QString topDir = "";
    QString subDir = "";
    if(CreatTopDir(topDir))
    {
        m_topDirPath = topDir;
        SaveSettingAsJson();
        subDir = CreatSubDir(topDir);
        if(subDir != "")
        {
            return subDir;
        }
    }
    return "";
}

QString FileRecordSettingDialog::CreatSubDir(const QString &TopDir)
{
    QString subDir = TopDir + "/" + "Raw/";
    if(tryMakeDir(subDir))
    {
        return subDir;
    }
    else
        return "";
}

bool FileRecordSettingDialog::CreatTopDir(QString &reTopDir)
{
    QString nFeatures  ="";
    QString current_date =QDateTime::currentDateTime().toString("yyyyMMdd");
    QString nDataType = "_" + QString("%1").arg(ui->nDataTypeComb->currentText());
    if(ui->nFeatureLineEdit->text() != "")
        nFeatures  ="_" + QString("%1").arg(ui->nFeatureLineEdit->text());
    QString strRate = ui->nSampRatespbox->text();
    QString fixTopDir = m_curChooseDir + "/" + current_date + nDataType +"_"+ strRate + nFeatures;
    QString tmpTopDir = fixTopDir;
    int count = 1;
    while(QDir(tmpTopDir).exists())
    {
        tmpTopDir = fixTopDir + "_" +QString::number(count++);
    }
    if(tryMakeDir(tmpTopDir))
    {
        reTopDir = tmpTopDir;
        return true;
    }
    return false;

}

bool FileRecordSettingDialog::tryMakeDir(const QString& sPath)
{
    bool bRet = false;
    QDir newDir(sPath);
    if(!(bRet = newDir.exists()))
    {
        QDir dir("");
        bRet = dir.mkpath(sPath);
    }
    return bRet;
}



void FileRecordSettingDialog::UpdateDirPath(const QString &Path)
{
    m_curChooseDir = Path;
}


void FileRecordSettingDialog::LoadFileClickSlot(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Json File"), QString::fromStdString(AppConfig::getDataFolder()),
        "Json (*.json)",nullptr,
        QFileDialog::HideNameFilterDetails);
    if (fileName.isEmpty()) {
        return;
    }
     qDebug()<<"fileName "<<fileName;
     if(!parseJson(fileName))
     {
         QMessageBox::warning(this, tr("Warning"),
                              tr("LoadJsonFile is failed."));
     }
}


void FileRecordSettingDialog::on_nOkBtn_clicked()
{
    this->hide();
}
