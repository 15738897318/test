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
    // findpeaks in vector<double> segment, with minPeakDistance and threhold arg, return 2 vectors: max_peak_strengths, max_peak_locs
    // complexity: O(n^2), n = number of peaks
    void findpeaks(vector<double> segment, double minPeakDistance, double threshold, vector<double> &max_peak_strengths, vector<int> &max_peak_locs);

    // unique_stable with vector<pair<double,int>>
    vector<pair<double,int>> unique_stable(vector<pair<double,int>> arr);

    // conv(seg1, seg2, 'same')
    vector<double> conv(vector<double> seg1, vector<double> seg2);

    // [counts, centres] = hist(arr, nbins)
    void hist( vector<double> arr, int nbins, vector<int> &counts, vector<double> &centers);

    // invprctile
    double invprctile(vector<double> arr, double x);

    //prctile
    double prctile(vector<double> arr, double percent);

    //filter function for frames2signal function
    vector<double> low_pass_filter(vector<double> arr);
    
    // return Discrete Fourier Transform of a 2-2 Mat by dimension
	Mat fft(const Mat &src, int dimension);
    
    // return Inverse Discrete Fourier Transform of a 2-2 Mat by dimension
	Mat ifft(const Mat &src, int dimension);
}

#endif /* defined(__MisfitHeartRate__matlab__) */
