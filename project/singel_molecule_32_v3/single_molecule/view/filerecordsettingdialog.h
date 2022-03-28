#ifndef FILERECORDSETTINGDIALOG_H
#define FILERECORDSETTINGDIALOG_H

#include <QDialog>
#include <QRegExpValidator>
namespace Ui {
class FileRecordSettingDialog;
}


enum DATA_TYPE
{
    MULTI32 = 32,
    MULTI256 = 256,
    MULTI1024 = 1024,
};

enum SAMPLING_RATE
{
    TEN_THOUSAND = 10000,
    TWENTY_THOUSAND = 20000,
    ONE_HUNDRED_THOUSAND = 100000,
};

enum ANALYSIS_REQUIREMENTS
{
    DEBUGING_BOARD = 0,
    ACQUIRE_RAW_DATA,
    EMBEDDED_HOLE_AND_SEQUENCING,
};


class FileRecordSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileRecordSettingDialog(QWidget *parent = nullptr);
    ~FileRecordSettingDialog();
    QString creatDataFolder(void);    
    void UpdateDirPath(const QString &Path);
    void updateEndTime(void);
    void UpdateSampRate(const int &value);

private slots:
    void on_nOkBtn_clicked();

private:
    void InitViewer(void);
    void InitConnect(void);
    void SaveSettingAsJson(void);
    void LoadFileClickSlot(void);
    bool parseJson(const QString &fileName);

    bool CreatTopDir(QString &reTopDir);
    QString CreatSubDir(const QString &TopDir);
    bool tryMakeDir(const QString& sPath);

    Ui::FileRecordSettingDialog *ui;
    QString m_curChooseDir;
    QString m_topDirPath;
    QString m_jsonFilePath;
};

#endif // FILERECORDSETTINGDIALOG_H
