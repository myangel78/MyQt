/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "serialsettingsdialog.h"
#include "ui_serialsettingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QDebug>
#include <QMessageBox>

QT_USE_NAMESPACE
#define CUSTOM_PORT_NAME "STMicroelectronics Virtual COM Port"
static const char blankString[] = QT_TRANSLATE_NOOP("SerialSettingsDialog", "N/A");

SerialSettingsDialog::SerialSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialSettingsDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->serialPortInfoListBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SerialSettingsDialog::showPortInfo);
    connect(ui->AOserialPortInfoListBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SerialSettingsDialog::showAOPortInfo);
    connect(ui->baudRateBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SerialSettingsDialog::checkCustomBaudRatePolicy);

    fillPortsParameters();
    fillPortsInfo();

    fillAoPortsParameters();
    fillAoPortsInfo();

    updateSettings();
    updateAoSettings();
}

SerialSettingsDialog::~SerialSettingsDialog()
{
    delete ui;
}

SerialSettingsDialog::Settings SerialSettingsDialog::settings() const
{
    return currentSettings;
}


void SerialSettingsDialog::LoadConfig(QSettings *nConfig)
{
    if(!nConfig)
    {
        qDebug()<<"SerialSettingsDialog nConfig is nullptr"<<nConfig;
        return;
    }
    m_config = nConfig;

    nConfig->beginGroup("PortSetting");
    m_PortOutCom = nConfig->value("OutPort").toString();
    m_PortInCom = nConfig->value("InPort").toString();
    nConfig->endGroup();

    if(findPortByName(m_PortOutCom,m_PortInCom))
    {
        updateSettings();
        updateAoSettings();
        autoConnect();
    }

//    plotSetSlot(); //更新plot设置参数；
}

void SerialSettingsDialog::SaveConfig(QSettings * nConfig)
{
    if(!nConfig)
    {
        qDebug()<<"Serial nConfig is nullptr"<<nConfig;
        return;
    }
    nConfig->beginGroup("PortSetting");

    QStringList portInlist = ui->serialPortInfoListBox->itemData(ui->serialPortInfoListBox->currentIndex()).toStringList();
    QStringList portOutlist = ui->AOserialPortInfoListBox->itemData(ui->AOserialPortInfoListBox->currentIndex()).toStringList();
    nConfig->setValue("OutPort", portOutlist.at(0));
    nConfig->setValue("InPort",  portInlist.at(0));
    nConfig->endGroup();
}
void SerialSettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("Name: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SerialSettingsDialog::showAOPortInfo(int idx)
{
    if (idx == -1)
        return;

    QStringList list = ui->AOserialPortInfoListBox->itemData(idx).toStringList();
    ui->AOdescriptionLabel->setText(tr("Name: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->AOmanufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->AOserialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->AOvidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->AOpidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}


void SerialSettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}



void SerialSettingsDialog::fillAoPortsParameters()
{
    ui->AObaudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->AObaudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->AObaudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->AObaudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    ui->AOdataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->AOdataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->AOdataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->AOdataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->AOdataBitsBox->setCurrentIndex(3);

    ui->AOparityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->AOparityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->AOparityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->AOparityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->AOparityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->AOstopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->AOstopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->AOstopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->AOflowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->AOflowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->AOflowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SerialSettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

}

void SerialSettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    int  nCount = 0 ;
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
            QStringList list;
            description = info.description();
            manufacturer = info.manufacturer();
            serialNumber = info.serialNumber();
            if(QString::compare(description,CUSTOM_PORT_NAME) == 0)
            {
                list << info.portName()
                     << (!description.isEmpty() ? description : blankString)
                     << (!manufacturer.isEmpty() ? manufacturer : blankString)
                     << (!serialNumber.isEmpty() ? serialNumber : blankString)
                     << info.systemLocation()
                     << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
                     << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

                ui->serialPortInfoListBox->addItem(list.first(), list);
                nCount++;
            }
    }

    ui->serialPortInfoListBox->setCurrentIndex(nCount-1);

}

bool SerialSettingsDialog::findPortByName(QString &OutPortName,QString &InPortName)
{
    bool bFindOut = false;
    bool bFindIn= false;
    int counts = ui->serialPortInfoListBox->count();
    for(int i = 0; i < counts; i++)
    {
        QStringList list = ui->serialPortInfoListBox->itemData(i).toStringList();
        QString port = list.count() > 0 ? list.at(0) : tr(blankString);
        QString description = list.count() > 1 ? list.at(1) : tr(blankString);
        if(port == InPortName && QString::compare(description,CUSTOM_PORT_NAME) == 0)
        {
            ui->serialPortInfoListBox->setCurrentIndex(i);
            bFindIn = true;
            break;
        }
    }

    counts = ui->AOserialPortInfoListBox->count();
    for(int i = 0; i < counts; i++)
    {
        QStringList list = ui->AOserialPortInfoListBox->itemData(i).toStringList();
        QString port = list.at(0);
        QString description = list.count() > 1 ? list.at(1) : tr(blankString);
        if(port == OutPortName && QString::compare(description,CUSTOM_PORT_NAME) == 0)
        {
            ui->AOserialPortInfoListBox->setCurrentIndex(i);
            bFindOut = true;
            break;
        }
    }
    return (bFindIn && bFindOut);
}

void SerialSettingsDialog::fillAoPortsInfo()
{
    ui->AOserialPortInfoListBox->clear();
    int  AOnCount = 0 ;
    QString AOdescription;
    QString AOmanufacturer;
    QString AOserialNumber;
    const auto AOinfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &AOinfo : AOinfos) {
            QStringList AOlist;
            AOdescription = AOinfo.description();
            AOmanufacturer = AOinfo.manufacturer();
            AOserialNumber = AOinfo.serialNumber();
            if(QString::compare(AOdescription,CUSTOM_PORT_NAME) == 0)
            {
                AOlist << AOinfo.portName()
                     << (!AOdescription.isEmpty() ? AOdescription : blankString)
                     << (!AOmanufacturer.isEmpty() ? AOmanufacturer : blankString)
                     << (!AOserialNumber.isEmpty() ? AOserialNumber : blankString)
                     << AOinfo.systemLocation()
                     << (AOinfo.vendorIdentifier() ? QString::number(AOinfo.vendorIdentifier(), 16) : blankString)
                     << (AOinfo.productIdentifier() ? QString::number(AOinfo.productIdentifier(), 16) : blankString);

                ui->AOserialPortInfoListBox->addItem(AOlist.first(), AOlist);
                AOnCount++;
            }
    }

    ui->AOserialPortInfoListBox->setCurrentIndex(0);
}


