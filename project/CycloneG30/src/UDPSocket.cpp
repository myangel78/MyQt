#include "UDPSocket.h"
#include <QtNetwork>
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QThread>
#include "Log.h"
#include "DataHandle.h"
#ifdef Q_OS_WIN32
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#endif



#define SELECT_SOCKET


UDPSocket::UDPSocket(QObject *parent):QObject(parent)
{
    m_rcvBytesCnt = 0;
    QObject::connect(&m_rcvBytesTimer,&QTimer::timeout,[&](){
//        LOGCI("Total bytes is {}Bytes/sec, Turn to MB is {}MB/sec",m_rcvBytesCnt,m_rcvBytesCnt/1024/1024);
//        LOGCI("Datahandle queue size:{}",m_pDataHandle->m_queDataPack.size_approx());
        emit SpeedDispSig(m_rcvBytesCnt * 1.0/1024/1024);
        m_rcvBytesCnt = 0;
    });

}

UDPSocket::~UDPSocket(void)
{
     qDebug()<< "~UDPSocket thread id = " << QThread::currentThreadId();
}

QStringList UDPSocket::OnSearchLocalIp(void)
{
    QStringList ipStrList;
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);

    foreach (QHostAddress address, info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ipStrList.append(address.toString());
        }
    }
    return ipStrList;
}



bool UDPSocket::OnSetUdpSocket(const QString localIp,const int localPort)
{
    m_localIp = localIp;
    m_localPort = localPort;
    return true;
}


void UDPSocket::ThreadRecvFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());

    std::vector<uint8_t> vctDataCache;
    vctDataCache.reserve(4096);
    static int cnt = 0;


    fd_set readfd;                // 读描述符集
    struct sockaddr_in addr,clientAddr;        // 客户端地址
    int cliAddrLen; // 客户端地址长度!!!!!!
    char recv_buf[4096];        // 接收缓冲区
    memset(recv_buf, 0, sizeof(recv_buf)); // 清空接收缓冲区

    int nRet = 0;

#ifdef Q_OS_WIN32
    WSADATA wsaData; // 套接口信息数据
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        LOGCI("failed to laod winsock!/n");
        return;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == INVALID_SOCKET)
    {
        LOGCI("Create socket failed!!!");
        closesocket(udpSocket);
        return;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_localPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(udpSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)// bind socka
    {
        LOGCI("Bind socket failed");
        closesocket(udpSocket);
        return;
    }

    int nRecvBuf = 50*1024 * 1024; //50M
    if (0 != setsockopt(udpSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int)))
    {
        LOGCI("setsockopt buf size failed.{}", WSAGetLastError());
        return ;
    }

    int opt_val = 0;
    int opt_val_MB = 0;
    int opt_len = sizeof(opt_val);
    if(0 != getsockopt(udpSocket, SOL_SOCKET, SO_RCVBUF,(char*)&opt_val,  &opt_len))
    {
        LOGCI("fail to getsockopt");
    }
    opt_val_MB = opt_val *1.0/1024/1024;
    LOGCI("getsockopt Udp RecvBuf = {}M", opt_val_MB );

#ifdef SELECT_SOCKET
    // 设置超时时间为6s
    timeval timeout;        // 定时变量
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
#else
    // 设置超时
    struct timeval recvTimeout;
    recvTimeout.tv_sec = 2;//秒
    recvTimeout.tv_usec = 0;//微秒
    if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO,(const char*)&recvTimeout, sizeof(recvTimeout)) == -1)
    {
        LOGCI("setsockopt failed:");
    }
#endif

    bool bUdpTestPrint = ConfigServer::GetInstance()->GetIsStreamPrint();

    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexRecv);
            m_cvRecv.wait(lck, [&]{return !m_bRunning || !m_bSuspend;});
        }

        if (!m_bRunning)
        {
            break;
        }
