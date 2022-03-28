#include "RegisterMapWdgt.h"
#include "qpushbutton.h"
#include "qtableview.h"
#include <QSortFilterProxyModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QXmlStreamReader>
#include <QFile>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTextCodec>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QCoreApplication>
#include <QGroupBox>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QSqlField>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSqlDriver>
#include <QFileDialog>
#include <QToolButton>
#include <QTimer>
#include <QSqlError>
#include <QProgressDialog>
#include <QApplication>
#include <QInputDialog>

#include "Log.h"
#include "ConfigServer.h"
#include "CustomDelegate.h"
#include "SqlDataBase.h"
#include "RegisterTableModel.h"
#include "RegFieldTableModel.h"
#include "CustomDelegate.h"
#include "InputNewRecordDlg.h"
#include "RegisterTableView.h"
#include "RegFieldTableView.h"




RegisterMapWdgt::RegisterMapWdgt(QWidget *parent):QWidget(parent)
{
    InitCtr();
}

RegisterMapWdgt::~RegisterMapWdgt()
{
    delete m_pXmlStreamReader;
    delete m_pSqlDataBase;

    delete m_pRegCustomDelegte;
    delete m_pRegsTableModel;
    delete m_pRegistMapTableView;

    delete m_pFiledValueDelegate;
    delete m_pRegFieldTableModel;
    delete m_pRegFieldTableView;
}

