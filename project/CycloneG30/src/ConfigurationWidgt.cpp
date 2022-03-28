#include "ConfigurationWidgt.h"
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <qmath.h>
#include <QProgressDialog>
#include <QApplication>
#include <QTimer>
#include <QMessageBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QInputDialog>

#include "Log.h"
#include "CustomDelegate.h"
#include "ConfigServer.h"
#include "UsbFtdDevice.h"
#include "UsbProtoMsg.h"




ConfigurationWidgt::ConfigurationWidgt(UsbFtdDevice *pUsbFtdDevice,QWidget *parent):QWidget(parent),m_pUsbFtdDevice(pUsbFtdDevice)
{
    InitCtrl();
    LVDSCurRangeChangeSlot();
}


ConfigurationWidgt::~ConfigurationWidgt()
{

}


void ConfigurationWidgt::InitCtrl(void)
{
    m_pSysInitBtn = new QPushButton(QIcon(":/img/img/initialization.png"),QStringLiteral("Initial"),this);
    m_pSysLoadConfBtn = new QPushButton(QIcon(":/img/img/loadconf.png"),QStringLiteral("LoadReg"),this);
    m_pReadConfBtn = new QPushButton(QStringLiteral("Read Configuration"),this);
    m_pWriteConfBtn = new QPushButton(QStringLiteral("Write Configuration"),this);

    m_pAdcFreqLab = new QLabel(QStringLiteral("ADC sample rate"),this);
    m_pAdcFreqComb = new QComboBox(this);

    m_pIsRawDatalab = new QLabel(QStringLiteral("Raw or ADC Mode"),this);
    m_pIsUnSignRawDataBtn =  new QRadioButton("RawData", this);
    m_pIsSignAdcDataBtn =  new QRadioButton("ADC out", this);
    QButtonGroup *grpRawBtn = new QButtonGroup(this);
    grpRawBtn->addButton(m_pIsUnSignRawDataBtn,ENUM_RAW_DATA);
    grpRawBtn->addButton(m_pIsSignAdcDataBtn,ENUM_RAW_ADC_DATA);
    m_pIsUnSignRawDataBtn->setChecked(false);
    m_pIsSignAdcDataBtn->setChecked(true);

    m_pbTransDatalab = new QLabel(QStringLiteral("Convert to current"),this);
    m_pbNeedToTransBtn =  new QRadioButton("Convert", this);
    m_pbNotToTransBtn =  new QRadioButton("Not Convert", this);
    QButtonGroup *grpTransBtn = new QButtonGroup(this);
    grpTransBtn->addButton(m_pbNeedToTransBtn,ENUM_NEED_TO_TRANS);
    grpTransBtn->addButton(m_pbNotToTransBtn,ENUM_NOT_TO_TRANS);
    m_pbNeedToTransBtn->setChecked(true);

    m_pInCurrentLab = new QLabel(QStringLiteral("Input current range"),this);

    m_samprate[ADC_FREQ_SET_5K_DEFAULT] = SAMPLE_RATE_5K;
    m_samprate[ADC_FREQ_SET_10K] = SAMPLE_RATE_10K;
    m_samprate[ADC_FREQ_SET_20K] = SAMPLE_RATE_20K;
    m_samprate[ADC_FREQ_SET_3_75K] = SAMPLE_RATE_3_75K;

    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT] = new QComboBox(this);
    m_pInCurrentArrComb[ADC_FREQ_SET_10K] = new QComboBox(this);
    m_pInCurrentArrComb[ADC_FREQ_SET_20K] = new QComboBox(this);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K] = new QComboBox(this);


    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("62.5pA"),INPUT_5K_CURRENT_RANGE_62_5PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("125pA"),INPUT_5K_CURRENT_RANGE_125PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("156.25pA"),INPUT_5K_CURRENT_RANGE_156_25PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("250pA"),INPUT_5K_CURRENT_RANGE_250PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("312.5pA"),INPUT_5K_CURRENT_RANGE_312_5PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("500pA"),INPUT_5K_CURRENT_RANGE_500PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("625pA"),INPUT_5K_CURRENT_RANGE_625PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("781.25pA"),INPUT_5K_CURRENT_RANGE_781_25PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("1250pA"),INPUT_5K_CURRENT_RANGE_1250PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->addItem(QStringLiteral("1562.5pA"),INPUT_5K_CURRENT_RANGE_1562_5PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->setCurrentText(QStringLiteral("500pA"));


    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("125pA"),INPUT_10K_CURRENT_RANGE_125PA   );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("250pA"),INPUT_10K_CURRENT_RANGE_250PA   );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("312.5pA"),INPUT_10K_CURRENT_RANGE_312_5PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("500pA"),INPUT_10K_CURRENT_RANGE_500PA   );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("625pA"),INPUT_10K_CURRENT_RANGE_625PA   );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("1000pA"),INPUT_10K_CURRENT_RANGE_1000PA  );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("1250pA"),INPUT_10K_CURRENT_RANGE_1250PA  );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("1562.5pA"),INPUT_10K_CURRENT_RANGE_1562_5PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("2500pA"),INPUT_10K_CURRENT_RANGE_2500PA  );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->addItem(QStringLiteral("3125pA"),INPUT_10K_CURRENT_RANGE_3125PA  );
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->setCurrentText(QStringLiteral("500pA"));
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->setVisible(false);

    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("250pA"),INPUT_20K_CURRENT_RANGE_250PA  );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("500pA"),INPUT_20K_CURRENT_RANGE_500PA  );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("625pA"),INPUT_20K_CURRENT_RANGE_625PA  );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("1000pA"),INPUT_20K_CURRENT_RANGE_1000PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("1250pA"),INPUT_20K_CURRENT_RANGE_1250PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("2000pA"),INPUT_20K_CURRENT_RANGE_2000PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("2500pA"),INPUT_20K_CURRENT_RANGE_2500PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("3125pA"),INPUT_20K_CURRENT_RANGE_3125PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("5000pA"),INPUT_20K_CURRENT_RANGE_5000PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->addItem(QStringLiteral("6250pA"),INPUT_20K_CURRENT_RANGE_6250PA );
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->setCurrentText(QStringLiteral("500pA"));
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->setVisible(false);

    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("46.875pA"),INPUT_3_75K_CURRENT_RANGE_46_875PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("93.72pA"),INPUT_3_75K_CURRENT_RANGE_93_75PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("117.1875pA"),INPUT_3_75K_CURRENT_RANGE_117_1875PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("187.5pA"),INPUT_3_75K_CURRENT_RANGE_187_5PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("234.375pA"),INPUT_3_75K_CURRENT_RANGE_234_375PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("375pA"),INPUT_3_75K_CURRENT_RANGE_375PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("468.75pA"),INPUT_3_75K_CURRENT_RANGE_468_75PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("589.9375pA"),INPUT_3_75K_CURRENT_RANGE_589_9375PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("937.5pA"),INPUT_3_75K_CURRENT_RANGE_937_5PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->addItem(QStringLiteral("1171.875pA"),INPUT_3_75K_CURRENT_RANGE_1171_875PA);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->setCurrentText(QStringLiteral("468.75pA"));
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->setVisible(false);

    m_pOtherSettingGrpbx = new QGroupBox(QStringLiteral("Other setting"),this);
    m_pbPowerComsumplab = new QLabel(QStringLiteral("Power consumption:"),this);
    m_pLowPowerComsumpBtn =  new QRadioButton("Low", this);
    m_pHighPowerComsumpBtn =  new QRadioButton("High", this);
    m_pOtherReadConfBtn = new QPushButton(QStringLiteral("Read Configuration"),this);
    m_pOtherWriteConfBtn = new QPushButton(QStringLiteral("Write Configuration"),this);
    QButtonGroup *grpComsumpBtn = new QButtonGroup(this);
    grpComsumpBtn->addButton(m_pLowPowerComsumpBtn,ENUM_POWER_CONSUMPTION_LOW);
    grpComsumpBtn->addButton(m_pHighPowerComsumpBtn,ENUM_POWER_CONSUMPTION_HIGH);
    m_pLowPowerComsumpBtn->setChecked(true);

    m_pProgressDlg = new QProgressDialog("Loading commands...", "Abort Loading", 0, 100, this);
    m_pProgressEventLoop = new QEventLoop(this);
    m_pProgresssTimer = new QTimer(this);

    m_pProgressDlg->setWindowModality(Qt::WindowModal);
    m_pProgressDlg->setModal(true);
    m_pProgressDlg->reset();
    m_pProgressDlg->hide();
    m_pProgresssTimer->setSingleShot(true);


    m_pAdcFreqComb->addItem(QStringLiteral("5K(default)"),ADC_FREQ_SET_5K_DEFAULT);
    m_pAdcFreqComb->addItem(QStringLiteral("10K"),ADC_FREQ_SET_10K);
    m_pAdcFreqComb->addItem(QStringLiteral("20K"),ADC_FREQ_SET_20K);
    m_pAdcFreqComb->addItem(QStringLiteral("3.75K"),ADC_FREQ_SET_3_75K);
    m_pAdcFreqComb->setCurrentText(QStringLiteral("5K(default)"));

    QGroupBox *pSysGrpbox = new QGroupBox(QStringLiteral("Sysconfig"),this);
    QHBoxLayout *pSyshorlayout = new QHBoxLayout();
    m_pSysInitBtn->setFixedSize(100,50);
    m_pSysInitBtn->setIconSize(QSize(50,25));
    m_pSysLoadConfBtn->setIconSize(QSize(50,25));
    m_pSysLoadConfBtn->setFixedSize(150,50);
    pSyshorlayout->addWidget(m_pSysInitBtn);
    pSyshorlayout->addWidget(m_pSysLoadConfBtn);
    pSyshorlayout->addStretch();
    pSysGrpbox->setLayout(pSyshorlayout);

    QHBoxLayout *curhorlayout = new QHBoxLayout();
    curhorlayout->addWidget(m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]);
    curhorlayout->addWidget(m_pInCurrentArrComb[ADC_FREQ_SET_10K]);
    curhorlayout->addWidget(m_pInCurrentArrComb[ADC_FREQ_SET_20K]);
    curhorlayout->addWidget(m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]);


    QGroupBox *pAFEgrpbox = new QGroupBox(QStringLiteral("AFE analog setting "),this);
    QGridLayout *pAfegridlayout = new QGridLayout();
    int raw = 0;
    pAfegridlayout->addWidget(m_pAdcFreqLab,raw,0,1,1);
    pAfegridlayout->addWidget(m_pAdcFreqComb,raw++,1,1,1);
    pAfegridlayout->addWidget(m_pIsRawDatalab,raw,0,1,1);
    pAfegridlayout->addWidget(m_pIsUnSignRawDataBtn,raw,1,1,1);
    pAfegridlayout->addWidget(m_pIsSignAdcDataBtn,raw++,2,1,1);
    pAfegridlayout->addWidget(m_pbTransDatalab,raw,0,1,1);
    pAfegridlayout->addWidget(m_pbNeedToTransBtn,raw,1,1,1);
    pAfegridlayout->addWidget(m_pbNotToTransBtn,raw++,2,1,1);
    pAfegridlayout->addWidget(m_pInCurrentLab,raw,0,1,1);
    pAfegridlayout->addLayout(curhorlayout,raw++,1,1,1);
    pAfegridlayout->addWidget(m_pWriteConfBtn,raw,1,1,1);
    pAfegridlayout->addWidget(m_pReadConfBtn,raw,2,1,1);
    pAfegridlayout->setColumnStretch(raw,1);
    pAFEgrpbox->setLayout(pAfegridlayout);

    QGridLayout *pOtherGridlayout = new QGridLayout();
    pOtherGridlayout->addWidget(m_pbPowerComsumplab,0,0,1,1);
    pOtherGridlayout->addWidget(m_pLowPowerComsumpBtn,0,1,1,1);
    pOtherGridlayout->addWidget(m_pHighPowerComsumpBtn,0,2,1,1);
    pOtherGridlayout->addWidget(m_pOtherWriteConfBtn,1,1,1,1);
    pOtherGridlayout->addWidget(m_pOtherReadConfBtn,1,2,1,1);
    pOtherGridlayout->setColumnStretch(3,1);
    m_pOtherSettingGrpbx->setLayout(pOtherGridlayout);

    InitAdvance();
    QVBoxLayout *phorlayout = new QVBoxLayout();
    phorlayout->addWidget(pSysGrpbox);
    phorlayout->addWidget(pAFEgrpbox);
    phorlayout->addWidget(m_pOtherSettingGrpbx);
    phorlayout->addWidget(m_pAdvanceSettingGrpbx);
    phorlayout->addStretch();
    setLayout(phorlayout);

    connect(m_pSysLoadConfBtn,&QPushButton::clicked,this,&ConfigurationWidgt::OnLoadRegBtnSlot);
    connect(m_pProgresssTimer,&QTimer::timeout,m_pProgressEventLoop,&QEventLoop::quit);
    connect(m_pProgressDlg,&QProgressDialog::canceled,m_pProgressEventLoop,&QEventLoop::quit);
    connect(m_pAdcFreqComb,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ConfigurationWidgt::SampleRateChangeSlot);
    connect(m_pSysInitBtn,&QPushButton::clicked,this,&ConfigurationWidgt::InitSysCfgSlot);

    connect(grpRawBtn, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),[=](int id, bool checked)
    {
        if(id == ENUM_RAW_DATA)
            ConfigServer::GetInstance()->SetIsRawData(checked);
    });

    connect(grpTransBtn, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),[=](int id, bool checked)
    {
        if(id == ENUM_NEED_TO_TRANS)
        {
            ConfigServer::GetInstance()->SetIsNeedToTrans(checked);
            m_pInCurrentLab->setVisible(checked);
            m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->setVisible(checked);
        }
    });
    connect(grpComsumpBtn, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),[=](int id, bool checked)
    {
        if(checked)
        {
            emit SigAFEPowerConsumpChange(POWER_CONSUMPTION_ENUM(id));
        }
    });
    connect(m_pVoltInvertBtnGrp, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),[=](int id, bool checked)
    {
        if(checked)
        {
            ConfigServer::GetInstance()->SetVoltagteInvert(id == ENUM_VOLTAGE_OUTPUT_INVERTER ? true : false );
        }
    });
    connect(m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT],QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ConfigurationWidgt::LVDSCurRangeChangeSlot);
    connect(m_pInCurrentArrComb[ADC_FREQ_SET_10K],QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ConfigurationWidgt::LVDSCurRangeChangeSlot);
    connect(m_pInCurrentArrComb[ADC_FREQ_SET_20K],QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ConfigurationWidgt::LVDSCurRangeChangeSlot);
    connect(m_pInCurrentArrComb[ADC_FREQ_SET_3_75K],QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ConfigurationWidgt::LVDSCurRangeChangeSlot);
    connect(m_pReadConfBtn,&QPushButton::clicked,this,&ConfigurationWidgt::OnReadConfBtnSlot);
    connect(m_pWriteConfBtn,&QPushButton::clicked,this,&ConfigurationWidgt::OnWriteConfBtnSlot);
    connect(m_pOtherReadConfBtn,&QPushButton::clicked,this,&ConfigurationWidgt::OnOtherReadConfBtnSlot);
    connect(m_pOtherWriteConfBtn,&QPushButton::clicked,this,&ConfigurationWidgt::OnOtherWriteConfBtnSlot);
    connect(m_pAdvanceSettingGrpbx,&QGroupBox::clicked,this,&ConfigurationWidgt::PermissionToModifyAdvanceSlot);