void SerialSettingsDialog::updateAoSettings()
{
    AoCurrentSettings.com = ui->AOserialPortInfoListBox->currentText();
    AoCurrentSettings.name = ui->AOdescriptionLabel->text().mid(6,-1);
//    qDebug()<<AoCurrentSettings.name <<ui->AOdescriptionLabel->text();
    if (ui->AObaudRateBox->currentIndex() == 4) {
        AoCurrentSettings.baudRate = ui->AObaudRateBox->currentText().toInt();
    } else {
        AoCurrentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->AObaudRateBox->itemData(ui->AObaudRateBox->currentIndex()).toInt());
    }
    AoCurrentSettings.stringBaudRate = QString::number(AoCurrentSettings.baudRate);

    AoCurrentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->AOdataBitsBox->itemData(ui->AOdataBitsBox->currentIndex()).toInt());
    AoCurrentSettings.stringDataBits = ui->AOdataBitsBox->currentText();

    AoCurrentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->AOparityBox->itemData(ui->AOparityBox->currentIndex()).toInt());
    AoCurrentSettings.stringParity = ui->AOparityBox->currentText();

    AoCurrentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->AOstopBitsBox->itemData(ui->AOstopBitsBox->currentIndex()).toInt());
    AoCurrentSettings.stringStopBits = ui->AOstopBitsBox->currentText();

    AoCurrentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->AOflowControlBox->itemData(ui->AOflowControlBox->currentIndex()).toInt());
    AoCurrentSettings.stringFlowControl = ui->AOflowControlBox->currentText();
}

void SerialSettingsDialog::updateSettings()
{
    currentSettings.com = ui->serialPortInfoListBox->currentText();
    currentSettings.name = ui->descriptionLabel->text().mid(6,-1);
//    qDebug()<<currentSettings.name <<ui->descriptionLabel->text();
    if (ui->baudRateBox->currentIndex() == 4) {
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

   // currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
}


void SerialSettingsDialog::on_updateBtn_clicked()
{
    fillPortsInfo();
}

void SerialSettingsDialog::on_connectBtn_clicked()
{
    updateSettings();
    updateAoSettings();
    emit openPortSignal(currentSettings);
    emit openAoPortSignal(AoCurrentSettings);
    hide();
}

void SerialSettingsDialog::autoConnect()
{
    emit openPortSignal(currentSettings);
    emit openAoPortSignal(AoCurrentSettings);
}


void SerialSettingsDialog::on_AOupdateBtn_clicked()
{
     fillAoPortsInfo();
}

void SerialSettingsDialog::on_disConnectBtn_clicked()
{
    emit closePortSignal();
}

void SerialSettingsDialog::on_saveBtn_clicked()
{
   SaveConfig(m_config);
   QMessageBox::information(this, tr("Port Setting "),tr("Save Port Setting Success"));
}
