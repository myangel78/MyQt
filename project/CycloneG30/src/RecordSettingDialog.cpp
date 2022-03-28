#include "RecordSettingDialog.h"

#include <qlayout.h>
#include <qfiledialog.h>
#include <qdatetime.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qthread.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <Log.h>
#include <QTableWidget>
#include <QSignalMapper>
#include <QHeaderView>
#include <QDebug>
#include <QUuid>

#include "ConfigServer.h"
#include "HttpClient.h"
#include "mainversion.h"


RecordSettingDialog::RecordSettingDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Data Record Config Setting"));
    InitCtrl();

    connect(this, &RecordSettingDialog::setRecordConfigSignal, this, &RecordSettingDialog::setRecordConfigSlot);
    connect(this, &RecordSettingDialog::setRecordConfigSignal2, this, &RecordSettingDialog::setRecordConfigSlot2);

    //m_tmrInitRecordConfig.setSingleShot(true);
    //connect(&m_tmrInitRecordConfig, &QTimer::timeout, this, &RecordSettingDialog::timerInitRecordConfig);
    m_tmrStartAlgorithmServer.setSingleShot(true);
    connect(&m_tmrStartAlgorithmServer, &QTimer::timeout, this, &RecordSettingDialog::timerStartAlgorithmServer);
    //m_tmrInitRecordConfig.start(1000);

#if 0

    auto qfunc = std::function<void(void)>([&](void)
        {
            InitRequestRecordConfig();
        });
    QThread* qthr = QThread::create(qfunc);
    qthr->start();

#else

    std::thread thrd([this]()
        {
            InitRequestRecordConfig();
        });
    thrd.detach();

#endif

}

RecordSettingDialog::~RecordSettingDialog()
{

}

