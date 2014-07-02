//
//  frames2signal.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__frames2signal__
#define __MisfitHeartRate__frames2signal__

#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "MHRMath.h"
#include "MHRMatrix.h"

namespace cv {
    vector<double> frames2signal(const Mat& monoframes, String conversion_method, double frameRate, double cutoff_freq);
}

#endif /* defined(__MisfitHeartRate__frame2signal__) */
