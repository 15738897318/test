//
//  build_Gdown_stack.cpp
//  Pulsar
//
//  Created by Bao Nguyen on 7/5/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "build_Gdown_stack.h"


namespace MHR {
	// Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to END_INDEX
	// and select a specific band indicated by LEVEL.
	// GDOWN_STACK: stack of one band of Gaussian pyramid of each frame
	// the first dimension is the time axis
	// the second dimension is the y axis of the video
	// the third dimension is the x axis of the video
	// the forth dimension is the color channel
	void build_Gdown_Stack(const vector<Mat>& vid, vector<Mat> &GDownStack, int startIndex, int endIndex, int level) {
        clock_t t1 = clock();
        
        // firstFrame
        Mat frame;
        vid[0].convertTo(frame, CV_64FC3);
 
        // Blur and downsample the frame
        Mat blurred;
        blurDnClr(frame, blurred, level);
        int nRow = blurred.size.p[0], nCol = blurred.size.p[1];
//        int nTime = endIndex - startIndex + 1;

        if (DEBUG_MODE) {
            printf("blurred.size = (%d, %d)\n", blurred.rows, blurred.cols);
            frameToFile(blurred, _outputPath + "test_frame_blurred.jpg");
        }
        
        // create pyr stack
        // Note that this stack is actually just a SINGLE level of the pyramid
        GDownStack.clear();
        
        // The first frame in the stack is saved
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
                GDownStack.push_back(blurred.clone());
        
        for (int i = startIndex+1, k = 1; i <= endIndex; ++i, ++k) {
            // Create a frame from the ith array in the stream
            vid[i].convertTo(frame, CV_64FC3);
    
            // Blur and downsample the frame
            blurDnClr(frame, blurred, level);
            
            // The kth element in the stack is saved
            // Note that this stack is actually just a SINGLE level of the pyramid
            for (int i = 0; i < nRow; ++i)
                for (int j = 0; j < nCol; ++j)
                    GDownStack.push_back(blurred.clone());
        }
        
        if (DEBUG_MODE)
            printf("build_Gdown_Stack() runtime = %lf\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
	}
}