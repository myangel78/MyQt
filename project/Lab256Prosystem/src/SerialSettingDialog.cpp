#include "SerialSettingDialog.h"

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>

SerialSettingDialog::SerialSettingDialog(QWidget* parent) :QDialog(parent)
{
    InitCtrl();
}

SerialSettingDialog::~SerialSettingDialog()
{

}

void SerialSettingDialog::InitCtrl(void)
{
    m_pSettingsGrpbox = new QGroupBox(QStringLiteral("Settings"), this);

    m_pDataBitLab = new QLabel(QStringLiteral("DataBits"), m_pSettingsGrpbox);
    m_pPriorityLab = new QLabel(QStringLiteral("Priority"), m_pSettingsGrpbox);
    m_pStopbitLab = new QLabel(QStringLiteral("StopBit"), m_pSettingsGrpbox);
    m_pFlowControlLab = new QLabel(QStringLiteral("FlowControl"), m_pSettingsGrpbox);


    m_pDataBitComb = new QComboBox(m_pSettingsGrpbox);
    m_pPriorityComb = new QComboBox(m_pSettingsGrpbox);
    m_pStopbitComb = new QComboBox(m_pSettingsGrpbox);
    m_pFlowControlComb = new QComboBox(m_pSettingsGrpbox);

    m_pUpdateBtn = new QPushButton(QStringLiteral("Ok"), m_pSettingsGrpbox);

    m_pDataBitComb->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_pDataBitComb->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_pDataBitComb->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_pDataBitComb->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_pDataBitComb->setCurrentIndex(3);

    m_pPriorityComb->addItem(tr("None"), QSerialPort::NoParity);
    m_pPriorityComb->addItem(tr("Even"), QSerialPort::EvenParity);
    m_pPriorityComb->addItem(tr("Odd"), QSerialPort::OddParity);
    m_pPriorityComb->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_pPriorityComb->addItem(tr("Space"), QSerialPort::SpaceParity);

    m_pFlowControlComb->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_pFlowControlComb->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_pFlowControlComb->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

    m_pStopbitComb->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_pStopbitComb->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_pStopbitComb->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    QGridLayout* gridlayout = new QGridLayout();
    gridlayout->addWidget(m_pDataBitLab, 0, 0, 1, 1);
    gridlayout->addWidget(m_pDataBitComb, 0, 1, 1, 1);
    gridlayout->addWidget(m_pPriorityLab, 1, 0, 1, 1);
    gridlayout->addWidget(m_pPriorityComb, 1, 1, 1, 1);
    gridlayout->addWidget(m_pStopbitLab, 2, 0, 1, 1);
    gridlayout->addWidget(m_pStopbitComb, 2, 1, 1, 1);
    gridlayout->addWidget(m_pFlowControlLab, 3, 0, 1, 1);
    gridlayout->addWidget(m_pFlowControlComb, 3, 1, 1, 1);
    gridlayout->addWidget(m_pUpdateBtn, 4, 1, 1, 1);
    m_pSettingsGrpbox->setLayout(gridlayout);

    QVBoxLayout* vertlayout = new QVBoxLayout();
    vertlayout->addWidget(m_pSettingsGrpbox);
    setLayout(vertlayout);

    connect(m_pUpdateBtn, SIGNAL(clicked()), this, SLOT(OnClickOkBtnSlot()));


}


SerialSettingDialog::SerialSettings SerialSettingDialog::Settings() const
{
    return m_pCurrentSettings;
}

void SerialSettingDialog::OnUpdateParam(void)
{
    m_pCurrentSettings.dataBits = static_cast<QSerialPort::DataBits>(
        m_pDataBitComb->itemData(m_pDataBitComb->currentIndex()).toInt());
    m_pCurrentSettings.parity = static_cast<QSerialPort::Parity>(
        m_pPriorityComb->itemData(m_pPriorityComb->currentIndex()).toInt());
    m_pCurrentSettings.stopBits = static_cast<QSerialPort::StopBits>(
        m_pStopbitComb->itemData(m_pStopbitComb->currentIndex()).toInt());
    m_pCurrentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
        m_pFlowControlComb->itemData(m_pFlowControlComb->currentIndex()).toInt());
}

void SerialSettingDialog::OnClickOkBtnSlot(void)
{
    OnUpdateParam();
    this->close();
}
