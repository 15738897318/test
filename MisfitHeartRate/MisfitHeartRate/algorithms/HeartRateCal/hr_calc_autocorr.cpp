//
//  hr_calc_autocorr.cpp
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hr_calc_autocorr.h"

double hr_calc_autocorr(vector<double> temporal_mean, double fr, int firstSample, int window_size, double overlap_ratio, double minPeakDistance){
    // Step 1: calc the window-based autocorrelation of the signal stream
    
    int windowStart = firstSample;
    vector<double> autocorrelation;
    vector<pair<double, int>> heartBeats;
    vector<double> heartRates;
    
    
    while(windowStart <= (int) temporal_mean.size() - window_size){
        vector<double> segment;
        vector<double> max_peak_strengths, min_peak_strengths;
        vector<int> max_peak_locs, min_peak_locs;
        int segment_length;
        
        for(int i=windowStart; i<windowStart+window_size; ++i) segment.push_back(temporal_mean[i]);
        
    }
}

