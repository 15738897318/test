//
//  image.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "image.h"

namespace MHR {
	void rgb2tsl(const Mat& rgbmap, Mat &dst)
	{
		int nRow = rgbmap.rows;
		int nCol = rgbmap.cols;
        
        Mat rgb_sumchannels;
    Mat rgb_channel[3];
    split(rgbmap,rgb_channel);
    rgb_sumchannels = rgb_channel[0] + rgb_channel[1] + rgb_channel[2];
                

//        r_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 1), sum(rgbmap, 3)), 1/3);
//        r_primes(isnan(r_primes)) = -1/3;
		Mat r_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(rgb_channel[0], rgb_sumchannels, r_primes);
		r_primes = r_primes - 1.0/3.0;
        
//        g_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 2), sum(rgbmap, 3)), 1/3);
//        g_primes(isnan(g_primes)) = -1/3;
		Mat g_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(rgb_channel[1], rgb_sumchannels, g_primes);
		g_primes = g_primes - 1.0/3.0;
        
//        temp1 = zeros(size(g_primes));
//        temp1(bsxfun(@gt, g_primes, 0)) = 1/4;
//        temp1(bsxfun(@lt, g_primes, 0)) = 3/4;
//        temp2 = ones(size(g_primes));
//        temp2(bsxfun(@eq, g_primes, 0)) = 0;
		Mat temp1 = Mat::zeros(nRow, nCol, CV_64F);
		Mat temp2 = Mat::zeros(nRow, nCol, CV_64F);
    Mat index = (g_primes > 0)/255;
    Mat doubleIndex;
    index.convertTo(doubleIndex, CV_64F);
    temp1 = temp1 + (1.0/(4.0)) * doubleIndex;
    temp2 = temp2 + doubleIndex;
    index = g_primes < 0;
    temp1 = temp1 + (3.0/(4.0)) * doubleIndex ;
    temp2 = temp2 + doubleIndex;
        
//        tslmap(:, :, 1) = 1 / (2 * pi) * bsxfun(@atan2, r_primes, g_primes) .* temp2 + temp1;
    Mat tmp[3];
		tmp[0] = atan2Mat(r_primes, g_primes);
    tmp[0] = (1.0/(2*M_PI)) * tmp[0];
		multiply(tmp[0], temp2, tmp[0]);
		tmp[0] = tmp[0] + temp1;
        
//        tslmap(:, :, 2) = bsxfun(@power, (9/5 * (r_primes.^2 + g_primes.^2)), 1/2);
		tmp[1] = (9.0/5.0) *(multiply(r_primes, r_primes) + multiply(g_primes, g_primes));
		sqrt(tmp[1], tmp[1]);
        
//        tslmap(:, :, 3) = 0.299 * rgbmap(:, :, 1) + 0.587 * rgbmap(:, :, 2) + 0.114 * rgbmap(:, :, 3);
		tmp[2] = 0.299 * rgb_channel[0] + 0.587 * rgb_channel[1] + 0.114 * rgb_channel[2];
    merge(tmp, 3, dst);
	}
    
    void blurDnClr(const Mat& src, Mat &dst, int level) {
        dst = src.clone();
        for (int i = 0; i < level; ++i) {
            int nRow = dst.rows/2 + int(dst.rows%2 > 0);
            int nCol = dst.cols/2 + int(dst.cols%2 > 0);
            pyrDown(dst, dst, Size(nCol, nRow));
        }
    }


    void corrDn(const Mat &src, Mat &dst, const Mat &filter, int rectRow, int rectCol) {
        resize(src,dst,Size(src.rows/rectRow,src.cols/rectCol),0,0,INTER_NEAREST);
    }
    
    /*
     Reimplement ideal band passing
     */
    void ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, double wl, double wh, int nChannel, double samplingRate) {
            //        src: T*M*N*C;
        
            // extract src info
        int nTime = (int)src.size();
        int nRow = src[0].rows;
        int nCol = src[0].cols;

        
            // copy and convert data from src to dst (CV_32FC(nChannels))
        Mat tmp;
        dst.clear();
        for (int i = 0; i < nTime; ++i)
            {
            if (MHR::_THREE_CHAN_MODE)
                src[i].convertTo(tmp, CV_32FC3);
            else
                src[i].convertTo(tmp, CV_32F);
            dst.emplace_back(tmp.clone());
            }
        
            // masking indexes
        int f1 = ceil(wl * nTime/samplingRate);
        int f2 = floor(wh * nTime/samplingRate);
        int ind1 = 2*f1, ind2 = 2*f2 - 1;
        
            // FFT: http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#dft
        Mat dft_out = Mat::zeros(nRow, nTime, CV_32F);
        for (int channel = 0; channel < nChannel; ++channel) {
            for (int col = 0; col < nCol; ++col) {
                    // select only 1 channel in the dst's Mats
                for (int time = 0; time < nTime; ++time)
                    for (int row = 0; row < nRow; ++row)
                        if (MHR::_THREE_CHAN_MODE)
                            dft_out.at<float>(row, time) = dst[time].at<Vec3f>(row, col)[channel];
                        else
                            dft_out.at<float>(row, time) = dst[time].at<float>(row, col);
                
                    // call FFT
                dft(dft_out, dft_out, DFT_ROWS);
                
                    // masking: all elements with time-index in ranges [0, ind1] and [ind2, nTime-1]
                    // will be set to 0
                for (int row = 0; row < nRow; ++row) {
                    for (int time = 0; time <= ind1; ++time)
                        dft_out.at<float>(row, time) = 0;
                    for (int time = ind2; time < nTime; ++time)
                        dft_out.at<float>(row, time) = 0;
                }
                
                    // assign values in dft_out to dst
                dft(dft_out, dft_out, DFT_ROWS + DFT_INVERSE + DFT_REAL_OUTPUT + DFT_SCALE);
                for (int time = 0; time < nTime; ++time)
                    for (int row = 0; row < nRow; ++row)
                        if (MHR::_THREE_CHAN_MODE)
                            dst[time].at<Vec3f>(row, col)[channel] = dft_out.at<float>(row, time);
                        else
                            dst[time].at<float>(row, col) = dft_out.at<float>(row, time);
            }
        }
        
            // convert the dst Mat to CV_64FC3 or CV_64F
        for (int i = 0; i < nTime; ++i)
            if (MHR::_THREE_CHAN_MODE)
                dst[i].convertTo(dst[i], CV_64FC3);
            else
                dst[i].convertTo(dst[i], CV_64F);
    }
}