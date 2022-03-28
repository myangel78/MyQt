#include "UsbProtoMsg.h"
#include "Util.h"
#include "ConfigServer.h"

static uint8_t const crc8x_table[] = {
    0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 0xb9, 0x88, 0xdb, 0xea, 0x7d,
    0x4c, 0x1f, 0x2e, 0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 0xfa, 0xcb,
    0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d, 0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20,
    0x11, 0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8, 0xc5, 0xf4, 0xa7, 0x96,
    0x01, 0x30, 0x63, 0x52, 0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb, 0x3d,
    0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71,
    0x22, 0x13, 0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 0xc7, 0xf6, 0xa5,
    0x94, 0x03, 0x32, 0x61, 0x50, 0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c,
    0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95, 0xf8, 0xc9, 0x9a, 0xab, 0x3c,
    0x0d, 0x5e, 0x6f, 0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6, 0x7a, 0x4b,
    0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65,
    0x54, 0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 0x80, 0xb1, 0xe2, 0xd3,
    0x44, 0x75, 0x26, 0x17, 0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 0x45,
    0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2, 0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a,
    0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91, 0x47, 0x76, 0x25,
    0x14, 0x83, 0xb2, 0xe1, 0xd0, 0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 0xbd, 0x8c, 0xdf, 0xee, 0x79,
    0x48, 0x1b, 0x2a, 0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49,
    0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef, 0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24,
    0x15, 0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac};

bool UsbProtoMsg::SplitString(const char* pSrc, const char* pde, std::vector<std::string>& vetOut)
{
    if (pSrc == nullptr)
    {
        return false;
    }
    std::string strSrc = pSrc;
    size_t beg = strlen(pde);
    size_t pos = strSrc.find_first_of(pde);
    while (pos != std::string::npos)
    {
        std::string temp = strSrc.substr(0, pos);
        vetOut.push_back(strSrc.substr(0, pos));
        pos += beg;
        strSrc = strSrc.substr(pos);
        pos = strSrc.find_first_of(pde);
    }
    if (strSrc.size() > 0)
    {
        vetOut.push_back(strSrc.c_str());
    }

    return vetOut.size() > 0;
}

long long UsbProtoMsg::String2Hex(const char* pSrc)
{
    if (pSrc == nullptr)
    {
        return 0;
    }
    long long iret = 0;

#if 1
    sscanf(pSrc, "%Ix", &iret);
#else
    char* pDst;
    iret = strtol(pSrc, &pDst, 16);
#endif

    return iret;
}




qulonglong UsbProtoMsg::AsicDataEncode(const int &mode,const uchar &addr, const uint &data)
{
    ASICControlUnion control;
    control.ASICControl.reserve = 0;
    control.ASICControl.rwmode = mode;
    control.ASICControl.addr = addr;
    control.ASICControl.data = data;
    return qulonglong(control.controlData);
}

qulonglong UsbProtoMsg::WriteAsicDataEncode(const uchar &addr, const uint32_t &data)
{
    return AsicDataEncode(ASIC_SPI_WRITE_MODE,addr,data);
}
qulonglong UsbProtoMsg::ReadAsicDataEncode(const uchar &addr, const uint32_t &data)
{
    return AsicDataEncode(ASIC_SPI_READ_MODE,addr,data);
}

qulonglong UsbProtoMsg::SwitchToUSBCmdOrDataEncode(const bool bStream)
{
    return (bStream == true ? 0xF00000000001 : 0xF00000000000);
}

qulonglong UsbProtoMsg::SwitchToUdpTestModeCmdEncode(void)
{
    return  0xF00000000003;
}

qulonglong UsbProtoMsg::SetStreamHeader(const uchar &head)
{
    qulonglong cmd = 0xF00000000001 | (qulonglong)head << 32;
    return cmd;
}

size_t UsbProtoMsg::SendWriteCmdToFPGAEncode(const int & type,const int & spiRwMode,uchar *acWriteBuf,const unsigned long &length,const qulonglong &data)
{
    memset(acWriteBuf,0,length);
    size_t Index = 0;
    acWriteBuf[Index++] = 0x55;
    acWriteBuf[Index++] = 0xAA;
    acWriteBuf[Index++] = (FPGA_WRITE_MODE &0x0F) << 4 | type &0x0f;
    acWriteBuf[Index++] = spiRwMode;
    WriteUint48WithBigEndian(data, acWriteBuf, Index);
    acWriteBuf[Index++] = 0xFA;
    acWriteBuf[Index++] = 0xFA;
    return Index;
}
size_t UsbProtoMsg::SendReadCmdToFPGAEncode(const int &type,const int & spiRwMode,uchar *acWriteBuf,const unsigned long &length,const qulonglong &data)
{
    memset(acWriteBuf,0,length);
    size_t Index = 0;
    acWriteBuf[Index++] = 0x55;
    acWriteBuf[Index++] = 0xAA;
    acWriteBuf[Index++] = (FPGA_READ_MODE &0x0F) << 4 | type &0x0f;
    acWriteBuf[Index++] = spiRwMode;
    WriteUint48WithBigEndian(data, acWriteBuf, Index);
    acWriteBuf[Index++] = 0xFA;
    acWriteBuf[Index++] = 0xFA;
    return Index;
}

