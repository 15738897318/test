//
//  filter_bandpassing.h
//  Pulsar
//
//  Created by Bao Nguyen on 7/8/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __Pulsar__filter_bandpassing__
#define __Pulsar__filter_bandpassing__

#include "config.h"
#include "matlab.h"

using namespace cv;
using namespace std;


namespace MHR {
	// Spatial Filtering: Gaussian blur and down sample
	// Temporal Filtering: Ideal bandpass
    void filter_bandpassing(const Mat &GdownStack, Mat &filteredStack);
}

#endif /* defined(__Pulsar__filter_bandpassing__) */
