//
//  SelfCorrPeakHRCounter.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/26/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef __MIsfitHRDetection__SelfCorrPeakHRCounter__
#define __MIsfitHRDetection__SelfCorrPeakHRCounter__

#include "AbstractHRCounter.h"
#include <vector>
#include "config.h"

class SelfCorrPeakHRCounter: public AbstractHRCounter {
private:
    static double _time_lag;              // seconds
    static double _max_bpm;             // BPM
    static double _window_size_in_sec;
    static double _overlap_ratio;
    
    static int _beatSignalFilterKernel_size;
    static Mat _beatSignalFilterKernel;
    
    int window_size;
    int firstSample;

    std::vector<double> filtered_temporal_mean;
    /*!
     * \ref: http://www.cs.cornell.edu/courses/CS1114/2013sp/sections/S06_convolution.pdf
     * \return 1D convolution operation of 2 vectors signal and kernel
     * \param subtractMean if is true, then before all calculations,
     * each elements of the signal vector will be subtracted by mean(\a signal),
     * and each elements of the kernel vector will be subtracted by mean(\a kernel).
     */
    void corr_linear(std::vector<double> &signal, std::vector<double> &kernel, bool subtractMean = true);
    
    /**
     * filter function for frames2signal function, apply low pass filter on vector \a arr.
     * \ref: http://en.wikipedia.org/wiki/Low-pass_filter
     */
public:
    void low_pass_filter(vector<double> &arr);
public:
    SelfCorrPeakHRCounter();
    ~SelfCorrPeakHRCounter();
    static void setFaceParameters();
    static void setFingerParameter();
    uint8_t getHR(vector<double> &temporal_mean);
    
    vector<double> &getTemporalMeanFilt() {
        return filtered_temporal_mean;
    };
};

#endif /* defined(__MIsfitHRDetection__SelfCorrPeakHRCounter__) */
