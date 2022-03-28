#include "controlpanel.h"
#include "ui_controlpanel.h"
#include "models/DataDefine.h"
#include "models/DataPackageMode.h"
#include "models/DatacCommunicaor.h"

#define g_pCommunicaor DataCommunicaor::Instance()

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);
    initView();
}

ControlPanel::~ControlPanel()
{
    if(SecTimer != nullptr && SecTimer->isActive())
        SecTimer->stop();
    if (StopSaveSecTimer != nullptr && StopSaveSecTimer->isActive())
        StopSaveSecTimer->stop();
    delete SecTimer;
    delete TimeRecord;
    delete m_pQProgressDialog;
    delete ui;
}

void ControlPanel::initView(void)
{
    QStringList sampModeList = {"SingelChannel","AllChannel"};
    ui->chSampModeComb->addItems(sampModeList);
    ui->chSampModeComb->setCurrentIndex(ALL_CAHNNEL_MODE);

    ui->chSampSlectSpinBox->setRange(1,CHANNEL_NUM);
    ui->chSampSlectSpinBox->setPrefix("CH ");

    ui->sampSpBox->setRange(1,100);
    ui->sampSpBox->setSuffix(" KHz");
    ui->sampSpBox->setSingleStep(1);
    ui->sampSpBox->setValue(20);

    m_pQProgressDialog = new QProgressDialog("Operation in progress.", "Cancel", 0, 100);
    m_pQProgressDialog->reset();
    m_pQProgressDialog->setModal(true);

    connect(ui->chSampModeComb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ControlPanel::commFreqModeChanged);//频率通道模式切换

    initTimerView();
    initStopSaveTimerView();
}



void ControlPanel::commFreqModeChanged(int index)
{
    bool bVisible = (index == SINGEL_CHANNEL_MODE); //当前模式是SINGEL_CHANNELL
    ui->chSampSlectLabel->setVisible(bVisible);
    ui->chSampSlectSpinBox->setVisible(bVisible);
    if(bVisible)
        ui->sampSpBox->setRange(1,100);
    else
         ui->sampSpBox->setRange(1,20);
}

void ControlPanel::on_sampApplyBtn_clicked()
{
    bool isAllChanel = (ui->chSampModeComb->currentIndex() == ALL_CAHNNEL_MODE ? true : false);
    int frequency = ui->sampSpBox->value()*1000;
    char channel = ui->chSampSlectSpinBox->value()-1;
    qDebug()<<" frequency is "<< frequency;
    QByteArray dataArray = ProtocolModule_SampSet(channel,frequency,isAllChanel);
    emit transmitAIData(dataArray);
    emit SendSampRateSig(frequency);
    qDebug()<<" data is "<< dataArray;

}


void ControlPanel::CancelZeroAdjust(void)
{
    g_pCommunicaor->g_bZeroAdjust = false;
    g_pCommunicaor->g_bZeroAdjustProcess = false;
    emit setZeroAdjustSig(ZERO_ADJUST_CANCEL);
}

void ControlPanel::on_zeroOffsetBtn_clicked()
{
    g_pCommunicaor->g_bZeroAdjustProcess = true;
    int i = 0;
    int backwardDurationTime = ui->zeroBackwardDurationDpbox->value();
    int zeroDurationTime     = ui->zeroAdustSecDpbox->value();
    int zeroAdjustTime       = ui->zeroAdjustTimeSpbox->value();
    int totalTime = backwardDurationTime + zeroDurationTime + zeroAdjustTime;
    double backwardVolt = ui->zeroBackwardVoltageDpbox->value();
    m_pQProgressDialog->setRange(0,totalTime);
    progressBarSet(i);
    emit setZeroAdjustSig(ZERO_ADJUST_SET_PORE_BACKWARD, backwardVolt);
    for (int v = 0; v < backwardDurationTime; v++)
    {
        if (m_pQProgressDialog->wasCanceled())
        {
            //qDebug() << "wasCanceled ";
            CancelZeroAdjust();
            m_pQProgressDialog->reset();
            return;
        }
        caliSleep(1000);
        progressBarSet(++i);
    }

    emit setZeroAdjustSig(ZERO_ADJUST_SET_ZERO_VOLT);
    for(int v = 0;v < zeroDurationTime; v++)
    {
        if(m_pQProgressDialog->wasCanceled())
        {
            //qDebug() << "wasCanceled ";
            CancelZeroAdjust();
            m_pQProgressDialog->reset();
            return;
        }
        caliSleep(1000);
        progressBarSet(++i);
    }

    emit setZeroAdjustSig(ZERO_ADJUST_START);
    for(int v = 0;v < zeroAdjustTime; v++)
    {
        if(m_pQProgressDialog->wasCanceled())
        {
            qDebug()<<"wasCanceled ";
            CancelZeroAdjust();
            m_pQProgressDialog->reset();
            return;
        }
        caliSleep(1000);
        progressBarSet(++i);
    }
    emit setZeroAdjustSig(ZERO_ADJUST_CANCEL);
}

