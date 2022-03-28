#include "currentsingelsettingpage.h"
#include "ui_currentsingelsettingpage.h"
#include "singlechcomb.h"
#include "models/DataDefine.h"
#include "models/DataPackageMode.h"
#include "models/DatacCommunicaor.h"
#include "helper/messagedialog.h"
#include "view/degatparamsdialog.h"

CurrentSingelSettingPage::CurrentSingelSettingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentSingelSettingPage)
{
    ui->setupUi(this);
    initViewer();
    initConnect();


    m_pDegatParamsDialog = new DegatParamsDialog(this);
    connect(m_pDegatParamsDialog,SIGNAL(setDegatingParameter(double,int,int,int,int,int)),this,SIGNAL(setDegatingParameter(double,int,int,int,int,int)));

    double degatVolt = ui->degatVoltDpbox->value();
    int degatDurationTime = ui->degatDurationTimeSpbox->value();
    int degatCheckDutyTime = ui->checkDutyTimeSpbox->value();

    m_vctSingleChComb.resize(CHANNEL_NUM);
    for (int i = 0; i < CHANNEL_NUM; i++) {

        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
//        item->setSizeHint(QSize(60, 23));
        m_vctSingleChComb[i] = std::make_shared<SingleChComb>();
        m_vctSingleChComb[i]->init(i);
        m_vctSingleChComb[i]->initDegat(degatVolt,degatDurationTime, degatCheckDutyTime);
        ui->listWidget->setItemWidget(item,m_vctSingleChComb[i].get());
        connect(m_vctSingleChComb[i].get(),&SingleChComb::SingelChanged,this,&CurrentSingelSettingPage::SingelItemSlot);
        connect(m_vctSingleChComb[i].get(),&SingleChComb::SingelDegateItemChanged,this,&CurrentSingelSettingPage::SingelDegatItemSlot);
    }

    memset(&m_polymerVoltParam,0,sizeof(m_polymerVoltParam));
    connect(&m_polyVoltCtrTimer,SIGNAL(timeout()),this,SLOT(PolymerVoltCtrolTimeoutSlot()));


    m_bTriangularDirection = TRIANGULAR_RISE;
    memset(&m_triangularPulseSimulateParam,0,sizeof(m_triangularPulseSimulateParam));
    connect(&m_triangularPulseSimulateTimer,SIGNAL(timeout()),this,SLOT(TrianPulseVoltCtrolTimeoutSlot()));

    this->setAttribute(Qt::WA_Hover, true);
}

CurrentSingelSettingPage::~CurrentSingelSettingPage()
{
    delete m_pDegatParamsDialog;
    delete ui;
}

void CurrentSingelSettingPage::LoadConfig(QSettings *config)
{
    config->beginGroup("PolymerModule");
    ui->polyCurrentlimitdpbox->setValue(config->value("PolyCurrentLimit").toDouble());
    ui->polyVoltSetDpbox->setValue(config->value("PolyVoltSet").toDouble());
    ui->polyVoltstartdpbox->setValue(config->value("PolyVoltstart").toDouble());
    ui->polyVoltEndDpbox->setValue(config->value("PolyVoltEnd").toDouble());
    ui->polyVoltStepdpbox->setValue(config->value("PolyVoltStep").toDouble());
    ui->polyStepTimespbox->setValue(config->value("PolyStepTimes").toDouble());
    config->endGroup();

    config->beginGroup("TriangularPulseModule");
    ui->TrianPuseVoltStartDpbox->setValue(config->value("TrianPuseVoltStart").toDouble());
    ui->TrianPuseVoltEndDpbox->setValue(config->value("TrianPuseVoltEnd").toDouble());
    ui->TrianPuseVoltStepDpbox->setValue(config->value("TrianPuseVoltStep").toDouble());
    ui->TrianPuseVoltStepTimeDpbox->setValue(config->value("TrianPuseVoltStepTime").toDouble());
    config->endGroup();

}