void RegisterMapWdgt::InitCtr(void)
{
    m_dataBasePath = QCoreApplication::applicationDirPath()+ "/RegsDb.db";
    m_pSqlDataBase = new SqlDataBase(this);
    m_pSqlDataBase->InitDataBase(m_dataBasePath);

    m_pXmlStreamReader = new XmlStreamReader(this);
//    QString regsFile (QCoreApplication::applicationDirPath()+"/Registers.xml");
//    if(m_pXmlStreamReader->ReadFile(regsFile))
//    {
////        m_pXmlStreamReader->PrintAllMembers();
//        m_pXmlStreamReader->UpLoadDataToRegsTable();
//    }

    m_nRegsTableName = "Registers";
    m_pSqlDataBase->InitRegsTable(m_nRegsTableName);

    m_pRegSyncFieBtn = new QPushButton(QStringLiteral("Sync"),this);

    m_pRegSearchLab = new QLabel(tr("Search type:"),this);
    m_pRegSerchBtn = new QPushButton(QStringLiteral("Search"),this);
    m_pRegRefreshBtn = new QPushButton(QStringLiteral("Whole Table"),this);
    m_PRegReloadBtn = new QPushButton(QIcon(":/img/img/xml.png"),QStringLiteral("Reload Register By Xml"),this);
    m_pRegSerchContEdit = new QLineEdit(this);
    m_pRegSearchType = new QComboBox(this);
    m_pRegSearchType->addItem("Address",SEARCH_TYPE_ADDRSS);
    m_pRegSearchType->addItem("Reg_Name",SEARCH_TYPE_REG_NAME);
//    m_pRegSearchType->addItem("Id",SEARCH_TYPE_ID);

    m_pRegAllRsetDefBtn = new QPushButton(QStringLiteral("All reset default"),this);
    m_pRegAllWriteBtn = new QPushButton(QStringLiteral("All write"),this);
    m_pRegAllReadfBtn = new QPushButton(QStringLiteral("All read"),this);
    m_pRegRevertBtn = new QPushButton(QStringLiteral("Revert Change"),this);
    m_pRegSubmitBtn = new QPushButton(QStringLiteral("Submit Change"),this);


    m_pRegWriteReadBtn = new QPushButton(QStringLiteral("Write-Read"),this);
//    m_pFieldTabSubmitBtn = new QPushButton(QStringLiteral("Submit Change"),this);
    m_pFieldTabRevertBtn = new QPushButton(QStringLiteral("Revert Change"),this);
    m_pFieldTabResetBtn = new QPushButton(QStringLiteral("Reset Default"),this);


    m_pRegMenu = new QMenu(this);
    m_pRegModifyAction = new QAction("Modify",this);
    m_pRegSendAction = new QAction("Write-Only",this);
    m_pRegReadAction = new QAction("Read-Only",this);
    m_pRegMenu->addAction(m_pRegReadAction);
    m_pRegMenu->addAction(m_pRegSendAction);
    m_pRegMenu->addSeparator();
    m_pRegTableMenu = m_pRegMenu->addMenu("Item");
    m_pRegTableMenu->addAction(m_pRegModifyAction);

    m_pRegsTableModel = new RegisterTableModel(this);
//    m_pRegsTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_pRegsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_pRegsTableModel->setTable(m_nRegsTableName);
    m_pRegsTableModel->select();

    m_pRegistMapTableView = new RegisterTableView(m_pRegsTableModel,this);
    m_pRegCustomDelegte = new CustomDelegate(this);
    m_pRegistMapTableView->setModel(m_pRegsTableModel);
    m_pRegistMapTableView->setItemDelegate(m_pRegCustomDelegte);
    m_pRegistMapTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pRegistMapTableView->ResetView();

    m_pRegFieldTableModel =  new RegFieldTableModel(this);
    m_pRegFieldTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_pFiledValueDelegate = new RegFiledValueDelegate(this);
    m_pRegFieldTableView = new RegFieldTableView(m_pRegFieldTableModel,this);
    m_pRegFieldTableView->setModel(m_pRegFieldTableModel);
    m_pRegFieldTableView->setItemDelegate(m_pFiledValueDelegate);

    QHBoxLayout *serhorlayout = new QHBoxLayout();
    serhorlayout->addWidget(m_pRegSearchLab);
    serhorlayout->addWidget(m_pRegSearchType);
    serhorlayout->addWidget(m_pRegSerchContEdit);
    serhorlayout->addWidget(m_pRegSerchBtn);
    serhorlayout->addSpacing(20);
    serhorlayout->addWidget(m_pRegRefreshBtn);
    serhorlayout->addStretch();

    QHBoxLayout *regMoBtnlayout = new QHBoxLayout();
    regMoBtnlayout->addWidget(m_PRegReloadBtn);
    regMoBtnlayout->addStretch();
    regMoBtnlayout->addWidget(m_pRegAllRsetDefBtn);

    QHBoxLayout *regOpBtnhorlayout = new QHBoxLayout();
    regOpBtnhorlayout->addWidget(m_pRegRevertBtn);
    regOpBtnhorlayout->addWidget(m_pRegSubmitBtn);
    regOpBtnhorlayout->addStretch();
    regOpBtnhorlayout->addWidget(m_pRegAllReadfBtn);
    regOpBtnhorlayout->addWidget(m_pRegAllWriteBtn);


    m_pRegSigleReadBtn =  new QPushButton(QStringLiteral("Read-Only"),this);
    m_pRegSigleWriteBtn = new QPushButton(QStringLiteral("Write-Only"),this);;

    QGroupBox *fieldOperGpbox = new QGroupBox(QStringLiteral("Operating") ,this);
    QGridLayout *filedOperGrdlayout = new QGridLayout();
    filedOperGrdlayout->addWidget(m_pRegSyncFieBtn,0,0,1,1);
    filedOperGrdlayout->addWidget(m_pFieldTabResetBtn,0,1,1,1);
    filedOperGrdlayout->addWidget(m_pFieldTabRevertBtn,0,2,1,1);
    filedOperGrdlayout->addWidget(m_pRegSigleReadBtn,1,0,1,1);
    filedOperGrdlayout->addWidget(m_pRegSigleWriteBtn,1,1,1,1);
    filedOperGrdlayout->addWidget(m_pRegWriteReadBtn,1,2,1,1);
    filedOperGrdlayout->setColumnStretch(3,1);
    fieldOperGpbox->setLayout(filedOperGrdlayout);

    QVBoxLayout *vRegTablayout = new QVBoxLayout();
    vRegTablayout->addLayout(serhorlayout);
    vRegTablayout->addLayout(regMoBtnlayout);
    vRegTablayout->addWidget(m_pRegistMapTableView);
    vRegTablayout->addLayout(regOpBtnhorlayout);

    QVBoxLayout *vFieldTablayout = new QVBoxLayout();
    vFieldTablayout->addWidget(m_pRegFieldTableView);
    vFieldTablayout->addWidget(fieldOperGpbox);

    QGroupBox *regGrpbox = new QGroupBox(QStringLiteral("Registers"),this);
    regGrpbox->setLayout(vRegTablayout);
    regGrpbox->setFixedWidth(415);

    QGroupBox *fieldGrpbox = new QGroupBox(QStringLiteral("Field"),this);
    fieldGrpbox->setLayout(vFieldTablayout);

    m_pRegWholeGrpbx = new QGroupBox(QStringLiteral("Whole"),this);
    m_pRegWholeGrpbx->setCheckable(true);
#ifdef EASY_PROGRAM_DEBUGGING
    m_pRegWholeGrpbx->setChecked(true);
#else
    m_pRegWholeGrpbx->setChecked(false);
#endif
    QHBoxLayout *wholehorlayout = new QHBoxLayout();
    wholehorlayout->addWidget(regGrpbox);
    wholehorlayout->addWidget(fieldGrpbox);
    m_pRegWholeGrpbx->setLayout(wholehorlayout);
    QHBoxLayout *horlayout = new QHBoxLayout();
    horlayout->addWidget(m_pRegWholeGrpbx);
    setLayout(horlayout);

    connect(m_pRegistMapTableView->selectionModel(),&QItemSelectionModel::currentRowChanged,this,&RegisterMapWdgt::OnCurrentRowChange);
    connect(m_pRegsTableModel,&RegisterTableModel::RegsValueChangeSig,this,&RegisterMapWdgt::RegValueChangeSlot);
    connect(m_pRegSerchBtn,&QPushButton::clicked,this,&RegisterMapWdgt::OnRegTabSearchBtnSlot);
    connect(m_pRegRefreshBtn,&QPushButton::clicked,[=](){
        m_pRegsTableModel->setTable(m_pRegsTableModel->tableName());
        m_pRegsTableModel->select();
        m_pRegistMapTableView->ResetView();});
    connect(m_PRegReloadBtn,&QPushButton::clicked,this,&RegisterMapWdgt::OnParseXmlFile);
    connect(m_pRegAllWriteBtn,&QPushButton::clicked,this,&RegisterMapWdgt::OnAllRegWriteBtnSlot);
    connect(m_pRegAllReadfBtn,&QPushButton::clicked,this,&RegisterMapWdgt::OnAllRegReadBtnSlot);
    connect(m_pRegAllRsetDefBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegTabResetDefBtnSlot);
    connect(m_pRegRevertBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegTabRevetBtnSlot);
    connect(m_pRegSubmitBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegTabSubmBtnSlot);

    connect(m_pFieldTabResetBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegFieldResetDefBtnSlot);
    connect(m_pRegSyncFieBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegVueSyncAllFieldBtnSlot);
    connect(m_pFieldTabRevertBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegFiedldTabRevetBtnSlot);
    connect(m_pRegSigleReadBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegTabSigleReadBtnSlot);
    connect(m_pRegSigleWriteBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegTabSigleWriteBtnSlot);
    connect(m_pRegWriteReadBtn,&QPushButton::clicked,this,&RegisterMapWdgt::RegTabSigleWriteReadBtnSlot);


    connect(m_pRegistMapTableView,&QTableView::customContextMenuRequested, this,&RegisterMapWdgt::RegTabCreatMenuSlot);
    connect(m_pRegModifyAction,&QAction::triggered,this,&RegisterMapWdgt::RegTabModifyActSlot);
    connect(m_pRegSendAction,&QAction::triggered,this,&RegisterMapWdgt::RegTabSendActSlot);
    connect(m_pRegReadAction,&QAction::triggered,this,&RegisterMapWdgt::RegTabReadActSlot);
    connect(m_pRegWholeGrpbx,&QGroupBox::toggled,this,&RegisterMapWdgt::WholeWdgtEnableOperSlot);


}

