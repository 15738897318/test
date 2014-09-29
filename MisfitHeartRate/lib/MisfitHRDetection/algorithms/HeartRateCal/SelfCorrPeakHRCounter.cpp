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
}

double mean(const vector<double> &signal) {
    double sum = 0;
    for (int i = 0;i < signal.size();i++) {
        sum += signal[i];
    }
    return sum/signal.size();
}

void SelfCorrPeakHRCounter::corr_linear(vector<double> &signal, vector<double> &kernel, bool subtractMean ) {
    int m = (int)signal.size(), n = (int)kernel.size();
    
        // -meanValue
    if (subtractMean) {
        double meanValue = mean(signal);
        for (int i = 0; i < m; ++i) signal[i] -= meanValue;
        meanValue = mean(kernel);
        for (int i = 0; i < n; ++i) kernel[i] -= meanValue;
    }
    
        // padding of zeors
    for(int i = m; i < m+n-1; i++) signal.push_back(0);
    for(int i = n; i < m+n-1; i++) kernel.push_back(0);
    

    for(int i = 0; i < m+n-1; i++)
        {
        filtered_temporal_mean.push_back(0);
        for(int j = 0; j <= i; j++)
            filtered_temporal_mean[i] += signal[j]*kernel[i-j];
        }
    
    for (int i = 0; i < n-1; ++i)
        filtered_temporal_mean.pop_back();
    
    if (subtractMean) {
        double minValue = *min_element(filtered_temporal_mean.begin(), filtered_temporal_mean.end());
        if (minValue < 0)
            for (int i = 0, sz = (int)filtered_temporal_mean.size(); i < sz; ++i)
                filtered_temporal_mean[i] -= minValue;
    }
    
}

void SelfCorrPeakHRCounter::low_pass_filter(vector<double> &temporal_mean) {
    filtered_temporal_mean.clear();
    if (filtered_temporal_mean.capacity() != temporal_mean.size())
        filtered_temporal_mean.reserve(temporal_mean.size());
    
    assert(temporal_mean.size() ==  filtered_temporal_mean.size());
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
    corr_linear(temporal_mean, kernel, false);
    
        // assign values in all old NaN positions to NaN
    for (int i = 0, sz = (int)nAnPositions.size(); i < sz; ++i)
        filtered_temporal_mean[nAnPositions[i]] = NaN;
    
        // remove first _beatSignalFilterKernel_size/2 elements when use FilterBandPassing
    filtered_temporal_mean.erase(filtered_temporal_mean.begin(), filtered_temporal_mean.begin()+ _beatSignalFilterKernel_size/2);
}