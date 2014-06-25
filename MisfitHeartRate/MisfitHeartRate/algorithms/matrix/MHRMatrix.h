//
//  MHRMatrix.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/24/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__MHRMatrix__
#define __MisfitHeartRate__MHRMatrix__

#include <iostream>

namespace cv {
    
    // sum all channels in one pixcel
    // default output type is double - CV_32FC1
    Mat sumChannels(const Mat &src);
    
    // create a new Mat with only one channel from old Mat
    // default output type is double - CV_32FC1
    Mat cloneWithChannel(const Mat &src, int channel);
    
    // atan2 of 2 Mats which have same size
    // default intput/output type is double - CV_32FC1
    Mat atan2Mat(const Mat &src1, const Mat &src2);
    
    // return src.^n
    // default intput/output type is double - CV_32FC1
    Mat powMat(const Mat &src, double n);
    
    // return a + b
    Mat add(const Mat &a, const Mat &b);
    
    // return a .* b
    Mat multiply(const Mat &a, const Mat &b);
    
    // return mat .* x
    Mat multiply(const Mat &a, double x);
}

#endif /* defined(__MisfitHeartRate__MHRMatrix__) */