#ifdef SELECT_SOCKET
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&readfd); // 在使用之前总是要清空
        FD_SET(udpSocket, &readfd);

        nRet = select(0, &readfd, NULL, NULL, &timeout);// 检测是否有套接口是否可读
        if (nRet == SOCKET_ERROR)
        {
            LOGCI("Select SOCKET_ERROR");
            break;
        }
        else if (nRet == 0)        // 超时
        {
            LOGCI("Select socket TimeOut");
            continue;
//            break;
        }
        else        // 检测到有套接口可读
        {
            if (FD_ISSET(udpSocket, &readfd))        // socka可读
            {
                cliAddrLen = sizeof(clientAddr);
                int nRecEcho = recvfrom(udpSocket, recv_buf, sizeof(recv_buf), 0, (sockaddr*)&clientAddr, &cliAddrLen);
                if (nRecEcho == INVALID_SOCKET)
                {
                    LOGCI("recvfrom error");
                    break;
                }
                else
                {
                    if(bUdpTestPrint)
                    {
                        LOGCI("Receive package!!! Print Binary {}: {:32}",nRecEcho,spdlog::to_hex(std::begin(recv_buf), std::begin(recv_buf) + nRecEcho));
                    }
                    else
                    {
                        #if 0
                        auto dtptr = std::make_shared<DataHandle::SDataPack>((unsigned char *)&recv_buf[0],nRecEcho);
                        m_rcvBytesCnt += dtptr->dataPack.size();
                        m_pDataHandle->m_queDecodePack.enqueue(std::move(dtptr));
                        m_pDataHandle->m_cvDcode.notify_one();
                        #else
                        int oldsize = (int)vctDataCache.size();
                        vctDataCache.resize(oldsize + nRecEcho);
                        m_rcvBytesCnt += nRecEcho;
                        std::copy(recv_buf,recv_buf+ nRecEcho,vctDataCache.begin() + oldsize);

                        if(++cnt >= 4)
                        {
                            auto dtptr = std::make_shared<DataHandle::SDataPack>(std::move(vctDataCache));
                            m_pDataHandle->m_queDecodePack.enqueue(std::move(dtptr));
                            m_pDataHandle->m_cvDcode.notify_one();
                            cnt = 0;
                        }
                        #endif
                    }
                }
            }
        }
#else
        cliAddrLen = sizeof(clientAddr);
        int nRecEcho = recvfrom(udpSocket, recv_buf, sizeof(recv_buf), 0, (sockaddr*)&clientAddr, &cliAddrLen);
        if (nRecEcho == INVALID_SOCKET)
        {
            LOGCI("Udp recvfrom error");
//            break;
        }
        else
        {
            if(bUdpTestPrint)
            {
                LOGCI("Receive package!!! Print Binary {}: {:32}",nRecEcho,spdlog::to_hex(std::begin(recv_buf), std::begin(recv_buf) + nRecEcho));
            }
            else
            {
                #if 0
                if(nRecEcho >= 4 )
                {
                    uint32_t leng = (uint8_t)recv_buf[0] <<24 | (uint8_t)recv_buf[1]<<16 | (uint8_t)recv_buf[2]<<8 | (uint8_t)recv_buf[3];
                    if(m_saveLen != leng-1 )
                    {
                        LOGCI("Len is not discontinuity, last prev{:x} now {:x}",m_saveLen,leng);
                    }
                    m_saveLen = leng;
                }
//                LOGCI("Receive package!!! Print Binary {}: {:4}",nRecEcho,spdlog::to_hex(std::begin(recv_buf), std::begin(recv_buf) + 4));
                #endif
                #if 1
                auto dtptr = std::make_shared<DataHandle::SDataPack>((unsigned char *)&recv_buf[0],nRecEcho);
                m_rcvBytesCnt += dtptr->dataPack.size();
                m_pDataHandle->m_queDecodePack.enqueue(std::move(dtptr));
                m_pDataHandle->m_cvDcode.notify_one();
                #else
                int oldsize = (int)vctDataCache.size();
                vctDataCache.resize(oldsize + nRecEcho);
                m_rcvBytesCnt += nRecEcho;
                std::copy(recv_buf,recv_buf + nRecEcho,vctDataCache.begin() + oldsize);

                if(++cnt >= 4)
                {
                    auto dtptr = std::make_shared<DataHandle::SDataPack>(std::move(vctDataCache));
                    m_pDataHandle->m_queDecodePack.enqueue(std::move(dtptr));
                    m_pDataHandle->m_cvDcode.notify_one();
                    cnt = 0;
                }
                #endif
            }
        }
#endif
    }
    closesocket(udpSocket);
