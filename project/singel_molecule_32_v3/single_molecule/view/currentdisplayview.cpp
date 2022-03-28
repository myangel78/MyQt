#include "currentdisplayview.h"
#include "ui_currentdisplayview.h"
#include "models/DataDefine.h"
#include <QDockWidget>


CurrentDisplayView::CurrentDisplayView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentDisplayView)
{
    ui->setupUi(this);
    initView();
    m_bAutoHole = false;
    m_dutyTimer =  new QTimer;
    connect(m_dutyTimer,SIGNAL(timeout()),this,SLOT(HoleEmbedDutyTimeOut()));
}

CurrentDisplayView::~CurrentDisplayView()
{
    delete ui;
}

void CurrentDisplayView::initView(void)
{
    ui->nUnHoleColorLab->setStyleSheet(colorStyle[UN_HOLE_STATE]);
    ui->nSingelColorLab->setStyleSheet(colorStyle[SINGEL_HOLE_STATE]);
    ui->nMultColorLab->setStyleSheet(colorStyle[MULTDEM_HOLE_STATE]);
    ui->nSetZeroColorLab->setStyleSheet("border:3px solid black;");

    m_vctCurrentDisp.resize(CHANNEL_NUM);
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
//        item->setSizeHint(QSize(60, 20));
        m_vctCurrentDisp[ch] = std::make_shared<CurrentDispItem>();
        m_vctCurrentDisp[ch]->init(ch,vctPenColor[ch%32]);
        m_vctCurrentDisp[ch]->setChecked(true);
        ui->listWidget->setItemWidget(item,m_vctCurrentDisp[ch].get());

        connect(m_vctCurrentDisp[ch].get(),&CurrentDispItem::setZeroSig,this,&CurrentDisplayView::setZeroItemSlot);
        connect(m_vctCurrentDisp[ch].get(),SIGNAL(GraphColorChangeSig(int,QColor)),this,SIGNAL(GraphColorChangeSig(int,QColor)));
        connect(m_vctCurrentDisp[ch].get(),SIGNAL(GraphVisibleSig(int,bool)),this,SIGNAL(GraphVisibleSig(int,bool)));
    }

}

void CurrentDisplayView::LoadConfig(QSettings *config)
{
    config->beginGroup("HoleEmbedded");
    ui->nSingelLmtDpbox->setValue(config->value("SingelHoleLimit").toDouble());
    ui->nMultHoleLmtDpbox->setValue(config->value("MultHoleLmt").toDouble());
    config->endGroup();

    config->beginGroup("Graphs");
    config->beginReadArray("Channels");
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        config->setArrayIndex(i);
        QColor color(config->value("Color").toString());
        m_vctCurrentDisp[i]->setColor(color);
        m_vctCurrentDisp[i]->setChecked(config->value("Visible").toBool());
    }
    config->endArray();
    config->endGroup();
}

void CurrentDisplayView::SaveConfig(QSettings *config)
{
    config->beginGroup("HoleEmbedded");
    config->setValue("SingelHoleLimit",  ui->nSingelLmtDpbox->value());
    config->setValue("MultHoleLmt", ui->nMultHoleLmtDpbox->value());
    config->endGroup();

    config->beginGroup("Graphs");
    config->beginWriteArray("Channels");
    for (int i = 0; i < CHANNEL_NUM; ++i) {
        config->setArrayIndex(i);
        config->setValue("Visible", m_vctCurrentDisp[i]->isChecked());
        config->setValue("Color", m_vctCurrentDisp[i]->color().name());
    }
    config->endArray();
    config->endGroup();
}



void CurrentDisplayView::setZeroItemSlot(CurrentDispItem *item)
{
    float volt = 0.0;
    int channel  =item->getChannel();
    PORE_STATE nPoreState = item->getPoreState();
    if(nPoreState == SINGEL_HOLE_STATE)
    {
        volt = ui->nSingleVoltDpbox->value();
    }
    emit setChannelZeroSig(channel,volt);
}

void CurrentDisplayView::dispMeansSlot(QVector<float> dispValue)
{
    float singelLimit = ui->nSingelLmtDpbox->value();
    float mulitLimit  = ui->nMultHoleLmtDpbox->value();
    for(int ch = 0 ; ch < dispValue.size(); ch++)
    {
        m_vctCurrentDisp[ch]->setMeanValue(dispValue.at(ch));
        if(m_bAutoHole)
        {
            m_vctCurrentDisp[ch]->JudgeHoleType(singelLimit,mulitLimit,dispValue.at(ch));
        }
    }
}


