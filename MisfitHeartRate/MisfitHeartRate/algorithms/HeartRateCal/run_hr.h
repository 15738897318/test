//
//  run_hr.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__run_hr__
#define __MisfitHeartRate__run_hr__

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
#include "heartRate_calc.h"


namespace MHR {
    // run Heart Rate calculation
    hrResult run_hr(vector<Mat> &vid, String resultsDir, const String &vidType,
                    double min_hr, double max_hr,
                    double alpha, int level, double chromAtn);
}

#endif /* defined(__MisfitHeartRate__run_hr__) */
