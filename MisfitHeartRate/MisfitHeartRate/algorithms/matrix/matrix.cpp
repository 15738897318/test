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
    Mat arrayToMat(const mTYPE a[], int rows, int cols) {
		Mat ans = Mat::zeros(rows, cols, mCV_F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                ans.at<mTYPE>(i, j) = a[i*cols + j];
        return ans;
    }
    
    
    // vector to Mat
    Mat vectorToMat(const vector<mTYPE>& arr){
        int sz = (int)arr.size();
        Mat ans = Mat::zeros(1, sz, mCV_F);
        for(int i = 0; i < sz; ++i)
            ans.at<mTYPE>(0, i) = arr[i];
        return ans;
    }
    
    // Mat to vector 1D (just get the first row)
    vector<mTYPE> matToVector1D(const Mat &m) {
        vector<mTYPE> arr;
        for(int i = 0; i < m.cols; ++i)
            arr.push_back(m.at<mTYPE>(0, i));
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
            
            if (c == old_c+1)
                printf("nChannel = %d\n", frame.channels());
            
            cvtColor(frame, frame, CV_BGR2RGB);
            dst.push_back(frame.clone());
        }
        return false;
    }
    
	
	// atan2 of 2 Mats which have same size
	// default intput/output type is mTYPE - mCV_F
	Mat atan2Mat(const Mat &src1, const Mat &src2)
	{
		int nRow = src1.rows, nCol = src1.cols;
		Mat ans = Mat::zeros(nRow, nCol, mCV_F);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
				ans.at<mTYPE>(i, j) += atan2(src1.at<mTYPE>(i, j), src2.at<mTYPE>(i, j));
		return ans;
	}
	
	
	// return src.^n
	// default intput/output type is mTYPE - mCV_F
	Mat powMat(const Mat &src, mTYPE n)
	{
		Mat ans = Mat::zeros(src.rows, src.cols, mCV_F);
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
	Mat multiply(const Mat &a, mTYPE x)
	{
		Mat ans = Mat::zeros(a.rows, a.cols, a.type());
		multiply(a, Mat(a.rows, a.cols, mCV_F, x), ans);
		return ans;
	}
    
    Mat read2DMatFromFile(FILE* &file, int rows, int cols)
    {
        Mat ans = Mat::zeros(rows, cols, mCV_F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                fscanf(file, "%lf", &ans.at<mTYPE>(i, j));
        return ans;
    }
    
    vector<mTYPE> readVectorFromFile(FILE* &file, int n)
    {
        vector<mTYPE> ans;
        mTYPE value;
        for (int i = 0; i < n; ++i) {
            fscanf(file, "%lf", &value);
            ans.push_back(value);
        }
        return ans;
    }
    
    
    int readInt(FILE* &file)
    {
        int value;
        fscanf(file, "%d", &value);
        return value;
    }
    
    
    mTYPE readmTYPE(FILE* &file)
    {
        mTYPE value;
        fscanf(file, "%lf", &value);
        return value;
    }
}