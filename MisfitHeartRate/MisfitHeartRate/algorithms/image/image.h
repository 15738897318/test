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
     * From RGB to YIQ: \ref: http://en.wikipedia.org/wiki/YIQ
     */
    const Mat rgb2ntsc_baseMat = (Mat_<double>(3, 3) <<
                                  0.299, 0.587, 0.114,
                                  0.596, -0.274, -0.322,
                                  0.211, -0.523, 0.312);

    
    /**
     * From YIQ to RGB: \ref: http://en.wikipedia.org/wiki/YIQ
     */
    const Mat ntsc2rgb_baseMat = (Mat_<double>(3, 3) <<
                                  1.0000, 0.9562, 0.6214,
                                  1.0000, -0.2727, -0.6468,
                                  1.0000, -1.1037, 1.7006);

    
    /**
     * convert a RGB frame to a TSL frame \n
     * Data type: CV_64FC3 \n
     * \ref: http://en.wikipedia.org/wiki/TSL_color_space
     */
	void rgb2tsl(const Mat& rgbmap, Mat &dst);

    
    /**
     * Blur and downsample an image. The blurring is done with
     * filter kernel specified by FILT (default = 'binom5') \n
     * \ref: https://github.com/diego898/matlab-utils/blob/master/toolbox/EVM_Matlab/blurDnClr.m \n
     * \ref: http://docs.opencv.org/doc/tutorials/imgproc/pyramids/pyramids.html \n
     */
    void blurDnClr(const Mat& src, Mat &dst, int level);

    
    /**
     * Compute correlation of 2D matrices \a src with \a filter, followed by downsampling. \n
     * \ref: http://www.mathworks.com/matlabcentral/fileexchange/43909-separable-steerable-pyramid-toolbox/content/sepspyr/deps/matlabPyrTools-1.3/mpt_corrDn.m \n
     * \ref: http://docs.opencv.org/modules/imgproc/doc/filtering.html#void%20filter2D%28InputArray%20src,%20OutputArray%20dst,%20int%20ddepth,%20InputArray%20kernel,%20Point%20anchor,%20double%20delta,%20int%20borderType%29 \n
     * \param src must be larger (in both dimensions) than \a filter. \n
     * \param filter is assumed to be floor(size(\a filter)/2)+1. \n
     * \return \a dst's data types: CV_64F
     */
    void corrDn(const Mat &src, Mat &dst, const Mat &filter, int rectRow, int rectCol);    
}

#endif /* defined(__MisfitHeartRate__image__) */
