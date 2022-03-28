#include "VoltCtrPanel.h"

#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QMessageBox>

#include "Global.h"
#include "Log.h"
#include "UsbFtdDevice.h"



VoltCtrPanel::VoltCtrPanel(QWidget *parent):QWidget(parent)
{
    m_vcmVolt = 1.6;
    m_dac1MidVolt = 1.6;
    m_dac0LSB = m_vcmVolt/ LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN;
    m_dac1LSB = m_vcmVolt/ LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN;
    m_dac0VoltSet = 1.6;
    m_dac1VoltSet = 0;
    m_dac1MaxInput = 0;
    m_dac1MinInput = 0;
    m_unblockVolt = 0;
    InitCtrl();
    CustomVoltRangeSetBtnSlot();
}

VoltCtrPanel::~VoltCtrPanel()
{

}

void VoltCtrPanel::InitCtrl(void)
{
    QGroupBox *pCustomInpGrpbx = new QGroupBox(QStringLiteral("Custom Dac range input:"),this);
    QLabel *pVoltRangedescLab = new QLabel(QStringLiteral("Volt range:"),this);
    QLabel *pDac0descLab = new QLabel(QStringLiteral("DAC0:"),this);
    QLabel *pDac0VoltMinInpLab = new QLabel(QStringLiteral("Min"),this);
    QLabel *pDac0VoltMaxInpLab = new QLabel(QStringLiteral("Max"),this);

    m_pDac0MinVoltdpbx = new QDoubleSpinBox(this);
    m_pDac0MaxVoltdpbx = new QDoubleSpinBox(this);
    m_pDac0MinVoltdpbx->setSuffix(QStringLiteral(" V"));
    m_pDac0MaxVoltdpbx->setSuffix(QStringLiteral(" V"));
    m_pDac0MinVoltdpbx->setRange(0,3.3);
    m_pDac0MaxVoltdpbx->setRange(0,3.3);
    m_pDac0MinVoltdpbx->setValue(0.6);
    m_pDac0MaxVoltdpbx->setValue(2.6);
    m_pDac0MinVoltdpbx->setSingleStep(0.1);
    m_pDac0MaxVoltdpbx->setSingleStep(0.1);
    m_pDac0MinVoltdpbx->setDecimals(3);
    m_pDac0MaxVoltdpbx->setDecimals(3);

    QLabel *pDac1descLab = new QLabel(QStringLiteral("DAC1:"),this);
    QLabel *pDac1VoltMinInpLab = new QLabel(QStringLiteral("Min"),this);
    QLabel *pDac1VoltMaxInpLab = new QLabel(QStringLiteral("Max"),this);

    m_pDac1MinVoltdpbx = new QDoubleSpinBox(this);
    m_pDac1MaxVoltdpbx = new QDoubleSpinBox(this);
    m_pDac1MinVoltdpbx->setSuffix(QStringLiteral(" V"));
    m_pDac1MaxVoltdpbx->setSuffix(QStringLiteral(" V"));
    m_pDac1MinVoltdpbx->setRange(0,3.3);
    m_pDac1MaxVoltdpbx->setRange(0,3.3);
    m_pDac1MinVoltdpbx->setValue(0.6);
    m_pDac1MaxVoltdpbx->setValue(2.6);
    m_pDac1MinVoltdpbx->setSingleStep(0.1);
    m_pDac1MaxVoltdpbx->setSingleStep(0.1);
    m_pDac1MinVoltdpbx->setDecimals(3);
    m_pDac1MaxVoltdpbx->setDecimals(3);
    m_pDacRangeInpAplyBtn = new QPushButton(QStringLiteral("Apply"),this);
    m_pDacReadBackBtn = new QPushButton(QStringLiteral("Read back"),this);


    QGroupBox *pVoltSetGrpbx = new QGroupBox(QStringLiteral("Voltage Setting:"),this);
    QLabel *pVoltChooseLab = new QLabel(QStringLiteral("Choose"),this);
    QLabel *pVoltTypeLab = new QLabel(QStringLiteral("type"),this);
    m_pVoltTypeComb = new QComboBox(this);
    QLabel *pVoltAmpInpLab = new QLabel(QStringLiteral("Amplitude"),this);
    m_pVoltAmpInpDpbx =new QDoubleSpinBox(this);
    m_pVoltTrianSinAmpInpDpbx =new QDoubleSpinBox(this);
    m_pVoltFreqInpLab = new QLabel(QStringLiteral("Freq"),this);
    m_pVoltFreqInpComb = new QComboBox(this);
    m_pDac0VoltApplyBtn =new QPushButton(QStringLiteral("Apply"),this);
    m_pDac0NegPosiApplyBtn =new QPushButton(QStringLiteral("+/-"),this);

    m_pVoltTrianSinAmpInpDpbx->setVisible(false);
    m_pVoltFreqInpLab->setVisible(false);
    m_pVoltFreqInpComb->setVisible(false);


    QLabel *pUnblockVoltInpLab = new QLabel(QStringLiteral("Volt"),this);
    QLabel *pUnblockDescLab = new QLabel(QStringLiteral("Degating:"),this);
    m_pUnblockVoltInpDpbx =new QDoubleSpinBox(this);
    m_pUnblockVoltApplyBtn =new QPushButton(QStringLiteral("Apply"),this);

    m_pVoltAmpInpDpbx->setRange(0,1.5);
    m_pVoltAmpInpDpbx->setSingleStep(0.1);
    m_pUnblockVoltInpDpbx->setRange(-1.5,0);
    m_pUnblockVoltInpDpbx->setSingleStep(0.1);
    m_pVoltAmpInpDpbx->setSuffix(QStringLiteral(" V"));
    m_pVoltAmpInpDpbx->setDecimals(5);

    m_pVoltTrianSinAmpInpDpbx->setRange(0,1.5);
    m_pVoltTrianSinAmpInpDpbx->setSingleStep(0.1);
    m_pVoltTrianSinAmpInpDpbx->setSuffix(QStringLiteral(" V"));
    m_pVoltTrianSinAmpInpDpbx->setDecimals(5);
    m_pVoltTrianSinAmpInpDpbx->setValue(0.05);

    m_pUnblockVoltInpDpbx->setSuffix(QStringLiteral(" V"));
    m_pUnblockVoltInpDpbx->setDecimals(3);

    m_pVoltTypeComb->addItem(QStringLiteral("Direct"),VOLTAGE_DIRECT_ENUM);
    m_pVoltTypeComb->addItem(QStringLiteral("Triangular"),VOLTAGE_TRIANGULAR_ENUM);
    m_pVoltTypeComb->addItem(QStringLiteral("Sinwave"),VOLTAGE_SINWAVE_ENUM);
    m_pVoltFreqInpComb->addItem(QStringLiteral("7.8125Hz"),VOLTAGE_FREQ_SEVEN_P_EIGHT_HZ_ENUM);

    int cusColIndx =0;
    QGridLayout *pcusInpGridlyout = new QGridLayout();
    pcusInpGridlyout->addWidget(pVoltRangedescLab,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(pDac0descLab,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(pDac0VoltMinInpLab,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(m_pDac0MinVoltdpbx,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(pDac0VoltMaxInpLab,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(m_pDac0MaxVoltdpbx,0,cusColIndx++,1,1);
    pcusInpGridlyout->addItem(new QSpacerItem(20, 10),0,cusColIndx++,1,1);

    pcusInpGridlyout->addWidget(pDac1descLab,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(pDac1VoltMinInpLab,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(m_pDac1MinVoltdpbx,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(pDac1VoltMaxInpLab,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(m_pDac1MaxVoltdpbx,0,cusColIndx++,1,1);
    pcusInpGridlyout->addItem(new QSpacerItem(20, 10),0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(m_pDacReadBackBtn,0,cusColIndx++,1,1);
    pcusInpGridlyout->addWidget(m_pDacRangeInpAplyBtn,0,cusColIndx++,1,1);
    pcusInpGridlyout->setColumnStretch(cusColIndx++,10);
    pCustomInpGrpbx->setLayout(pcusInpGridlyout);

    QHBoxLayout *horlaoutInpdpx = new QHBoxLayout();
    horlaoutInpdpx->addWidget(m_pDac0NegPosiApplyBtn);
    horlaoutInpdpx->addWidget(m_pVoltAmpInpDpbx);
    horlaoutInpdpx->addWidget(m_pVoltTrianSinAmpInpDpbx);


    QGridLayout *pVoltSetGridlayout =new QGridLayout();
    pVoltSetGridlayout->addWidget(pVoltChooseLab,1,0,1,1);
    pVoltSetGridlayout->addWidget(pVoltTypeLab,0,1,1,1);
    pVoltSetGridlayout->addWidget(m_pVoltTypeComb,1,1,1,1);
    pVoltSetGridlayout->addWidget(pVoltAmpInpLab,0,2,1,1);
    pVoltSetGridlayout->addLayout(horlaoutInpdpx,1,2,1,1);
    pVoltSetGridlayout->addWidget(m_pVoltFreqInpLab,0,3,1,1);
    pVoltSetGridlayout->addWidget(m_pVoltFreqInpComb,1,3,1,1);
    pVoltSetGridlayout->addWidget(m_pDac0VoltApplyBtn,1,4,1,1);
    pVoltSetGridlayout->addItem(new QSpacerItem(20, 10),1,5,1,1);
    pVoltSetGridlayout->addWidget(pUnblockDescLab,1,6,1,1);
    pVoltSetGridlayout->addWidget(pUnblockVoltInpLab,0,7,1,1);
    pVoltSetGridlayout->addWidget(m_pUnblockVoltInpDpbx,1,7,1,1);
    pVoltSetGridlayout->addWidget(m_pUnblockVoltApplyBtn,1,8,1,1);
    pVoltSetGridlayout->setColumnStretch(9,10);
    pVoltSetGrpbx->setLayout(pVoltSetGridlayout);

    QVBoxLayout *vertlayout = new QVBoxLayout();
    vertlayout->addWidget(pCustomInpGrpbx);
    vertlayout->addWidget(pVoltSetGrpbx);
    vertlayout->addStretch();

    setLayout(vertlayout);

    connect(m_pDacRangeInpAplyBtn,&QPushButton::clicked,this,&VoltCtrPanel::CustomVoltRangeSetBtnSlot);
    connect(m_pVoltTypeComb, QOverload<int>::of(&QComboBox::currentIndexChanged),this,&VoltCtrPanel::VoltTypeChangeSlot);
    connect(m_pDac0VoltApplyBtn,&QPushButton::clicked,this,&VoltCtrPanel::VoltApplyBtnSlot);
    connect(m_pDac0NegPosiApplyBtn,&QPushButton::clicked,this,&VoltCtrPanel::NegPosiVoltApplyBtnSlot);
    connect(m_pUnblockVoltApplyBtn,&QPushButton::clicked,this,&VoltCtrPanel::UnblockVoltApplyBtnSlot);
    connect(m_pDacReadBackBtn,&QPushButton::clicked,this,&VoltCtrPanel::OnDacReadBackByExtnADCBtnSlot);

}

void VoltCtrPanel::VoltTypeChangeSlot()
{
    bool bIsVisible = false;
    if(m_pVoltTypeComb->currentData().toInt() == VOLTAGE_DIRECT_ENUM)
        bIsVisible = false;
    else
        bIsVisible = true;
    m_pVoltFreqInpComb->setVisible(bIsVisible);
    m_pVoltFreqInpLab->setVisible(bIsVisible);
    m_pVoltTrianSinAmpInpDpbx->setVisible(bIsVisible);
    m_pVoltAmpInpDpbx->setVisible(!bIsVisible);
    m_pDac0NegPosiApplyBtn->setVisible(!bIsVisible);
}

void VoltCtrPanel::CustomVoltRangeSetBtnSlot(void)
{
    double dac0Max = m_pDac0MaxVoltdpbx->value();
    double dac0Min = m_pDac0MinVoltdpbx->value();
    m_vcmVolt = (dac0Max - dac0Min) /2 + dac0Min;
    m_dac0LSB = (dac0Max - dac0Min)/ LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN;
    m_pVoltAmpInpDpbx->setRange(dac0Min - m_vcmVolt,dac0Max - m_vcmVolt);
    m_pVoltTrianSinAmpInpDpbx->setRange(dac0Min - m_vcmVolt,dac0Max - m_vcmVolt);
    m_dac0VoltSet = m_vcmVolt - m_pVoltAmpInpDpbx->value();


    double dac1Max = m_pDac1MaxVoltdpbx->value();
    double dac1Min = m_pDac1MinVoltdpbx->value();
    m_dac1MidVolt = (dac1Max - dac1Min) /2 + dac1Min;
    m_dac1LSB = (dac1Max - dac1Min)/ LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN;
//    m_pUnblockVoltInpDpbx->setRange(-dac1Max,0);
    m_pUnblockVoltInpDpbx->setRange(dac1Min -m_dac0VoltSet ,dac1Max - m_dac0VoltSet);
}

/**
 * @brief VoltCtrPanel::VoltApplyBtnSlot
 * Normal voltage : VCM-Vdac0 = 界面输入电压AmpInput;
 * Unblock voltage: Vdac1 - Vdac0 = 界面输入电压 UnblockInput；
 */
void VoltCtrPanel::VoltApplyBtnSlot(void)
{
   if(m_pVoltTypeComb->currentData().toInt() == VOLTAGE_DIRECT_ENUM)
   {
       LOGCI("m_dac0LSB{:f}  ",m_dac0LSB);
       double voltDirect = m_pVoltAmpInpDpbx->value();
       m_dac0VoltSet = m_vcmVolt -  voltDirect;
       int16_t actualHex = CalcDACHexByDiffer(m_dac0VoltSet,m_vcmVolt,m_dac0LSB);
       LOGCI("VCMVoltSet{:f} DAC0VoltSet {:f} actural Hex {:x} ",m_vcmVolt,m_dac0VoltSet,actualHex);
       uint32_t regValue = actualHex  << 5 | 0x14;
//       uint32_t regValue =actualHex;
       m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808080);
       m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,regValue);

//       m_dac1MaxInput = m_pDac0MaxVoltdpbx->value() - m_dac0VoltSet;
//       m_pUnblockVoltInpDpbx->setRange(-m_dac1MaxInput,0);
       m_dac1MaxInput = m_pDac1MaxVoltdpbx->value() - m_dac0VoltSet;
       m_dac1MinInput = m_pDac1MinVoltdpbx->value() - m_dac0VoltSet;
       m_pUnblockVoltInpDpbx->setRange(m_dac1MinInput,m_dac1MaxInput);

       UnblockVoltApplyBtnSlot();
   }
   else if (m_pVoltTypeComb->currentData().toInt() == VOLTAGE_TRIANGULAR_ENUM)
   {
       double trianAmp = qAbs(m_pVoltTrianSinAmpInpDpbx->value());
       TriangularWaveGener(trianAmp);
   }
   else if(m_pVoltTypeComb->currentData().toInt() == VOLTAGE_SINWAVE_ENUM)
   {
       double sinAmp = qAbs(m_pVoltTrianSinAmpInpDpbx->value());
       SinWaveGener(sinAmp);
   }


}

void VoltCtrPanel::UnblockVoltApplyBtnSlot(void)
{
    m_unblockVolt = m_pUnblockVoltInpDpbx->value();
    m_dac1VoltSet = m_unblockVolt + m_dac0VoltSet;
    int16_t actualHex =  CalcDACOneHexByDiffer(m_dac1VoltSet,m_dac1MidVolt,m_dac1LSB);
    LOGCI("Unblock voltage {:f} DAC0VoltSet{:f} DAC1VoltSet {:f} actural Hex {:x}", m_unblockVolt,m_dac0VoltSet,m_dac1VoltSet, actualHex);
    uint32_t regValue = actualHex << 0;
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808080);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0B,regValue);
}


void VoltCtrPanel::NegPosiVoltApplyBtnSlot(void)
{
      m_pVoltAmpInpDpbx->setValue(m_pVoltAmpInpDpbx->value() * -1);
      VoltApplyBtnSlot();
}


int16_t VoltCtrPanel::CalcDACHexByDiffer(const double &voltSet,const double &midVolt,const double &lsb)
{
    double diffMidVolt = midVolt - voltSet;
    uint16_t numsOfVoltSet = qAbs(diffMidVolt/lsb);
    //电压是否需要反向设置
    bool bIsVoltInvert = ConfigServer::GetInstance()->GetIsVoltagteInvert();
    bool bDifferPositive  = (diffMidVolt >= FLOAT_COMPARE_POSITIVE ? true : false);

    if(numsOfVoltSet >= LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF )
    {
        if(bIsVoltInvert)
        {
            numsOfVoltSet = bDifferPositive ? LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF -1 : LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF;
        }
        else
        {
            numsOfVoltSet = bDifferPositive ? LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF : LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF -1;
        }

    }

    int16_t actualHex = 0;
    if(bIsVoltInvert)
    {
        //反向设置
        actualHex = bDifferPositive ? actualHex + numsOfVoltSet :  actualHex - numsOfVoltSet ;
    }
    else
    {
         //正常设置
        actualHex = bDifferPositive ? actualHex - numsOfVoltSet :  actualHex + numsOfVoltSet ;
    }

    return actualHex;
}

int16_t VoltCtrPanel::CalcDACOneHexByDiffer(const double &voltSet,const double &midVolt,const double &lsb)
{
    double diffMidVolt = midVolt - voltSet;
    uint16_t numsOfVoltSet = qAbs(diffMidVolt/lsb);
    bool bDifferPositive  = (diffMidVolt >= FLOAT_COMPARE_POSITIVE ? true : false);

    if(numsOfVoltSet >= LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF )
    {

        numsOfVoltSet = bDifferPositive ? LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF : LVDS_DAC_MAXHEX_TWO_POW_SIXTEEN_HALF -1;
    }

    int16_t actualHex = 0;
    actualHex = bDifferPositive ? actualHex - numsOfVoltSet :  actualHex + numsOfVoltSet ;

    return actualHex;
}

void VoltCtrPanel::TriangularWaveGener(const double &trianAmp)
{
    int16_t highAmpHex = CalcDACHexByDiffer(m_vcmVolt + trianAmp,m_vcmVolt,m_dac0LSB);
    int16_t lowAmpHex = CalcDACHexByDiffer(m_vcmVolt - trianAmp,m_vcmVolt,m_dac0LSB);
    uint16_t stepHex = qAbs((highAmpHex - lowAmpHex)/250);
    int16_t setHex = 0;
    uint32_t regValue = 0;

    LOGCI("highAmpHex {:X}  lowAmpHex {:X}  stepHex{:X} ",highAmpHex,lowAmpHex,stepHex);

    m_pUsbFtdDevice->ASICRegisterWiteOne(0x16,0x0000000C);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808000);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,0x0000000C);
    for(int cnt = -125,indx = 0; cnt < 125 ;++cnt,++indx)
    {
         setHex = 0 + cnt * stepHex;
//         LOGCI("Triangular cnt{}  setHex{:X} indx{:d}",cnt,setHex,indx);
         regValue = indx << 16 | (setHex &0x0000FFFF);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0A,regValue);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0E,0x80000000);
//         qDebug("Triangular cnt %d  setHex %X ",indx,regValue);
    }
    for(int cnt = 125,indx = 250; cnt > -125 ;--cnt,++indx)
    {
         setHex = 0 + cnt * stepHex;
//         LOGCI("Triangular cnt{}  setHex{:X} indx{:d}",cnt,setHex,indx);
         regValue = indx << 16 | (setHex &0x0000FFFF);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0A,regValue);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0E,0x80000000);
    }

    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,0x00000004);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808080);
}

