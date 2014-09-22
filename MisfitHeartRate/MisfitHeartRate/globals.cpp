//
//  globals.cpp
//  Pulsar
//
//  Created by Thanh Le on 8/14/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#include "globals.h"

MHR::hrResult hrGlobalResult;
MHR::hrResult hrOldGlobalResult;

Mat firstFrameWithFace;

cv::Rect cropArea;
cv::Rect ROI_upper;
cv::Rect ROI_lower;

bool fastMode;