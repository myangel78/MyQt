#pragma once

#include <QObject>
#include <QWidget>

class QTabWidget;
class RegisterMapWdgt;
class QPushButton;
class ComunicateTestWdgt;
class UsbFtdDevice;

class RegisterWidget:public QWidget
{
    Q_OBJECT
public:
    RegisterWidget(UsbFtdDevice *usbFtdDevice,QWidget *parent = nullptr);
    ~RegisterWidget();

signals:
    void ASICRegisterWiteOneSig(const uchar &addr,const uint &data);
    void ASICRegisterReadOneSig(const uchar &addr,uint &data,bool &bRet);
    void ASICRegisterReadAllSig(std::vector<std::tuple<uchar, uint32_t >> &vctAddrData,bool &bRet);

private:
    UsbFtdDevice *m_pUsbFtdDevice = nullptr;

    //Register Map widget
    RegisterMapWdgt *m_pRegisterMapWdgt = nullptr;
    //Communicate View
    ComunicateTestWdgt *m_pComunicateTestWdgt = nullptr;

    QTabWidget *m_pTabSwitWidget = nullptr;

private:
    void InitCtr(void);
};

