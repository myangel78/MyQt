#include "UsbCyDevice.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <CyApi.h>

#include "ConfigServer.h"


#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")



UsbCyDevice::UsbCyDevice()
{
    m_vetInterestDevice.emplace_back(0x04B4, 0x00F3, "BGI Cyclone LAB256 Pro Bootloader");
    m_vetInterestDevice.emplace_back(0x04B4, 0x00F1, "BGI Cyclone LAB256 Test");
    m_vetInterestDevice.emplace_back(0x0451, 0xAF01, "BGI Cyclone LAB256 Pro");
}

UsbCyDevice::~UsbCyDevice()
{
    SAFE_DELETEP(m_pUsbDevie);
}

inline bool UsbCyDevice::isInterestDevice(unsigned short vid, unsigned short pid, std::string& nam)
{
    for (const auto& ite : m_vetInterestDevice)
    {
        if (std::get<0>(ite) == vid && std::get<1>(ite) == pid)
        {
            nam = std::get<2>(ite);
            return true;
        }
    }
    return false;
}

bool UsbCyDevice::ControlPointRead(PUCHAR buf, LONG& len)
{
    if (m_pUsbDevie == nullptr)
    {
        return false;
    }
    CCyControlEndPoint* CtrlEndPt = m_pUsbDevie->ControlEndPt;
    if (CtrlEndPt == nullptr)
    {
        return false;
    }
    CtrlEndPt->Target = TGT_DEVICE;
    CtrlEndPt->ReqType = REQ_VENDOR;
    CtrlEndPt->Direction = DIR_FROM_DEVICE;
    CtrlEndPt->ReqCode = 0x00;
    CtrlEndPt->Value = 0x00;
    CtrlEndPt->Index = 0x00;
    bool success = CtrlEndPt->XferData(buf, len);
    return success;
}

bool UsbCyDevice::ControlPointWrite(PUCHAR buf, LONG& len)
{
    if (m_pUsbDevie == nullptr)
    {
        return false;
    }
    CCyControlEndPoint* CtrlEndPt = m_pUsbDevie->ControlEndPt;
    if (CtrlEndPt == nullptr)
    {
        return false;
    }
    CtrlEndPt->Target = TGT_DEVICE;
    CtrlEndPt->ReqType = REQ_VENDOR;
    CtrlEndPt->Direction = DIR_TO_DEVICE;
    CtrlEndPt->ReqCode = 0x00;
    CtrlEndPt->Value = 0x00;
    CtrlEndPt->Index = 0x00;
    bool success = CtrlEndPt->XferData(buf, len);
    return success;
}

CCyUSBDevice* UsbCyDevice::GetCurDeviceHandle(void) const
{
    return m_pUsbDevie;
}

int UsbCyDevice::SearchDeviceInfo(HANDLE hwnd, std::vector<std::tuple<std::string, USHORT, USHORT>>& vetOut)
{
    CCyUSBDevice* USBDevice = new CCyUSBDevice(hwnd, CYUSBDRV_GUID, true);
    if (USBDevice != NULL)
    {
        int nDeviceCount = USBDevice->DeviceCount();
        std::string vectorId;
        std::string productId;
        std::string friendlyName;
        for (int nCount = 0; nCount < nDeviceCount; nCount++)
        {
            friendlyName = std::string(USBDevice->FriendlyName);
            //char arTemp[16] = { 0 };
            //vectorId = std::string(::_itoa(USBDevice->VendorID, arTemp, 16));
            //std::transform(vectorId.begin(), vectorId.end(), vectorId.begin(), ::toupper);//tolower
            //productId = std::string(::_itoa(USBDevice->ProductID, arTemp, 16));
            //std::transform(productId.begin(), productId.end(), productId.begin(), ::toupper);//tolower
            bool bf = isInterestDevice(USBDevice->VendorID, USBDevice->ProductID, friendlyName);
            vetOut.emplace_back(friendlyName, USBDevice->VendorID, USBDevice->ProductID);

        }
        USBDevice->Close();
        delete USBDevice;
    }

    return 0;
}