void CurrentSingelSettingPage::SaveConfig(QSettings *config)
{
    config->beginGroup("PolymerModule");
    config->setValue("PolyCurrentLimit",  ui->polyCurrentlimitdpbox->value());
    config->setValue("PolyVoltSet",  ui->polyVoltSetDpbox->value());
    config->setValue("PolyVoltstart",  ui->polyVoltstartdpbox->value());
    config->setValue("PolyVoltEnd",  ui->polyVoltEndDpbox->value());
    config->setValue("PolyVoltStep",  ui->polyVoltStepdpbox->value());
    config->setValue("PolyStepTimes", ui->polyStepTimespbox->value());
    config->endGroup();

    config->beginGroup("TriangularPulseModule");
    config->setValue("TrianPuseVoltStart",  ui->TrianPuseVoltStartDpbox->value());
    config->setValue("TrianPuseVoltEnd",  ui->TrianPuseVoltEndDpbox->value());
    config->setValue("TrianPuseVoltStep",  ui->TrianPuseVoltStepDpbox->value());
    config->setValue("TrianPuseVoltStepTime",  ui->TrianPuseVoltStepTimeDpbox->value());
    config->endGroup();
}



void CurrentSingelSettingPage::SingelItemSlot(SingleChComb *item)
{
    ushort amplitude = doubleTransformToDacHex(item->getVoltSet());
    QByteArray data = ProtocolModule_DiectCurrentSet(item->channel(),amplitude,SINGELCHANNEL);
    emit transmitData(data);
}


void CurrentSingelSettingPage::SingelDegatItemSlot(int channel,int mode, double Volt)
{
    if(mode == DEGATTING)
    {
        ushort amplitude = doubleTransformToDacHex(Volt);
        QByteArray data = ProtocolModule_DiectCurrentSet(channel,amplitude,SINGELCHANNEL);
        emit transmitData(data);


    }
    else if(mode == NORMAL)
    {
        setDegatCurrentDirectVolt(channel);
    }
}

void CurrentSingelSettingPage::caliSleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}


void CurrentSingelSettingPage::initViewer(void)
{
    //初始化电平模式选项设置
    QStringList voltModeList = {"Direct","Impulse","Square","Triangular"};
    ui->nVoltTypeComb->addItems(voltModeList);
    ui->nVoltTypeComb->setCurrentIndex(DIRECT_CURRENT);
    ui->nVoltFreqLabel->setVisible(false);
    ui->nVoltFreqSpinBox->setVisible(false);

    ui->nSingleVoltTypeComb->addItem("Direct");
}


void CurrentSingelSettingPage::initConnect(void)
{
    connect(ui->nVoltTypeComb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CurrentSingelSettingPage::commVoltTypeChanged);//电压控制模式切换
    connect(ui->nPosOrNegBtn, SIGNAL(clicked()),this,SLOT(onPosOrNegBtnClicked()));
    connect(ui->nVoltAPpplyBtn, SIGNAL(clicked()),this,SLOT(onVoltAPpplyBtnClicked()));

}


void CurrentSingelSettingPage::onPosOrNegBtnClicked(void)
{
    ui->nVoltAmpDoubBox->setValue(-ui->nVoltAmpDoubBox->value());
    onVoltAPpplyBtnClicked();
}

void CurrentSingelSettingPage::commVoltTypeChanged(int index)
{
    bool bDirect = (index == DIRECT_CURRENT);
    bool bVisible = (index == SQUARE_VOLTAGE || index == TRIANGULAR_VOLTAGE); //当前模式是方波或者三角波
    ui->nVoltFreqLabel->setVisible(bVisible);
    ui->nVoltFreqSpinBox->setVisible(bVisible);
    ui->nPosOrNegBtn->setVisible(bDirect);

    //脉冲电压限幅正负5V，其他量程限幅正负2.5V
    if(index == IMPULSE_VOLTAGE)
         ui->nVoltAmpDoubBox->setRange(-4,4);
    else ui->nVoltAmpDoubBox->setRange(-2.5,2.5);

}