bool RecordSettingDialog::InitCtrl(void)
{
	m_pgrpDataRecord = new QGroupBox("Experiment Configs", this);

	m_plabTemperature = new QLabel("Temperature", m_pgrpDataRecord);
	m_pdspTemperature = new QDoubleSpinBox(m_pgrpDataRecord);
	m_pdspTemperature->setDecimals(1);
	m_pdspTemperature->setSuffix(QStringLiteral(" ℃"));//176
	m_pdspTemperature->setMinimum(10.0);
	m_pdspTemperature->setMaximum(50.0);

	m_plabStartTime = new QLabel("StartTime", m_pgrpDataRecord);
	m_peditStartTime = new QLineEdit(m_pgrpDataRecord);
	m_peditStartTime->setReadOnly(true);

	m_plabEndTime = new QLabel("EndTime", m_pgrpDataRecord);
	m_peditEndTime = new QLineEdit(m_pgrpDataRecord);
	m_peditEndTime->setReadOnly(true);

	m_plabSequencerVersion = new QLabel("SequencerVersion", m_pgrpDataRecord);
	m_pcmbSequencerVersion = new QComboBox(m_pgrpDataRecord);

	m_plabSequencerID = new QLabel("SequencerID", m_pgrpDataRecord);
	m_pcmbSequencerID = new QComboBox(m_pgrpDataRecord);

	m_plabOperator = new QLabel("Operator", m_pgrpDataRecord);
	m_pcmbOperator = new QComboBox(m_pgrpDataRecord);

	m_plabFolderPath = new QLabel("FolderPath", m_pgrpDataRecord);
	m_peditFolderPath = new QLineEdit(m_pgrpDataRecord);
	m_peditFolderPath->setReadOnly(true);

	m_plabPoreID = new QLabel("PoreID", m_pgrpDataRecord);
	m_pcmbPoreID = new QComboBox(m_pgrpDataRecord);
	m_peditPoreID = new QLineEdit(m_pgrpDataRecord);
	QRegExpValidator* pPoreID = new QRegExpValidator(QRegExp("^[^\\s]*$"));//filter space
	m_peditPoreID->setValidator(pPoreID);

	m_plabPoreBatch = new QLabel("PoreBatch", m_pgrpDataRecord);
	m_pdatePoreBatch = new QDateEdit(m_pgrpDataRecord);
	m_pdatePoreBatch->setDisplayFormat("yyyyMMdd");
	m_pdatePoreBatch->setCalendarPopup(true);
	m_pispPoreBatch = new QSpinBox(m_pgrpDataRecord);
	m_pispPoreBatch->setMaximum(999999);

	m_plabMotorID = new QLabel("MotorID", m_pgrpDataRecord);
	m_pcmbMotorID = new QComboBox(m_pgrpDataRecord);
	m_peditMotorID = new QLineEdit(m_pgrpDataRecord);
	QRegExpValidator* pMotorID = new QRegExpValidator(QRegExp("^[^\\s]*$"));//filter space
	m_peditMotorID->setValidator(pMotorID);

	m_plabSeqBuffer = new QLabel("SeqBuffer", m_pgrpDataRecord);
	m_pcmbSeqBuffer = new QComboBox(m_pgrpDataRecord);
	m_pispSeqBuffer = new QSpinBox(m_pgrpDataRecord);

	m_plabElectrochemBufferID = new QLabel("ElectrochemBufferID", m_pgrpDataRecord);
	m_pcmbElectrochemBufferID = new QComboBox(m_pgrpDataRecord);

	m_plabNotes = new QLabel("Notes", m_pgrpDataRecord);
	m_peditNotes = new QLineEdit(m_pgrpDataRecord);

	m_plabSeqVoltage = new QLabel("SeqVoltage", m_pgrpDataRecord);
	m_pcmbSeqVoltage = new QComboBox(m_pgrpDataRecord);
	m_pdspSeqVoltage = new QDoubleSpinBox(m_pgrpDataRecord);
	m_pdspSeqVoltage->setDecimals(2);
	m_pdspSeqVoltage->setSuffix("V");

	m_plabMembrane = new QLabel("Membrane", m_pgrpDataRecord);
	m_pcmbMembrane = new QComboBox(m_pgrpDataRecord);
	m_peditMembrane = new QLineEdit(m_pgrpDataRecord);

	m_plabChipID = new QLabel("ChipID", m_pgrpDataRecord);
	m_pcmbChipID = new QComboBox(m_pgrpDataRecord);
	m_peditChipID = new QLineEdit(m_pgrpDataRecord);
	QRegExpValidator* pChipIDReg = new QRegExpValidator(QRegExp("\\d+(\\.\\d+){0,1}"));
	m_peditChipID->setValidator(pChipIDReg);
	m_plabChipID2 = new QLabel("No.", m_pgrpDataRecord);
	m_pispChipID2 = new QSpinBox(m_pgrpDataRecord);
	m_pispChipID2->setMaximum(999999);

	m_plabFlowcellID = new QLabel("FlowcellID", m_pgrpDataRecord);
	m_pcmbFlowcellID = new QComboBox(m_pgrpDataRecord);
	m_peditFlowcellID = new QLineEdit(m_pgrpDataRecord);
	QRegExpValidator* pFlowcellIDReg = new QRegExpValidator(QRegExp("\\d+(\\.\\d+){0,1}"));
	m_peditFlowcellID->setValidator(pFlowcellIDReg);
	m_plabFlowcellID2 = new QLabel("No.", m_pgrpDataRecord);
	m_pispFlowcellID2 = new QSpinBox(m_pgrpDataRecord);
	m_pispFlowcellID2->setMaximum(999999);

	m_plabLibraryAdaptor = new QLabel("LibraryAdaptor", m_pgrpDataRecord);
	m_pcmbLibraryAdaptor = new QComboBox(m_pgrpDataRecord);
	m_pdateLibraryAdaptor = new QDateEdit(m_pgrpDataRecord);
	m_pdateLibraryAdaptor->setDisplayFormat("yyyyMMdd");
	m_pdateLibraryAdaptor->setCalendarPopup(true);

	m_plabASIC = new QLabel("ASIC", m_pgrpDataRecord);
	m_pcmbASIC = new QComboBox(m_pgrpDataRecord);
	m_peditASIC = new QLineEdit(m_pgrpDataRecord);
	QRegExpValidator* pASICReg = new QRegExpValidator(QRegExp("^[0-9][0-9]?$"));
	m_peditASIC->setValidator(pASICReg);

	m_plabLibraryDNA = new QLabel("LibraryDNA", m_pgrpDataRecord);
	m_pcmbLibraryDNA = new QComboBox(m_pgrpDataRecord);

	m_plabOriginalSampeRate = new QLabel("OriginalSampeRate", m_pgrpDataRecord);
	m_pispOriginalSampeRate = new QSpinBox(m_pgrpDataRecord);
	m_pispOriginalSampeRate->setSuffix(" KHz");
	m_pispOriginalSampeRate->setValue(DEFAULT_SAMPLE_RATE / 1000);
	//m_pispOriginalSampeRate->setEnabled(false);
	m_pispOriginalSampeRate->setReadOnly(true);


	m_pbtnLoadTemp = new QPushButton("Load...", m_pgrpDataRecord);
	m_pbtnLoadTemp->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>加载之前保存的参数配置</p></body></html>"));
	m_pbtnOK = new QPushButton("Save", m_pgrpDataRecord);
	m_pbtnOK->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>保存当前的参数配置到./etc/Data目录下</p></body></html>"));


	QGridLayout* gdlayout1 = new QGridLayout();
	int row = 0;
	int col = 0;
	gdlayout1->addWidget(m_plabTemperature, row, col++);
	gdlayout1->addWidget(m_pdspTemperature, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabStartTime, row, col++);
	gdlayout1->addWidget(m_peditStartTime, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabEndTime, row, col++);
	gdlayout1->addWidget(m_peditEndTime, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabSequencerVersion, row, col++);
	gdlayout1->addWidget(m_pcmbSequencerVersion, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabSequencerID, row, col++);
	gdlayout1->addWidget(m_pcmbSequencerID, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabOperator, row, col++);
	gdlayout1->addWidget(m_pcmbOperator, row, col++);

	//++row;
	//col = 0;
	//gdlayout1->addWidget(m_plabFolderPath, row, col++);
	//gdlayout1->addWidget(m_peditFolderPath, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabPoreID, row, col++);
	gdlayout1->addWidget(m_pcmbPoreID, row, col++);
	gdlayout1->addWidget(m_peditPoreID, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabPoreBatch, row, col++);
	//gdlayout1->addWidget(m_peditPoreBatch, row, col++);
	gdlayout1->addWidget(m_pdatePoreBatch, row, col++);
	gdlayout1->addWidget(m_pispPoreBatch, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabMotorID, row, col++);
	gdlayout1->addWidget(m_pcmbMotorID, row, col++);
	gdlayout1->addWidget(m_peditMotorID, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabSeqBuffer, row, col++);
	gdlayout1->addWidget(m_pcmbSeqBuffer, row, col++);
	gdlayout1->addWidget(m_pispSeqBuffer, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabElectrochemBufferID, row, col++);
	gdlayout1->addWidget(m_pcmbElectrochemBufferID, row, col++);

	//++row;
	//col = 0;
	//gdlayout1->addWidget(m_plabNotes, row, col++);
	//gdlayout1->addWidget(m_peditNotes, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabSeqVoltage, row, col++);
	gdlayout1->addWidget(m_pcmbSeqVoltage, row, col++);
	gdlayout1->addWidget(m_pdspSeqVoltage, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabMembrane, row, col++);
	gdlayout1->addWidget(m_pcmbMembrane, row, col++);
	gdlayout1->addWidget(m_peditMembrane, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabLibraryAdaptor, row, col++);
	gdlayout1->addWidget(m_pcmbLibraryAdaptor, row, col++);
	gdlayout1->addWidget(m_pdateLibraryAdaptor, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabASIC, row, col++);
	gdlayout1->addWidget(m_pcmbASIC, row, col++);
	gdlayout1->addWidget(m_peditASIC, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabLibraryDNA, row, col++);
	gdlayout1->addWidget(m_pcmbLibraryDNA, row, col++);

	++row;
	col = 0;
	gdlayout1->addWidget(m_plabOriginalSampeRate, row, col++);
	gdlayout1->addWidget(m_pispOriginalSampeRate, row, col++);


	QGridLayout* gdlayout2 = new QGridLayout();
	row = 0;
	col = 0;
	gdlayout2->addWidget(m_plabChipID, row, col++);
	gdlayout2->addWidget(m_pcmbChipID, row, col++);
	gdlayout2->addWidget(m_peditChipID, row, col++);
	gdlayout2->addWidget(m_plabChipID2, row, col++);
	gdlayout2->addWidget(m_pispChipID2, row, col++);

	++row;
	col = 0;
	gdlayout2->addWidget(m_plabFlowcellID, row, col++);
	gdlayout2->addWidget(m_pcmbFlowcellID, row, col++);
	gdlayout2->addWidget(m_peditFlowcellID, row, col++);
	gdlayout2->addWidget(m_plabFlowcellID2, row, col++);
	gdlayout2->addWidget(m_pispFlowcellID2, row, col++);


	QGridLayout* gdlayout3 = new QGridLayout();
	row = 0;
	col = 0;
	gdlayout3->addWidget(m_plabFolderPath, row, col++);
	gdlayout3->addWidget(m_peditFolderPath, row, col++);

	++row;
	col = 0;
	gdlayout3->addWidget(m_plabNotes, row, col++);
	gdlayout3->addWidget(m_peditNotes, row, col++);


	QHBoxLayout* horlayout3 = new QHBoxLayout();
	horlayout3->addStretch(1);
	horlayout3->addWidget(m_pbtnLoadTemp);
	horlayout3->addStretch(1);
	horlayout3->addWidget(m_pbtnOK);
	horlayout3->addStretch(5);

	QVBoxLayout* verlayout1 = new QVBoxLayout();
	verlayout1->addLayout(gdlayout1);
	verlayout1->addLayout(gdlayout2);
	verlayout1->addLayout(gdlayout3);
	verlayout1->addLayout(horlayout3);

	m_pgrpDataRecord->setLayout(verlayout1);


	m_pgrpNanoporeSequencing = new QGroupBox("Enable Realtime Sequencing Analysis (Model Configs)", this);
	m_pgrpNanoporeSequencing->setCheckable(true);
	m_pgrpNanoporeSequencing->setChecked(false);
#ifdef Q_OS_WIN32
	m_pgrpNanoporeSequencing->setEnabled(false);
#else
#endif

	m_plabModel = new QLabel("Model", m_pgrpNanoporeSequencing);
	m_pcmbModel = new QComboBox(m_pgrpNanoporeSequencing);

	m_plabBasecallModel = new QLabel("BasecallModel", m_pgrpNanoporeSequencing);
	m_pcmbBasecallModel = new QComboBox(m_pgrpNanoporeSequencing);

	m_plabMinWidth = new QLabel("MinWidth", m_pgrpNanoporeSequencing);
	//m_peditMinWidth = new QLineEdit(m_pgrpNanoporeSequencing);
	m_pdspMinWidth = new QDoubleSpinBox(m_pgrpNanoporeSequencing);
	m_pdspMinWidth->setMaximum(99999.99);

	m_plabWindowWidth = new QLabel("WindowWidth", m_pgrpNanoporeSequencing);
	//m_peditWindowWidth = new QLineEdit(m_pgrpNanoporeSequencing);
	m_pdspWindowWidth = new QDoubleSpinBox(m_pgrpNanoporeSequencing);
	m_pdspWindowWidth->setMaximum(99999.99);

	m_plabMinGainPerSample = new QLabel("MinGainPerSample", m_pgrpNanoporeSequencing);
	//m_peditMinGainPerSample = new QLineEdit(m_pgrpNanoporeSequencing);
	m_pdspMinGainPerSample = new QDoubleSpinBox(m_pgrpNanoporeSequencing);
	m_pdspMinGainPerSample->setMaximum(99999.99);

	m_plabIsAdaptorThre = new QLabel("IsAdaptorThre", m_pgrpNanoporeSequencing);
	//m_peditIsAdaptorThre = new QLineEdit(m_pgrpNanoporeSequencing);
	m_pdspIsAdaptorThre = new QDoubleSpinBox(m_pgrpNanoporeSequencing);
	m_pdspIsAdaptorThre->setMaximum(99999.99);

	m_plabSpeed = new QLabel("Speed", m_pgrpNanoporeSequencing);
	//m_peditSpeed = new QLineEdit(m_pgrpNanoporeSequencing);
	m_pdspSpeed = new QDoubleSpinBox(m_pgrpNanoporeSequencing);
	m_pdspSpeed->setMaximum(99999.99);

	row = 0;
	col = 0;
	QGridLayout* gdlayout6 = new QGridLayout();
	gdlayout6->addWidget(m_plabModel, row, col++);
	gdlayout6->addWidget(m_pcmbModel, row, col++);

	++row;
	col = 0;
	gdlayout6->addWidget(m_plabBasecallModel, row, col++);
	gdlayout6->addWidget(m_pcmbBasecallModel, row, col++);

	++row;
	col = 0;
	gdlayout6->addWidget(m_plabMinWidth, row, col++);
	gdlayout6->addWidget(m_pdspMinWidth, row, col++);

	++row;
	col = 0;
	gdlayout6->addWidget(m_plabWindowWidth, row, col++);
	gdlayout6->addWidget(m_pdspWindowWidth, row, col++);

	++row;
	col = 0;
	gdlayout6->addWidget(m_plabMinGainPerSample, row, col++);
	gdlayout6->addWidget(m_pdspMinGainPerSample, row, col++);

	++row;
	col = 0;
	gdlayout6->addWidget(m_plabIsAdaptorThre, row, col++);
	gdlayout6->addWidget(m_pdspIsAdaptorThre, row, col++);

	++row;
	col = 0;
	gdlayout6->addWidget(m_plabSpeed, row, col++);
	gdlayout6->addWidget(m_pdspSpeed, row, col++);

	m_pgrpNanoporeSequencing->setLayout(gdlayout6);


	m_pgrpRecordChannel = new QGroupBox("Record Channel", this);
    //record channel table
    m_pRecordTableWgt = new QTableWidget(this);
    m_pRecordTableWgt->setRowCount(CHANNEL_NUM);
    m_pRecordTableWgt->setColumnCount(2);
    m_pSignal_mapper = new QSignalMapper(this);

    QStringList recordHeader;
    recordHeader << "Channel" << "Save";
    m_pRecordTableWgt->setHorizontalHeaderLabels(recordHeader);
    m_pRecordTableWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);                                  //列宽自动平分
    m_pRecordTableWgt->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);                      //根据列内容自动调整列宽
    m_pRecordTableWgt->horizontalHeader()->setHighlightSections(false);                                                 //设置后点击某行表头颜色、字体就不会发生改变
    m_pRecordTableWgt->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(156,156,156);color: white;}");
    m_pRecordTableWgt->setSelectionBehavior(QAbstractItemView::SelectRows);                                             //设置点击选中为一行
    m_pRecordTableWgt->setSelectionMode(QAbstractItemView::SingleSelection);                                            //只能选择一行
    m_pRecordTableWgt->setAlternatingRowColors(true);
    m_pRecordTableWgt->verticalHeader()->setHidden(true);

    QString checkQss =   "QCheckBox::indicator { width: 20px;height: 20px;}"
                         "QCheckBox::indicator::unchecked {image: url(:/img/img/checkun.png);}"
                         "QCheckBox::indicator::checked {image: url(:/img/img/check.png);}";

    m_vetChannelItem.resize(CHANNEL_NUM);
    m_vetChannelSelect.resize(CHANNEL_NUM);
    for (int ch = 0; ch < CHANNEL_NUM; ++ch)
    {
        m_pRecordTableWgt->setItem(ch,0,new QTableWidgetItem(QString("CH %1").arg(ch +1)));
        m_vetChannelItem[ch] = std::make_shared<QCheckBox>(QStringLiteral("Yes"), m_pRecordTableWgt);
        m_vetChannelItem[ch]->setChecked(true);
        m_vetChannelItem[ch]->setStyleSheet(checkQss);
        m_pRecordTableWgt->setCellWidget(ch,1, m_vetChannelItem[ch].get());
        m_vetChannelSelect[ch] = true;
        m_pSignal_mapper->setMapping(m_vetChannelItem[ch].get(),ch);
        connect(m_vetChannelItem[ch].get(),SIGNAL(stateChanged(int)),m_pSignal_mapper,SLOT(map()));
    }

    m_pAllChChk = new QCheckBox(QStringLiteral("All Save"),this);
    m_pchkShowVisible = new QCheckBox(QStringLiteral("ShowVisible"),this);
    m_pchkShowVisible->setToolTip(QString::fromLocal8Bit("<html><head/><body><p>只显示勾选通道;</p></body></html>"));
    m_pAllChChk->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_pAllChChk->setChecked(true);

    QHBoxLayout *btnHorlayout = new QHBoxLayout();
    btnHorlayout->addWidget(m_pAllChChk);
    btnHorlayout->addWidget(m_pchkShowVisible);
    btnHorlayout->addStretch();

    QVBoxLayout *vertlaout = new QVBoxLayout();
    vertlaout->addWidget(m_pRecordTableWgt);
    vertlaout->addLayout(btnHorlayout);

    m_pgrpRecordChannel->setLayout(vertlaout);
    m_pgrpRecordChannel->setMaximumWidth(160);


	QVBoxLayout* verlayout9 = new QVBoxLayout();
	verlayout9->addWidget(m_pgrpDataRecord);
	verlayout9->addWidget(m_pgrpNanoporeSequencing);

	QHBoxLayout* horlayout9 = new QHBoxLayout();
	horlayout9->addLayout(verlayout9);
	horlayout9->addWidget(m_pgrpRecordChannel);

	setLayout(horlayout9);


    connect(m_pbtnLoadTemp, &QPushButton::clicked, this, &RecordSettingDialog::onClickLoadSlot);
    connect(m_pbtnOK, &QPushButton::clicked, this, &RecordSettingDialog::onClickSaveSlot);
    connect(m_pgrpNanoporeSequencing, &QGroupBox::clicked, this, &RecordSettingDialog::onClickNanoporeSequencing);
    connect(m_pAllChChk, &QCheckBox::stateChanged, this, &RecordSettingDialog::OnCheckAllSelect);
    connect(m_pSignal_mapper,SIGNAL(mapped(int)),this,SLOT(UpdateSaveChkState(int)));
    connect(m_pchkShowVisible, &QCheckBox::stateChanged, this, &RecordSettingDialog::OnCheckShowVisible);