#else

    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1)
    {
        LOGI("Create socket failed!!!");
        close(udpSocket);
        return;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_localPort);
    addr.sin_addr.s_addr = inet_addr("192.168.0.3");//htonl(INADDR_ANY);//INADDR_ANY;//

    int nRecvBuf = 50*1024 * 1024; //50M
    if (0 != setsockopt(udpSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int)))
    {
        //LOGCI("setsockopt buf size failed.{}", WSAGetLastError());
        return ;
    }

    int opt_val = 0;
    int opt_val_MB = 0;
    int opt_len = sizeof(opt_val);
    if(0 != getsockopt(udpSocket, SOL_SOCKET, SO_RCVBUF,(void*)&opt_val,  (socklen_t*)&opt_len))
    {
        LOGI("fail to getsockopt");
    }
    opt_val_MB = opt_val *1.0/1024/1024;
    LOGI("getsockopt Udp RecvBuf = {}M", opt_val_MB );

    //int flag = 1;
    //setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

    if (bind(udpSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1)// bind socka
    {
        LOGI("Bind socket failed");
        close(udpSocket);
        return;
    }

    // 设置超时时间为6s
    timeval timeout;        // 定时变量
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    bool bUdpTestPrint = ConfigServer::GetInstance()->GetIsStreamPrint();

    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexRecv);
            m_cvRecv.wait(lck, [&]{return !m_bRunning || !m_bSuspend;});
        }

        if (!m_bRunning)
        {
            break;
        }

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        FD_ZERO(&readfd); // 在使用之前总是要清空
        FD_SET(udpSocket, &readfd);

        nRet = select(udpSocket+1, &readfd, NULL, NULL, &timeout);// 检测是否有套接口是否可读
        if (nRet == -1)
        {
            LOGI("Select SOCKET_ERROR");
            break;
        }
        else if (nRet == 0)        // 超时
        {
            LOGI("Select socket TimeOut");
            continue;
        }
        else        // 检测到有套接口可读
        {
            if (FD_ISSET(udpSocket, &readfd))        // socka可读
            {
                cliAddrLen = sizeof(clientAddr);
                int nRecEcho = recvfrom(udpSocket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&clientAddr, (socklen_t*)&cliAddrLen);
                if (nRecEcho == -1)
                {
                    LOGI("recvfrom error");
                    break;
                }
                else
                {
                    if(bUdpTestPrint)
                    {
                        LOGI("Receive package!!! Print Binary {}: {:32}",nRecEcho,spdlog::to_hex(std::begin(recv_buf), std::begin(recv_buf) + nRecEcho));
                    }
                    else
                    {
                        int oldsize = (int)vctDataCache.size();
                        vctDataCache.resize(oldsize + nRecEcho);
                        m_rcvBytesCnt += nRecEcho;
                        std::copy(recv_buf,recv_buf+ nRecEcho,vctDataCache.begin() + oldsize);

                        if(++cnt >= 4)
                        {
                            auto dtptr = std::make_shared<DataHandle::SDataPack>(std::move(vctDataCache));
                            m_pDataHandle->m_queDecodePack.enqueue(std::move(dtptr));
                            m_pDataHandle->m_cvDcode.notify_one();
                            cnt = 0;
                        }
                    }
                }
            }
        }
    }
    close(udpSocket);
#endif
    LOGI("ThreadExit={}!!!={}", __FUNCTION__,ConfigServer::GetCurrentThreadSelf());
}

bool UDPSocket::StartThread(void)
{
    if (!m_bRunning)
    {
        m_rcvBytesTimer.start(1000);
        m_bRunning = true;
        m_ThreadRecvPtr = std::make_shared<std::thread>(&UDPSocket::ThreadRecvFunc, this);
        return m_ThreadRecvPtr != nullptr;
    }
    return false;
}

bool UDPSocket::EndThread(void)
{
    m_rcvBytesTimer.stop();
    m_bRunning = false;
    m_cvRecv.notify_one();
    return true;
}

bool UDPSocket::StopThread(void)
{
    m_bRunning = false;
    m_cvRecv.notify_one();

    if (m_ThreadRecvPtr != nullptr)
    {
        if (m_ThreadRecvPtr->joinable())
        {
            m_ThreadRecvPtr->join();
        }
        m_ThreadRecvPtr.reset();
    }
    return true;
}