#if 0
    m_pbNotToTransBtn->setChecked(true);
#else
    m_pbNotToTransBtn->setChecked(false);
#endif

}

bool ConfigurationWidgt::LoadConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    bool bVoltInvert = ConfigServer::GetInstance()->GetIsVoltagteInvert();

    if(bVoltInvert)
    {
        m_pVoltInverterBtn->setChecked(true);
    }
    else
    {
        m_pVoltUnInverterBtn->setChecked(true);
    }

    return true;
}

bool ConfigurationWidgt::SaveConfig(QSettings* pset)
{
    if (pset == nullptr)
    {
        return false;
    }

    pset->beginGroup("AdvanceConfig");
    pset->setValue("VoltageInvert",  ConfigServer::GetInstance()->GetIsVoltagteInvert());
    pset->endGroup();

    return true;
}


void ConfigurationWidgt::InitAdvance(void)
{
    m_pAdvanceSettingGrpbx = new QGroupBox(QStringLiteral("Advance setting"),this);
    m_pVoltInverterlab = new QLabel(QStringLiteral("Voltage Inverter"),this);
    m_pVoltUnInverterBtn = new QRadioButton(QStringLiteral("UnInvert"),this);
    m_pVoltInverterBtn = new QRadioButton(QStringLiteral("Invert"),this);

    QLabel *pAllsourceLab = new QLabel(QStringLiteral("All source"),this);
    m_pAllIntalSoureSetBtn = new QPushButton(QStringLiteral("Set"),this);
    m_pVoltInvertBtnGrp = new QButtonGroup(this);
    m_pVoltInvertBtnGrp->addButton(m_pVoltUnInverterBtn,ENUM_VOLTAGE_OUTPUT_NORMAL);
    m_pVoltInvertBtnGrp->addButton(m_pVoltInverterBtn,ENUM_VOLTAGE_OUTPUT_INVERTER);
    m_pVoltUnInverterBtn->setChecked(true);
    m_pAdvanceSettingGrpbx->setCheckable(true);
    m_pAdvanceSettingGrpbx->setChecked(false);

    QGridLayout *pgridlayout = new QGridLayout();
    pgridlayout->addWidget(m_pVoltInverterlab,0,0,1,1);
    pgridlayout->addWidget(m_pVoltUnInverterBtn,0,1,1,1);
    pgridlayout->addWidget(m_pVoltInverterBtn,0,2,1,1);
    pgridlayout->addWidget(pAllsourceLab,1,0,1,1);
    pgridlayout->addWidget(m_pAllIntalSoureSetBtn,1,1,1,1);
    pgridlayout->setColumnStretch(3,10);
    m_pAdvanceSettingGrpbx->setLayout(pgridlayout);
    connect(m_pAllIntalSoureSetBtn,&QPushButton::clicked,this,&ConfigurationWidgt::SigSetAllChIntalSource);

}


