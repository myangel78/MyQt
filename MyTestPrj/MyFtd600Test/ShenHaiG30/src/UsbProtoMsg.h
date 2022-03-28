#pragma once
#include "qglobal.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <memory>


typedef union {
    struct ASICControl
    {
        uint32_t data:32;
        uint8_t addr:8;
        uint8_t rwmode:8;
        uint16_t reserve:16;
    }ASICControl;
    qulonglong controlData;
}ASICControlUnion;

typedef enum
{
    ASIC_SPI_READ_MODE = 0x81,
    ASIC_SPI_WRITE_MODE = 0x80,
}ASIC_SPI_RW_MODE_ENUM;

typedef enum
{
    FPGA_READ_MODE = 0x0,
    FPGA_WRITE_MODE = 0xF,
}FPGA_RW_MODE_ENUM;

typedef enum
{
    FPGA_CTRL_NOT_SPI_MODE = 0x00,
    FPGA_CTRL_SPI_READ_MODE = 0x80,
    FPGA_CTRL_SPI_WRITE_MODE = 0x40,
}FPGA_CTRL_SPI_RW_MODE_ENUM;

typedef enum
{
    ASIC_FIRST_CHIP = 0x0,
    ASIC_SECOND_CHIP,
}ASIC_CHIP_SERIAL_NUMBER;



typedef enum
{
    FPGA_ALL_CONTROL_REG =0,
    FPGA_ASIC_ADDR_REG ,
    FPGA_EXTRA_DA_WRIGE_REG ,
    FPGA_EXTRA_AD_WRIGE_REG ,
    FPGA_POWER_ENABLE_REG ,
    FPGA_ASIC_MAIN_CLK_REG ,
    FPGA_TEST_IO_WR_REG ,
    FPGA_SENSER_SFT_REG,
}FPGA_CONTROL_REG_ENUM;

typedef union {
    struct LVDSFormat
    {
        uint32_t crc1:2;
        uint32_t dat6_2:5;
        uint32_t :1;
        uint32_t dat13_7:7;
        uint32_t :1;
        uint32_t dat15_14:2;
        uint32_t crc2:2;
        uint32_t stateID_2_0:3;
        uint32_t :1;
        uint32_t stateID_3:1;
        uint32_t frameID:4;
        uint32_t start:1;
        uint32_t direct:1;
        uint32_t :1;
    }LVDSFormat;
    uint32_t lvdsData;
}ADCLVDSUnion;




class UsbProtoMsg
{
public:
    UsbProtoMsg() = default;
    ~UsbProtoMsg() = default;

public:
    /*
    * 写读ASIC存器命令
    * chip 芯片片选
    * mode 读写模式
    * addr 寄存器地址
    * data 寄存器值
    * 返回命令码
    */
    static qulonglong AsicDataEncode(const int &mode,const uchar &addr, const uint32_t &data);

    static qulonglong WriteAsicDataEncode(const uchar &addr, const uint32_t &data);
    static qulonglong ReadAsicDataEncode(const uchar &addr, const uint32_t &data);

    /**
     * @brief 控制USB是数据流还是命令
     * @param bStream 是否是数据流
     * @return 控制指令
     */
    static qulonglong SwitchToUSBCmdOrDataEncode(const bool bStream);
    static qulonglong SetStreamHeader(const uchar &head);

    /**
     * @brief 控制UDP转向TestMode模式
     * @return
     */
    static qulonglong SwitchToUdpTestModeCmdEncode(void);


    /*
    * 发给FPGA寄存器命令
    * type 控制的类型 ASIC/DA/AD
    * acWriteBuf 数据缓存区
    * length 数据缓存区长度
    * 返回实际长度
    */
    static size_t SendWriteCmdToFPGAEncode(const int & type,const int & spiRwMode,uchar *acWriteBuf,const unsigned long &length,const qulonglong &data);
    static size_t SendReadCmdToFPGAEncode(const int &type,const int & spiRwMode,uchar *acWriteBuf,const unsigned long &length,const qulonglong &data);


    /*
    * 模拟生成通道的LVDS数据
    */
    static std::unique_ptr<ADCLVDSUnion> OnMakeOneChannel(const uint16_t &channel,const uint16_t &data);
    static bool OnMakeOneFrame(const char &FrameIndex,uint8_t *array,uint32_t &index);
    static uint32_t OnMakeSixteenthFrame(uint8_t *dataArray,uint32_t &rIndex);
    static uint32_t OnEncodeTheFrame(uint8_t *dataArray,uint32_t &arrayIndex);

    /**
     * @brief 分割字符串
     * @param pSrc 原始字符串
     * @param pde  分割字符
     * @param vetOut 分割后容器
     * @return 是否成功
     */
    static bool SplitString(const char* pSrc, const char* pde, std::vector<std::string>& vetOut);

    static long long String2Hex(const char* pSrc);

    /**
     * @brief OnDecodeReturnFrame 解析返回的命令帧
     * @param pDataArray 需要被解析的源数据包
     * @param length 需要被解析的源数据包长度
     * @param addr  需要读回的ASIC地址
     * @param data  需要读回的寄存器数据
     * @return
     */
    static bool OnDecodeReturnFrame(const uint8_t *pDataArray,ulong length,const uchar &addr,uint &data);


    static uint8_t crc8x_simple(uint8_t crc, void const *mem, size_t len);
    static uint8_t crc8x_fast(uint8_t crc, void const *mem, size_t len);

    static bool CalculateCRCFromFrame(const unsigned char* psrc,const char &frame);
    static bool CalculateCRCForTwoCh(const unsigned char* ptr,unsigned char &crcExtract,unsigned char &crcCalRes);


};

