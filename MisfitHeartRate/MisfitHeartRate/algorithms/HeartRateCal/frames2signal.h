//
//  frames2signal.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.


#ifndef __MisfitHeartRate__frames2signal__
#define __MisfitHeartRate__frames2signal__

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "matrix.h"
#include "matlab.h"
#include "config.h"

using namespace cv;
using namespace std;


namespace MHR {
    vector<mTYPE> frames2signal(const vector<Mat>& monoframes, const String &conversion_method,
                                 mTYPE fr, mTYPE cutoff_freq,
                                 mTYPE &lower_range, mTYPE &upper_range, bool isCalcMode);
}

#endif /* defined(__MisfitHeartRate__frame2signal__) */
