#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <windows.h>
#include <minwindef.h>



class CCyUSBDevice;
class CCyUSBEndPoint;
class CCyBulkEndPoint;


class UsbCyDevice
{
public:
	UsbCyDevice();
	~UsbCyDevice();
private:
	CCyUSBDevice* m_pUsbDevie = nullptr;
	CCyBulkEndPoint* m_pBulkInEpt = nullptr;
	CCyBulkEndPoint* m_pBulkOutEpt = nullptr;

	std::vector<std::tuple<unsigned short, unsigned short, std::string>> m_vetInterestDevice;
	inline bool isInterestDevice(unsigned short vid, unsigned short pid, std::string& nam);

	//bool ControlPointReadWrite(PUCHAR buf, CTL_XFER_DIR_TYPE type, LONG& len);
	bool ControlPointRead(PUCHAR buf, LONG & len);
	bool ControlPointWrite(PUCHAR buf, LONG & len);

public:
	CCyUSBDevice* GetCurDeviceHandle(void) const;
	int SearchDeviceInfo(HANDLE hwnd, std::vector<std::tuple<std::string, USHORT, USHORT>> & vetOut);
	bool OpenUsbDevice(USHORT vendorID, USHORT productID);
	bool CloseUsbDevice(void);
	bool DeviceIsOpen(void);
	bool DownloadFirmware(const char* pFile);
	bool LoadDevRegister(void);
	int FPGARegisterRead(USHORT addr, LONG datalen);
	int FPGARegisterWrite(USHORT addr, UINT data);
	void FPGARegisterReadBack(unsigned short addr, unsigned int& rValue, bool& result);
	int AFERegisterRead(USHORT addr, LONG datalen);
	int AFERegisterWrite(USHORT addr, LONG data);
};

