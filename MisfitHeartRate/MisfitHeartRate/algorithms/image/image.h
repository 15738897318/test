//
//  image.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__image__
#define __MisfitHeartRate__image__

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "time.h"
#include "matrix.h"


namespace MHR {
    const Mat rgb2ntsc_baseMat = (Mat_<double>(3, 3) <<
                                  0.299, 0.587, 0.114,
                                  0.596, -0.274, -0.322,
                                  0.211, -0.523, 0.312);

    const Mat ntsc2rgb_baseMat = (Mat_<double>(3, 3) <<
                                  1.0, 0.956, 0.621,
                                  1.0, -0.272, -0.647,
                                  1.0, -1.106, 1.703);
    
    
    // print a frame to file
    bool frameToFile(const Mat& frame, const String& outFile);

    // multiply each pixel of a frame with a base matrix
    // and clip the result's values by range [lower_bound, upper_bound]
    void mulAndClip(const Mat &frame, const Mat &base, Mat &dst,
                    double lower_bound, double upper_bound);

	// convert a RGB Mat to a TSL Mat
	void rgb2tsl(const Mat& srcRGBmap, Mat &dst);

	// convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
	void rgb2ntsc(const Mat& rgbFrame, Mat &dst);

	// convert a RGB Mat to a NTSC Mat
    // ref: http://en.wikipedia.org/wiki/YIQ
	void ntsc2rgb(const Mat& ntscFrame, Mat &dst);

    // Blur and downsample an image.  The blurring is done with
    // filter kernel specified by FILT (default = 'binom5')
    Mat blurDnClr(const Mat& src, int level);

    // Compute correlation of matrices IM with FILT, followed by
    // downsampling.  These arguments should be 1D or 2D matrices, and IM
    // must be larger (in both dimensions) than FILT.  The origin of filt
    // is assumed to be floor(size(filt)/2)+1.
    Mat corrDn(const Mat &src, const Mat &filter, int rectRow, int rectCol);

	// Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to END_INDEX
	// and select a specific band indicated by LEVEL.
	// GDOWN_STACK: stack of one band of Gaussian pyramid of each frame
	// the first dimension is the time axis
	// the second dimension is the y axis of the video
	// the third dimension is the x axis of the video
	// the forth dimension is the color channel
	Mat buildGDownStack(const vector<Mat>& vid, int startIndex, int endIndex, int level);
}

#endif /* defined(__MisfitHeartRate__image__) */
