//
//  image.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "image.h"


namespace MHR {
    // print a frame to file
    bool frameToFile(const Mat& frame, const String& outFile)
    {
        printf("Save a frame to %s\n", outFile.c_str());
        return imwrite(outFile, frame);
    }


	// convert a RGB Mat to a TSL Mat
	void rgb2tsl(const Mat& srcRGBmap, Mat &dst)
	{
		int nRow = srcRGBmap.rows;
		int nCol = srcRGBmap.cols;
		Mat rbgmap(nRow, nCol, CV_64FC3, srcRGBmap.data);

//        r_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 1), sum(rgbmap, 3)), 1/3);
//        r_primes(isnan(r_primes)) = -1/3;
		Mat r_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(cloneWithChannel(rbgmap, 0), sumChannels(rbgmap), r_primes);
		subtract(r_primes, Mat(nRow, nCol, CV_64F, cvScalar(1.0/3.0)), r_primes);

//        g_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 2), sum(rgbmap, 3)), 1/3);
//        g_primes(isnan(g_primes)) = -1/3;
		Mat g_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(cloneWithChannel(rbgmap, 1), sumChannels(rbgmap), g_primes);
		subtract(r_primes, Mat(nRow, nCol, CV_64F, cvScalar(1.0/3.0)), g_primes);

//        temp1 = zeros(size(g_primes));
//        temp1(bsxfun(@gt, g_primes, 0)) = 1/4;
//        temp1(bsxfun(@lt, g_primes, 0)) = 3/4;
//        temp2 = ones(size(g_primes));
//        temp2(bsxfun(@eq, g_primes, 0)) = 0;
		Mat temp1 = Mat::zeros(nRow, nCol, CV_64F);
		Mat temp2 = Mat::ones(nRow, nCol, CV_64F);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
				if (g_primes.at<double>(i, j) > 0)
				{
					temp1.at<double>(i, j) = 1.0/4.0;
				}
				else if (g_primes.at<double>(i, j) > 0)
				{
					temp1.at<double>(i, j) = 3.0/4.0;
				}
				else
				{
					temp2.at<double>(i, j) = 0;
				}

        dst = Mat::zeros(nRow, nCol, CV_64FC3);
//        tslmap(:, :, 1) = 1 / (2 * pi) * bsxfun(@atan2, r_primes, g_primes) .* temp2 + temp1;
		Mat tmp0 = atan2Mat(r_primes, g_primes);
		multiply(tmp0, Mat(nRow, nCol, CV_64F, cvScalar(1.0/(2*M_PI))), tmp0);
		multiply(tmp0, temp2, tmp0);
		add(tmp0, temp1, tmp0);
//        tslmap(:, :, 2) = bsxfun(@power, (9/5 * (r_primes.^2 + g_primes.^2)), 1/2);
		Mat tmp1 = powMat(r_primes, 2);
		add(tmp1, powMat(g_primes, 2), tmp1);
		multiply(tmp1, Mat(nRow, nCol, CV_64F, cvScalar(9.0/5.0)), tmp1);
		pow(tmp1, 0.5, tmp1);
//        tslmap(:, :, 3) = 0.299 * rgbmap(:, :, 1) + 0.587 * rgbmap(:, :, 2) + 0.114 * rgbmap(:, :, 3);
		Mat tmp2 = add(multiply(cloneWithChannel(rbgmap, 0), 0.299),
					   multiply(cloneWithChannel(rbgmap, 1), 0.587));
		add(tmp2, multiply(cloneWithChannel(rbgmap, 2), 0.114), tmp2);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
			{
				dst.at<Vec3d>(i, j)[0] = tmp0.at<double>(i, j);
				dst.at<Vec3d>(i, j)[1] = tmp1.at<double>(i, j);
				dst.at<Vec3d>(i, j)[2] = tmp2.at<double>(i, j);
			}
	}


    // convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
    //      http://www.mathworks.com/help/images/ref/rgb2ntsc.html
    void rgb2ntsc(const Mat& rgbFrame, Mat &dst) {
        /*double baseArray[9] = {
            0.299, 0.587, 0.114,
            0.595716, -0.274453, -0.321263,
            0.211456, -0.522591, 0.311135,
        };*/
//        Mat base = arrayToMat(baseArray, 3, 3);
//        mulAndClip(rgbFrame, dst, rgb2ntsc_baseMat, 0, 255);
        rgbFrame.convertTo(dst, CV_64FC3);
        int nRow = dst.rows, nCol = dst.cols;
        int nChannel = _number_of_channels;
        double maxChannelValue[_number_of_channels] = {0.0000001};
        // mutiply and find max value in each channel
        Mat tmp = Mat::zeros(nChannel, 1, CV_64F);
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j) {
//                tmp = base * Mat(dst.at<Vec3d>(i, j));
                for (int channel = 0; channel < nChannel; ++channel)
                    tmp.at<double>(channel, 0) = dst.at<Vec3d>(i, j)[channel];
                tmp = rgb2ntsc_baseMat * tmp;
                for (int channel = 0; channel < nChannel; ++channel)
                    maxChannelValue[channel] = max(maxChannelValue[channel], tmp.at<double>(channel, 0));
                dst.at<Vec3d>(i, j) = Vec3d(tmp);
            }
        // clip
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j) {
                for (int channel = 0; channel < nChannel; ++channel)
                    dst.at<Vec3d>(i, j)[channel] *= 255/maxChannelValue[channel];
            }
    }


    // convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
    //      http://www.mathworks.com/help/images/ref/ntsc2rgb.html
    void ntsc2rgb(const Mat& ntscFrame, Mat &dst)  {
        //double baseArray[9] = {
        //  1, 0.9563, 0.6210,
        //  1, -0.2721, -0.6474,
        //  1, -1.1070, 1.7046,
        //};
//        Mat base = arrayToMat(baseArray, 3, 3);
//        mulAndClip(ntscFrame, dst, ntsc2rgb_baseMat, 0, 255);
        
        ntscFrame.convertTo(dst, CV_64FC3);
        int nRow = dst.rows, nCol = dst.cols;
        int nChannel = _number_of_channels;
        double maxChannelValue[_number_of_channels] = {0.0000001};
        // mutiply and find max value in each channel
        Mat tmp = Mat::zeros(nChannel, 1, CV_64F);
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j) {
//                tmp = base * Mat(dst.at<Vec3d>(i, j));
                for (int channel = 0; channel < nChannel; ++channel)
                    tmp.at<double>(channel, 0) = dst.at<Vec3d>(i, j)[channel];
                tmp = ntsc2rgb_baseMat * tmp;
                for (int channel = 0; channel < nChannel; ++channel)
                    maxChannelValue[channel] = max(maxChannelValue[channel], tmp.at<double>(channel, 0));
                dst.at<Vec3d>(i, j) = Vec3d(tmp);
            }
        // clip
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j) {
                for (int channel = 0; channel < nChannel; ++channel)
                    dst.at<Vec3d>(i, j)[channel] *= 255/maxChannelValue[channel];
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
        printf("corrDn, (m, n) = (%d, %d)\n", m, n);
        dst = Mat::zeros(m, n, CV_64F);
        for (int i = 0, x = 0; i < m; ++i, x += rectRow)
            for (int j = 0, y = 0; j < n; ++j, y += rectCol)
                dst.at<double>(i, j) = tmp.at<double>(x, y);
    }
}