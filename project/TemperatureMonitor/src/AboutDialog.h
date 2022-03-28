#pragma once

#include <QDialog>
#include <qlabel.h>
#include <qtextedit.h>
#include <qgroupbox.h>


class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    //virtual ~AboutDialog();

private:
    QLabel* m_plabLogoImage;
    QLabel* m_plabAboutInfo;
    QGroupBox* m_pgrpLicence;
    QLabel* m_plabLicence;
    QTextEdit* m_peditReleaseLog;

private:
    bool InitCtrl(void);
    bool LoadReleaseInfo(void);

signals:

};

