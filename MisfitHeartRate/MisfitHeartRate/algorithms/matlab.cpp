//
//  matlab.cpp
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "matlab.h"

    // findpeaks in vector<double> segment, with minPeakDistance and threhold arg, return 2 vectors: max_peak_strengths, max_peak_locs
    // complexity: O(n^2), n = number of peaks
    void findpeaks(vector<double> segment, double minPeakDistance, double threshold, vector<double> &max_peak_strengths, vector<int> &max_peak_locs){
        max_peak_strengths.clear(); max_peak_locs.clear();
        
        vector<pair<double,int>> peak_list;
        
        for(int i=1; i<(int) segment.size()-1; ++i){
            if(segment[i] - segment[i-1] > threshold && segment[i] - segment[i+1] > threshold)
                peak_list.push_back(pair<double,int> (-segment[i], i));
        }
        
        sort(peak_list.begin(), peak_list.end());
        for(int i=0; i<(int) peak_list.size(); ++i){
            int pos=peak_list[i].second;
            if(pos==-1) continue;
            for(int j=0; j<(int) peak_list.size(); ++j) if(j!=i && peak_list[j].second!=-1 && abs(peak_list[j].second-pos) <= minPeakDistance)
                peak_list[j].second=-1;
        }
        
        for(int i=0; i<(int) peak_list.size(); ++i)
            if(peak_list[i].second!=-1){
                max_peak_locs.push_back(peak_list[i].second);
                max_peak_strengths.push_back(peak_list[i].first);
            }
    }


vector<pair<double,int>> unique_stable(vector<pair<double,int>> arr){
    set<int> mys;
    vector<pair<double,int>> res;
    for(int i=0; i<(int) arr.size(); ++i){
        if(mys.count(arr[i].second)>0) continue;
        res.push_back(arr[i]);
        mys.insert(arr[i].second);
    }
    return res;
}
