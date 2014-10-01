//
//  SelfCorrPeakHRCounter.cpp
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/26/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#include "SelfCorrPeakHRCounter.h"
#include "face_params.h"
#include "finger_params.h"
#include "matlab.h"

using namespace MHR;

double SelfCorrPeakHRCounter::_window_size_in_sec;
double SelfCorrPeakHRCounter::_max_bpm;
double SelfCorrPeakHRCounter::_overlap_ratio;
double SelfCorrPeakHRCounter::_time_lag;
int SelfCorrPeakHRCounter::_beatSignalFilterKernel_size;
Mat SelfCorrPeakHRCounter::_beatSignalFilterKernel;

void SelfCorrPeakHRCounter::setFaceParameters() {
    _window_size_in_sec = _face_window_size_in_sec;
    _overlap_ratio = _face_overlap_ratio;
    _max_bpm = _face_max_bpm;
    _time_lag = _face_time_lag;
    _beatSignalFilterKernel_size = _face_beatSignalFilterKernel_size;
    _beatSignalFilterKernel = _face_beatSignalFilterKernel.clone();
}

void SelfCorrPeakHRCounter::setFingerParameter() {
    _window_size_in_sec = _finger_window_size_in_sec;
    _overlap_ratio = _finger_overlap_ratio;
    _max_bpm = _finger_max_bpm;
    _time_lag = _finger_time_lag;
    _beatSignalFilterKernel_size = _finger_beatSignalFilterKernel_size;
    _beatSignalFilterKernel = _finger_beatSignalFilterKernel.clone();
}

SelfCorrPeakHRCounter::SelfCorrPeakHRCounter() {
    window_size = round(_window_size_in_sec * MHR::_frameRate);
    firstSample = round(MHR::_frameRate * _time_lag);
    threshold_fraction = 0;
}

void SelfCorrPeakHRCounter::low_pass_filter(vector<double> &temporal_mean) {
    filtered_temporal_mean.clear();
    if (filtered_temporal_mean.capacity() != temporal_mean.size())
        filtered_temporal_mean.reserve(temporal_mean.size());
    
    // assign values in all NaN positions to 0
    vector<int> nAnPositions;
    int n = (int)temporal_mean.size();
    for (int i = 0; i < n; ++i)
        if (abs(temporal_mean[i] - NaN) < 1e-11) {
            temporal_mean[i] = 0;
            nAnPositions.push_back(i);
        }
    
        // using corr_linear()
    vector<double> kernel;
    for (int i = 0; i < _beatSignalFilterKernel.size.p[0]; ++i)
        for (int j = 0; j < _beatSignalFilterKernel.size.p[1]; ++j)
            kernel.push_back(_beatSignalFilterKernel.at<double>(i, j));
    corr_linear(temporal_mean, kernel, filtered_temporal_mean, false);
    
        // assign values in all old NaN positions to NaN
    for (int i = 0, sz = (int)nAnPositions.size(); i < sz; ++i) {
        temporal_mean[nAnPositions[i]] = NaN;
        filtered_temporal_mean[nAnPositions[i]] = NaN;
    }
    
        // remove first _beatSignalFilterKernel_size/2 elements when use FilterBandPassing
    filtered_temporal_mean.erase(filtered_temporal_mean.begin(), filtered_temporal_mean.begin()+ _beatSignalFilterKernel_size/2);
}

MHR::hrResult SelfCorrPeakHRCounter::getHR(vector<double> &temporal_mean) {
    /*-----------------Perform HR calculation for the frames processed so far-----------------*/
        // Low-pass-filter the signal stream to remove unwanted noises
        //temporal_mean_filt = low_pass_filter(temporal_mean);
    low_pass_filter(temporal_mean);
    
    
    clock_t t1 = clock();
        // Set peak-detection params
    if (firstSample > filtered_temporal_mean.size())
        firstSample = 0;
    double threshold = threshold_fraction * (*max_element(filtered_temporal_mean.begin() + firstSample, filtered_temporal_mean.end()));
    int minPeakDistance = round(60 / _max_bpm * _frameRate);
    
        // Calculate heart-rate using peak-detection on the signal
    hrDebug debug_pda;
    vector<int> hb_locations_pda = hb_counter_pda(filtered_temporal_mean, _frameRate, firstSample,
                                                  window_size, _overlap_ratio,
                                                  minPeakDistance, threshold,
                                                  debug_pda);
    vector<double> ans_pda;
    hr_calculator(hb_locations_pda, _frameRate, ans_pda);
    double avg_hr_pda = ans_pda[0];     // average heart rate
    
        // Calculate heart-rate using autocorr algorithm on the signal
    hrDebug debug_autocorr;
    vector<int> hb_locations_autocorr = hb_counter_autocorr(filtered_temporal_mean, _frameRate, firstSample,
                                                            window_size, _overlap_ratio,
                                                            minPeakDistance,
                                                            debug_autocorr);
    vector<double> ans_autocorr;
    hr_calculator(hb_locations_autocorr, _frameRate, ans_autocorr);
    double avg_hr_autocorr = ans_autocorr[0];     // average heart rate
    
    
    if (_DEBUG_MODE)
        printf("hr_signal_calc() time = %f\n", ((float)clock() - (float)t1)/1000.0);
    return hrResult(avg_hr_autocorr, avg_hr_pda);
}

