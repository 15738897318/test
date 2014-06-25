//
//  MHRMatrix.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/24/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "MHRMatrix.h"


namespace cv {
    
    // sum all channels in one pixcel
    // default output type is double - CV_32FC1
    Mat sumChannels(const Mat &src)
    {
        int nRow = src.rows, nCol = src.cols;
        int nChannel = src.channels();
        Mat sum = Mat::zeros(nRow, nCol, CV_32FC1);
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
                for (int k = 0; k < nChannel; ++k)
                    sum.at<double>(i, j) += src.at<Vec3d>(i, j)[k];
        return sum;
    }
    
    
    // create a new Mat with only one channel from old Mat
    // default output type is double - CV_32FC1
    Mat cloneWithChannel(const Mat &src, int channel)
    {
        int nRow = src.rows, nCol = src.cols;
        Mat ans = Mat::zeros(nRow, nCol, CV_32FC1);
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
                ans.at<double>(i, j) = src.at<Vec3d>(i, j)[channel];
        return ans;
    }
    
    
    // atan2 of 2 Mats which have same size
    // default intput/output type is double - CV_32FC1
    Mat atan2Mat(const Mat &src1, const Mat &src2)
    {
        int nRow = src1.rows, nCol = src1.cols;
        Mat ans = Mat::zeros(nRow, nCol, CV_32FC1);
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
                ans.at<double>(i, j) += atan2(src1.at<double>(i, j), src2.at<double>(i, j));
        return ans;
    }
    
    
    // return src.^n
    // default intput/output type is double - CV_32FC1
    Mat powMat(const Mat &src, double n)
    {
        Mat ans;
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
        multiply(a, Mat(a.rows, a.cols, CV_32FC1, x), ans);
        return ans;
    }
}
