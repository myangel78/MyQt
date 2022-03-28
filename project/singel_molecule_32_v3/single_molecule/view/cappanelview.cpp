#include "cappanelview.h"
#include "ui_cappanelview.h"
#include "models/DatacCommunicaor.h"
#include "Log.h"

#define g_pCommunicaor DataCommunicaor::Instance()


CapPanelView::CapPanelView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CapPanelView)
{
    ui->setupUi(this);
    initWidget();
    m_maxsize = 300;
    m_vctCapItem.resize(CHANNEL_NUM);
    m_vctData.resize(CHANNEL_NUM);
    m_vctCount.resize(CHANNEL_NUM);
    m_vctCapValue.resize(CHANNEL_NUM);
    m_vctYmax.resize(CHANNEL_NUM);
    memset(m_vctYmax.data(),0,m_vctYmax.size()*sizeof(int));

    connect(this,SIGNAL(reDrawSig()),this,SLOT(reDrawSlot()));
    connect(ui->capFilterApplyBtn,&QPushButton::clicked,this,&CapPanelView::onCapFilterApplyBtnclicked);

}

CapPanelView::~CapPanelView()
{
    delete ui;
}

void CapPanelView::initWidget(void)
{
    QStringList pageList;
    pageList<< "CH 1-32";
    ui->chanelSelComb->addItems(pageList);
    ui->chanelSelComb->setCurrentIndex(0);
    connect(ui->chanelSelComb, SIGNAL(activated(int)), this, SLOT(onChanleViewChange(int)));

    m_vctCapItem.resize(CHANNEL_NUM);
    for (int i = 0; i < CHANNEL_NUM; i++) {
        m_vctCapItem[i] = std::make_shared<CapItem>(nullptr,i);
        ui->gridLayout->addWidget(m_vctCapItem[i].get(),i/4,i%4);
    }
    for(int row =0; row < CHANNEL_NUM/4; row++)
    {
        ui->gridLayout->setRowMinimumHeight(row,200);
    }

    listViewInit();
}

void CapPanelView::listViewInit(void)
{
    m_vctCapListItem.resize(CHANNEL_NUM);
    for (int i = 0; i < CHANNEL_NUM; i++) {
        QListWidgetItem *item = new QListWidgetItem(ui->capDispList);
//        ui->channelList->addItem(m_vctWidgetItem[i].get());
        m_vctCapListItem[i] = std::make_shared<CapListItem>(ui->capDispList,i+1);
        ui->capDispList->setItemWidget(item, m_vctCapListItem[i].get());
    }
}


void CapPanelView::threadFunc(void)
{
    LOGI("Start run CapPanelView thread.");
    qDebug()<<"CapPanelView run thread id :" <<GetCurrentThreadId();
    int nRet = THREAD_NORMAL_EXIT;
    int nCapResSemCnt =0;
    m_bRunThread =true;
    static int sCounts = 0;
    while(m_bRunThread)
    {
        if(g_pCommunicaor->m_sCapResSem.usedBuf.tryAcquire(1,SEM_TIMEOUT))
        {
             std::lock_guard<std::mutex> lock(m_lock);
             if(g_pCommunicaor->m_vctCapResBuf[0 + CHANNEL_NUM*nCapResSemCnt].size() > 0)
             {
                 int plotBufIndex = CHANNEL_NUM * nCapResSemCnt;
                 for(int i = 0; i<CHANNEL_NUM;i++)
                 {
                    appendData(i,g_pCommunicaor->m_vctCapResBuf[plotBufIndex+i]);
                 }

             }

             nCapResSemCnt = (nCapResSemCnt == MEANSCAPRES_BUF_BLOCK_END_INDEX) ? 0 : (nCapResSemCnt+1);
             g_pCommunicaor->m_sCapResSem.freeBuf.release();

             if(sCounts++ > 5)
             {
                 sCounts = 0;
                 emit reDrawSig();
             }

        }

    }
    g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,nRet);
    LOGI("Quit CapPanelView thread.");
}

void CapPanelView::reDrawSlot(void)
{
    if(isVisible())
    {
        std::lock_guard<std::mutex> lock(m_lock);
        int startch =  ui->chanelSelComb->currentIndex() * CHANNEL_NUM;
        updateView(startch);
    }
}



void CapPanelView::onChanleViewChange(int pageIndex)
{
    int startch =  pageIndex * CHANNEL_NUM;
    updateView(startch);
    onCapListViewChange(startch);
}

