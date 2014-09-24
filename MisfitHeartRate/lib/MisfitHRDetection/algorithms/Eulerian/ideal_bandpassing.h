//
//  ideal_bandpassing.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/8/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__ideal_bandpassing__
#define __MisfitHeartRate__ideal_bandpassing__

#include "matlab.h"

using namespace cv;
using namespace std;

namespace MHR {
	/**
	 * Apply ideal band pass filter on \a src. \n
     * \ref: http://en.wikipedia.org/wiki/Band-pass_filter
     * \param src,dst:
	 *  + the first dimension is the time axis \n
	 *  + the second dimension is the y axis of the video's frames \n
	 *  + the third dimension is the x axis of the video's frames \n
	 *  + the forth dimension is the color channel \n
	 * \param wl lower cutoff frequency of ideal band pass filter \n
	 * \param wh higher cutoff frequency of ideal band pass filter \n
	 * \param samplingRate sampling rate of \a src \n
     * Data type: CV_64FC3 or CV_64F
	 */
    void ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, double wl, double wh, double samplingRate);
}

#endif /* defined(__MisfitHeartRate__ideal_bandpassing__) */