void SelfCorrPeakHRCounter::hr_calculator(const vector<int> &heartBeatPositions, double frameRate, vector<double> &ans) {
    
    ans.clear();
    
    if ((int)heartBeatPositions.size() > 2) {
            // Calculate the instantaneous heart-rates
        vector<double> heartRate_inst;
        for (int i = 1, sz = (int)heartBeatPositions.size(); i < sz; ++i)
            heartRate_inst.push_back( 1.0 / (heartBeatPositions[i] - heartBeatPositions[i-1]) );
        
            // Find the mode of the instantaneous heart-rates
        vector<double> centres;
        vector<int> counts;
        
        hist(heartRate_inst, _number_of_bins_heartRate, counts, centres);
        
        int argmax = 0;
        for(int i = 0, sz = (int)counts.size(); i < sz; ++i)
            if(counts[i] > counts[argmax]) argmax = i;
        double centre_mode = centres[argmax];
        
            // Create a convolution kernel from the found frequency
        vector<double> kernel;
        gaussianFilter(cvCeil(2.0 / centre_mode), 1.0 / (4.0 * centre_mode), kernel);
        double threshold = 2.0 * kernel[cvCeil(1.0 / (4.0 * centre_mode)) - 1];
        
            // Create a heart-beat count signal
        vector<double> count_signal;
        int temp = heartBeatPositions[heartBeatPositions.size() - 1] - heartBeatPositions[0] + 1;
        for (int i = 0; i < temp; ++i) {
            count_signal.push_back(0);
        }
        for (int i = 0, sz = (int)heartBeatPositions.size(); i < sz; ++i) {
            temp = heartBeatPositions[i] - heartBeatPositions[0];
            count_signal[temp] = 1;
        }
        
            // Convolve the count_signal with the kernel to generate a score_signal
        vector<double> score_signal;
        corr_linear(count_signal, kernel, score_signal, false);
        
            // Decide if the any beats are missing and fill them in if need be
        vector<double> min_peak_strengths;
        vector<int> min_peak_locs;
        
        for (int i = 0, sz = (int)score_signal.size(); i < sz; ++i)
            score_signal[i] = -score_signal[i];
        
        findpeaks(score_signal, 0, 0, min_peak_strengths, min_peak_locs);
        for (int i = 0, sz = (int)min_peak_strengths.size(); i < sz; ++i)
            min_peak_strengths[i] = -min_peak_strengths[i];
        
        for (int i = 0, sz = (int)score_signal.size(); i < sz; ++i)
            score_signal[i] = -score_signal[i];
        
        double factor = 1.5;
        threshold *= factor;
        for (int i = 0, len = (int)min_peak_locs.size(); i < len; ++i) {
            if (min_peak_strengths[i] < threshold) {
                count_signal[min_peak_locs[i]] = -1;
            }
        }
        
            // Calculate the heart-rate from the new beat count
        ans.push_back(0);
        int len = (int)count_signal.size();
        for (int i = 0; i < len; ++i)
            ans[0] += abs(count_signal[i]);
        ans[0] /= (double(len) + 1.0/centre_mode);
        ans[0] *= frameRate * 60;
        
        ans.push_back(centre_mode * frameRate * 60);
    }
    else if ((int)heartBeatPositions.size() == 2) {
        ans.push_back(1.0 / (heartBeatPositions[1] - heartBeatPositions[0]));
        ans.push_back(1.0 / (heartBeatPositions[1] - heartBeatPositions[0]));
    }
    else {
        ans.push_back(0);
        ans.push_back(0);
    }
}


