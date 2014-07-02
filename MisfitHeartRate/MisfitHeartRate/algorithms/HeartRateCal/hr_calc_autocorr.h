//
//  hr_calc_autocorr.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hr_calc_autocorr__
#define __MisfitHeartRate__hr_calc_autocorr__



#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "MHRImage.h"
#include "MHRMath.h"
#include "MHRMatrix.h"
#include "hrDebug.h"
#include "matlab.h"

using namespace std;
using namespace cv;

double hr_calc_autocorr(vector<double> temporal_mean, double fr, int firstSample,
                        int window_size, double overlap_ratio, double minPeakDistance);

#endif /* defined(__MisfitHeartRate__hr_calc_autocorr__) */