bool UsbCyDevice::OpenUsbDevice(USHORT vendorID, USHORT productID)
{
    m_pUsbDevie = new CCyUSBDevice();
    if (m_pUsbDevie != nullptr)
    {
        int deviceCount = m_pUsbDevie->DeviceCount();
        for (int cnt = 0; cnt < deviceCount; ++cnt)
        {
            bool bopen = m_pUsbDevie->Open(cnt);
            if (!bopen)
            {
                continue;
            }
            if (m_pUsbDevie->VendorID == vendorID && m_pUsbDevie->ProductID == productID)
            {
                int intfc = m_pUsbDevie->AltIntfcCount() + 1;
                //bool highSpeedDev = m_pUsbDevie->bHighSpeed;
                //bool superSpeedDev = m_pUsbDevie->bSuperSpeed;

                for (int i = 0; i < intfc; ++i)
                {
                    bopen = m_pUsbDevie->SetAltIntfc(i);
                    auto epcnt = m_pUsbDevie->EndPointCount();
                    for (int j = 1; j < epcnt; ++j)
                    {
                        CCyUSBEndPoint* puep = m_pUsbDevie->EndPoints[j];
                        if (puep->Attributes >= 1 && puep->Attributes <= 3)
                        {
                            //auto attr = puep->Attributes;
                            //auto bin = puep->bIn;
                            //auto maxpktsize = puep->MaxPktSize;
                            //auto maxburst = m_pUsbDevie->BcdUSB == 0x0300 ? puep->ssmaxburst : 0;
                            //auto index = i;
                            //auto address = puep->Address;

                            bool binep = ((puep->Address & 0x81) == 0x81);
                            bool bbulk = (puep->Address == 2);
                            if (bbulk)
                            {
                                if (binep)
                                {
                                    m_pBulkInEpt = dynamic_cast<CCyBulkEndPoint*>(puep);
                                }
                                else
                                {
                                    m_pBulkOutEpt = dynamic_cast<CCyBulkEndPoint*>(puep);
                                }
                            }
                        }
                    }
                }
                return true;
            }
            m_pUsbDevie->Close();
        }
        SAFE_DELETEP(m_pUsbDevie);
    }
    return false;
}

bool UsbCyDevice::CloseUsbDevice(void)
{
    if (nullptr != m_pUsbDevie)
    {
        if (m_pUsbDevie->IsOpen())
        {
            m_pUsbDevie->Close();
        }

        SAFE_DELETEP(m_pUsbDevie);

        return true;
    }
    return false;
}

bool UsbCyDevice::DeviceIsOpen(void)
{
    if (m_pUsbDevie != nullptr)
    {
        return m_pUsbDevie->IsOpen();
    }
    return false;
}

