//
//  matrix.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "matrix.h"


namespace MHR {
	// return a vector of integer from a to b with specific step
	vector<int> vectorRange(int a, int b, int step) {
		vector<int> ans;
		for (int i = a; i <= b; i += step)
			ans.push_back(i);
		return ans;
	}
    
    
    // import data from a array to a Mat
    Mat arrayToMat(const double a[], int rows, int cols) {
		Mat ans = Mat::zeros(rows, cols, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                ans.at<double>(i, j) = a[i*cols + j];
        return ans;
    }
    
    
    // vector to Mat
    Mat vectorToMat(const vector<double>& arr){
        int sz = (int)arr.size();
        Mat ans = Mat::zeros(1, sz, CV_64F);
        for(int i = 0; i < sz; ++i)
            ans.at<double>(0, i) = arr[i];
        return ans;
    }
    
    // Mat to vector 1D (just get the first row)
    vector<double> matToVector1D(const Mat &m) {
        vector<double> arr;
        for(int i = 0; i < m.cols; ++i)
            arr.push_back(m.at<double>(0, i));
        return arr;
    }


    // read frames from a VideoCapture to a vector<Mat>
    // return true if endOfFile
    bool videoCaptureToVector(VideoCapture &src, vector<Mat> &dst, int nFrames)
    {
        Mat frame;
        int c = (int)dst.size(), old_c = c;
        while(nFrames == -1 || c++ < nFrames) {
            src >> frame;
            if (frame.empty())
                return (c == old_c+1);
            dst.push_back(frame.clone());
        }
        return false;
    }
    
	
	// atan2 of 2 Mats which have same size
	// default intput/output type is double - CV_64F
	Mat atan2Mat(const Mat &src1, const Mat &src2)
	{
		int nRow = src1.rows, nCol = src1.cols;
		Mat ans = Mat::zeros(nRow, nCol, CV_64F);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
				ans.at<double>(i, j) += atan2(src1.at<double>(i, j), src2.at<double>(i, j));
		return ans;
	}
	
	
	// return src.^n
	// default intput/output type is double - CV_64F
	Mat powMat(const Mat &src, double n)
	{
		Mat ans = Mat::zeros(src.rows, src.cols, CV_64F);
		pow(src, n, ans);
		return ans;
	}
	
	
	// return a + b
	Mat add(const Mat &a, const Mat &b)
	{
		Mat ans = Mat::zeros(a.rows, a.cols, a.type());
		add(a, b, ans);
		return ans;
	}
	
	
	// return a .* b
	Mat multiply(const Mat &a, const Mat &b)
	{
		Mat ans = Mat::zeros(a.rows, a.cols, a.type());
		multiply(a, b, ans);
		return ans;
	}
	
	
	// return mat .* x
	Mat multiply(const Mat &a, double x)
	{
		Mat ans = Mat::zeros(a.rows, a.cols, a.type());
		multiply(a, Mat(a.rows, a.cols, CV_64F, x), ans);
		return ans;
	}
}