#pragma once

#include <QDialog>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlistwidget.h>
#include <qdatetimeedit.h>
#include <qtimer.h>
#include <qsettings.h>
#include <string>

class QTableWidget;
class QCheckBox;
class QSignalMapper;


class RecordSettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecordSettingDialog(QWidget *parent = nullptr);
    virtual ~RecordSettingDialog();

private:
    QGroupBox* m_pgrpDataRecord;

    QLabel* m_plabTemperature;
    QDoubleSpinBox* m_pdspTemperature;

    QLabel* m_plabStartTime;
    QLineEdit* m_peditStartTime;

    QLabel* m_plabEndTime;
    QLineEdit* m_peditEndTime;

    QLabel* m_plabSequencerVersion;
    QComboBox* m_pcmbSequencerVersion;

    QLabel* m_plabSequencerID;
    QComboBox* m_pcmbSequencerID;

    QLabel* m_plabOperator;
    QComboBox* m_pcmbOperator;

    QLabel* m_plabFolderPath;
    QLineEdit* m_peditFolderPath;

    QLabel* m_plabPoreID;
    QComboBox* m_pcmbPoreID;
    QLineEdit* m_peditPoreID;

    QLabel* m_plabMotorID;
    QComboBox* m_pcmbMotorID;
    QLineEdit* m_peditMotorID;

    QLabel* m_plabPoreBatch;
    //QLineEdit* m_peditPoreBatch;
    QDateEdit* m_pdatePoreBatch;
    QSpinBox* m_pispPoreBatch;

    QLabel* m_plabSeqBuffer;
    QComboBox* m_pcmbSeqBuffer;
    QSpinBox* m_pispSeqBuffer;

    QLabel* m_plabElectrochemBufferID;
    QComboBox* m_pcmbElectrochemBufferID;

    QLabel* m_plabNotes;
    QLineEdit* m_peditNotes;

    QLabel* m_plabSeqVoltage;
    QComboBox* m_pcmbSeqVoltage;
    QDoubleSpinBox* m_pdspSeqVoltage;

    QLabel* m_plabMembrane;
    QComboBox* m_pcmbMembrane;
    QLineEdit* m_peditMembrane;

    QLabel* m_plabChipID;
    QComboBox* m_pcmbChipID;
    QLineEdit* m_peditChipID;
    QLabel* m_plabChipID2;
    QSpinBox* m_pispChipID2;

    QLabel* m_plabFlowcellID;
    QComboBox* m_pcmbFlowcellID;
    QLineEdit* m_peditFlowcellID;
    QLabel* m_plabFlowcellID2;
    QSpinBox* m_pispFlowcellID2;

    QLabel* m_plabLibraryAdaptor;
    QComboBox* m_pcmbLibraryAdaptor;
    //QLineEdit* m_peditLibraryAdaptor;
    QDateEdit* m_pdateLibraryAdaptor;

    QLabel* m_plabASIC;
    QComboBox* m_pcmbASIC;
    QLineEdit* m_peditASIC;

    QLabel* m_plabLibraryDNA;
    QComboBox* m_pcmbLibraryDNA;

    QLabel* m_plabOriginalSampeRate;
    QSpinBox* m_pispOriginalSampeRate;

    QPushButton* m_pbtnLoadTemp;
    QPushButton* m_pbtnOK;


    QGroupBox* m_pgrpNanoporeSequencing;

    QLabel* m_plabModel;
    QComboBox* m_pcmbModel;

    QLabel* m_plabBasecallModel;
    QComboBox* m_pcmbBasecallModel;

    QLabel* m_plabMinWidth;
    //QLineEdit* m_peditMinWidth;
    QDoubleSpinBox* m_pdspMinWidth;

    QLabel* m_plabWindowWidth;
    //QLineEdit* m_peditWindowWidth;
    QDoubleSpinBox* m_pdspWindowWidth;

    QLabel* m_plabMinGainPerSample;
    //QLineEdit* m_peditMinGainPerSample;
    QDoubleSpinBox* m_pdspMinGainPerSample;

    QLabel* m_plabIsAdaptorThre;
    //QLineEdit* m_peditIsAdaptorThre;
    QDoubleSpinBox* m_pdspIsAdaptorThre;

    QLabel* m_plabSpeed;
    //QLineEdit* m_peditSpeed;
    QDoubleSpinBox* m_pdspSpeed;

    //QTimer m_tmrInitRecordConfig;
    QTimer m_tmrStartAlgorithmServer;


    std::string m_strSavePath;
    std::string m_strTaskName;
    std::string m_strTaskPath;
    std::string m_strSaveJson;
    std::string m_strExperimentConfigs;

    bool m_bStartAlgorithmServer = false;

    QGroupBox* m_pgrpRecordChannel = nullptr;
    QTableWidget *m_pRecordTableWgt = nullptr;
    QCheckBox *m_pAllChChk = nullptr;
    QCheckBox* m_pchkShowVisible = nullptr;
    std::vector<std::shared_ptr<QCheckBox>> m_vetChannelItem;
    std::vector<bool> m_vetChannelSelect;
    QSignalMapper *m_pSignal_mapper = nullptr;

    QString m_strRunId;