void RegisterMapWdgt::OnAllRegWriteBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }
    if(m_pRegsTableModel->isDirty())
    {
         QMessageBox::warning(this, tr("Writing all registers"), QString("Found some regs value not submit,please confirm it before writting all !!!"));
         return;
    }

    m_pRegistMapTableView->selectAll();
    QModelIndexList indexes =m_pRegistMapTableView->selectionModel()->selectedRows(RegisterTableModel::COLUMN_REG_VALUE);
    OnWriteAllRegByModelIndexList(indexes);
}


void RegisterMapWdgt::OnAllRegReadBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }

#if 0
    int rowCount = m_pRegsTableModel->rowCount();
    int i = 0;
    std::vector<std::tuple<uchar, uint32_t >> vctAddrData;
    vctAddrData.clear();
    for(int row = 0; row < rowCount; ++row)
    {
        uchar addr =0; uint data =0;
        bool bResult  = m_pRegistMapTableView->GetRegAddrByRow(row,addr);
        bool bResData = m_pRegistMapTableView->GetRegVueByRow(row,data);
        if(bResult && bResData)
        {
            vctAddrData.emplace_back(addr,0);
            i++;
        }
    }

    bool bRet = false;
    emit ASICRegisterReadAllSig(vctAddrData,bRet);
