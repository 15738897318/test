//
//  image.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "image.h"


namespace MHR {
	// convert a RGB Mat to a TSL Mat
    // rgbmap is a CV_64F Mat
	void rgb2tsl(const Mat& rgbmap, Mat &dst)
	{
		int nRow = rgbmap.rows;
		int nCol = rgbmap.cols;
        int nChannel = rgbmap.channels();
        
        Mat rgb_sumchannels = Mat::zeros(nRow, nCol, CV_64F);
        Mat rgb_channel[3] = {Mat::zeros(nRow, nCol, CV_64F), Mat::zeros(nRow, nCol, CV_64F), Mat::zeros(nRow, nCol, CV_64F)};
        for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
                for (int channel = 0; channel < nChannel; ++channel) {
                    rgb_sumchannels.at<double>(i, j) += rgbmap.at<Vec3d>(i, j)[channel];
                    rgb_channel[channel].at<double>(i, j) = rgbmap.at<Vec3d>(i, j)[channel];
                }

		Mat r_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(rgb_channel[0], rgb_sumchannels, r_primes);
		r_primes = r_primes - Mat(nRow, nCol, CV_64F, cvScalar(1.0/3.0));
        
		Mat g_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(rgb_channel[1], rgb_sumchannels, g_primes);
		g_primes = g_primes - Mat(nRow, nCol, CV_64F, cvScalar(1.0/3.0));
        
		Mat temp1 = Mat::zeros(nRow, nCol, CV_64F);
		Mat temp2 = Mat::ones(nRow, nCol, CV_64F);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
				if (g_primes.at<double>(i, j) > 0)
				{
					temp1.at<double>(i, j) = 1.0/4.0;
				}
				else if (g_primes.at<double>(i, j) < 0)
				{
					temp1.at<double>(i, j) = 3.0/4.0;
				}
				else
				{
					temp2.at<double>(i, j) = 0;
				}
        

        dst = Mat::zeros(nRow, nCol, CV_64FC3);
		Mat tmp0 = atan2Mat(r_primes, g_primes);
		multiply(tmp0, Mat(nRow, nCol, CV_64F, cvScalar(1.0/(2*M_PI))), tmp0);
		multiply(tmp0, temp2, tmp0);
		tmp0 = tmp0 + temp1;
        
		Mat tmp1 = powMat(r_primes, 2);
		tmp1 = tmp1 + powMat(g_primes, 2);
		multiply(tmp1, Mat(nRow, nCol, CV_64F, cvScalar(9.0/5.0)), tmp1);
		pow(tmp1, 0.5, tmp1);
        
		Mat tmp2 = add(multiply(rgb_channel[0], 0.299),
					   multiply(rgb_channel[1], 0.587));
		tmp2 = tmp2 + multiply(rgb_channel[2], 0.114);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
			{
				dst.at<Vec3d>(i, j)[0] = tmp0.at<double>(i, j);
				dst.at<Vec3d>(i, j)[1] = tmp1.at<double>(i, j);
				dst.at<Vec3d>(i, j)[2] = tmp2.at<double>(i, j);
			}
	}


    // Blur and downsample an image.  The blurring is done with
    // filter kernel specified by FILT (default = 'binom5')
    void blurDnClr(const Mat& src, Mat &dst, int level) {
        dst = src.clone();
        for (int i = 0; i < level; ++i) {
            int nRow = dst.rows/2 + int(dst.rows%2 > 0);
            int nCol = dst.cols/2 + int(dst.cols%2 > 0);
            pyrDown(dst, dst, Size(nCol, nRow));
        }
    }


    // Compute correlation of matrices IM with FILT, followed by
    // downsampling.  These arguments should be 1D or 2D matrices, and IM
    // must be larger (in both dimensions) than FILT.  The origin of filt
    // is assumed to be floor(size(filt)/2)+1.
    void corrDn(const Mat &src, Mat &dst, const Mat &filter, int rectRow, int rectCol)
    {
        Mat tmp;
        filter2D(src, tmp, -1, filter);
        int m = tmp.rows/rectRow + (tmp.rows%rectRow > 0);
        int n = tmp.cols/rectCol + (tmp.cols%rectCol > 0);
        dst = Mat::zeros(m, n, CV_64F);
        int last_i = -1, last_j = -1;
        for (int i = 0, x = 0; x < src.rows; ++i, x += rectRow)
            for (int j = 0, y = 0; y < src.cols; ++j, y += rectCol) {
                dst.at<double>(i, j) = tmp.at<double>(x, y);
                last_i = max(last_i, i);
                last_j = max(last_j, j);
            }
        if (_DEBUG_MODE)
            if (last_i+1 != m && last_j+1 != n)
                printf("Error: last_i = %d, last_j = %d, m = %d, n = %d,", last_i, last_j, m, n);
    }
}