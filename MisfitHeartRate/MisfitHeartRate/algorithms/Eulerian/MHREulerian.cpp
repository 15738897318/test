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
        String outFile = outDir + vidName + "-ideal-from-" + std::to_string(freqBandLowEnd)
        + "-to-" + std::to_string(freqBandHighEnd)
        + "-alpha-" + std::to_string(alpha)
        + "-level-" + std::to_string(level)
        + "-chromAtn-" + std::to_string(chromAttenuation)
        + ".mp4";
        
        printf("outFile = %s", outFile.c_str());
		
		// Read video
		VideoCapture vidIn(vidFile);
        if (!vidIn.isOpened())
        {
            printf("%s is not opened!\n", vidFile.c_str());
            return;
        }
        
		// Extract video info
        vector<Mat> vid = videoCaptureToVector(vidIn);
		int vidHeight = vid[0].rows;
		int vidWidth = vid[1].cols;
		int nChannels = 3;		// should get from vid?
		int frameRate = 30;     // Can not get it from vidIn!!!! :((
		int len = vid.size();
        

        printf("width = %d, height = %d\n", vidWidth, vidHeight);
        printf("frameRate = %d, len = %d\n", frameRate, len);

        
		samplingRate = frameRate;
		int filter_length = 5;
		level = min(level, (int)floor(log(min(vidHeight, vidWidth) / filter_length) / log(2)));
        
		// Prepare the output video-writer
//        VideoWriter vidOut(outFile, -1, frameRate, cvSize(vidWidth, vidHeight), true);
		VideoWriter vidOut(outFile, CV_FOURCC('M','J','P','G'), frameRate, cvSize(vidWidth, vidHeight), true);
		if (!vidOut.isOpened()) {
			printf("outFile %s is not opened!\n", outFile.c_str());
			return;
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
		// =================
        
		// Render on the input video
		printf("Rendering...\n");
        
		// output video
		// init
		Mat frame;
		// Convert each frame from the filtered stream to movie frame
		for (int i = startIndex, k = 0; i <= endIndex && k <= filteredStack.size.p[0]; ++i, ++k) {
			// Reconstruct the frame from pyramid stack
			// by removing the singleton dimensions of the kth filtered array
			// since the filtered stack is just a selected level of the Gaussian pyramid
			Mat filtered = Mat::zeros(filteredStack.size.p[1], filteredStack.size.p[2], CV_64FC3);
            
            printf("filteredStack size = (%d, %d)\n", filteredStack.size.p[1], filteredStack.size.p[2]);
            
			for (int x = 0; x < filteredStack.size.p[1]; ++x)
				for (int y = 0; y < filteredStack.size.p[2]; ++y)
					filtered.at<Vec3d>(x, y) = filteredStack.at<Vec3d>(k, x, y);
            
			// Format the image to the right size
			resize(filtered, filtered, cvSize(vidWidth, vidHeight), 0, 0, INTER_CUBIC);
            
			// Extract the ith frame in the video stream
            frame = vid[i];
			// Convert the extracted frame to RGB (double-precision) image
			Mat rgbframe = convertTo(frame, CV_64FC3);
            
			// Convert the image from RGB colour-space to NTSC colour-space
			frame = rgb2ntsc(rgbframe);
            
			// Add the filtered frame to the original frame
			filtered = filtered + frame;
            
			// Convert the colour-space from NTSC back to RGB
			frame = ntsc2rgb(filtered);
            
            printf("Convert each frame from the filtered stream to movie frame: %d --> %d\n", i, endIndex);
            
			// Clip the values of the frame by 0 and 1
			for (int x = 0; x < frame.rows; ++x)
				for (int y = 0; y < frame.cols; ++y)
					for (int t = 0; t < 3; ++t) {
						double tmp = frame.at<Vec3d>(x, y)[t];
						if (tmp > 1)
							frame.at<Vec3d>(x, y)[t] = 1;
						else if (tmp < 0)
							frame.at<Vec3d>(x, y)[t] = 0;
					}
            
			// Write the frame into the video as unsigned 8-bit integer array
            vidOut << convertTo(frame, CV_8UC3);
		}
        vidOut.release();
		printf("Finished\n");
	}
    
    
	// run Eulerian
	void runEulerian(String srcDir, String fileName, String fileTemplate, String resultsDir) {
		//String resultsDir = "Results";
		//String src_folder = "/Users/storm5906/Desktop/eulerianMagnifcation/codeMatlab/";
		//String file_template = "*Finger*.mp4";
		String inFile = srcDir + "/" + fileName;
		printf("Processing file: %s\n", inFile.c_str());
        
		vector<int> alpha = vectorRange(30, 30);            // Eulerian magnifier
		vector<int> pyrLevel = vectorRange(6, 6);           // Standard: 4, but updated by the real frame size
		vector<int> minHR = vectorRange(30, 30);            // BPM Standard: 50
		vector<int> maxHR = vectorRange(240, 240);          // BPM Standard: 90
		vector<int> frameRate = vectorRange(30, 30);        // Standard: 30, but updated by the real frame-rate
		vector<int> chromaMagnifier = vectorRange(1, 1);    // Standard: 1
        
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