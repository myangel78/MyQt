//#include "pch.h"
#include "UsbProtoMsg.h"

#include <string.h>

#include "ConfigServer.h"


//同步更新
static const char* PVERCODE = "1001";//16进制版本号//"V1.0.0.1"
//constexpr long long LLVERCODE = 0x1001;


std::atomic<long long> UsbProtoMsg::m_iSeqNum = 0;



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
        vetOut.push_back(temp);
        pos += beg;
        strSrc = strSrc.substr(pos);
        pos = strSrc.find_first_of(pde);
    }
    if (strSrc.size() > 0)
    {
        vetOut.push_back(strSrc);
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


int UsbProtoMsg::Hex2Dec(char hex)
{
    if (hex >= 'a' && hex <= 'f')
    {
        return hex - 'a' + 10;
    }
    else if (hex >= 'A' && hex <= 'F')
    {
        return hex - 'A' + 10;
    }
    else if (hex >= '0' && hex <= '9')
    {
        return hex - '0';
    }
    return hex;
}

float UsbProtoMsg::GetHexCurrent(short dc, int range, int tot)
{
    auto val = dc * 2.0 * range / tot;
    return val;
    double cur = range / 2.0;
    auto ret = dc * 1.0 * range / tot - cur;
    return ret;
}

int UsbProtoMsg::GetCurrentHex(double dc, int range, int tot)
{
    double cur = range / 2.0;
    int ret = (int)(tot * (cur + dc) / range);
    return ret;
}

std::string UsbProtoMsg::GetCurrentVer(void)
{
    int mainver = Hex2Dec(PVERCODE[0]);
    int secdver = Hex2Dec(PVERCODE[1]);
    int thrdver = Hex2Dec(PVERCODE[2]);
    int miniver = Hex2Dec(PVERCODE[3]);

    char arver[16] = { 0 };
    sprintf(arver, "V%d.%d.%d.%d", mainver, secdver, thrdver, miniver);

    return std::string(arver);
}

size_t UsbProtoMsg::StartCollect(unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    pout[0] = '0';
    return 0x80000000;
    memcpy(pout, "80000000", 8);
    return 8;

#else

    const char* pcmd = "55AA00120001";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    const char* pbody = "00000111";
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA00120001000000001001000001110000";

#endif

    return strlen((const char*)pout);// == 36;
}

size_t UsbProtoMsg::StartCollect(int& pout)
{
    pout = 0;
    return 0x10000000;
}

size_t UsbProtoMsg::StopCollect(unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    pout[0] = '0';
    return 0x00000000;
    memcpy(pout, "00000000", 8);
    return 8;

#else

    const char* pcmd = "55AA00120002";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    const char* pbody = "00000117";
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA00120002000000001001000001170000";

#endif

    return strlen((const char*)pout);// == 36;
}

size_t UsbProtoMsg::StopCollect(int& pout)
{
    pout = 0;
    return 0;
}

size_t UsbProtoMsg::SetAllChannelRate(int rate, unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    //return (0x4800 << 16) + rate;
    //char szbody[32] = { 0 };
    //sprintf(szbody, "4800%04X", rate);
    //int iret = strlen(szbody);
    //memcpy(pout, szbody, iret);
    //return iret;

#else

    const char* pcmd = "55AA00160003";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    char arbody[32] = { 0 };
    sprintf(arbody, "01162620%08X", rate);
    const char* pbody = arbody;
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA001600030000000010010116262000004E200000";

#endif

    return strlen((const char*)pout);// == 44;
}

size_t UsbProtoMsg::SetAllChannelRate(int rate, int& pout)
{
    pout = 4;
    if (rate == SAMPLE_RATE_20K)
    {
        return 0;
    }
    else if (rate == SAMPLE_RATE_5K)
    {
        return 2;
    }
    return 0;
}

size_t UsbProtoMsg::SetOverSampMultiple(int multiple, int& pout)
{
    pout = 1;
    return (0x8460 + multiple);
}

size_t UsbProtoMsg::SetOneDCLevel(int chan, double lvl, unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    pout[0] = '2';
    return ((chan & 0xF3) << 20) + ((chan & 0x0F) << 16) + GetCurrentHex(lvl);
    char szbody[32] = { 0 };
    sprintf(szbody, "40%02X%04X", chan, GetCurrentHex(lvl));
    int iret = strlen(szbody);
    memcpy(pout, szbody, iret);
    return iret;

#else

    const char* pcmd = "55AA00160005";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    char arbody[32] = { 0 };
    sprintf(arbody, "00011221%04X%04X", chan, GetCurrentHex(lvl));
    const char* pbody = arbody;
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA00160005000000001001000112210000851E0000";

#endif

    return strlen((const char*)pout);// == 44;
}

size_t UsbProtoMsg::SetOneDCLevel(int chan, double lvl, int& pout)
{
    pout = 2;
    //int hi = (chan & 0xF0) << 20;
    //int li = (chan & 0x0F) << 16;
    //int ci = 0x3 << 20;
    //size_t res = hi + ci + li + GetCurrentHex(lvl);
    return ((chan & 0xF0) << 20) + (0x3 << 20) + ((chan & 0x0F) << 16) + GetCurrentHex(lvl);
    return ((chan & 0xF3) << 20) + ((chan & 0x0F) << 16) + GetCurrentHex(lvl);
}

size_t UsbProtoMsg::SetAllDCLevel(double lvl, unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    pout[0] = '2';
    return (0x10A0 << 16) + GetCurrentHex(lvl);
    char szbody[32] = { 0 };
    sprintf(szbody, "4800%04X", GetCurrentHex(lvl));
    int iret = strlen(szbody);
    memcpy(pout, szbody, iret);
    return iret;

#else

    const char* pcmd = "55AA00160006";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    char arbody[32] = { 0 };
    sprintf(arbody, "000112220020%04X", GetCurrentHex(lvl));
    const char* pbody = arbody;
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA00160006000000001001000112220020851E0000";

#endif

    return strlen((const char*)pout);// == 44;
}

size_t UsbProtoMsg::SetAllDCLevel(double lvl, int& pout)
{
    pout = 2;
    return (0x10A0 << 16) + GetCurrentHex(lvl);
}

size_t UsbProtoMsg::SetOnePulseLevel(int chan, double lvl, unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    //return (0x40 << 24) + (chan << 16) + GetCurrentHex(lvl);
    //char szbody[32] = { 0 };
    //sprintf(szbody, "40%02X%04X", chan, GetCurrentHex(lvl));
    //int iret = strlen(szbody);
    //memcpy(pout, szbody, iret);
    //return iret;

#else

    const char* pcmd = "55AA00160007";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    char arbody[32] = { 0 };
    sprintf(arbody, "00011423%04X%04X", chan, GetCurrentHex(lvl));
    const char* pbody = arbody;
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA00160007000000001001000114230000851E0000";

#endif

    return strlen((const char*)pout);// == 44;
}

size_t UsbProtoMsg::SetOnePulseLevel(int chan, double lvl, int& pout)
{
    pout = 2;
    return ((chan & 0xF3) << 20) + ((chan & 0x0F) << 16) + GetCurrentHex(lvl);
}

size_t UsbProtoMsg::SetAllPulseLevel(double lvl, unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    //return (0x4800 << 16) + GetCurrentHex(lvl);
    //char szbody[32] = { 0 };
    //sprintf(szbody, "4800%04X", GetCurrentHex(lvl));
    //int iret = strlen(szbody);
    //memcpy(pout, szbody, iret);
    //return iret;

#else

    const char* pcmd = "55AA00160008";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    char arbody[32] = { 0 };
    sprintf(arbody, "000114240020%04X", GetCurrentHex(lvl));
    const char* pbody = arbody;
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA00160008000000001001000114240020851E0000";

#endif

    return strlen((const char*)pout);// == 44;
}

size_t UsbProtoMsg::SetAllPulseLevel(double lvl, int& pout)
{
    pout = 2;
    return (0x10A0 << 16) + GetCurrentHex(lvl);
}

size_t UsbProtoMsg::SetAllTrianglewaveLevel(double lvl, int rate, unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    pout[0] = '2';
    return (0x50A0 << 16) + GetCurrentHex(lvl);
    char szbody[32] = { 0 };
    sprintf(szbody, "4800%04X", GetCurrentHex(lvl));
    int iret = strlen(szbody);
    memcpy(pout, szbody, iret);
    return iret;

#else

    const char* pcmd = "55AA00160009";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    char arbody[32] = { 0 };
    sprintf(arbody, "00000115%04X%04X", GetCurrentHex(lvl), rate);
    const char* pbody = arbody;
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA0016000900000000100100000115851E000A0000";

#endif

    return strlen((const char*)pout);// == 44;
}

size_t UsbProtoMsg::SetAllTrianglewaveLevel(double lvl, int rate, int& pout)
{
    pout = 2;
    return (0x50A0 << 16) + GetCurrentHex(lvl);
}

size_t UsbProtoMsg::SetAllRectanglewaveLevel(double lvl, int rate, unsigned char* pout)
{
#ifdef USE_OLD_CTRLPROTOCOL

    //return (0x4800 << 16) + GetCurrentHex(lvl);
    //char szbody[32] = { 0 };
    //sprintf(szbody, "4800%04X", GetCurrentHex(lvl));
    //int iret = strlen(szbody);
    //memcpy(pout, szbody, iret);
    //return iret;

#else

    const char* pcmd = "55AA0016000A";
    char arseq[16] = { 0 };
    sprintf(arseq, "%08llX", m_iSeqNum++);
    const char* pseq = arseq;
    char arbody[32] = { 0 };
    sprintf(arbody, "00000113%04X%04X", GetCurrentHex(lvl), rate);
    const char* pbody = arbody;
    const char* pcrc = "0000";

    int cmdlen = strlen(pcmd);
    memcpy(pout, pcmd, cmdlen);
    int seqlen = strlen(pseq);
    memcpy(pout + cmdlen, pseq, seqlen);
    size_t nret = cmdlen + seqlen;//
    int verlen = strlen(PVERCODE);
    memcpy(pout + nret, PVERCODE, verlen);
    nret += verlen;
    int bodylen = strlen(pbody);
    memcpy(pout + nret, pbody, bodylen);
    nret += bodylen;
    int crclen = strlen(pcrc);
    memcpy(pout + nret, pcrc, crclen);
    nret += crclen;

    return nret;//"55AA0016000A00000000100100000113851E000A0000";

#endif

    return strlen((const char*)pout);// == 44;
}

size_t UsbProtoMsg::SetAllRectanglewaveLevel(double lvl, int rate, int& pout)
{
    pout = 2;
    return (0x10A0 << 16) + GetCurrentHex(lvl);
}