//    connect(m_pbtnSynchronize, &QPushButton::clicked, this, &RecordSettingDialog::onClickSynchronize);



	return false;
}

bool RecordSettingDialog::InitItemDefault(void)
{
    std::string strjson = ConfigServer::GetCurrentPath() + "/etc/Conf/experiment_configs.json";
    if (!std::filesystem::exists(strjson))
    {
        return false;
    }

    std::ifstream ifs(strjson);
    std::stringstream sst;
    sst << ifs.rdbuf();

    //return ParseRecordConfig(sst.str());
    std::string str = sst.str();
    emit setRecordConfigSignal(QString::fromStdString(str), str.size(), false);

    return true;
}

bool RecordSettingDialog::InitItemLocal(const std::string& scont)
{
    std::string strjson = ConfigServer::GetCurrentPath() + "/etc/Conf/experiment_configs.json";
    FILE* pf = fopen(strjson.c_str(), "w");
    if (pf == nullptr)
    {
        return false;
    }
    auto ret = fwrite(scont.c_str(), 1, scont.size(), pf);
    fclose(pf);
    return ret != 0;
}


bool RecordSettingDialog::InitItemOnline(void)
{
    HttpClient httpcl;
    httpcl.SetIngorSSL(true);
    auto strUrl = ConfigServer::GetInstance()->GetRecordConfig();
    //if (strUrl.size() <= 0)
    {
        if (ConfigServer::GetInstance()->GetWorknet() == 1)
        {
            strUrl = "http://172.16.18.2:9085/v1/experiment_configs/latest";//"http://172.16.16.44:9085/v1/experiment_configs/latest"
        }
        else
        {
            strUrl = "http://192.168.0.77:9106/cycloneconfig/v1/experiment_configs/latest/";//"http://192.168.0.106:9085/v1/experiment_configs/latest";
        }
        ConfigServer::GetInstance()->SetRecordConfig(strUrl);
    }
    bool ret = httpcl.RequestGet(strUrl);
    if (!ret)
    {
        return false;
    }

    size_t retsz = 0;
    auto retstr = httpcl.GetReadString(retsz);
    emit setRecordConfigSignal(QString::fromStdString(retstr), retsz, true);
    //ret = ParseRecordConfig(retstr, retsz);
    //if (ret)
    //{
    //	if (!InitItemLocal(retstr))
    //	{
    //		LOGW("RecordSettingLocal FAILED!!!");
    //	}
    //}
    //else
    //{
    //	LOGW("RecordSettingOnline FAILED msg={}", retstr.c_str());
    //}

    return ret;
}

