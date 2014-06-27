//
//  MHRImage.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/24/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__MHRImage__
#define __MisfitHeartRate__MHRImage__

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "MHRMatrix.h"

namespace cv {
	// convert a RGB Mat to a TSL Mat
	Mat rgb2tsl(const Mat& srcRGBmap);
    
	// convert a RGB Mat to a NTSC Mat
	Mat rgb2ntsc(const Mat& srcRGBmap);
    
	// convert a RGB Mat to a NTSC Mat
	Mat ntsc2rgb(const Mat& srcNTSCmap);
    
    // 3-color version of blurDn
    Mat blurDnClr(const Mat& src, int level);
    
	// Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to END_INDEX
	// and select a specific band indicated by LEVEL.
	// GDOWN_STACK: stack of one band of Gaussian pyramid of each frame
	// the first dimension is the time axis
	// the second dimension is the y axis of the video
	// the third dimension is the x axis of the video
	// the forth dimension is the color channel
	Mat buildGDownStack(String vidFile, int startIndex, int endIndex, int level);
    
	// Apply ideal band pass filter on INPUT along dimension DIM.
	// WL: lower cutoff frequency of ideal band pass filter
	// WH: higher cutoff frequency of ideal band pass filter
	// SAMPLINGRATE: sampling rate of INPUT
	Mat idealBandpassing(Mat input, int dim, double wl, double wh, double samplingRate);
    
    // convert a frame to signal
    void frames2signal(Mat monoframes, String conversion_method, double frameRate, double cutoff_freq,
                       double &temporal_mean, Mat &debug_frames2signal);
}

#endif /* defined(__MisfitHeartRate__MHRImage__) */


