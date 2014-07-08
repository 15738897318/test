//
//  eulerian_old.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/8/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__eulerian_old__
#define __MisfitHeartRate__eulerian_old__

#include <string>
#include <vector>
#include "config.h"
#include "build_Gdown_stack.h"
#include "matlab.h"


namespace MHR {
	// Spatial Filtering: Gaussian blur and down sample
	// Temporal Filtering: Ideal bandpass
	vector<Mat> amplifySpatialGdownTemporalIdeal_old(const vector<Mat> &vid, String outDir,
                                                 double alpha, int level,
                                                 double freqBandLowEnd, double freqBandHighEnd,
                                                 double samplingRate, double chromAttenuation);
}

#endif /* defined(__MisfitHeartRate__eulerian_old__) */
