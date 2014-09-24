//
//  build_Gdown_stack.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/5/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "build_Gdown_stack.h"


namespace MHR {
	void build_Gdown_Stack(const vector<Mat>& vid, vector<Mat> &GDownStack, int startIndex, int endIndex, int level) {
        // firstFrame
        Mat frame;
        if (_THREE_CHAN_MODE)
        	vid[startIndex].convertTo(frame, CV_64FC3);
        else
        	vid[startIndex].convertTo(frame, CV_64F);
 
        // Blur and downsample the frame
        Mat blurred;
        blurDnClr(frame, blurred, level);
        
        // create pyr stack
        // Note that this stack is actually just a SINGLE level of the pyramid
        // The first frame in the stack is saved
        GDownStack.clear();
        GDownStack.push_back(blurred.clone());
        
        for (int i = startIndex+1, k = 1; i <= endIndex; ++i, ++k) {
            // Create a frame from the ith array in the stream
            if (_THREE_CHAN_MODE)
				vid[i].convertTo(frame, CV_64FC3);
			else
				vid[i].convertTo(frame, CV_64F);
                
            // Blur and downsample the frame
            blurDnClr(frame, blurred, level);
            
            // The kth element in the stack is saved
            // Note that this stack is actually just a SINGLE level of the pyramid
            GDownStack.push_back(blurred.clone());
        }
	}
}