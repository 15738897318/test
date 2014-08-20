//
//  eulerian.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__eulerian__
#define __MisfitHeartRate__eulerian__

#include "build_Gdown_stack.h"
#include "ideal_bandpassing.h"


namespace MHR {
    /**
     * Spatial Filtering: Gaussian blur and down sample
     * Temporal Filtering: Ideal bandpass
     * ref: http://graphics.cs.cmu.edu/courses/15-463/2012_fall/hw/proj2g-eulerian/
     */
    void eulerianGaussianPyramidMagnification(const vector<Mat> &vid, vector<Mat> &ans,
                                              String outDir, double alpha, int level,
                                              double freqBandLowEnd, double freqBandHighEnd,
                                              double samplingRate, double chromAttenuation);
}

#endif /* defined(__MisfitHeartRate__eulerian__) */