void CurrentSingelSettingPage::onVoltAPpplyBtnClicked()
{
    int channel  = CHANNEL_NUM;
    double volt = ui->nVoltAmpDoubBox->value();
    ushort amplitude = doubleTransformToDacHex(volt);
    qDebug()<<" amplitude is "<< ui->nVoltAmpDoubBox->value() <<"doubleTransformToDacHex is"<<doubleTransformToDacHex(ui->nVoltAmpDoubBox->value());
    int voltfreq = ui->nVoltFreqSpinBox->value();
    int channelMode = ALL_CAHNNEL_MODE;
    int voltageMode = ui->nVoltTypeComb->currentIndex();
    QByteArray dataArray =  EnCode(channelMode,channel,amplitude,voltageMode,voltfreq);
    if(channelMode == ALL_CAHNNEL_MODE && voltageMode  == DIRECT_CURRENT)
    {
        for(int ch = 0; ch < CHANNEL_NUM; ch++)
        {
            m_vctSingleChComb[ch]->setVolt(volt);
        }
    }
    else if(channelMode == ALL_CAHNNEL_MODE && voltageMode == TRIANGULAR_VOLTAGE)
    {
        g_pCommunicaor->g_capParameter.CapMutex.lockForWrite();
        g_pCommunicaor->g_capParameter.triangleAmp  = ui->nVoltAmpDoubBox->value();
        g_pCommunicaor->g_capParameter.triangleFreq = ui->nVoltFreqSpinBox->value();
        g_pCommunicaor->g_capParameter.CapMutex.unlock();
    }
    emit transmitData(dataArray);
}


QByteArray CurrentSingelSettingPage::EnCode(int channelMode,ushort channel,ushort amplitude,\
                                        int voltageMode,int voltfreq)
{
    bool isAllChanel = (channelMode == ALL_CAHNNEL_MODE ? true : false);
    switch(voltageMode)
    {
        case DIRECT_CURRENT:
            return ProtocolModule_DiectCurrentSet(channel,amplitude,isAllChanel);
          break;
        case IMPULSE_VOLTAGE:
            return ProtocolModule_PulseSet(channel,amplitude,isAllChanel);
          break;
        case SQUARE_VOLTAGE:
            return ProtocolModule_SquareSet(channel,amplitude,voltfreq);
           break;
        case TRIANGULAR_VOLTAGE:
            return ProtocolModule_TriangularSet(channel,amplitude,voltfreq);
          break;
    }
    return "";
}

void CurrentSingelSettingPage::SetZeroAdjustSlot(int mode,const double backVolt)
{
    if(mode == ZERO_ADJUST_START)
    {
        g_pCommunicaor->g_bZeroAdjust = true;
    }
    else if(mode  == ZERO_ADJUST_CANCEL)
    {
        g_pCommunicaor->g_bZeroAdjust = false;
        g_pCommunicaor->g_bZeroAdjustProcess = false;
        for(int ch = 0; ch < CHANNEL_NUM; ch++)
        {
            m_vctSingleChComb[ch]->applyBtnClick();
        }
    }
    else if(mode  == ZERO_ADJUST_SET_ZERO_VOLT)
    {
        int channel = CHANNEL_NUM;
        ushort amplitude = doubleTransformToDacHex(0.0);
        QByteArray data = ProtocolModule_DiectCurrentSet(channel,amplitude,ALLCHANNEL);
        emit transmitData(data);
    }
    else if(mode == ZERO_ADJUST_SET_PORE_BACKWARD)
    {
        for(int ch = 0; ch < CHANNEL_NUM; ch++)
        {
            if(m_vctSingleChComb[ch]->getPoreState())
            {
                ushort amplitude = doubleTransformToDacHex(backVolt);
                QByteArray data = ProtocolModule_DiectCurrentSet(ch,amplitude,SINGELCHANNEL);
                emit transmitData(data);
            }
        }
    }

}

void CurrentSingelSettingPage::setSpecChanelDirect(int &channel,const float &volt)
{
    ushort amplitude = doubleTransformToDacHex(volt);
    QByteArray data = ProtocolModule_DiectCurrentSet(channel,amplitude,SINGELCHANNEL);
    m_vctSingleChComb[channel].get()->setVolt(volt);
    emit transmitData(data);
}