#endif
    m_pRegistMapTableView->selectAll();
    QModelIndexList indexes =m_pRegistMapTableView->selectionModel()->selectedRows(RegisterTableModel::COLUMN_REG_VALUE);
    OnReadAllRegByModelIndexList(indexes);
}


void RegisterMapWdgt::RegTabSendActSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }
//    if(m_pRegsTableModel->isDirty())
//    {
//         QMessageBox::warning(this, tr("Writing all registers"), QString("Found some regs value not submit,please confirm it before writting all !!!"));
//         return;
//    }

    QModelIndexList indexes =m_pRegistMapTableView->selectionModel()->selectedRows(RegisterTableModel::COLUMN_REG_VALUE);
    OnWriteAllRegByModelIndexList(indexes);

}

void RegisterMapWdgt::RegTabReadActSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }

    QModelIndexList indexes =m_pRegistMapTableView->selectionModel()->selectedRows(RegisterTableModel::COLUMN_REG_VALUE);
    OnReadAllRegByModelIndexList(indexes);
}



void RegisterMapWdgt::RegTabSigleWriteBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }
    if(m_pRegFieldTableModel->isDirty())
    {
        RegFieldSubmBtnSlot();
    }
    uchar addr =0; uint data =0;
    int row = m_pRegistMapTableView->currentIndex().row();
    bool bResult  = m_pRegistMapTableView->GetRegAddrByRow(row,addr);
    bool bResData = m_pRegistMapTableView->GetRegVueByRow(row,data);
    if(bResult && bResData)
    {
        emit  ASICRegisterWiteOneSig(addr,data);
    }
    else
    {
        LOGI("{} acquire cur addr:{} and data:{} is wrong!!!",__FUNCTION__,addr,data);
    }
}

void RegisterMapWdgt::RegTabSigleReadBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }
    uchar addr =0; uint data =0;
    int row = m_pRegistMapTableView->currentIndex().row();
    bool bResult  = m_pRegistMapTableView->GetRegAddrByRow(row,addr);
    if(bResult)
    {
        bool bRet = false;
        emit  ASICRegisterReadOneSig(addr,data,bRet);
        if(bRet)
        {
            QModelIndex valueIndex = m_pRegistMapTableView->GetRegVueIndexByRow(row);
            QString valueHexStr = QString("%1").arg(data, 8 , 16, QLatin1Char('0')).toUpper();
            m_pRegsTableModel->setData(valueIndex,valueHexStr);
            m_pRegsTableModel->UpdateVueReadResult(true,row);
        }
        else
        {
            m_pRegsTableModel->UpdateVueReadResult(false,row);
            ConfigServer::GetInstance()->GetNotifyPtr()->Notify(QString("Frame format is wrong,read back address 0x%1 failed").arg(addr,2,16,QChar('0')),QNotify::NOTIFI_ERROR,1000);
        }
    }
}

void RegisterMapWdgt::RegTabSigleWriteReadBtnSlot(void)
{
    if(!ConfigServer::GetInstance()->GetIsUsbConnect())
    {
        QMessageBox::critical(this, tr("Comunicate failed!!!"), tr("Plese connect valid usb device !!!"));
        return;
    }
    if(m_pRegFieldTableModel->isDirty())
    {
        RegFieldSubmBtnSlot();
    }
    uchar addr =0; uint data =0;
    int row = m_pRegistMapTableView->currentIndex().row();
    bool bResult  = m_pRegistMapTableView->GetRegAddrByRow(row,addr);
    bool bResData = m_pRegistMapTableView->GetRegVueByRow(row,data);
    if(bResult && bResData)
    {
        bool bRet = false;
        emit  ASICRegisterWiteOneSig(addr,data);
        emit  ASICRegisterReadOneSig(addr,data,bRet);
        if(bRet)
        {
            QModelIndex valueIndex = m_pRegistMapTableView->GetRegVueIndexByRow(row);
            QString valueHexStr = QString("%1").arg(data, 8 , 16, QLatin1Char('0')).toUpper();
            m_pRegsTableModel->setData(valueIndex,valueHexStr);
            m_pRegsTableModel->UpdateVueReadResult(true,row);
        }
        else
        {
            m_pRegsTableModel->UpdateVueReadResult(false,row);
            ConfigServer::GetInstance()->GetNotifyPtr()->Notify(QString("Frame format is wrong,read back address 0x%1 failed").arg(addr,2,16,QChar('0')),QNotify::NOTIFI_ERROR,1000);
        }
    }
}


