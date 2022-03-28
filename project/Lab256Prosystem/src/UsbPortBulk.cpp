#include "UsbPortBulk.h"

#ifdef Q_OS_WIN32
#include "UsbCyDevice.h"
#include <CyApi.h>
#else
#include "UsbCyclone.h"
#endif
#include <logger/Log.h>

//#include <qdebug.h>
#include "ConfigServer.h"
#include "CustomPlotView.h"
#include "ControlView.h"
#include "CalibrationDialog.h"
#include "SavePane.h"
#include "CapacitanceView.h"


UsbPortBulk::UsbPortBulk()
{
    m_dataHandle.resize(CHANNEL_NUM);
    //m_dataHandle2.resize(CHANNEL_NUM);
    //m_dataHandle3.resize(CHANNEL_NUM);
    //m_dataHandleAvg.resize(CHANNEL_NUM);
}

UsbPortBulk::~UsbPortBulk()
{
    //m_vetQue.clear();
}

bool UsbPortBulk::StartThread(void)
{
    if (ConfigServer::GetInstance()->GetHardwareAmpBoardVer() == 1)
    {
        m_pRealChannel = REAL_CHANNEL;
    }
    else
    {
        m_pRealChannel = REAL_CHANNEL2;
    }

    if (m_pUsbCyDevice == nullptr)
    {
        LOGE("get handleusbdev addr null");
        return false;
    }
#if MOCKDATA_DEMO_TEST

    //if (ConfigServer::GetInstance()->GetUseDemoData() != 0)
#else

    {
        if (m_pUsbCyDevice->GetCurDeviceHandle() == nullptr)
        {
            LOGE("get handlecurusbdev addr null");
            return false;
        }
    }

#endif

    if (!m_bRunning && m_ThreadPtr == nullptr)
    {
        m_bRunning = true;
        m_ThreadPtr = std::make_shared<std::thread>(&UsbPortBulk::ThreadFunc, this);


#ifdef USE_EACHTHREADRECVANDHANDLE_DATA
        if (!m_bRunningHnadle && m_ThreadHandlePtr == nullptr)
        {
            m_bRunningHnadle = true;
            m_ThreadHandlePtr = std::make_shared<std::thread>(&UsbPortBulk::ThreadHandleFunc, this);
        }
#endif

        return m_ThreadPtr != nullptr;
    }
    return true;
}

