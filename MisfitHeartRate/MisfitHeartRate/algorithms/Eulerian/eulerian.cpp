//
//  eulerian.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "eulerian.h"


namespace MHR {
	// Spatial Filtering: Gaussian blur and down sample
	// Temporal Filtering: Ideal bandpass
    void amplifySpatialGdownTemporalIdeal(const vector<Mat> &vid, vector<Mat> &ans,
                                          String outDir, double alpha, int level,
                                          double freqBandLowEnd, double freqBandHighEnd,
                                          double samplingRate, double chromAttenuation)
	{
        clock_t t1 = clock();

        ans.clear();        
		// Extract video info
		int vidHeight = vid[0].rows;
		int vidWidth = vid[1].cols;
		int nChannel = _number_of_channels;		// should get from vid?
		int frameRate = _frameRate;             // Can not get it from vidIn!!!! :((
		int len = (int)vid.size();
        
        printf("width = %d, height = %d\n", vidWidth, vidHeight);
        printf("frameRate = %d, len = %d\n", frameRate, len);
        
        frameToFile(vid[0], outDir + "test_frame_in.jpg");
      
		samplingRate = frameRate;
		level = min(level, (int)floor(log(min(vidHeight, vidWidth) / _Gpyr_filter_length) / log(2)));
        
		// Define the indices of the frames to be processed
		int startIndex = _startFrame;
        int endIndex = len - 1;
        if (_endFrame > 0)
            endIndex = min(endIndex, _endFrame);
        else
            endIndex = max(0, endIndex + _endFrame);
        
		// ================= Core part of the algo described in literature
		// compute Gaussian blur stack
		// This stack actually is just a single level of the pyramid
		printf("Spatial filtering...\n");
		vector<Mat> GdownStack;
        build_Gdown_Stack(vid, GdownStack, startIndex, endIndex, level);
		printf("Finished\n");
        
		// Temporal filtering
		printf("Temporal filtering...\n");
        vector<Mat> filteredStack;
        ideal_bandpassing(GdownStack, filteredStack, freqBandLowEnd, freqBandHighEnd, samplingRate);
//        filter_bandpassing(GdownStack, filteredStack);
		printf("Finished\n");
        
        
		// amplify
        int nTime = (int)filteredStack.size();
        int nRow = filteredStack[0].rows;
        int nCol = filteredStack[0].cols;
        Mat base_B = (Mat_<double>(3, 3) <<
                      alpha, 0, 0,
                      0, alpha*chromAttenuation, 0,
                      0, 0, alpha*chromAttenuation);
        Mat base_C = (ntsc2rgb_baseMat * base_B) * rgb2ntsc_baseMat;
        Mat tmp = Mat::zeros(nChannel, nCol, CV_64F);
		for (int t = 0; t < nTime; ++t)
            for (int i = 0; i < nRow; ++i) {
                for (int j = 0; j < nCol; ++j)
                    for (int channel = 0; channel < nChannel; ++channel)
                        tmp.at<double>(channel, j) = filteredStack[t].at<Vec3d>(i, j)[channel];
                tmp = base_C * tmp;
                for (int j = 0; j < nCol; ++j)
                    for (int channel = 0; channel < nChannel; ++channel)
                        filteredStack[t].at<Vec3d>(i, j)[channel] = tmp.at<double>(channel, j);
            }
        
		// =================
        
		// Render on the input video
		printf("Rendering...\n");
        printf("startIndex = %d, endIndex = %d, nTime = %d\n", startIndex, endIndex, nTime);
		// output video
		// Convert each frame from the filtered stream to movie frame
        Mat frame, filtered;
        Mat tmp_filtered = Mat::zeros(nRow, nCol, CV_64FC3);
		for (int i = startIndex, k = 0; i <= endIndex && k < nTime; ++i, ++k) {
			// Reconstruct the frame from pyramid stack
			// by removing the singleton dimensions of the kth filtered array
			// since the filtered stack is just a selected level of the Gaussian pyramid
			for (int x = 0; x < nRow; ++x)
				for (int y = 0; y < nCol; ++y)
					tmp_filtered.at<Vec3d>(x, y) = filteredStack[k].at<Vec3d>(x, y);
            
			// Format the image to the right size
			resize(tmp_filtered, filtered, cvSize(vidWidth, vidHeight), 0, 0, INTER_CUBIC);
            
			// Convert the ith frame in the video stream to RGB (double-precision) image
            vid[i].convertTo(frame, CV_64FC3);
            
			// Add the filtered frame to the original frame
            filtered = filtered + frame;
            
            // clip the frame
            for (int i = 0; i < vidHeight; ++i)
                for (int j = 0; j < vidWidth; ++j) {
                    for (int channel = 0; channel < nChannel; ++channel) {
                        double tmp = filtered.at<Vec3d>(i, j)[channel];
                        tmp = min(tmp, 255.0);
                        tmp = max(tmp, 0.0);
                        filtered.at<Vec3d>(i, j)[channel] = tmp;
                    }
                }
            
            ans.push_back(filtered.clone());
		}
        if (DEBUG_MODE)
            printf("amplifySpatialGdownTemporalIdeal() time = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
	}
}