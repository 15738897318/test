//
//  heartRate_calc.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "heartRate_calc.h"


namespace cv {
    vector<double> heartRate_calc(vector<Mat> &vid, double window_size_in_sec, double overlap_ratio,
                                  double max_bpm, double cutoff_freq, int colour_channel,
                                  String colourspace, double time_lag)
    {
        double threshold_fraction = 0;
        String conversion_method = "mode-balance";
        
        // Block 1 ==== Load the video & convert it to the desired colour-space
        // Extract video info
        int vidHeight = vid[0].rows;
        int vidWidth = vid[0].cols;
        int nChannels = 3;
        double frameRate = 30;
        int len = vid.size();
        
        int window_size = round(window_size_in_sec * frameRate);
        int firstSample = round(frameRate * time_lag);
        
        // Define the indices of the frames to be processed
        int startIndex = 0;     // 400
        int endIndex = len-1;   // 1400
        
        // Convert colourspaces for each frame
        Mat rgbframe, colorframe;
        int monoframesSize[] = {vidHeight, vidWidth, endIndex-startIndex+1};
        Mat monoframes = Mat(3, monoframesSize, CV_64F, CvScalar(0));
        
        double filtArray[] = {
            0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085,
            0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127,
            0.0162, 0.0241, 0.0307, 0.0332, 0.0307, 0.0241, 0.0162,
            0.0175, 0.0261, 0.0332, 0.0360, 0.0332, 0.0261, 0.0175,
            0.0162, 0.0241, 0.0307, 0.0332, 0.0307, 0.0241, 0.0162,
            0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127,
            0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085
        };
        Mat filt = arrayToMat(filtArray, 7, 7);

        for (int i = startIndex, k = 0; i <= endIndex; ++i, ++k)
        {
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
            
            Mat monoframe = Mat::zeros(colorframe.size.p[0], colorframe.size.p[1], CV_64F);
            for (int x = 0; x < colorframe.size.p[0]; ++x)
                for (int y = 0; y < colorframe.size.p[1]; ++y)
                    monoframe.at<double>(x, y) = colorframe.at<Vec3d>(x, y)[colour_channel];
			
			// Downsample the frame for ease of computation
            monoframe = corrDn(monoframe, filt, 4, 4);
			
			// Put the frame into the video stream
            for (int x = 0; x < vidHeight; ++x)
                for (int y = 0; y < vidWidth; ++y)
                    monoframes.at<double>(x, y, k) = monoframe.at<double>(x, y);
        }

        // Block 2 ==== Extract a signal stream & pre-process it
        // Convert the frame stream into a 1-D signal
        Mat debug_monoframes;
        vector<double> temporal_mean = frames2signal(monoframes, conversion_method, frameRate, cutoff_freq, debug_monoframes);
        
        // Block 3 ==== Heart-rate calculation
        // Set peak-detection params
        double threshold = threshold_fraction * (*max_element(temporal_mean.begin() + firstSample, temporal_mean.begin()));
        int minPeakDistance = round(60 / max_bpm * frameRate);
        
        // Calculate heart-rate using peak-detection on the signal
        hrDebug debug_pda;
        double avg_hr_pda = hr_calc_pda(temporal_mean, frameRate, firstSample,
                                        window_size, overlap_ratio,
                                        minPeakDistance, threshold,
                                        debug_pda);
        
        // Calculate heart-rate using peak-detection on the signal
        hrDebug debug_autocorr;
        double avg_hr_autocorr = hr_calc_autocorr(temporal_mean, frameRate, firstSample,
                                                  window_size, overlap_ratio, minPeakDistance,
                                                  debug_autocorr);
//        double avg_hr_autocorr = hr_calc_autocorr(temporal_mean, frameRate, firstSample,
//                                                  window_size, overlap_ratio,
//                                                  minPeakDistance,
//                                                  debug_autocorr);

        return vector<double>{avg_hr_autocorr, avg_hr_pda};
    }
}