bool UsbCyDevice::DownloadFirmware(const char* pFile)
{
    std::shared_ptr<CCyFX3Device> pCCyFX3Device = std::make_shared<CCyFX3Device>();
    if (nullptr != pCCyFX3Device)
    {
        std::string friendlyName(pCCyFX3Device->FriendlyName);
        std::transform(friendlyName.begin(), friendlyName.end(), friendlyName.begin(), ::tolower);//toupper
        if (friendlyName.find("bootloader", 0) != std::string::npos)
        {
            if (pCCyFX3Device->IsBootLoaderRunning())
            {
                int retCode = pCCyFX3Device->DownloadFw(pFile, FX3_FWDWNLOAD_MEDIA_TYPE::RAM);
                if (FX3_FWDWNLOAD_ERROR_CODE::SUCCESS == retCode)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool UsbCyDevice::LoadDevRegister(void)
{
    int m_tscan;
    int m_AutoRev;
    int m_isolate_panel;
    int m_Addcap;
    int m_lownoise;
    int m_en_tdef;
    int m_intg_down;
    int m_lpf;
    int m_CFb;
    double m_QFb;

    m_tscan = 3;
    m_AutoRev = 0;
    m_isolate_panel = 0;
    m_Addcap = 0;
    m_lownoise = 0;
    m_en_tdef = 1;
    m_intg_down = 0;
    m_lpf = 0;
    m_CFb = 0;
    m_QFb = 0;

    m_tscan = 3;
    m_Addcap = 1;
    m_lownoise = 1;
    m_lpf = 15;
    m_CFb = 0;

    AFERegisterWrite(0x00, 0x0001); //# Software Reset
    Sleep(50);
    AFERegisterWrite(0x30, 0x0002); //#fuse acq
    Sleep(50);
    AFERegisterWrite(0x30, 0x0000);
    Sleep(50);
    AFERegisterWrite(0x11, (m_tscan << 4) | (m_AutoRev << 13) | (1 << 10));
    AFERegisterWrite(0x5D, (m_isolate_panel << 14) | (m_Addcap << 11) | (m_en_tdef << 10) | (m_lownoise << 1));
    AFERegisterWrite(0x5E, m_intg_down << 12);
    AFERegisterWrite(0x10, 0x0800);
    AFERegisterWrite(18, 0x4000);
    AFERegisterWrite(22, 0x00C0);
    Sleep(50);

    AFERegisterWrite(0x61, 1 << 14);
    if (m_lownoise == 1)
        AFERegisterWrite(22, 0x00C8);
    AFERegisterWrite(0x18, 0x0001);
    Sleep(50);
    if (m_CFb == 0)
    {
        AFERegisterWrite(0x5C, (16 << 11) | (m_lpf << 6) | (m_lpf << 1));
        AFERegisterWrite(0x62, 0x0100);
        AFERegisterWrite(44, 1);
        AFERegisterWrite(18, 0x0000);
        AFERegisterWrite(22, 0x99C0);

    }
    else if (m_CFb == 1)
    {
        AFERegisterWrite(0x5C, (16 << 11) | (m_lpf << 6) | (m_lpf << 1));
        AFERegisterWrite(0x62, 0x0100);
        AFERegisterWrite(44, 1);
        AFERegisterWrite(18, 0x0000);
    }
    else if (m_CFb == 2)
        AFERegisterWrite(0x5C, (1 << 11) | (m_lpf << 6) | (m_lpf << 1));
    else if (m_CFb == 3)
        AFERegisterWrite(0x5C, (2 << 11) | (m_lpf << 6) | (m_lpf << 1));
    else if (m_CFb == 4)
        AFERegisterWrite(0x5C, (4 << 11) | (m_lpf << 6) | (m_lpf << 1));
    else if (m_CFb == 5)
        AFERegisterWrite(0x5C, (8 << 11) | (m_lpf << 6) | (m_lpf << 1));
    else if (m_CFb == 6)
        AFERegisterWrite(0x5C, (12 << 11) | (m_lpf << 6) | (m_lpf << 1));
    else if (m_CFb == 7)
        AFERegisterWrite(0x5C, (31 << 11) | (m_lpf << 6) | (m_lpf << 1));
    Sleep(50);

    FPGARegisterWrite(5, 0 << 2);
    FPGARegisterWrite(5, (0 << 2) | (2));
    FPGARegisterWrite(5, 0 << 2);
    Sleep(50);

    AFERegisterWrite(0x40, 0x0105);
    AFERegisterWrite(0x42, 0x066C);
    AFERegisterWrite(0x43, 0x6DFE);
    AFERegisterWrite(0x46, 0x106D);
    AFERegisterWrite(0x47, 0xA2FF);
    AFERegisterWrite(0x4A, 0x069D);
    AFERegisterWrite(0x50, 0);
    AFERegisterWrite(0x51, 0);
    AFERegisterWrite(0x52, 0);
    AFERegisterWrite(0x53, 0);
    AFERegisterWrite(0x54, 0);
    AFERegisterWrite(0x55, 0);
    Sleep(50);

    FPGARegisterWrite(5, 1 << 2); //2:TPSEL, 1:Sync
    FPGARegisterWrite(5, (1 << 2) | (2));
    FPGARegisterWrite(5, 1 << 2);
    Sleep(50);

    AFERegisterWrite(16, 0x0800);
    AFERegisterWrite(16, 0x0A00);
    AFERegisterWrite(16, 0x0BC0);
    Sleep(1);
    FPGARegisterWrite(2, (1 << 30) | (1 << 31));
    Sleep(1);
    FPGARegisterWrite(2, 0);
    AFERegisterWrite(16, 0x09C0);
    AFERegisterWrite(16, 0x0800);
    Sleep(1);

#define ONLYAFEB 1
#ifdef ONLYAFEB
    FPGARegisterWrite(2, 0x00000000);
    Sleep(50);
    FPGARegisterWrite(4, 0x00000000);
    Sleep(50);
    FPGARegisterWrite(3, 0x00000004);
    Sleep(50);
    FPGARegisterWrite(3, 0x007FFFF9);
    Sleep(50);
    FPGARegisterWrite(3, 0x00000004);
    Sleep(50);
    FPGARegisterWrite(3, 0x007FFFFB);
#endif

    return true;
}

int UsbCyDevice::FPGARegisterRead(USHORT addr, LONG datalen)
{
    UCHAR buf[] = { 0xFF, 0xAA, 0x01, 0x07, 0x00, 0x00, 0x00, 0x02, (addr & 0xff), (addr >> 8) & 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    datalen = 16;
    ControlPointWrite(buf, datalen);

    UCHAR rbuf[16] = { 0 };
    LONG rlen = sizeof(rbuf) / sizeof(rbuf[0]);
    ControlPointRead(rbuf, rlen);

    return rlen;
}

int UsbCyDevice::FPGARegisterWrite(USHORT addr, UINT data)
{
    LONG len = 16;
    UCHAR buf[] = { 0xFF, 0xAA, 0x01, 0x07, 0x00, 0x00, 0x00, 0x1, (addr & 0xff), 00, data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff, (data >> 24) & 0xff, 0x00, 0x00 };
    ControlPointWrite(buf, len);
    //Sleep(150); //这个延时很关键，去掉或小于50ms找不到入口
    return len;
}

void UsbCyDevice::FPGARegisterReadBack(unsigned short addr, unsigned int& rValue, bool& result)
{
    result = false;
    UCHAR buf[] = { 0xFF, 0xAA, 0x01, 0x07, 0x00, 0x00, 0x00, 0x02, (addr & 0xff), (addr >> 8) & 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    LONG datalen = 16;
    ControlPointWrite(buf, datalen);

    UCHAR rbuf[16] = { 0 };
    LONG rlen = sizeof(rbuf) / sizeof(rbuf[0]);
    if (ControlPointRead(rbuf, rlen) == true)
    {
        rValue = 0;
        if (rbuf[3] == 0x05 && rlen >= 8)
        {
            rValue = rbuf[4] | (rbuf[5] << 8) | (rbuf[6] << 16) | (rbuf[7] << 24);
            result = true;
        }
    }
}

int UsbCyDevice::AFERegisterRead(USHORT addr, LONG datalen)
{
    //TODO:
    return 0;
}

int UsbCyDevice::AFERegisterWrite(USHORT addr, LONG data)
{
    LONG len = 14;
    UCHAR buf[] = { 0xFF, 0xAA, 0x03, 0x05, 0x00, 0x00, 0x00, 0x11, 0x03, data & 0xff, (data >> 8) & 0xff, addr, 0x00, 0x00 };
    ControlPointWrite(buf, len);
    return len;
}

