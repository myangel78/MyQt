#include "DataBase.h"
#include<thread>
#include"models/DatacCommunicaor.h"

#define g_pCommunicaor DataCommunicaor::Instance()
using namespace std;

DataBase::DataBase()
{
}

DataBase::~DataBase()
{
    stopThread();
}

void DataBase::startThread()
{
    if(!m_bRunThread)
    {
        m_bRunThread = true;
        std::thread thrd(std::bind(&DataBase::threadFunc,this));
        thrd.detach();
    }else
    {
        g_pCommunicaor->OnExecuteStatusNotify(MAINWINDOW,LAST_THREAD_NOT_STOP);
    }
}

void DataBase::stopThread()
{
    m_bRunThread = false;
}

void DataBase::threadFunc()
{
    while(m_bRunThread)
    {
        this_thread::sleep_for(std::chrono::seconds(1));
    };
}

