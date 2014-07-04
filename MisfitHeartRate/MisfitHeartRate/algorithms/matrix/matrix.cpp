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
    Mat arrayToMat(double a[], int rows, int cols) {
		Mat ans = Mat::zeros(rows, cols, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                ans.at<double>(i, j) = a[i*cols + j];
        return ans;
    }
    
    Mat arrayToMat(const double a[], int rows, int cols) {
		Mat ans = Mat::zeros(rows, cols, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                ans.at<double>(i, j) = a[i*cols + j];
        return ans;
    }
    
    
    // vector to Mat
    Mat vectorToMat(vector<double> arr){
        Mat ans = Mat::zeros(1, (int) arr.size(), CV_64F);
        for(int i=0; i<(int) arr.size(); ++i){
            ans.at<double>(0,i) = arr[i];
        }
        return ans;
    }
    
    // Mat to vector 1D (just get the first row)
    vector<double> matToVector1D(Mat m){
        vector<double> arr;
        for(int i=0; i<m.cols; ++i) arr.push_back(m.at<double>(0,i));
        return arr;
    }
    
    
	// convert a Mat to another type Mat
	Mat convertTo(const Mat &src, int type, double alpha, double beta) {
		Mat ans;
		src.convertTo(ans, type, alpha, beta);
		return ans;
	}
    
    
    // convert a VideoCapture to vector<Mat>
    vector<Mat> videoCaptureToVector(VideoCapture &src, int nFrames) {
        vector<Mat> ans;
        Mat frame;
        int c = 0;
        while(nFrames == -1 || c++ < nFrames) {
            printf("c = %d\n", c);
            src >> frame;
            if (frame.empty())
                break;
//            if (frame.rows > 256 || frame.cols > 256)
//                pyrDown(frame, frame, Size(frame.cols/2, frame.rows/2));
            ans.push_back(frame.clone());
        }
        return ans;
    }
    
    
    // read frames from a VideoCapture to a vector<Mat>
    // return true if endOfFile
    bool videoCaptureToVector(VideoCapture &src, vector<Mat> &dst, int nFrames)
    {
        Mat frame;
        int c = 0;
        while(nFrames == -1 || c++ < nFrames) {
            printf("c = %d\n", c);
            src >> frame;
            if (frame.empty())
                return (c == 1);
//            if (frame.rows > 256 || frame.cols > 256)
//                pyrDown(frame, frame, Size(frame.cols/2, frame.rows/2));
            dst.push_back(frame.clone());
        }
        return false;
    }
    
    
	// sum all channels in one pixcel
	// default output type is double - CV_64F
	Mat sumChannels(const Mat &src)
	{
		int nRow = src.rows, nCol = src.cols;
		int nChannel = src.channels();
		Mat sum = Mat::zeros(nRow, nCol, CV_64F);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
				for (int k = 0; k < nChannel; ++k)
					sum.at<double>(i, j) += src.at<Vec3d>(i, j)[k];
		return sum;
	}
    
    
	// create a new Mat with only one channel from old Mat
	// default output type is double - CV_64F
	Mat cloneWithChannel(const Mat &src, int channel)
	{
		int nRow = src.rows, nCol = src.cols;
		Mat ans = Mat::zeros(nRow, nCol, CV_64F);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
				ans.at<double>(i, j) = src.at<Vec3d>(i, j)[channel];
		return ans;
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
    
    
	// allcomb(A1, A2, A3, ..., AN) returns all combinations of the elements in A1, A2, ..., and AN.
	// B is P-by-N matrix is which P is the product of the number of elements of the N inputs.
	vector<vector<int>> allcomb(std::vector<vector<int>> a) {
		int p = 1, n = int(a.size());
		for (int i = 0; i < n; ++i)
			p = p*int(a[i].size());
        
		// generate all combinations of set a
		vector<vector<int>> ans;
		int id = 0;
		bool isPop = false;
		vector<int> stackPos;
		stackPos.push_back(0);
		while (!stackPos.empty()) {
			if (id == n) {
				vector<int> newElement;
				for (int i = 0; i < n; ++i)
					newElement.push_back(a[i][stackPos[i]]);
				ans.push_back(newElement);
				stackPos.pop_back();
				isPop = true;
				--id;
				continue;
			}
			int sz = int(a[id].size()), pos = stackPos[id];
			if (isPop) {
				++pos;
				stackPos.pop_back();
				if (pos == sz) --id;
				else {
					stackPos.push_back(pos);
					isPop = false;
				}
			}
			else {
				++id;
				stackPos.push_back(0);
			}
		}
		return ans;
	}
}