//
//  ImageUtilities.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/24/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__ImageUtilities__
#define __MisfitHeartRate__ImageUtilities__

#include <iostream>
#include "Matrix.h"

namespace cv {

    // convert a RGB Mat to a TSL Mat
    Mat rgb2tsl(const Mat& srcRGBmap);
}

#endif /* defined(__MisfitHeartRate__ImageUtilities__) */


