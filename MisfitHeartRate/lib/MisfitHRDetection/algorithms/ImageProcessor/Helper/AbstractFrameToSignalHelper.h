//
//  FrameToSignalHelper.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 10/1/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef __MIsfitHRDetection__FrameToSignalHelper__
#define __MIsfitHRDetection__FrameToSignalHelper__

#include <iostream>
#include "config.h"

class AbstractFrameToSignalHelper{
    double _cutoff_freq;
public:
    virtual void setFaceParams() {
    };
    virtual void setFingerParams() {
    };
    virtual void convert(std::vector<Mat> &eulerianVid, std::vector<double> &signal, double frameRate, bool isCalcMode) = 0;
};

#endif /* defined(__MIsfitHRDetection__FrameToSignalHelper__) */
