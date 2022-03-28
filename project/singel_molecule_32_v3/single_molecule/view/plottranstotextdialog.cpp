#include "plottranstotextdialog.h"
#include "ui_plottranstotextdialog.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>


PlotTransToTextDialog::PlotTransToTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotTransToTextDialog)
{
    ui->setupUi(this);
    // 不显示问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);
    this->setWindowTitle("Data conversion and viewing");
}

PlotTransToTextDialog::~PlotTransToTextDialog()
{
    delete ui;
}

void PlotTransToTextDialog::on_openBinFileBtn_clicked()
{
    QString curPath=QDir::currentPath();
    QString dlgTitle=QStringLiteral("Open Binary File");
    QString filter="Binary(*.dat)";
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if (aFileName.isEmpty())
        return;
    openTextByIODevice(aFileName);
}

bool PlotTransToTextDialog::openTextByIODevice(const QString &aFileName)
{

    QFileInfo fileInfo;
    QString  fileBaseName, filePath,fileName;
    fileInfo = QFileInfo(aFileName);
    fileBaseName = fileInfo.baseName();
    filePath = fileInfo.absolutePath();
    fileName =  filePath + "/" + fileBaseName +  ".txt";
    qDebug() << fileName<<endl;


    float reciveBuf[1024];
    FILE* fileread = fopen(aFileName.toStdString().c_str(), "rb");
    FILE* fileWrite = fopen(fileName.toStdString().c_str(), "wt");
    if(fileread == nullptr || fileWrite == nullptr)
    {
        QMessageBox::information(this,
                   tr("Failed to save the image"),
                   tr("Failed to save the image!"));
        return false;
    }


    while(1)
    {
        size_t  readCount = fread(&reciveBuf[0], sizeof(float), sizeof(reciveBuf)/sizeof(float), fileread);
        QString temp= "";
        if(readCount >0)
        {
            for(int i = 0; i < readCount; i++)
            {
               fprintf(fileWrite,"%.1f ",reciveBuf[i]);
            }
        }
        else
        {
            qDebug()<<"file has read out";
            QMessageBox::information(this,
                        tr("Succeess save the file"),
                        tr("Successfuly save in path: ")+fileName);
            break;
        }
    }

    return true;
}

void PlotTransToTextDialog::on_openTextBtn_clicked()
{
    QString fileName;
   fileName = QFileDialog::getOpenFileName(this,"Open File","","Text File(*.txt)");
   if(fileName == "")
   {
       return ;
   }
   else
   {
       QFile file(fileName);
       if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
           QMessageBox::warning(this,"error","open file error!");
           return;
       }
       else
       {
           if(!file.isReadable())
               QMessageBox::warning(this,"error","this file is not readable!");
           else
           {
               QTextStream textStream(&file);
               while(!textStream.atEnd())
               {
                   ui->textEdit->setPlainText(textStream.readAll());
               }
               ui->FileNameLab->setText(fileName);
               file.close();
           }
       }
   }


}
