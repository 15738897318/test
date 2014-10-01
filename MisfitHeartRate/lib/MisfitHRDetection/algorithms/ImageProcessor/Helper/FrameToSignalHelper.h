//
//  SimpleMeanFrameToSignalHelper.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 10/1/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef __MIsfitHRDetection__SimpleMeanFrameToSignalHelper__
#define __MIsfitHRDetection__SimpleMeanFrameToSignalHelper__

#include <iostream>
#include "AbstractFrameToSignalHelper.h"

class SimpleMeanFrameToSignalHelper: public AbstractFrameToSignalHelper {
public:
    void convert(std::vector<Mat> &eulerianVid, std::vector<double> &sig, double frameRate, bool isCalcMode);
};

class TrimmedMeanFrameToSignalHelper: public AbstractFrameToSignalHelper {
    int _trimmed_size;
public:
    void setFaceParams();
    void setFingerParams();
    void convert(std::vector<Mat> &eulerianVid, std::vector<double> &sig, double frameRate, bool isCalcMode);
};

class BalancedModeFrameToSignalHelper: public AbstractFrameToSignalHelper {
    double lower_range;
    double upper_range;
    double _training_time_start;    // seconds
    double _training_time_end;        // seconds
    int _number_of_bins;             // 50 * round(fr * training_time);
    double _pct_reach_below_mode;    // Percent
    double _pct_reach_above_mode;    // Percent
public:
    void setFaceParams();
    void setFingerParams();
    void convert(std::vector<Mat> &eulerianVid, std::vector<double> &sig, double frameRate, bool isCalcMode);
};

#endif /* defined(__MIsfitHRDetection__SimpleMeanFrameToSignalHelper__) */
