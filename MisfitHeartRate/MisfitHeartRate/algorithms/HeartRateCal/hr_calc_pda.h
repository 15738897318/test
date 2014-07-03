//
//  hr_calc_pda.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hr_calc_pda__
#define __MisfitHeartRate__hr_calc_pda__

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "image.h"
#include "hrDebug.h"
#include "matlab.h"
#include "config.h"

using namespace std;
using namespace cv;


namespace MHR {
    double hr_calc_pda(vector<double> temporal_mean, double fr, int firstSample, int window_size, double overlap_ratio, double minPeakDistance, double threshold, hrDebug& debug);
}

#endif /* defined(__MisfitHeartRate__hr_calc_pda__) */
