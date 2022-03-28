#include "calibrationdialog.h"
#include "ui_calibrationdialog.h"
#include "models/DataDefine.h"
#include "models/DataPackageMode.h"
#include "models/DatacCommunicaor.h"
#include <QLineEdit>
#include <QString>
#include <QSettings>
#include "helper/appconfig.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>
#include <QAction>
#include <QTime>
#include "helper/Point.h"
#include "logger/Log.h"

#define g_pCommunicaor DataCommunicaor::Instance()

Calibrationdialog::Calibrationdialog(QAction *actionStart,QAction *actionStop,QAction *actionClear, QWidget *parent):
    m_StartActon(actionStart),
    m_StopActon(actionStop),
    m_ClearAction(actionClear),
    QDialog(parent),
    ui(new Ui::Calibrationdialog)
{
    // 不显示问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);
    ui->setupUi(this);

    //初始化界面
    initViewer();
    initConnect();
    m_currentVolt = 0;
    m_nVctPoints.resize(CHANNEL_NUM);

    setAttribute(Qt::WA_WState_WindowOpacitySet);
    this->setWindowTitle(tr("calibration current"));

}

Calibrationdialog::~Calibrationdialog()
{
    delete ui;
}

void Calibrationdialog::initViewer(void)
{
    //设置最大加载电压
    ui->maxVolDoubSpin->setRange(0, 1);
    ui->maxVolDoubSpin->setSingleStep(0.01);
    ui->maxVolDoubSpin->setValue(1);
    ui->maxVolDoubSpin->setDecimals(2);
    ui->maxVolDoubSpin->setSuffix(" V");

    //显示当前开始加载电压
    ui->currentVolDoubSpin->setRange(0, 0.2);
    ui->currentVolDoubSpin->setSingleStep(0.01);
    ui->currentVolDoubSpin->setValue(0.00);
    ui->currentVolDoubSpin->setDecimals(2);
    ui->currentVolDoubSpin->setSuffix(" V");

    //设置步进电压
    ui->stepVolDoubSpin->setRange(0, 0.1);
    ui->stepVolDoubSpin->setSingleStep(0.01);
    ui->stepVolDoubSpin->setValue(0.01);
    ui->stepVolDoubSpin->setDecimals(2);
    ui->stepVolDoubSpin->setSuffix(" V");


    //初始化tableWidget界面
    initTable();

    m_isItemStateChange = false;

}

void Calibrationdialog::initTable(void)
{
    QStringList strListHorHeader = QStringList()<<"Channel"<<"Resistence"<<"Slope"<<"Offset"<<"Formula";
    ui->calibtabWidget->setColumnCount(strListHorHeader.size());
    ui->calibtabWidget->setRowCount(CHANNEL_NUM);
    ui->calibtabWidget->setHorizontalHeaderLabels(strListHorHeader);
    ui->calibtabWidget->setColumnWidth(4,250);

    //设置通道序号
    for(int i = 0; i < CHANNEL_NUM; i++ )
    {
        QTableWidgetItem *item_chNum = new QTableWidgetItem(QString("ch%1").arg(i+1));
        item_chNum->setBackground(QBrush(QColor(Qt::white)));
        item_chNum->setFlags(item_chNum->flags() & (~Qt::ItemIsEditable));
        ui->calibtabWidget->setItem(i, 0, item_chNum);
    }

    //设置电阻输入框只能输入double值
    QDoubleValidator* resInputValidator = new QDoubleValidator(0, 100000, 3, this);
    for(int i = 0; i < CHANNEL_NUM; i++ )
    {
        QLineEdit * ReslineEdit = new QLineEdit;
        ReslineEdit->setPlaceholderText("Resistence Value");
        ReslineEdit->setValidator(resInputValidator);
        ui->calibtabWidget->setCellWidget(i,1,ReslineEdit);
    }

     //设置slope 和offset 和 formula
    for(int i = 0; i < CHANNEL_NUM; i++ )
    {
        QTableWidgetItem *item_chSlope = new QTableWidgetItem("0");
        item_chSlope->setBackground(QBrush(QColor(Qt::lightGray)));
//        item_chSlope->setFlags(item_chSlope->flags() & (~Qt::ItemIsEditable));
         item_chSlope->setFlags(item_chSlope->flags() | (Qt::ItemIsEditable));
        ui->calibtabWidget->setItem(i, 2, item_chSlope);

         QTableWidgetItem *item_chOffset = new QTableWidgetItem("0");
        item_chOffset->setBackground(QBrush(QColor(Qt::lightGray)));
//        item_chOffset->setFlags(item_chOffset->flags() & (~Qt::ItemIsEditable));
        item_chOffset->setFlags(item_chOffset->flags() | (Qt::ItemIsEditable));
        ui->calibtabWidget->setItem(i, 3, item_chOffset);

        QTableWidgetItem *item_formula = new QTableWidgetItem("");
       item_formula->setBackground(QBrush(QColor(Qt::lightGray)));
       item_formula->setFlags(item_formula->flags() & (~Qt::ItemIsEditable));
       ui->calibtabWidget->setItem(i, 4, item_formula);
    }

}

