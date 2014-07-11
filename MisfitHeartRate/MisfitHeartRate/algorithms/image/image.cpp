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
//    void mulAndClip(const Mat &frame, Mat &dst, const Mat &base,
//                    double lower_bound, double upper_bound)
//    {
//        frame.convertTo(dst, CV_64FC3);
//        int nChannel = _number_of_channels;
//        double maxChannelValue[_number_of_channels] = {0.0000001};
//        // mutiply and find max value in each channel
////        Mat tmp;
////        MatIterator_<Vec3d> it = dst.begin<Vec3d>();
////        MatIterator_<double> tmp_it;
////        for (int i = 0; i < dst.rows; ++i)
////            for (int j = 0; j < dst.cols; ++j) {
////                tmp = base * Mat(*it);
////                tmp_it = tmp.begin<double>();
////                for (int channel = 0; channel < nChannel; ++channel)
////                    maxChannelValue[channel] = max(maxChannelValue[channel], *tmp_it++);
////                *it++ = Vec3d(tmp);
////            }
////        // clip
////        it = dst.begin<Vec3d>();
////        for (int i = 0; i < dst.rows; ++i)
////            for (int j = 0; j < dst.cols; ++j) {
////                for (int channel = 0; channel < nChannel; ++channel)
////                    (*it)[channel] *= upper_bound/maxChannelValue[channel];
////                ++it;
////            }
//        
//        // use .at<>
//        // mutiply and find max value in each channel
//        Mat tmp = Mat::zeros(nChannel, 1, CV_64F);
//        for (int i = 0; i < dst.rows; ++i)
//            for (int j = 0; j < dst.cols; ++j) {
////                tmp = base * Mat(dst.at<Vec3d>(i, j));
//                for (int channel = 0; channel < nChannel; ++channel)
//                    tmp.at<double>(channel, 0) = dst.at<Vec3d>(i, j)[channel];
//                tmp = base * tmp;
//                for (int channel = 0; channel < nChannel; ++channel)
//                    maxChannelValue[channel] = max(maxChannelValue[channel], tmp.at<double>(channel, 0));
//                dst.at<Vec3d>(i, j) = Vec3d(tmp);
//            }
//        // clip
//        for (int i = 0; i < dst.rows; ++i)
//            for (int j = 0; j < dst.cols; ++j) {
//                for (int channel = 0; channel < nChannel; ++channel)
//                    dst.at<Vec3d>(i, j)[channel] *= upper_bound/maxChannelValue[channel];
//            }
//    }


	// convert a RGB Mat to a TSL Mat
    // rgbmap is a CV_64F Mat
	void rgb2tsl(const Mat& rbgmap, Mat &dst)
	{
//        clock_t t1 = clock();
        
		int nRow = rbgmap.rows;
		int nCol = rbgmap.cols;
        int nChannel = rbgmap.channels();
//		Mat rbgmap(nRow, nCol, CV_64FC3, srcRGBmap.data);
        
        Mat rgb_sumchannels = Mat::zeros(nRow, nCol, CV_64F);
        Mat rgb_channel[3] = {Mat::zeros(nRow, nCol, CV_64F), Mat::zeros(nRow, nCol, CV_64F), Mat::zeros(nRow, nCol, CV_64F)};
        for (int i = 0; i < nRow; ++i)
			for (int j = 0; j < nCol; ++j)
                for (int channel = 0; channel < nChannel; ++channel) {
                    rgb_sumchannels.at<double>(i, j) += rbgmap.at<Vec3d>(i, j)[channel];
                    rgb_channel[channel].at<double>(i, j) = rbgmap.at<Vec3d>(i, j)[channel];
                }

//        printf("rgb2tsl() - Block 0 runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
//        t1 = clock();

//        r_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 1), sum(rgbmap, 3)), 1/3);
//        r_primes(isnan(r_primes)) = -1/3;
		Mat r_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(rgb_channel[0], rgb_sumchannels, r_primes);
		r_primes = r_primes - Mat(nRow, nCol, CV_64F, cvScalar(1.0/3.0));
        
//        printf("rgb2tsl() - Block 1 runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
//        t1 = clock();

//        g_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 2), sum(rgbmap, 3)), 1/3);
//        g_primes(isnan(g_primes)) = -1/3;
		Mat g_primes = Mat::zeros(nRow, nCol, CV_64F);
		divide(rgb_channel[1], rgb_sumchannels, g_primes);
		g_primes = g_primes - Mat(nRow, nCol, CV_64F, cvScalar(1.0/3.0));
        
//        printf("rgb2tsl() - Block 2 runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
//        t1 = clock();

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
				else if (g_primes.at<double>(i, j) < 0)
				{
					temp1.at<double>(i, j) = 3.0/4.0;
				}
				else
				{
					temp2.at<double>(i, j) = 0;
				}
        
//        printf("rgb2tsl() - Block 3 runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
//        t1 = clock();

        dst = Mat::zeros(nRow, nCol, CV_64FC3);
//        tslmap(:, :, 1) = 1 / (2 * pi) * bsxfun(@atan2, r_primes, g_primes) .* temp2 + temp1;
		Mat tmp0 = atan2Mat(r_primes, g_primes);
		multiply(tmp0, Mat(nRow, nCol, CV_64F, cvScalar(1.0/(2*M_PI))), tmp0);
		multiply(tmp0, temp2, tmp0);
		tmp0 = tmp0 + temp1;
        
//        printf("rgb2tsl() - Block 4 runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
//        t1 = clock();
        
//        tslmap(:, :, 2) = bsxfun(@power, (9/5 * (r_primes.^2 + g_primes.^2)), 1/2);
		Mat tmp1 = powMat(r_primes, 2);
		tmp1 = tmp1 + powMat(g_primes, 2);
		multiply(tmp1, Mat(nRow, nCol, CV_64F, cvScalar(9.0/5.0)), tmp1);
		pow(tmp1, 0.5, tmp1);
        
//        printf("rgb2tsl() - Block 5 runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
//        t1 = clock();
        
//        tslmap(:, :, 3) = 0.299 * rgbmap(:, :, 1) + 0.587 * rgbmap(:, :, 2) + 0.114 * rgbmap(:, :, 3);
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
        
//        printf("rgb2tsl() - Block 6 runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
	}


    // convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
    //      http://www.mathworks.com/help/images/ref/rgb2ntsc.html