void RegisterMapWdgt::RegValueChangeSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if(topLeft == bottomRight)
    {
        const QModelIndex &index = topLeft;
        int row  = index.row();
        if(index.column() == RegisterTableModel::COLUMN_REG_VALUE)
        {
            QString fieldTabName  = m_pRegistMapTableView->GetFieldTableNameByRow(row);
            if(fieldTabName != m_pRegFieldTableModel->tableName()) // 如果域表不是当前行的表名，就直接返回，无须更新
            {
//                    qDebug()<<"\n~~~~~~~~~~~not the current field tabel ~~~~~~~~~~~~~~~~~~~~~"<<index;
                return;
            }
            OnRegVueSyncAllField(row,!m_pRegsTableModel->isDirty(index));
        }
    }
}


void RegisterMapWdgt::RegFieldSubmBtnSlot(void)
{
    bool bResult = m_pRegFieldTableView->OnSubmitAll();
    if(!bResult)
    {
        QMessageBox::critical(this,"Modify","Field Table Modify failed !!!");
        return;
    }

    uint regVue = 0;
    int rId = -1;
    if(!m_pRegFieldTableView->GenerRegVueByAllField(regVue,rId))
    {
        QMessageBox::critical(this,"Modify","Field Table Modify failed !!!");
        return;
    }

    int row = rId;
    if(m_pRegistMapTableView->SetRegVueByRow(row,regVue)) //按Rid寻找行，所以Reg表格一定不能乱或者排序
    {
         QMessageBox::information(this,"Modify","Field Table modify successfully !!!");
         return;
    }

//failed:
    QMessageBox::critical(this,"Modify","Field Table Modify failed !!!");
    return;
}


void RegisterMapWdgt::RegVueSyncAllFieldBtnSlot(void)
{
    QModelIndex index = m_pRegistMapTableView->currentIndex();
    if(index.isValid())
    {
        OnRegVueSyncAllField(index.row(),!m_pRegsTableModel->isDirty(index));
    }
}

void RegisterMapWdgt::OnRegVueSyncAllField(const int &regRow,const bool &submit)
{
        uint32_t regValue = 0;
        QString fieldTable = m_pRegistMapTableView->GetFieldTableNameByRow(regRow);
        if(m_pRegistMapTableView->GetRegVueByRow(regRow,regValue))
        {
            m_pRegFieldTableView->ModifyAllFieldVueByRegVue(regValue,fieldTable);
        }
        if(submit)
        {
            m_pRegFieldTableView->OnSubmitAll();
        }
}


void RegisterMapWdgt::RegFiedldTabRevetBtnSlot(void)
{
    if(m_pRegFieldTableModel->isDirty())
    {
        QMessageBox message(QMessageBox::Warning, "Field Table", "Do you want to revert all change in field table?", QMessageBox::Yes | QMessageBox::No, NULL);
        if(message.exec() == QMessageBox::Yes)
        {
                m_pRegFieldTableModel->revertAll();
        }
    }

}



bool RegisterMapWdgt::CurrentRegsRowInfo(RegIdItemSimp &item) const
{
    QModelIndex curIndex = m_pRegistMapTableView->currentIndex();
    if( !curIndex.isValid())
        return false;
    int row = curIndex.row();
    QSqlRecord record = m_pRegsTableModel->record(row);  //取得当前行的数据集
    for( int colum = 0; colum < record.count(); ++colum)//从Title位置开始取数据
    {

        QSqlField field = record.field(colum);
        switch (colum) {
        case RegisterTableModel::COLUMN_ID:
            item.id = field.value().toInt();
            break;
        case RegisterTableModel::COLUMN_REG_NAME:
            item.name = field.value().toString();
            break;
        case RegisterTableModel::COLUMN_REG_ADDR:
            item.addr = field.value().toString();
            break;
        case RegisterTableModel::COLUMN_REG_SIZE:
            item.size = field.value().toInt();
            break;
        case RegisterTableModel::COLUMN_REG_DEFAULT_VALUE:
            item.defalVue = field.value().toString();
            break;
        case RegisterTableModel::COLUMN_REG_VALUE:
            item.value = field.value().toString();
            break;
        case RegisterTableModel::COLUMN_REG_FIELD_TAB_NAME:
            item.fieldTabStr = field.value().toString();
            break;
        default:
            break;
        }
    }
    return true;
}