bool RecordSettingDialog::ParseRecordConfig(const std::string& scont, size_t sz)
{
    if (sz <= 0)
    {
        sz = scont.size();
    }
    if (sz <= 0)
    {
        return false;
    }


    QJsonParseError jsonError;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(scont.c_str(), &jsonError));

    if (jsonError.error != QJsonParseError::NoError)
    {
        LOGE("Wrong record config json content!!!={}", scont.c_str());
        return false;
    }

    if (jsonDocument.isObject())
    {
        QJsonObject jsonObject = jsonDocument.object();
        auto ite = jsonObject.find("status");
        if (ite != jsonObject.end())
        {
            auto state = ite->toString();
            if (state != "done")
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        ite = jsonObject.find("data");
        if (ite == jsonObject.end())
        {
            return false;
        }


        auto dataobj = ite->toObject();
        ite = dataobj.find("sequencer_version");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbSequencerVersion->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbSequencerVersion->addItem(val.toString());
                }
            }
        }

        ite = dataobj.find("sequencer_id");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbSequencerID->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbSequencerID->addItem(QString::number(val.toInt()));
                }
            }
        }
        ite = dataobj.find("operator");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbOperator->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbOperator->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("pore_id");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbPoreID->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbPoreID->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("motor_id");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbMotorID->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbMotorID->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("seq_buffer");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbSeqBuffer->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbSeqBuffer->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("electrochem_buffer_id");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbElectrochemBufferID->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbElectrochemBufferID->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("seq_voltage");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbSeqVoltage->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbSeqVoltage->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("membrane");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbMembrane->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbMembrane->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("chip_id");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbChipID->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbChipID->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("flowcell_id");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbFlowcellID->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbFlowcellID->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("library_adaptor");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbLibraryAdaptor->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbLibraryAdaptor->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("asic");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbASIC->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbASIC->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("library_dna");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                m_pcmbLibraryDNA->clear();
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbLibraryDNA->addItem(val.toString());
                }
            }
        }

    }

    return true;
}


const std::string RecordSettingDialog::GetTaskName(void) const
{
    std::string strDatetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toStdString();
    std::string strDataType = m_pcmbSequencerVersion->currentText().toStdString();
    std::string strPorinID = m_pcmbPoreID->currentText().toStdString();
    if (strPorinID.empty() || strPorinID == "Other")
    {
        strPorinID = m_peditPoreID->text().toStdString();
    }
    int temperature = m_pdspTemperature->value();
    std::string strSeqBuf = (m_pcmbSeqBuffer->currentText() + m_pispSeqBuffer->text()).toStdString();
    std::string strMotorID = m_pcmbMotorID->currentText().toStdString();
    if (strMotorID.empty() || strMotorID == "Other")
    {
        strMotorID = m_peditMotorID->text().toStdString();
    }
    std::string strLibAdptor = m_pcmbLibraryAdaptor->currentText().toStdString();
    std::string strLibDNA = m_pcmbLibraryDNA->currentText().toStdString();
    //std::string strSeqID = m_pcmbSequencerID->currentText().toStdString();
    std::string strOperator = m_pcmbOperator->currentText().toStdString();
    int samplerate = m_pispOriginalSampeRate->value();

    //return strDatetime + "_" + strDataType + "_" + strPorinID + "_" + ConfigServer::tostring(temperature, 1) + "_" + strSeqBuf + "_" + strMotorID + "_" + strLibAdptor + "_" + strLibDNA + "_" + std::to_string(samplerate) + "K_" + strSeqID + "_" + strOperator;

    std::string strTaskName = strDatetime + "_" + strDataType + "_" + std::to_string(samplerate) + "K_" + strPorinID + "_" + std::to_string(temperature) + "_" + strSeqBuf + "_" + strMotorID + "_" + strLibAdptor + "_" + strLibDNA + "_" + strOperator;
    std::regex regrep("\\s+");//filter space
    return std::regex_replace(strTaskName, regrep, "");
}

bool RecordSettingDialog::Json4SaveSetting(const std::string& spath, const std::string& sname)
{
    char arJsonName[1024] = { 0 };
    sprintf(arJsonName, "%s/%s.json", spath.c_str(), sname.c_str());
    QFile jsonfile(arJsonName);
    if (!jsonfile.open(QIODevice::WriteOnly))
    {
        LOGE("Save json setting FAILED!!!={}", arJsonName);
        return false;
    }
    m_strSaveJson = arJsonName;

    QString startTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_peditStartTime->setText(startTime);
    m_peditEndTime->setText(startTime);

    m_peditFolderPath->setText(sname.c_str());

    QJsonObject jsonObject;
    jsonObject.insert("Temperature", m_pdspTemperature->text());//��
    jsonObject.insert("StartTime", m_peditStartTime->text());//
    jsonObject.insert("EndTime", m_peditEndTime->text());//
    jsonObject.insert("SequencerVersion", m_pcmbSequencerVersion->currentText());//LAB256V1
    jsonObject.insert("SequencerID", m_pcmbSequencerID->currentText());//1-10
    jsonObject.insert("Operator", m_pcmbOperator->currentText());//
    jsonObject.insert("FolderPath", m_peditFolderPath->text());//

    QString porinID = m_pcmbPoreID->currentText();
    if (porinID.isEmpty() || porinID == "Other")
    {
        porinID = m_peditPoreID->text();
    }
    jsonObject.insert("PoreID", porinID);//PC28/Other

    QString motorID = m_pcmbMotorID->currentText();
    if (motorID.isEmpty() || motorID == "Other")
    {
        motorID = m_peditMotorID->text();
    }
    jsonObject.insert("MotorID", motorID);//HD4/HD42/HD23/HD8/Other

    jsonObject.insert("PoreBatch", m_pdatePoreBatch->text());//202003
    jsonObject.insert("PoreBatchMore", m_pispPoreBatch->text());//2023
    jsonObject.insert("SeqBuffer", m_pcmbSeqBuffer->currentText() + m_pispSeqBuffer->text());//B/Z 01-99
    jsonObject.insert("ElectrochemBufferID", m_pcmbElectrochemBufferID->currentText());//EB1
    jsonObject.insert("Notes", m_peditNotes->text());//Notes

    QString seqVoltage = m_pcmbSeqVoltage->currentText();//0.18V/Other
    if (seqVoltage.isEmpty() || seqVoltage == "Other")
    {
        seqVoltage = m_pdspSeqVoltage->text();
    }
    jsonObject.insert("SeqVoltage", seqVoltage);

    jsonObject.insert("Membrane", m_pcmbMembrane->currentText());//Lipid Polymer
    jsonObject.insert("MembraneMore", m_peditMembrane->text());//more comment

    jsonObject.insert("ChipID", m_pcmbChipID->currentText() + "_V" + m_peditChipID->text() + "_No." + m_pispChipID2->text());//32CH_VX.X_No.X
    jsonObject.insert("FlowcellID", m_pcmbFlowcellID->currentText() + "_V" + m_peditFlowcellID->text() + "_No." + m_pispFlowcellID2->text());//256CH_VX.X_No.X

    jsonObject.insert("LibraryAdaptor", m_pcmbLibraryAdaptor->currentText());//AD1
    jsonObject.insert("LibraryAdaptorMore", m_pdateLibraryAdaptor->text());//more comment

    jsonObject.insert("ASIC", m_pcmbASIC->currentText() + "_U" + m_peditASIC->text());//WUTONG_U01

    jsonObject.insert("LibraryDNA", m_pcmbLibraryDNA->currentText());//Ecoli

    jsonObject.insert("OriginalSampeRate", m_pispOriginalSampeRate->text());//20 KHz

    QString hardver = "V1_";
    jsonObject.insert("Version", hardver + SOFTWARE_VERSION);//V2_V1.9.2.1
#ifdef Q_OS_WIN32
    QString osplat = "Windows";
#else
    QString osplat = "Linux";
#endif
    jsonObject.insert("OSPlatform", osplat);//V2_V1.9.2.1

    if(ConfigServer::GetInstance()->GetNeedGenerateRunId())
    {
        GenerateRunID();
    }
    jsonObject.insert("run_id", m_strRunId);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);

    m_strExperimentConfigs = jsonDoc.toJson().toStdString();

    jsonfile.write(jsonDoc.toJson());
    jsonfile.close();

    return true;
}

