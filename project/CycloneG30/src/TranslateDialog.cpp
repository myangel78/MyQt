#include "TranslateDialog.h"

#include <qlayout.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include "ConfigServer.h"


TranslateDialog::TranslateDialog(QWidget *parent) : QDialog(parent)
{
    // 不显示问号
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
    InitCtrl();
    resize(600, 400);
}

//TranslateDialog::~TranslateDialog()
//{
//    delete m_plabFileName;
//    delete m_pbtnBinaryTranslat2Text;
//    delete m_pbtnOpenTextFile;
//    delete m_ptxtTextContent;
//}

bool TranslateDialog::InitCtrl(void)
{
	m_pbtnBinaryTranslat2Text = new QPushButton("Binary Translate to Text", this);
    m_pbtnBinaryTranslatToRawData = new QPushButton("Back to rawData", this);
	m_pbtnOpenTextFile = new QPushButton("Open Text File", this);

	connect(m_pbtnOpenTextFile, &QPushButton::clicked, this, &TranslateDialog::OnClickOpenTextFile);
    connect(m_pbtnBinaryTranslat2Text, &QPushButton::clicked, this, &TranslateDialog::OnClickBinaryTranslate2Text);
    connect(m_pbtnBinaryTranslatToRawData, &QPushButton::clicked, this, &TranslateDialog::OnClickBinaryTranslateToRawData);

	m_plabFileName = new QLabel("File Name", this);

	m_ptxtTextContent = new QTextEdit("File Content", this);


	QHBoxLayout* horlayout1 = new QHBoxLayout();
	horlayout1->addWidget(m_pbtnBinaryTranslat2Text);
	horlayout1->addWidget(m_pbtnOpenTextFile);
    horlayout1->addWidget(m_pbtnBinaryTranslatToRawData);
	horlayout1->addStretch(1);

	QVBoxLayout* verlayout6 = new QVBoxLayout();
	verlayout6->addLayout(horlayout1);
	verlayout6->addWidget(m_plabFileName);
	verlayout6->addWidget(m_ptxtTextContent);

	setLayout(verlayout6);

	return false;
}

void TranslateDialog::OnClickBinaryTranslate2Text(void)
{
	QString aFileName = QFileDialog::getOpenFileName(this, QStringLiteral("Open Binary File"), "", "Binary(*.dat)");
	if (!aFileName.isEmpty())
	{
        QFileInfo fileInfo(aFileName);
        QString fileBaseName = fileInfo.baseName();
        QString filePath = fileInfo.absolutePath();
        QString tFileName = filePath + "/" + fileBaseName + ".txt";

        float reciveBuf[1024];
        FILE* fileread = fopen(aFileName.toStdString().c_str(), "rb");
        FILE* fileWrite = fopen(tFileName.toStdString().c_str(), "wt");
        if (fileread != nullptr && fileWrite != nullptr)
        {
            while (true)
            {
                size_t  readCount = fread(&reciveBuf[0], sizeof(float), sizeof(reciveBuf) / sizeof(float), fileread);
                QString temp = "";
                if (readCount > 0)
                {
                    for (int i = 0; i < readCount; i++)
                    {
                        fprintf(fileWrite, "%.1f ", reciveBuf[i]);
                    }
                }
                else
                {
                    fclose(fileread);
                    fclose(fileWrite);
                    QMessageBox::information(this, tr("Success save the file"), tr("Success save in path: ") + tFileName);
                    break;
                }
            }
            return;
        }
        QMessageBox::information(this, tr("Failed save the file"), tr("Failed save in path: ") + tFileName);
    }
}

void TranslateDialog::OnClickOpenTextFile(void)
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text File(*.txt)");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Warning", "Open file FAILED!");
            return;
        }
        else
        {
            if (!file.isReadable())
            {
                QMessageBox::warning(this, "Warning", "This file is not readable!");
            }
            else
            {
                QTextStream textStream(&file);
                while (!textStream.atEnd())
                {
                    m_ptxtTextContent->setPlainText(textStream.readAll());
                }
                m_plabFileName->setText(fileName);
                file.close();
            }
        }
    }
}

void TranslateDialog::OnClickBinaryTranslateToRawData(void)
{
    QString aFileName = QFileDialog::getOpenFileName(this, QStringLiteral("Open Binary File"), "", "Binary(*.dat)");
    if (!aFileName.isEmpty())
    {
        QFileInfo fileInfo(aFileName);
        QString fileBaseName = fileInfo.baseName();
        QString filePath = fileInfo.absolutePath();
        QString tFileName = filePath + "/" + fileBaseName + "_raw.dat";

        std::vector<float> reciveBuf(4096,0);
        std::vector<float> writeBuf(4096,0);
        FILE* fileread = fopen(aFileName.toStdString().c_str(), "rb");
        FILE* fileWrite = fopen(tFileName.toStdString().c_str(), "wb");
        if (fileread != nullptr && fileWrite != nullptr)
        {
            while (true)
            {
                size_t  readCount = fread(&reciveBuf[0], sizeof(float), reciveBuf.size(), fileread);
                if (readCount > 0)
                {
                    if(InverseTransformToRawData(reciveBuf,writeBuf,readCount))
                    {
                        fwrite(&writeBuf[0], sizeof(float), readCount, fileWrite);
                    }
                }
                else
                {
                    fclose(fileread);
                    fclose(fileWrite);
                    QMessageBox::information(this, tr("Success save the file"), tr("Success save in path: ") + tFileName);
                    break;
                }
            }
            return;
        }
        QMessageBox::information(this, tr("Failed save the file"), tr("Failed save in path: ") + tFileName);
    }
}

bool TranslateDialog::InverseTransformToRawData(const std::vector<float> &vctSource,std::vector<float> &vctDest,const size_t &length)
{
    if(vctSource.size() < length || vctDest.size() < length)
        return false;

    const bool bRawDat  = ConfigServer::GetInstance()->GetIsRawData();
    const float rawUnit = ConfigServer::GetInstance()->GetLvdsCurConvt().first;
    const float adcUnit = ConfigServer::GetInstance()->GetLvdsCurConvt().second;

    if(bRawDat)
    {
        std::transform(vctSource.cbegin(),vctSource.cbegin() +length ,vctDest.begin(),[&rawUnit](const float &value)
        {
            return (value/rawUnit + LVDS_CVT_RAW_MIDDLE);
        });
    }
    else
    {
        std::transform(vctSource.cbegin(),vctSource.cbegin() + length,vctDest.begin(),[&adcUnit](const float &value)
        {
            return (value/adcUnit + LVDS_CVT_ADC_MIDDLE);
        });

    }
    return true;
}