void UsbPortBulk::ThreadFunc(void)
{
#if MOCKDATA_DEMO_TEST
    //if (ConfigServer::GetInstance()->GetUseDemoData() != 0)
    {
#if 0

        ::srand((unsigned)time(nullptr));
        int readLength = 576 * 14;// 512 * 16;
        unsigned char* pdatabuf = new unsigned char[readLength];
        while (m_bRunning)
        {
            {
                std::unique_lock<std::mutex> lck(m_mutexRecv);
                m_cvRecv.wait(lck, [this] {return !m_bRunning || !m_bSuspend; });
            }
            memset(pdatabuf, 0, readLength);
            int inx = 0;
            int i = 0;
            for (int j = 0; j < 14; ++j)
            {
                for (; i < inx + 32; i += 2)
                {
                    pdatabuf[i] = 0xAA;
                    pdatabuf[i + 1] = 0xFF;
                }
                inx = i;
                for (; i < inx + 256; ++i)
                {
                    pdatabuf[i] = ::rand() % 256;
                }
                inx = i;
                for (; i < inx + 32; ++i)
                {
                    pdatabuf[i] = 0xFA;
                }
                inx = i;
                for (; i < inx + 256; ++i)
                {
                    pdatabuf[i] = ::rand() % 256;
                }
                inx = i;
            }

            //if (ConfigServer::GetInstance()->GetPrintDemoData() != 0)
            //{
            //    int cntnum = 0;
            //    for (int i = 0; i < readLength; ++i)
            //    {
            //        printf("%02X ", pdatabuf[i]);
            //        ++cntnum;
            //        if (cntnum >= 32)
            //        {
            //            printf("\n");
            //            cntnum = 0;
            //        }
            //    }
            //    printf("\n\n");
            //}
            //else
            //{
            //    Sleep(20);
            //}

#ifdef USE_EACHTHREADRECVANDHANDLE_DATA
            auto dtptr = std::make_shared<SDataPack>(pdatabuf, readLength);
            m_queDataPack.enqueue(std::move(dtptr));

            m_cvHandle.notify_one();

#else
            {
                auto& dp = pdatabuf;// dtpk->dataPack;
                int dtsz = readLength;// dp.size();
                auto& tableData = m_pCalibrationDlg->GetTableData();

                //printf("Datapack_begin!!!=%d\n", dtsz);
                int indx = 0;
                int dtend = dtsz - 576;
                while (m_bRunning && indx < dtend)
                {
                    bool bfound = false;
                    for (int i = indx; i < dtend; ++i)
                    {
                        int item = FindStartFlag(&dp[i], 0xAA, 0xFF);
                        if (item != -1)
                        {
                            indx = item + i;
                            bfound = true;
                            break;
                        }
                    }

                    if (bfound)
                    {
                        if (indx >= dtsz - 544)
                        {
                            break;
                        }
                        for (int i = indx + 256, n = indx + 288; i < n; ++i)
                        {
                            if (dp[i] != 0xFA)
                            {
                                bfound = false;
                                break;
                            }
                        }
                        if (bfound)
                        {
                            if (indx < dtsz - 544 - 576)
                            {
                                for (int i = indx + 544, n = indx + 576; i < n; i += 2)
                                {
                                    if (dp[i] != 0xAA || dp[i + 1] != 0xFF)
                                    {
                                        bfound = false;
                                        break;
                                    }
                                }
                                if (!bfound)
                                {
                                    //printf("Pack incorrect. Not found AA FF2\n");
                                    LOGW("Pack incorrect. Not found AA FF2");
                                    indx += 32;
                                }
                            }
                            if (bfound)
                            {
                                for (int i = 0; i < 128; ++i, indx += 2)
                                {
                                    SetDataHandle(i, dp[indx + 1], dp[indx], tableData[i]);
                                }

                                indx += 32;
                                for (int i = 128; i < 256; ++i, indx += 2)
                                {
                                    SetDataHandle(i, dp[indx + 1], dp[indx], tableData[i]);
                                }

                                if (m_dataHandle[0].size() >= 200)
                                {
                                    auto dtptr = std::make_shared<CustomPlotView::SDataHandle>(m_dataHandle);
                                    m_pCustomPlot->m_queDataHandle.enqueue(std::move(dtptr));

                                    m_pCustomPlot->m_cv.notify_one();

                                    for (int i = 0; i < CHANNEL_NUM; ++i)
                                    {
                                        m_dataHandle[i].clear();
                                    }
                                }

                                //m_queDataHandle2.enqueue(m_dataHandle2);
                                //m_pCurrentView->m_cv.notify_one();
                            }
                        }
                        else
                        {
                            //printf("Pack incorrect. Not found FA FA\n");
                            LOGW("Pack incorrect. Not found FA FA");
                            indx += 32;
                        }
                    }
                    else
                    {
                        //printf("Pack incorrect. Not found AA FF\n");
                        LOGW("Pack incorrect. Not found AA FF");
                        break;
                    }
                }
                //printf("Datapack_end!!!\n");
            }
#endif

        }

#else

//#ifdef Q_OS_WIN32
//        std::filesystem::path chanpath("E:\\20211010_LAB256_5K_test_yx\\Raw");
//#else
//        std::filesystem::path chanpath("/home/cyclone_public/Documents/20211010_LAB256_5K_test_yx/Raw");
//#endif

        const std::string& mockdata = ConfigServer::GetInstance()->GetMockDataDemo();
        std::filesystem::path chanpath(mockdata);

        if (!std::filesystem::exists(chanpath))
        {
            LOGW("Mockdata not exist={}!", mockdata.c_str());
            return;
        }
        if (!std::filesystem::is_directory(chanpath))
        {
            LOGW("Mockdata not folder={}!", mockdata.c_str());
            return;
        }

        //int channun = -1;
        std::map<int, std::vector<std::vector<float>>> mapChanData;
        std::filesystem::directory_iterator dirite(chanpath);
        for (auto&& ite : dirite)
        {
            if (ite.status().type() == std::filesystem::file_type::directory) //is folder
            {
                std::string strsubdir = ite.path().filename().string();//ite.path().string();
                int chan = atoi(strsubdir.substr(7).c_str());
                int channun = chan - 1;
                for (int i = 0; i < CHANNEL_NUM; ++i)
                {
                    if (m_pRealChannel[i] == channun)
                    {
                        channun = i;
                        break;
                    }
                }

                std::filesystem::directory_iterator subdirite(ite.path().string());
                for (auto&& subite : subdirite)
                {
                    if (subite.status().type() == std::filesystem::file_type::regular)//is file
                    {
                        std::string subfile = subite.path().string();
                        FILE* subpf = fopen(subfile.c_str(), "rb");
                        const int cibuf = 20;
                        float arbuf[cibuf] = { 0.0 };
                        size_t rdsz = 0;
                        rdsz = fread(arbuf, 4, cibuf, subpf);
                        while (rdsz > 0)
                        {
                            mapChanData[channun].emplace_back(arbuf, arbuf + rdsz);

                            rdsz = fread(arbuf, 4, cibuf, subpf);
                        }

                        fclose(subpf);
                    }
                }
            }
        }

        int readLength = 576 * 20;// 512 * 16;
        unsigned char* pdatabuf = new unsigned char[readLength];
        while (m_bRunning)
        {
            {
                std::unique_lock<std::mutex> lck(m_mutexRecv);
                m_cvRecv.wait(lck, [this] {return !m_bRunning || !m_bSuspend; });
            }
            LOGI("Mock raw data begin....");
            if (mapChanData.size() > 0)
            {
                const auto& mapbeg = mapChanData.begin();
                for (int j = 0, n = mapbeg->second.size(); j < n && m_bRunning; ++j)
                {
                    {
                        std::unique_lock<std::mutex> lck(m_mutexRecv);
                        m_cvRecv.wait(lck, [this] {return !m_bRunning || !m_bSuspend; });
                    }
                    memset(pdatabuf, 0, readLength);
                    int inx = 0;
                    int i = 0;
                    for (int itm = 0, m = mapbeg->second[j].size(); itm < m && m_bRunning; ++itm)
                    {
                        for (; i < inx + 32; i += 2)
                        {
                            pdatabuf[i] = 0xAA;
                            pdatabuf[i + 1] = 0xFF;
                        }
                        inx = i;
                        for (int chan = 0; i < inx + 256 && chan < 128; i += 2, ++chan)
                        {
                            float cudt = 0.0;
                            auto iteFind = mapChanData.find(chan);
                            if (iteFind != mapChanData.end())
                            {
                                auto& arsz = iteFind->second;
                                if (arsz.size() > j)
                                {
                                    auto& ardt = arsz[j];
                                    if (ardt.size() >= m)
                                    {
                                        cudt = ardt[itm];
                                    }
                                }
                            }
                            int temp = 6553.6 * cudt / 500.0;
                            pdatabuf[i] = (temp & 0x00FF);
                            pdatabuf[i + 1] = ((temp >> 8) & 0x00FF);
                        }
                        inx = i;
                        for (; i < inx + 32; ++i)
                        {
                            pdatabuf[i] = 0xFA;
                        }
                        inx = i;
                        for (int chan = 128; i < inx + 256 && chan < 256; i += 2, ++chan)
                        {
                            float cudt = 0.0;
                            auto iteFind = mapChanData.find(chan);
                            if (iteFind != mapChanData.end())
                            {
                                auto& arsz = iteFind->second;
                                if (arsz.size() > j)
                                {
                                    auto& ardt = arsz[j];
                                    if (ardt.size() >= m)
                                    {
                                        cudt = ardt[itm];
                                    }
                                }
                            }
                            int temp = 6553.6 * cudt / 500.0;
                            pdatabuf[i] = (temp & 0x00FF);
                            pdatabuf[i + 1] = ((temp >> 8) & 0x00FF);
                        }
                        inx = i;
                    }
                    //if (ConfigServer::GetInstance()->GetPrintDemoData() != 0)
                    //{
                    //    int cntnum = 0;
                    //    for (int i = 0; i < readLength; ++i)
                    //    {
                    //        printf("%02X ", pdatabuf[i]);
                    //        ++cntnum;
                    //        if (cntnum >= 32)
                    //        {
                    //            printf("\n");
                    //            cntnum = 0;
                    //        }
                    //    }
                    //    printf("\n\n");
                    //}
                    //else
                    //{
                    //    Sleep(20);
                    //}

                    auto dtptr = std::make_shared<SDataPack>(pdatabuf, readLength);
                    m_queDataPack.enqueue(std::move(dtptr));
                    m_cvHandle.notify_one();

                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }

#endif

        delete[] pdatabuf;

        return;
    }

#else //if MOCKDATA_DEMO_TEST

    if (m_pUsbCyDevice == nullptr)
    {
        LOGE("get usbdev addr null");
        return;
    }
    auto pusbDevice = m_pUsbCyDevice->GetCurDeviceHandle();
    if (pusbDevice == nullptr)
    {
        LOGE("get usbdevhandle addr null");
        return;
    }

#ifdef Q_OS_WIN32
    CCyUSBEndPoint* epBulkOut = pusbDevice->EndPointOf(0x01);
    CCyUSBEndPoint* epBulkIn = pusbDevice->EndPointOf(0x81);

    if (epBulkOut == NULL || epBulkIn == NULL)
    {
        LOGE("get 01or81 addr null");
        return;
    }

    //FILE* pf = fopen("D:\\testorgdata.txt", "wb");
    //if (pf == nullptr)
    //{
    //    qDebug("Can't open testorgdata.");
    //    return;
    //}
    //long long filen = 0;

    const int QueueSize = 32;
    int totalbufsz = epBulkIn->MaxPktSize * 16;//576 * 14;//

    epBulkIn->SetXferSize(totalbufsz);
    OVERLAPPED ovlap[QueueSize] ;
    unsigned char** pdatabuf = new unsigned char*[QueueSize];
    
    unsigned char** pinputbuf = new unsigned char*[QueueSize];

    m_iHandleBuf = totalbufsz + 1024;
    m_pHandleBuf = new unsigned char[m_iHandleBuf];
    memset(m_pHandleBuf, 0, m_iHandleBuf);

    for (int i = 0; i < QueueSize; ++i)
    {
        ovlap[i].hEvent = CreateEvent(NULL, false, false, NULL);
        pdatabuf[i] = new unsigned char[totalbufsz];
        memset(pdatabuf[i], 0, totalbufsz);

        pinputbuf[i] = epBulkIn->BeginDataXfer(pdatabuf[i], totalbufsz, &ovlap[i]);
        if (epBulkIn->NtStatus || epBulkIn->UsbdStatus)
        {
            if (epBulkIn->UsbdStatus == USBD_STATUS_ENDPOINT_HALTED)
            {
                epBulkIn->Reset();
                epBulkIn->Abort();
                Sleep(50);

                pinputbuf[i] = epBulkIn->BeginDataXfer(pdatabuf[i], totalbufsz, &ovlap[i]);
            }
            if (epBulkIn->NtStatus || epBulkIn->UsbdStatus)
            {
                // BeginDataXfer failed// Handle the error now.
                epBulkIn->Abort();
                CloseHandle(ovlap[i].hEvent);

                //qDebug("Start_BeginDataXfer failed");
                LOGE("Start_BeginDataXfer failed");
                delete[] pinputbuf;
                delete[] pdatabuf;

                return;
            }
        }
    }

    long nCount = 0;
    long readLength = 0;
    int errcnt = 0;

    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    while (m_bRunning)
    {
        //QueryPerformanceCounter(&m_TimeContentStart);
        {
            std::unique_lock<std::mutex> lck(m_mutexRecv);
            m_cvRecv.wait(lck, [this] {return !m_bRunning || !m_bSuspend; });
        }
        if (!m_bRunning)
        {
            break;
        }

        if (!epBulkIn->WaitForXfer(&ovlap[nCount], TIMEOUT_PER_TRANSFER_MILLI_SEC))
        {
            //m_nFailure++;
            //epBulkIn->Abort();
            if (epBulkIn->LastError == ERROR_IO_PENDING)
            {
                WaitForSingleObject(ovlap[nCount].hEvent, TIMEOUT_PER_TRANSFER_MILLI_SEC);
            }
            //printf("FinishDataXfer_WaitForXfer failed\n");
            LOGE("FinishDataXfer_WaitForXfer failed");
            //break;
        }

        ////////////Read the trasnferred data from the device///////////////////////////////////////
        if (epBulkIn->FinishDataXfer(pdatabuf[nCount], readLength, &ovlap[nCount], pinputbuf[nCount]))
        {
            //if (filen <= 20 * 1024 * 1024)
            //{
            //    filen += fwrite(pdatabuf, 1, readLength, pf);
            //    fflush(pf);
            //}

#if 0

            if (ConfigServer::GetInstance()->GetPrintDemoData() != 0)
            {
                int cntnum = 0;
                for (int i = 0; i < readLength; ++i)
                {
                    printf("%02X ", pdatabuf[nCount][i]);
                    ++cntnum;
                    if (cntnum >= 32)
                    {
                        printf("\n");
                        cntnum = 0;
                    }
                }
                printf("\n\n");
            }

#endif

#ifdef USE_EACHTHREADRECVANDHANDLE_DATA

            auto dtptr = std::make_shared<SDataPack>(pdatabuf[nCount], readLength);
            m_queDataPack.enqueue(std::move(dtptr));
            readLength = 0;

            m_cvHandle.notify_one();

#else

            {
                auto& dp = pdatabuf[nCount];// dtpk->dataPack;
                int dtsz = readLength;// dp.size();
                auto& tableData = m_pCalibrationDlg->GetTableData();

                //printf("Datapack_begin!!!=%d\n", dtsz);
                int indx = 0;
                int dtend = dtsz - 576;
                while (m_bRunning && indx < dtend)
                {
                    bool bfound = false;
                    for (int i = indx; i < dtend; ++i)
                    {
                        int item = FindStartFlag(&dp[i], 0xAA, 0xFF);
                        if (item != -1)
                        {
                            indx = item + i;
                            bfound = true;
                            break;
                        }
                    }

                    if (bfound)
                    {
                        if (indx >= dtsz - 544)
                        {
                            break;
                        }
                        for (int i = indx + 256, n = indx + 288; i < n; ++i)
                        {
                            if (dp[i] != 0xFA)
                            {
                                bfound = false;
                                break;
                            }
                        }
                        if (bfound)
                        {
                            if (indx < dtsz - 544 - 576)
                            {
                                for (int i = indx + 544, n = indx + 576; i < n; i += 2)
                                {
                                    if (dp[i] != 0xAA || dp[i + 1] != 0xFF)
                                    {
                                        bfound = false;
                                        break;
                                    }
                                }
                                if (!bfound)
                                {
                                    //printf("Pack incorrect. Not found AA FF2\n");
                                    LOGW("Pack incorrect. Not found AA FF2");
                                    indx += 32;
                                }
                            }
                            if (bfound)
                            {
                                for (int i = 0; i < 128; ++i, indx += 2)
                                {
                                    SetDataHandle(i, dp[indx + 1], dp[indx], tableData[i]);
                                }

                                indx += 32;
                                for (int i = 128; i < 256; ++i, indx += 2)
                                {
                                    SetDataHandle(i, dp[indx + 1], dp[indx], tableData[i]);
                                }

                                if (m_dataHandle[0].size() >= 200)
                                {
                                    auto dtptr = std::make_shared<CustomPlotView::SDataHandle>(m_dataHandle);
                                    m_pCustomPlot->m_queDataHandle.enqueue(std::move(dtptr));

                                    m_pCustomPlot->m_cv.notify_one();

                                    for (int i = 0; i < CHANNEL_NUM; ++i)
                                    {
                                        m_dataHandle[i].clear();
                                    }
                                }

                                //m_queDataHandle2.enqueue(m_dataHandle2);
                                //m_pCurrentView->m_cv.notify_one();
                            }
                        }
                        else
                        {
                            //printf("Pack incorrect. Not found FA FA\n");
                            LOGW("Pack incorrect. Not found FA FA");
                            indx += 32;
                        }
                    }
                    else
                    {
                        //printf("Pack incorrect. Not found AA FF\n");
                        LOGW("Pack incorrect. Not found AA FF");
                        break;
                    }
                }
                //printf("Datapack_end!!!\n");
            }
#endif
        }
        else
        {
            //printf("FinishDataXfer_FinishDataXfer failed!!!=%d\n", nCount);
            LOGW("FinishDataXfer_FinishDataXfer failed!!!={}", nCount);
            if (++errcnt > 10)
            {
                //printf("Too much FinishDataXfer_FinishDataXfer failed will exit....=%d\n", nCount);
                LOGE("Too much FinishDataXfer_FinishDataXfer failed!!!....={}", errcnt);
                //emit EndStopThreadSignal();
                //break;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if (errcnt % 59 == 0)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(8));
                }
            }
        }

        //Re-submit this queue element to keep the queue full
        pinputbuf[nCount] = epBulkIn->BeginDataXfer(pdatabuf[nCount], totalbufsz, &ovlap[nCount]);
        if (epBulkIn->NtStatus || epBulkIn->UsbdStatus)
        {
            // BeginDataXfer failed............// Time to bail out now............
            //epBulkIn->Abort();
            //CloseHandle(ovlap[nCount].hEvent);
            //delete[] pinputbuf;
            //delete[] pdatabuf;
            //qDebug("FinishDataXfer_BeginDataXfer failed\n");
            LOGE("FinishDataXfer_BeginDataXfer failed failed");
            //m_nFailure++;
            //break;
        }

        if (++nCount >= QueueSize) nCount = 0;
    }
    //fclose(pf);

    delete[] pinputbuf;
    for (int i = 0; i < QueueSize; ++i)
    {
        delete[] pdatabuf[i];
        pdatabuf[i] = nullptr;
    }
    delete[] pdatabuf;
    pdatabuf = nullptr;

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

#else //ifdef Q_OS_WIN32

    const int QueueSize = 32;
    int totalbufsz = (16 << 10);//576 * 14;//

    unsigned char** pdatabuf = new unsigned char * [QueueSize];

    int ret = 0;
    long nCount = 0;
    int readLength = 0;
    int errcnt = 0;

    for (int i = 0; i < QueueSize; ++i)
    {
        pdatabuf[i] = new unsigned char[totalbufsz];
        memset(pdatabuf[i], 0, totalbufsz);
    }

    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    while (m_bRunning)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexRecv);
            m_cvRecv.wait(lck, [this] {return !m_bRunning || !m_bSuspend; });
        }
        if (!m_bRunning)
        {
            break;
        }

        ret = m_pUsbCyDevice->BulkDataTransfer(pdatabuf[nCount], totalbufsz, &readLength, 5000);
        if (ret == 0)
        {

#if 0

            if (ConfigServer::GetInstance()->GetPrintDemoData() != 0)
            {
                int cntnum = 0;
                for (int i = 0; i < readLength; ++i)
                {
                    printf("%02X ", pdatabuf[nCount][i]);
                    ++cntnum;
                    if (cntnum >= 32)
                    {
                        printf("\n");
                        cntnum = 0;
                    }
                }
                printf("\n\n");
            }

#endif


            auto dtptr = std::make_shared<SDataPack>(pdatabuf[nCount], readLength);
            m_queDataPack.enqueue(std::move(dtptr));
            readLength = 0;

            m_cvHandle.notify_one();
        }
        else
        {
            //printf("FinishDataXfer_FinishDataXfer failed!!!=%d\n", nCount);
            LOGW("FinishDataXfer_FinishDataXfer failed!!!={}", nCount);
            if (++errcnt > 10)
            {
                //printf("Too much FinishDataXfer_FinishDataXfer failed will exit....=%d\n", nCount);
                LOGE("Too much FinishDataXfer_FinishDataXfer failed!!!....={}", errcnt);
                //emit EndStopThreadSignal();
                //break;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if (errcnt % 59 == 0)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(8));
                }
            }
        }

        if (++nCount >= QueueSize) nCount = 0;
    }

    for (int i = 0; i < QueueSize; ++i)
    {
        delete[] pdatabuf[i];
        pdatabuf[i] = nullptr;
    }
    delete[] pdatabuf;
    pdatabuf = nullptr;

    LOGI("ThreadExit={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

#endif //ifdef Q_OS_WIN32

#endif //if MOCKDATA_DEMO_TEST
}

