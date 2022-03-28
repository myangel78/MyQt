#include <vector>
#include <iostream>

using namespace std;

class DBSCAN2D {
    public:
        DBSCAN2D(vector<vector<double>> data, int eps_d=10, int d_min_pts=1)
        { 
            pts = data; 
            eps = eps_d; 
            min_pts = d_min_pts;
            label.resize(pts.size());
        };
        void CalDist(vector<double> &cur_pt, vector<double> &dist);
        void GetNeighbors(vector<double> &cur_pt, vector<int> &pos);
        vector<int> Fit();

    private:
        vector<vector<double>> pts;
        int eps;
        int min_pts;
        vector<int> label;
    
};