void RegisterMapWdgt::OnRegTabSearchBtnSlot(void)
{
    if(m_pRegSearchType->currentIndex() == SEARCH_TYPE_ADDRSS)
    {
        m_pRegsTableModel->setFilter(QString("Address = '%1' ").arg(m_pRegSerchContEdit->text()));
        m_pRegsTableModel->select();

    }else if(m_pRegSearchType->currentIndex() == SEARCH_TYPE_REG_NAME)
    {
        m_pRegsTableModel->setFilter(QString("Reg_Name like '%1%' ").arg(m_pRegSerchContEdit->text()));
        m_pRegsTableModel->select();
    }
    else if(m_pRegSearchType->currentIndex() == SEARCH_TYPE_ID)
    {
        bool bResult = false;
        int rId = m_pRegSerchContEdit->text().toInt(&bResult,10);
        if(bResult)
        {
            m_pRegsTableModel->setFilter(QString("Id = %1").arg(rId));
            m_pRegsTableModel->select();
        }
    }
}

void RegisterMapWdgt::RefreshTable(void)
{
    m_pRegsTableModel->select();
    m_pRegistMapTableView->ResetView();
    m_pRegFieldTableModel->select();
}

bool RegisterMapWdgt::InitRegsTable(void)
{
    return m_pSqlDataBase->InitRegsTable(m_nRegsTableName);
}

bool RegisterMapWdgt::UpLoadOneRegsInfoToDb(const RegisterStr &regStr)
{
    m_pSqlDataBase->UpLoadOneRegsInfoToDb(m_nRegsTableName,regStr);
    return true;
}


bool RegisterMapWdgt::WriteElementRegToDataBase(const Cyclone::RegisterStr &elment)
{
    return UpLoadOneRegsInfoToDb(elment);
}


bool RegisterMapWdgt::ParseXmlToReloadDatabase(const QString &filePath)
{
    m_pRegFieldTableView->setModel(nullptr);
    delete m_pRegFieldTableModel; //必须删除Model重新加载数据库才正常
#if 1
    if(m_pSqlDataBase->DeleteDataBase(m_dataBasePath))
    {
        m_pSqlDataBase->InitDataBase(m_dataBasePath);
        bool bReuslt =InitRegsTable();

        m_pRegFieldTableModel =  new RegFieldTableModel(this);
        m_pRegFieldTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        m_pRegFieldTableView->SetModelPtr(m_pRegFieldTableModel);
        m_pRegFieldTableView->setModel(m_pRegFieldTableModel);

        if(bReuslt && m_pXmlStreamReader->ReadFile(filePath))
        {
             QSqlDatabase db = QSqlDatabase::database();
            if (db.driver()->hasFeature(QSqlDriver::Transactions))
            {
                //开启事务
                db.transaction();
                if(m_pXmlStreamReader->UpLoadDataToRegsTable())
                {
                    db.commit();
                    RefreshTable();
                    return true;
                }
                db.rollback();
                return false;
            }
        }


    }
    return false;
#endif
}



/**
 * @brief 重新加载XML按钮槽函数
 */
void RegisterMapWdgt::OnParseXmlFile(void)
{
    if(QMessageBox::Yes != QMessageBox::question(this,"Parse","Are you sure to delete current registers table and reload from xml file ",QMessageBox::Yes|QMessageBox::No))
    {
       return;
    }


    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Registers Xml File"),
                                                    QCoreApplication::applicationDirPath(),
                                                    tr("Xml (*.xml)"));
   if(!fileName.isEmpty() && QFile(fileName).exists())
   {
       if(ParseXmlToReloadDatabase(fileName))
       {
            QMessageBox::information(this, tr("Parse Xml"), tr("Reload successfully!!!"));
       }
       else
       {
            QMessageBox::information(this, tr("Parse Xml"), tr("Reload failed!!!"));
       }
   }
}




/**
 * @brief 右键获取坐标槽函数
 * @param p
 */
void RegisterMapWdgt::RegTabCreatMenuSlot(const QPoint & p)
{
    QModelIndex index = m_pRegistMapTableView->indexAt(p);//获取鼠标点击位置项的索引
    if(index.isValid())//数据项是否有效，空白处点击无菜单s
    {
        m_pRegMenu->exec(QCursor::pos());
    }
}


/**
 * @brief 右键菜单修改单个项目
 */
void RegisterMapWdgt::RegTabModifyActSlot(void)
{
    InputNewRecordDlg inputnewrecorddlg(MODIFY_DLG);
    RegIdItemSimp curRowInfo;
    if(CurrentRegsRowInfo(curRowInfo))
    {
        inputnewrecorddlg.LoaddingData(curRowInfo);
        if(inputnewrecorddlg.exec() == QDialog::Accepted)
        {
            inputnewrecorddlg.GetInRegIdItemStu(curRowInfo);
            m_pSqlDataBase->ModifyOneRegsInfoToDb(m_nRegsTableName,curRowInfo);
            m_pRegsTableModel->select();
        }
    }

}


