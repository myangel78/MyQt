#pragma once

#include <QMainWindow>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include "UsbFtdDevice.h"
class QWidget;
class ComunicateTestWdgt;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QWidget *m_pCenterWdgt = nullptr;

    QPushButton *m_pSearchUsbDeviceBtn = nullptr;
    QPushButton *m_pOpenUsbDeviceBtn = nullptr;
    QLabel *m_pUsbDeviceItemLab = nullptr;
    QComboBox *m_pUsbDeviceItemComb= nullptr;

    UsbFtdDevice m_usbFtdDevice;

    ComunicateTestWdgt *m_pComunicateTestWdgt = nullptr;


private:
    void InitCtrl(void);

private slots:

    bool OnSearchPort(void);
    void OnOpenPort(bool checked);


};
