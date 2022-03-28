#include "singlechcomb.h"
#include "ui_singlechcomb.h"
#include "models/DataDefine.h"
#include"models/DatacCommunicaor.h"

#define g_pCommunicaor DataCommunicaor::Instance()

SingleChComb::SingleChComb(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleChComb)
{
    ui->setupUi(this);
    initConnect();
    m_bOriColor = NODEGATE_GREEN;
    m_bPoreState = NOT_SINGLE_PORE;
}

SingleChComb::~SingleChComb()
{
    if(m_degatDurationTimer != nullptr)
    {
        if(m_degatDurationTimer->isActive())
            m_degatDurationTimer->stop();
        m_degatDurationTimer->deleteLater();

    }
   if(m_degatChkDutyTimer !=  nullptr)
   {
       if(m_degatChkDutyTimer->isActive())
           m_degatChkDutyTimer->stop();
       m_degatChkDutyTimer->deleteLater();
   }
    delete ui;
}

void SingleChComb::initDegat(const double &degatVolt,const int &degatDurationTime, const int &degatCheckDutyTime)
{
    m_degatVolt              = degatVolt;
    m_degatDurationTime  = degatDurationTime;
    m_degatCheckDutyTime = degatCheckDutyTime * 1000;


    m_degatDurationTimer = new QTimer;
    m_degatDurationTimer->setSingleShot(true);
    connect(m_degatDurationTimer,SIGNAL(timeout()),this,SLOT(degatDurationTimeOut()));

    m_degatChkDutyTimer =new QTimer;
    m_degatChkDutyTimer->setSingleShot(false);
    connect(m_degatChkDutyTimer,SIGNAL(timeout()),this,SLOT(degatCheckDutyTimeOut()));

    ui->degatColorBtn->setStyleSheet(m_sheetStyle + "green");

    ui->nPoreStateBtn->setStyleSheet(notSinglePoreStyle);
}

void SingleChComb::setDegatParam(const double &degatVolt,const int &degatDurationTime, const int &degatCheckDutyTime)
{
    m_degatVolt          = degatVolt;
    m_degatDurationTime  = degatDurationTime;
    m_degatCheckDutyTime = degatCheckDutyTime * 1000;
}


void SingleChComb::init(int &index)
{
    ui->singleVoltChk->setText(QString("CH%1").arg(index+1));
    setChannel(index);
}


/**
* @brief            组合框的值变化时相关操作
* @return           void
*/
void SingleChComb::initConnect(void)
{
    connect(ui->applyBtn,SIGNAL(clicked()),this,SLOT(applyBtnClick()));
    connect(ui->ampPlusOrMinusBtn,SIGNAL(clicked(bool)),this,SLOT(ampPlusBtnClick()));
}



void SingleChComb::applyBtnClick(void)
{
    emit SingelChanged(this);
}

void SingleChComb::ampPlusBtnClick(void)
{
    double nVolt = ui->ampDpBox->value();
    ui->ampDpBox->setValue(-nVolt);
    emit SingelChanged(this);
}

float SingleChComb:: getVoltSet(void)const
{
    return ui->ampDpBox->value();
}

void SingleChComb::setVolt(double value)
{
    ui->ampDpBox->setValue(value);
}


bool SingleChComb::isAutoChecked(void)const
{
    return ui->degatAutoChk->isChecked();
}

void SingleChComb::setAutoCheck(const bool &check)
{
    ui->degatAutoChk->setChecked(check);
}

bool SingleChComb::isSingleVoltChecked(void)const
{
    return ui->singleVoltChk->isChecked();
}

void SingleChComb::setSingleVoltCheck(const bool &check)
{
    ui->singleVoltChk->setChecked(check);
}



void SingleChComb::setAutoCheckEnable(const bool &state)const
{
    ui->degatAutoChk->setEnabled(state);
}

void SingleChComb::setDegateColor(bool state)
{
    if(m_bOriColor != state)
    {
        ui->degatColorBtn->setStyleSheet(m_sheetStyle + (state == false ? "green" : "red"));
        m_bOriColor =state;
    }
}


void SingleChComb::degatDurationTimeOut(void)
{
    int mode = NORMAL;
    emit SingelDegateItemChanged(m_channel,mode,m_degatVolt);
}



void SingleChComb::degatCheckDutyTimeOut(void)
{
    if(!m_degatDurationTimer->isActive())
    {
        if(m_bOriColor) //先判断是否是红色Degating 再动作；
        {
            if(g_pCommunicaor->g_bZeroAdjustProcess  != true) //调零跟degating冲突
            {
                on_degatColorBtn_clicked();
            }
        }
    }
}


void SingleChComb::on_degatColorBtn_clicked()
{
    if(!m_degatDurationTimer->isActive())
    {
        int mode = DEGATTING;
        emit SingelDegateItemChanged(m_channel,mode,m_degatVolt);
        m_degatDurationTimer->setInterval(m_degatDurationTime);
        m_degatDurationTimer->start();
    }
}

void SingleChComb::on_degatAutoChk_toggled(bool checked)
{
    if(checked)
    {
        m_degatChkDutyTimer->setInterval(m_degatCheckDutyTime);
        m_degatChkDutyTimer->start();
    }
    else
    {
        if(m_degatChkDutyTimer->isActive())
            m_degatChkDutyTimer->stop();
    }
}


void SingleChComb::on_nPoreStateBtn_toggled(bool checked)
{
    if(checked)
    {
        ui->nPoreStateBtn->setStyleSheet(SinglePoreStyle);
        ui->singleVoltChk->setChecked(true);
        m_bPoreState = true;
    }
    else
    {
        ui->nPoreStateBtn->setStyleSheet(notSinglePoreStyle);
        ui->singleVoltChk->setChecked(false);
        m_bPoreState = false;
    }

}
