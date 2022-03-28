#include "fileselectdialog.h"
#include "ui_fileselectdialog.h"
#include "models/DataDefine.h"
#include "view/channelitem.h"

FileSelectDialog::FileSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSelectDialog)
{
    // 不显示问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);

    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("Select channel to Select"));

    initView();
}

FileSelectDialog::~FileSelectDialog()
{
    delete ui;
}

void FileSelectDialog::initView(void)
{
    m_vctChannelItem.resize(CHANNEL_NUM);
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
//        item->setSizeHint(QSize(60, 20));
        m_vctChannelItem[ch] = std::make_shared<ChannelItem>("CH"+ QString::number(ch+1));
        ui->listWidget->setItemWidget(item,m_vctChannelItem[ch].get());
    }

}
