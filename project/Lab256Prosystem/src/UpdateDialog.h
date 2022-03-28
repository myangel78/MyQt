#pragma once

#include <QObject>
#include <QDialog>
#include <QUrl>
#include <QNetworkRequest>
#include <QPushButton>
#include <QTextBrowser>

class QLabel;
class QGroupBox;
class QTextBrowser;
class QNetworkAccessManager;
class QNetworkReply;
class QCheckBox;
class QProgressBar;


class UpdateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateDialog(QWidget *parent = nullptr);
    ~UpdateDialog(void);

    void AutoCheckUpdate(void);

private:
    QLabel *m_pIconLab;
    QLabel *m_pCurVerDescLab;
    QLabel *m_pCurVerLab;
    QLabel *m_pLastVerDescLab;
    QLabel *m_pLastVerLab;
    QGroupBox *m_pLatestVerGpbox;
    QPushButton *m_pUpdateBtn;
    QTextBrowser *m_pLatestVerBrow;
    QProgressBar *m_pUpdateProgressBar;

    QString m_updatefilePath;
    QString m_intallPath;
    QString m_requireversion;


private slots:
    void OnUpdateBtnclickedSlot(void);
private:
    void InitCtrl(void);
    bool ParseLatestVerInfo(const QString &jsonStr,QMap<QString, QString> &reMap);
    bool OnQuerySerVersionInfo(const QUrl url, bool& bisNeedUpdate);

    bool CreatProDir(const QString &dirPath);
    bool HasGitToolEnv(void);
    QString GetGitProjectPath(const QString& path, bool& bIsExist);
    bool UpdateGitProjet(const QString& path, const bool& bIsExist);
    bool ProcessUpdateCommand(const QString &command,const QString workDir = "");
    bool SearchUpdatePackage(const QString searchPath,const QString &version,QString &upFile);
    int CompareVersion(QString verQuery, QString verLocal);

    void OnEnableUpdateBtn(const bool &chk){m_pUpdateBtn->setEnabled(chk); }
    QString PlatformPathTrans(QString str);
    bool OnUnzipLuxupFile(const QString &filepath);
    void SetLabelFontStyle(QLabel *pLabel, const QString& family, int size, QFont::Weight weight,const QString &color);
    void UpdateFailedOpera(void);
};