int UsbPortBulk::FindStartFlag(unsigned char* psrc, unsigned char a, unsigned char b)
{
    int beg = 0;
    for (beg = 0; beg < 32; beg += 2)
    {
        if (psrc[beg] != a || psrc[beg + 1] != b)
        {
            return -1;
        }
    }
    return beg;
}

inline bool UsbPortBulk::SetDataHandle(int i, unsigned char high, unsigned char low, const std::vector<std::tuple<double, double, double>>* ptabdt)
{
    i = m_pRealChannel[i];
    float off = std::get<2>((*ptabdt)[i]);
    float slop = std::get<1>((*ptabdt)[i]);
    short dtab = (short)(high << 8) + low;
    float val = (UsbProtoMsg::GetHexCurrent(dtab) * 1000.0 - off) / slop - m_pControlView->m_arZeroAdjustOffset[i];

    m_dataHandle[i].emplace_back(val);

    return true;
}

inline bool UsbPortBulk::SetDataHandleCalibration(int i, unsigned char high, unsigned char low)
{
    i = m_pRealChannel[i];
    short dtab = (short)(high << 8) + low;
    float val = UsbProtoMsg::GetHexCurrent(dtab) * 1000.0;

    m_dataHandle[i].emplace_back(val);

    return true;
}

inline bool UsbPortBulk::SetDataHandleZeroAdjust(int i, unsigned char high, unsigned char low, const std::vector<std::tuple<double, double, double>>* ptabdt)
{
    i = m_pRealChannel[i];
    float off = std::get<2>((*ptabdt)[i]);
    float slop = std::get<1>((*ptabdt)[i]);
    short dtab = (short)(high << 8) + low;
    float val = (UsbProtoMsg::GetHexCurrent(dtab) * 1000.0 - off) / slop - m_pControlView->GetZeroAdjustOffset(i);

    m_dataHandle[i].emplace_back(val);

    return true;
}

