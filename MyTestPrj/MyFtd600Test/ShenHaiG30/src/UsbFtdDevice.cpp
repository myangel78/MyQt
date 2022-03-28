#include "UsbFtdDevice.h"
#include <QDebug>
//#include <initguid.h>
#include "ConfigServer.h"
#include "UsbProtoMsg.h"


UsbFtdDevice::UsbFtdDevice()
{
}

UsbFtdDevice::~UsbFtdDevice()
{
    CloseDevice();
}

bool UsbFtdDevice::OnSearchUsbDevice(QStringList &devDescrlist)
{
    DWORD dwNumDevices = 0;
    FT_DEVICE_LIST_INFO_NODE *ptDevicesInfo = nullptr;
    devDescrlist.clear();


    dwNumDevices = GetNumberOfDevicesConnected();
    if (dwNumDevices == 0)
    {
        qDebug("No device is connected to the machine! Please connect a device.");
        return false;
    }

    if (GetDevicesInfoList(&ptDevicesInfo) == 0)
    {
        qDebug("No device is connected!");
        return false;
    }

    qDebug("List of Connected Devices!");
    for (DWORD i = 0; i < dwNumDevices; i++)
    {
        devDescrlist.append(QString::fromLocal8Bit((ptDevicesInfo[i].Description)));
    }

    DisplayDeviceInfoList();

    return true;
}


bool UsbFtdDevice::OpenDeviceByDescr(QString description)
{
    FT_STATUS ftStatus = FT_OK;
    FT_DEVICE_LIST_INFO_NODE *ptDeviceInfo = NULL;
    DWORD dwNumDevices = 0;
    DWORD dwDeviceIndex = 0xFFFFFFFF;


    dwNumDevices = GetNumberOfDevicesConnected();
    if (dwNumDevices == 0)
    {
        qDebug("No device is connected!");
        return false;
    }

    if (GetDevicesInfoList(&ptDeviceInfo) == 0)
    {
        qDebug("No device is connected!");
        return false;
    }

    for (DWORD i = 0; i < dwNumDevices; i++)
    {
        if (strcmp(ptDeviceInfo[i].Description, description.toStdString().c_str()) == 0)
        {
            dwDeviceIndex = i;
            break;
        }
    }

    if(dwDeviceIndex == 0xFFFFFFFF)
    {
        qDebug("Device with Description [{}] not found!",description.toStdString().c_str());
        free(ptDeviceInfo);
        ptDeviceInfo = nullptr;
        return false;
    }

    char Description[32] = { 0 };
    if (!GetDeviceDescription(dwDeviceIndex, Description))
    {
        return false;
    }

    ftStatus = FT_Create(Description, FT_OPEN_BY_DESCRIPTION, &m_ftHandle);
    if (FT_FAILED(ftStatus))
    {
        m_ftHandle = nullptr;
        return false;
    }
//    ftStatus = FT_SetPipeTimeout(m_ftHandle, 0x82,1000);
//    if (FT_FAILED(ftStatus))
//    {
//        m_ftHandle = nullptr;
//        return FALSE;
//    }

    if (!ValidateDevice(m_ftHandle))
    {
        FT_Close(m_ftHandle);
        m_ftHandle = nullptr;
        return false;
    }
    return true;

}

