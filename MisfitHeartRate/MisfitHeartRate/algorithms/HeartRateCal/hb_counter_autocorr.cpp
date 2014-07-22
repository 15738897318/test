//
//  hb_counter_autocorr.cpp
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hb_counter_autocorr.h"


namespace MHR {
    vector<int> hb_counter_autocorr(vector<double> &temporal_mean, double fr, int firstSample,
                            int window_size, double overlap_ratio, double minPeakDistance, hrDebug& debug)
    {
        // Step 1: calc the window-based autocorrelation of the signal stream
        
        int windowStart = firstSample;
        vector<double> autocorrelation;
        double lastSegmentEndVal = 0;

        while(windowStart < (int)temporal_mean.size() - 1){
            
            vector<double> segment;
            vector<double> max_peak_strengths, min_peak_strengths;
            vector<int> max_peak_locs, min_peak_locs;
            
            int segment_length;
            
            //Window to calculate the autocorrelation
            int windowEnd = min(windowStart + window_size, (int)temporal_mean.size());
            for(int i = windowStart; i < windowEnd; ++i)
                segment.push_back(temporal_mean[i]);
            
            //calc mean and get segment = segment - mean
            double sum = accumulate(segment.begin(), segment.end(), 0);
            double mean = sum/segment.size();
            for(int i=0; i<(int) segment.size(); ++i) segment[i]-=mean;
            //get the reverse vector of segment
            vector<double> rev_segment=segment;
            reverse(rev_segment.begin(), rev_segment.end());
            
            //Calculate the autocorrelation for the current window
            vector<double> local_autocorr = corr_linear(segment, rev_segment);
            for(int i = 0, sz = (int)local_autocorr.size(); i < sz; ++i)
                local_autocorr[i] -= local_autocorr[0] - lastSegmentEndVal;
            
            //Define the segment length
            
            // a. Shine-step-counting style
            findpeaks(local_autocorr, minPeakDistance, 0, max_peak_strengths, max_peak_locs);
            
            if(max_peak_locs.empty()){
                segment_length = (int)segment.size();
            }else{
                for(int i=0; i<(int) local_autocorr.size(); ++i) local_autocorr[i] = -local_autocorr[i];
                findpeaks(local_autocorr, minPeakDistance, 0, min_peak_strengths, min_peak_locs);
                if(min_peak_locs.empty()){
                    segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size + 1) / 2 ; //round
                    segment_length = min(segment_length, window_size);
                }else{
                    segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end())
                                      + *max_element(min_peak_locs.begin(), min_peak_locs.end()) + 1) / 2 ; //round
                }
                for(int i=0; i<(int) local_autocorr.size(); ++i) local_autocorr[i] = -local_autocorr[i];
            }
            
            
            // b. Equal-step progression
            // segment_length = window_size
            
            for(int i=0; i<(int)local_autocorr.size(); ++i) autocorrelation.push_back(local_autocorr[i]);

            // Define the start of the next window
            windowStart = windowStart + int((1-overlap_ratio)*segment_length+0.5+1e-9);
            lastSegmentEndVal = autocorrelation[windowStart - 1];
        }
        
        // Step 2: perform peak-counting on the autocorrelation stream
        windowStart = firstSample;
        vector<pair<double, int>> heartBeats;
        vector<double> heartRates;
        while(windowStart < (int)autocorrelation.size() - 1){
            
            vector<double> segment;
            vector<double> max_peak_strengths, min_peak_strengths;
            vector<int> max_peak_locs, min_peak_locs;
            int segment_length;
            
            int windowEnd = min(windowStart + window_size, (int)autocorrelation.size());
            for(int i = windowStart; i < windowEnd; ++i)
                segment.push_back(autocorrelation[i]);
            
            //Count the number of peaks in this window
            findpeaks(segment, minPeakDistance, 0, max_peak_strengths, max_peak_locs);
            
            //Define the segment length
            // a. Shine-step-counting style
            if(max_peak_locs.empty()){
                segment_length = (int)segment.size();
            }else{
                for(int i=0; i<(int) segment.size(); ++i) segment[i]=-segment[i];
                findpeaks(segment, minPeakDistance, 0, min_peak_strengths, min_peak_locs);
                if(min_peak_locs.empty()){
                    segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size + 1) / 2 ; //round
                }else{
                    segment_length = ( *max_element(max_peak_locs.begin(), max_peak_locs.end())
                                      + *max_element(min_peak_locs.begin(), min_peak_locs.end()) + 1) / 2 ; //round
                }
                for(int i=0; i<(int) segment.size(); ++i) segment[i]=-segment[i];
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
            //avg_hr = round((double)heartBeats.size() / ((double)heartRates.size() - firstSample) * fr * 60);
            int cnt=0;
            for(int i=firstSample; i<(int)temporal_mean.size(); ++i)
                if(temporal_mean[i] != NaN) ++cnt;
            if(cnt==0) avg_hr = 0;
            else
                avg_hr = round((double)heartBeats.size() / cnt * fr * 60);
        }
        
        debug.avg_hr = avg_hr;
        debug.heartBeats = heartBeats;
        debug.heartRates = heartRates;
        
        vector<int> locations;
        for (int i = 0, sz = (int)heartBeats.size(); i < sz; ++i)
            locations.push_back(heartBeats[i].second);
        sort(locations.begin(), locations.end());
        return locations;
    }
}