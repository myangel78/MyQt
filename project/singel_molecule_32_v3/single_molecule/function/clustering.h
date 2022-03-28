#include <vector>

using namespace std;

void GetPosition(vector<int> label, int lbl, vector<int> pos);
void GetUnique(vector<int> &label, vector<int> &unique_label);
double CustomMean(vector<float> &data, vector<int> &pos);
void CustomMeanAndStd(vector<float> &data, vector<int> &pos, double &mean, double &stdev);
void PreClustering(vector<float> &data, vector<int> &label, int eps_c);
void GetClusterStats(vector<float> &data, vector<int> &label, vector<vector<double>> &stats);

void CleanLabel(vector<int> &label, 
                vector<vector<double>> &cluster_stats, 
                vector<int> &cluster_label);

void LinearSearch(vector<float> &data, 
                  vector<int> &label, 
                  vector<vector<double>> &cluster_stats, 
                  int eps_p, 
                  int eps_c, 
                  int min_pts, 
                  bool pre_flag);

vector<vector<double>> LinearDBSCAN(vector<float> &data, 
                                    vector<int> &label, 
                                    int eps_p, 
                                    int eps_c, 
                                    int s_min_pts, 
                                    int eps_d,
                                    int d_min_pts,
                                    bool pre_flag);