private:
    bool InitCtrl(void);
    bool InitItemDefault(void);
    bool InitItemLocal(const std::string& scont);
    bool InitItemOnline(void);
    bool ParseRecordConfig(const std::string& scont, size_t sz = 0);

    const std::string GetTaskName(void) const;
    bool Json4SaveSetting(const std::string& spath, const std::string& sname);
    bool ParseJsonSettingFile(const std::string& jsonFile);

    bool InitItemDefault2(void);
    bool InitItemLocal2(const std::string& scont);
    bool InitItemOnline2(void);
    bool ParseRecordConfig2(const std::string& scont, size_t sz = 0);

    void InitRequestRecordConfig(void);
    void GenerateRunID(void);

public:
    bool CreateTopPath(void);
    std::string CreateDataPath(void);
    void SetSavePath(const std::string& dir);

    void SetSamplingRate(int rate);


//    void SetControlView(ControlView* pview) { m_pControlView = pview; }

    std::vector<bool>& GetChannelSelect(void) { return m_vetChannelSelect; }
    void SetChannelSelect(const std::vector<bool> &vctIsSave);

    bool UpdateEndtime4Json(void);

    bool LoadConfig(QSettings* pset);
    bool SaveConfig(QSettings* pset);

    //realtime sequencing
    bool Json4RegisterTask(std::string& retjson);
    const std::string GetExperimentConfigs(void) const { return m_strExperimentConfigs; }

    bool GetNanoporeSequencingCheck(void) const { return m_pgrpNanoporeSequencing->isChecked(); }
    void SetNanoporeSequencingCheck(bool chk) { m_pgrpNanoporeSequencing->setChecked(chk); }
    void EnableNanoporeSequencingCheck(bool chk) { m_pgrpNanoporeSequencing->setEnabled(chk); }
    QString GetUniqueRunID(void){return m_strRunId;}


private slots:
    void onClickSaveSlot(void);
    void onClickLoadSlot(void);
    void onChangeSequencingIdSlot(int val);

    void setRecordConfigSlot(const QString& qsconf, int sz, bool loc);
    void setRecordConfigSlot2(const QString& qsconf, int sz, bool loc);

    void onClickNanoporeSequencing(bool val);
    //void timerInitRecordConfig(void);
    void timerStartAlgorithmServer(void);

    void OnCheckAllSelect(int stat);
    void UpdateSaveChkState(int ch);
    void OnCheckShowVisible(bool chk);

signals:
    void onSequencingIdChangedSignal(int val);

    void setRecordConfigSignal(const QString& qsconf, int sz, bool loc);
    void setRecordConfigSignal2(const QString& qsconf, int sz, bool loc);

    void onEnableStartSaveDataSignal(bool val);
    void checkAlgorithmServerStatusCodeSignal(void);
};

