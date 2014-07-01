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
		
		Mat tslmap = Mat::zeros(nRow, nCol, CV_64FC3);
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
				tslmap.at<Vec3d>(i, j)[0] = tmp0.at<double>(i, j);
				tslmap.at<Vec3d>(i, j)[1] = tmp1.at<double>(i, j);
				tslmap.at<Vec3d>(i, j)[2] = tmp2.at<double>(i, j);
			}
		return tslmap;
	}
    
    
	// convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
	Mat rgb2ntsc(const Mat& rgbFrame) {
        int rows = rgbFrame.rows, cols = rgbFrame.cols;
        double baseArray[9] = {
            0.299, 0.587, 0.114,
            0.595716, -0.274453, -0.321263,
            0.211456, -0.522591, 0.311135,
        };
        Mat base = arrayToMat(baseArray, 3, 3);
        // calculate result Mat
        Mat ans = Mat::zeros(rows, cols, CV_64FC3);
        Mat tmp = Mat::zeros(3, 1, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                for (int channel = 0; channel < 3; ++channel)
                    tmp.at<double>(channel, 0) = rgbFrame.at<Vec3d>(i, j)[channel];
                tmp = base * tmp;
                for (int channel = 0; channel < 3; ++channel)
                    ans.at<Vec3d>(i, j)[channel] = tmp.at<double>(channel, 0);
            }
		return ans;
	}
    
    
	// convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
	Mat ntsc2rgb(const Mat& ntscFrame)	{
        int rows = ntscFrame.rows, cols = ntscFrame.cols;
        double baseArray[9] = {
            1, 0.9563, 0.6210,
            1, -0.2721, -0.6474,
            1, -1.1070, 1.7046,
        };
        Mat base = arrayToMat(baseArray, 3, 3);
        // calculate result Mat
		Mat ans = Mat::zeros(rows, cols, CV_64FC3);
        Mat tmp = Mat::zeros(3, 1, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                for (int channel = 0; channel < 3; ++channel)
                    tmp.at<double>(channel, 0) = ntscFrame.at<Vec3d>(i, j)[channel];
                tmp = base * tmp;
                for (int channel = 0; channel < 3; ++channel)
                    ans.at<Vec3d>(i, j)[channel] = tmp.at<double>(channel, 0);
            }
		return ans;
	}

    
    // Blur and downsample an image.  The blurring is done with
    // filter kernel specified by FILT (default = 'binom5')
    Mat blurDnClr(const Mat& src, int level) {
        Mat ans(src);
        for (int i = 0; i < level; ++i)
            pyrDown(ans, ans, Size(ans.cols/2, ans.rows/2));
        return ans;
    }
    
	
	// Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to END_INDEX
	// and select a specific band indicated by LEVEL.
	// GDOWN_STACK: stack of one band of Gaussian pyramid of each frame
	// the first dimension is the time axis
	// the second dimension is the y axis of the video
	// the third dimension is the x axis of the video
	// the forth dimension is the color channel
	Mat buildGDownStack(const vector<Mat>& vid, int startIndex, int endIndex, int level) {
		// Extract video info
		int vidHeight = vid[0].rows;
		int vidWidth = vid[0].cols;
		int nChannels = vid[0].channels(); // 3 ????

        // firstFrame
        Mat frame = vid[0];
        Mat rgbframe = convertTo(frame, CV_64FC3);
        frame = rgb2ntsc(rgbframe);
        
        // Blur and downsample the frame
        Mat blurred = blurDnClr(frame, level);
        
        printf("blurred.size = (%d, %d)\n", blurred.rows, blurred.cols);
        
        // create pyr stack
        // Note that this stack is actually just a SINGLE level of the pyramid
        int GdownSize[] = {endIndex - startIndex + 1, blurred.size.p[0], blurred.size.p[1]};
		Mat GDownStack = Mat(3, GdownSize, CV_64FC3, cvScalar(0));
        
        printf("GdownSize: (%d, %d, %d)\n", GDownStack.size.p[0], GDownStack.size.p[1], GDownStack.size.p[2]);
        
        // The first frame in the stack is saved
        for (int i = 0; i < GDownStack.size.p[1]; ++i)
            for (int j = 0; j < GDownStack.size.p[2]; ++j)
                for (int t = 0; t < 3; ++t)
                    GDownStack.at<Vec3d>(0, i, j)[t] = blurred.at<Vec3d>(i, j)[t];
        
        for (int i = startIndex+1, k = 1; i <= endIndex; ++i, ++k) {
            // Create a frame from the ith array in the stream
            frame = vid[i];
            rgbframe = convertTo(frame, CV_64FC3);
            frame = rgb2ntsc(rgbframe);
            
//            printf("buildGDownStack: %d --> %d\n", i, endIndex);
            
            // Blur and downsample the frame
            blurred = blurDnClr(frame, level);
            
            // The kth element in the stack is saved
            // Note that this stack is actually just a SINGLE level of the pyramid
            for (int i = 0; i < GDownStack.size.p[1]; ++i)
                for (int j = 0; j < GDownStack.size.p[2]; ++j)
                    GDownStack.at<Vec3d>(k, i, j) = blurred.at<Vec3d>(i, j);
        }
        return GDownStack;
	}
}