bool RecordSettingDialog::ParseJsonSettingFile(const std::string& jsonFile)
{
    if (!std::filesystem::exists(jsonFile))
    {
        return false;
    }
    std::ifstream ifs(jsonFile);
    std::stringstream sst;
    sst << ifs.rdbuf();

    QJsonParseError jsonError;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(sst.str().c_str(), &jsonError));

    if (jsonError.error != QJsonParseError::NoError)
    {
        LOGE("Wrong json content!!!={}", sst.str().c_str());
        return false;
    }

    if (jsonDocument.isObject())
    {
        QJsonObject jsonObject = jsonDocument.object();
        auto ite = jsonObject.find("ASIC");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            int pos = txt.indexOf("_U");
            if (pos == -1)
            {
                return false;
            }
            auto pre = txt.left(pos);
            for (int i = 0, n = m_pcmbASIC->count(); i < n; ++i)
            {
                if (pre == m_pcmbASIC->itemText(i))
                {
                    m_pcmbASIC->setCurrentIndex(i);
                    break;
                }
            }
            auto suf = txt.right(txt.size() - pos - 2);
            m_peditASIC->setText(suf);
        }

        ite = jsonObject.find("ChipID");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            int pos = txt.indexOf("_V");
            if (pos == -1)
            {
                return false;
            }
            auto pre = txt.left(pos);
            for (int i = 0, n = m_pcmbChipID->count(); i < n; ++i)
            {
                if (pre == m_pcmbChipID->itemText(i))
                {
                    m_pcmbChipID->setCurrentIndex(i);
                    break;
                }
            }
            int pos2 = txt.indexOf("_No.");
            if (pos2 == -1)
            {
                return false;
            }
            auto mid = txt.mid(pos + 2, pos2 - pos - 2);
            m_peditChipID->setText(mid);
            auto suf = txt.right(txt.size() - pos2 - 4);
            m_pispChipID2->setValue(suf.toInt());
        }

        ite = jsonObject.find("FlowcellID");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            int pos = txt.indexOf("_V");
            if (pos == -1)
            {
                return false;
            }
            auto pre = txt.left(pos);
            for (int i = 0, n = m_pcmbFlowcellID->count(); i < n; ++i)
            {
                if (pre == m_pcmbFlowcellID->itemText(i))
                {
                    m_pcmbFlowcellID->setCurrentIndex(i);
                    break;
                }
            }
            int pos2 = txt.indexOf("_No.");
            if (pos2 == -1)
            {
                return false;
            }
            auto mid = txt.mid(pos + 2, pos2 - pos - 2);
            m_peditFlowcellID->setText(mid);
            auto suf = txt.right(txt.size() - pos2 - 4);
            m_pispFlowcellID2->setValue(suf.toInt());
        }

        ite = jsonObject.find("ElectrochemBufferID");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            for (int i = 0, n = m_pcmbElectrochemBufferID->count(); i < n; ++i)
            {
                if (txt == m_pcmbElectrochemBufferID->itemText(i))
                {
                    m_pcmbElectrochemBufferID->setCurrentIndex(i);
                    break;
                }
            }
        }

        ite = jsonObject.find("LibraryAdaptor");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            for (int i = 0, n = m_pcmbLibraryAdaptor->count(); i < n; ++i)
            {
                if (txt == m_pcmbLibraryAdaptor->itemText(i))
                {
                    m_pcmbLibraryAdaptor->setCurrentIndex(i);
                    break;
                }
            }
        }

        ite = jsonObject.find("LibraryAdaptorMore");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            if (txt.size() >= 8)
            {
                int year = txt.left(4).toInt();
                int month = txt.mid(4, 2).toInt();
                int day = txt.right(2).toInt();
                m_pdateLibraryAdaptor->setDate(QDate(year, month, day));
            }
        }

        ite = jsonObject.find("LibraryDNA");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            for (int i = 0, n = m_pcmbLibraryDNA->count(); i < n; ++i)
            {
                if (txt == m_pcmbLibraryDNA->itemText(i))
                {
                    m_pcmbLibraryDNA->setCurrentIndex(i);
                    break;
                }
            }
        }

        ite = jsonObject.find("Membrane");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            for (int i = 0, n = m_pcmbMembrane->count(); i < n; ++i)
            {
                if (txt == m_pcmbMembrane->itemText(i))
                {
                    m_pcmbMembrane->setCurrentIndex(i);
                    break;
                }
            }
        }

        ite = jsonObject.find("MembraneMore");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            m_peditMembrane->setText(txt);
        }

        ite = jsonObject.find("Notes");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            m_peditNotes->setText(txt);
        }

        ite = jsonObject.find("Operator");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            for (int i = 0, n = m_pcmbOperator->count(); i < n; ++i)
            {
                if (txt == m_pcmbOperator->itemText(i))
                {
                    m_pcmbOperator->setCurrentIndex(i);
                    break;
                }
            }
        }

        ite = jsonObject.find("OriginalSampeRate");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            int pos = txt.indexOf(" ");
            if (pos == -1)
            {
                return false;
            }
            m_pispOriginalSampeRate->setValue(txt.left(pos).toInt());
        }

        ite = jsonObject.find("PoreBatch");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            if (txt.size() >= 8)
            {
                int year = txt.left(4).toInt();
                int month = txt.mid(4, 2).toInt();
                int day = txt.right(2).toInt();
                m_pdatePoreBatch->setDate(QDate(year, month, day));
            }
        }

        ite = jsonObject.find("PoreBatchMore");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            m_pispPoreBatch->setValue(txt.toInt());
        }

        ite = jsonObject.find("MotorID");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            bool bfd = false;
            for (int i = 0, n = m_pcmbMotorID->count() - 1; i < n; ++i)
            {
                if (txt == m_pcmbMotorID->itemText(i))
                {
                    m_pcmbMotorID->setCurrentIndex(i);
                    bfd = true;
                    break;
                }
            }
            if (!bfd)
            {
                m_pcmbMotorID->setCurrentIndex(m_pcmbMotorID->count() - 1);
                m_peditMotorID->setText(txt);
            }
        }

        ite = jsonObject.find("PoreID");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            bool bfd = false;
            for (int i = 0, n = m_pcmbPoreID->count() - 1; i < n; ++i)
            {
                if (txt == m_pcmbPoreID->itemText(i))
                {
                    m_pcmbPoreID->setCurrentIndex(i);
                    bfd = true;
                    break;
                }
            }
            if (!bfd)
            {
                m_pcmbPoreID->setCurrentIndex(m_pcmbPoreID->count() - 1);
                m_peditPoreID->setText(txt);
            }
        }

        ite = jsonObject.find("SeqBuffer");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            auto pre = txt.left(1);
            bool bfd = false;
            for (int i = 0, n = m_pcmbSeqBuffer->count() - 1; i < n; ++i)
            {
                if (pre == m_pcmbSeqBuffer->itemText(i))
                {
                    m_pcmbSeqBuffer->setCurrentIndex(i);
                    bfd = true;
                    break;
                }
            }
            if (!bfd)
            {
                //TODO:
            }
            auto suf = txt.right(txt.size() - 1);
            m_pispSeqBuffer->setValue(suf.toInt());
        }

        ite = jsonObject.find("SeqVoltage");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            bool bfd = false;
            for (int i = 0, n = m_pcmbSeqVoltage->count() - 1; i < n; ++i)
            {
                if (txt == m_pcmbSeqVoltage->itemText(i))
                {
                    m_pcmbSeqVoltage->setCurrentIndex(i);
                    bfd = true;
                    break;
                }
            }
            if (!bfd)
            {
                m_pcmbSeqVoltage->setCurrentIndex(m_pcmbSeqVoltage->count() - 1);
                m_pdspSeqVoltage->setValue(txt.left(txt.size() - 1).toDouble());
            }
        }

        ite = jsonObject.find("SequencerID");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            bool bfd = false;
            for (int i = 0, n = m_pcmbSequencerID->count(); i < n; ++i)
            {
                if (txt == m_pcmbSequencerID->itemText(i))
                {
                    m_pcmbSequencerID->setCurrentIndex(i);
                    bfd = true;
                    break;
                }
            }
            if (!bfd)
            {
                //TODO:
            }
        }

        ite = jsonObject.find("SequencerVersion");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            bool bfd = false;
            for (int i = 0, n = m_pcmbSequencerVersion->count(); i < n; ++i)
            {
                if (txt == m_pcmbSequencerVersion->itemText(i))
                {
                    m_pcmbSequencerVersion->setCurrentIndex(i);
                    bfd = true;
                    break;
                }
            }
            if (!bfd)
            {
                //TODO:
            }
        }

        ite = jsonObject.find("StartTime");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            m_peditStartTime->setText(txt);
        }

        ite = jsonObject.find("EndTime");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            m_peditEndTime->setText(txt);
        }

        ite = jsonObject.find("FolderPath");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            m_peditFolderPath->setText(txt);
        }

        ite = jsonObject.find("Temperature");
        if (ite != jsonObject.end())
        {
            auto txt = ite->toString();
            int pos = txt.indexOf(" ");
            if (pos == -1)
            {
                return false;
            }
            auto pre = txt.left(pos);
            m_pdspTemperature->setValue(pre.toDouble());
        }

    }

    return true;
}