void VoltCtrPanel::SinWaveGener(const double &sinAmp)
{
    int16_t highAmpHex = CalcDACHexByDiffer(m_vcmVolt + sinAmp,m_vcmVolt,m_dac0LSB);
    int16_t lowAmpHex = CalcDACHexByDiffer(m_vcmVolt - sinAmp,m_vcmVolt,m_dac0LSB);
    int16_t setHex = 0;
    uint32_t regValue = 0;
    double temp = 0;

    LOGCI("highAmpHex {:X}  lowAmpHex {:X}",highAmpHex,lowAmpHex);

    m_pUsbFtdDevice->ASICRegisterWiteOne(0x16,0x0000000C);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808000);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,0x0000000C);
    for(int cnt = 0,indx = 0; cnt < 250 ;++cnt,++indx)
    {
        temp = sin(PI_VALUE*cnt/250);
        setHex =temp *highAmpHex;
//        LOGCI("sinwave  temp{:f} cnt{} indx{:d}  setHex{:X} ",temp,cnt,indx,setHex);
        regValue = indx << 16 | (setHex &0x0000FFFF);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0A,regValue);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0E,0x80000000);
    }
    for(int cnt = 250,indx = 250; cnt < 500 ;++cnt,++indx)
    {
        temp = sin(PI_VALUE*cnt*-1.0/250);
        setHex = temp*lowAmpHex;
//        LOGCI("sinwave  temp{:f} cnt{}  setHex{:X} indx{:d}",temp,cnt,setHex,indx);
        regValue = indx << 16 | (setHex &0x0000FFFF);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0A,regValue);
         m_pUsbFtdDevice->ASICRegisterWiteOne(0x0E,0x80000000);
    }

    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,0x00000004);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808080);
}


