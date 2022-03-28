#include "TranslateDialog.h"

#include <qlayout.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtextstream.h>


TranslateDialog::TranslateDialog(QWidget *parent) : QDialog(parent)
{
    // ²»ÏÔÊ¾ÎÊºÅ
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
	connect(m_pbtnBinaryTranslat2Text, &QPushButton::clicked, this, &TranslateDialog::OnClickBinaryTranslate2Text);

	m_pbtnOpenTextFile = new QPushButton("Open Text File", this);
	connect(m_pbtnOpenTextFile, &QPushButton::clicked, this, &TranslateDialog::OnClickOpenTextFile);

	m_plabFileName = new QLabel("File Name", this);

	m_ptxtTextContent = new QTextEdit("File Content", this);


	QHBoxLayout* horlayout1 = new QHBoxLayout();
	horlayout1->addWidget(m_pbtnBinaryTranslat2Text);
	horlayout1->addWidget(m_pbtnOpenTextFile);
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