void ConfigurationWidgt::OnLoadRegBtnSlot(void)
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Load file to send register addresses and data continuously"));
    fileDialog->setNameFilter(tr("csv files(*.csv)"));
    fileDialog->setViewMode(QFileDialog::Detail);


    if (fileDialog->exec())
    {
        QString loadFileNamePath = fileDialog->selectedFiles().at(0);
        LOGI(loadFileNamePath.toStdString().c_str());
        if (loadFileNamePath != "")
        {
             FILE* pf = ::fopen(loadFileNamePath.toStdString().c_str(), "r");
             if (pf != nullptr)
             {
                 ::fseek(pf, 0L, SEEK_END);
                 int len = ::ftell(pf);

                 char* pbuf = new char[len];
                 memset(pbuf, 0, len);

                 ::fseek(pf, 0L, SEEK_SET);
                 int buflen = ::fread(pbuf, 1, len, pf);

                 std::vector<std::vector<std::string>> vetvetdt;
                 std::vector<std::string> vetrow;
                 UsbProtoMsg::SplitString(pbuf, "\n", vetrow);

                 delete[] pbuf;
                 pbuf = nullptr;
                 fclose(pf);
                 pf = nullptr;

                 if (vetrow.size() > 0)
                 {
                     for (auto&& ite : vetrow)
                     {
                         std::vector<std::string> vetdt;
                         UsbProtoMsg::SplitString(ite.c_str(), ",", vetdt);
                         vetvetdt.emplace_back(vetdt);
                     }

                     int maximum = vetvetdt.size() -2;
                     m_pProgressDlg->setRange(0,maximum);
                     m_pProgressDlg->reset();
                     m_pProgressDlg->show();

                     int cnt = 0;
                     bool bRet =false;
                     for (int i = 2;  i < vetvetdt.size(); ++i)
                     {
                         auto& ite = vetvetdt.at(i);
                         if (ite.size() >= 4)
                         {
                             if (!ite[0].empty() && !ite[1].empty() && !ite[2].empty() && !ite[3].empty())
                             {
                                 int type = ::atoi(ite[0].c_str());
//                                 uint interval =  ::atoi(ite[3].c_str());
                                 uint interval = QString::fromStdString(ite[3]).toUInt(&bRet,10);
                                 if(bRet && type == COMMAND_ADDR_ASIC)
                                 {
                                     uchar addr = UsbProtoMsg::String2Hex(ite[1].c_str());
                                     uint32_t data = UsbProtoMsg::String2Hex(ite[2].c_str());
                                     m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
                                 }
                                 cnt++;
                                 m_pProgresssTimer->start(interval);
                                 m_pProgressEventLoop->exec();

                               //printf("ite[0] %d, ite[1] :%x  ite[2] :%x ite[3] :%x\n",::atoi(ite[0].c_str()), UsbProtoMsg::String2Hex(ite[1].c_str()), UsbProtoMsg::String2Hex(ite[2].c_str()),::atoi(ite[3].c_str()));
                             }
                         }

                         QApplication::processEvents();
                         m_pProgressDlg->setValue(i);
                         if (m_pProgressDlg->wasCanceled())
                         {
                             break;
                         }

                     }
                     m_pProgressDlg->setValue(maximum);

                     LOGI("Load LoadReg.csv register config Success. Total items count is: {}",cnt);
                     return;
                 }
             }

        }
    }
    SAFE_DELETEP(fileDialog);

}