void CurrentSingelSettingPage::setAllChannelDirect(float &volt)
{
    ui->nVoltAmpDoubBox->setValue(volt);
    ui->nVoltTypeComb->setCurrentIndex(DIRECT_CURRENT);
    onVoltAPpplyBtnClicked();
}

void CurrentSingelSettingPage::setDegatCurrentDirectVolt(int &channel)
{
    float curVolt = m_vctSingleChComb[channel].get()->getVoltSet();
    setSpecChanelDirect(channel,curVolt);
}

void CurrentSingelSettingPage::setListItemVisiableSlot(QVector<bool> listVisiable)
{
    for(int ch = 0; ch < listVisiable.size(); ch++)
    {
        ui->listWidget->item(ch)->setHidden(listVisiable.at(ch));
    }
}

void CurrentSingelSettingPage::on_degatSetApplyBtn_clicked()
{
    double degatVolt = ui->degatVoltDpbox->value();
    int degatDurationTime = ui->degatDurationTimeSpbox->value();
    int degatCheckDutyTime = ui->checkDutyTimeSpbox->value();
    for(int ch  = 0 ; ch < CHANNEL_NUM; ch++)
    {
         m_vctSingleChComb[ch]->setDegatParam(degatVolt,degatDurationTime, degatCheckDutyTime);
    }

}


void CurrentSingelSettingPage::recDegatResultSlot(QVariant rawInfo)
{
    QVector<bool> nDegateRult = rawInfo.value<QVector<bool>>();
//    qDebug()<<nDegateRult;
    for(int ch = 0; ch < nDegateRult.size(); ch++ )
    {
        m_vctSingleChComb.at(ch)->setDegateColor(nDegateRult[ch]);
    }

}

void CurrentSingelSettingPage::on_degatAllChk_toggled(bool checked)
{
    for(int ch = 0; ch < CHANNEL_NUM; ch++ )
    {
        if(m_vctSingleChComb.at(ch)->getPoreState())
        {
             m_vctSingleChComb.at(ch)->setAutoCheck(checked);
        }
    }
}

void CurrentSingelSettingPage::on_degatEnableBtn_clicked(bool checked)
{
    if(!checked)
    {
        ui->degatEnableBtn->setText("Enable");
        for(auto &&singleChanel: m_vctSingleChComb)
        {
            singleChanel->setAutoCheck(false);
            singleChanel->setAutoCheckEnable(false);
        }
        g_pCommunicaor->g_bDegattingRunning = false;
    }
    else
    {
        ui->degatEnableBtn->setText("Disable");
        for(auto &&singleChanel: m_vctSingleChComb)
        {
            singleChanel->setAutoCheckEnable(true);
        }
        g_pCommunicaor->g_bDegattingRunning = true;
    }
}

void CurrentSingelSettingPage::on_degatAdvancedBtn_clicked()
{
//    QString result = MessageDialogHelper::showInputPasswordBox("Please Enter password : 123456",false);
//    if(QString::compare(result,PROGRAME_PASSWORD) == 0)
//    {
//         qDebug()<<"Password correct"<<result;
         m_pDegatParamsDialog->show();
//    }

}


void CurrentSingelSettingPage::on_nSingleVoltAPpplyBtn_clicked()
{
    double volt = ui->nSingleVoltAmpDoubBox->value();
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        if(m_vctSingleChComb[ch]->isSingleVoltChecked())
        {
             m_vctSingleChComb[ch]->setVolt(volt);
             m_vctSingleChComb[ch]->applyBtnClick();
        }
    }
}

void CurrentSingelSettingPage::on_nSinglePosOrNegBtn_clicked()
{
    ui->nSingleVoltAmpDoubBox->setValue( - ui->nSingleVoltAmpDoubBox->value());
    on_nSingleVoltAPpplyBtn_clicked();
}

