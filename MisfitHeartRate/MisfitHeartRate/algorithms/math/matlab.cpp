//
//  matlab.cpp
//  Pulsar
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "matlab.h"


namespace MHR {
    // get mean value of a double vector
    double mean(const vector<double> &a)
    {
        double sum = 0;
        int n = (int)a.size();
        for (int i = 0; i < n; ++i)
            sum += a[i];
        return sum/double(n);
    }

    
    // findpeaks in vector<double> segment, with minPeakDistance and threhold arg, return 2 vectors: max_peak_strengths, max_peak_locs
    // complexity: O(n^2), n = number of peaks
    void findpeaks(const vector<double> &segment, double minPeakDistance, double threshold,
                   vector<double> &max_peak_strengths, vector<int> &max_peak_locs)
    {
        max_peak_strengths.clear(); max_peak_locs.clear();
        
        vector<pair<double,int>> peak_list;
        
        for(int i=1; i<(int) segment.size()-1; ++i){
            if(segment[i] - segment[i-1] >= threshold && segment[i] - segment[i+1] > threshold)
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
    vector<pair<double,int>> unique_stable(const vector<pair<double,int>> &arr) {
        set<int> mys;
        
        vector<pair<double,int>> res;
        for(int i=0; i<(int) arr.size(); ++i){
            if(mys.count(arr[i].second)>0) continue;
            res.push_back(arr[i]);
            mys.insert(arr[i].second);
        }
        return res;
    }

    
    vector<double> corr_linear(vector<double> signal, vector<double> kernel) {
        int m = (int)signal.size(), n = (int)kernel.size();
        
        // -meanValue
        double meanValue = mean(signal);
        for (int i = 0; i < m; ++i) signal[i] -= meanValue;
        meanValue = mean(kernel);
        for (int i = 0; i < n; ++i) kernel[i] -= meanValue;

        // padding of zeors
        for(int i = m; i < m+n-1; i++) signal.push_back(0);
        for(int i = n; i < m+n-1; i++) kernel.push_back(0);
        
        /* convolution operation */
        vector<double> ans;
        for(int i = 0; i < m+n-1; i++)
        {
            ans.push_back(0);
            for(int j = 0; j <= i; j++)
                ans[i] += signal[j]*kernel[i-j];
        }
        
        for (int i = 0; i < n-1; ++i)
            ans.pop_back();
        double minValue = *min_element(ans.begin(), ans.end());
        if (minValue < 0)
            for (int i = 0, sz = (int)ans.size(); i < sz; ++i)
                ans[i] -= minValue;
        return ans;
    }

    
    // [counts, centres] = hist(arr, nbins)
    void hist(const vector<double> &arr, int nbins, vector<int> &counts, vector<double> &centers) {
        if (&arr == &centers) {
            throw invalid_argument("hist() error: &arr == &centers");
            return;
        }
        
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
        for(int i=0; i<nbins; ++i)
            centers[i] = bin_length * i + bin_length / 2.0 + minv;
        
        for(int i=0; i<(int) arr.size(); ++i){
            double v=arr[i]-minv;
            int p = (int)((v - 1e-9)/bin_length);
            ++counts[p];
        }
        
    }

    
    // invprctile
    double invprctile(const vector<double> &arr, double x) {
        int cnt = 0;
        for(int i=0; i<(int) arr.size(); ++i)
            if(arr[i] < x + 1e-9) ++cnt;
        return 100.0 * cnt / arr.size();
    }

    
    //prctile
    double prctile(vector<double> arr, double percent) {
        sort(arr.begin(), arr.end());
        int n = (int) arr.size();
        double idx = percent * n / 100;
        int int_idx = (int) (idx+1e-9);
        if(fabs(idx - int_idx)<1e-9){
            // idx is a whole number
//            if(int_idx==0) return NaN;
            if(int_idx==0) return arr[0];
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
    vector<double> low_pass_filter(vector<double> arr) {
        clock_t t1 = clock();
        
        // assign values in all NaN positions to 0
        vector<int> nAnPositions;
        int n = (int)arr.size();
        for (int i = 0; i < n; ++i)
            if (abs(arr[i] - NaN) < 1e-11) {
                arr[i] = 0;
                nAnPositions.push_back(i);
            }
        
        // apply low pass filter
        Mat src = vectorToMat(arr);
        Mat filt = arrayToMat(_beatSignalFilterKernel, 1, _beatSignalFilterKernel_size);
        Mat dst;
        filter2D(src, dst, -1, filt, Point(-1,-1), 0, BORDER_CONSTANT);
        vector<double> ans = matToVector1D(dst);
        
        // assign values in all old NaN positions to NaN
        for (int i = 0, sz = (int)nAnPositions.size(); i < sz; ++i)
            ans[nAnPositions[i]] = NaN;
        
        // remove last 7 elements when use FilterBandPassing
        for(int i = 0; i < 7; ++i)
            if(!ans.empty()) ans.pop_back();
        
        if (DEBUG_MODE)
            printf("low_pass_filter() runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
        
        return ans;
    }
    
    
    double diff_percent(double a, double b)
    {
        return abs(a-b)/abs(b)*100;
    }
}