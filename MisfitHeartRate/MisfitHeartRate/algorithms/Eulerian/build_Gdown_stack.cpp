//
//  build_Gdown_stack.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/5/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "build_Gdown_stack.h"


namespace MHR
{
	// Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to END_INDEX
	// and select a specific band indicated by LEVEL.
	// GDOWN_STACK: stack of one band of Gaussian pyramid of each frame
	// the first dimension is the time axis
	// the second dimension is the y axis of the video
	// the third dimension is the x axis of the video
	// the forth dimension is the color channel
	void build_Gdown_Stack(const vector<Mat>& vid, vector<Mat> &GDownStack, int startIndex, int endIndex, int level)
    {
        clock_t t1 = clock();
        
        // firstFrame
        Mat frame;
        if (_THREE_CHAN_MODE)
        	vid[startIndex].convertTo(frame, CV_64FC3);
        else
        	vid[startIndex].convertTo(frame, CV_64F);
 
        // Blur and downsample the frame
        Mat blurred;
        blurDnClr(frame, blurred, level);
        
        if (_DEBUG_MODE)
        {
            printf("blurred.size = (%d, %d)\n", blurred.rows, blurred.cols);
            frameChannelToFile(frame, _outputPath + "1_vid[0]_build_Gdown_Stack.txt", _channels_to_process);
            frameChannelToFile(blurred, _outputPath + "1_GDownStack[0]_build_Gdown_Stack.txt", _channels_to_process);
        }
        
        // create pyr stack
        // Note that this stack is actually just a SINGLE level of the pyramid
        GDownStack.clear();
        
        // The first frame in the stack is saved
        GDownStack.emplace_back(blurred.clone());
        
        for (int i = startIndex+1, k = 1; i <= endIndex; ++i, ++k)
        {
            // Create a frame from the ith array in the stream
            if (_THREE_CHAN_MODE)
				vid[i].convertTo(frame, CV_64FC3);
			else
				vid[i].convertTo(frame, CV_64F);
                
            // Blur and downsample the frame
            blurDnClr(frame, blurred, level);
            
            // The kth element in the stack is saved
            // Note that this stack is actually just a SINGLE level of the pyramid
            GDownStack.emplace_back(blurred.clone());
        }
        
        if (_DEBUG_MODE)
            printf("build_Gdown_Stack() runtime = %lf\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
	}
}