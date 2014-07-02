//
//  hr_calc_autocorr.cpp
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hr_calc_autocorr.h"

double hr_calc_autocorr(vector<double> temporal_mean, double fr, int firstSample, int window_size, double overlap_ratio, double minPeakDistance, hrDebug& debug){
    // Step 1: calc the window-based autocorrelation of the signal stream
    
    int windowStart = firstSample;
    vector<double> autocorrelation;
    
    
    while(windowStart <= (int) temporal_mean.size() - window_size){
        
        vector<double> segment;
        vector<double> max_peak_strengths, min_peak_strengths;
        vector<int> max_peak_locs, min_peak_locs;
        
        int segment_length;
        
        //Window to calculate the autocorrelation
        for(int i=windowStart; i<windowStart+window_size; ++i) segment.push_back(temporal_mean[i]);
        

        
        //calc mean and get segment = segment - mean
        double sum = accumulate(segment.begin(), segment.end(), 0);
        double mean = sum/segment.size();
        for(int i=0; i<(int) segment.size(); ++i) segment[i]-=mean;
        //get the reverse vector of segment
        vector<double> rev_segment=segment;
        reverse(rev_segment.begin(), rev_segment.end());
        
        //Calculate the autocorrelation for the current window
        vector<double> local_autocorr = conv(segment, rev_segment);
        
        //Define the segment length
        
        // a. Shine-step-counting style
        findpeaks(local_autocorr, minPeakDistance, 0, max_peak_strengths, max_peak_locs);
        
        if(max_peak_locs.empty()){
            segment_length = window_size;
        }else{
            for(int i=0; i<(int) local_autocorr.size(); ++i) local_autocorr[i] = -local_autocorr[i];
            findpeaks(local_autocorr, minPeakDistance, 0, min_peak_strengths, min_peak_locs);
            if(min_peak_locs.empty()){
                segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size + 1) / 2 ; //round
            }else{
                segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end())
                                  + *max_element(min_peak_locs.begin(), min_peak_locs.end()) + 1) / 2 ; //round
            }
        }
        
        // b. Equal-step progression
        // segment_length = window_size
        
        for(int i=0; i<(int)local_autocorr.size(); ++i) autocorrelation.push_back(local_autocorr[i]);
        
        // Define the start of the next window
        windowStart = windowStart + int((1-overlap_ratio)*segment_length+0.5+1e-9);
        
    }
    
    // Step 2: perform peak-counting on the autocorrelation stream
    windowStart = firstSample;
    vector<pair<double, int>> heartBeats;
    vector<double> heartRates;
    
    while(windowStart <= (int) autocorrelation.size() - window_size){
        
        vector<double> segment;
        vector<double> max_peak_strengths, min_peak_strengths;
        vector<int> max_peak_locs, min_peak_locs;
        int segment_length;
        
        for(int i=windowStart; i<windowStart+window_size; ++i) segment.push_back(autocorrelation[i]);
        
        //Count the number of peaks in this window
        findpeaks(segment, minPeakDistance, 0, max_peak_strengths, max_peak_locs);
        
        //Define the segment length
        // a. Shine-step-counting style
        if(max_peak_locs.empty()){
            segment_length = window_size;
        }else{
            for(int i=0; i<(int) segment.size(); ++i) segment[i]=-segment[i];
            findpeaks(segment, minPeakDistance, 0, min_peak_strengths, min_peak_locs);
            if(min_peak_locs.empty()){
                segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size + 1) / 2 ; //round
            }else{
                segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end())
                                  + *max_element(min_peak_locs.begin(), min_peak_locs.end()) + 1) / 2 ; //round
            }
        }
        
        // b. Equal_step progression
        // segment_length = window_size;
        
        // Record all beats in the window, even if there are duplicates
        for(int i=0; i<(int) max_peak_locs.size(); ++i)
            heartBeats.push_back(pair<double, int> (max_peak_strengths[i], max_peak_locs[i] + windowStart));
        
        // Calculate the HR for this window
        
        double rate = (double) max_peak_locs.size() / segment_length * fr;
        for(int i=windowStart; i<windowStart+segment_length; ++i) heartRates.push_back(rate);
        
        windowStart = windowStart + int((1-overlap_ratio)*segment_length+0.5+1e-9);
        
    }
    
    //Prune the beats counted to include only unique ones
    heartBeats = unique_stable(heartBeats);
    
    //Calc the avg HR for the whole stream
    
    double avg_hr=0;
    if(!heartBeats.empty()){
        avg_hr = round((double)heartBeats.size() / ((double)heartRates.size() - firstSample) * fr * 60);
    }
    
    debug.heartBeats = heartBeats;
    debug.heartRates=heartRates;
    debug.autocorrelation = autocorrelation;
    
    return avg_hr;

}