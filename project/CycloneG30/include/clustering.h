#pragma once

#include <vector>
#include <numeric>
#include <stdlib.h>
#include <math.h>

#include "DBSCAN.h"

using namespace std;


static double CustomMean(const vector<float> &data, const vector<int> &pos)
{
    double sum = 0.0;
    for (int i = 0; i < pos.size(); i++)
    {
        sum += data[pos[i]];
    }
    return sum / pos.size();
}

static void CustomMeanAndStd(const vector<float> &data, const vector<int> &pos, double &mean, double &stdev)
{
    double sum(0.0), var(0.0);
    for (int i = 0; i < pos.size(); i++)
    {
        sum += data[pos[i]];
    }
    mean = sum / pos.size();

    for (int i = 0; i < pos.size(); i++)
    {
        var += (data[pos[i]] - mean) * (data[pos[i]] - mean);
    }
    stdev = sqrt(var / pos.size());
}

static void GetPosition(const vector<int>& label, int lbl, vector<int>& pos)
{
    pos.clear();
    for (int i = 0; i < label.size(); i++)
    {
        if (label[i] == lbl)
        {
            pos.push_back(i);
        }
    }
}

// find unique elements from input label
static void GetUnique(const vector<int> &label, vector<int> &unique_label)
{
    unique_label.clear();
    int lbl1, lbl2;
    bool exist_flag(false);
    unique_label.push_back(label[0]);
    for (int i = 0; i < label.size() - 1; i++)
    {
        lbl1 = label[i];
        lbl2 = label[i + 1];
        if (lbl2 - lbl1 != 0)
        {
            for (int j = 0; j < unique_label.size(); j++)
            {
                if (unique_label[j] == lbl2)
                {
                    exist_flag = true;
                    break;
                }
            }
            if (!exist_flag)
            {
                unique_label.push_back(lbl2);
            }
            else
            {
                exist_flag = false;
            }
        }
    }
}

/* Pre-clustering, combine neighboring clusters if
 their absolute difference of mean values is less than eps_c. */
static void PreClustering(const vector<float> &data, vector<int> &label, int eps_c = 10)
{
    vector<int> old_label(label);
    vector<int> new_label(label);

    // temporary params
    vector<int> pos;
    double mu1, mu2;
    int lbl1, lbl2;

    while (true)
    {
        // get location where adjacent points are from difference clusters
        for (int i = 0; i < (int)(label.size() - 1); i++)
        {
            lbl1 = old_label[i];
            lbl2 = old_label[i + 1];
            // if 
            if (lbl2 - lbl1 != 0)
            {
                GetPosition(old_label, lbl1, pos);
                mu1 = CustomMean(data, pos);
                GetPosition(old_label, lbl2, pos);
                mu2 = CustomMean(data, pos);
                if (abs(mu1 - mu2) < eps_c)
                {
                    for (int i = 0; i < new_label.size(); i++)
                    {
                        if (new_label[i] == lbl2)
                        {
                            new_label[i] = lbl1;
                        }
                    }
                }
            }
        }

        // compare old and new labels
        if (old_label == new_label)
        {
            break;
        }
        else
        {
            old_label = new_label;
        }
    }
    label = new_label;
}

static void GetClusterStats(const vector<float> &data, const vector<int> &label, vector<vector<double>> &stats)
{
    vector<int> unique_label;
    GetUnique(label, unique_label);

    stats.clear();
    vector<double> tmp;
    vector<int> pos;
    double mean(0.0), stdev(0.0);
    for (int i = 0; i < unique_label.size(); i++)
    {
        tmp.clear();
        GetPosition(label, unique_label[i], pos);
        CustomMeanAndStd(data, pos, mean, stdev);
        tmp.push_back(unique_label[i]);
        tmp.push_back(mean);
        tmp.push_back(stdev);
        stats.push_back(tmp);
    }
}

static void CleanLabel(vector<int> &label,
                const vector<vector<double>> &cluster_stats, 
                const vector<int> &cluster_label)
{
    // vector<int> new_label(label.size())
    for (int i = 0; i < label.size(); i++)
    {
        for (int j = 0; j < cluster_label.size(); j++)
        {
            if (label[i] == cluster_stats[j][0])
            {
                label[i] = cluster_label[j];
                break;
            }
        }
    }
}