void VoltCtrPanel::TrianStartAndGetAmplitudeSlot(const bool bStart,double &amplitude)
{
    if(bStart)
    {
        m_pVoltTypeComb->setCurrentIndex(VOLTAGE_TRIANGULAR_ENUM);
        amplitude = m_pVoltTrianSinAmpInpDpbx->value();
        VoltApplyBtnSlot();
    }
    else
    {
        m_pVoltTypeComb->setCurrentIndex(VOLTAGE_DIRECT_ENUM);
//        amplitude = m_pVoltTrianSinAmpInpDpbx->value();
        VoltApplyBtnSlot();
    }
}

void VoltCtrPanel::DegatVoltSetSlot(const float &negativeVolt)
{
    m_pVoltTypeComb->setCurrentIndex(VOLTAGE_DIRECT_ENUM);
    m_pUnblockVoltInpDpbx->setValue(negativeVolt);
    UnblockVoltApplyBtnSlot();
}
void VoltCtrPanel::VoltageSetSlot(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double &freq)
{
#if 0
    m_pVoltTypeComb->setCurrentIndex(type);
    switch (type) {
    case VOLTAGE_DIRECT_ENUM:
        m_pVoltAmpInpDpbx->setValue(amplitude);
        break;
    case VOLTAGE_TRIANGULAR_ENUM:
    case VOLTAGE_SINWAVE_ENUM:
        m_pVoltTrianSinAmpInpDpbx->setValue(amplitude);
        break;
    default:
        break;
    }
#endif
    VoltSetParameters(type,amplitude,freq);
    VoltApplyBtnSlot();
}