bool RecordSettingDialog::InitItemDefault2(void)
{
    std::string strjson = ConfigServer::GetCurrentPath() + "/etc/Conf/model_configs.json";
    if (!std::filesystem::exists(strjson))
    {
        return false;
    }

    std::ifstream ifs(strjson);
    std::stringstream sst;
    sst << ifs.rdbuf();

    //return ParseRecordConfig2(sst.str());
    std::string str = sst.str();
    emit setRecordConfigSignal2(QString::fromStdString(str), str.size(), false);

    return true;
}

bool RecordSettingDialog::InitItemLocal2(const std::string& scont)
{
    std::string strjson = ConfigServer::GetCurrentPath() + "/etc/Conf/model_configs.json";
    FILE* pf = fopen(strjson.c_str(), "w");
    if (pf == nullptr)
    {
        return false;
    }
    auto ret = fwrite(scont.c_str(), 1, scont.size(), pf);
    fclose(pf);
    return ret != 0;
}

bool RecordSettingDialog::InitItemOnline2(void)
{
    HttpClient httpcl;
    httpcl.SetIngorSSL(true);
    auto strUrl = ConfigServer::GetInstance()->GetSequenceConfig();
    //if (strUrl.size() <= 0)
    {
        if (ConfigServer::GetInstance()->GetWorknet() == 1)
        {
            strUrl = "http://172.16.18.2:9085/v1/model_configs/latest";//"http://172.16.16.44:9085/v1/model_configs/latest"
        }
        else
        {
            strUrl = "http://192.168.0.77:9106/cycloneconfig/v1/model_configs/latest/";//"http://192.168.0.106:9085/v1/model_configs/latest";
        }
        ConfigServer::GetInstance()->SetSequenceConfig(strUrl);
    }
    bool ret = httpcl.RequestGet(strUrl);
    if (!ret)
    {
        return false;
    }

    size_t retsz = 0;
    auto retstr = httpcl.GetReadString(retsz);
    emit setRecordConfigSignal2(QString::fromStdString(retstr), retsz, true);
    //ret = ParseRecordConfig2(retstr, retsz);
    //if (ret)
    //{
    //	if (!InitItemLocal2(retstr))
    //	{
    //		LOGW("SequenceSettingLocal FAILED!!!");
    //	}
    //}
    //else
    //{
    //	LOGW("SequenceSettingOnline FAILED msg={}", retstr.c_str());
    //}

    return ret;
}

bool RecordSettingDialog::ParseRecordConfig2(const std::string& scont, size_t sz)
{
    if (sz <= 0)
    {
        sz = scont.size();
    }
    if (sz <= 0)
    {
        return false;
    }


    QJsonParseError jsonError;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(scont.c_str(), &jsonError));

    if (jsonError.error != QJsonParseError::NoError)
    {
        LOGE("Wrong model config json content!!!={}", scont.c_str());
        return false;
    }

    if (jsonDocument.isObject())
    {
        QJsonObject jsonObject = jsonDocument.object();
        auto ite = jsonObject.find("status");
        if (ite != jsonObject.end())
        {
            auto state = ite->toString();
            if (state != "done")
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        ite = jsonObject.find("data");
        if (ite == jsonObject.end())
        {
            return false;
        }

        auto dataobj = ite->toObject();
        ite = dataobj.find("model");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbModel->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("basecall_model");
        if (ite != dataobj.end())
        {
            if (ite->isArray())
            {
                auto dataarray = ite->toArray();
                for (int i = 0, n = dataarray.size(); i < n; ++i)
                {
                    auto val = dataarray.at(i);
                    m_pcmbBasecallModel->addItem(val.toString());
                }
            }
        }
        ite = dataobj.find("min_width");
        if (ite != dataobj.end())
        {
            if (ite->isDouble())
            {
                //m_peditMinWidth->setText(QString::number(ite->toDouble()));
                m_pdspMinWidth->setValue(ite->toDouble());
            }
        }
        ite = dataobj.find("window_width");
        if (ite != dataobj.end())
        {
            if (ite->isDouble())
            {
                //m_peditWindowWidth->setText(QString::number(ite->toDouble()));
                m_pdspWindowWidth->setValue(ite->toDouble());
            }
        }
        ite = dataobj.find("min_gain_per_sample");
        if (ite != dataobj.end())
        {
            if (ite->isDouble())
            {
                //m_peditMinGainPerSample->setText(QString::number(ite->toDouble()));
                m_pdspMinGainPerSample->setValue(ite->toDouble());
            }
        }
        ite = dataobj.find("is_adaptor_thre");
        if (ite != dataobj.end())
        {
            if (ite->isDouble())
            {
                //m_peditIsAdaptorThre->setText(QString::number(ite->toDouble()));
                m_pdspIsAdaptorThre->setValue(ite->toDouble());
            }
        }
        ite = dataobj.find("speed");
        if (ite != dataobj.end())
        {
            if (ite->isDouble())
            {
                //m_peditSpeed->setText(QString::number(ite->toDouble()));
                m_pdspSpeed->setValue(ite->toDouble());
            }
        }

    }

    return true;
}

void RecordSettingDialog::InitRequestRecordConfig(void)
{
    bool ret = InitItemOnline();
    if (!ret)
    {
        LOGW("Request experiment_configs FAILED!!! Will Load Default configs.");
        ret = InitItemDefault();
        if (!ret)
        {
            LOGW("Load Default experiment_configs FAILED!!!");
        }
    }
    else
    {
        LOGI("Request experiment_configs SUCCESS!");
    }

    ret = InitItemOnline2();
    if (!ret)
    {
        LOGW("Request model_configs FAILED!!! Will Load Default configs.");
        ret = InitItemDefault2();
        if (!ret)
        {
            LOGW("Load Default model_configs FAILED!!!");
        }
    }
    else
    {
        LOGI("Request model_configs SUCCESS!");
    }
}