/**
 * @brief Register当前行改变
 * @param 当前行
 * @param 之前的行
 */
void RegisterMapWdgt::OnCurrentRowChange(const QModelIndex &current, const QModelIndex &previous)
{
    if(current.row() != previous.row())
    {
        QSqlRecord query =  m_pRegsTableModel->record(current.row());
        QString fldTabName = query.value(RegisterTableModel::COLUMN_REG_FIELD_TAB_NAME).toString();
        m_pRegFieldTableModel->setTable(fldTabName);
        m_pRegFieldTableModel->select();
        m_pRegFieldTableView->ResetView();

        //同步当前的值到域表
        bool bIsDirty = m_pRegsTableModel->isDirty(m_pRegistMapTableView->GetRegVueIndexByCurIndex(current));
        OnRegVueSyncAllField(current.row(),!bIsDirty);
    }
}

/**
 * @brief Reg Field 恢复默认
 */
void RegisterMapWdgt::RegFieldResetDefBtnSlot(void)
{
    if(!m_pRegFieldTableView->OnResetAllDef())
    {
        QMessageBox::critical(this, tr("Field Table"), tr("Reset all Default failed!!!"));
    }
}

void RegisterMapWdgt::RegTabResetDefBtnSlot(void)
{
    if(!m_pRegistMapTableView->OnResetAllDef())
    {
        QMessageBox::critical(this, tr("Registers Table"), tr("Reset all Default failed!!!"));
    }
}


void RegisterMapWdgt::RegTabRevetBtnSlot(void)
{
    if(m_pRegsTableModel->isDirty())
    {
        QMessageBox message(QMessageBox::Warning, "Field Table", "Do you want to revert all change in field table?", QMessageBox::Yes | QMessageBox::No, NULL);
        if(message.exec() == QMessageBox::Yes)
        {
                m_pRegsTableModel->revertAll();
        }
    }

}

void RegisterMapWdgt::RegTabSubmBtnSlot(void)
{
    bool bResult = m_pRegistMapTableView->OnSubmitAll();
    if(!bResult)
    {
        goto failed;
    }
    return;

failed:
    QMessageBox::critical(this,"Modify","Field Table Modify failed !!!");
}

void RegisterMapWdgt::OnWriteAllRegByModelIndexList(const QList<QModelIndex> &indexes)
{
    if(indexes.isEmpty())
    {
        return;
    }

    QProgressDialog progressDlg("Writing all registers...", "Abort writing", 0,indexes.size() , this);
    progressDlg.setWindowModality(Qt::ApplicationModal);
    progressDlg.reset();
    progressDlg.setValue(0);
    progressDlg.show();
    int cnt = 0;
    uchar addr =0; uint data =0;
    int row = 0;

    for(int i = 0; i < indexes.size(); ++i)
    {
        progressDlg.setValue(i);
        QCoreApplication::processEvents();
        if (progressDlg.wasCanceled())
            break;

        row = indexes.at(i).row();
        bool bResult  = m_pRegistMapTableView->GetRegAddrByRow(row,addr);
        bool bResData = m_pRegistMapTableView->GetRegVueByRow(row,data);
        if(bResult && bResData)
        {
            emit  ASICRegisterWiteOneSig(addr,data);
            cnt++;
        }
    }
    if(cnt == indexes.size())
    {
         progressDlg.setValue(cnt);
    }
    qDebug("%s has been already send %d items write",__FUNCTION__,cnt);
    QMessageBox::information(this, tr("Writing all registers"), QString("Writing registers Total: %1,succeed: %2, failed: %3 !!!").arg(indexes.size()).arg(cnt).arg(indexes.size() -cnt));
}