void VoltCtrPanel::VoltSetParameters(const VOLTAGE_TYPE_ENUM &type,const double &amplitude,const double freq)
{
    Q_UNUSED(freq);
    m_pVoltTypeComb->setCurrentIndex(type);
    switch (type) {
    case VOLTAGE_DIRECT_ENUM:
        m_pVoltAmpInpDpbx->setValue(amplitude);
        break;
    case VOLTAGE_TRIANGULAR_ENUM:
    case VOLTAGE_SINWAVE_ENUM:
        m_pVoltTrianSinAmpInpDpbx->setValue(amplitude);
        break;
    default:
        break;
    }
}

void VoltCtrPanel::EnablePolymerSimulationSlot(const bool &enable)
{
    setEnabled(!enable);
    m_pVoltTypeComb->setCurrentIndex(VOLTAGE_DIRECT_ENUM);
}

void VoltCtrPanel::EnableSimulationSlot(const bool &enable)
{
    setEnabled(!enable);
    m_pVoltTypeComb->setCurrentIndex(VOLTAGE_DIRECT_ENUM);
    m_pVoltAmpInpDpbx->setValue(0);
    m_pUnblockVoltInpDpbx->setValue(0);
    VoltApplyBtnSlot();

}

void VoltCtrPanel::SetSimulationVoltSlot(const float &voltage,const MEMBRANE_SIMULATION_ENUM &type)
{
    if(type == MEMBRANE_SIMULATION_ELECTROCHEMISTRY)
    {
        float halfVolt = voltage/2;

        m_dac0VoltSet = m_vcmVolt -  halfVolt;
        int16_t DAC0actualHex = CalcDACHexByDiffer(m_dac0VoltSet,m_vcmVolt,m_dac0LSB);
    //    LOGCI("VCMVoltSet{:f} DAC0VoltSet {:f} actural Hex {:x} ",m_vcmVolt,m_dac0VoltSet,DAC0actualHex);
        uint32_t dac0RegValue = DAC0actualHex  << 5 | 0x14;

        m_dac1VoltSet = voltage + m_dac0VoltSet;
        int16_t DAC1actualHex =  CalcDACOneHexByDiffer(m_dac1VoltSet,m_dac1MidVolt,m_dac1LSB);
    //    LOGCI("DAC0VoltSet{:f} DAC1VoltSet {:f} actural Hex {:x}", m_dac0VoltSet,m_dac1VoltSet, DAC1actualHex);
        uint32_t dac1regValue = DAC1actualHex << 0;

        m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808080);
        m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,dac0RegValue);
        m_pUsbFtdDevice->ASICRegisterWiteOne(0x0B,dac1regValue);
    }
    else if(type == MEMBRANE_SIMULATION_POLYMER)
    {
        m_pVoltAmpInpDpbx->setValue(voltage);
//        m_pUnblockVoltInpDpbx->setValue(0);
        VoltApplyBtnSlot();
    }
}