#ifdef USE_EACHTHREADRECVANDHANDLE_DATA

void UsbPortBulk::ThreadHandleFunc(void)
{
    LOGI("ThreadStart={}!!!={}", __FUNCTION__, ConfigServer::GetCurrentThreadSelf());

    //ConfigServer::GetInstance()->SetRawDataLength(0);
    std::vector<unsigned char> vetUnhandleBuf;
    std::vector<unsigned char> vetHandleBuf;

    while (m_bRunningHnadle)
    {
        {
            std::unique_lock<std::mutex> lck(m_mutexHandle);
            m_cvHandle.wait(lck, [this] {return !m_bRunningHnadle || m_queDataPack.size_approx() > 0; });
        }
        //int quepacksz = m_queDataPack.size_approx();
        std::shared_ptr<SDataPack> dtpk;
        bool bret = m_queDataPack.try_dequeue(dtpk);
        if (bret)
        {
            auto& tableData = m_pCalibrationDlg->GetTableData();
#if 0
            auto& dp = dtpk->dataPack;
            int dtsz = dp.size();
#else
#if 0
            memset(m_pHandleBuf, 0, m_iHandleBuf);
            int iunsz = strlen((const char*)m_arUnhandleBuf);
            if (iunsz > 0)
            {
                memcpy(m_pHandleBuf, m_arUnhandleBuf, iunsz);
            }
            int dpsz = dtpk->dataPack.size();
            memcpy(m_pHandleBuf + iunsz, &dtpk->dataPack[0], dpsz);

            auto& dp = m_pHandleBuf;
            int dtsz = iunsz + dpsz;
#else
            vetHandleBuf.clear();
            int dtsz = 0;
            int dpsz = dtpk->dataPack.size();
            int iunsz = vetUnhandleBuf.size();
            //if (iunsz > 0)
            //{
                dtsz = iunsz + dpsz;
                vetHandleBuf.resize(dtsz);
                memcpy(&vetHandleBuf[0], vetUnhandleBuf.data(), iunsz);
                memcpy(&vetHandleBuf[iunsz], &dtpk->dataPack[0], dpsz);
            //}
            //else
            //{
            //    dtsz = dpsz;
            //    vetHandleBuf.resize(dtsz);
            //    memcpy(&vetHandleBuf[0], &dtpk->dataPack[0], dpsz);
            //}
            auto& dp = vetHandleBuf;
#endif
#endif

            //printf("Datapack_begin!!!=%d\n", dtsz);
            int indx = 0;
            int dtend = dtsz - 576;
            while (m_bRunningHnadle && indx < dtend)
            {
                bool bfound = false;
                for (int i = indx; i < dtend; ++i)
                {
                    int item = FindStartFlag(&dp[i], 0xAA, 0xFF);
                    if (item != -1)
                    {
                        indx = item + i;
                        bfound = true;
                        break;
                    }
                }

                if (bfound)
                {
                    if (indx >= dtsz - 544)
                    {
                        break;
                    }
                    for (int i = indx + 256, n = indx + 288; i < n; ++i)
                    {
                        if (dp[i] != 0xFA)
                        {
                            bfound = false;
                            break;
                        }
                    }
                    if (bfound)
                    {
                        if (indx < dtend - 544)
                        {
                            for (int i = indx + 544, n = indx + 576; i < n; i += 2)
                            {
                                if (dp[i] != 0xAA || dp[i + 1] != 0xFF)
                                {
                                    bfound = false;
                                    break;
                                }
                            }
                            if (!bfound)
                            {
                                //printf("Pack incorrect. Not found AA FF2\n");
                                LOGW("Pack incorrect. Not found AA FF2");
                                indx += 32;
                            }
                        }
                        if (bfound)
                        {
                            if (ConfigServer::GetInstance()->GetZeroAdjust())
                            {
                                for (int i = 0; i < 128; ++i, indx += 2)
                                {
                                    SetDataHandleZeroAdjust(i, dp[indx + 1], dp[indx], &tableData);
                                }

                                indx += 32;
                                for (int i = 128; i < 256; ++i, indx += 2)
                                {
                                    SetDataHandleZeroAdjust(i, dp[indx + 1], dp[indx], &tableData);
                                }
                            }
                            else if (ConfigServer::GetInstance()->GetCalibration())
                            {
                                for (int i = 0; i < 128; ++i, indx += 2)
                                {
                                    SetDataHandleCalibration(i, dp[indx + 1], dp[indx]);
                                }

                                indx += 32;
                                for (int i = 128; i < 256; ++i, indx += 2)
                                {
                                    SetDataHandleCalibration(i, dp[indx + 1], dp[indx]);
                                }
                            }
                            else
                            {
                                for (int i = 0; i < 128; ++i, indx += 2)
                                {
                                    SetDataHandle(i, dp[indx + 1], dp[indx], &tableData);
                                }

                                indx += 32;
                                for (int i = 128; i < 256; ++i, indx += 2)
                                {
                                    SetDataHandle(i, dp[indx + 1], dp[indx], &tableData);
                                }
                            }

                            if (m_dataHandle[0].size() >= 200)
                            {
                                auto dtptr = std::make_shared<ConfigServer::SDataHandle>(m_dataHandle);
                                m_pCustomPlot->m_queDataHandle.enqueue(std::move(dtptr));
                                m_pCustomPlot->m_cv.notify_one();

                                if (ConfigServer::GetInstance()->GetSaveData())
                                {
                                    //ConfigServer::GetInstance()->AddRawDataLength(m_dataHandle[0].size());

                                    auto dtsaveptr = std::make_shared<ConfigServer::SDataHandle>(m_dataHandle);
                                    m_pSavePane->m_queDataHandle.enqueue(std::move(dtsaveptr));
                                    m_pSavePane->m_cv.notify_one();

                                    if (ConfigServer::GetInstance()->GetRealtimeSequence())
                                    {
                                        auto dtrtseqptr = std::make_shared<ConfigServer::SDataHandle>(m_dataHandle);
                                        m_pSavePane->m_queDataHandle2.enqueue(std::move(dtrtseqptr));
                                        m_pSavePane->m_cv2.notify_one();
                                    }
                                }

                                if (ConfigServer::GetInstance()->GetCollectCap())
                                {
                                    auto dtscapptr = std::make_shared<ConfigServer::SDataHandle>(m_dataHandle);
                                    m_pCollectCap->m_queDataHandle.enqueue(std::move(dtscapptr));
                                    m_pCollectCap->m_cv.notify_one();
                                }

                                {
                                    auto dtctrlptr = std::make_shared<ConfigServer::SDataHandle>(m_dataHandle);
                                    m_pControlView->m_queDataHandle.enqueue(std::move(dtctrlptr));
                                    m_pControlView->m_cv.notify_one();
                                }

                                for (int i = 0; i < CHANNEL_NUM; ++i)
                                {
                                    m_dataHandle[i].clear();
                                }
                            }
                        }
                    }
                    else
                    {
                        //printf("Pack incorrect. Not found FA FA\n");
                        LOGW("Pack incorrect. Not found FA FA");
                        indx += 32;
                    }
                }
                else
                {
                    //printf("Pack incorrect. Not found AA FF\n");
                    LOGW("Pack incorrect. Not found AA FF");
                    break;
                }
            }
#if 0
#else
#if 0
            memset(m_arUnhandleBuf, 0, 1024);
            iunsz = dtsz - indx;
            iunsz = iunsz < 1024 ? iunsz : 1024;
            if (iunsz > 0)
            {
                memcpy(m_arUnhandleBuf, &dp[dtsz - iunsz], iunsz);
            }
#else
            vetUnhandleBuf.clear();
            iunsz = dtsz - indx;
            if (iunsz > 0)
            {
                vetUnhandleBuf.resize(iunsz);
                memcpy(vetUnhandleBuf.data(), &dp[indx], iunsz);
            }
#endif
#endif
            //printf("Datapack_end!!!\n");
        }
    }
    LOGI("ThreadExit={}!Left={}!={}", __FUNCTION__, m_queDataPack.size_approx(), ConfigServer::GetCurrentThreadSelf());

    std::shared_ptr<SDataPack> dtclear;
    while (m_queDataPack.try_dequeue(dtclear));
}
#endif

bool UsbPortBulk::EndThread(bool all)
{
#ifdef USE_EACHTHREADRECVANDHANDLE_DATA
    if (all)
    {
        m_bRunningHnadle = false;
        m_cvHandle.notify_one();
    }
#endif

    m_bRunning = false;
    m_cvRecv.notify_one();

    return false;
}

bool UsbPortBulk::StopThread(bool all)
{
    m_bRunning = false;
    m_cvRecv.notify_one();

    if (m_ThreadPtr.get() != nullptr)
    {
        if (m_ThreadPtr->joinable())
        {
            m_ThreadPtr->join();
        }
        m_ThreadPtr.reset();
    }

#ifdef USE_EACHTHREADRECVANDHANDLE_DATA
    if (all)
    {
        m_bRunningHnadle = false;
        m_cvHandle.notify_one();

        if (m_ThreadHandlePtr.get() != nullptr)
        {
            if (m_ThreadHandlePtr->joinable())
            {
                m_ThreadHandlePtr->join();
            }
            m_ThreadHandlePtr.reset();
        }
    }
#endif

    return true;
}