bool RecordSettingDialog::CreateTopPath(void)
{
    if(m_strSavePath.empty())
        return false;
    m_strTaskName = GetTaskName();

    std::string strtopdir = m_strSavePath + "/" + m_strTaskName;
    m_strTaskPath = strtopdir;

    std::filesystem::path toppath(strtopdir);
    int cnt = 0;
    while (std::filesystem::exists(toppath))
    {
        m_strTaskPath = strtopdir + "_" + std::to_string(++cnt);
        toppath = std::filesystem::path(m_strTaskPath);
    }

    if (!std::filesystem::exists(toppath))
    {
        return std::filesystem::create_directories(toppath);
    }

    return true;
}

std::string RecordSettingDialog::CreateDataPath(void)
{
    if (CreateTopPath())
    {
        bool ret = Json4SaveSetting(m_strTaskPath, m_strTaskName);
        if (ret)
        {
            auto subdir = m_strTaskPath + "/Raw";
            std::filesystem::path subpath(subdir);
            if (!std::filesystem::exists(subpath))
            {
                if (std::filesystem::create_directories(subpath))
                {
                    return subdir;
                }
            }
            else
            {
                return subdir;
            }
        }
    }
    return std::string();
}

void RecordSettingDialog::SetSavePath(const std::string& dir)
{
    m_strSavePath = dir;
}

void RecordSettingDialog::SetSamplingRate(int rate)
{
    m_pispOriginalSampeRate->setValue(rate);
}


bool RecordSettingDialog::UpdateEndtime4Json(void)
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString strCurrentTime = curDateTime.toString("yyyy-MM-dd hh:mm:ss");
    m_peditEndTime->setText(strCurrentTime);

    QFile file(m_strSaveJson.c_str());
    if (!file.open(QIODevice::ReadWrite))
    {
        LOGE("Json File {} open FAILED!!!", m_strSaveJson.c_str());
        return false;
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    //update EndTime
    obj["EndTime"] = strCurrentTime;
    jdc.setObject(obj);
    file.seek(0);
    m_strExperimentConfigs = jdc.toJson().toStdString();
    file.write(jdc.toJson());
    file.flush();
    file.close();

    return true;
}

bool RecordSettingDialog::LoadConfig(QSettings* pset)
{
    if (pset != nullptr)
    {
        int seqid = ConfigServer::GetInstance()->GetSequencingId();
        if (m_pcmbSequencerID->count() > seqid)
        {
            m_pcmbSequencerID->setCurrentIndex(seqid);
            return true;
        }
    }

    return false;
}

bool RecordSettingDialog::SaveConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    pset->beginGroup("RecordConfig");
    pset->setValue("SequencingId", m_pcmbSequencerID->currentIndex());
    pset->endGroup();

    pset->beginGroup("RecordConfig");
    pset->setValue("url", ConfigServer::GetInstance()->GetRecordConfig().c_str());
    pset->setValue("sequrl", ConfigServer::GetInstance()->GetSequenceConfig().c_str());
    pset->endGroup();


    return true;
}

bool RecordSettingDialog::Json4RegisterTask(std::string& retjson)
{
    QJsonObject jsonObject;
    jsonObject.insert("data_name", m_strTaskName.c_str());//
    jsonObject.insert("option", "reads_detection");//
    jsonObject.insert("status", "runing");//

    QJsonObject jsonSubobj;
    jsonSubobj.insert("data_name", m_strTaskName.c_str());//
    //std::string stroutput = m_strSavePath + "/outputs/" + m_strTaskName;
    std::string stroutput = "/data/output_data/";
    std::filesystem::path pathoutput(stroutput);
    if (!std::filesystem::exists(pathoutput))
    {
        std::filesystem::create_directories(pathoutput);
    }
    stroutput += m_strTaskName;
    jsonSubobj.insert("out_path", stroutput.c_str());//
    jsonSubobj.insert("model", m_pcmbModel->currentText());//
    jsonSubobj.insert("basecall_model", m_pcmbBasecallModel->currentText());//
    jsonSubobj.insert("min_width", m_pdspMinWidth->value());//m_peditMinWidth->text().toInt()
    jsonSubobj.insert("window_width", m_pdspWindowWidth->value());//m_peditWindowWidth->text().toInt()
    jsonSubobj.insert("min_gain_per_sample", m_pdspMinGainPerSample->value());//m_peditMinGainPerSample->text().toFloat()
    jsonSubobj.insert("is_adaptor_thre", m_pdspIsAdaptorThre->value());//m_peditIsAdaptorThre->text().toInt()
    jsonSubobj.insert("speed", m_pdspSpeed->value());//m_peditSpeed->text().toInt()

    jsonSubobj.insert("is_parsing_events", true);//
    jsonSubobj.insert("analysing_options", "reads_detection");//
    jsonSubobj.insert("save_intermediate_results", true);//
    jsonSubobj.insert("channel_select", "all");//
    jsonSubobj.insert("using_tmp_path_to_save_results", false);//
    jsonSubobj.insert("library", "ecoli");//
    jsonSubobj.insert("frame_duration", 10000);//
    jsonSubobj.insert("type_of_task", "reads_detection");//
    jsonSubobj.insert("chn_num_total", 0);//
    jsonSubobj.insert("save_img", true);//

    //QJsonArray jsonArray;
    //jsonArray.append(false);
    //jsonArray.append(false);
    //jsonArray.append(true);
    //jsonSubobj.insert("is_plot_list", jsonArray);//

    jsonSubobj.insert("task_owner", "C++");

    jsonObject.insert("task_params", jsonSubobj);


    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);
    retjson = jsonDoc.toJson().toStdString();

    return false;
}

void RecordSettingDialog::onClickSaveSlot(void)
{
    std::filesystem::path pathdata(ConfigServer::GetCurrentPath() + "/etc/Data");
    if (!std::filesystem::exists(pathdata))
    {
        std::filesystem::create_directories(pathdata);
    }
    Json4SaveSetting(pathdata.string(), GetTaskName());
    hide();
}

void RecordSettingDialog::onClickLoadSlot(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Json File"), QString::fromStdString(ConfigServer::GetCurrentPath() + "/etc/Data"),
        "Json (*.json)", nullptr, QFileDialog::HideNameFilterDetails);

    if (!fileName.isEmpty())
    {
        if (!ParseJsonSettingFile(fileName.toStdString()))
        {
            QMessageBox::warning(this, tr("Warning"), tr("LoadJsonFile FAILED."));
            LOGW("LoadJsonFile FAILED.");
        }
    }
}

void RecordSettingDialog::onChangeSequencingIdSlot(int val)
{
    ConfigServer::GetInstance()->SetSequencingId(val);
    emit onSequencingIdChangedSignal(val);
}

void RecordSettingDialog::setRecordConfigSlot(const QString& qsconf, int sz, bool loc)
{
    std::string retstr(qsconf.toStdString());
    bool ret = ParseRecordConfig(retstr, sz);
    if (ret)
    {
        if (loc)
        {
            if (!InitItemLocal(retstr))
            {
                LOGW("Local experiment_configs FAILED!!!");
            }
        }
    }
    else
    {
        LOGW("Parse experiment_configs FAILED msg={}", retstr.c_str());
    }

    connect(m_pcmbSequencerID, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeSequencingIdSlot(int)));
    int seqid = ConfigServer::GetInstance()->GetSequencingId();
    if (m_pcmbSequencerID->count() > seqid)
    {
        m_pcmbSequencerID->setCurrentIndex(seqid);
    }
}

void RecordSettingDialog::setRecordConfigSlot2(const QString& qsconf, int sz, bool loc)
{
    std::string retstr(qsconf.toStdString());
    bool ret = ParseRecordConfig2(retstr, sz);
    if (ret)
    {
        if (loc)
        {
            if (!InitItemLocal2(retstr))
            {
                LOGW("Local model_configs FAILED!!!");
            }
        }
    }
    else
    {
        LOGW("Parse model_configs FAILED msg={}", retstr.c_str());
    }
}