std::unique_ptr<ADCLVDSUnion> UsbProtoMsg::OnMakeOneChannel(const uint16_t &channel,const uint16_t &data)
{
    std::unique_ptr<ADCLVDSUnion> pLVDSUnion(new ADCLVDSUnion());
    memset(&pLVDSUnion->lvdsData,0,sizeof(pLVDSUnion->lvdsData));
    char frameId = channel%16;
    pLVDSUnion->LVDSFormat.start =(channel < 16 ? 1: 0);
    pLVDSUnion->LVDSFormat.frameID = frameId;
    pLVDSUnion->LVDSFormat.stateID_2_0 = 0x1;
    pLVDSUnion->LVDSFormat.dat15_14 = (data >> 12) &0x03;
    pLVDSUnion->LVDSFormat.dat13_7 = (data >> 5) &0x7F;
    pLVDSUnion->LVDSFormat.dat6_2 = (data) &0x1F;
    return pLVDSUnion;
}

bool UsbProtoMsg::OnMakeOneFrame(const char &FrameIndex,uint8_t *array,uint32_t &index)
{
    uint16_t startch = FrameIndex;
    array[index++] = LVDS_FRAMEHEAD_FIRST_2D;
    array[index++] = LVDS_FRAMEHEAD_SECOND_5A;
    for(int ch = startch,i =0; i < 64; i ++,ch += 16)
    {
#if 1
        std::unique_ptr<ADCLVDSUnion> ptrCh1 = OnMakeOneChannel(ch,ch *5);
        uint32_t lvdsdata = ptrCh1->lvdsData;
        array[index++] =  (lvdsdata >> 24 )& 0xff;
        array[index++] =  (lvdsdata >> 16) & 0xff;
        array[index++] =  (lvdsdata >> 8) & 0xff;
        array[index++] =  (lvdsdata & 0xff);
//        printf("firstCh : %d firstData: %d \n",ch,ch *5);

#endif
    }
    return true;

}

uint32_t UsbProtoMsg::OnEncodeTheFrame(uint8_t *dataArray,uint32_t &arrayIndex)
{
    return OnMakeSixteenthFrame(dataArray,arrayIndex);
}

uint32_t UsbProtoMsg::OnMakeSixteenthFrame(uint8_t *dataArray,uint32_t &rIndex)
{
    for(char frame = 0; frame <16; frame++)
    {
        OnMakeOneFrame(frame,dataArray,rIndex);
    }
    return rIndex;
}



bool UsbProtoMsg::OnDecodeReturnFrame(const uint8_t *pDataArray,ulong length,const uchar &addr,uint &data)
{
    if(length < USB_CMD_VALID_LENGTH)
    {
        return false;
    }
    if(pDataArray[0] !=  0x5A || pDataArray[1] != 0x5A || pDataArray[10] !=0xFF  || pDataArray[11] != 0xAA )
    {
        return false;
    }
    if(pDataArray[5] == addr)
//    if(pDataArray[3] == 0x00)
    {
        data = 0;
        data = pDataArray[6] << 24 | pDataArray[7] << 16 | pDataArray[8] << 8 | pDataArray[9];
        return true;
    }
    return false;
}


uint8_t UsbProtoMsg::crc8x_simple(uint8_t crcExtract, void const *mem, size_t len) {
    uint8_t const *data = (uint8_t *)mem;
    if (data == NULL)
        return 0xff;
    while (len--) {
        crcExtract ^= *data++;
        for (uint8_t k = 0; k < 8; k++)
            crcExtract = crcExtract & 0x80 ? (crcExtract << 1) ^ 0x31 : crcExtract << 1;
    }
    crcExtract &= 0xff;
    return crcExtract;
}


uint8_t UsbProtoMsg::crc8x_fast(uint8_t crcExtract, void const *mem, size_t len) {
    uint8_t const *data = (uint8_t *)mem;
    if (data == NULL)
        return 0xff;
    crcExtract &= 0xff;
    while (len--)
        crcExtract = crc8x_table[crcExtract ^ *data++];
    return crcExtract;
}