void CurrentSingelSettingPage::on_nSingleAllChk_toggled(bool checked)
{
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        if(!ui->listWidget->item(ch)->isHidden())
            m_vctSingleChComb[ch]->setSingleVoltCheck(checked);
    }

}


void CurrentSingelSettingPage::setCapTrangularWaveSlot(const bool &bIsSet)
{
    if(bIsSet)
    {
        ui->nVoltTypeComb->setCurrentIndex(TRIANGULAR_VOLTAGE);
        ui->nVoltAmpDoubBox->setValue(0.05);
        onVoltAPpplyBtnClicked();
    }
    else
    {
        setAllChannelOriVoltBySingle();
    }
}

void CurrentSingelSettingPage::setAllChannelOriVoltBySingle(void)
{
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        m_vctSingleChComb[ch]->applyBtnClick();
    }
}


void CurrentSingelSettingPage::on_polyTestStartBtn_toggled(bool checked)
{
    if(checked)
    {
        ui->polyCurrentlimitdpbox->setEnabled(!checked);
        ui->polyVoltSetDpbox->setEnabled(!checked);
        ui->polyVoltstartdpbox->setEnabled(!checked);
        ui->polyVoltEndDpbox->setEnabled(!checked);
        ui->polyVoltStepdpbox->setEnabled(!checked);
        ui->polyStepTimespbox->setEnabled(!checked);

        ui->polyTestStartBtn->setText(QStringLiteral("Stop"));
        UpdatePolymerParam();
        m_polyVoltCtrTimer.start(ui->polyStepTimespbox->value()*1000);
        m_polymerVoltParam.nPoreVoltCur  = m_polymerVoltParam.nPoreVoltStart;
        PolymerAllChVoltSet(m_polymerVoltParam.nPoreVoltCur);

        emit setPoreInsertionWorkSig(checked,ui->polyCurrentlimitdpbox->value(),ui->polyVoltSetDpbox->value());
    }
    else
    {
        ui->polyCurrentlimitdpbox->setEnabled(!checked);
        ui->polyVoltSetDpbox->setEnabled(!checked);
        ui->polyVoltstartdpbox->setEnabled(!checked);
        ui->polyVoltEndDpbox->setEnabled(!checked);
        ui->polyVoltStepdpbox->setEnabled(!checked);
        ui->polyStepTimespbox->setEnabled(!checked);

        ui->polyTestStartBtn->setText(QStringLiteral("Start"));
        if(m_polyVoltCtrTimer.isActive())
        {
            m_polyVoltCtrTimer.stop();
        }
        emit setPoreInsertionWorkSig(checked,ui->polyCurrentlimitdpbox->value(),ui->polyVoltSetDpbox->value());
    }
}

void CurrentSingelSettingPage::UpdatePolymerParam(void)
{
    m_polymerVoltParam.nPoreCurrent = ui->polyCurrentlimitdpbox->value();
    m_polymerVoltParam.nPoreVolt    = ui->polyVoltSetDpbox->value();
    m_polymerVoltParam.nPoreVoltStart=ui->polyVoltstartdpbox->value();
    m_polymerVoltParam.nPoreVoltEnd = ui->polyVoltEndDpbox->value();
    m_polymerVoltParam.nPoreVoltStep= ui->polyVoltStepdpbox->value();
    m_polymerVoltParam.nPoreTimeStep= ui->polyStepTimespbox->value();
}

void CurrentSingelSettingPage::PolymerVoltCtrolTimeoutSlot(void)
{
    if(m_polymerVoltParam.nPoreVoltCur < m_polymerVoltParam.nPoreVoltEnd)
    {
        m_polymerVoltParam.nPoreVoltCur += m_polymerVoltParam.nPoreVoltStep;
        PolymerAllChVoltSet(m_polymerVoltParam.nPoreVoltCur);
    }
    else
    {
        m_polymerVoltParam.nPoreVoltCur = m_polymerVoltParam.nPoreVoltStart;
        PolymerAllChVoltSet(m_polymerVoltParam.nPoreVoltCur);
    }
}

