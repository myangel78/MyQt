#include "aboutbox.h"
#include "ui_aboutbox.h"
#include <QFile>
#include "helper/version.h"
#include <QTextStream>

AboutBox::AboutBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutBox)
{
    // 不显示问号
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    ui->setupUi(this);
    setFixedSize(400, 400); // 不能伸缩的对话框

    QPixmap pix(":/images/BGI.png");
    pix = pix.scaledToWidth(110, Qt::SmoothTransformation);
    ui->label1->setPixmap(pix);
    ui->label2->setText(
        "<b>SINGEL_MOLECULE</b><br>"
        "Version: " SOFTWARE_VERSION  " (" BUILD_VERSION ")<br>"
        "theme : BGI research<br>"
        "Home Page: <a href=\"https://www.genomics.cn\">View On the website</a><br>"
        "Build Date: " __DATE__ "<br>" // 编译时间
        COPYRIGHT
    );
    ui->label2->setOpenExternalLinks(true); // 允许访问链接
    ui->ReleaseInfoText->setReadOnly(true);
    readReleaseInfo();
    ui->ReleaseInfoText->moveCursor(QTextCursor::End);
}

AboutBox::~AboutBox()
{
    delete ui;
}


void AboutBox::readReleaseInfo(void)
{
    QFile file(":/info/release.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    else
    {
        if(!file.isReadable())
           return;
        else
        {
            QTextStream textStream(&file);
            while(!textStream.atEnd())
            {
                ui->ReleaseInfoText->setPlainText(textStream.readAll());
            }
            ui->ReleaseInfoText->show();
            file.close();
        }
    }

}