void ConfigurationWidgt::SampleRateChangeSlot(void)
{
    int sample = m_pAdcFreqComb->currentData(Qt::UserRole).toInt();
    switch (sample) {
    case ADC_FREQ_SET_5K_DEFAULT:
        m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->setVisible(true);
        m_pInCurrentArrComb[ADC_FREQ_SET_10K]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_20K]->setVisible(false);
        LVDSCurRangeChangeSlot();
        ConfigServer::GetInstance()->SetSampleRate(SAMPLE_RATE_5K);
        emit SigSampleRateChanged();
        break;
    case ADC_FREQ_SET_10K:
        m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_10K]->setVisible(true);
        m_pInCurrentArrComb[ADC_FREQ_SET_20K]->setVisible(false);
        LVDSCurRangeChangeSlot();
        ConfigServer::GetInstance()->SetSampleRate(SAMPLE_RATE_10K);
        emit SigSampleRateChanged();
        break;
    case ADC_FREQ_SET_20K:
        m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_10K]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_20K]->setVisible(true);
        LVDSCurRangeChangeSlot();
        ConfigServer::GetInstance()->SetSampleRate(SAMPLE_RATE_20K);
        emit SigSampleRateChanged();
        break;
    case ADC_FREQ_SET_3_75K:
        m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->setVisible(true);
        m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_10K]->setVisible(false);
        m_pInCurrentArrComb[ADC_FREQ_SET_20K]->setVisible(false);
        LVDSCurRangeChangeSlot();
        ConfigServer::GetInstance()->SetSampleRate(SAMPLE_RATE_3_75K);
        emit SigSampleRateChanged();
        break;
    default:
        break;
    }
}


