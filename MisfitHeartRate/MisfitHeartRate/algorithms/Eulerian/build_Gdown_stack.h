//
//  build_Gdown_stack.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/5/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__build_Gdown_stack__
#define __MisfitHeartRate__build_Gdown_stack__

#include "files.h"

namespace MHR {
	/**
	 * Apply Gaussian pyramid decomposition on \a vid from \a startIndex to \a endIndex,
	 * and select a specific band indicated by \a level. \n
	 * \return \a GDownStack is stack of one band of Gaussian pyramid of each frame
     * \param vid,GDownStack:
	 *  + the first dimension is the time axis \n
	 *  + the second dimension is the y axis of the video's frames \n
	 *  + the third dimension is the x axis of the video's frames \n
	 *  + the forth dimension is the color channel \n
     * Data type: CV_64FC3 or CV_64F
	 */
	void build_Gdown_Stack(const vector<Mat>& vid, vector<Mat> &GDownStack, int startIndex, int endIndex, int level);
}

#endif /* defined(__MisfitHeartRate__build_Gdown_stack__) */
