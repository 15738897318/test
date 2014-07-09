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
//		int nChannels = _number_of_channels;		// should get from vid?
		int frameRate = _frameRate;                 // Can not get it from vidIn!!!! :((
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
		Mat GdownStack = build_Gdown_Stack(vid, startIndex, endIndex, level);
		printf("Finished\n");
        //////////////////////////////////////////
        Mat tmpGdownStack = Mat::zeros(GdownStack.size.p[1], GdownStack.size.p[2], CV_64FC3);
        for (int i = 0; i < GdownStack.size.p[1]; ++i)
            for (int j = 0; j < GdownStack.size.p[2]; ++j)
                tmpGdownStack.at<Vec3d>(i, j) = GdownStack.at<Vec3d>(0, i, j);
        frameToFile(tmpGdownStack, outDir + "test_GdownStack.jpg");
        //////////////////////////////////////////
        
		// Temporal filtering
		printf("Temporal filtering...\n");
        Mat filteredStack;
        ideal_bandpassing(GdownStack, filteredStack, freqBandLowEnd, freqBandHighEnd, samplingRate);
//        filter_bandpassing(GdownStack, filteredStack);
		printf("Finished\n");
        
		// amplify
		for (int i = 0; i < filteredStack.size.p[0]; ++i)
			for (int j = 0; j < filteredStack.size.p[1]; ++j)
				for (int k = 0; k < filteredStack.size.p[2]; ++k) {
					filteredStack.at<Vec3d>(i, j, k)[0] *= alpha;
					filteredStack.at<Vec3d>(i, j, k)[1] *= alpha*chromAttenuation;
					filteredStack.at<Vec3d>(i, j, k)[2] *= alpha*chromAttenuation;
				}
        //////////////////////////////////////////
        Mat tmpFilteredStack = Mat::zeros(GdownStack.size.p[1], GdownStack.size.p[2], CV_64FC3);
        for (int i = 0; i < filteredStack.size.p[1]; ++i)
            for (int j = 0; j < filteredStack.size.p[2]; ++j)
                tmpFilteredStack.at<Vec3d>(i, j) = filteredStack.at<Vec3d>(0, i, j);
        frameToFile(tmpFilteredStack, outDir + "test_FilteredStack.jpg");
        //////////////////////////////////////////
        
		// =================
        
		// Render on the input video
		printf("Rendering...\n");
        
		// output video
		// Convert each frame from the filtered stream to movie frame
        Mat rgbframe, filtered, rgbFiltered;
        Mat tmp_filtered = Mat::zeros(filteredStack.size.p[1], filteredStack.size.p[2], CV_64FC3);
		for (int i = startIndex, k = 0; i <= endIndex && k < filteredStack.size.p[0]; ++i, ++k) {
			// Reconstruct the frame from pyramid stack
			// by removing the singleton dimensions of the kth filtered array
			// since the filtered stack is just a selected level of the Gaussian pyramid
			for (int x = 0; x < filteredStack.size.p[1]; ++x)
				for (int y = 0; y < filteredStack.size.p[2]; ++y)
					tmp_filtered.at<Vec3d>(x, y) = filteredStack.at<Vec3d>(k, x, y);
            
            printf("filteredStack size = (%d, %d)\n", filteredStack.size.p[1], filteredStack.size.p[2]);
            if (i == 0)
                frameToFile(tmp_filtered, outDir + "test_filtered_beforeResize.jpg");
            
			// Format the image to the right size
			resize(tmp_filtered, filtered, cvSize(vidWidth, vidHeight), 0, 0, INTER_CUBIC);
            
            if (i == 0)
                frameToFile(filtered, outDir + "test_filtered_after_Resize.jpg");
            
			// Convert the ith frame in the video stream to RGB (double-precision) image
            vid[i].convertTo(rgbframe, CV_64FC3);
            
			// Add the filtered frame to the original frame
            ntsc2rgb(filtered, rgbFiltered);
            filtered = rgbFiltered + rgbframe;
            
            if (i == 0) {
                frameToFile(rgbFiltered, outDir + "test_filtered_before_Add.jpg");
                frameToFile(filtered, outDir + "test_processed_frame.jpg");
            }
            
            printf("Convert each frame from the filtered stream to movie frame: %d --> %d\n", i, endIndex);
            ans.push_back(filtered.clone());
//            vid[i] = filtered.clone();
//            vid[i].Mat::~Mat();
		}
		printf("Finished\n");

        clock_t t2 = clock();
        printf("amplifySpatialGdownTemporalIdeal() time = %f\n", ((float)t2 - (float)t1)/CLOCKS_PER_SEC);
	}
}