void ConfigurationWidgt::LVDSCurRangeChangeSlot(void)
{
    int colIndx = m_pAdcFreqComb->currentData().toInt();
    int rowIndx = m_pInCurrentArrComb[colIndx]->currentData().toInt();

    ConfigServer::GetInstance()->SetLvdsCvtUnitArry(rowIndx,colIndx);

    std::pair<float,float> temp = ConfigServer::GetInstance()->GetLvdsCurConvt();
    LOGCI("Now the {}th parameter, Raw LSB:{}, ADC LSB:{}",rowIndx,temp.first,temp.second);


}

void ConfigurationWidgt::InitSysCfgSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }

    uchar addr     = 0xA4;
    uint32_t data  = 0x04030009;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA3;
    data = 0x00000014;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA3;
    data = 0x0000000C;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA1;
    data = 0x00000010;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA1;
    data = 0x00000000;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA2;
    data = 0x000000A0;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA2;
    data = 0x000000B0;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA2;
    data = 0x000000A8;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x00;
    data = 0x20201204;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA2;
    data = 0x000000A0;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA2;
    data = 0x000001A0;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x00;
    data = 0x20201204;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x06;
    data = 0x00001000;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x05;
    data = 0x00000001;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x06;
    data = 0x00000000;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x00;
    data = 0x20201204;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x12;
    data = 0x12CBCB0B;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0xA3;
    data = 0x0000004C;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x0D;
    data = 0x00000014;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x0A;
    data = 0x00000000;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x0B;
    data = 0x00000000;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);
    addr = 0x09;