void Calibrationdialog::initConnect(void)
{
    //按钮信号槽
    connect(ui->autoCalibraBtn,&QPushButton::clicked,this,&Calibrationdialog::autoCaliburationBtnClicked);
    connect(ui->saveChangeBtn,&QPushButton::clicked,this,&Calibrationdialog::onSaveChangeBtnClicked);
    connect(ui->cancelBtn,&QPushButton::clicked,this,&Calibrationdialog::onCancelBtnClicked);
    connect(ui->clearChangeBtn,&QPushButton::clicked,this,&Calibrationdialog::onClearAllDataBtnClicked);

    //输入变化信号槽
    connect(ui->maxVolDoubSpin, static_cast<void (QDoubleSpinBox::*)(double)>
        (&QDoubleSpinBox::valueChanged), this, &Calibrationdialog::somethingChanged);
    connect(ui->currentVolDoubSpin, static_cast<void (QDoubleSpinBox::*)(double)>
        (&QDoubleSpinBox::valueChanged), this, &Calibrationdialog::somethingChanged);
    connect(ui->stepVolDoubSpin, static_cast<void (QDoubleSpinBox::*)(double)>
        (&QDoubleSpinBox::valueChanged), this, &Calibrationdialog::somethingChanged);

    // 是否有修改表值
    connect(ui->calibtabWidget, SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(somethingChanged()));

    // 是否有修改电阻值
    for(int i = 0; i < CHANNEL_NUM; i++ )
    {
        connect(static_cast<QLineEdit*>(ui->calibtabWidget->cellWidget(i,1)),
                &QLineEdit::textChanged,this,&Calibrationdialog::somethingChanged);
    }
}

void Calibrationdialog::loadConfig(QSettings *config)
{
    if(!config)
    {
        qDebug()<<"CalibrationDialog config is nullptr";
        return;
    }
    m_config = config;  //保存config指针;

    config->beginGroup("Calibration");
    // load voltages settings
    ui->maxVolDoubSpin->setValue(config->value("MaxLoadVoltage").toDouble() == 0 ? 0.19 : config->value("MaxLoadVoltage").toDouble());
    ui->currentVolDoubSpin->setValue(config->value("CurrentLoadVoltage").toDouble() == 0 ? 0 : config->value("CurrentLoadVoltage").toDouble());
    ui->stepVolDoubSpin->setValue(config->value("StepIncVoltage").toDouble() == 0 ? 0.01 : config->value("StepIncVoltage").toDouble());

    // load channels settings
    config->beginReadArray("Channels");
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        config->setArrayIndex(i);
        static_cast<QLineEdit*>(ui->calibtabWidget->cellWidget(i,1))->setText(config->value("Resistence").toString());
        ui->calibtabWidget->item(i,2)->setData(Qt::EditRole,config->value("Slop"));
        ui->calibtabWidget->item(i,3)->setData(Qt::EditRole,config->value("Offset"));
        ui->calibtabWidget->item(i,4)->setData(Qt::EditRole,config->value("Formula"));
        m_nCalibraionStrArrayp[i].resistence = config->value("Resistence").toDouble();
        m_nCalibraionStrArrayp[i].offset = config->value("Offset").toDouble();
        m_nCalibraionStrArrayp[i].slope = config->value("Slop").toDouble();
        m_nCalibraionStrArrayp[i].formula = config->value("Formula").toString();
    }
    config->endArray();
    config->endGroup();

    clearSomeChangeState();
}

