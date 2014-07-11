//
//  build_Gdown_stack.cpp
//  MisfitHeartRate
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
	void build_Gdown_Stack(const vector<Mat>& vid, Mat &GDownStack, int startIndex, int endIndex, int level) {
        clock_t t1 = clock();
        // firstFrame
        Mat frame, rgbframe;
//        vid[0].convertTo(rgbframe, CV_64FC3);
//        rgb2ntsc(rgbframe, frame);
        vid[0].convertTo(frame, CV_64FC3);
        
        frameToFile(vid[0], "/var/mobile/Applications/40BBE745-97D5-4BEA-B486-AB77BCE9B3B2/Documents/test_frame_rgb2ntsc.jpg");
        
        // Blur and downsample the frame
        Mat blurred;
        blurDnClr(frame, blurred, level);
        
        printf("blurred.size = (%d, %d)\n", blurred.rows, blurred.cols);
        
        frameToFile(blurred, "/var/mobile/Applications/40BBE745-97D5-4BEA-B486-AB77BCE9B3B2/Documents/test_frame_blurred.jpg");
        
        // create pyr stack
        // Note that this stack is actually just a SINGLE level of the pyramid
        int GdownSize[] = {endIndex - startIndex + 1, blurred.size.p[0], blurred.size.p[1]};
		GDownStack = Mat(3, GdownSize, CV_64FC3, cvScalar(0));
        
        printf("GdownSize: (%d, %d, %d)\n", GDownStack.size.p[0], GDownStack.size.p[1], GDownStack.size.p[2]);
        
        // The first frame in the stack is saved
        for (int i = 0; i < GDownStack.size.p[1]; ++i)
            for (int j = 0; j < GDownStack.size.p[2]; ++j)
                GDownStack.at<Vec3d>(0, i, j) = blurred.at<Vec3d>(i, j);
        
        for (int i = startIndex+1, k = 1; i <= endIndex; ++i, ++k) {
            // Create a frame from the ith array in the stream
            frame = vid[i];
            frame.convertTo(rgbframe, CV_64FC3);
            
//            rgb2ntsc(rgbframe, frame);
            
            // Blur and downsample the frame
            blurDnClr(frame, blurred, level);
            
            // The kth element in the stack is saved
            // Note that this stack is actually just a SINGLE level of the pyramid
            for (int i = 0; i < GDownStack.size.p[1]; ++i)
                for (int j = 0; j < GDownStack.size.p[2]; ++j)
                    GDownStack.at<Vec3d>(k, i, j) = blurred.at<Vec3d>(i, j);
        }
        
        printf("build_Gdown_Stack() runtime = %lf\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
	}
}