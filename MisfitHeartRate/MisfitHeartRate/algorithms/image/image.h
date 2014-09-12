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
#include "config.h"


namespace MHR
{
    const Mat rgb2ntsc_baseMat = (Mat_<double>(3, 3) <<
                                  0.299, 0.587, 0.114,
                                  0.596, -0.274, -0.322,
                                  0.211, -0.523, 0.312);

    const Mat ntsc2rgb_baseMat = (Mat_<double>(3, 3) <<
                                  1.0000, 0.9562, 0.6214,
                                  1.0000, -0.2727, -0.6468,
                                  1.0000, -1.1037, 1.7006);

	// convert a RGB Mat to a TSL Mat
	void rgb2tsl(const Mat& rgbmap, Mat &dst);

    // Blur and downsample an image.  The blurring is done with
    // filter kernel specified by FILT (default = 'binom5')
    void blurDnClr(const Mat& src, Mat &dst, int level);

    // Compute correlation of matrices IM with FILT, followed by
    // downsampling.  These arguments should be 1D or 2D matrices, and IM
    // must be larger (in both dimensions) than FILT.  The origin of filt
    // is assumed to be floor(size(filt)/2)+1.
    void corrDn(const Mat &src, Mat &dst, const Mat &filter, int rectRow, int rectCol);    
}

#endif /* defined(__MisfitHeartRate__image__) */