void CurrentSingelSettingPage::PolymerAllChVoltSet(const float &volt)
{
    for(int ch =0; ch < CHANNEL_NUM; ch++)
    {
        bool isSet = false;
        emit getPoreStateBackSig(ch,isSet);
        if(!isSet)
        {
            setSpecChanelDirect(ch,volt);
        }
    }
}

void CurrentSingelSettingPage::UpdateTrianPulseParam(void)
{
    m_triangularPulseSimulateParam.nVoltStart = ui->TrianPuseVoltStartDpbox->value();
    m_triangularPulseSimulateParam.nVoltEnd   = ui->TrianPuseVoltEndDpbox->value();
    m_triangularPulseSimulateParam.nVoltStep  =ui->TrianPuseVoltStepDpbox->value();
    m_triangularPulseSimulateParam.nVoltTimeStep = ui->TrianPuseVoltStepTimeDpbox->value();
    m_triangularPulseSimulateParam.nVoltCur   =  m_triangularPulseSimulateParam.nVoltStart;
}


void CurrentSingelSettingPage::TrianPulseVoltCtrolTimeoutSlot(void)
{
    if(m_bTriangularDirection == TRIANGULAR_RISE)
    {
        if(m_triangularPulseSimulateParam.nVoltCur < m_triangularPulseSimulateParam.nVoltEnd)
        {
            m_triangularPulseSimulateParam.nVoltCur  += m_triangularPulseSimulateParam.nVoltStep;
            setAllChannelDirect(m_triangularPulseSimulateParam.nVoltCur);
        }
        else if(m_triangularPulseSimulateParam.nVoltCur >= m_triangularPulseSimulateParam.nVoltEnd)
        {
            m_triangularPulseSimulateParam.nVoltCur = m_triangularPulseSimulateParam.nVoltEnd;
            setAllChannelDirect(m_triangularPulseSimulateParam.nVoltCur);
             m_bTriangularDirection = TRIANGULAR_FALLING;
        }
    }
    else if(m_bTriangularDirection == TRIANGULAR_FALLING)
    {
        if(m_triangularPulseSimulateParam.nVoltCur > m_triangularPulseSimulateParam.nVoltStart)
        {
            m_triangularPulseSimulateParam.nVoltCur -= m_triangularPulseSimulateParam.nVoltStep;
            setAllChannelDirect(m_triangularPulseSimulateParam.nVoltCur);
        }else if(m_triangularPulseSimulateParam.nVoltCur <= m_triangularPulseSimulateParam.nVoltStart)
        {
            m_triangularPulseSimulateParam.nVoltCur = m_triangularPulseSimulateParam.nVoltStart;
            setAllChannelDirect(m_triangularPulseSimulateParam.nVoltCur);
            m_bTriangularDirection = TRIANGULAR_RISE;
        }
    }
}

void CurrentSingelSettingPage::on_TrianPuseStartBtn_toggled(bool checked)
{
    ui->TrianPuseVoltEndDpbox->setEnabled(!checked);
    ui->TrianPuseVoltStartDpbox->setEnabled(!checked);
    ui->TrianPuseVoltStepDpbox->setEnabled(!checked);
    ui->TrianPuseVoltStepTimeDpbox->setEnabled(!checked);
    if(checked)
    {
        ui->TrianPuseStartBtn->setText(QStringLiteral("Stop"));
        UpdateTrianPulseParam();
        setAllChannelDirect(m_triangularPulseSimulateParam.nVoltCur);
        m_triangularPulseSimulateTimer.start(m_triangularPulseSimulateParam.nVoltTimeStep * 1000);
    }
    else
    {
        ui->TrianPuseStartBtn->setText(QStringLiteral("Start"));
        if(m_triangularPulseSimulateTimer.isActive())
        {
            m_triangularPulseSimulateTimer.stop();
        }
    }
}

void CurrentSingelSettingPage::onTriangularTestStop(void)
{
    if(ui->TrianPuseStartBtn->isChecked())
    {
        ui->TrianPuseStartBtn->setChecked(false);
    }
}
