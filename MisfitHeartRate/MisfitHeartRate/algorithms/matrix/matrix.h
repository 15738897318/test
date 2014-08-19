//
//  matrix.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__matrix__
#define __MisfitHeartRate__matrix__

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "config.h"

using namespace cv;
using namespace std;


namespace MHR {
	// return a vector of integer from a to b with specific step
	vector<int> vectorRange(int a, int b, int step = 1);
    
    // data from vector 1D to Mat
    Mat vectorToMat(const vector<double>& arr);
    
    // data from Mat to vector 1D
    vector<double> matToVector1D(const Mat &m);
	
	// atan2 of 2 Mats which have same size
	// default intput/output type is double - CV_64F
	Mat atan2Mat(const Mat &src1, const Mat &src2);
	
	// return src.^n
	// default intput/output type is double - CV_64F
	Mat powMat(const Mat &src, double n);
	
	// return a + b
	Mat add(const Mat &a, const Mat &b);
	
	// return a .* b
	Mat multiply(const Mat &a, const Mat &b);
	
	// return mat .* x
	Mat multiply(const Mat &a, double x);
}

#endif /* defined(__MisfitHeartRate__matrix__) */
