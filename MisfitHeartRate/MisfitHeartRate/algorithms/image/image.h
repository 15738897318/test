//
//  image.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__image__
#define __MisfitHeartRate__image__

#include "matrix.h"

namespace MHR {
    /**
     * From RGB to YIQ: http://en.wikipedia.org/wiki/YIQ
     */
    const Mat rgb2ntsc_baseMat = (Mat_<double>(3, 3) <<
                                  0.299, 0.587, 0.114,
                                  0.596, -0.274, -0.322,
                                  0.211, -0.523, 0.312);

    
    /**
     * From YIQ to RGB: http://en.wikipedia.org/wiki/YIQ
     */
    const Mat ntsc2rgb_baseMat = (Mat_<double>(3, 3) <<
                                  1.0000, 0.9562, 0.6214,
                                  1.0000, -0.2727, -0.6468,
                                  1.0000, -1.1037, 1.7006);

    
    /**
     * convert a RGB frame to a TSL frame
     * data type: CV_64FC3
     * ref: http://en.wikipedia.org/wiki/TSL_color_space
     */
	void rgb2tsl(const Mat& rgbmap, Mat &dst);

    
    /**
     * Blur and downsample an image. The blurring is done with
     * filter kernel specified by FILT (default = 'binom5')
     * ref: https://github.com/diego898/matlab-utils/blob/master/toolbox/EVM_Matlab/blurDnClr.m
     *      http://docs.opencv.org/doc/tutorials/imgproc/pyramids/pyramids.html
     */
    void blurDnClr(const Mat& src, Mat &dst, int level);

    
    /**
     * Compute correlation of matrices <src> with <filter>, followed by downsampling.
     * These arguments should be 1D or 2D matrices,
     * and <src> must be larger (in both dimensions) than <filter>.
     * The origin of <filter> is assumed to be floor(size(<filter>)/2)+1.
     * output <dst> data types: CV_64F
     * ref: http://www.mathworks.com/matlabcentral/fileexchange/43909-separable-steerable-pyramid-toolbox/content/sepspyr/deps/matlabPyrTools-1.3/mpt_corrDn.m
     *      http://docs.opencv.org/modules/imgproc/doc/filtering.html#void%20filter2D%28InputArray%20src,%20OutputArray%20dst,%20int%20ddepth,%20InputArray%20kernel,%20Point%20anchor,%20double%20delta,%20int%20borderType%29
     */
    void corrDn(const Mat &src, Mat &dst, const Mat &filter, int rectRow, int rectCol);    
}

#endif /* defined(__MisfitHeartRate__image__) */