void Calibrationdialog::saveConfig(QSettings *config)
{
    if(!config)
    {
        qDebug()<<"CalibrationDialog config is nullptr";
        return;
    }
    config->beginGroup("Calibration");
    config->setValue("MaxLoadVoltage", QVariant(ui->maxVolDoubSpin->value()));
    config->setValue("CurrentLoadVoltage", QVariant(ui->currentVolDoubSpin->value()));
    config->setValue("StepIncVoltage", QVariant(ui->stepVolDoubSpin->value()));
    // save channels settings
    config->beginWriteArray("Channels");
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        config->setArrayIndex(i);
        config->setValue("Resistence", static_cast<QLineEdit*>(ui->calibtabWidget->cellWidget(i,1))->text());
        config->setValue("Slop", ui->calibtabWidget->item(i,2)->data(Qt::DisplayRole));
        config->setValue("Offset",  ui->calibtabWidget->item(i,3)->data(Qt::DisplayRole));
        config->setValue("Formula",  ui->calibtabWidget->item(i,4)->data(Qt::DisplayRole));
    }
    config->endArray();
    config->endGroup();

}

void Calibrationdialog::SaveCaliToConfig(QSettings * nConfig)
{
    nConfig->beginGroup("Calibration");

    // load channels settings
    nConfig->beginReadArray("Channels");
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        nConfig->setArrayIndex(i);
        nConfig->setValue("Slop", m_nCalibraionStrArrayp[i].slope);
        nConfig->setValue("Offset",  m_nCalibraionStrArrayp[i].offset);
        nConfig->setValue("Formula",  m_nCalibraionStrArrayp[i].formula);
    }
    nConfig->endArray();
    nConfig->endGroup();
}


void Calibrationdialog::clearAllValue(void)
{
    for(int i = 0;i < CHANNEL_NUM;i++)
    {
//        static_cast<QLineEdit*>(ui->calibtabWidget->cellWidget(i,1))->setText("");
        ui->calibtabWidget->item(i,2)->setData(Qt::EditRole,"");
        ui->calibtabWidget->item(i,3)->setData(Qt::EditRole,"");
        ui->calibtabWidget->item(i,4)->setData(Qt::EditRole,"");
    }
}

void Calibrationdialog::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        saveConfig(m_config);
        event->accept();
        emit CalibrationFinished(true);
    }
    m_isItemStateChange = false;
}

