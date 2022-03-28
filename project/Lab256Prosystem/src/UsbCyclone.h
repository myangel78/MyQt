#pragma once

#include <libusb.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <memory>



class UsbCyclone
{
public:
	UsbCyclone();
	UsbCyclone(const UsbCyclone& other) = delete;
	UsbCyclone(UsbCyclone&& other) = delete;
	UsbCyclone& operator=(const UsbCyclone& othre) = delete;

    ~UsbCyclone();

	struct SUsbDeviceInfo
	{
		libusb_device* usbDev;
		unsigned short vendorId;
		unsigned short productId;
		unsigned char busNum;
		unsigned char devAddr;
		std::string devName;
		std::map<unsigned char, std::map<unsigned char, std::vector<unsigned char>>> mapIfAltifEp;

		SUsbDeviceInfo(libusb_device* dev, unsigned short vid, unsigned short pid, unsigned char busnum, unsigned char devaddr, const std::string& nam)
			: usbDev(dev)
			, vendorId(vid)
			, productId(pid)
			, busNum(busnum)
			, devAddr(devaddr)
			, devName(nam)
		{}
	};
private:
    libusb_context* m_pctx = nullptr;
    libusb_device** m_devlist = nullptr;
	libusb_device_handle* m_pCurDeviceHandle = nullptr;
	unsigned char m_ucSendEndpoint = 0x01;
	unsigned char m_ucRecvEndpoint = 0x81;
	//std::shared_ptr<libusb_endpoint_descriptor> m_endpointDesc = nullptr;
	std::vector<std::tuple<unsigned short, unsigned short, std::string>> m_vetInterestDevice;
	std::vector<SUsbDeviceInfo> m_vetDeviceInfo;

private:
	inline bool isInterestDevice(unsigned short vid, unsigned short pid, std::string & nam);

public:
	std::vector<SUsbDeviceInfo>& GetDeviceInfo(void) { return m_vetDeviceInfo; }
    libusb_device_handle* GetCurDeviceHandle(void) { return m_pCurDeviceHandle; }

	int InitUsbDevice(void);
    int SearchDeviceInfo(void* hwnd, std::vector<std::tuple<std::string, unsigned short, unsigned short>>& vetOut);
	bool OpenUsbDevice(int idx);
	bool OpenUsbDevice(unsigned short vid, unsigned short pid);
	bool DownloadFirmware(const char* pfilename);
	bool SetInterAltinter(unsigned char selif, unsigned char altif);
	bool ReleaseInterAltinter(unsigned char selif, unsigned char altif);
	int FPGARegisterWrite(unsigned short addr, unsigned int data);
	void FPGARegisterReadBack(unsigned short addr, unsigned int& rValue, bool& result);
	int BulkDataTransfer(unsigned char* data, int len, int* reallen, unsigned int timeout);
	bool CloseUsbDevice(void);
};

