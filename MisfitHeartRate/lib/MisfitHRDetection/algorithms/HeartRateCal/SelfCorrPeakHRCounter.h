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
    double threshold_fraction;

    std::vector<double> filtered_temporal_mean;

    
    /**
     * filter function for frames2signal function, apply low pass filter on vector \a arr.
     * \ref: http://en.wikipedia.org/wiki/Low-pass_filter
     */
    void low_pass_filter(vector<double> &arr);
    
    /**
     * Calculate the heart-rate from a list of heart-beat positions. \n
     * \param ans
     *  + the first number is average heart-rate
     *  + the second number is mode of the instantaneous heart-rates multiply with frameRate*60
     */
	void hr_calculator(const vector<int> &heartBeatPositions, double frameRate, vector<double> &ans);
    /*!
     This function will convert the signal array (after using the frame2signal() function) to an array of heart beats' position
     The function will shift a window with size \a window_size from \a firstSample position to calculate the heart beats in that window. \n
     \param fr the frame rate.
     \param overlap_ratio the ratio of the next window will be identical with the current window, at default this ratio value is 0
     \param minPeakDistance,threshold these arguments are for the findPeaks function.
     */
    vector<int> hb_counter_pda(vector<double> temporal_mean, double fr, int firstSample, int window_size,
                               double overlap_ratio, double minPeakDistance, double threshold, MHR::hrDebug& debug);
    /*!
     This function will convert the signal array (after using the frame2signal function) to an autocorelation array and then convert to an array of heart beats' position. \n
     The function will shift a window with size \a window_size from \a firstSample position to calculate the heart beats in that window. \n
     This function is different from the hb_counter_pda function, instead of calculating the heart beats directly from the signal array, we will first convert the signal array to an autocorrelation array (\ref: http://en.wikipedia.org/wiki/Autocorrelation) then use this array to calculate the heart beats. \n
     \param fr the frame rate.
     \param overlap_ratio the ratio of the next window will be identical with the current window, at default this ratio value is 0
     \param minPeakDistance,threshold these arguments are for the findPeaks function.
     */
    vector<int> hb_counter_autocorr(vector<double> &temporal_mean, double fr, int firstSample,
                                    int window_size, double overlap_ratio, double minPeakDistance, MHR::hrDebug& debug);
        //!
        //! findpeaks in \a segment, with \a minPeakDistance and \a threshold,\n
        //! complexity O(n^2) with n = number of peaks
        //! \param segment a vector of signals
        //! \param minPeakDistance minimum distance between two peaks
        //! \param threshold the minimum value that a peak point should be larger than its two neighbors point
        //! \return \a max_peak_strengths
        //! \return \a max_peak_locs
        //!

    void findpeaks(const vector<double> &segment, double minPeakDistance, double threshold,
                   vector<double> &max_peak_strengths, vector<int> &max_peak_locs);
public:
    SelfCorrPeakHRCounter();
    ~SelfCorrPeakHRCounter();
    static void setFaceParameters();
    static void setFingerParameter();
    MHR::hrResult getHR(vector<double> &temporal_mean);
#ifdef DEBUG
    const vector<double> &getTemporalMeanFilt() {
        return filtered_temporal_mean;
    };
#endif
};

#endif /* defined(__MIsfitHRDetection__SelfCorrPeakHRCounter__) */
