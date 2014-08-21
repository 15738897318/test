//
//  hb_counter_autocorr.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hb_counter_autocorr__
#define __MisfitHeartRate__hb_counter_autocorr__

#include "image.h"
#include "matlab.h"
#include "hr_structures.h"

using namespace std;
using namespace cv;

namespace MHR {
    /*!
     This function will convert the signal array (after using the frame2signal function) to an autocorelation array and then convert to an array of heart beats' position. \n
     The function will shift a window with size \a window_size from \a firstSample position to calculate the heart beats in that window. \n
     This function is different from the hb_counter_pda function, instead of calculating the heart beats directly from the signal array, we will first convert the signal array to an autocorrelation array (\ref: http://en.wikipedia.org/wiki/Autocorrelation) then use this array to calculate the heart beats. \n
     \param fr the frame rate.
     \param overlap_ratio the ratio of the next window will be identical with the current window, at default this ratio value is 0
     \param minPeakDistance,threshold these arguments are for the findPeaks function.
     */
    vector<int> hb_counter_autocorr(vector<double> &temporal_mean, double fr, int firstSample,
                            int window_size, double overlap_ratio, double minPeakDistance, hrDebug& debug);
}

#endif /* defined(__MisfitHeartRate__hb_counter_autocorr__) */
