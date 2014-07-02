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
#include "MHRImage.h"
#include "MHRMath.h"
#include "MHRMatrix.h"
#include "heartRate_calc.h"
//#import <opencv2/highgui/ios.h>
//#import <opencv2/highgui/cap_ios.h>


namespace cv {
    
    // run Heart Rate calculation
    void run_hr(vector<Mat> &vid, String resultsDir,
                double min_hr, double max_hr,
                double alpha, int level, double chromAtn);
}

#endif /* defined(__MisfitHeartRate__run_hr__) */
