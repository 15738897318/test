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
	vector<Mat> amplifySpatialGdownTemporalIdeal(String vidFile, String outDir,
										  double alpha, int level,
										  double freqBandLowEnd, double freqBandHighEnd,
										  double samplingRate, double chromAttenuation)
	{
		// Get the filename-only part of the full path
		String vidName = vidFile.substr(vidFile.find_last_of('/') + 1);
        
		// Create the output file with full path
        String outFile = outDir + vidName + "-ideal-from-" + std::to_string(freqBandLowEnd)
        + "-to-" + std::to_string(freqBandHighEnd)
        + "-alpha-" + std::to_string(alpha)
        + "-level-" + std::to_string(level)
        + "-chromAtn-" + std::to_string(chromAttenuation)
        + ".mp4";
        printf("outFile = %s", outFile.c_str());
        
        vector<Mat> ans;
		
		// Read video
		VideoCapture vidIn(vidFile);
        if (!vidIn.isOpened())
        {
            printf("%s is not opened!\n", vidFile.c_str());
            return ans;
        }
        
		// Extract video info
        vector<Mat> vid = videoCaptureToVector(vidIn);
		int vidHeight = vid[0].rows;
		int vidWidth = vid[1].cols;
//		int nChannels = 3;		// should get from vid?
		int frameRate = 30;     // Can not get it from vidIn!!!! :((
		int len = (int)vid.size();
        
        
        printf("width = %d, height = %d\n", vidWidth, vidHeight);
        printf("frameRate = %d, len = %d\n", frameRate, len);
        
        frameToFile(vid[0], outDir + "test_frame_in.jpg");
        
		samplingRate = frameRate;
		int filter_length = 5;
		level = min(level, (int)floor(log(min(vidHeight, vidWidth) / filter_length) / log(2)));
        
		// Prepare the output video-writer
//        VideoWriter vidOut(outFile, -1, frameRate, cvSize(vidWidth, vidHeight), true);
		VideoWriter vidOut(outFile, CV_FOURCC('M','J','P','G'), frameRate, cvSize(vidWidth, vidHeight), true);
		if (!vidOut.isOpened()) {
			printf("outFile %s is not opened!\n", outFile.c_str());
			return ans;
		}
        
		// Define the indices of the frames to be processed
		int startIndex = 0;
		int endIndex = len - 10;
        
		// ================= Core part of the algo described in literature
		// compute Gaussian blur stack
		// This stack actually is just a single level of the pyramid
		printf("Spatial filtering...\n");
		Mat GdownStack = buildGDownStack(vid, startIndex, endIndex, level);
		printf("Finished\n");
        //////////////////////////////////////////
        Mat tmpGdownStack = Mat::zeros(GdownStack.size.p[1], GdownStack.size.p[2], CV_64FC3);
        for (int i = 0; i < GdownStack.size.p[1]; ++i)
            for (int j = 0; j < GdownStack.size.p[2]; ++j)
                tmpGdownStack.at<Vec3d>(i, j) = GdownStack.at<Vec3d>(0, i, j);
        frameToFile(tmpGdownStack, "/var/mobile/Applications/64B8F9E2-660D-4F0F-8B6C-870F6CC686E8/Documents/test_GdownStack.jpg");
        //////////////////////////////////////////
		// Temporal filtering
		printf("Temporal filtering...\n");
        //		Mat filteredStack = idealBandpassing(GdownStack, 1, freqBandLowEnd, freqBandHighEnd, samplingRate);
        int filteredSize[3] = {GdownStack.size.p[0]-7, GdownStack.size.p[1], GdownStack.size.p[2]};
        Mat filteredStack(3, filteredSize, CV_64FC3, CvScalar(0));
        
        double kernelArray[15] = {0.0034, 0.0087, 0.0244, 0.0529, 0.0909, 0.1300, 0.1594,
            0.1704, 0.1594, 0.1300, 0.0909, 0.0529, 0.0244, 0.0087, 0.0034};
        Mat kernel = arrayToMat(kernelArray, 1, 15);
        
        Mat tmp = Mat::zeros(1, GdownStack.size.p[0], CV_64FC3);
        for (int x = 0; x < GdownStack.size.p[1]; ++x)
            for (int y = 0; y < GdownStack.size.p[2]; ++y) {
                for (int t = 0; t < GdownStack.size.p[0]; ++t)
                    //                    for (int channel = 0; channel < 3; ++channel)
                    tmp.at<Vec3d>(0, t) = GdownStack.at<Vec3d>(t, x, y);
                filter2D(tmp, tmp, -1, kernel);   // ???? should do with each channel ????
                for (int t = 7; t < GdownStack.size.p[0]; ++t)
                    filteredStack.at<Vec3d>(t-7, x, y) = tmp.at<Vec3d>(0, t);
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
        frameToFile(tmpFilteredStack, "/var/mobile/Applications/64B8F9E2-660D-4F0F-8B6C-870F6CC686E8/Documents/test_FilteredStack.jpg");
        //////////////////////////////////////////
        
        
		// =================
        
		// Render on the input video
		printf("Rendering...\n");
        
		// output video
		// init
		Mat frame;
		// Convert each frame from the filtered stream to movie frame
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
                frameToFile(filtered, "/var/mobile/Applications/64B8F9E2-660D-4F0F-8B6C-870F6CC686E8/Documents/test_filtered_beforeResize.jpg");
            
			// Format the image to the right size
			resize(filtered, filtered, cvSize(vidWidth, vidHeight), 0, 0, INTER_CUBIC);
            
            if (i == 0)
                frameToFile(filtered, "/var/mobile/Applications/64B8F9E2-660D-4F0F-8B6C-870F6CC686E8/Documents/test_filtered_afterResize.jpg");
            
			// Extract the ith frame in the video stream
            frame = vid[i];
			// Convert the extracted frame to RGB (double-precision) image
			Mat rgbframe = convertTo(frame, CV_64FC3);
            
			// Convert the image from RGB colour-space to NTSC colour-space
			frame = rgb2ntsc(rgbframe);
            
			// Add the filtered frame to the original frame
			filtered = filtered + frame;
            
            if (i == 0)
                frameToFile(filtered, "/var/mobile/Applications/64B8F9E2-660D-4F0F-8B6C-870F6CC686E8/Documents/test_filtered_afterAdd.jpg");
            
			// Convert the colour-space from NTSC back to RGB
			frame = ntsc2rgb(filtered);
            
            if (i == 0)
                frameToFile(filtered, "/var/mobile/Applications/64B8F9E2-660D-4F0F-8B6C-870F6CC686E8/Documents/test_filtered_ntsc2rgb.jpg");
            
            printf("Convert each frame from the filtered stream to movie frame: %d --> %d\n", i, endIndex);
            
			// Clip the values of the frame by 0 and 1
			for (int x = 0; x < frame.rows; ++x)
				for (int y = 0; y < frame.cols; ++y)
					for (int t = 0; t < 3; ++t) {
						double tmp = frame.at<Vec3d>(x, y)[t];
                        tmp = min(tmp, 255.0);
                        tmp = max(tmp, 0.0);
                        frame.at<Vec3d>(x, y)[t] = tmp;
					}
            
            // test frame
            if (i == 0)
                frameToFile(frame, outDir + "test_processed_frame_out.jpg");
            
            // Write the frame into the video as unsigned 8-bit integer array
//            vidOut << frame;
            vidOut << convertTo(frame, CV_8UC3);
            ans.push_back(frame.clone());
		}
        vidOut.release();
		printf("Finished\n");
        return ans;
	}
    
    
	// run Eulerian
	vector<Mat> runEulerian(String srcDir, String fileName, String fileTemplate, String resultsDir) {
		//String file_template = "*Finger*.mp4";
		String inFile = srcDir + "/" + fileName;
		printf("Processing file: %s\n", inFile.c_str());
        
        return amplifySpatialGdownTemporalIdeal(inFile, resultsDir,
                                                _eulerian_alpha, _eulerian_pyrLevel,
                                                _eulerian_minHR/60.0, _eulerian_maxHR/60.0,
                                                _eulerian_frameRate, _eulerian_chromaMagnifier
                                                );
	}
}