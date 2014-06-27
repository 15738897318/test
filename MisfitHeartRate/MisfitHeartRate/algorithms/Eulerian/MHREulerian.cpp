//
//  MHREulerian.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/25/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "MHREulerian.h"


namespace cv {
	// Spatial Filtering: Gaussian blur and down sample
	// Temporal Filtering: Ideal bandpass
	void amplifySpatialGdownTemporalIdeal(String vidFile, String outDir,
										  double alpha, int level,
										  double freqBandLowEnd, double freqBandHighEnd,
										  double samplingRate, double chromAttenuation)
	{
		// Get the filename-only part of the full path
		String vidName = vidFile.substr(vidFile.find_last_of('/') + 1);
        
		// Create the output file with full path
		String outFile = outDir + "-ideal-from-" + std::to_string(freqBandLowEnd)
        + "-to-" + std::to_string(freqBandHighEnd)
        + "-alpha-" + std::to_string(alpha)
        + "-level-" + std::to_string(level)
        + "-chromAtn-" + std::to_string(chromAttenuation)
        + ".avi";
		
		// Read video
		VideoCapture vid(vidFile);
        
		// Extract video info
		int vidHeight = (int)vid.get(CV_CAP_PROP_FRAME_HEIGHT);
		int vidWidth = (int)vid.get(CV_CAP_PROP_FRAME_WIDTH);
		int nChannels = 3;		// should get from vid?
		int frameRate = (int)vid.get(CV_CAP_PROP_FPS);
		int len = (int)vid.get(CV_CAP_PROP_FRAME_COUNT);
        
		samplingRate = frameRate;
		int filter_length = 5;
		level = min(level, (int)floor(log(min(vidHeight, vidWidth) / filter_length) / log(2)));
        
		// Prepare the output video-writer
		VideoWriter vidOut(outFile, CV_FOURCC('A','E','M','I'), frameRate, cvSize(vidWidth, vidHeight), true);
		if (!vidOut.isOpened()) {
			printf("outFile %s is not opened!", outFile.c_str());
			return;
		}
        
		// Define the indices of the frames to be processed
		int startIndex = 0;
		int endIndex = len - 10;
        
		// ================= Core part of the algo described in literature
		// compute Gaussian blur stack
		// This stack actually is just a single level of the pyramid
		printf("Spatial filtering...\n");
		Mat GdownStack = buildGDownStack(vidFile, startIndex, endIndex, level);
		printf("Finished\n");
        
		// Temporal filtering
		printf("Temporal filtering...\n");
		Mat filteredStack = idealBandpassing(GdownStack, 1, freqBandLowEnd, freqBandHighEnd, samplingRate);
		printf("Finished\n");
        
		// amplify
		for (int i = 0; i < filteredStack.size.p[0]; ++i)
			for (int j = 0; j < filteredStack.size.p[1]; ++j)
				for (int k = 0; k < filteredStack.size.p[2]; ++k) {
					filteredStack.at<Vec3d>(i, j, k)[0] *= alpha;
					filteredStack.at<Vec3d>(i, j, k)[1] *= alpha*chromAttenuation;
					filteredStack.at<Vec3d>(i, j, k)[0] *= alpha*chromAttenuation;
				}
		// =================
        
		// Render on the input video
		printf("Rendering...\n");
        
		// output video
		// init
		Mat frame;
		for (int i = 0; i < startIndex; ++i)
			vid >> frame;
		// Convert each frame from the filtered stream to movie frame
		for (int i = startIndex, k = 1; i <= endIndex; ++i, ++k) {
			// Reconstruct the frame from pyramid stack
			// by removing the singleton dimensions of the kth filtered array
			// since the filtered stack is just a selected level of the Gaussian pyramid
			Mat filtered = Mat::zeros(filteredStack.size.p[1], filteredStack.size.p[2], CV_64F);
			for (int i = 0; i < filteredStack.size.p[1]; ++i)
				for (int j = 0; j < filteredStack.size.p[2]; ++j)
					filtered.at<double>(i, j) = filteredStack.at<double>(k, i, j);
			
			// Format the image to the right size
			resize(filtered, filtered, cvSize(vidHeight, vidWidth), 0, 0, INTER_CUBIC);
            
			// Extract the ith frame in the video stream
			vid >> frame;
			// Convert the extracted frame to RGB (double-precision) image
			Mat rgbframe = convertTo(frame, CV_64FC3);
            
			// Convert the image from RGB colour-space to NTSC colour-space
			frame = rgb2ntsc(rgbframe);
            
			// Add the filtered frame to the original frame
			filtered = filtered + frame;
            
			// Convert the colour-space from NTSC back to RGB
			frame = ntsc2rgb(filtered);
            
			// Clip the values of the frame by 0 and 1
			for (int i = 0; i < frame.rows; ++i)
				for (int j = 0; j < frame.cols; ++j)
					for (int k = 0; k < 3; ++k) {
						double tmp = frame.at<Vec3d>(i, j)[k];
						if (tmp > 1)
							frame.at<Vec3d>(i, j)[k] = 1;
						else if (tmp < 0)
							frame.at<Vec3d>(i, j)[k] = 0;
					}
            
			// Write the frame into the video as unsigned 8-bit integer array
			vidOut.write(convertTo(frame, CV_8UC3));
		}
		printf("Finished\n");
	}
    
    
	// run Eulerian
	void runEulerian(String srcDir, String fileName, String fileTemplate, String resultsDir) {
		//String resultsDir = "Results";
		//String src_folder = "/Users/storm5906/Desktop/eulerianMagnifcation/codeMatlab/";
		//String file_template = "*Finger*.mp4";
		String inFile = srcDir + "\\" + fileName;
		printf("Processing file: %s\n", inFile.c_str());
        
		vector<int> alpha = vectorRange(20, 80, 30);		// Eulerian magnifier
		vector<int> pyrLevel = vectorRange(4, 6, 2);		// Standard: 4, but updated by the real frame size
		vector<int> minHR = vectorRange(30, 50, 10);		// BPM Standard: 50
		vector<int> maxHR = vectorRange(60, 210, 30);		// BPM Standard: 90
		vector<int> frameRate = vectorRange(30, 30);		// Standard: 30, but updated by the real frame-rate
		vector<int> chromaMagnifier = vectorRange(1, 2);	// Standard: 1
        
		// generate all combinations of parameters
		vector<vector<int>> tmp;
		tmp.push_back(alpha);
		tmp.push_back(pyrLevel);
		tmp.push_back(minHR);
		tmp.push_back(maxHR);
		tmp.push_back(frameRate);
		tmp.push_back(chromaMagnifier);
		vector<vector<int>> paramsSet = allcomb(tmp);
        
		// amplify_spatial_Gdown_temporal_ideal each combination
		int nRow = int(paramsSet.size());
		int nCol = int(paramsSet[0].size());
		for (int i = 0; i < nRow; ++i) {
			double currAlpha = paramsSet[i][0];
			int currPyrLevel = paramsSet[i][1];
			double currMinHr = paramsSet[i][2];
			double currMaxHr = paramsSet[i][3];
			double currFrameRate = paramsSet[i][4];
			double currChromaMagnifier = paramsSet[i][5];
			amplifySpatialGdownTemporalIdeal(inFile, resultsDir,
                                             currAlpha, currPyrLevel,
                                             currMinHr/60.0, currMaxHr/60.0,
                                             currFrameRate, currChromaMagnifier
                                             );
		}
	}
}