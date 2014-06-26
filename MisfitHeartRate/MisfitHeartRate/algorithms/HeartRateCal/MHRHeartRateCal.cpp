//
//  MHRHeartRateCal.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/25/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "MHRHeartRateCal.h"

namespace cv {
    
//    window_size = 90;
//    overlap_ratio = 0;
    Mat heartRate_calc(String vidFile, double window_size_in_sec, double overlap_ratio,
                       double max_bpm, double cutoff_freq, int colour_channel,
                       int ref_reading, String colourspace, double time_lag)
    {
//        unknown types:
//        ref_reading
//        colourspace
//        time_lag
    
        int debug = 1;
        int getRaw = 0;
        int threshold_fraction = 0;
        std::string conversion_method = "mode-balance";
        
//        Block 1 ==== Load the video & convert it to the desired colour-space
//        Get the filename-only part of the full path
        String fileName = vidFile.substr(vidFile.find_last_of('/') + 1);
        printf("filePath = %s", fileName.c_str());
        printf("fileName = %s", fileName.c_str());
        
//        Read video
        VideoCapture videoCapture(vidFile);
        
//        Extract video info
        int vidHeight = videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
        int vidWidth = videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
        int nChannels = 3;
        int frameRate = videoCapture.get(CV_CAP_PROP_FPS);
        int len = videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
        
        double windowSize = round(window_size_in_sec*frameRate);
        double firstSample = round(frameRate*time_lag);
        
//        Define the indices of the frames to be processed
        int startIndex = 0;
        int endIndex = len-1;
        std::vector<Mat> frames;
        
//        Convert colourspaces for each frame
        int monoframesSize[] = {vidHeight, vidWidth, len};
        Mat monoframes = Mat(3, monoframesSize, CV_32FC1, 0);
        Mat gaussianFilter = getGaussianKernel(7, 2.5);
        for (int k = 0; k < len; ++k)
        {
            Mat frame;
            videoCapture >> frame;
            frames.push_back(frame);
            
            if (colourspace == "rgb")
            {
//                cvCvtColor(&frame, &frame, CV_BGR2HSV);
            }
            else if (colourspace == "hsv")
                cvCvtColor(&frame, &frame, CV_BGR2HSV);
            else if (colourspace == "ntsc")
            {
//                frame = rgb2ntsc(frame);
            }
            else if (colourspace == "ycbcr")
                cvCvtColor(&frame, &frame, CV_BGR2YCrCb);
            else if (colourspace == "tsl")
                frame = rgb2tsl(frame);
            
//            getRaw
            if (getRaw)
            {
                for (int i = 0; i < vidHeight; ++i)
                    for (int j = 0; j < vidWidth; ++j)
                        monoframes.at<double>(i, j, k) = frame.at<Vec3b>(i, j)[colour_channel];
            }
            else
            {
//                monoframes(:, :, k) = squeeze(double(colorframe(:, :, colour_channel)));
//                 Downsample the frame for ease of computation
//                    monoframe = filt_img(squeeze(double(colorframe(:, :, colour_channel))));
//                monoframes(:, :, k) = monoframe(1 : 4 : end, 1 : 4 : end);
                
                Mat monoframe = Mat(vidHeight, vidWidth, CV_32FC1, 0);
//                monoframes(:, :, k) = corrDn(monoframe, filt, 'reflect1', [4 4], [1 1], size(monoframe));
            }
        }
        
//        Block 2 ==== Extract a signal stream & pre-process it
//        Convert the frame stream into a 1-D signal
        double temporal_mean;
        Mat debug_frames2signal;
        frames2signal(monoframes, conversion_method, frameRate, cutoff_freq, temporal_mean, debug_frames2signal);
        std::vector<int>{1, 2, 3, 4, 5, 6};
        
//        Block 3 ==== Heart-rate calculation
//        Set peak-detection params
        
//        threshold = threshold_fraction * max(temporal_mean(firstSample : end));
//        minPeakDistance = round(60 / max_bpm * fr);
        
        
        
//        Calculate heart-rate using peak-detection on the signal
        
//        [avg_hr_pda, debug_pda] = hr_calc_pda(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
        
        
        
//        Calculate heart-rate using peak-detection on the signal
        
//        [avg_hr_autocorr, debug_autocorr] = hr_calc_autocorr(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance);
        
    
        Mat src, dst;
        cvCvtColor(&src, &dst, CV_BGR2YCrCb);
        
        return dst;
    }

}