//    data = 0x00388080;
    data = 0x00808080;
    m_pUsbFtdDevice->ASICRegisterWiteOne(addr,data);

    OnReadConfBtnSlot();
    if(!OnOtherReadConf())
    {
        QMessageBox::critical(this, tr("Error"), tr("Read other configure failed!!!"));
        return;
    }
}


void ConfigurationWidgt::SetEnable(const bool &enable)
{
    m_pAdcFreqComb->setEnabled(enable);
    m_pIsUnSignRawDataBtn->setEnabled(enable);
    m_pIsSignAdcDataBtn->setEnabled(enable);
    m_pbNeedToTransBtn->setEnabled(enable);
    m_pbNotToTransBtn->setEnabled(enable);
    m_pReadConfBtn->setEnabled(enable);
    m_pWriteConfBtn->setEnabled(enable);
    m_pInCurrentArrComb[ADC_FREQ_SET_5K_DEFAULT]->setEnabled(enable);
    m_pInCurrentArrComb[ADC_FREQ_SET_10K]->setEnabled(enable);
    m_pInCurrentArrComb[ADC_FREQ_SET_20K]->setEnabled(enable);
    m_pInCurrentArrComb[ADC_FREQ_SET_3_75K]->setEnabled(enable);
}


//    A0 SARADC_RAW 是否是RAW   AFE_IMMUX_SEL_LOAD mux选择;
//    12 是采样率选择 reg_adc_clk_div 240 * 1000/value+1 = 采样率hz;
//    A4 是电流量程选择 reg_afe_int_ctr;
void ConfigurationWidgt::OnReadConfBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }

    //读取采样率
    bool res =false;
    uint sampReg = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0x12,sampReg,res))
    {
        QMessageBox::critical(this, tr("Error"), tr("Read configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    int regDacClkDiv = sampReg >> 14 & 0x3f;
    int samprate = 240 * 1000/(regDacClkDiv+1); //Hz
    LOGCI("Read configuration the addr 0x{:X},regDacClkDiv 0x{:X}",0x12,regDacClkDiv);

    switch (samprate) {
    case SAMPLE_RATE_5K:
         m_pAdcFreqComb->setCurrentIndex(ADC_FREQ_SET_5K_DEFAULT);
        break;
    case SAMPLE_RATE_10K:
         m_pAdcFreqComb->setCurrentIndex(ADC_FREQ_SET_10K);
        break;
    case SAMPLE_RATE_20K:
         m_pAdcFreqComb->setCurrentIndex(ADC_FREQ_SET_20K);
        break;
    case SAMPLE_RATE_3_75K:
         m_pAdcFreqComb->setCurrentIndex(ADC_FREQ_SET_3_75K);
        break;
    default:
        QMessageBox::warning(this, tr("Warning"), tr("the samplerete %1hz result is beyond expectation!!!").arg(samprate));
        break;
    }

    //读取是否RawData
    uint saradc_raw = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0xA0,saradc_raw,res))
    {
        QMessageBox::critical(this, tr("Error"), tr("Read configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    int saradc_raw_flag = saradc_raw >> 23 & 0x01;
    saradc_raw_flag ==  1 ? m_pIsUnSignRawDataBtn->setChecked(true) : m_pIsSignAdcDataBtn->setChecked(true);
    LOGCI("Read configuration the addr 0x{:X},saradc_raw_flag 0x{:X}",0xA0,saradc_raw_flag);

    //读取量程参数
    uint regAFEintCtr = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0xA4,regAFEintCtr,res))
    {
        QMessageBox::critical(this, tr("Error"), tr("Read configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    uint afe_ctr_seq = regAFEintCtr &0x0f;
    LOGCI("Read configuration the addr 0x{:X},afe_ctr_seq 0x{:X}",0xA4,afe_ctr_seq);
    const int sampEnum = m_pAdcFreqComb->currentData().toInt();
    int reultIndx = m_pInCurrentArrComb[sampEnum]->findData(afe_ctr_seq ,Qt::UserRole,Qt::MatchExactly);
    if(reultIndx == -1)
    {
        QMessageBox::critical(this, tr("Error"), tr("the current seq num:%1 result is beyond expectation!!!").arg(afe_ctr_seq));
        return;
    }
    m_pInCurrentArrComb[sampEnum]->setCurrentIndex(reultIndx == -1 ? 0 : reultIndx);
    QMessageBox::information(this, tr("Info"), tr("Read configuration successfully!!!"));
    return;

//failed:
    QMessageBox::critical(this, tr("Error"), tr("Read configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
    return;
}

void ConfigurationWidgt::OnWriteConfBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }


    bool res =false;

    //发送采样率设置
    uint sampReg = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0x12,sampReg,res))
    {
        QMessageBox::critical(this, tr("Error"), tr("Write configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    int sampIndx = m_pAdcFreqComb->currentData().toInt();
    if(sampIndx > LVDS_SAMPLE_KIINDS_NUM)
    {
        QMessageBox::critical(this, tr("Error!!!"), tr("the samplerete indx:%1 is beyond expectation!!!").arg(sampIndx));
        return;
    }
    int samprate = m_samprate[sampIndx];
    int regDacClkDivSet = (240 * 1000/samprate) -1;
    sampReg &= ~ (0x3F << 14);
    sampReg |= (regDacClkDivSet &0x3F) << 14;

    if(!m_pUsbFtdDevice->ASICRegisterWiteOne(0x12,sampReg))
    {
        QMessageBox::critical(this, tr("Error"), tr("Write configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    LOGCI("Wirte configuration the addr 0x{:X},regDacClkDivSet 0x{:X}",0x12,regDacClkDivSet);

    //发送是否rawData
    uint saradc_raw = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0xA0,saradc_raw,res))
    {
        QMessageBox::critical(this, tr("Error"), tr("Write configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    int saradc_raw_flag = m_pIsUnSignRawDataBtn->isChecked() == true ? 1 : 0;
    saradc_raw &= ~(0x03 << 22); //清除 bit22 和 bit 23 为0;
    saradc_raw |= saradc_raw_flag<<23; //设置bit 23
    if(!m_pUsbFtdDevice->ASICRegisterWiteOne(0xA0,saradc_raw))
    {
        QMessageBox::critical(this, tr("Error"), tr("Write configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    LOGCI("Wirte configuration the addr 0x{:X},saradc_raw_flag 0x{:X}",0xA0,saradc_raw_flag);

    //发送量程
    uint regAFEintCtr = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0xA4,regAFEintCtr,res))
    {
        QMessageBox::critical(this, tr("Error"), tr("Write configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    const int sampEnum = m_pAdcFreqComb->currentData().toInt();
    int afe_ctr_seq = m_pInCurrentArrComb[sampEnum]->currentData().toInt();
    regAFEintCtr &= ~(0x0f <<0); //清除bit 0-3
    regAFEintCtr |= afe_ctr_seq << 0;  //设置bit 0-3
    if(!m_pUsbFtdDevice->ASICRegisterWiteOne(0xA4,regAFEintCtr))
    {
        QMessageBox::critical(this, tr("Error"), tr("Write configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
        return;
    }
    LOGCI("Wirte configuration the addr 0x{:X},afe_ctr_seq 0x{:X}",0xA4,afe_ctr_seq);


    QMessageBox::information(this, tr("Info"), tr("Write configuration successfully!!!"));
    return;
//failed:
    QMessageBox::critical(this, tr("Error"), tr("Write configuration failed,please check usb or whether reg value is reasonable!!!!!!"));
    return;
}
void ConfigurationWidgt::OnOtherReadConfBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }
    if(!OnOtherReadConf())
    {
        QMessageBox::critical(this, tr("Error"), tr("Read configure failed!!!"));
        return;
    }
    QMessageBox::information(this, tr("Info"), tr("Read configuration successfully!!!"));
    return;
}
void ConfigurationWidgt::OnOtherWriteConfBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }
    if(!OnOtherWriteConf())
    {
        QMessageBox::critical(this, tr("Error"), tr("Write configure failed!!!"));
        return;
    }
    QMessageBox::information(this, tr("Info"), tr("Write configuration successfully!!!"));
    return;
}
bool ConfigurationWidgt::OnOtherReadConf(void)
{
    bool res =false;
    uint regAFEintCtr = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0xA4,regAFEintCtr,res))
    {
        return res;
    }
    uint reg_afe_ib_ctr2_1 = regAFEintCtr >> 23  & 0x03 ;
    LOGCI("Read configuration the addr 0x{:X},reg_afe_ib_ctr2_1 0x{:X}",0xA4,reg_afe_ib_ctr2_1);
    if(reg_afe_ib_ctr2_1 != ENUM_POWER_CONSUMPTION_LOW &&  reg_afe_ib_ctr2_1 != ENUM_POWER_CONSUMPTION_HIGH)
    {
        LOGI("Read 0xA4 reg_afe_ib_ctr2_1 value is not reasonable!!!!!!");
        return false;
    }
    reg_afe_ib_ctr2_1 ==  ENUM_POWER_CONSUMPTION_LOW ? m_pLowPowerComsumpBtn->setChecked(true) : m_pHighPowerComsumpBtn->setChecked(true);
    return res;
}
bool ConfigurationWidgt::OnOtherWriteConf(void)
{
    bool res =false;
    uint regAFEintCtr = 0;
    if(!m_pUsbFtdDevice->ASICRegisterReadOne(0xA4,regAFEintCtr,res))
    {
        return res;
    }
    uint power_consumption_flag = m_pHighPowerComsumpBtn->isChecked() == true ? 1 : 0;
    regAFEintCtr &= ~(0x03 <<23); //清除bit 23-24
    regAFEintCtr |= power_consumption_flag << 23;  //设置bit 23
    res = m_pUsbFtdDevice->ASICRegisterWiteOne(0xA4,regAFEintCtr);
    LOGCI("Wirte configuration the addr 0x{:X},regAFEintCtr 0x{:X}",0xA4,regAFEintCtr);
    return res;
}


void ConfigurationWidgt::PermissionToModifyAdvanceSlot(const bool &checked)
{
    if(checked)
    {
        bool ok = false;
        QString text = QInputDialog::getText(this, QString::fromLocal8Bit("密码确认"), QString::fromLocal8Bit("请输入密码"), QLineEdit::Password, 0, &ok);
        if (ok)
        {
            if (QString::compare(text, QString::fromStdString(PROGRAME_PASSWORD)) == 0)
            {
                return;
            }
            else
            {
                m_pAdvanceSettingGrpbx->setChecked(false);
                QMessageBox::critical(this, "Warning ", tr("wrong password"), QMessageBox::Ok);
            }
        }
    }
}
