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
            max_peak_strengths.push_back(segment[peak_list[i].second]);
        }
}


// unique_stable with vector<pair<double,int>>
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

// conv(seg1, seg2, 'same')
vector<double> conv(vector<double> seg1, vector<double> seg2){
    
    Mat src = vectorToMat(seg1);
    Mat dst;
    Mat kernel = vectorToMat(seg2);
    
    filter2D(src, dst, -1, kernel, Point(-1,-1), 0 , BORDER_CONSTANT);
    return matToVector1D(dst);
    
}

// [counts, centres] = hist(arr, nbins)
void hist( vector<double> arr, int nbins, vector<int> &counts, vector<double> &centers){
    counts.clear();
    centers.clear();
    
    double minv=arr[0], maxv=arr[0];
    for(int i=0; i<(int)arr.size(); ++i){
        minv=min(minv,arr[i]);
        maxv=max(maxv,arr[i]);
    }
    
    double length = maxv-minv;
    double bin_length = length/nbins;
    
    counts.resize(nbins,0);
    centers.resize(nbins,0);
    for(int i=0; i<nbins; ++i) centers[i] = bin_length * i + bin_length / 2;
    
    for(int i=0; i<(int) arr.size(); ++i){
        double v=arr[i]-minv;
        int p = (int)((v - 1e-9)/bin_length);
        ++counts[p];
    }
    
}

// invprctile
double invprctile(vector<double> arr, double x){
    int cnt = 0;
    for(int i=0; i<(int) arr.size(); ++i)
        if(arr[i] < x + 1e-9) ++cnt;
    return 100.0 * cnt / arr.size();
}

//prctile
double prctile(vector<double> arr, double percent){
    sort(arr.begin(), arr.end());
    int n = (int) arr.size();
    double idx = percent * n / 100;
    int int_idx = (int) (idx+1e-9);
    if(fabs(idx - int_idx)<1e-9){
        // idx is a whole number
        if(int_idx==0) return NaN;
        int next_int = int_idx;
        if(next_int < n) next_int++;
        return (arr[int_idx-1] + arr[next_int-1])/2;
    }else{
        int ceil_int = (int)(ceil(idx));
        int floor_int = (int)(floor(idx));
        double vfloor = arr[floor_int-1];
        double vceil = arr[ceil_int-1];
        return vfloor + (idx - floor_int)/(ceil_int-floor_int) * (vceil - vfloor);
        
    }
}

//filter function for frames2signal function
vector<double> low_pass_filter(vector<double> arr){
    Mat src = vectorToMat(arr);
    Mat filt = arrayToMat(_filtArray,1,15);
    Mat dst;
    filter2D(src, dst, -1, filt, Point(-1,-1), 0, BORDER_CONSTANT);
    return matToVector1D(dst);
}