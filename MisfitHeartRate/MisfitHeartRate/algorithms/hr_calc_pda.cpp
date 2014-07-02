//
//  hr_calc_pda.cpp
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hr_calc_pda.h"


    
    double hr_calc_pda(vector<double> temporal_mean, double fr, int firstSample, int window_size, double overlap_ratio, double minPeakDistance, double threshold, hrDebug& debug){
        
        //Perform peak counting for each window
        int windowStart = firstSample;
        vector<pair<double, int>> heartBeats;
        vector<double> heartRates;
        
        
        while(windowStart <= (int) temporal_mean.size() - window_size){
            
            //Window to perform peak-couting in
            vector<double> segment;
            vector<double> max_peak_strengths, min_peak_strengths;
            vector<int> max_peak_locs, min_peak_locs;
            int segment_length;
            
            for(int i=windowStart; i<windowStart+window_size; ++i) segment.push_back(temporal_mean[i]);
            
            //Count the number of peaks in this window
            findpeaks(segment, minPeakDistance, threshold, max_peak_strengths, max_peak_locs);
            
            //Define the segment length
            // a. Shine-step-counting style
            if(max_peak_locs.empty()){
                segment_length = window_size;
            }else{
                for(int i=0; i<(int) segment.size(); ++i) segment[i]=-segment[i];
                findpeaks(segment, minPeakDistance, threshold, min_peak_strengths, min_peak_locs);
                if(min_peak_locs.empty()){
                    segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size + 1) / 2 ; //round
                }else{
                    segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end())
                                      + *max_element(min_peak_locs.begin(), min_peak_locs.end())) / 2 ; //round
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
        heartBeats = matlabFunc.unique_stable(heartBeats);
        
        //Calc the avg HR for the whole stream
        
        double avg_hr=0;
        if(!heartBeats.empty()){
            avg_hr = round((double)heartBeats.size() / ((double)heartRates.size() - firstSample) * fr * 60);
        }
        
        debug.heartBeats = heartBeats;
        debug.heartRates=heartRates;
        
        return avg_hr;
        
    }
    
    
