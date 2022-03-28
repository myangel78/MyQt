#include "UsbCyclone.h"
#include "UsbCyclone.h"

#include <string.h>
#include <algorithm>
#include <thread>


UsbCyclone::UsbCyclone()
{
    m_vetInterestDevice.emplace_back(0x04B4, 0x00F3, "BGI Cyclone LAB256 Pro Bootloader");
    m_vetInterestDevice.emplace_back(0x04B4, 0x00F1, "BGI Cyclone LAB256 Test");
    m_vetInterestDevice.emplace_back(0x0451, 0xAF01, "BGI Cyclone LAB256 Pro");

    InitUsbDevice();
}

UsbCyclone::~UsbCyclone()
{
    CloseUsbDevice();

    if (m_devlist != nullptr)
    {
        libusb_free_device_list(m_devlist, 1);
        m_devlist = nullptr;
    }
    libusb_exit(m_pctx);
}

inline bool UsbCyclone::isInterestDevice(unsigned short vid, unsigned short pid, std::string& nam)
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

int UsbCyclone::InitUsbDevice(void)
{
    int ret = libusb_init(&m_pctx);
    if (ret != 0)
    {
        printf("Error in initializing libusb library...\n");
        return -999;
    }
    return 0;
}

int UsbCyclone::SearchDeviceInfo(void* hwnd, std::vector<std::tuple<std::string, unsigned short, unsigned short>>& vetOut)
{
    if (m_devlist != nullptr)
    {
        libusb_free_device_list(m_devlist, 1);
        m_devlist = nullptr;
    }
    int devnum = libusb_get_device_list(m_pctx, &m_devlist);
    if (devnum < 0)
    {
        printf("Library: Error in enumerating devices...\n");
        libusb_exit(nullptr);
        return devnum;
    }
    m_vetDeviceInfo.clear();
    for (int i = 0; i < devnum; ++i)
    {
        libusb_device* pdev = m_devlist[i];
        libusb_device_descriptor devdesc;
        libusb_get_device_descriptor(pdev, &devdesc);

        auto vid = devdesc.idVendor;
        auto pid = devdesc.idProduct;
        std::string name;
        if (isInterestDevice(vid, pid, name))
        {
            auto busnum = libusb_get_bus_number(pdev);
            auto devaddr = libusb_get_device_address(pdev);
            m_vetDeviceInfo.emplace_back(pdev, vid, pid, busnum, devaddr, name);
            vetOut.emplace_back(name, vid, pid);
        }
    }
    return devnum;
}