void RegisterMapWdgt::OnReadAllRegByModelIndexList(const QList<QModelIndex> &indexes)
{
    if(indexes.isEmpty())
    {
        return;
    }

    QProgressDialog progressDlg("Reading all registers...", "Abort reading", 0,indexes.size() , this);
    progressDlg.setWindowModality(Qt::ApplicationModal);
    progressDlg.reset();
    progressDlg.setValue(0);
    progressDlg.show();

    int cnt = 0,row = 0;
    uchar addr =0; uint data =0;
    bool bResult = false,bRet = false;

    for(int i = 0; i < indexes.size(); ++i)
    {
        progressDlg.setValue(i);
        QCoreApplication::processEvents();
        if (progressDlg.wasCanceled())
            break;

        row = indexes.at(i).row();
        bResult = m_pRegistMapTableView->GetRegAddrByRow(row,addr);
        if(bResult)
        {
            bRet = false;
            emit  ASICRegisterReadOneSig(addr,data,bRet);
            if(bRet)
            {
                QModelIndex valueIndex = m_pRegistMapTableView->GetRegVueIndexByRow(row);
                QString valueHexStr = QString("%1").arg(data, 8 , 16, QLatin1Char('0')).toUpper();
                m_pRegsTableModel->setData(valueIndex,valueHexStr);
                m_pRegsTableModel->UpdateVueReadResult(true,row);
                cnt++;
            }
            else
            {
                m_pRegsTableModel->UpdateVueReadResult(false,row);
                ConfigServer::GetInstance()->GetNotifyPtr()->Notify(QString("Frame format is wrong,read back addr 0x%1 failed!!").arg(addr,2,16,QChar('0')),QNotify::NOTIFI_WARNING,1000);
            }
        }
    }
    if(cnt == indexes.size())
    {
         progressDlg.setValue(cnt);
    }
    qDebug("% has been already read %d items successuflly!!!!",__FUNCTION__,cnt);
    QMessageBox::information(this, tr("Reading all registers"), QString("Reading registers Total: %1,succeed: %2, failed: %3 !!!").arg(indexes.size()).arg(cnt).arg(indexes.size() -cnt));
}


void RegisterMapWdgt::OnReadWriteAllRegByModelIndexList(const QList<QModelIndex> &indexes)
{
    if(indexes.isEmpty())
    {
        return;
    }

    QProgressDialog progressDlg("Write-Read all registers...", "Abort Write-Read", 0,indexes.size() , this);
    progressDlg.setWindowModality(Qt::ApplicationModal);
    progressDlg.reset();
    progressDlg.setValue(0);
    progressDlg.show();

    int cnt = 0,row = 0;
    uchar addr =0; uint data =0;
    bool bResult = false,bResData = false,bRet = false;

    for(int i = 0; i < indexes.size(); ++i)
    {
        progressDlg.setValue(i);
        QCoreApplication::processEvents();
        if (progressDlg.wasCanceled())
            break;

        row = indexes.at(i).row();
        bResult  = m_pRegistMapTableView->GetRegAddrByRow(row,addr);
        bResData = m_pRegistMapTableView->GetRegVueByRow(row,data);

        if(bResult && bResData)
        {
            bRet = false;
            emit  ASICRegisterWiteOneSig(addr,data);
            emit  ASICRegisterReadOneSig(addr,data,bRet);
            if(bRet)
            {
                QModelIndex valueIndex = m_pRegistMapTableView->GetRegVueIndexByRow(row);
                QString valueHexStr = QString("%1").arg(data, 8 , 16, QLatin1Char('0')).toUpper();
                m_pRegsTableModel->setData(valueIndex,valueHexStr);
                m_pRegsTableModel->UpdateVueReadResult(true,row);
                cnt++;
            }
            else
            {
                m_pRegsTableModel->UpdateVueReadResult(false,row);
                ConfigServer::GetInstance()->GetNotifyPtr()->Notify(QString("Frame format is wrong,read back address 0x%1 failed").arg(addr,2,16,QChar('0')),QNotify::NOTIFI_ERROR,1000);
            }
        }
    }
    if(cnt == indexes.size())
    {
         progressDlg.setValue(cnt);
    }
    qDebug("% has been already write-read %d items successuflly!!!!",__FUNCTION__,cnt);
    QMessageBox::information(this, tr("Write-Read all registers"), QString("Write-Read registers Total: %1,succeed: %2, failed: %3 !!!").arg(indexes.size()).arg(cnt).arg(indexes.size() -cnt));
}
void RegisterMapWdgt::WholeWdgtEnableOperSlot(const bool &enabled)
{
    if(enabled)
    {
        bool ok = false;
        QString text = QInputDialog::getText(this, QString::fromLocal8Bit("密码确认"), QString::fromLocal8Bit("请输入密码"), QLineEdit::Password, 0, &ok);
        if (ok)
        {
            if (!QString::compare(text, QString::fromStdString(PROGRAME_PASSWORD)) == 0)
            {
                m_pRegWholeGrpbx->setChecked(false);
                QMessageBox::critical(this, "Warning ", tr("Wrong password,please input again!!!"), QMessageBox::Ok);
            }
        }
        else
        {
            m_pRegWholeGrpbx->setChecked(false);
        }
    }
}
