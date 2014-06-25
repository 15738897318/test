//
//  MHRImage.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/24/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__MHRImage__
#define __MisfitHeartRate__MHRImage__

#include <iostream>
#include "MHRMatrix.h"

namespace cv {

    // convert a RGB Mat to a TSL Mat
    Mat rgb2tsl(const Mat& srcRGBmap);
}

#endif /* defined(__MisfitHeartRate__MHRImage__) */