bool UsbCyclone::OpenUsbDevice(int idx)
{
    if (idx < 0 || idx >= m_vetDeviceInfo.size())
    {
        return false;
    }
    auto& item = m_vetDeviceInfo[idx];
    auto pdev = item.usbDev;
    
    int ret = 0;

    ret = libusb_open(pdev, &m_pCurDeviceHandle);
    if (ret != 0)
    {
        printf("Error in opening device\n");
        return false;
    }
    libusb_clear_halt(m_pCurDeviceHandle, m_ucSendEndpoint);
    libusb_clear_halt(m_pCurDeviceHandle, m_ucRecvEndpoint);
//    if (m_pCurDeviceHandle != nullptr)
//    {
//        ret = libusb_reset_device(m_pCurDeviceHandle);
//        if (ret != 0)
//        {
//            printf("Reset Device Failed!!!\n");
//            return false;
//        }
//    }
//    unsigned char ucdata = 0;
//    ret = libusb_control_transfer(m_pCurDeviceHandle, 0xC0, 0xA0, 0xE600, 0x00, &ucdata, 1, 1000);

    libusb_config_descriptor* confdesc = nullptr;
    //ret = libusb_get_config_descriptor(pdev, idx, &confdesc);
    ret = libusb_get_active_config_descriptor(pdev, &confdesc);
    if (ret != 0)
    {
        printf("Failed to get %d: USB Configuration descriptor\n", idx);
        CloseUsbDevice();
        return false;
    }

    for (int i = 0; i < confdesc->bNumInterfaces; ++i)
    {
        //ret = libusb_claim_interface(m_pCurDeviceHandle, i);
        //if (ret != 0)
        //{
        //    printf("Failed to claim %d: interface %d\n", idx, i);
        //    libusb_free_config_descriptor(confdesc);
        //    CloseUsbDevice();
        //    return false;
        //}

        if (libusb_kernel_driver_active(m_pCurDeviceHandle, i))
        {
            libusb_detach_kernel_driver(m_pCurDeviceHandle, i);
        }

        int altsettingnum = confdesc->interface[i].num_altsetting;
        for (int j = 0; j < altsettingnum; ++j)
        {
            const libusb_interface_descriptor& altdecs = confdesc->interface[i].altsetting[j];

            for (int k = 0; k < altdecs.bNumEndpoints; ++k)
            {
                const auto& iteFind = item.mapIfAltifEp.find(i);
                if (iteFind != item.mapIfAltifEp.end())
                {
                    const auto& iteFind2 = iteFind->second.find(j);
                    if (iteFind2 != iteFind->second.end())
                    {
                        iteFind2->second.emplace_back(k);
                    }
                    else
                    {
                        iteFind->second[j].emplace_back(k);
                    }
                }
                else
                {
                    item.mapIfAltifEp[i][j].emplace_back(k);
                }

#if 0
                const libusb_endpoint_descriptor& endpointDesc = altdecs.endpoint[k];
                unsigned char endpoint = 0x81;
                if (endpointDesc.bEndpointAddress == endpoint)
                {
                    printf("Found %d: endpoint 0x%x in interface %d, setting %d\n", idx, endpoint, i, j);

                    // If the alt setting is not 0, select it
                    libusb_set_interface_alt_setting(m_pCurDeviceHandle, i, j);

                    m_endpointDesc = std::make_shared<libusb_endpoint_descriptor>();
                    memcpy(m_endpointDesc.get(), &endpointDesc, sizeof(libusb_endpoint_descriptor));

                    return true;

                    auto eptype = endpointDesc.bmAttributes;

                    int pktsize = 0;
                    libusb_device_descriptor devdesc;
                    libusb_get_device_descriptor(pdev, &devdesc);
                    if (devdesc.bcdUSB >= 0x0300)
                    {
                        // If this is a USB 3.0 connection, get the endpoint companion descriptor
                        // The packet size for the endpoint is calculated as the product of the max
                        // packet size and the burst size. For Isochronous endpoints, this value is
                        // multiplied by the mult value as well.
                        libusb_ss_endpoint_companion_descriptor* companionDesc;
                        libusb_get_ss_endpoint_companion_descriptor(NULL, &endpointDesc, &companionDesc);
                        if (eptype == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
                            pktsize = endpointDesc.wMaxPacketSize * (companionDesc->bMaxBurst + 1) * (companionDesc->bmAttributes + 1);
                        else
                            pktsize = endpointDesc.wMaxPacketSize * (companionDesc->bMaxBurst + 1);

                        libusb_free_ss_endpoint_companion_descriptor(companionDesc);

                    }
                    else
                    {
                        // Not USB 3.0. For Isochronous endpoints, get the packet size as computed by
                        // the library. For other endpoints, use the max packet size as it is.
                        if (eptype == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
                            pktsize = libusb_get_max_iso_packet_size(pdev, endpoint);
                        else
                            pktsize = endpointDesc.wMaxPacketSize;

                    }

                    libusb_transfer* ptrans = libusb_alloc_transfer((eptype == LIBUSB_TRANSFER_TYPE_BULK) ? 32 : 0);
                    libusb_fill_bulk_transfer(ptrans, m_pCurDeviceHandle, endpoint, (unsigned char*)"HelloWorld", 10, nullptr, nullptr, 5000);
                    ret = libusb_submit_transfer(ptrans);
                    if (ret != 0)
                    {
                        printf("transfer error!!!\n");
                    }

                    int traned = 0;
                    ret = libusb_bulk_transfer(m_pCurDeviceHandle, endpoint, (unsigned char*)"HelloWorld", 10, &traned, 5000);

                    libusb_handle_events(nullptr);

                    libusb_free_transfer(ptrans);
                    libusb_free_config_descriptor(confdesc);
                }
#endif
            }
        }
        //libusb_detach_kernel_driver(m_pCurDeviceHandle, i);
        //libusb_release_interface(m_pCurDeviceHandle, i);
    }
    libusb_free_config_descriptor(confdesc);

    SetInterAltinter(0, 0);

    return true;
}

bool UsbCyclone::OpenUsbDevice(unsigned short vid, unsigned short pid)
{
    for (int i = 0, n = m_vetDeviceInfo.size(); i < n; ++i)
    {
        if (m_vetDeviceInfo[i].vendorId == vid && m_vetDeviceInfo[i].productId == pid)
        {
            return OpenUsbDevice(i);
        }
    }
    return false;
}

bool UsbCyclone::DownloadFirmware(const char* pfilename)
{
    libusb_device** devlist = nullptr;
    libusb_device_handle* pCurDeviceHandle = nullptr;
    int devnum = libusb_get_device_list(m_pctx, &devlist);
    if (devnum < 0)
    {
        printf("Library: Error in enumerating devices...\n");
        libusb_exit(nullptr);
        return false;
    }

    for (int i = 0; i < devnum; ++i)
    {
        libusb_device* pdev = devlist[i];
        libusb_device_descriptor devdesc;
        libusb_get_device_descriptor(pdev, &devdesc);

        auto vid = devdesc.idVendor;
        auto pid = devdesc.idProduct;
        std::string name;
        if (isInterestDevice(vid, pid, name))
        {
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (name.find("bootloader", 0) == std::string::npos)
            {
                return true;
            }
            int ret = libusb_open(pdev, &pCurDeviceHandle);
            if (ret != 0)
            {
                printf("Error in opening device\n");
                return false;
            }
            else
            {
                break;
            }
        }
    }

    if (pCurDeviceHandle == nullptr)
    {
        return false;
    }

    FILE* pf = fopen(pfilename, "rb");
    if (pf == nullptr)
    {
        return false;
    }
    int ret = fseek(pf, 0, SEEK_END);
    int filelen = ftell(pf);
    ret = fseek(pf, 0, SEEK_SET);

    unsigned char* buf = new unsigned char[filelen];
    memset(buf, 0, filelen);

    ret = fread(buf, 1, filelen, pf);
    fclose(pf);
    pf = nullptr;

    if (strncmp((char*)buf, "CY", 2))
    {
        fprintf(stderr, "Error: Image does not have 'CY' at start.\n");
        delete[] buf;
        return false;
    }
    if (buf[2] & 0x01)
    {
        fprintf(stderr, "Error: Image does not contain executable code\n");
        delete[] buf;
        return false;
    }

    if (buf[3] != 0xB0)
    {
        fprintf(stderr, "Error: Not a normal FW binary with checksum\n");
        delete[] buf;
        return false;
    }

    unsigned int nCheckSum = 0;
    int istart = 4;
    while (istart < filelen)
    {
        unsigned int* pdata = (unsigned int*)(buf + istart);
        unsigned int dataleng = pdata[0];
        unsigned int dataaddr = pdata[1];

        if (dataleng != 0)
        {
            unsigned int* psum = pdata + 2;
            for (int i = 0; i < dataleng; ++i)
            {
                nCheckSum += psum[i];
            }
            int len = dataleng * 4;
            int block = 0;
            while (len > 0)
            {
                int trslen = (len > 4096) ? 4096 : len;
                ret = libusb_control_transfer(pCurDeviceHandle, 0x40, 0xA0, (dataaddr & 0x0000FFFF), dataaddr >> 16, &buf[istart + block + 8], trslen, 5000);
                if (ret != trslen)
                {
                    fprintf(stderr, "Error: Vendor write to FX3 RAM failed\n");
                    delete[] buf;
                    return false;
                }
                dataaddr += trslen;
                block += trslen;
                len -= trslen;
            }
        }
        else
        {
            if (nCheckSum != pdata[2])
            {
                fprintf(stderr, "Error: Checksum error in firmware binary\n");
                delete[] buf;
                return false;
            }

            ret = libusb_control_transfer(pCurDeviceHandle, 0x40, 0xA0, (dataaddr & 0x0000FFFF), dataaddr >> 16, nullptr, 0, 5000);
            if (ret != 0)
            {
                printf("Ignored error in control_transfer: %d\n", ret);
            }

            break;
        }

        istart += (8 + dataleng * 4);
    }

    delete[] buf;

    //libusb_reset_device(pCurDeviceHandle);
    libusb_close(pCurDeviceHandle);
    pCurDeviceHandle = nullptr;
    libusb_free_device_list(devlist, 1);
    devlist = nullptr;

    return true;
}

bool UsbCyclone::SetInterAltinter(unsigned char selif, unsigned char altif)
{
    if (m_pCurDeviceHandle == nullptr)
    {
        return false;
    }
    int ret = libusb_claim_interface(m_pCurDeviceHandle, selif);
    if (ret != 0)
    {
        return false;
    }
    ret = libusb_set_interface_alt_setting(m_pCurDeviceHandle, selif, altif);
    if (ret != 0)
    {
        return false;
    }
    return true;
}

bool UsbCyclone::ReleaseInterAltinter(unsigned char selif, unsigned char altif)
{
    if (m_pCurDeviceHandle == nullptr)
    {
        return false;
    }
    int ret = libusb_detach_kernel_driver(m_pCurDeviceHandle, selif);
    if (ret != 0)
    {
        return false;
    }
    ret = libusb_release_interface(m_pCurDeviceHandle, selif);
    if (ret != 0)
    {
        return false;
    }
    return true;
}

int UsbCyclone::FPGARegisterWrite(unsigned short addr, unsigned int data)
{
    if (m_pCurDeviceHandle == nullptr)
    {
        return -1;
    }
    long len = 16;
    unsigned char buf[] = { 0xFF, 0xAA, 0x01, 0x07, 0x00, 0x00, 0x00, 0x1, (unsigned char)(addr & 0xff), 00, (unsigned char)(data & 0xff), (unsigned char)((data >> 8) & 0xff), (unsigned char)((data >> 16) & 0xff), (unsigned char)((data >> 24) & 0xff), 0x00, 0x00 };
    return libusb_control_transfer(m_pCurDeviceHandle, 0x40, 0, 0, 0, buf, len, 1000);
}

void UsbCyclone::FPGARegisterReadBack(unsigned short addr, unsigned int& rValue, bool& result)
{
    result = false;
    if (m_pCurDeviceHandle == nullptr)
    {
        return;
    }
    unsigned char buf[] = { 0xFF, 0xAA, 0x01, 0x07, 0x00, 0x00, 0x00, 0x02, (unsigned char)(addr & 0xff), (unsigned char)((addr >> 8) & 0xff), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    long datalen = 16;
    libusb_control_transfer(m_pCurDeviceHandle, 0x40, 0, 0, 0, buf, datalen, 1000);

    unsigned char rbuf[16] = { 0 };
    long rlen = sizeof(rbuf) / sizeof(rbuf[0]);
    if (libusb_control_transfer(m_pCurDeviceHandle, 0xC0, 0, 0, 0, rbuf, rlen, 1000) > 0)
    {
        rValue = 0;
        if (rbuf[3] == 0x05 && rlen >= 8)
        {
            rValue = rbuf[4] | (rbuf[5] << 8) | (rbuf[6] << 16) | (rbuf[7] << 24);
            result = true;
        }
    }
}

int UsbCyclone::BulkDataTransfer(unsigned char* data, int len, int* reallen, unsigned int timeout)
{
    return libusb_bulk_transfer(m_pCurDeviceHandle, m_ucRecvEndpoint, data, len, reallen, timeout);
}

bool UsbCyclone::CloseUsbDevice(void)
{
    if (m_pCurDeviceHandle == nullptr)
    {
        return false;
    }
    libusb_clear_halt(m_pCurDeviceHandle, m_ucSendEndpoint);
    libusb_clear_halt(m_pCurDeviceHandle, m_ucRecvEndpoint);
    ReleaseInterAltinter(0, 0);
    libusb_close(m_pCurDeviceHandle);
    m_pCurDeviceHandle = nullptr;

    return true;
}
