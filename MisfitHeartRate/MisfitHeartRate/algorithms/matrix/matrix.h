//
//  matrix.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__matrix__
#define __MisfitHeartRate__matrix__

#include "config.h"

using namespace cv;
using namespace std;


namespace MHR {
    /**
     * input: two 2D Mats a, b which have the same size
     * output: a new Mat which each element[i, j] = atan2(a[i, j], b[i, j])
     * data type: CV_64F
     */
	Mat atan2Mat(const Mat &a, const Mat &b);
	
    
    /**
     * return the n-th power of src Mat (element-wise)
     * data type: CV_64F
     */
	Mat powMat(const Mat &src, double n);

	
    /**
     * return a + b
     */
	Mat add(const Mat &a, const Mat &b);
	
    
    /**
     * return a * b (element-wise)
     */
	Mat multiply(const Mat &a, const Mat &b);
	
    
    /**
     * return a new Mat which each element[i, j] = a[i, j] * x
     * data type: CV_64F
     */
	Mat multiply(const Mat &a, double x);
}

#endif /* defined(__MisfitHeartRate__matrix__) */
