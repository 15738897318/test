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
    vector<int> hb_counter_autocorr(vector<double> &temporal_mean, double fr, int firstSample,
                            int window_size, double overlap_ratio, double minPeakDistance, hrDebug& debug);
}

#endif /* defined(__MisfitHeartRate__hb_counter_autocorr__) */
