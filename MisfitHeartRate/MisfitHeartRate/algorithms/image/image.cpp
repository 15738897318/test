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


    // multiply each pixel of a frame with a base matrix
    // and clip the result's values by range [lower_bound, upper_bound]
    void mulAndClip(const Mat &frame, const Mat &base, Mat &dst,
                    double lower_bound, double upper_bound)
    {
        frame.convertTo(dst, CV_64FC3);
        int nChannel = dst.channels();
        MatIterator_<Vec3d> it = dst.begin<Vec3d>();
        for (int i = 0; i < dst.rows; ++i)
            for (int j = 0; j < dst.cols; ++j) {
                Mat tmp = base * Mat(*it);
                MatIterator_<double> tmp_it = tmp.begin<double>();
                for (int channel = 0; channel < nChannel; ++channel) {
                    *tmp_it = min(*tmp_it, upper_bound);
                    *tmp_it++ = max(*tmp_it, lower_bound);
                }
                *it++ = Vec3d(tmp);
            }
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
        mulAndClip(rgbFrame, rgb2ntsc_baseMat, dst, 0, 255);
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
        mulAndClip(ntscFrame, ntsc2rgb_baseMat, dst, 0, 255);
    }


    // Blur and downsample an image.  The blurring is done with
    // filter kernel specified by FILT (default = 'binom5')
    Mat blurDnClr(const Mat& src, int level) {
        Mat ans = src.clone();
        for (int i = 0; i < level; ++i)
            pyrDown(ans, ans, Size(ans.cols/2, ans.rows/2));
        return ans;
    }


    // Compute correlation of matrices IM with FILT, followed by
    // downsampling.  These arguments should be 1D or 2D matrices, and IM
    // must be larger (in both dimensions) than FILT.  The origin of filt
    // is assumed to be floor(size(filt)/2)+1.
    Mat corrDn(const Mat &src, const Mat &filter, int rectRow, int rectCol)
    {
        Mat tmp;
        filter2D(src, tmp, -1, filter);
        int m = tmp.rows/rectRow + (tmp.rows%rectRow > 0);
        int n = tmp.cols/rectCol + (tmp.cols%rectCol > 0);
        printf("corrDn, (m, n) = (%d, %d)\n", m, n);
        Mat ans = Mat::zeros(m, n, CV_64F);
        for (int i = 0, x = 0; i < m; ++i, x += rectRow)
            for (int j = 0, y = 0; j < n; ++j, y += rectCol)
                ans.at<double>(i, j) = tmp.at<double>(x, y);
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
//		int vidHeight = vid[0].rows;
//		int vidWidth = vid[0].cols;
//		int nChannels = vid[0].channels(); // 3 ????

        // firstFrame
        Mat frame, rgbframe = convertTo(vid[0], CV_64FC3);
        rgb2ntsc(rgbframe, frame);

        frameToFile(vid[0], "/var/mobile/Applications/40BBE745-97D5-4BEA-B486-AB77BCE9B3B2/Documents/test_frame_rgb2ntsc.jpg");

        // Blur and downsample the frame
        Mat blurred = blurDnClr(frame, level);

        printf("blurred.size = (%d, %d)\n", blurred.rows, blurred.cols);

        frameToFile(blurred, "/var/mobile/Applications/40BBE745-97D5-4BEA-B486-AB77BCE9B3B2/Documents/test_frame_blurred.jpg");

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
            rgb2ntsc(rgbframe, frame);

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