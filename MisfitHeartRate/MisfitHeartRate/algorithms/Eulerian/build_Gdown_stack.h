//
//  build_Gdown_stack.h
//  Pulsar
//
//  Created by Bao Nguyen on 7/5/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __Pulsar__build_Gdown_stack__
#define __Pulsar__build_Gdown_stack__

#include "image.h"


namespace MHR {
	// Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to END_INDEX
	// and select a specific band indicated by LEVEL.
	// GDOWN_STACK: stack of one band of Gaussian pyramid of each frame
	// the first dimension is the time axis
	// the second dimension is the y axis of the video
	// the third dimension is the x axis of the video
	// the forth dimension is the color channel
	void build_Gdown_Stack(const vector<Mat>& vid, vector<Mat> &GDownStack, int startIndex, int endIndex, int level);

}

#endif /* defined(__Pulsar__build_Gdown_stack__) */
