//
//  eulerian.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__eulerian__
#define __MisfitHeartRate__eulerian__

#include <string>
#include <vector>
#include "config.h"
#include "matlab.h"
#include "build_Gdown_stack.h"
#include "ideal_bandpassing.h"
#include "filter_bandpassing.h"


namespace MHR {
	// Spatial Filtering: Gaussian blur and down sample
	// Temporal Filtering: Ideal bandpass
    void eulerianGaussianPyramidMagnification(const vector<Mat> &vid, vector<Mat> &ans,
                                              String outDir, double alpha, int level,
                                              double freqBandLowEnd, double freqBandHighEnd,
                                              double samplingRate, double chromAttenuation);
}

#endif /* defined(__MisfitHeartRate__eulerian__) */