//    void rgb2ntsc(const Mat& rgbFrame, Mat &dst) {
//        rgbFrame.convertTo(dst, CV_64FC3);
//        int nRow = dst.rows, nCol = dst.cols;
//        int nChannel = _number_of_channels;
//        
//        Mat tmp = Mat::zeros(3, nCol, CV_64F);
//        for (int i = 0; i < nRow; ++i) {
//            for (int j = 0; j < nCol; ++j)
//                for (int channel = 0; channel < nChannel; ++channel)
//                    tmp.at<double>(channel, j) = dst.at<Vec3d>(i, j)[channel];
//            tmp = rgb2ntsc_baseMat * tmp;    // slow down the program
//            for (int j = 0; j < nCol; ++j)
//                for (int channel = 0; channel < nChannel; ++channel)
//                    dst.at<Vec3d>(i, j)[channel] = tmp.at<double>(channel, j);
//        }
//    }


    // convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
    //      http://www.mathworks.com/help/images/ref/ntsc2rgb.html
//    void ntsc2rgb(const Mat& ntscFrame, Mat &dst)  {
//        ntscFrame.convertTo(dst, CV_64FC3);
//        int nRow = dst.rows, nCol = dst.cols;
//        int nChannel = _number_of_channels;
//        Mat tmp = Mat::zeros(3, nCol, CV_64F);
//        for (int i = 0; i < nRow; ++i) {
//            for (int j = 0; j < nCol; ++j)
//                for (int channel = 0; channel < nChannel; ++channel)
//                    tmp.at<double>(channel, j) = dst.at<Vec3d>(i, j)[channel];
//            tmp = ntsc2rgb_baseMat * tmp;    // slow down the program
//            for (int j = 0; j < nCol; ++j) {
//                // find max channel value in a pixel
//                double max_channel = 1;
//                for (int channel = 0; channel < nChannel; ++channel)
//                    max_channel = max(max_channel, tmp.at<double>(channel, j));
//                // clip each pixel by max channel value of that pixel, if that max > 1
//                for (int channel = 0; channel < nChannel; ++channel)
//                    dst.at<Vec3d>(i, j)[channel] = tmp.at<double>(channel, j) / max_channel;
//            }
//        }
//    }


    // Blur and downsample an image.  The blurring is done with
    // filter kernel specified by FILT (default = 'binom5')
    void blurDnClr(const Mat& src, Mat &dst, int level) {
        dst = src.clone();
        for (int i = 0; i < level; ++i)
            pyrDown(dst, dst, Size(dst.cols/2, dst.rows/2));
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
//        printf("corrDn, (m, n) = (%d, %d)\n", m, n);
        dst = Mat::zeros(m, n, CV_64F);
        for (int i = 0, x = 0; i < m; ++i, x += rectRow)
            for (int j = 0, y = 0; j < n; ++j, y += rectCol)
                dst.at<double>(i, j) = tmp.at<double>(x, y);
    }
    
    
//    // ScaleRotateTranslate
//    void ScaleRotateTranslate(const Mat &src, Mat &dst, Point2d center, double angle)
//    {
//        
//    }
    
    
//    // crop face
//    void cropFace(const Mat &src, Mat &dst,
//                  Point2d eye_left, Point2d eye_right,
//                  Point2d offset_pct, Point2d dest_sz)
//    {
//        // calculate offsets in original image
//        double offset_h = floor(float(offset_pct.x)*dest_sz.x);
//        double offset_v = floor(float(offset_pct.y)*dest_sz.y);
//        // get the direction
//        Point2d eye_direction = Point2d(eye_right.x - eye_left.x, eye_right.y - eye_left.y);
//        // calc rotation angle in radians
//        double rotation = -atan2(eye_direction.y, eye_direction.x);
//        // distance between them
//        double dist = sqrt(pow(eye_left.x - eye_right.x, 2) + pow(eye_left.y - eye_right.y, 2));
//        // calculate the reference eye-width
//        double reference = dest_sz.x - 2.0*offset_h;
//        // scale factor
//        double scale = dist/reference;
//        // rotate original around the left eye
//        ScaleRotateTranslate(src, dst, eye_left, rotation);
//        //crop the rotated image
//        Point2d crop_xy = Point2d(eye_left.x - scale*offset_h, eye_left.y - scale*offset_v);
//        Point2d crop_size = Point2d(dest_sz.x*scale, dest_sz.y*scale);
//        image = image.crop((int(crop_xy[0]), int(crop_xy[1]), int(crop_xy[0]+crop_size[0]), int(crop_xy[1]+crop_size[1])))
//        // resize it
//        dst.resize(dst, dst, dest_sz, )
//        image = image.resize(dest_sz, Image.ANTIALIAS)
//        return image
//    }
}