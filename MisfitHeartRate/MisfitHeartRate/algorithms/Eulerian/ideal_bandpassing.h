//
//  ideal_bandpassing.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/8/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__ideal_bandpassing__
#define __MisfitHeartRate__ideal_bandpassing__

#include "config.h"
#include "matlab.h"
#include "image.h"

using namespace cv;
using namespace std;

namespace MHR {
    // Apply ideal band pass filter on SRC
    // WL: lower cutoff frequency of ideal band pass filter
    // WH: higher cutoff frequency of ideal band pass filter
    // SAMPLINGRATE: sampling rate of SRC
    void ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, double wl, double wh, double samplingRate);
}

#endif /* defined(__MisfitHeartRate__ideal_bandpassing__) */
