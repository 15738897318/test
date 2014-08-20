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
     * <vid>: CV_64FC3 or CV_64F
     * <overlap_ratio>:
     * <max_bpm>:
     * <colour_channel>: if in _THREE_CHAN_MODE, then convert all frames of <vid> to
     *  monoframes by select only one channel of each frame.
     * <colourspace>: if in _THREE_CHAN_MODE, then convert colourspace of
     *  all frames of <vid> to "hsv", "ycbcr" or "tsl" before converting them to monoframes
     * <cutoff_freq>, <lower_range>, <upper_range>, <isCalcMode>: see frame2signal()
     */
    vector<double> temporal_mean_calc(const vector<Mat> &vid, double overlap_ratio,
                                      double max_bpm, double cutoff_freq,
                                      int colour_channel, String colourspace,
                                      double &lower_range, double &upper_range, bool isCalcMode);
}

#endif /* defined(__MisfitHeartRate__temporal_mean_calc__) */
