//
//  MHRMath.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/25/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__MHRMath__
#define __MisfitHeartRate__MHRMath__

#include <iostream>

#endif /* defined(__MisfitHeartRate__MHRMath__) */

namespace cv {
    
    // return Discrete Fourier Transform of a 2-2 Mat by dimension
	Mat fft(const Mat &src, int dimension);
    
    // return Inverse Discrete Fourier Transform of a 2-2 Mat by dimension
	Mat ifft(const Mat &src, int dimension);

    // convert a frame to signal
    void frames2signal(Mat monoframes, String conversion_method, double frameRate, double cutoff_freq,
                       double &temporal_mean, Mat &debug_frames2signal);
}