//
//  frames2signal.h
//  Pulsar
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.


#ifndef __Pulsar__frames2signal__
#define __Pulsar__frames2signal__

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
    vector<double> frames2signal(const vector<Mat>& monoframes, const String &conversion_method,
                                 double fr, double cutoff_freq,
                                 double &lower_range, double &upper_range, bool isCalcMode);
}

#endif /* defined(__Pulsar__frame2signal__) */
