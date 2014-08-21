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
     * Use Eulerian magnification technique on input frames. \n
     * Spatial Filtering: Gaussian blur and down sample \n
     * Temporal Filtering: Ideal bandpass \n
     * \ref: http://graphics.cs.cmu.edu/courses/15-463/2012_fall/hw/proj2g-eulerian/ \n
     * \param outDir output folder for debug files
     * \param alpha magnification rate
     * \param level see blurDnClr()
     * \param freqBandLowEnd see \a wl argument in ideal_bandpassing()
     * \param freqBandHighEnd see \a wh argument in ideal_bandpassing()
     * \param samplingRate the video's frame rate
     * \param chromAttenuation the magnification rate of chromA channel in YIQ coulourspace
     */
    void eulerianGaussianPyramidMagnification(const vector<Mat> &vid, vector<Mat> &ans,
                                              String outDir, double alpha, int level,
                                              double freqBandLowEnd, double freqBandHighEnd,
                                              double samplingRate, double chromAttenuation);
}

#endif /* defined(__MisfitHeartRate__eulerian__) */
