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


namespace MHR {
    const Mat rgb2ntsc_baseMat = (Mat_<mTYPE>(3, 3) <<
                                  0.299, 0.587, 0.114,
                                  0.596, -0.274, -0.322,
                                  0.211, -0.523, 0.312);

    const Mat ntsc2rgb_baseMat = (Mat_<mTYPE>(3, 3) <<
                                  1.0, 0.956, 0.621,
                                  1.0, -0.272, -0.647,
                                  1.0, -1.106, 1.703);
    
    
    // print a frame to file
    bool frameToFile(const Mat& frame, const String& outFile);

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
    
//    // ScaleRotateTranslate
//    void ScaleRotateTranslate(const Mat &src, Mat &dst, Point2d center, mTYPE angle);
//    
//    // crop face
//    void cropFace(const Mat &src, Mat &dst,
//                  Point2d eye_left = Point2d(0, 0), Point2d eye_right = Point2d(0, 0),
//                  Point2d offset_pct = Point2d(0.2, 0.2), Point2d dest_sz = Point2d(70, 70));

}

#endif /* defined(__MisfitHeartRate__image__) */
