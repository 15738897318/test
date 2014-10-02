//
//  matrix.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "matrix.h"


namespace MHR {
        /* 
         Reimplement atan following this
         http://www.embedded.com/design/other/4216719/Performing-efficient-arctangent-approximation
         */
	Mat atan2Mat(const Mat &a, const Mat &b)
	{
		int nRow = a.rows, nCol = b.cols;
		Mat ans = Mat::zeros(nRow, nCol, CV_64F);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
				ans.at<double>(i, j) += atan2(a.at<double>(i, j), b.at<double>(i, j));
		return ans;
	}
	
	
	Mat powMat(const Mat &src, double n)
	{
		Mat ans = Mat::zeros(src.rows, src.cols, CV_64F);
		pow(src, n, ans);
		return ans;
	}
	
	
	Mat add(const Mat &a, const Mat &b)
	{
		Mat ans = Mat::zeros(a.rows, a.cols, a.type());
		add(a, b, ans);
		return ans;
	}
	
	
	Mat multiply(const Mat &a, const Mat &b)
	{
		Mat ans = Mat::zeros(a.rows, a.cols, a.type());
		multiply(a, b, ans);
		return ans;
	}
	
	
	Mat multiply(const Mat &a, double x)
	{
		Mat ans = Mat::zeros(a.rows, a.cols, a.type());
        multiply(a, Mat(a.rows, a.cols, CV_64F, x), ans, 1, -1);
		return ans;
	}
}