void SelfCorrPeakHRCounter::findpeaks(const vector<double> &segment, double minPeakDistance, double threshold,
               vector<double> &max_peak_strengths, vector<int> &max_peak_locs)
{
    max_peak_strengths.clear();
    max_peak_locs.clear();
    
    vector<pair<double,int>> peak_list;
    
    int nSegment = (int)segment.size();
    for (int i = 1; i < nSegment - 1; ++i) {
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
    for (int i = n+1; i < nSegment; ++i) {
        minValue = min(minValue, segment[i]);
        maxValue = max(maxValue, segment[i]);
    }
    if (maxValue == minValue)
        peak_list.pop_back();
    
    
    sort(peak_list.begin(), peak_list.end());
    for (int i = 0; i < nPeaks; ++i){
        int pos=peak_list[i].second;
        if(pos==-1) continue;
        for (int j = 0; j < nPeaks; ++j)
            if(j!=i && peak_list[j].second!=-1 && abs(peak_list[j].second-pos) <= minPeakDistance)
                peak_list[j].second=-1;
    }
    
    for (int i = 0; i < nPeaks; ++i)
        if(peak_list[i].second!=-1){
            max_peak_locs.push_back(peak_list[i].second);
            max_peak_strengths.push_back(segment[peak_list[i].second]);
        }
}

vector<int> SelfCorrPeakHRCounter::hb_counter_pda(vector<double> temporal_mean, double fr, int firstSample, int window_size,
                                                  double overlap_ratio, double minPeakDistance, double threshold, MHR::hrDebug& debug)
{
        //Perform peak counting for each window
    int windowStart = firstSample - 1;
    bool isFirstSegment = true;
    vector<pair<double, int>> heartBeats;
    vector<double> heartRates;
    while(windowStart < (int)temporal_mean.size() - 1) {
        
            //Window to perform peak-couting in
        vector<double> segment;
        vector<double> max_peak_strengths, min_peak_strengths;
        vector<int> max_peak_locs, min_peak_locs;
        int segment_length;
        
        int windowEnd = min(windowStart + window_size, (int)temporal_mean.size());
        for(int i = windowStart; i < windowEnd; ++i)
            segment.push_back(temporal_mean[i]);
        
            //Count the number of peaks in this window
        findpeaks(segment, minPeakDistance, threshold, max_peak_strengths, max_peak_locs);
        
            //Define the segment length
            // a. Shine-step-counting style
        if(max_peak_locs.empty()){
            segment_length = (int)segment.size();
        }else{
            for(int i=0; i<(int) segment.size(); ++i) segment[i]=-segment[i];
            findpeaks(segment, minPeakDistance, threshold, min_peak_strengths, min_peak_locs);
            if(min_peak_locs.empty()){
                segment_length = round((*max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size)/2.0 + 1); //round
                segment_length = min(segment_length, (int)segment.size());
            }else{
                segment_length = round((*max_element(max_peak_locs.begin(), max_peak_locs.end())
                                        + *max_element(min_peak_locs.begin(), min_peak_locs.end()))/2.0 + 1) ; //round
            }
            for(int i=0; i<(int) segment.size(); ++i) segment[i]=-segment[i];
        }
        
            // b. Equal_step progression
            // segment_length = window_size;
        
            // Record all beats in the window, even if there are duplicates
        for(int i=0; i<(int) max_peak_locs.size(); ++i)
            heartBeats.push_back(pair<double, int> (max_peak_strengths[i], max_peak_locs[i] + windowStart));
        
            // Calculate the HR for this window
        int windowUpdate = int((1-overlap_ratio)*segment_length+0.5+1e-9);
        if (isFirstSegment) {
            for (int i = 0; i < windowStart; ++i)
                heartRates.push_back(0);
            isFirstSegment = false;
        }
        
        int count = 0;
        for (int i = 0, sz = (int)segment.size(); i < sz; ++i)
            if (segment[i] > NaN && segment[i] < INFINITY)
                ++count;
        double rate = (double) max_peak_locs.size() / count * fr;
        
        for(int i = windowStart; i < windowStart+windowUpdate; ++i)
            heartRates.push_back(rate);
        
        windowStart = windowStart + windowUpdate;
    }
    
        //Prune the beats counted to include only unique ones
    heartBeats = unique_stable(heartBeats);
    
        //Calc the avg HR for the whole stream
    
    double avg_hr=0;
    if(!heartBeats.empty()){
            //avg_hr = round((double)heartBeats.size() / ((double)heartRates.size() - firstSample) * fr * 60);
        int cnt=0;
        for(int i=firstSample-1; i<(int)temporal_mean.size(); ++i)
            if(temporal_mean[i] != NaN) ++cnt;
        if(cnt==0) avg_hr = 0;
        else
            avg_hr = round((double)heartBeats.size() / cnt * fr * 60);
    };
    
    debug.avg_hr = avg_hr;
    debug.heartBeats = heartBeats;
    debug.heartRates = heartRates;
    
    vector<int> locations;
    for (int i = 0, sz = (int)heartBeats.size(); i < sz; ++i)
        locations.push_back(heartBeats[i].second);
    sort(locations.begin(), locations.end());
    
    return locations;
}

vector<int> SelfCorrPeakHRCounter::hb_counter_autocorr(vector<double> &temporal_mean, double fr, int firstSample,
                                                       int window_size, double overlap_ratio, double minPeakDistance, MHR::hrDebug& debug)
{
        // Step 1: calc the window-based autocorrelation of the signal stream
    
    int windowStart = firstSample-1;
    vector<double> autocorrelation;
    double lastSegmentEndVal = 0;
    bool isFirstSegment = true;
    
    while(windowStart < (int)temporal_mean.size() - 1){
        
        vector<double> segment;
        vector<double> max_peak_strengths, min_peak_strengths;
        vector<int> max_peak_locs, min_peak_locs;
        
        int segment_length;
        
            //Window to calculate the autocorrelation
        int windowEnd = min(windowStart + window_size, (int)temporal_mean.size());
        for(int i = windowStart; i < windowEnd; ++i)
            segment.push_back(temporal_mean[i]);
        
            //Calculate the autocorrelation for the current window
        vector<double> local_autocorr;
        corr_linear(segment, segment, local_autocorr);
        double tmp = local_autocorr[0] - lastSegmentEndVal;
        for(int i = 0, sz = (int)local_autocorr.size(); i < sz; ++i)
            local_autocorr[i] -= tmp;
        
            //Define the segment length
        
            // a. Shine-step-counting style
        findpeaks(local_autocorr, minPeakDistance, 0, max_peak_strengths, max_peak_locs);
        
        if(max_peak_locs.empty()){
            segment_length = (int)segment.size();
        }else{
            for(int i=0; i<(int) local_autocorr.size(); ++i) local_autocorr[i] = -local_autocorr[i];
            findpeaks(local_autocorr, minPeakDistance, 0, min_peak_strengths, min_peak_locs);
            
            if(min_peak_locs.empty()){
                segment_length = round((*max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size)/2.0 + 1); //round
                segment_length = min(segment_length, (int)segment.size());
            }else{
                segment_length = round((*max_element(max_peak_locs.begin(), max_peak_locs.end())
                                        + *max_element(min_peak_locs.begin(), min_peak_locs.end()))/2.0 + 1); //round
            }
            for(int i=0; i<(int) local_autocorr.size(); ++i) local_autocorr[i] = -local_autocorr[i];
        }
        
        
            // b. Equal-step progression
            // segment_length = window_size
        
            // c. autocorrelation
        int windowUpdate = int((1-overlap_ratio)*segment_length+0.5+1e-9);
        if (isFirstSegment) {
            for (int i = 0; i < windowStart; ++i)
                autocorrelation.push_back(0);
            isFirstSegment = false;
        }
        for(int i = 0, sz = min((int)local_autocorr.size(), windowUpdate); i < sz; ++i)
            autocorrelation.push_back(local_autocorr[i]);
        
            // Define the start of the next window
        windowStart = windowStart + windowUpdate;
        lastSegmentEndVal = autocorrelation[(int)autocorrelation.size() - 1];
    }
    
    
        // Step 2: perform peak-counting on the autocorrelation stream
    windowStart = firstSample-1;
    vector<pair<double, int>> heartBeats;
    vector<double> heartRates;
    isFirstSegment = true;
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
                segment_length = round((*max_element(max_peak_locs.begin(), max_peak_locs.end()) + window_size)/2.0 + 1); //round
                segment_length = min(segment_length, (int)segment.size());
            }else{
                segment_length = round((*max_element(max_peak_locs.begin(), max_peak_locs.end())
                                        + *max_element(min_peak_locs.begin(), min_peak_locs.end()))/2.0 + 1); //round
            }
            for(int i=0; i<(int) segment.size(); ++i) segment[i]=-segment[i];
        }
        
            // b. Equal_step progression
            // segment_length = window_size;
        
            // Record all beats in the window, even if there are duplicates
        for(int i=0; i<(int) max_peak_locs.size(); ++i)
            heartBeats.push_back(pair<double, int> (max_peak_strengths[i], max_peak_locs[i] + windowStart));
        
            // Calculate the HR for this window
        int windowUpdate = int((1-overlap_ratio)*segment_length+0.5+1e-9);
        if (isFirstSegment) {
            for (int i = 0; i < windowStart; ++i)
                heartRates.push_back(0);
            isFirstSegment = false;
        }
        
        int count = 0;
        for (int i = 0, sz = (int)segment.size(); i < sz; ++i)
            if (segment[i] > NaN && segment[i] < INFINITY)
                ++count;
        double rate = (double) max_peak_locs.size() / count * fr;
        
        for(int i = windowStart; i < windowStart+windowUpdate; ++i)
            heartRates.push_back(rate);
        
        windowStart = windowStart + windowUpdate;
        
    }
    
        //Prune the beats counted to include only unique ones
    heartBeats = unique_stable(heartBeats);
    
        //Calc the avg HR for the whole stream
    
    double avg_hr=0;
    if(!heartBeats.empty()){
        int cnt=0;
        for(int i=firstSample-1; i<(int)temporal_mean.size(); ++i)
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


