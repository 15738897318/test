//
//  globals.h
//  Pulsar
//
//  Created by Thanh Le on 8/14/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#ifndef __Pulsar__globals__
#define __Pulsar__globals__

#include <iostream>
#include "hr_signal_calc.h"

extern MHR::hrResult hrGlobalResult;
extern MHR::hrResult hrOldGlobalResult;

extern Mat firstFrameWithFace;

extern cv::Rect leftEye;
extern cv::Rect rightEye;
extern cv::Rect mouth;
extern cv::Rect ROI_upper, cropArea, ROI_lower;

#endif /* defined(__Pulsar__globals__) */
