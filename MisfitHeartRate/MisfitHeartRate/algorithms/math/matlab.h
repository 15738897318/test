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
#include "MHRMatrix.h"

using namespace std;
using namespace cv;

    // findpeaks in vector<double> segment, with minPeakDistance and threhold arg, return 2 vectors: max_peak_strengths, max_peak_locs
    // complexity: O(n^2), n = number of peaks
    void findpeaks(vector<double> segment, double minPeakDistance, double threshold, vector<double> &max_peak_strengths, vector<int> &max_peak_locs);
    
    // unique_stable with vector<pair<double,int>>
    vector<pair<double,int>> unique_stable(vector<pair<double,int>> arr);

    // conv(seg1, seg2, 'same')
    vector<double> conv(vector<double> seg1, vector<double> seg2);

#endif /* defined(__MisfitHeartRate__matlab__) */
