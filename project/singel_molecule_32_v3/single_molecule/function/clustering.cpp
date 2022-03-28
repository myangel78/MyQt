#include <vector>
#include <numeric>
#include <stdlib.h>
#include <math.h>

#include "DBSCAN.h"

using namespace std;


double CustomMean(vector<float> &data, vector<int> &pos)
{
    double sum = 0.0;
    for (int i=0; i<pos.size(); i++)
    {
        sum += data[pos[i]];
    }
    return sum/pos.size();
}

void CustomMeanAndStd(vector<float> &data, vector<int> &pos, double &mean, double &stdev)
{
    double sum(0.0), var(0.0);
    for (int i=0; i<pos.size(); i++)
    {
        sum += data[pos[i]];
    }
    mean = sum/pos.size();

    for (int i=0; i<pos.size(); i++)
    {
        var += (data[pos[i]] - mean)*(data[pos[i]] - mean);
    }
    stdev = sqrt(var/pos.size());
}

void GetPosition(vector<int> &label, int lbl, vector<int> &pos)
{
    pos.clear();
    for (int i=0; i<label.size(); i++)
    {
        if (label[i] == lbl)
        {
            pos.push_back(i);
        }
    }
}

// find unique elements from input label
void GetUnique(vector<int> &label, vector<int> &unique_label)
{
    unique_label.clear();
    int lbl1, lbl2;
    bool exist_flag(false);
    unique_label.push_back(label[0]);
    for (int i=0; i<label.size()-1; i++)
    {
        lbl1 = label[i];
        lbl2 = label[i+1];
        if (lbl2-lbl1 != 0)
        {
            for (int j=0; j<unique_label.size(); j++)
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
void PreClustering(vector<float> &data, vector<int> &label, int eps_c=10)
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
        for (int i=0; i<label.size()-1; i++)
        {
            lbl1 = old_label[i];
            lbl2 = old_label[i+1];
            // if 
            if (lbl2-lbl1 != 0)
            {
                GetPosition(old_label, lbl1, pos);
                mu1 = CustomMean(data, pos);
                GetPosition(old_label, lbl2, pos);
                mu2 = CustomMean(data, pos);
                if (abs(mu1-mu2) < eps_c)
                {
                    for (int i=0; i<new_label.size(); i++)
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

void GetClusterStats(vector<float> &data, vector<int> &label, vector<vector<double>> &stats)
{   
    vector<int> unique_label;
    GetUnique(label, unique_label);

    stats.clear();
    vector<double> tmp;
    vector<int> pos;
    double mean(0.0), stdev(0.0);
    for (int i=0; i<unique_label.size(); i++)
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

void LinearSearch(vector<float> &data, 
                   vector<int> &label, 
                   vector<vector<double>> &cluster_stats, 
                   int eps_p=50, 
                   int eps_c=10, 
                   int min_pts=1, 
                   bool pre_flag=true)
{
    int cluster = 1;
    int count = 0;
    for (int i=0; i<data.size(); i++)
    {
        if (i==0)
        {
            label[i] = cluster;
            count += 1;
            continue;
        }

        if (abs(data[i]-data[i-1]) < eps_p)
        {
            label[i] = label[i-1];
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

void CleanLabel(vector<int> &label, 
                vector<vector<double>> &cluster_stats, 
                vector<int> &cluster_label)
{   
    // vector<int> new_label(label.size())
    for (int i=0; i<label.size(); i++)
    {
        for (int j=0; j<cluster_label.size(); j++)
        {   
            if (label[i] == cluster_stats[j][0])
            {
                label[i] = cluster_label[j];
                break;
            }
        }
    }
}

vector<vector<double>> LinearDBSCAN(vector<float> &data, 
                  vector<int> &label, 
                  int eps_p=50, 
                  int eps_c=10, 
                  int s_min_pts=1, 
                  int eps_d=10,
                  int d_min_pts=1,
                  bool pre_flag=true)
{
    vector<vector<double>> cluster_stats;
    // Call linear search
    LinearSearch(data, label, cluster_stats, 
                 eps_p, eps_c, s_min_pts, pre_flag);
    

    // call DBSCAN
    DBSCAN2D db(cluster_stats, eps_d, d_min_pts);
    vector<int> cluster_label = db.Fit();

    // relabel
    CleanLabel(label, cluster_stats, cluster_label);
    // output cluster stats
    GetClusterStats(data, label, cluster_stats);
    return cluster_stats;
}

