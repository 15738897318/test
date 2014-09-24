//
//  temporal_mean_calc.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__temporal_mean_calc__
#define __MisfitHeartRate__temporal_mean_calc__

#include "image.h"
#include "frames2signal.h"
#include "hb_counter_pda.h"
#include "hb_counter_autocorr.h"
#include "hr_signal_calc.h"


namespace MHR {
    /**
     * Convert frames of <vid> to signals.
     * \param vid data type is CV_64FC3 or CV_64F
     * \param overlap_ratio overlap ratio between 2 consecutive segments
     * \param max_bpm maximum heart rate detectable (use in determining minPeaksDistance in findpeaks())
     * \param colour_channel if in _THREE_CHAN_MODE, then convert all frames of \a vid to
     *  monoframes by select only one channel of each frame.
     * \param colourspace if in _THREE_CHAN_MODE, then convert colourspace of
     *  all frames of \a vid to "hsv", "ycbcr" or "tsl" before converting them to monoframes
     * \param cutoff_freq,lower_range,upper_range>,isCalcMode: see frames2signal()
     */
    vector<double> temporal_mean_calc(const vector<Mat> &vid, double overlap_ratio,
                                      double max_bpm, double cutoff_freq,
                                      int colour_channel, String colourspace,
                                      double &lower_range, double &upper_range, bool isCalcMode);
}

#endif /* defined(__MisfitHeartRate__temporal_mean_calc__) */
