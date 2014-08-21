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
     * \param a,b which have the same size \n
     * \return a new Mat which each element[i, j] = atan2(a[i, j], b[i, j]) \n
     * Data type: CV_64F
     */
	Mat atan2Mat(const Mat &a, const Mat &b);
	
    
    /**
     * \return the \a n-th power of \a src (element-wise) \n
     * Data type: CV_64F
     */
	Mat powMat(const Mat &src, double n);

	
    /**
     * \return a + b
     */
	Mat add(const Mat &a, const Mat &b);
	
    
    /**
     * \return a * b (element-wise)
     */
	Mat multiply(const Mat &a, const Mat &b);
	
    
    /**
     * \return a new Mat which each element[i, j] = a[i, j] * x \n
     * Data type: CV_64F
     */
	Mat multiply(const Mat &a, double x);
}

#endif /* defined(__MisfitHeartRate__matrix__) */