void VoltCtrPanel::OnDacReadBackByExtnADCBtnSlot(void)
{
    float dac0MinVolt = 0.0;
    float dac0MaxVolt = 0.0;
    float dac1MinVolt = 0.0;
    float dac1MaxVolt = 0.0;

    //将所有通道置为off
    emit AllChanSensorStateSetOffSig();

    if(!DacReadBackByExtnADC(dac0MinVolt,dac0MaxVolt,dac1MinVolt,dac1MaxVolt))
    {
        QMessageBox::critical(this, tr("Read back ext adc failed!!!"), tr("Read back ext adc failed !!!"));
        return;
    }

    m_pDac0MinVoltdpbx->setValue(dac0MinVolt);
    m_pDac0MaxVoltdpbx->setValue(dac0MaxVolt);
    m_pDac1MinVoltdpbx->setValue(dac1MinVolt);
    m_pDac1MaxVoltdpbx->setValue(dac1MaxVolt);

    CustomVoltRangeSetBtnSlot();

    //将正常电压设置0，unblock电压设置为0
    m_pVoltAmpInpDpbx->setValue(0);
    m_pUnblockVoltInpDpbx->setValue(0);
    VoltApplyBtnSlot();

}


bool VoltCtrPanel::DacReadBackByExtnADC(float &dac0MinVolt,float &dac0MaxVolt,float &dac1MinVolt,float &dac1MaxVolt)
{

    //电压是否需要反向设置
    bool bVoltInvert = ConfigServer::GetInstance()->GetIsVoltagteInvert();

    //设置DAC0 DAC1 最小值
    uint32_t DAC0minVoltHex = 0x8000;
    uint32_t DAC1minVoltHex = 0x8000;
    DAC0minVoltHex = bVoltInvert ? 0x7FFF: 0x8000;//是否需要反向设置

    uint32_t dac0RegValue = DAC0minVoltHex  << 5 | 0x14;
    uint32_t dac1regValue = DAC1minVoltHex << 0;

    m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808080);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,dac0RegValue);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0B,dac1regValue);

    QEventLoop eventloop;
    QTimer::singleShot(10, &eventloop, SLOT(quit()));
    eventloop.exec();

    static bool bIsIntial = true;
    //设置外部ADC
    qulonglong lData = 0;
    if(bIsIntial)
    {
        bIsIntial = false;
        lData =0x0000000088aa;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x000000008aaa;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x000000008caa;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x000000008eaa;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x00000000c000;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x00000000c211;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x00000000c422;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);

        lData =0x00000000c633;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x00000000c844;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x00000000ca55;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x00000000cc66;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x000000008cf77;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x000000008464;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);
        lData =0x000000000000;
        m_pUsbFtdDevice->SendForExtADCRegisterWriteOne(lData);

    }

    lData =0xff0000000000;
    m_pUsbFtdDevice->SendForExtADCRegisterReadOne(lData);


    UCHAR acReadBuf[BUFFER_SIZE] = {0};
    memset(acReadBuf,0x0,BUFFER_SIZE);
    ULONG ulActualBytesToRead = 0;
    ULONG ulBytesToRead = sizeof(acReadBuf);

    if(m_pUsbFtdDevice->ReadDataASync(acReadBuf,ulBytesToRead,&ulActualBytesToRead))
    {
#if 0
        qDebug("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<Plan to read bytes: %d; recvdata actual bytes: %d ",ulBytesToRead,ulActualBytesToRead);
        for(uint i = 0 ; i < ulActualBytesToRead;++i)
        {
            if(i%12 == 0)
            {
                printf("\n");
            }
            printf(" 0x%02X",acReadBuf[i]);
        }
        printf("\n");
#endif

        if(ulActualBytesToRead >= 48)
        {
            UCHAR *headPtr =  &acReadBuf[12];
            if(*headPtr == 0x5A && *(headPtr+1) == 0x5A && *(headPtr+2) == 0x03 && *(headPtr+10) == 0xFF && *(headPtr+11) == 0xAA)
            {
                //先A引脚再B引脚
                printf("dac1HexRb high: 0x%02X low: 0x%02X \n",*(headPtr + 8),*(headPtr + 9));
                uint16_t dac1HexRbu = (uint16_t)(*(headPtr + 8) << 8) + (uint16_t)(*(headPtr + 9));
                int16_t dac1HexRb = (int16_t)dac1HexRbu;
                float dac1Volt = dac1HexRb *1.0 /32768 * 5.0;
                dac1MinVolt = dac1Volt;
                printf("dac1HexRbu 0x%04X dac1HexRb %d, dac1Volt %fV\n",dac1HexRbu,dac1HexRb,dac1Volt);
            }

            headPtr =  &acReadBuf[36];
            if(*headPtr == 0x5A && *(headPtr+1) == 0x5A && *(headPtr+2) == 0x03 && *(headPtr+10) == 0xFF && *(headPtr+11) == 0xAA)
            {
                //先A引脚再B引脚
                printf("dac0HexRb high: 0x%02X low: 0x%02X \n",*(headPtr + 8),*(headPtr + 9));
                uint16_t dac0HexRbu = (uint16_t)(*(headPtr + 8) << 8) + (uint16_t)(*(headPtr + 9));
                int16_t dac0HexRb = (int16_t)dac0HexRbu;
                float dac0Volt = dac0HexRb *1.0 /32768 * 5.0;
                dac0MinVolt = dac0Volt;
                printf("dac0HexRbu 0x%04X dac0HexRb %d, dac0Volt %fV\n",dac0HexRbu,dac0HexRb,dac0Volt);
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    //设置DAC0 DAC1 最大值
    qDebug("Setting dac0,dac1 max voltage");
    uint32_t DAC0maxVoltHex = 0x7FFF;
    uint32_t DAC1maxVoltHex = 0x7FFF;
    DAC0maxVoltHex = bVoltInvert ? 0x8000 : 0x7FFF;//是否需要反向设置

    uint32_t dac0RegMaxValue = DAC0maxVoltHex  << 5 | 0x14;
    uint32_t dac1regMaxValue = DAC1maxVoltHex << 0;

    m_pUsbFtdDevice->ASICRegisterWiteOne(0x09,0x00808080);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0D,dac0RegMaxValue);
    m_pUsbFtdDevice->ASICRegisterWiteOne(0x0B,dac1regMaxValue);
    QEventLoop eventloop1;
    QTimer::singleShot(10, &eventloop1, SLOT(quit()));
    eventloop1.exec();

    lData =0xff0000000000;
    m_pUsbFtdDevice->SendForExtADCRegisterReadOne(lData);

    memset(acReadBuf,0x0,BUFFER_SIZE);
    ulActualBytesToRead = 0;

    if(m_pUsbFtdDevice->ReadDataASync(acReadBuf,ulBytesToRead,&ulActualBytesToRead))
    {
#if 0
        qDebug("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<Plan to read bytes: %d; recvdata actual bytes: %d ",ulBytesToRead,ulActualBytesToRead);
        for(uint i = 0 ; i < ulActualBytesToRead;++i)
        {
            if(i%12 == 0)
            {
                printf("\n");
            }
            printf(" 0x%02X",acReadBuf[i]);
        }
        printf("\n");
#endif
        if(ulActualBytesToRead >= 48)
        {
            UCHAR *headPtr =  &acReadBuf[12];
            if(*headPtr == 0x5A && *(headPtr+1) == 0x5A && *(headPtr+2) == 0x03 && *(headPtr+10) == 0xFF && *(headPtr+11) == 0xAA)
            {
                //先B再A
                printf("dac1HexRb high: 0x%02X low: 0x%02X \n",*(headPtr + 8),*(headPtr + 9));
                uint16_t dac1HexRbu = (uint16_t)(*(headPtr + 8) << 8) + (uint16_t)(*(headPtr + 9));
                int16_t dac1HexRb = (int16_t)dac1HexRbu;
                float dac1Volt = dac1HexRb *1.0 /32768 * 5.0;
                dac1MaxVolt = dac1Volt;
                printf("dac1HexRbu 0x%04X dac1HexRb %d, dac1Volt %fV\n",dac1HexRbu,dac1HexRb,dac1Volt);
            }

            headPtr =  &acReadBuf[36];
            if(*headPtr == 0x5A && *(headPtr+1) == 0x5A && *(headPtr+2) == 0x03 && *(headPtr+10) == 0xFF && *(headPtr+11) == 0xAA)
            {
                //先B再A
                printf("dac0HexRb high: 0x%02X low: 0x%02X \n",*(headPtr + 8),*(headPtr + 9));
                uint16_t dac0HexRbu = (uint16_t)(*(headPtr + 8) << 8) + (uint16_t)(*(headPtr + 9));
                int16_t dac0HexRb = (int16_t)dac0HexRbu;
                float dac0Volt = dac0HexRb *1.0 /32768 * 5.0;
                dac0MaxVolt = dac0Volt;
                printf("dac0HexRbu 0x%04X dac0HexRb %d, dac0Volt %fV\n",dac0HexRbu,dac0HexRb,dac0Volt);
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;


}
void VoltCtrPanel::VoltInital(void)
{
    VoltageSetSlot(VOLTAGE_DIRECT_ENUM,0,0);
    DegatVoltSetSlot(-0.15);
}