void CurrentDisplayView::dispInstantSlot(QVector<float> dispValue)
{
    if(isVisible())
    {
        for(int ch = 0 ; ch < dispValue.size(); ch++)
        {
             m_vctCurrentDisp[ch]->setInstantValue(dispValue.at(ch));
        }
    }
}

void CurrentDisplayView::on_nMeansLimitBtn_clicked(bool checked)
{
    ui->nAutoEnableBtn->setEnabled(checked);
    ui->nSingelLmtDpbox->setEnabled(!checked);
    ui->nMultHoleLmtDpbox->setEnabled(!checked);
    if(checked)
    {
        ui->nMeansLimitBtn->setText(QStringLiteral("Disable"));
        m_bAutoHole = true;
    }
    else
    {
        ui->nMeansLimitBtn->setText(QStringLiteral("Enable"));
        ui->nAutoEnableBtn->setChecked(checked);
        m_bAutoHole = false;
        ResetAllHoleColor();
    }
    for(int ch = 0; ch < CHANNEL_NUM; ch ++)
    {
        m_vctCurrentDisp[ch]->setHoleEmbedEnable(checked);
    }
}


void CurrentDisplayView::ResetAllHoleColor(void)
{
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        m_vctCurrentDisp[ch]->resetHoleType();
    }
}

void CurrentDisplayView::HoleEmbedDutyTimeOut(void)
{
    for(int ch =0; ch < CHANNEL_NUM; ch++)
    {
        m_vctCurrentDisp[ch]->AutoSetZeroAction();
    }
}

void CurrentDisplayView::on_nAutoEnableBtn_toggled(bool checked)
{
    if(checked)
    {
        ui->nAutoEnableBtn->setText(QStringLiteral("Disable Auto"));
        m_dutyTimer->start(500);
    }
    else
    {
        ui->nAutoEnableBtn->setText(QStringLiteral("Auto Set Zero"));
        if(m_dutyTimer->isActive())
        {
            m_dutyTimer->stop();
        }
    }
}


void CurrentDisplayView::on_allCheck_toggled(bool checked)
{
    emit  GraphAllVisibleSig(checked);
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        m_vctCurrentDisp[ch]->setChecked(checked);
    }
}

void CurrentDisplayView::on_onlyShowCheckChk_toggled(bool checked)
{
    QVector<bool> nSlideItemVisable(CHANNEL_NUM,true);
    if(checked)
    {
        for(int ch = 0; ch < CHANNEL_NUM; ch++)
        {
            if(m_vctCurrentDisp[ch]->isChecked())
            {
                nSlideItemVisable[ch] = false;
                ui->listWidget->item(ch)->setHidden(false);
            }
            else
            {
                 nSlideItemVisable[ch] = true;
                 ui->listWidget->item(ch)->setHidden(true);
            }

        }
    }
    else
    {
        for(int ch = 0; ch < CHANNEL_NUM; ch++)
        {
            nSlideItemVisable[ch] = false;
            ui->listWidget->item(ch)->setHidden(false);
        }

    }
    emit ListItemVisableSig(nSlideItemVisable);
}



void CurrentDisplayView::setPoreInsertionWorkSlot(bool bIsOn,const float &singleLimt,const float &singleVolt)
{
    ui->nSingelLmtDpbox->setValue(singleLimt);
    ui->nSingleVoltDpbox->setValue(singleVolt);
    if(bIsOn)
    {
        if(!ui->nMeansLimitBtn->isChecked())
        {
             ui->nMeansLimitBtn->click();
        }
//        if(!ui->nAutoEnableBtn->isChecked())
//        {
//            ui->nAutoEnableBtn->click();
//        }
    }
    else
    {
//        if(ui->nAutoEnableBtn->isChecked())
//        {
//            ui->nAutoEnableBtn->click();
//        }
    }

}

void CurrentDisplayView::getPoreStateBackSlot(const int &channel,bool &bIsSet)
{
    bIsSet = m_vctCurrentDisp[channel]->GetSpecChannelIsShutoff();
}