void ControlPanel::progressBarSet(const int &value)
{
    m_pQProgressDialog->setValue(value);
}


void ControlPanel::caliSleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}



void ControlPanel::initTimerView(void)
{
    SecTimer = new QTimer;   //定时器 每秒更新时间
    TimeRecord = new QTime(0,5,0,0);  //记录时间
    ui->ZeroTimeDispLcd->setDigitCount(8);
    ui->ZeroTimeDispLcd->setSegmentStyle(QLCDNumber::Flat);
    ui->ZeroTimeDispLcd->display(TimeRecord->toString("hh:mm:ss"));

    ui->ZeroInputUnitComb->addItem(QStringLiteral("Seconds"), SECOND);
    ui->ZeroInputUnitComb->addItem(QStringLiteral("Minutes"), MINUTE);
    ui->ZeroInputUnitComb->setCurrentIndex(MINUTE);

    connect(SecTimer,SIGNAL(timeout()),this,SLOT(SecTimerUpdataTimeSlot()));
    connect(ui->ZeroInputUnitComb,SIGNAL(currentIndexChanged(int)),this,SLOT(SecTimerUnitChangeSlot(int)));
}


void ControlPanel::SecTimerUnitChangeSlot(int index)
{
    if(index == SECOND)
    {
        ui->ZeroInputUnitSpbox->setRange(6,99);
        ui->ZeroInputUnitSpbox->setValue(6);
    }
    else if(index == MINUTE)
    {
        ui->ZeroInputUnitSpbox->setRange(1,60);
        ui->ZeroInputUnitSpbox->setValue(1);
    }
}


void ControlPanel::SecTimerUpdataTimeSlot(void)
{
    *TimeRecord = TimeRecord->addSecs(-1);
    ui->ZeroTimeDispLcd->display(TimeRecord->toString("hh:mm:ss"));
    if(*TimeRecord == QTime(0,0,0,0))
    {
        if(ui->ZeroStartStopBtn->isChecked())
        {
            ui->ZeroStartStopBtn->click();
        }
        on_zeroOffsetBtn_clicked();
        on_ZeroRestartBtn_clicked();
        if(!ui->ZeroStartStopBtn->isChecked())
        {
             ui->ZeroStartStopBtn->click();
        }
    }
}


void ControlPanel::on_ZeroStartStopBtn_toggled(bool checked)
{
    if(checked)
    {
        on_ZeroRestartBtn_clicked();
        ui->ZeroStartStopBtn->setText("Stop");
        ui->ZeroInputUnitSpbox->setEnabled(false);
        ui->ZeroInputUnitComb->setEnabled(false);
        if(SecTimer != nullptr)
        {
             SecTimer->start(1000);
        }
    }
    else
    {
        ui->ZeroStartStopBtn->setText("Start");
        ui->ZeroInputUnitSpbox->setEnabled(true);
        ui->ZeroInputUnitComb->setEnabled(true);
        if(SecTimer != nullptr && SecTimer->isActive())
        {
            SecTimer->stop();
        }
    }
}


void ControlPanel::on_ZeroRestartBtn_clicked()
{
    if(ui->ZeroInputUnitComb->currentIndex() == SECOND)
    {
        TimeRecord->setHMS(0,0,ui->ZeroInputUnitSpbox->value(),0);
         ui->ZeroTimeDispLcd->display(TimeRecord->toString("hh:mm:ss"));
    }
    else
    {
         TimeRecord->setHMS(0,ui->ZeroInputUnitSpbox->value(),0,0);
         ui->ZeroTimeDispLcd->display(TimeRecord->toString("hh:mm:ss"));
    }

}

