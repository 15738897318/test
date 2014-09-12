//
//  matlab.cpp
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "matlab.h"


namespace MHR
{
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
        max_peak_strengths.clear();
        max_peak_locs.clear();
        
        vector<pair<double,int>> peak_list;
        
        int nSegment = (int)segment.size();
        for (int i = 1; i < nSegment - 1; ++i)
        {
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
        for (int i = n+1; i < nSegment; ++i)
        {
            minValue = min(minValue, segment[i]);
            maxValue = max(maxValue, segment[i]);
        }
        if (maxValue == minValue)
            peak_list.pop_back();
        

        sort(peak_list.begin(), peak_list.end());
        for (int i = 0; i < nPeaks; ++i)
        {
            int pos=peak_list[i].second;
            if (pos==-1)
                continue;
            
            for (int j = 0; j < nPeaks; ++j)
                if (j != i && peak_list[j].second != -1 && abs(peak_list[j].second - pos) <= minPeakDistance)
                    peak_list[j].second = -1;
        }
        
        for (int i = 0; i < nPeaks; ++i)
            if (peak_list[i].second != -1)
            {
                max_peak_locs.push_back(peak_list[i].second);
                max_peak_strengths.push_back(segment[peak_list[i].second]);
            }
    }


    // unique_stable with vector<pair<double,int>>
    vector<pair<double,int>> unique_stable(const vector<pair<double,int>> &arr)
    {
        set<int> mys;
        
        vector<pair<double,int>> res;
        for (int i=0; i<(int) arr.size(); ++i)
        {
            if (mys.count(arr[i].second)>0)
                continue;
            res.push_back(arr[i]);
            mys.insert(arr[i].second);
        }
        return res;
    }

    
    vector<double> corr_linear(vector<double> signal, vector<double> kernel, bool subtractMean)
    {
        int m = (int)signal.size(), n = (int)kernel.size();
        
        // -meanValue
        if (subtractMean)
        {
            double meanValue = mean(signal);
            for (int i = 0; i < m; ++i) signal[i] -= meanValue;
            meanValue = mean(kernel);
            for (int i = 0; i < n; ++i) kernel[i] -= meanValue;
        }

        // padding of zeors
        for (int i = m; i < m+n-1; i++) signal.push_back(0);
        for (int i = n; i < m+n-1; i++) kernel.push_back(0);
        
        /* convolution operation */
        vector<double> ans;
        for (int i = 0; i < m+n-1; i++)
        {
            ans.push_back(0);
            for (int j = 0; j <= i; j++)
                ans[i] += signal[j] * kernel[i - j];
        }
        
        for (int i = 0; i < n - 1; ++i)
            ans.pop_back();
        
        if (subtractMean)
        {
            double minValue = *min_element(ans.begin(), ans.end());
            if (minValue < 0)
                for (int i = 0, sz = (int)ans.size(); i < sz; ++i)
                    ans[i] -= minValue;
        }
        return ans;
    }

    
    // [counts, centres] = hist(arr, nbins)
    void hist(const vector<double> &arr, int nbins, vector<int> &counts, vector<double> &centers)
    {
        if (&arr == &centers)
        {
            throw invalid_argument("hist() error: &arr == &centers");
            return;
        }
        
        counts.clear();
        centers.clear();
        
        double minv = arr[0], maxv = arr[0];
        for (int i = 0; i < (int)arr.size(); ++i)
        {
            minv = min(minv,arr[i]);
            maxv = max(maxv,arr[i]);
        }
        
        double length = maxv - minv;
        
        double bin_length;
        if (length > 0)
        {
            bin_length = length / nbins;
        }
        else
        {
            bin_length = 1.0;
        }
        
        counts.resize(nbins,0);
        centers.resize(nbins,0);
        for (int i = 0; i < nbins; ++i)
            centers[i] = bin_length * i + bin_length / 2.0 + minv;
        
        for (int i = 0; i < (int) arr.size(); ++i)
        {
            double v = arr[i] - minv;
            int p = (int)((v - 1e-9) / bin_length);
            ++counts[p];
        }
    }

    
    // invprctile
    double invprctile(const vector<double> &arr, double x)
    {
        int cnt = 0;
        for (int i = 0; i < (int)arr.size(); ++i)
            if (arr[i] < x + 1e-9)
                ++cnt;
        return 100.0 * cnt / arr.size();
    }

    
    //prctile
    double prctile(vector<double> arr, double percent)
    {
        sort(arr.begin(), arr.end());
        
        int n = (int) arr.size();
        double idx = percent * n / 100;
        int int_idx = (int) (idx + 1e-9);
        
        if (fabs(idx - int_idx) < 1e-9)
        {
            // idx is a whole number
//            if (int_idx == 0) return NaN;
            if (int_idx == 0) return arr[0];
            int next_int = int_idx;
            if (next_int < n)
                next_int++;
            return (arr[int_idx - 1] + arr[next_int - 1]) / 2;
        }
        else
        {
            int ceil_int = (int)(ceil(idx));
            int floor_int = (int)(floor(idx));
            double vfloor = arr[floor_int - 1];
            double vceil = arr[ceil_int - 1];
            return vfloor + (idx - floor_int) / (ceil_int-floor_int) * (vceil - vfloor);
        }
    }

    
    //filter function for frames2signal function
    vector<double> low_pass_filter(vector<double> arr)
    {
        clock_t t1 = clock();
        
        // assign values in all NaN positions to 0
        vector<int> nAnPositions;
        int n = (int)arr.size();
        for (int i = 0; i < n; ++i)
            if (abs(arr[i] - NaN) < 1e-11)
            {
                arr[i] = 0;
                nAnPositions.push_back(i);
            }

        // using corr_linear()
        vector<double> kernel;
        for (int i = 0; i < _beatSignalFilterKernel.size.p[0]; ++i)
            for (int j = 0; j < _beatSignalFilterKernel.size.p[1]; ++j)
                kernel.push_back(_beatSignalFilterKernel.at<double>(i, j));
        vector<double> ans = corr_linear(arr, kernel, false);

        // assign values in all old NaN positions to NaN
        for (int i = 0, sz = (int)nAnPositions.size(); i < sz; ++i)
            ans[nAnPositions[i]] = NaN;
        
        // remove first _beatSignalFilterKernel_size/2 elements when use FilterBandPassing
        ans = vector<double>(ans.begin() + _beatSignalFilterKernel_size/2, ans.end());
        
        if (_DEBUG_MODE)
            printf("low_pass_filter() runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
        
        return ans;
    }
    
    
    double diff_percent(double a, double b)
    {
        return abs(a-b)/abs(b)*100;
    }
    
    
    void hr_polisher(double &hr, double &old_hr, double &hrThreshold, double &hrStanDev)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> d(0, hrStanDev);
        
        double randomiser;
        
        if (hr < hrThreshold)
        {
//            randomiser = arc4random() % (10 + 5 + 1) - 5;
//            randomiser = (int)arc4random() % (6) - 2;
            randomiser = d(gen);
            
            hr = hrThreshold + randomiser;
        }
        else
        {
            // If the new HR is same as the old HR, then show a randomised number based on the old HR
            if (int(hr) == int(old_hr))
            {
//                randomiser = arc4random() % (6 + 3 + 1) - 3;
//                randomiser = (int)arc4random() % (3) - 1;
                randomiser = d(gen);
                
                hr = old_hr + randomiser;
            }
        }
        
        old_hr = hr;
    }
    
}