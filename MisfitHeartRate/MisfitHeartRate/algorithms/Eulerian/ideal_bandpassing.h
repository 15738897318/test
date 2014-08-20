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
	 * Apply ideal band pass filter on <src>.
     * <src> and <dst> structure:
	 *  + the first dimension is the time axis
	 *  + the second dimension is the y axis of the video's frames
	 *  + the third dimension is the x axis of the video's frames
	 *  + the forth dimension is the color channel
	 * <wl>: lower cutoff frequency of ideal band pass filter
	 * <wh>: higher cutoff frequency of ideal band pass filter
	 * <samplingRate>: sampling rate of <src>
     * data type: CV_64FC3 or CV_64F
     * ref: http://en.wikipedia.org/wiki/Band-pass_filter
	 */
    void ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, double wl, double wh, double samplingRate);
}

#endif /* defined(__MisfitHeartRate__ideal_bandpassing__) */
