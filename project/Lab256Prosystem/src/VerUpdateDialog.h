#pragma once

#include <QDialog>
#include <qlabel.h>
#include <qtextedit.h>
#include <qgroupbox.h>
#include <qpushbutton.h>


class VerUpdateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VerUpdateDialog(QWidget *parent = nullptr);

    virtual void accept();
    virtual void reject();

private:
    QLabel* m_plabLogoImage;
    QLabel* m_plabVerInfo;
    QLabel* m_plabCurVersion;
    QLabel* m_plabCurVersion2;
    QLabel* m_plabLatestVersion;
    QLabel* m_plabLatestVersion2;
    QLabel* m_plabHasUpdate;

    QGroupBox* m_pgrpUpdateInfo;
    QTextEdit* m_peditUpdateInfo;

    QPushButton* m_pbtnUpdateNow;
    QPushButton* m_pbtnIgnoreVer;


    bool m_bExitMain = false;
    bool m_bHasUpdate = false;
    int m_iMustUpdate = 0;
    std::string m_strVersion;

private:
    bool InitCtrl(void);

public:
    bool RequestVersionInfo(void);

    bool GetExitMain(void) const { return m_bExitMain; }
    bool GetHasUpdate(void) const { return m_bHasUpdate; }
    int GetMustUpdate(void) const { return m_iMustUpdate; }
    const std::string GetVersion(void) const { return m_strVersion; }

private slots:
    void OnClickUpdatenow(void);
    void OnClickIgnoreVersion(void);

    void failedVersionInfoSlot(void);
    void parseVersionInfoSlot(const QString& qres);

signals:
    void failedVersionInfoSignal(void);
    void parseVersionInfoSignal(const QString& qres);
};

