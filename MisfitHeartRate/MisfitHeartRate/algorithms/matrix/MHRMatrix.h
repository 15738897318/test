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
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;


namespace cv {
	// return a vector of integer from a to b with specific step
	vector<int> vectorRange(int a, int b, int step = 1);
    
    // import data from a array to a Mat
    Mat arrayToMat(double a[], int rows, int cols);
    
	// convert a Mat to another type Mat
	Mat convertTo(const Mat &src, int type, double alpha = 1.0, double beta = 0.0);
    
    // convert a VideoCapture to vector<Mat>
    vector<Mat> videoCaptureToVector(VideoCapture &src);
    
	// sum all channels in one pixcel
	// default output type is double - CV_64F
	Mat sumChannels(const Mat &src);
	
	// create a new Mat with only one channel from old Mat
	// default output type is double - CV_64F
	Mat cloneWithChannel(const Mat &src, int channel);
	
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
	
	// allcomb(A1,A2,A3,...,AN) returns all combinations of the elements in A1, A2, ..., and AN.
	// B is P-by-N matrix is which P is the product of the number of elements of the N inputs.
	vector<vector<int>> allcomb(std::vector<vector<int>> a);
}

#endif /* defined(__MisfitHeartRate__MHRMatrix__) */