void RecordSettingDialog::onClickNanoporeSequencing(bool val)
{
    if (val)
    {
        if (!m_bStartAlgorithmServer)
        {
            m_bStartAlgorithmServer = true;

            //bool ret = InitItemOnline2();
            //if (!ret)
            //{
            //	LOGW("Request model_configs FAILED!!! And will Load Default configs.");
            //	ret = InitItemDefault2();
            //	if (!ret)
            //	{
            //		LOGW("Load Default model_configs FAILED!!!");
            //	}
            //}
            //else
            //{
            //	LOGI("Request model_configs SUCCESS!");
            //}

            emit onEnableStartSaveDataSignal(false);
            m_tmrStartAlgorithmServer.start(0);
        }
        else
        {
            if (!ConfigServer::GetInstance()->GetAlgorithmServerStatus())
            {
                emit onEnableStartSaveDataSignal(false);
            }
        }
    }
    else
    {
        emit onEnableStartSaveDataSignal(true);
    }
}

#if 0
void RecordSettingDialog::timerInitRecordConfig(void)
{
    bool ret = InitItemOnline();
    //if (!ret)
    //{
    //	LOGW("Request experiment_configs FAILED!!! And will Load Default setting.");
    //	ret = InitItemDefault();
    //	if (!ret)
    //	{
    //		LOGW("Load Default experiment_configs FAILED!!!");
    //	}
    //}
    //else
    //{
    //	LOGI("Request experiment_configs SUCCESS!");
    //}
    //connect(m_pcmbSequencerID, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeSequencingIdSlot(int)));
    //int seqid = ConfigServer::GetInstance()->GetSequencingId();
    //if (m_pcmbSequencerID->count() > seqid)
    //{
    //	m_pcmbSequencerID->setCurrentIndex(seqid);
    //}
    //ret = InitItemOnline2();
    //if (!ret)
    //{
    //	LOGW("SequenceSettingOnline FAILED!!! And will Load Default setting.");
    //	ret = InitItemDefault2();
    //	if (!ret)
    //	{
    //		LOGW("SequenceSettingDefault FAILED!!!");
    //	}
    //}
    //else
    //{
    //	LOGI("Request model_configs SUCCESS!");
    //}
}
#endif

void RecordSettingDialog::timerStartAlgorithmServer(void)
{
    ConfigServer::GetInstance()->SetAlgorithmServerStatus(false);

#ifdef Q_OS_WIN32
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));
#else

#if 0
    auto strUrl = std::string("http://192.168.0.106:9103/auto_analysis.git");//
    if (ConfigServer::GetInstance()->GetWorknet() == 1)
    {
        strUrl = std::string("http://172.16.18.2:9103/auto_analysis.git");
    }
    const int m_ciThread = 4;
    std::string struser[m_ciThread] = { "wt_one", "wt_two", "wt_three", "wt_four" };
    std::string strhome[m_ciThread] = { "/home/wt_one", "/home/wt_two", "/home/wt_three", "/home/wt_four" };
    char arcmd[1024] = { 0 };
    sprintf(arcmd, "sudo killall -v supervisord ; sudo killall -v gunicorn ; sudo killall -v celery");
    LOGI(arcmd);
    system(arcmd);
    for (int i = 0; i < m_ciThread; ++i)
    {
#if 1
        std::string strAlgorithmServerPath = (strhome[i] + "/deploy_autonanopore/autonanopore");
        sprintf(arcmd, "cd %s && sudo -u %s bash run.sh online", strAlgorithmServerPath.c_str(), struser[i].c_str());
        LOGI(arcmd);
        system(arcmd);
#else
        std::string strAlgorithmServerPath = (strhome[i] + "/auto_analysis");
        std::string strStatusCodePath = (strhome[i] + "/auto_analysis/scripts/status_code.info");
        std::filesystem::path pathStatusCodePath(strStatusCodePath);
        if (!std::filesystem::exists(pathStatusCodePath))
        {
            sprintf(arcmd, "rm -rf %s ; cd %s && sudo -u %s git clone %s", strAlgorithmServerPath.c_str(), strhome[i].c_str(), struser[i].c_str(), strUrl.c_str());
            LOGI(arcmd);
            system(arcmd);
        }
        else
        {
            sprintf(arcmd, "cd %s && sudo -u %s git checkout scripts/*.log ; sudo -u %s git pull", strAlgorithmServerPath.c_str(), struser[i].c_str(), struser[i].c_str());
            LOGI(arcmd);
            system(arcmd);
        }
        sprintf(arcmd, "cd %s && sudo -u %s chmod 755 auto_analysis ; sudo -u %s ./auto_analysis", strAlgorithmServerPath.c_str(), struser[i].c_str(), struser[i].c_str());
        LOGI(arcmd);
        system(arcmd);
#endif
    }
#else
    //std::string mainUserName = ConfigServer::GetInstance()->GetMainUserName();
    //const char* puser = mainUserName.c_str();//$USER//me=${SUDO_USER:-$LOGNAME}//$me
    auto strUrl = std::string("http://192.168.0.106:9103/auto_analysis.git");//
    if (ConfigServer::GetInstance()->GetWorknet() == 1)
    {
        strUrl = std::string("http://172.16.18.2:9103/auto_analysis.git");
    }
    char arcmd[1024] = { 0 };
    std::string strAlgorithmServerPath = (ConfigServer::GetHomePath() + "/auto_analysis");
    //std::string strStatusCodePath = (ConfigServer::GetHomePath() + "/auto_analysis/scripts/status_code.info");
    //std::filesystem::path pathStatusCodePath(strStatusCodePath);
    //if (!std::filesystem::exists(pathStatusCodePath))
    //{
    //    sprintf(arcmd, "rm -rf %s ; cd %s && sudo -u $SUDO_USER git clone %s", strAlgorithmServerPath.c_str(), ConfigServer::GetHomePath().c_str(), strUrl.c_str());
    //    system(arcmd);
    //}
    //else
    //{
    //    sprintf(arcmd, "cd %s && sudo -u $SUDO_USER git checkout scripts/*.log ; sudo -u $SUDO_USER git pull", strAlgorithmServerPath.c_str());
    //    system(arcmd);
    //}
    sprintf(arcmd, "cd %s && sudo -u $SUDO_USER chmod 755 auto_analysis ; sudo -u $SUDO_USER ./auto_analysis", strAlgorithmServerPath.c_str());
    system(arcmd);
#endif

    ConfigServer::GetRunLog("StartAlgorithmServer", "top -b -o %MEM", 5000);

    emit checkAlgorithmServerStatusCodeSignal();

#endif
}



void RecordSettingDialog::OnCheckAllSelect(int stat)
{
    for (int i = 0; i < CHANNEL_NUM; ++i)
    {
        bool chk = (stat != Qt::CheckState::Unchecked);
        m_vetChannelItem[i]->setChecked(chk);
        m_vetChannelSelect[i] = chk;
    }
}


void RecordSettingDialog::UpdateSaveChkState(int ch)
{
    m_vetChannelSelect[ch] = m_vetChannelItem[ch]->isChecked();
}


void RecordSettingDialog::OnCheckShowVisible(bool chk)
{
    if (chk)
    {
        int cnt = 0;
        for (int ch = 0; ch < CHANNEL_NUM; ++ch)
        {
            bool chk2 = m_vetChannelSelect[ch];
            m_pRecordTableWgt->setRowHidden(ch,!chk2);
            if (chk2)
            {
                ++cnt;
            }
        }
    }
    else
    {
        for (int ch = 0; ch < CHANNEL_NUM; ++ch)
        {
            m_pRecordTableWgt->setRowHidden(ch,chk);
        }
    }
}


void RecordSettingDialog::SetChannelSelect(const std::vector<bool> &vctIsSave)
{
    std::copy(vctIsSave.cbegin(),vctIsSave.cend(),m_vetChannelSelect.begin());
    for(int ch  = 0; ch < CHANNEL_NUM; ++ch)
    {
        m_vetChannelItem[ch]->setChecked(vctIsSave[ch]);
    }
}
void RecordSettingDialog::GenerateRunID(void)
{
#if 1
    m_strRunId = QString::fromStdString(m_strTaskName + "_RUN");
#else
    QUuid uuid = QUuid::createUuid();
    m_strRunId = uuid.toString();
#endif
}
