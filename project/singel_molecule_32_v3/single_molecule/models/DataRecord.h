#ifndef DATARECORD_H
#define DATARECORD_H
#include <QObject>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <mutex>
#include <QVector>



class DataRecord: public QObject
{
    Q_OBJECT
public:
    DataRecord();
    ~DataRecord();

    bool openFile(const std::string &savePath);
public slots:
    void SaveDataSlot(const QVector<QVector<float>> vctData);
    void updateSavePathSlot(const QString savePath);
    void closeFile();

private slots:
      void fflushFile(void);
private:
    std::string getTimeStr(int nFormat);
    bool reOpenFile(int channel);
    bool CreatNewFileName(const QString &dirUrl,int channel,QString *reName);

private:
    QVector<FILE *>m_pFile;
    std::string m_sFileSavePath = "null";
    QTimer *m_flushTimer;

};

#endif // TEMPERATURERECORD_H