static void LinearSearch(const vector<float> &data,
                  vector<int> &label, 
                  vector<vector<double>> &cluster_stats,
                  int eps_p = 50,
                  int eps_c = 10,
                  int min_pts = 1,
                  bool pre_flag = true)
{
    int cluster = 1;
    int count = 0;
    for (int i = 0; i < data.size(); i++)
    {
        if (i == 0)
        {
            label[i] = cluster;
            count += 1;
            continue;
        }

        if (abs(data[i] - data[i - 1]) < eps_p)
        {
            label[i] = label[i - 1];
            count += 1;
        }
        else
        {
            if (count > min_pts)
            {
                cluster += 1;
                count = 0;
            }
            label[i] = cluster;
        }
    }

    if (pre_flag)
    {
        PreClustering(data, label, eps_c);
    }

    GetClusterStats(data, label, cluster_stats);

}

static void LinearDBSCAN(vector<vector<double>>& vetout,
    const vector<float>& data,
    vector<int>& label,
    int eps_p = 50,
    int eps_c = 10,
    int s_min_pts = 1,
    int eps_d = 10,
    int d_min_pts = 1,
    bool pre_flag = true)
{
    // Call linear search
    LinearSearch(data, label, vetout, eps_p, eps_c, s_min_pts, pre_flag);

    // call DBSCAN
    DBSCAN2D db(vetout, eps_d, d_min_pts);
    vector<int> cluster_label = db.Fit();

    // relabel
    CleanLabel(label, vetout, cluster_label);

    // output cluster stats
    GetClusterStats(data, label, vetout);
}

bool isGating(vector<int>& label, const vector<vector<double>>& cluster_stats,
    double sampling_rate, double duration_thres,
    int signal_min, int signal_max,
    int gating_min, int gating_max, int hard_min, int gating_std, int gatingstdmax,
    int openpore_thres)
{
    // pre-check, whether current channel is sequencing
    double signal_mean(0);
    for (int i = 0; i < cluster_stats.size(); i++)
    {
        signal_mean += cluster_stats[i][1];
    }
    signal_mean = signal_mean / cluster_stats.size();
    // exclude signal if signal mean < min or signal mean > max
    // int exclude_min(20), exclude_max(150);
    //if ((signal_mean<signal_min) || (signal_mean>signal_max)) { return false; }
    if (cluster_stats[0][1] < signal_min) { return false; }
    if (signal_mean < signal_min) { return true; }  //为了检验0电流时判断为gating状态
    if (signal_mean > signal_max) { return false; }


    // int gating_min(40), gating_max(130), hard_min(10), gating_std(6);
    vector<int> queue;
    for (int i = 0; i < cluster_stats.size(); i++)
    {
        // mean and std condition
        //if ((cluster_stats[i][1] < gating_max && cluster_stats[i][1] > gating_min
        //     && cluster_stats[i][2] < gating_std) || cluster_stats[i][1] < hard_min)
        if ((cluster_stats[i][1] < gating_max && cluster_stats[i][1] > gating_min
            && (cluster_stats[i][2] < gating_std || cluster_stats[i][2] > gatingstdmax)))
        {
            queue.push_back(cluster_stats[i][0]);
        }
    }
    if (queue.empty()) { return false; }

    vector<int>::const_iterator beg;
    beg = queue.begin();
    int cluster_len(0);
    double length_thres(sampling_rate * duration_thres);
    // int openpore_thres(170);
    int tmp(0);

    for (beg = queue.begin(); beg < queue.end(); beg++)
    {
        for (int i = 0; i < label.size(); i++)
        {
            if (label[i] == *beg)
            {
                // Find connected region with the same label as in queue
                for (int j = i; j < label.size(); j++)
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
                    if (i == 0) { return true; }

                    for (int k = 0; k < cluster_stats.size(); k++)
                    {
                        if ((cluster_stats[k][0] == label[i - 1]) &&
                            (cluster_stats[k][1] > openpore_thres))
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