bool UsbProtoMsg::CalculateCRCFromFrame(const unsigned char* psrc,const char &frame)
{
    if(psrc[0] != LVDS_FRAMEHEAD_FIRST_2D || psrc[1] != LVDS_FRAMEHEAD_SECOND_5A  || (psrc[2] &0x3E) != TAGSTART[frame])
    {
        return false;
    }
    uint32_t channel = 0;
    uint32_t byte = 2;
    uint8_t crcExtract = 0;
    uint8_t crcCalRes = 0;
    uint8_t flag = 0;
    uint8_t TagADC48Bits2Ch[6] = {0};
    memset(&TagADC48Bits2Ch[0],0,sizeof(TagADC48Bits2Ch)/sizeof(TagADC48Bits2Ch[0]));

//    qDebug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>{0} {1:d}<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<",__FUNCTION__, frame);
    for(uchar i = 0;i < CHANNELS_IN_ONE_FRAME; ++i)
    {
        auto &&ptr = &psrc[byte];
        channel = (ptr[0] >>1 & 0x0F )+ i *16;
        if(flag == 0)
        {
            flag =1;
            crcExtract |= (ptr[1] >> 2 &0x03) << 6;
            crcExtract |= (ptr[3] &0x03) << 4;
            TagADC48Bits2Ch[0] = ptr[0] << 1 | (ptr[1] << 1 &0x80) >> 7 ;
            TagADC48Bits2Ch[1] = (ptr[1] &0x30) << 2 | (ptr[1] &0x03) << 4 | (ptr[2] &0x78) >> 3;
            TagADC48Bits2Ch[2] = (ptr[2] & 0x07)<< 5 | (ptr[3] &0x7c) >> 2;
//            qDebug("ptr[0]:{0:x},ptr[1]:{1:x},ptr[2]:{2:x},ptr[3]:{3:x}",ptr[0],ptr[1],ptr[2],ptr[3]);
        }
        else if(flag == 1)
        {
            flag = 0;
            crcExtract |= (ptr[1] >> 2 &0x03) << 2;
            crcExtract |= (ptr[3] &0x03) << 0;
            TagADC48Bits2Ch[3] = ptr[0] << 1 | (ptr[1] << 1 &0x80) >> 7 ;
            TagADC48Bits2Ch[4] = (ptr[1] &0x30) << 2 | (ptr[1] &0x03) << 4 | (ptr[2] &0x78) >> 3;
            TagADC48Bits2Ch[5] = (ptr[2] & 0x07)<< 5 | (ptr[3] &0x7c) >> 2;
            crcCalRes = crc8x_fast(0x00, &TagADC48Bits2Ch[0],6);
#if 0
            if(crcCalRes == crcExtract )
                qDebug("channel {0} extract CRC from data: {1:x}, Calculate CRC result: {2:x}",channel,crcExtract,crcCalRes);
            else
                qDebug("channel {0} extract CRC from data: {1:x}, Calculate CRC result: {2:x}",channel,crcExtract,crcCalRes);
#else
            if(crcCalRes != crcExtract )
                qDebug("channel {0} extract CRC from data: {1:x}, Calculate CRC result: {2:x}",channel,crcExtract,crcCalRes);
#endif
            crcCalRes = 0;
            crcExtract =0;
            memset(&TagADC48Bits2Ch[0],0,sizeof(TagADC48Bits2Ch)/sizeof(TagADC48Bits2Ch[0]));
        }
        byte +=4;
    }
    return true;
}

bool UsbProtoMsg::CalculateCRCForTwoCh(const unsigned char* ptr,unsigned char &crcExtract,unsigned char &crcCalRes)
{
    uint8_t TagADC48Bits2Ch[6] = {0};
    memset(&TagADC48Bits2Ch[0],0,sizeof(TagADC48Bits2Ch)/sizeof(TagADC48Bits2Ch[0]));

    crcExtract = 0;
    crcCalRes = 0;

    crcExtract |= (ptr[1] >> 2 &0x03) << 6;
    crcExtract |= (ptr[3] &0x03) << 4;
    crcExtract |= (ptr[5] >> 2 &0x03) << 2;
    crcExtract |= (ptr[7] &0x03) << 0;

    TagADC48Bits2Ch[0] = ptr[0] << 1 | (ptr[1] << 1 &0x80) >> 7 ;
    TagADC48Bits2Ch[1] = (ptr[1] &0x30) << 2 | (ptr[1] &0x03) << 4 | (ptr[2] &0x78) >> 3;
    TagADC48Bits2Ch[2] = (ptr[2] & 0x07)<< 5 | (ptr[3] &0x7c) >> 2;
    TagADC48Bits2Ch[3] = ptr[4] << 1 | (ptr[5] << 1 &0x80) >> 7 ;
    TagADC48Bits2Ch[4] = (ptr[5] &0x30) << 2 | (ptr[5] &0x03) << 4 | (ptr[6] &0x78) >> 3;
    TagADC48Bits2Ch[5] = (ptr[6] & 0x07)<< 5 | (ptr[7] &0x7c) >> 2;
    crcCalRes = crc8x_fast(0x00, &TagADC48Bits2Ch[0],6);

    return crcExtract == crcCalRes;
}

