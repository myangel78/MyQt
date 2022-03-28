#ifndef FILESELECTDIALOG_H
#define FILESELECTDIALOG_H

#include <QDialog>

namespace Ui {
class FileSelectDialog;
}

class ChannelItem;

class FileSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileSelectDialog(QWidget *parent = nullptr);
    ~FileSelectDialog();

private:
    void initView(void);

    Ui::FileSelectDialog *ui;
    std::vector<std::shared_ptr<ChannelItem>> m_vctChannelItem;
};

#endif // FILESELECTDIALOG_H
