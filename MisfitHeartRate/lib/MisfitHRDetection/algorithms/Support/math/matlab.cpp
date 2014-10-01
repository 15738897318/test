//
//  matlab.cpp
//  MisfitHeartRate
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

    void corr_linear(vector<double> &signal, vector<double> &kernel, vector<double> &result, bool subtractMean ) {
        int m = (int)signal.size(), n = (int)kernel.size();
        
            // -meanValue
        if (subtractMean) {
            double meanValue = mean(signal);
            for (int i = 0; i < m; ++i) signal[i] -= meanValue;
            meanValue = mean(kernel);
            for (int i = 0; i < n; ++i) kernel[i] -= meanValue;
        }
        
            // padding of zeors
        for(int i = m; i < m+n-1; i++) signal.push_back(0);
        for(int i = n; i < m+n-1; i++) kernel.push_back(0);
        
        
        for(int i = 0; i < m+n-1; i++)
            {
            result.push_back(0);
            for(int j = 0; j <= i; j++)
                result[i] += signal[j]*kernel[i-j];
            }
        
        for (int i = 0; i < n-1; ++i)
            result.pop_back();
        
        if (subtractMean) {
            double minValue = *min_element(result.begin(), result.end());
            if (minValue < 0)
                for (int i = 0, sz = (int)result.size(); i < sz; ++i)
                    result[i] -= minValue;
        }
        
        signal.erase(signal.begin() + m, signal.end());
        kernel.erase(kernel.begin() + m, kernel.end());
    }

    void findpeaks(const vector<double> &segment, double minPeakDistance, double threshold,
              vector<double> &max_peak_strengths, vector<int> &max_peak_locs)
    {
    max_peak_strengths.clear();
    max_peak_locs.clear();
    
    vector<pair<double,int>> peak_list;
    
    int nSegment = (int)segment.size();
    for (int i = 1; i < nSegment - 1; ++i) {
        if ((segment[i] - segment[i-1] > threshold) &&
            (segment[i] - segment[i+1] >= threshold))
            {
            peak_list.push_back(pair<double,int> (-segment[i], i));
            }
    }
    
    if (peak_list.empty())
        return;
    
        // Code to sort the peaks by position. The first & last peaks should be such that between
        // the peaks and the start / end of the segment there must be no 'straight line'
    int nPeaks = (int)peak_list.size();
    int n = peak_list[nPeaks - 1].second;
    double minValue = segment[n], maxValue = segment[n];
    for (int i = n+1; i < nSegment; ++i) {
        minValue = min(minValue, segment[i]);
        maxValue = max(maxValue, segment[i]);
    }
    if (maxValue == minValue)
        peak_list.pop_back();
    
    
    sort(peak_list.begin(), peak_list.end());
    for (int i = 0; i < nPeaks; ++i){
        int pos=peak_list[i].second;
        if(pos==-1) continue;
        for (int j = 0; j < nPeaks; ++j)
            if(j!=i && peak_list[j].second!=-1 && abs(peak_list[j].second-pos) <= minPeakDistance)
                peak_list[j].second=-1;
    }
    
    for (int i = 0; i < nPeaks; ++i)
        if(peak_list[i].second!=-1){
            max_peak_locs.push_back(peak_list[i].second);
            max_peak_strengths.push_back(segment[peak_list[i].second]);
        }
    }

    
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
        
        double bin_length;
        if (length > 0) {
            bin_length = length / nbins;
        }
        else {
            bin_length = 1.0;
        }
        
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

    
    double invprctile(const vector<double> &arr, double x) {
        int cnt = 0;
        for(int i=0; i<(int) arr.size(); ++i)
            if(arr[i] < x + 1e-9) ++cnt;
        return 100.0 * cnt / arr.size();
    }

    
    double prctile(vector<double> arr, double percent) {
        sort(arr.begin(), arr.end());
        int n = (int) arr.size();
        double idx = percent * n / 100;
        int int_idx = (int) (idx+1e-9);
        if(fabs(idx - int_idx)<1e-9){
            // idx is a whole number
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
    
    
    void gaussianFilter(int length, double sigma, vector<double> &ans) {
		ans.clear();
        Mat kernel = getGaussianKernel(length, sigma, CV_64F);
        for (int i = 0; i < kernel.size.p[0]; ++i)
            for (int j = 0; j < kernel.size.p[1]; ++j)
                ans.push_back(kernel.at<double>(i, j));
        double max_value = *max_element(ans.begin(), ans.end());
        for (int i = 0, sz = (int)ans.size(); i < sz; ++i)
            ans[i] /= max_value;
	}
    
    
    double diff_percent(double a, double b)
    {
        return abs(a-b)/abs(b)*100;
    }
}