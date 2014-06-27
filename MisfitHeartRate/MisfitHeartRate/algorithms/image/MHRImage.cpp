//
//  MHRImage.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/24/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "MHRImage.h"


namespace cv {
	// convert a RGB Mat to a TSL Mat
	Mat rgb2tsl(const Mat& srcRGBmap)
	{
		int nRow = srcRGBmap.rows;
		int nCol = srcRGBmap.cols;
		Mat rbgmap(nRow, nCol, CV_32FC3, srcRGBmap.data);
		
		//        r_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 1), sum(rgbmap, 3)), 1/3);
		//        r_primes(isnan(r_primes)) = -1/3;
		Mat r_primes = Mat::zeros(nRow, nCol, CV_32FC1);
		divide(cloneWithChannel(rbgmap, 0), sumChannels(rbgmap), r_primes);
		subtract(r_primes, Mat(nRow, nCol, CV_32FC1, 1.0/3.0), r_primes);
		
		//        g_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 2), sum(rgbmap, 3)), 1/3);
		//        g_primes(isnan(g_primes)) = -1/3;
		Mat g_primes = Mat::zeros(nRow, nCol, CV_32FC1);
		divide(cloneWithChannel(rbgmap, 1), sumChannels(rbgmap), g_primes);
		subtract(r_primes, Mat(nRow, nCol, CV_32FC1, 1.0/3.0), g_primes);
		
		//        temp1 = zeros(size(g_primes));
		//        temp1(bsxfun(@gt, g_primes, 0)) = 1/4;
		//        temp1(bsxfun(@lt, g_primes, 0)) = 3/4;
		//        temp2 = ones(size(g_primes));
		//        temp2(bsxfun(@eq, g_primes, 0)) = 0;
		Mat temp1 = Mat::zeros(nRow, nCol, CV_32FC1);
		Mat temp2 = Mat::ones(nRow, nCol, CV_32FC1);
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
		
		Mat tslmap = Mat::zeros(nRow, nCol, CV_32FC3);
		//        tslmap(:, :, 1) = 1 / (2 * pi) * bsxfun(@atan2, r_primes, g_primes) .* temp2 + temp1;
		Mat tmp0 = atan2Mat(r_primes, g_primes);
		multiply(tmp0, Mat(nRow, nCol, CV_32FC1, 1/(2*M_PI)), tmp0);
		multiply(tmp0, temp2, tmp0);
		add(tmp0, temp1, tmp0);
		//        tslmap(:, :, 2) = bsxfun(@power, (9/5 * (r_primes.^2 + g_primes.^2)), 1/2);
		Mat tmp1 = powMat(r_primes, 2);
		add(tmp1, powMat(g_primes, 2), tmp1);
		multiply(tmp1, Mat(nRow, nCol, CV_64F, 9.0/5.0), tmp1);
		pow(tmp1, 0.5, tmp1);
		//        tslmap(:, :, 3) = 0.299 * rgbmap(:, :, 1) + 0.587 * rgbmap(:, :, 2) + 0.114 * rgbmap(:, :, 3);
		Mat tmp2 = add(multiply(cloneWithChannel(rbgmap, 0), 0.299),
					   multiply(cloneWithChannel(rbgmap, 1), 0.587));
		add(tmp2, multiply(cloneWithChannel(rbgmap, 2), 0.114), tmp2);
		for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
			{
				tslmap.at<Vec3d>(i, j)[0] = tmp0.at<double>(i, j);
				tslmap.at<Vec3d>(i, j)[1] = tmp1.at<double>(i, j);
				tslmap.at<Vec3d>(i, j)[2] = tmp2.at<double>(i, j);
			}
		return tslmap;
	}
    
    
	// convert a RGB Mat to a NTSC Mat
	Mat rgb2ntsc(const Mat& srcRGBmap) {
		return srcRGBmap;
	}
    
    
	// convert a RGB Mat to a NTSC Mat
	Mat ntsc2rgb(const Mat& srcNTSCmap)	{
		return srcNTSCmap;
	}
    
	
	// Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to END_INDEX
	// and select a specific band indicated by LEVEL.
	// GDOWN_STACK: stack of one band of Gaussian pyramid of each frame
	// the first dimension is the time axis
	// the second dimension is the y axis of the video
	// the third dimension is the x axis of the video
	// the forth dimension is the color channel
	Mat buildGDownStack(String vidFile, int startIndex, int endIndex, int level) {
        // Read video
		VideoCapture vid(vidFile);
        
		// Extract video info
		int vidHeight = (int)vid.get(CV_CAP_PROP_FRAME_HEIGHT);
		int vidWidth = (int)vid.get(CV_CAP_PROP_FRAME_WIDTH);
		int nChannels = 3;		// should get from vid?
        
        // firstFrame
        Mat frame = Mat::zeros(vidHeight, vidWidth, CV_8UC3);
        for (int i = 0; i <= startIndex; ++i)
            vid >> frame;
        Mat rgbframe = convertTo(frame, CV_64FC3);
        frame = rgb2ntsc(rgbframe);
        
        // Blur and downsample the frame
        Mat blurred = blurDnClr(frame, level);
        
        // create pyr stack
        // Note that this stack is actually just a SINGLE level of the pyramid
        int GdownSize[] = {endIndex - startIndex + 1, blurred.size.p[0], blurred.size.p[1], blurred.size.p[2]};
        Mat GDownStack = Mat::zeros(4, GdownSize, CV_64F);
        
        // The first frame in the stack is saved
        for (int i = 0; i < GDownStack.size.p[1]; ++i)
            for (int j = 0; j < GDownStack.size.p[2]; ++j)
                for (int t = 0; t < GDownStack.size.p[3]; ++t)
                    GDownStack.at<Vec3d>(0, i, j)[t] = blurred.at<Vec3d>(i, j)[t];
        
        for (int i = startIndex+1, k = 1; i <= endIndex; ++i, ++k) {
            // Create a frame from the ith array in the stream
            vid >> frame;
            rgbframe = convertTo(frame, CV_64FC3);
            frame = rgb2ntsc(rgbframe);
            
            // Blur and downsample the frame
            blurred = blurDnClr(frame, level);
            
            // The kth element in the stack is saved
            // Note that this stack is actually just a SINGLE level of the pyramid
            for (int i = 0; i < GDownStack.size.p[1]; ++i)
                for (int j = 0; j < GDownStack.size.p[2]; ++j)
                    for (int t = 0; t < GDownStack.size.p[3]; ++t)
                        GDownStack.at<Vec3d>(k, i, j)[t] = blurred.at<Vec3d>(i, j)[t];
        }
        return GDownStack;
	}
    
    
	// Apply ideal band pass filter on INPUT along dimension DIM.
	// WL: lower cutoff frequency of ideal band pass filter
	// WH: higher cutoff frequency of ideal band pass filter
	// SAMPLINGRATE: sampling rate of INPUT
	Mat idealBandpassing(Mat input, int dim, double wl, double wh, double samplingRate) {
		return input;
	}
}