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
    // findpeaks in vector<mTYPE> segment, with minPeakDistance and threhold arg, return 2 vectors: max_peak_strengths, max_peak_locs
    // complexity: O(n^2), n = number of peaks
    void findpeaks(const vector<mTYPE> &segment, mTYPE minPeakDistance, mTYPE threshold,
                   vector<mTYPE> &max_peak_strengths, vector<int> &max_peak_locs);
    
    // unique_stable with vector<pair<mTYPE,int>>
    vector<pair<mTYPE,int>> unique_stable(const vector<pair<mTYPE,int>> &arr);

    // conv(seg1, seg2, 'same')
//    vector<mTYPE> conv(vector<mTYPE> signal, vector<mTYPE> kernel);
    
    vector<mTYPE> corr_linear(vector<mTYPE> signal, vector<mTYPE> kernel);
    
    // [counts, centres] = hist(arr, nbins)
    void hist(const vector<mTYPE> &arr, int nbins, vector<int> &counts, vector<mTYPE> &centers);

    // invprctile
    mTYPE invprctile(const vector<mTYPE> &arr, mTYPE x);

    //prctile
    mTYPE prctile(vector<mTYPE> arr, mTYPE percent);

    //filter function for frames2signal function
    vector<mTYPE> low_pass_filter(vector<mTYPE> arr);
    
    mTYPE diff_percent(mTYPE a, mTYPE b);
}

#endif /* defined(__MisfitHeartRate__matlab__) */
