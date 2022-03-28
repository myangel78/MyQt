#pragma once
#include <QStringList>
#include <QObject>
#ifdef Q_OS_WIN32
#include "FTD3XX/FTD3XX.h"
#else
#include "ftd3xxlinux/ftd3xx.h"
#endif


#define FTDI_VID                        0x0403
#define FTDI_PID_600                    0x601E
#define FTDI_PID_601                    0x601F
#define FTDI_CURRENT_FIRMWARE_VERSION   0x0102

static const std::string FT_STATUS_DESCRITION[] = {"FT_OK",
                                                   "FT_INVALID_HANDLE",
                                                   "FT_DEVICE_NOT_FOUND",
                                                   "FT_DEVICE_NOT_OPENED",
                                                   "FT_IO_ERROR",
                                                   "FT_INSUFFICIENT_RESOURCES",
                                                   "FT_INVALID_PARAMETER",
                                                   "FT_INVALID_BAUD_RATE",
                                                   "FT_DEVICE_NOT_OPENED_FOR_ERASE",
                                                   "FT_DEVICE_NOT_OPENED_FOR_WRITE",
                                                   "FT_FAILED_TO_WRITE_DEVICE",
                                                   "FT_EEPROM_READ_FAILED",
                                                   "FT_EEPROM_WRITE_FAILED",
                                                   "FT_EEPROM_ERASE_FAILED",
                                                   "FT_EEPROM_NOT_PRESENT",
                                                   "FT_EEPROM_NOT_PROGRAMMED",
                                                   "FT_INVALID_ARGS",
                                                   "FT_NOT_SUPPORTED",
                                                   "FT_NO_MORE_ITEMS",
                                                   "FT_TIMEOUT",
                                                   "FT_OPERATION_ABORTED",
                                                   "FT_RESERVED_PIPE",
                                                   "FT_INVALID_CONTROL_REQUEST_DIRECTION",
                                                   "FT_INVALID_CONTROL_REQUEST_TYPE",
                                                   "FT_IO_PENDING",
                                                   "FT_IO_INCOMPLETE",
                                                   "FT_HANDLE_EOF",
                                                   "FT_BUSY",
                                                   "FT_NO_SYSTEM_RESOURCES",
                                                   "FT_DEVICE_LIST_NOT_READY",
                                                   "FT_DEVICE_NOT_CONNECTED",
                                                   "FT_INCORRECT_DEVICE_PATH",
                                                   "FT_OTHER_ERROR",};



class UsbFtdDevice :public QObject
{
    Q_OBJECT
public:
    UsbFtdDevice();
    ~UsbFtdDevice();

    FT_HANDLE GetUsbHandle(void){ return m_ftHandle;}

    bool OnSearchUsbDevice(QStringList &devDescrlist);
    bool OpenDeviceByDescr(QString description);
    bool CloseDevice(void);
    bool SwitchOnUsbDataStream(const bool bStream);
    bool SwitchOnUdpTestMode(const bool res);

    bool ReadData( UCHAR* buf, const ULONG bufferlength,ULONG *ulActualBytesRead);
    bool WriteData( UCHAR *buf,const ULONG bufferlength,ULONG *ulActualBytesWritten);
    bool ReadDataASync( UCHAR* acReadBuf, const ULONG ulBytesToRead,ULONG *ulActualBytesRead);


signals:
    void SpeedDispSig(const float &speed);

public slots:
    bool ASICRegisterWiteOne(const uchar &addr,const uint &data);
    bool ASICRegisterReadOne(const uchar &addr,uint &data,bool &bRet);
    bool ASICRegisterReadAll(std::vector<std::tuple<uchar, uint32_t >> &vctAddrData,bool &bRet);

    bool SendForExtADCRegisterWriteOne(const qulonglong &lData);
    bool SendForExtADCRegisterReadOne(const qulonglong &lData);

private:
    DWORD GetNumberOfDevicesConnected();
    bool GetDeviceSerialNumber(DWORD dwDeviceIndex, char* SerialNumber);
    bool GetDeviceDescription(DWORD dwDeviceIndex, char* ProductDescription);
    DWORD GetDevicesInfoList(FT_DEVICE_LIST_INFO_NODE **pptDevicesInfo);
    void DisplayDeviceInfoList();
    bool ValidateDevice(FT_HANDLE ftHandle);

    bool SendForASICRegisterRead(const uchar &addr);
    bool SendForASICRegisterWrite(const uchar &addr,const uint &data);
    bool FtdReadOneASICRegister(const uchar &addr,uint &data);
    bool FtdReadASICRegisters(std::vector<std::tuple<uchar, uint32_t >> &vctAddrData,bool &bRet);

private:
    FT_HANDLE m_ftHandle =nullptr;

public:


};
