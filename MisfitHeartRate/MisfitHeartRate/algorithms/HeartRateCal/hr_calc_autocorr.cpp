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
        
        //Window to calculate the autocorrelation
        for(int i=windowStart; i<windowStart+window_size; ++i) segment.push_back(temporal_mean[i]);
        
        //Calculate the autocorrelation for the current window
        
        //calc mean and get segment = segment - mean
        double sum = 0;
        for(int i=0; i<(int) segment.size(); ++i) sum+=segment[i];
        double mean = sum/segment.size();
        for(int i=0; i<(int) segment.size(); ++i) segment[i]-=mean;
        
        //get the reverse vector of segment
        vector<double> rev_segment=segment;
        reverse(rev_segment.begin(), rev_segment.end());
        
    }
    
    return 0;
}