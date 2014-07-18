//
//  temporal_mean_calc.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__temporal_mean_calc__
#define __MisfitHeartRate__temporal_mean_calc__

#include <iostream>

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "image.h"
#include "matrix.h"
#include "frames2signal.h"
#include "hb_counter_pda.h"
#include "hb_counter_autocorr.h"
#include "hr_signal_calc.h"


namespace MHR {
    vector<mTYPE> temporal_mean_calc(const vector<Mat> &vid, mTYPE overlap_ratio,
                                      mTYPE max_bpm, mTYPE cutoff_freq,
                                      int colour_channel, String colourspace,
                                      mTYPE &lower_range, mTYPE &upper_range, bool isCalcMode);
}

#endif /* defined(__MisfitHeartRate__temporal_mean_calc__) */
