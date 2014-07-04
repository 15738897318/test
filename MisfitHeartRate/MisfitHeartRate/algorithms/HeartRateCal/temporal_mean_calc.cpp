//
//  temporal_mean_calc.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "temporal_mean_calc.h"


namespace MHR {
    vector<double> temporal_mean_calc(vector<Mat> &vid, double overlap_ratio,
                                      double max_bpm, double cutoff_freq,
                                      int colour_channel, String colourspace,
                                      double &lower_range, double &upper_range, bool isCalcMode)
    {
        String conversion_method = frames2signalConversionMethod;
        
        // Block 1 ==== Load the video & convert it to the desired colour-space
        // Extract video info
        int vidHeight = vid[0].rows;
        int vidWidth = vid[0].cols;
        double frameRate = _frameRate;
        int len = (int)vid.size();
        
        // Define the indices of the frames to be processed
        int startIndex = 0;     // 400
        int endIndex = len-1;   // 1400
        
        // Convert colourspaces for each frame
        Mat rgbframe, colorframe;
        vector<Mat> monoframes;
//        int monoframesSize[] = {vidHeight, vidWidth, endIndex-startIndex+1};
//        Mat monoframes = Mat(3, monoframesSize, CV_64F, CvScalar(0));
        
        Mat filt = arrayToMat(_frame_downsampling_filt, _frame_downsampling_filt_rows, _frame_downsampling_filt_cols);
        
        for (int i = startIndex, k = 0; i <= endIndex; ++i, ++k)
        {
            printf("temporal_mean_calc: index = %i\n", i);
            rgbframe = vid[i];
            if (colourspace == "rgb")
                colorframe = rgbframe;
            else if (colourspace == "hsv")
                cvtColor(rgbframe, colorframe, CV_RGB2HSV);
            else if (colourspace == "ntsc")
                colorframe = rgb2ntsc(rgbframe);
            else if (colourspace == "ycbcr")
                cvtColor(rgbframe, colorframe, CV_RGB2YCrCb);
            else if (colourspace == "tsl")
                colorframe = rgb2tsl(rgbframe);
            
            // Extract the right channel from the colour frame
            
            Mat monoframe = Mat::zeros(vidHeight, vidWidth, CV_64F);
            for (int x = 0; x < vidHeight; ++x)
                for (int y = 0; y < vidWidth; ++y)
                    monoframe.at<double>(x, y) = colorframe.at<Vec3d>(x, y)[colour_channel];
			
			// Downsample the frame for ease of computation
            monoframe = corrDn(monoframe, filt, 4, 4);
			
			// Put the frame into the video stream
            monoframes.push_back(monoframe);
        }
        
        // Block 2 ==== Extract a signal stream & pre-process it
        // Convert the frame stream into a 1-D signal
        vector<Mat> debug_monoframes;
        return frames2signal(monoframes, conversion_method, frameRate, cutoff_freq,
                             lower_range, upper_range, isCalcMode, debug_monoframes);
    }
}