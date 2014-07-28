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
            
            cvtColor(frame, frame, CV_BGR2RGB);
            
            if (THREE_CHAN_MODE)
            	dst.push_back(frame.clone());
            else {
				/*-----------------if using 1-chan mode, then do the colour conversion here (0)-----------------*/
				frame.convertTo(frame, CV_64FC3);
				if (_colourspace == "hsv")
					cvtColor(frame, frame, CV_RGB2HSV);
				else if (_colourspace == "ycbcr")
					cvtColor(frame, frame, CV_RGB2YCrCb);
//				else if (_colourspace == "tsl")
//					rgb2tsl(frame, frame);
				
				Mat tmp = Mat::zeros(frame.rows, frame.cols, CV_64F);
				for (int i = 0; i < frame.rows; ++i)
					for (int j = 0; j < frame.cols; ++j)
						tmp.at<double>(i, j) = frame.at<Vec3d>(i, j)[_channels_to_process];
				dst.push_back(tmp.clone());
            }
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
        multiply(a, Mat(a.rows, a.cols, CV_64F, x), ans, 1, -1);
		return ans;
	}
    
    Mat read2DMatFromFile(FILE* &file, int rows, int cols)
    {
        Mat ans = Mat::zeros(rows, cols, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                fscanf(file, "%lf", &ans.at<double>(i, j));
        return ans;
    }
    
    vector<double> readVectorFromFile(FILE* &file, int n)
    {
        vector<double> ans;
        double value;
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
    
    
    double readDouble(FILE* &file)
    {
        double value;
        fscanf(file, "%lf", &value);
        return value;
    }
    
    
    void writeVector(const vector<double>& src, const String& outFile, bool append)
    {
        printf("Write vector to file %s\n", outFile.c_str());
        FILE *file;
        if (append) file = fopen(outFile.c_str(), "a");
        else file = fopen(outFile.c_str(), "w");
        int n = (int)src.size();
        fprintf(file, "\n");
        fprintf(file, "size = %d\n", n);
        for (int i = 0; i < n; ++i)
            fprintf(file, "%lf, ", src[i]);
        fprintf(file, "\n");
        fclose(file);
    }
}