bool UsbFtdDevice::CloseDevice(void)
{
    if (m_ftHandle)
    {
        FT_Close(m_ftHandle);
        m_ftHandle = nullptr;
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Gets the number of devices connected to the machine using FT_ListDevices
///////////////////////////////////////////////////////////////////////////////////
DWORD UsbFtdDevice::GetNumberOfDevicesConnected()
{
    FT_STATUS ftStatus = FT_OK;
    DWORD dwNumDevices = 0;

    ftStatus = FT_ListDevices(&dwNumDevices, NULL, FT_LIST_NUMBER_ONLY);
    //if (FT_FAILED(ftStatus))
    //{
    //    return 0;
    //}

    return dwNumDevices;
}

///////////////////////////////////////////////////////////////////////////////////
// Gets the serial number of the specified device index using FT_ListDevices
///////////////////////////////////////////////////////////////////////////////////
bool UsbFtdDevice::GetDeviceSerialNumber(DWORD dwDeviceIndex, char* SerialNumber)
{
    FT_STATUS ftStatus = FT_OK;

    ftStatus = FT_ListDevices((PVOID)dwDeviceIndex, SerialNumber, FT_LIST_BY_INDEX | FT_OPEN_BY_SERIAL_NUMBER);
    if (FT_FAILED(ftStatus))
    {
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////
// Gets the device description of the specified device index using FT_ListDevices
///////////////////////////////////////////////////////////////////////////////////
bool UsbFtdDevice::GetDeviceDescription(DWORD dwDeviceIndex, char* ProductDescription)
{
    FT_STATUS ftStatus = FT_OK;

    ftStatus = FT_ListDevices((PVOID)dwDeviceIndex, ProductDescription, FT_LIST_BY_INDEX | FT_OPEN_BY_DESCRIPTION);
    if (FT_FAILED(ftStatus))
    {
        return false;
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Gets the information of all devices connected using FT_CreateDeviceInfoList and FT_GetDeviceInfoList
///////////////////////////////////////////////////////////////////////////////////
DWORD UsbFtdDevice::GetDevicesInfoList(FT_DEVICE_LIST_INFO_NODE **pptDevicesInfo)
{
    FT_STATUS ftStatus = FT_OK;
    DWORD dwNumDevices = 0;

    ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
    if (FT_FAILED(ftStatus))
    {
        return 0;
    }

    *pptDevicesInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * dwNumDevices);
    if (!(*pptDevicesInfo))
    {
        return 0;
    }

    ftStatus = FT_GetDeviceInfoList(*pptDevicesInfo, &dwNumDevices);
    if (FT_FAILED(ftStatus))
    {
        free(*pptDevicesInfo);
        *pptDevicesInfo = NULL;
        return 0;
    }

    return dwNumDevices;
}


void UsbFtdDevice::DisplayDeviceInfoList()
{
    FT_DEVICE_LIST_INFO_NODE *ptDeviceInfo = NULL;
    DWORD dwNumDevices = 0;

    dwNumDevices = GetDevicesInfoList(&ptDeviceInfo);
    if (dwNumDevices == 0)
    {
        return;
    }

    for (DWORD i = 0; i < dwNumDevices; i++)
    {
        qDebug("FT_GetDeviceInfoList:Device[%d]", i);
        qDebug("Flags: 0x%x [%s]", ptDeviceInfo[i].Flags,
            ptDeviceInfo[i].Flags & FT_FLAGS_SUPERSPEED ? "USB 3" :
            ptDeviceInfo[i].Flags & FT_FLAGS_HISPEED ? "USB 2" : "");
        qDebug("Type: %d", ptDeviceInfo[i].Type);
        qDebug("ID: 0x%08X", ptDeviceInfo[i].ID);
        if (ptDeviceInfo[i].Flags & FT_FLAGS_OPENED)
        {
            qDebug("ftHandle: 0x%x [Device is being used by another process]", ptDeviceInfo[i].ftHandle);
        }
        qDebug("SerialNumber: %s", ptDeviceInfo[i].SerialNumber);
        qDebug("Description: %s", ptDeviceInfo[i].Description);
    }
}

bool UsbFtdDevice::ValidateDevice(FT_HANDLE ftHandle)
{
    FT_STATUS ftStatus = FT_OK;
    USHORT uwVID = 0;
    USHORT uwPID = 0;


    //
    // Get and check device VID and PID
    // The default PID of FT60X is 0x0403
    // The default VID of FT60X is 0x601F if 32-bit, 0x601E if 16-bit
    //
    ftStatus = FT_GetVIDPID(ftHandle, &uwVID, &uwPID);
    if (FT_FAILED(ftStatus))
    {
        return false;
    }
    printf("\t\tVendor ID: 0x%04X\n", uwVID);
    printf("\t\tProduct ID: 0x%04X\n", uwPID);


    //
    // Get firmware version
    // The current firmware version is 0x0102
    //
    ULONG ulFirmwareVersion = 0;
    ftStatus = FT_GetFirmwareVersion(ftHandle, &ulFirmwareVersion);
    if (FT_FAILED(ftStatus))
    {
        return false;
    }
    printf("\t\tFirmware version: 0x%04x\n", ulFirmwareVersion);
    if (ulFirmwareVersion != FTDI_CURRENT_FIRMWARE_VERSION)
    {
        printf("\t\tWARNING! Expected firmware version: 0x%04x\n", FTDI_CURRENT_FIRMWARE_VERSION);
    }


    //
    // Get driver version
    //
    ULONG ulDriverVersion = 0;
    ftStatus = FT_GetDriverVersion(ftHandle, &ulDriverVersion);
    if (FT_FAILED(ftStatus))
    {
        return false;
    }
    printf("\t\tDriver version: 0x%04x\n", ulDriverVersion);


    //
    // Get library version
    //
    ULONG ulLibraryVersion = 0;
    ftStatus = FT_GetLibraryVersion(&ulLibraryVersion);
    if (FT_FAILED(ftStatus))
    {
        return false;
    }
    printf("\t\tLibrary version: 0x%04x\n", ulLibraryVersion);


    return true;
}


bool UsbFtdDevice::ReadData( UCHAR* buf, const ULONG bufferlength,ULONG *ulActualBytesRead)
{
    FT_STATUS ftStatus = FT_OK;
    if (ftStatus != FT_OK ||  m_ftHandle == nullptr)
    {
        return false;
    }
    *ulActualBytesRead = 0;
    ftStatus = FT_ReadPipe(m_ftHandle, 0x82, buf, bufferlength, ulActualBytesRead, NULL);

    bool result = true;
//    if (FT_FAILED(ftStatus) || bufferlength != *ulActualBytesRead)
//    {
//        *ulActualBytesRead = 0;
//        result = false;
//    }
    if (FT_FAILED(ftStatus))
    {
        qDebug("{} Read failed!!!! ftStatus is {} ",__FUNCTION__, FT_STATUS_DESCRITION[ftStatus]);
        *ulActualBytesRead = 0;
        result = false;
    }
    return result;
}

bool UsbFtdDevice::WriteData( UCHAR *buf,const ULONG bufferlength,ULONG *ulActualBytesWritten)
{
    FT_STATUS ftStatus = FT_OK;

    if (ftStatus != FT_OK || m_ftHandle == nullptr)
    {
        return false;
    }

    ftStatus = FT_WritePipe(m_ftHandle, 0x02, buf, bufferlength, ulActualBytesWritten, NULL);

    bool result = true;
//    if (FT_FAILED(ftStatus) || *ulActualBytesWritten != bufferlength)
//    {
//        result = false;
//    }

    if (FT_FAILED(ftStatus))
    {
        qDebug("{} write failed!!!!  ftStatus is {} ",__FUNCTION__,FT_STATUS_DESCRITION[ftStatus]);
        result = false;
    }
    return result;
}


bool UsbFtdDevice::ReadDataASync( UCHAR* acReadBuf, const ULONG ulBytesToRead,ULONG *ulActualBytesRead)
{
    if(m_ftHandle == nullptr)
    {
        return false;
    }
#if 1
    FT_STATUS ftStatus = FT_OK;
    OVERLAPPED oOverlapped = {0};
    memset(&oOverlapped,0,sizeof(oOverlapped));
    *ulActualBytesRead = 0;
    bool bResult = true;

    ftStatus = FT_InitializeOverlapped(m_ftHandle, &oOverlapped);
    if (FT_FAILED(ftStatus))
    {
        bResult = false;
        goto exit;
    }

//    qDebug("read pipe before %s",acReadBuf);
    ftStatus = FT_ReadPipe(m_ftHandle, 0x82, acReadBuf, ulBytesToRead, ulActualBytesRead, &oOverlapped);
    if (ftStatus == FT_IO_PENDING)
    {
        // Poll until all data requested ulBytesToRead is received
        do
        {
            // FT_GetOverlappedResult will return FT_IO_INCOMPLETE if not yet finish
//            ftStatus = FT_GetOverlappedResult(m_ftHandle, &oOverlapped, ulActualBytesRead, FALSE);
            ftStatus = FT_GetOverlappedResult(m_ftHandle, &oOverlapped, ulActualBytesRead, 0);
            if (ftStatus == FT_IO_INCOMPLETE)
            {
//                qDebug("{} {} ,waitting and will be continue!!!",__FUNCTION__,FT_STATUS_DESCRITION[ftStatus]);
                continue;
            }
            else if (FT_FAILED(ftStatus))
            {
                qDebug("{} Read failed!!!! ftStatus is {},usb fthandle will release!!! line391",__FUNCTION__,FT_STATUS_DESCRITION[ftStatus]);
#if 1
                FT_ReleaseOverlapped(m_ftHandle, &oOverlapped);
                bResult = false;
                goto exit;
#endif
                break;
            }
            else //if (ftStatus == FT_OK)
            {
                // exit now
//                qDebug("\n[%s %d] Reading total is : %d ,actual read is: %d ! ",__FUNCTION__,__LINE__, ulBytesToRead,*ulActualBytesRead);
                break;
            }
        }
        while (1);
    }
    else if (FT_FAILED(ftStatus))
    {
        FT_ReleaseOverlapped(m_ftHandle, &oOverlapped);
        qDebug("{} Read failed!!!! ftStatus is {},usb fthandle will release!!!",__FUNCTION__,FT_STATUS_DESCRITION[ftStatus]);
        bResult = false;
        goto exit;
    }
    else //if (ftStatus == FT_OK)
    {
        // do nothing
//        qDebug("\n[%s %d] Reading total is : %d ,actual read is: %d ! " ,__FUNCTION__,__LINE__,ulBytesToRead,*ulActualBytesRead);
    }

    // Verify if number of bytes requested is equal to the number of bytes received
//    if ( *ulActualBytesRead != ulBytesToRead)
//    {
//        FT_ReleaseOverlapped(m_ftHandle, &oOverlapped);
//        bResult = FALSE;
//        goto exit;
//    }

    // Delete the overlapped io event
    FT_ReleaseOverlapped(m_ftHandle, &oOverlapped);
    return bResult;

exit:
    FT_Close(m_ftHandle);
    return bResult;
#endif

#if 0
    FT_STATUS ftStatus = FT_OK;
    *ulActualBytesRead = 0;
    OVERLAPPED vOverlapped = {0};
    bool bResult = true;

    ftStatus = FT_InitializeOverlapped(m_ftHandle, &vOverlapped);
    ftStatus = FT_ReadPipe(m_ftHandle, 0x82, acReadBuf, ulBytesToRead, ulActualBytesRead, &vOverlapped);
    if (ftStatus == FT_IO_PENDING)
    {
        ftStatus = FT_GetOverlappedResult(m_ftHandle, &vOverlapped, ulActualBytesRead, true);
    }
    FT_ReleaseOverlapped(m_ftHandle, &vOverlapped);

    if (FT_FAILED(ftStatus))
    {
        bResult = false;
    }
    return bResult;
#endif
}

bool UsbFtdDevice::ASICRegisterWiteOne(const uchar &addr,const uint &data)
{
    return SendForASICRegisterWrite(addr,data);
}


bool UsbFtdDevice::ASICRegisterReadOne(const uchar &addr,uint &data,bool &bRet)
{
    bRet = SendForASICRegisterRead(addr);
    if(bRet)
    {
        bRet = FtdReadOneASICRegister(addr,data);
    }
    return bRet;
}


bool UsbFtdDevice::ASICRegisterReadAll(std::vector<std::tuple<uchar, uint32_t >> &vctAddrData,bool &bRet)
{
    bRet = false;
    for(int i = 0; i < vctAddrData.size(); ++i)
    {
        const uchar &addr = std::get<0>(vctAddrData.at(i));
        bRet = SendForASICRegisterRead(addr);
        if(!bRet)
        {
            break;
        }
//        qDebug("item %d, addr: 0x%02x, data : %x",i,std::get<0>(vctAddrData.at(i)),std::get<1>(vctAddrData.at(i)));
    }
    if(bRet)
    {
        if(FtdReadASICRegisters(vctAddrData,bRet))
        {
            return true;
        }
    }
    qDebug()<<__FUNCTION__<<"Failed!!!";
    return false;
}


bool UsbFtdDevice::SwitchOnUsbDataStream(const bool bStream)
{
    std::vector<uint8_t> vctWriteBuf(BUF_SIZE,0);
    qulonglong lData = UsbProtoMsg::SwitchToUSBCmdOrDataEncode(bStream);
    ULONG ulBytesToWrite = (ULONG)UsbProtoMsg::SendWriteCmdToFPGAEncode(FPGA_ALL_CONTROL_REG,FPGA_CTRL_NOT_SPI_MODE,vctWriteBuf.data(),BUF_SIZE,lData);
    ULONG ulActualBytesToWrite = 0;
    bool bResult = WriteData(vctWriteBuf.data(),ulBytesToWrite,&ulActualBytesToWrite);
//    LOGCD("<<<<<<<<< Write to Fpga size: {}/{}; Print Binary : {:n}",ulBytesToWrite,ulActualBytesToWrite\
//          , spdlog::to_hex(std::begin(vctWriteBuf), std::begin(vctWriteBuf) + ulActualBytesToWrite));
    return bResult;
}

bool UsbFtdDevice::SwitchOnUdpTestMode(const bool res)
{
    std::vector<uint8_t> vctWriteBuf(BUF_SIZE,0);
    qulonglong lData = UsbProtoMsg::SwitchToUSBCmdOrDataEncode(true);
    if(res)
    {
         lData = UsbProtoMsg::SwitchToUdpTestModeCmdEncode();
    }
    ULONG ulBytesToWrite = (ULONG)UsbProtoMsg::SendWriteCmdToFPGAEncode(FPGA_ALL_CONTROL_REG,FPGA_CTRL_NOT_SPI_MODE,vctWriteBuf.data(),BUF_SIZE,lData);
    ULONG ulActualBytesToWrite = 0;
    bool bResult = WriteData(vctWriteBuf.data(),ulBytesToWrite,&ulActualBytesToWrite);
//    LOGCD("<<<<<<<<< Write to Fpga size: {}/{}; Print Binary : {:n}",ulBytesToWrite,ulActualBytesToWrite\
//          , spdlog::to_hex(std::begin(vctWriteBuf), std::begin(vctWriteBuf) + ulActualBytesToWrite));
    return bResult;
}


bool UsbFtdDevice::SendForExtADCRegisterWriteOne(const qulonglong &lData)
{
    std::vector<uint8_t> vctWriteBuf(BUF_SIZE,0);
    ULONG ulBytesToWrite = (ULONG)UsbProtoMsg::SendWriteCmdToFPGAEncode(FPGA_EXTRA_AD_WRIGE_REG,FPGA_CTRL_SPI_WRITE_MODE,vctWriteBuf.data(),BUF_SIZE,lData);
    ULONG ulActualBytesToWrite = 0;
    bool bResult = WriteData(vctWriteBuf.data(),ulBytesToWrite,&ulActualBytesToWrite);
//    LOGCD("<<<<<<<<<Write Ext ADC, send to fpga size: {}/{}; Print: {:n}",ulBytesToWrite,ulActualBytesToWrite\
//          , spdlog::to_hex(std::begin(vctWriteBuf), std::begin(vctWriteBuf) + ulActualBytesToWrite));
    return bResult;
}
bool UsbFtdDevice::SendForExtADCRegisterReadOne(const qulonglong &lData)
{
    std::vector<uint8_t> vctWriteBuf(BUF_SIZE,0);
    ULONG ulBytesToWrite = (ULONG)UsbProtoMsg::SendWriteCmdToFPGAEncode(FPGA_EXTRA_AD_WRIGE_REG,FPGA_CTRL_SPI_READ_MODE,vctWriteBuf.data(),BUF_SIZE,lData);
    ULONG ulActualBytesToWrite = 0;
    bool bResult = WriteData(vctWriteBuf.data(),ulBytesToWrite,&ulActualBytesToWrite);
//    LOGCD("<<<<<<<<<Read Ext ADC one, send to fpga size: {}/{}; Print: {:n}",ulBytesToWrite,ulActualBytesToWrite\
//          , spdlog::to_hex(std::begin(vctWriteBuf), std::begin(vctWriteBuf) + ulActualBytesToWrite));
    return bResult;
}


bool UsbFtdDevice::SendForASICRegisterWrite(const uchar &addr,const uint &data)
{
    std::vector<uint8_t> vctWriteBuf(BUF_SIZE,0);
    qulonglong lData = UsbProtoMsg::WriteAsicDataEncode(addr, data);

    ULONG ulBytesToWrite = (ULONG)UsbProtoMsg::SendWriteCmdToFPGAEncode(FPGA_ASIC_ADDR_REG,FPGA_CTRL_SPI_WRITE_MODE,vctWriteBuf.data(),BUF_SIZE,lData);
    ULONG ulActualBytesToWrite = 0;
    bool bResult = WriteData(vctWriteBuf.data(),ulBytesToWrite,&ulActualBytesToWrite);
//    LOGCD("<<<<<<<<<Write Asic addr {:02X}, send to fpga size: {}/{}; Print: {:n}",addr,ulBytesToWrite,ulActualBytesToWrite\
//          , spdlog::to_hex(std::begin(vctWriteBuf), std::begin(vctWriteBuf) + ulActualBytesToWrite));
    return bResult;
}


bool UsbFtdDevice::SendForASICRegisterRead(const uchar &addr)
{
    std::vector<uint8_t> vctWriteBuf(BUF_SIZE,0);
    uint32_t data = 0;
    qulonglong lData = UsbProtoMsg::ReadAsicDataEncode(addr, data);
    ULONG ulBytesToWrite = (ULONG)UsbProtoMsg::SendWriteCmdToFPGAEncode(FPGA_ASIC_ADDR_REG,FPGA_CTRL_SPI_READ_MODE,vctWriteBuf.data(),BUF_SIZE,lData);
    ULONG ulActualBytesToWrite = 0;
    bool bResult = WriteData(vctWriteBuf.data(),ulBytesToWrite,&ulActualBytesToWrite);
//    LOGCD("<<<<<<<<< Read Asic addr {:02X} ,send command to fpga size: {}/{}; Print: {:n}",addr,ulBytesToWrite,ulActualBytesToWrite\
//          , spdlog::to_hex(std::begin(vctWriteBuf), std::begin(vctWriteBuf) + ulActualBytesToWrite));
    return bResult;
}


bool UsbFtdDevice::FtdReadOneASICRegister(const uchar &addr,uint &data)
{
    std::vector<uint8_t> vctReadBuf(USB_CMD_READ_BUF_SIZE,0);
    ULONG ulActualBytesToRead = 0;
    ULONG ulBytesToRead = vctReadBuf.size();
    if(ReadData(vctReadBuf.data(),ulBytesToRead,&ulActualBytesToRead))
//    if(ReadDataASync(vctReadBuf.data(),ulBytesToRead,&ulActualBytesToRead))
    {
//        LOGCD(">>>>>>>>> Read Ft600 size: {}/{}, print:{:n}",ulBytesToRead,ulActualBytesToRead, spdlog::to_hex(std::begin(vctReadBuf), std::begin(vctReadBuf) + ulActualBytesToRead));
//        return UsbProtoMsg::OnDecodeReturnFrame(vctReadBuf.data(),ulActualBytesToRead,addr,data);
    }
    return false;
}

bool UsbFtdDevice::FtdReadASICRegisters(std::vector<std::tuple<uchar, uint32_t >> &vctAddrData,bool &bRet)
{
    bRet = false;
    uint items = vctAddrData.size();
    uint bufSize = items *12 +2;
    std::vector<uint8_t> vctReadBuf(bufSize,0);
    ULONG ulActualBytesToRead = 0;
    ULONG ulBytesToRead = bufSize;
    if(ReadData(vctReadBuf.data(),ulBytesToRead,&ulActualBytesToRead))
//    if(ReadDataASync(vctReadBuf.data(),ulBytesToRead,&ulActualBytesToRead))
    {
//        LOGCD(">>>>>>>>> Ftd600 Plan to read bytes: {}; recvdata actual bytes: {}, Print Binary : {}",ulBytesToRead,ulActualBytesToRead, spdlog::to_hex(vctReadBuf));
        return true;
    }
    return false;
}



