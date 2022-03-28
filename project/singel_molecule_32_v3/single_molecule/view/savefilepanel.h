#ifndef SAVEFILEPANEL_H
#define SAVEFILEPANEL_H

#include <QWidget>
#include "view/filerecordsettingdialog.h"
#include "view/plottranstotextdialog.h"
#include "view/fileselectdialog.h"
#include <QSettings>

namespace Ui {
class SaveFIlePanel;
}

class SaveFIlePanel : public QWidget
{
    Q_OBJECT

public:
    explicit SaveFIlePanel(QWidget *parent = nullptr);
    ~SaveFIlePanel();

    QString creatDataFolder(void);
    void loadConfig(QSettings *config);
    void saveConfig(QSettings *config);
    void onFileSettingEnable(bool state);
public slots:
    void UpdateSampRateSlot(const int &rate);
    void on_stopSaveBtn_clicked();

signals:
    void saveFilePathSig(const QString savePath);
    void saveFileStopSig(void);

private slots:
    void on_startSaveBtn_clicked();
    void fileSettingShow(void);
    void on_openToolBtn_clicked();
    void onRetrievePlotClick(void);

    void on_transformBtn_clicked();

    void on_filePathChoseBtn_clicked();

    void on_FileChannelSelectBtn_clicked();

private:
    void initWidget(void);
    FileRecordSettingDialog * m_pFileRecordSettingDialog = new FileRecordSettingDialog(this);
    FileSelectDialog *m_pFileSelectDialog = new FileSelectDialog(this);
    QString m_saveFileDir;


private:
    Ui::SaveFIlePanel *ui;
};

#endif // SAVEFILEPANEL_H
