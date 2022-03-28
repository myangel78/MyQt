#pragma once

#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;

class DBSCAN2D {
public:
    DBSCAN2D(vector<vector<double>> data, int eps_d = 10, int d_min_pts = 1)
    {
        pts = data;
        eps = eps_d;
        min_pts = d_min_pts;
        label.resize(pts.size());
    }

    void CalDist(vector<double>& cur_pt, vector<double>& dist)
    {
        double tmp_dist(0.0);
        for (int i = 0; i < pts.size(); i++)
        {
            tmp_dist = 0.0;
            tmp_dist += (cur_pt[0] - pts[i][0]) * (cur_pt[0] - pts[i][0]);
            tmp_dist += (cur_pt[1] - pts[i][1]) * (cur_pt[1] - pts[i][1]);
            dist[i] = sqrt(tmp_dist);
        }
    }

    void GetNeighbors(vector<double>& cur_pt, vector<int>& neigh)
    {
        neigh.clear();
        vector<double> dist(pts.size());
        CalDist(cur_pt, dist);

        for (int i = 0; i < dist.size(); i++)
        {
            if (dist[i] < eps)
            {
                neigh.push_back(i);
            }
        }
    }

    vector<int> Fit()
    {
        int cluster = 1;

        vector<int> neigh;
        for (int i = 0; i < pts.size(); i++)
        {
            if (label[i] > 0)
            {
                continue;
            }
            GetNeighbors(pts[i], neigh);
            if (neigh.size() < min_pts)
            {
                label[i] = -1;
                continue;
            }

            label[i] = cluster;

            vector<int> loop_neigh(neigh);
            vector<int> n_pos;
            vector<int> set_diff;
            while (loop_neigh.size() > 0)
            {
                int pos = loop_neigh.at(0);
                loop_neigh.erase(loop_neigh.begin());
                if (pos == i || label[pos] > 0)
                {
                    continue;
                }
                // else if (label[pos] == -1)
                // {
                //     label[pos] = cluster;
                // }
                label[pos] = cluster;
                GetNeighbors(pts[pos], n_pos);
                if (n_pos.size() >= min_pts)
                {
                    sort(neigh.begin(), neigh.end());
                    set_difference(n_pos.begin(), n_pos.end(), neigh.begin(), neigh.end(),
                        inserter(set_diff, set_diff.begin()));
                    for (int j = 0; j < set_diff.size(); j++)
                    {
                        if (!count(neigh.begin(), neigh.end(), set_diff[j]))
                        {
                            neigh.push_back(set_diff[j]);
                            loop_neigh.push_back(set_diff[j]);
                        }
                    }
                }
            }
            cluster += 1;
        }
        return label;
    }

private:
    vector<vector<double>> pts;
    int eps;
    int min_pts;
    vector<int> label;

};