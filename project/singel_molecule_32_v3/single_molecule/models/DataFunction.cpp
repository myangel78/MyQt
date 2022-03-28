#include "DataFunction.h"
#include "models/DataDefine.h"
#include "models/DatacCommunicaor.h"
#include "function/clustering.h"
#include "function/DBSCAN.h"
#include"logger/Log.h"
#include <iostream>
using std::vector;
using std::cout;

DataFunction::DataFunction()
{
     QueryPerformanceFrequency(&m_frequency);
     qRegisterMetaType<QVariant>("DataFunction::QVariant");

     m_duration_thres = 0.5;
     m_gating_std = 5;
     m_gating_min = 40;
     m_gating_max = 130;
}

DataFunction::~DataFunction()
{
    stopThread();
}

//bool isGating(vector<int> &label, vector<vector<double>> &cluster_stats,
//              double sampling_rate=20000, double duration_thres=0.5,
//              int signal_min=20, int signal_max=150,
//              int gating_min=40, int gating_max=130, int hard_min=10, int gating_std=6,
//              int openpore_thres=170)
bool DataFunction::isGating(vector<int> &label, vector<vector<double>> &cluster_stats,
              double sampling_rate, double duration_thres,
              int signal_min, int signal_max,
              int gating_min, int gating_max, int hard_min, int gating_std,
              int openpore_thres)
{
    // pre-check, whether current channel is sequencing
    double signal_mean(0);
    for (int i=0; i<cluster_stats.size(); i++)
    {
        signal_mean += cluster_stats[i][1];
    }
    signal_mean = signal_mean/cluster_stats.size();
    // exclude signal if signal mean < min or signal mean > max
    // int exclude_min(20), exclude_max(150);
    if ((signal_mean<signal_min) || (signal_mean>signal_max)) { return false; }


    // int gating_min(40), gating_max(130), hard_min(10), gating_std(6);
    vector<int> queue;
    for (int i=0; i<cluster_stats.size(); i++)
    {
        // mean and std condition
        //if ((cluster_stats[i][1] < gating_max && cluster_stats[i][1] > gating_min
        //     && cluster_stats[i][2] < gating_std) || cluster_stats[i][1] < hard_min)
        if ((cluster_stats[i][1] < gating_max && cluster_stats[i][1] > gating_min
            && cluster_stats[i][2] < gating_std))
        {
            queue.push_back(cluster_stats[i][0]);
        }
    }
    if (queue.empty()) { return false; }

    vector<int>::const_iterator beg;
    beg = queue.begin();
    int cluster_len(0);
    double length_thres(sampling_rate*duration_thres);
    // int openpore_thres(170);
    int tmp(0);

    for (beg = queue.begin(); beg<queue.end(); beg++)
    {
        for (int i=0; i<label.size(); i++)
        {
            if (label[i] == *beg)
            {
                // Find connected region with the same label as in queue
                for (int j=i; j<label.size(); j++)
                {
                    tmp = j;
                    if (label[j] != *beg)
                    {
                        break;
                    }
                    cluster_len++;
                }
                // determine if current cluster is gating
                if (cluster_len > length_thres)
                {
                    if (i==0) { return true; }

                    for (int k=0; k<cluster_stats.size(); k++)
                    {
                        if ((cluster_stats[k][0]==label[i-1]) &&
                            (cluster_stats[k][1]>openpore_thres))
                        {
                            return true;
                        }
                    }
                }
                // reset and continue to next cluster
                cluster_len = 0;
                i = tmp;
            }
        }
    }
    return false;
}


void DataFunction::setDegatingParameterSlot(double duration_thres,int gating_std, int signal_min, int signal_max, int gating_max,int gating_min)
{
    LOGI("duration_thres: {}  gating_std: {} signal_min: {} signal_max: {} gating_max: {} gating_min: {}",duration_thres,gating_std,signal_min,signal_max,gating_max,gating_min);
    m_duration_thres = duration_thres;
    m_gating_std = gating_std;
    m_gating_min = gating_min;
    m_gating_max = gating_max;
    m_gating_sigMin = signal_min;
    m_gating_sigMax = signal_max;
}

void DataFunction::AnalyDegatingChannel(QVector<QVector<float>> &vctData)
{
    try
    {
        if(!m_bRunThread)
            QThread::currentThread()->quit();
        QVector<bool> nbDegateResult;
        nbDegateResult.resize(CHANNEL_NUM);
        QVariant nReasult;
        for(int ch = 0 ; ch < vctData.size(); ch++)
        {
            std::vector<float> buffer = vctData.at(ch).toStdVector();
            vector<int> label(buffer.size());
            int eps_p(50);
            int eps_c(10);
            int s_min_pts(10);
            int eps_d(10);
            int d_min_pts(1);
            bool pre_flag(true);

            vector<vector<double>> cluster_stats =
                LinearDBSCAN(buffer, label,
                             eps_p, eps_c, s_min_pts,
                             eps_d, d_min_pts,
                             pre_flag);

    //       nbDegateResult[ch] = isGating(label, cluster_stats);
             nbDegateResult[ch] = isGating(label, cluster_stats, 20000, m_duration_thres, m_gating_sigMin, m_gating_sigMax, m_gating_min, m_gating_max, 10, m_gating_std, 170);
        }
    //    qDebug()<<nbDegateResult;
        nReasult.setValue(nbDegateResult);
        emit DegateResultSig(nReasult);

    }
    catch (std::exception &e)
    {
        LOGI("FUNC: AnalyDegatingChannel {}",e.what());
    }
}


void DataFunction::stopThread()
{
    qDebug()<<"DataFunction stopThread";
    m_bRunThread = false;
}

void DataFunction::startThread()
{
//    qDebug()<<"DataFunction startThread";
    m_bRunThread = true;
}
