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
	vector<Mat> amplifySpatialGdownTemporalIdeal(const vector<Mat> &vid, String outDir,
                                                 double alpha, int level,
                                                 double freqBandLowEnd, double freqBandHighEnd,
                                                 double samplingRate, double chromAttenuation)
	{
        clock_t t1 = clock();
        vector<Mat> ans;
        
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
        //		Mat filteredStack = idealBandpassing(GdownStack, 1, freqBandLowEnd, freqBandHighEnd, samplingRate);
        int firstFramesRemove = _eulerianTemporalFilterKernel_size/2;
        int filteredSize[3] = {GdownStack.size.p[0] - firstFramesRemove, GdownStack.size.p[1], GdownStack.size.p[2]};
        Mat filteredStack(3, filteredSize, CV_64FC3, CvScalar(0));
        
        Mat kernel = arrayToMat(_eulerianTemporalFilterKernel, 1, _eulerianTemporalFilterKernel_size);
        Mat tmp = Mat::zeros(1, GdownStack.size.p[0], CV_64FC3);
        for (int x = 0; x < GdownStack.size.p[1]; ++x)
            for (int y = 0; y < GdownStack.size.p[2]; ++y) {
                for (int t = 0; t < GdownStack.size.p[0]; ++t)
//                    for (int channel = 0; channel < 3; ++channel)
                    tmp.at<Vec3d>(0, t) = GdownStack.at<Vec3d>(t, x, y);
                filter2D(tmp, tmp, -1, kernel);
                for (int t = firstFramesRemove; t < GdownStack.size.p[0]; ++t)
                    filteredStack.at<Vec3d>(t - firstFramesRemove, x, y) = tmp.at<Vec3d>(0, t);
            }
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
        Mat frame, rgbframe;
		for (int i = startIndex, k = 0; i <= endIndex && k < filteredStack.size.p[0]; ++i, ++k) {
			// Reconstruct the frame from pyramid stack
			// by removing the singleton dimensions of the kth filtered array
			// since the filtered stack is just a selected level of the Gaussian pyramid
			Mat filtered = Mat::zeros(filteredStack.size.p[1], filteredStack.size.p[2], CV_64FC3);
			for (int x = 0; x < filteredStack.size.p[1]; ++x)
				for (int y = 0; y < filteredStack.size.p[2]; ++y)
					filtered.at<Vec3d>(x, y) = filteredStack.at<Vec3d>(k, x, y);
            
            printf("filteredStack size = (%d, %d)\n", filteredStack.size.p[1], filteredStack.size.p[2]);
            if (i == 0)
                frameToFile(filtered, outDir + "test_filtered_beforeResize.jpg");
            
			// Format the image to the right size
			resize(filtered, filtered, cvSize(vidWidth, vidHeight), 0, 0, INTER_CUBIC);
            
            if (i == 0)
                frameToFile(filtered, outDir + "test_filtered_afterResize.jpg");
            
			// Extract the ith frame in the video stream
            frame = vid[i];
			// Convert the extracted frame to RGB (double-precision) image
            frame.convertTo(rgbframe, CV_64FC3);
            
			// Convert the image from RGB colour-space to NTSC colour-space
            rgb2ntsc(rgbframe, frame);
            
			// Add the filtered frame to the original frame
			filtered = filtered + frame;
            
            if (i == 0)
                frameToFile(filtered, outDir + "test_filtered_afterAdd.jpg");
            
			// Convert the colour-space from NTSC back to RGB
			ntsc2rgb(filtered, frame);
            
            if (i == 0)
                frameToFile(filtered, outDir + "test_filtered_ntsc2rgb.jpg");
            
            printf("Convert each frame from the filtered stream to movie frame: %d --> %d\n", i, endIndex);
            
			// Clip the values of the frame by 0 and 1
//			for (int x = 0; x < frame.rows; ++x)
//				for (int y = 0; y < frame.cols; ++y)
//					for (int t = 0; t < nChannels; ++t) {
//						double tmp = frame.at<Vec3d>(x, y)[t];
//                        tmp = min(tmp, 255.0);
//                        tmp = max(tmp, 0.0);
//                        frame.at<Vec3d>(x, y)[t] = tmp;
//					}
            
            // test frame
            if (i == 0)
                frameToFile(frame, outDir + "test_processed_frame_out.jpg");
            ans.push_back(frame.clone());
		}
		printf("Finished\n");

        clock_t t2 = clock();
        printf("amplifySpatialGdownTemporalIdeal() time = %f\n", ((float)t2 - (float)t1)/1000.0);
        return ans;
	}
}