bool Calibrationdialog::okToContinue()
{
//    qDebug()<<"m_isItemStateChange"<<m_isItemStateChange;
    if (m_isItemStateChange) {
        int response = QMessageBox::warning(this, tr("Spreadsheet"),
                        tr("The document has been modified.\n"
                           "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::No);
        if (response == QMessageBox::Yes) {
            return true;
        }else
            return false;
    }
    return false;
}

void Calibrationdialog::somethingChanged()
{
     m_isItemStateChange = true;
}


void Calibrationdialog::onCancelBtnClicked()
{
    m_isItemStateChange = false;
    this->close();
}

void Calibrationdialog::onClearAllDataBtnClicked()
{
    clearAllValue();
}

void Calibrationdialog::onSaveChangeBtnClicked()
{
    saveConfig(m_config);
    loadConfig(m_config);
    this->close();
    emit CalibrationFinished(true);
}

void Calibrationdialog::autoCaliburationBtnClicked()
{
    saveConfig(m_config);
    loadConfig(m_config);
    ui->autoCalibraBtn->setEnabled(false);

    bool bCalibration = false;

    g_pCommunicaor->g_bCalibration = true;
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        LOGI("Channel {} resistance {}",ch+1,m_nCalibraionStrArrayp[ch].resistence);
    }

    m_currentVolt= ui->currentVolDoubSpin->value();
    double maxVolt     = ui->maxVolDoubSpin->value();
    double voltStep    = ui->stepVolDoubSpin->value();
    int    counts      = (maxVolt-m_currentVolt)/voltStep;
//    QApplication::setOverrideCursor(Qt::WaitCursor);//设置鼠标为等待状态
    QProgressDialog progress;
    progress.setWindowTitle("Notice");
    progress.setLabelText("Loadding...");
    progress.setCancelButtonText("Cancel");
    progress.setModal(true);//设置为模态对话框
    progress.show();
    progress.setValue(0);
//    QCoreApplication::processEvents();
    progress.setRange(0, counts+1);//设置范围
    m_StopActon->trigger();  //先停止先
    caliSleep(2000);
    meansResultBufClear(); //先清楚之前的数据；
    for(int i = 0; i <= counts; i++)
    {
        if(progress.wasCanceled())
        {
            bCalibration =false;
            break;
        }
         emit caliDirectVoltSetSig(m_currentVolt);
         m_ClearAction->trigger();
         m_StartActon->trigger();
         progress.setValue(i);
         caliSleep(3000);
         m_StopActon->trigger();
         caliSleep(2000);
         m_currentVolt += voltStep;
         bCalibration =true;
    }

    if(bCalibration)
    {
          GetAllLineLeastSquare();
          progress.setValue(counts+1);
          SaveCaliToConfig(m_config);
          loadConfig(m_config);
          emit CalibrationFinished(true);
          QMessageBox::information(this, tr("Info"),tr("Calibration Success"));
    }
    else
    {
          QMessageBox::information(this, tr("Failed"),tr("Calibration failed"));
    }

//    QApplication::setOverrideCursor(Qt::ArrowCursor);
    g_pCommunicaor->g_bCalibration = false;
    ui->autoCalibraBtn->setEnabled(true);
}


void Calibrationdialog::caliSleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

}


void Calibrationdialog::on_loadConfigBtn_clicked()
{
   QString fileName;
   fileName = QFileDialog::getOpenFileName(this,"Open File","","config.ini(*.ini)");
   if(fileName == "")
   {
       return ;
   }
   else
   {
       QSettings caliFileIni(fileName, QSettings::IniFormat);
       loadConfig(&caliFileIni);
   }
}

void Calibrationdialog::meansResultSlot(QVector<float> result)
{
//    qDebug()<<"meansResultSlot"<<m_currentVolt;
    for(int ch = 0; ch < result.size(); ch++)
    {
        float nX = m_currentVolt/m_nCalibraionStrArrayp[ch].resistence*1000000;
        float nY = result.at(ch);
        LOGI("Channnel {} Point(x,y) {} {}",ch,nX,nY);
        m_nVctPoints[ch].push_back(Point(nX,nY));
    }
}

void Calibrationdialog::meansResultBufClear(void)
{
    for(int i = 0; i<m_nVctPoints.size(); i++)
    {
        m_nVctPoints[i].clear();
    }
}

void Calibrationdialog::GetAllLineLeastSquare(void)
{

    for(int ch = 0; ch < m_nVctPoints.size(); ch++)
    {
        Module_GetLineFit(m_nVctPoints.at(ch),&m_nCalibraionStrArrayp[ch].slope,
                          &m_nCalibraionStrArrayp[ch].offset,&m_nCalibraionStrArrayp[ch].formula);
        qDebug()<<ch<<m_nCalibraionStrArrayp[ch].formula;
        qDebug()<<ch<<m_nVctPoints[ch];
    }
    meansResultBufClear();
}

void Calibrationdialog::on_resistAllSetBtn_clicked()
{
    float resValue = ui->resistAllSetDpbox->value();
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        static_cast<QLineEdit*>(ui->calibtabWidget->cellWidget(i,1))->setText(QString::number(resValue,'f',2));
    }

}

void Calibrationdialog::on_slopeOffsetResetBtn_clicked()
{
    double slope = ui->slopeAllsetDpbox->value();
    double offset= ui->offsetAllsetDpbox->value();
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        ui->calibtabWidget->item(i,2)->setData(Qt::EditRole,slope);
        ui->calibtabWidget->item(i,3)->setData(Qt::EditRole,offset);
        QString Formula = QString("y = %1x + %2").arg(slope,0,'g',6).arg(offset,0,'g',6);
        ui->calibtabWidget->item(i,4)->setData(Qt::EditRole,Formula);
    }
}