void CapPanelView::onCapListViewChange(const int &startIndex)
{
    int endIndex   = startIndex + CHANNEL_NUM;
    for (int i = 0; i < CHANNEL_NUM; i++)
    {
        if(i >=startIndex && i< endIndex)
        {
            ui->capDispList->item(i)->setHidden(false);
        }
        else
        {
            ui->capDispList->item(i)->setHidden(true);
        }
    }
}

void CapPanelView::appendData(int ch,const QVector<float> &vctValue)
{
    for(QVector<float>::const_iterator iter = vctValue.begin(); iter != vctValue.end(); ++iter)
    {
       float tmpValue =  *iter;
       m_vctCapValue[ch] = tmpValue;
       m_vctYmax[ch] = (m_vctYmax.at(ch) > tmpValue ) ? m_vctYmax.at(ch) : tmpValue;
       QPointF tempPoint =  QPointF(m_vctCount[ch]++, tmpValue);
       m_vctData[ch].append(tempPoint);
       if(m_vctCount[ch] > m_maxsize)
       {
           m_vctData[ch].clear();
           m_vctCount[ch] = 0;
       }
    }
}

void CapPanelView::appendAllChData(QVector<QVector<float>> &vctValue)
{
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        appendData(ch,vctValue.at(ch));
    }
}

void CapPanelView::updateView(int startCh)
{
    int dataChannel = startCh;
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        m_vctCapItem[ch]->replace(dataChannel,m_vctData[dataChannel]);
        m_vctCapItem[ch]->setYAsix(m_vctYmax[ch]);
        dataChannel++;
    }
    updateCapValDisp();

}

void CapPanelView::updateCapValDisp(void)
{
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        m_vctCapListItem.at(ch)->setNum(m_vctCapValue[ch]);
    }

}

void CapPanelView::clearData(void)
{
    for(int ch =0; ch < CHANNEL_NUM; ch++)
    {
        m_vctData[ch].clear();
    }
}

bool CapPanelView::OverMaxCount(int channel)
{
   return m_vctData.at(channel).count() > m_maxsize;
}



void CapPanelView::on_startBtn_clicked(bool checked)
{
    if(checked)
    {
        g_pCommunicaor->g_capParameter.CapMutex.lockForWrite();
        g_pCommunicaor->g_capParameter.IsTrangleWave = true;
        g_pCommunicaor->g_capParameter.CapMutex.unlock();
        ui->startBtn->setText("Stop");
        ui->SaveCapToFileBtn->setEnabled(false);
        this->startThread();
        emit setTrangularWave(true);
    }
    else
    {
        g_pCommunicaor->g_capParameter.CapMutex.lockForWrite();
        g_pCommunicaor->g_capParameter.IsTrangleWave = false;
        g_pCommunicaor->g_capParameter.CapMutex.unlock();
        ui->startBtn->setText("Start");
        ui->SaveCapToFileBtn->setEnabled(true);
        this->stopThread();
        emit setTrangularWave(false);
    }
}

void CapPanelView::on_SaveCapToFileBtn_clicked()
{
    QFileDialog fileDialog;
    QString fileName = fileDialog.getSaveFileName(this,tr("open file"),"capacitance value",tr("Text File(*.txt)"));
    if(fileName == "")
    {
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,tr("error"),tr("open file failed"));
        return;
    }
    else
    {
        QTextStream textStream(&file);
        QString str = "capacitance value :\n";
        textStream<<str;
        for(int ch =0 ; ch < CHANNEL_NUM; ch++)
        {
            textStream<<"channel "<<ch+1<<": "<<m_vctCapValue[ch]<<" pf\n";
        }
        QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("保存文件成功"));
        file.close();
    }

}

void CapPanelView::onCapFilterApplyBtnclicked()
{
    const float capMax = ui->capFilterMaxDpbox->value();
    const float capMin = ui->capFilterMinDpbox->value();
    int count = 0;
    for(int ch = 0; ch < CHANNEL_NUM; ch++)
    {
        const float &value = m_vctCapValue[ch];
        if(value <= capMax && value >= capMin)
        {
            ui->capDispList->item(ch)->setHidden(false);
            count++;
        }
        else
        {
            ui->capDispList->item(ch)->setHidden(true);
        }
    }
    ui->capFilterCountLab->setText(tr("Count:%1").arg(count));
}