void ControlPanel::initStopSaveTimerView(void)
{
    StopSaveSecTimer = new QTimer;   //定时器 每秒更新时间
    StopSaveTimeRecord = new QTime(0, 5, 0, 0);  //记录时间
    ui->StopTimeDispLcd->setDigitCount(8);
    ui->StopTimeDispLcd->setSegmentStyle(QLCDNumber::Flat);
    ui->StopTimeDispLcd->display(TimeRecord->toString("hh:mm:ss"));

    ui->StopTimeInputUnitComb->addItem(QStringLiteral("Seconds"), SECOND);
    ui->StopTimeInputUnitComb->addItem(QStringLiteral("Minutes"), MINUTE);
    ui->StopTimeInputUnitComb->addItem(QStringLiteral("Hour"), HOUR);
    ui->StopTimeInputUnitComb->setCurrentIndex(MINUTE);

    connect(StopSaveSecTimer, SIGNAL(timeout()), this, SLOT(StopSaveSecTimerUpdataTimeSlot()));
    connect(ui->StopTimeInputUnitComb, SIGNAL(currentIndexChanged(int)), this, SLOT(StopSaveSecTimerUnitChangeSlot(int)));
}

void ControlPanel::StopSaveSecTimerUnitChangeSlot(int index)
{
    if (index == SECOND)
    {
        ui->StopTimeInputUnitSpbox->setRange(6, 99);
        ui->StopTimeInputUnitSpbox->setValue(6);
    }
    else if (index == MINUTE)
    {
        ui->StopTimeInputUnitSpbox->setRange(1, 60);
        ui->StopTimeInputUnitSpbox->setValue(1);
    }
    else if (index == HOUR)
    {
        ui->StopTimeInputUnitSpbox->setRange(1, 24);
        ui->StopTimeInputUnitSpbox->setValue(1);
    }
}


void ControlPanel::StopSaveSecTimerUpdataTimeSlot(void)
{
    *StopSaveTimeRecord = StopSaveTimeRecord->addSecs(-1);
    ui->StopTimeDispLcd->display(StopSaveTimeRecord->toString("hh:mm:ss"));
    if (*StopSaveTimeRecord == QTime(0, 0, 0, 0))
    {
        if (ui->StopTimeStartStopBtn->isChecked())
        {
            ui->StopTimeStartStopBtn->click();
        }
        on_StopTimeRestartBtn_clicked();
        emit StopRunningAndSaveSig();
    }
}

void ControlPanel::on_StopTimeStartStopBtn_toggled(bool checked)
{
    if (checked)
    {
        on_StopTimeRestartBtn_clicked();
        ui->StopTimeStartStopBtn->setText("Stop");
        ui->StopTimeInputUnitSpbox->setEnabled(false);
        ui->StopTimeInputUnitComb->setEnabled(false);
        if (StopSaveSecTimer != nullptr)
        {
            StopSaveSecTimer->start(1000);
        }
    }
    else
    {
        ui->StopTimeStartStopBtn->setText("Start");
        ui->StopTimeInputUnitSpbox->setEnabled(true);
        ui->StopTimeInputUnitComb->setEnabled(true);
        if (StopSaveSecTimer != nullptr && StopSaveSecTimer->isActive())
        {
            StopSaveSecTimer->stop();
        }
    }
}

void ControlPanel::on_StopTimeRestartBtn_clicked()
{
    if (ui->StopTimeInputUnitComb->currentIndex() == SECOND)
    {
        StopSaveTimeRecord->setHMS(0, 0, ui->StopTimeInputUnitSpbox->value(), 0);
        ui->StopTimeDispLcd->display(StopSaveTimeRecord->toString("hh:mm:ss"));
    }
    else if(ui->StopTimeInputUnitComb->currentIndex() == MINUTE)
    {
        StopSaveTimeRecord->setHMS(0, ui->StopTimeInputUnitSpbox->value(), 0, 0);
        ui->StopTimeDispLcd->display(StopSaveTimeRecord->toString("hh:mm:ss"));
    }
    else
    {
    StopSaveTimeRecord->setHMS(ui->StopTimeInputUnitSpbox->value(), 0, 0, 0);
    ui->StopTimeDispLcd->display(StopSaveTimeRecord->toString("hh:mm:ss"));
    }

}
