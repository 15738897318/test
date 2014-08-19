//
//  matlab.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__matlab__
#define __MisfitHeartRate__matlab__

#include <iostream>
#include <vector>
#include <set>
#include "matrix.h"
#include "config.h"

using namespace std;
using namespace cv;


namespace MHR {
    // get mean value of a double vector
    double mean(const vector<double> &a);
    
    // findpeaks in vector<double> segment, with minPeakDistance and threhold arg, return 2 vectors: max_peak_strengths, max_peak_locs
    // complexity: O(n^2), n = number of peaks
    void findpeaks(const vector<double> &segment, double minPeakDistance, double threshold,
                   vector<double> &max_peak_strengths, vector<int> &max_peak_locs);
    
    // unique_stable with vector<pair<double,int>>
    vector<pair<double,int>> unique_stable(const vector<pair<double,int>> &arr);
    
    vector<double> corr_linear(vector<double> signal, vector<double> kernel, bool subtractMean = true);
    
    // [counts, centres] = hist(arr, nbins)
    void hist(const vector<double> &arr, int nbins, vector<int> &counts, vector<double> &centers);

    // invprctile
    double invprctile(const vector<double> &arr, double x);

    //prctile
    double prctile(vector<double> arr, double percent);

    //filter function for frames2signal function
    vector<double> low_pass_filter(vector<double> arr);
    
    double diff_percent(double a, double b);
}

#endif /* defined(__MisfitHeartRate__matlab__) */
