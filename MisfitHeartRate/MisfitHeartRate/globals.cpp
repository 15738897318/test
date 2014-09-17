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

cv::Rect rightEye;
cv::Rect leftEye;
cv::Rect mouth;
cv::Rect ROI_upper, cropArea, ROI_lower;
cv::Rect faceCropArea[2];

int nFaces;
//int currentFace;