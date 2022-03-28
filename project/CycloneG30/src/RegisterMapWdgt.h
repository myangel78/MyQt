#pragma once
#include <QWidget>

#include "XmlStreamReader.h"


class QLabel;
class QComboBox;
class QLineEdit;
class QGroupBox;
class QPushButton;
class QToolButton;
class QSortFilterProxyModel;
class QSqlTableModel;
class SqlDataBase;
class RegisterTableModel;
class RegFieldTableModel;
class CustomDelegate;
class RegFiledValueDelegate;
class RegisterTableView;
class RegFieldTableView;
class QSqlRecord;


using namespace Cyclone;

typedef struct FiledValueStu
{
    int Row;
    QString BitsDescStr;
    int HighBtis;
    int LowBits;
    int BitsDiff;
    QString ValueBinStr;
    QString ValueHexStr;
    uint     Value;
}FiledValueStu;


class QMenu;

class RegisterMapWdgt :public QWidget,public IXmlTransformCallback
{
    Q_OBJECT
public:
    explicit RegisterMapWdgt(QWidget *parent = nullptr);
    ~RegisterMapWdgt();
    typedef enum
    {
        SEARCH_TYPE_ADDRSS = 0,
        SEARCH_TYPE_REG_NAME,
        SEARCH_TYPE_ID,
    }REG_SEARCH_TYPE_ENUM;

public:
    bool ParseXmlToReloadDatabase(const QString &filePath);
    bool WriteElementRegToDataBase(const Cyclone::RegisterStr &elment);

public:
    bool InitRegsTable(void);
    bool UpLoadOneRegsInfoToDb(const RegisterStr &regStr);

signals:
    void ASICRegisterWiteOneSig(const uchar &addr,const uint &data);
    void ASICRegisterReadOneSig(const uchar &addr,uint &data,bool &bRet);
    void ASICRegisterReadAllSig(std::vector<std::tuple<uchar, uint32_t >> &vctAddrData,bool &bRet);

private:

    SqlDataBase *m_pSqlDataBase = nullptr;
    XmlStreamReader *m_pXmlStreamReader = nullptr;

    CustomDelegate *m_pRegCustomDelegte = nullptr;
    RegisterTableModel *m_pRegsTableModel = nullptr ;
    RegisterTableView *m_pRegistMapTableView = nullptr;

    RegFiledValueDelegate *m_pFiledValueDelegate = nullptr;
    RegFieldTableModel *m_pRegFieldTableModel = nullptr ;
    RegFieldTableView *m_pRegFieldTableView = nullptr;

    QPushButton *m_pRegSyncFieBtn = nullptr;
    QLabel *m_pRegSearchLab = nullptr;
    QComboBox *m_pRegSearchType = nullptr;
    QLineEdit *m_pRegSerchContEdit =nullptr;
    QPushButton *m_pRegSerchBtn =nullptr;
    QPushButton *m_pRegRefreshBtn =nullptr;
    QPushButton *m_PRegReloadBtn =nullptr;
    QPushButton *m_pRegSigleReadBtn = nullptr;
    QPushButton *m_pRegSigleWriteBtn = nullptr;
    QPushButton *m_pRegWriteReadBtn = nullptr;


    QPushButton *m_pRegRevertBtn = nullptr;
    QPushButton *m_pRegSubmitBtn = nullptr;
    QPushButton *m_pRegAllRsetDefBtn = nullptr;
    QPushButton *m_pRegAllWriteBtn = nullptr;
    QPushButton *m_pRegAllReadfBtn = nullptr;


    QPushButton *m_pFieldTabRevertBtn = nullptr;
    QPushButton *m_pFieldTabResetBtn = nullptr;


    QMenu *m_pRegMenu = nullptr;
    QMenu *m_pRegTableMenu =nullptr;
    QAction *m_pRegModifyAction = nullptr;
    QAction *m_pRegSendAction = nullptr;
    QAction *m_pRegReadAction = nullptr;

    QString m_nRegsTableName;
    QString m_dataBasePath;

    QGroupBox *m_pRegWholeGrpbx = nullptr;
private slots:
    void RegTabRevetBtnSlot(void);
    void RegTabSubmBtnSlot(void);
    void OnAllRegWriteBtnSlot(void);
    void OnAllRegReadBtnSlot(void);
    void OnCurrentRowChange(const QModelIndex &current, const QModelIndex &previous);
    void RegValueChangeSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void RegTabResetDefBtnSlot(void);


    void RegFieldSubmBtnSlot(void);
    void RegVueSyncAllFieldBtnSlot(void);
    void RegFiedldTabRevetBtnSlot(void);
    void RegFieldResetDefBtnSlot(void);

    void RegTabSigleWriteBtnSlot(void);
    void RegTabSigleReadBtnSlot(void);
    void RegTabSigleWriteReadBtnSlot(void);


    //Reg Table  右键菜单
    void RegTabCreatMenuSlot(const QPoint & p);
    void RegTabModifyActSlot(void);
    void RegTabSendActSlot(void);      //寄存器写
    void RegTabReadActSlot(void);       //寄存器读

    //搜索表
    void OnRegTabSearchBtnSlot(void);

    //通过xml重新加载寄存器
    void OnParseXmlFile(void);

    void WholeWdgtEnableOperSlot(const bool &enabled);

private:
    void InitCtr(void);
    void RefreshTable(void);
    bool CurrentRegsRowInfo(RegIdItemSimp &item) const;

    void OnRegVueSyncAllField(const int &regRow,const bool &submit);

    void OnWriteAllRegByModelIndexList(const QList<QModelIndex> &indexes);
    void OnReadAllRegByModelIndexList(const QList<QModelIndex> &indexes);
    void OnReadWriteAllRegByModelIndexList(const QList<QModelIndex> &indexes);
};

