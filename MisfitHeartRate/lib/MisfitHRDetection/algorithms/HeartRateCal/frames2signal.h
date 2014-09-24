//
//  frames2signal.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.


#ifndef __MisfitHeartRate__frames2signal__
#define __MisfitHeartRate__frames2signal__

#include "matlab.h"

using namespace cv;
using namespace std;

namespace MHR {
    //!
    //! The function will convert the array of frames into an array of signal value (type double)
    //! note that the frame is mono channel.
    //! \param fr video's frame rate
    //! \param conversion_method we have 3 method for converting a frame into a double value:
    //! + simple-mean
    //! + trimmed-mean
    //! + mode-balance
    //!
    vector<double> frames2signal(const vector<Mat>& monoframes, const String &conversion_method,
                                 double fr, double cutoff_freq,
                                 double &lower_range, double &upper_range, bool isCalcMode);
}

#endif /* defined(__MisfitHeartRate__frame2signal__) */
