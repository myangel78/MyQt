#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <math.h>
#include <chrono>
// #include <unistd.h>

#include "clustering.h"
#include "DBSCAN.h"

// using namespace std;

vector<float> getData(string filename)
{
    // read data from file.
    ifstream file;
    file.open(filename);
    
    // get total number of doubles
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    int total_num = length / sizeof(float);
    // cout << total_num << endl;
    
    // save to data
    vector<float> data(total_num);
    char buffer[sizeof(float)];    
    for (int i=0; i<total_num; i++) {   
        file.read(buffer, sizeof(float));
        file.seekg((i+1)*sizeof(float));
        memcpy(&data[i], &buffer, sizeof(float));
    }
    
    // test output data
    // for (int i=0; i < 100; i++) {
    //     cout << data[i] << ' ';
    // }
    // cout << endl;
    
    file.close();
    return data;
}


bool isGating(vector<int> &label, vector<vector<double>> &cluster_stats, 
              double sampling_rate=20000, double duration_thres=0.5,
              int signal_min=20, int signal_max=150,
              int gating_min=40, int gating_max=130, int hard_min=10, int gating_std=6,
              int openpore_thres=170)
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
        if ((cluster_stats[i][1] < gating_max && cluster_stats[i][1] > gating_min
             && cluster_stats[i][2] < gating_std) || cluster_stats[i][1] < hard_min)
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

//int main()
//{
//    auto t0 = chrono::steady_clock::now();
//    cout.precision(16);
    
//    // ******************* Data loading *******************************
//    string filename = "/mnt/seqdata/raw_data/20200820_multi32_V2_WHH_lambda_genome/Raw/channel03.dat";
//    // string filename = "test_data/syn.dat";
//    vector<float> data = getData(filename);
//    auto t1 = chrono::steady_clock::now();

//    cout << "data loading completed in: "
//         << chrono::duration_cast<chrono::seconds>(t1-t0).count()
//         << " seconds, total length: " << data.size() << endl;

//    // ******************* Test 10k pts buffer loading ******************
//    // buffer segment
//    int segment_size = 20000;
//    // segment overlap
//    int overlap = 1000;

//    vector<float>::const_iterator beg;
//    vector<float>::const_iterator end;
    
//    int total_segments = data.size() / (segment_size-overlap) + 1;

//    vector<float> buffer(segment_size);
//    vector<int> label(buffer.size());
//    int eps_p(50);
//    int eps_c(10);
//    int s_min_pts(10);
//    int eps_d(10);
//    int d_min_pts(1);
//    bool pre_flag(true);
//    vector<vector<double>> cluster_stats;

//    bool gating_flag;

//    for (int i=0; i<total_segments; i++) {
//        // check whether current iteration reaches the end
//        if (i+1 == total_segments) {
//            beg = data.end()-segment_size;
//            end = data.end();
//        }
//        else {
//            beg = data.begin()+i*(segment_size-overlap);
//            end = data.begin()+i*(segment_size-overlap)+segment_size;
//        }
        
//        buffer.assign(beg, end);
//        cluster_stats = LinearDBSCAN(buffer, label,
//                     eps_p, eps_c, s_min_pts,
//                     eps_d, d_min_pts,
//                     pre_flag);
        
//        gating_flag = isGating(label, cluster_stats);
//        cout << "segment " << i+1 << ":" << gating_flag << endl;
//        for (int k=0; k<cluster_stats.size(); k++)
//        {
//            cout << cluster_stats[k][1] << " " << cluster_stats[k][2] << endl;
//        }
//        cout << endl;
//    }

//    // auto t2 = chrono::steady_clock::now();
//    // chrono::duration<double> total_t = t2-t1;
//    // cout << "gating check finished in: "
//    //      << total_t.count() << " seconds, average time for each segment: "
//    //      << (total_t/total_segments).count() << " seconds." << endl;
    
//    // ************* DBSCAN test *****************************
//    // vector<vector<double>> cur(1000, vector<double>(2, 0));
//    // buffer.assign(data.begin(), data.begin()+1000);
//    // for (int i=0; i<1000; i++)
//    // {
//    //     cur[i][0] = buffer[i];
//    // }
//    // DBSCAN2D db(cur);
//    // vector<int> label = db.Fit();
//    // for (int i=0; i<label.size(); i++)
//    // {
//    //     cout << label[i] << " ";
//    // }
//    // cout << endl;

//    // ***************** Start gating detection **************************
//    // vector<double> buffer(data);
//    // vector<int> label(buffer.size());
//    // int eps_p(50);
//    // int eps_c(10);
//    // int s_min_pts(10);
//    // int eps_d(10);
//    // int d_min_pts(1);
//    // bool pre_flag(true);

//    // vector<vector<double>> cluster_stats =
//    //     LinearDBSCAN(buffer, label,
//    //                  eps_p, eps_c, s_min_pts,
//    //                  eps_d, d_min_pts,
//    //                  pre_flag);

//    // for (int i=0; i<label.size(); i++)
//    // {
//    //     cout << label[i] << " ";
//    // }
//    // cout << endl;

//    // for (int i=0; i<cluster_stats.size(); i++)
//    // {
//    //     cout << cluster_stats[i][1] << " " << cluster_stats[i][2] << endl;
//    // }


//    // cout << "Check current buffer for gating: " << boolalpha << isGating(label, cluster_stats) << endl;
//    return 0;

//}
