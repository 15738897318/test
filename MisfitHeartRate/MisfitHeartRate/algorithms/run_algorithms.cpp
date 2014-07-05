//
//  run_algorithms.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "run_algorithms.h"


namespace MHR {
    hrResult run_algorithms(const String &srcDir, const String &fileName, const String &outDir)
    {
        String inFile = srcDir + "/" + fileName;
		printf("Processing file: %s\n", inFile.c_str());
        
        // Get the filename-only part of the full path
		String vidName = inFile.substr(inFile.find_last_of('/') + 1);
        
		// Create the output file with full path
        String outFile = outDir + vidName + "-ideal-from-" + std::to_string(_eulerian_minHR/60.0)
        + "-to-" + to_string(_eulerian_maxHR/60.0)
        + "-alpha-" + to_string(_eulerian_alpha)
        + "-level-" + to_string(_eulerian_pyrLevel)
        + "-chromAtn-" + to_string(_eulerian_chromaMagnifier)
        + ".mp4";
        printf("outFile = %s\n", outFile.c_str());
        
		// Read video
		VideoCapture vidIn(inFile);
        if (!vidIn.isOpened())
        {
            printf("%s is not opened!\n", inFile.c_str());
            return hrResult(-1, -1);
        }
        
        vector<Mat> vid = videoCaptureToVector(vidIn, _framesBlock_size);
        // Extract video info
        int vidHeight = vid[0].rows;
        int vidWidth = vid[1].cols;
//        int nChannels = _number_of_channels;		// should get from vid?
        int frameRate = _frameRate;
        int len = (int)vid.size();
        
//        printf("!!!!!!!!! len = %d\n", len);
//        bool end = videoCaptureToVector(vidIn, vid, _framesBlock_size);
//        printf("!!!!!!!!! end = %d\n", end);
//        printf("!!!!!!!!! len = %d\n", (int)vid.size());
//        return hrResult(-1, -1);
        
        // Prepare the output video-writer
//        VideoWriter vidOut(outFile, -1, frameRate, cvSize(vidWidth, vidHeight), true);
		VideoWriter vidOut(outFile, CV_FOURCC('M','J','P','G'), frameRate, cvSize(vidWidth, vidHeight), true);
		if (!vidOut.isOpened()) {
			printf("outFile %s is not opened!\n", outFile.c_str());
			return hrResult(-1, -1);
		}
        
        // Block 1: turn frames to signals
        double threshold_fraction = 0;
        int window_size = round(_window_size_in_sec * frameRate);
        int firstSample = round(frameRate * _time_lag);
        
        double lower_range, upper_range;
        bool isCalcMode = true;

        vector<Mat> ans;
        vector<Mat> monoframes, debug_monoframes;
        vector<double> temporal_mean;
        
        int c = 0;
        while(1) {
            printf("len before = %d\n", (int)vid.size());
            bool endOfFile = false;
            if (!isCalcMode) {
            /*-----------------------------------read M frames, add to odd frames (0)-----------------------------------*/
                endOfFile = videoCaptureToVector(vidIn, vid, _framesBlock_size);
                len = (int)vid.size();
                printf("len after = %d\n", len);
            }
            if (endOfFile)
                break;
            
            printf("load block: %d\n", ++c);
            
            /*-----------------------------------run_eulerian(): M frames (1)-----------------------------------*/
            vector<Mat> eulerianVid = amplifySpatialGdownTemporalIdeal(vid, outDir,
                                                                       _eulerian_alpha, _eulerian_pyrLevel,
                                                                       _eulerian_minHR/60.0, _eulerian_maxHR/60.0,
                                                                       _eulerian_frameRate, _eulerian_chromaMagnifier
                                                                       );
            
            // Write the frame into the video as unsigned 8-bit integer array
            for (int i = isCalcMode ? 0:15, sz = (int)eulerianVid.size(); i < sz; ++i) {
//              vidOut << frame;
                vidOut << convertTo(eulerianVid[i], CV_8UC3);
            }
        
            /*-----------------------------------turn M-7 (1) frames to signals-----------------------------------*/
            vector<double> tmp = temporal_mean_calc(eulerianVid, _overlap_ratio, _max_bpm, _cutoff_freq,
                                                    _channels_to_process, _colourspace,
                                                    lower_range, upper_range, isCalcMode);
            for (int i = 0, sz = (int)tmp.size(); i < sz - _eulerianTemporalFilterKernel_size/2; ++i)
                temporal_mean.push_back(tmp[i]);
            isCalcMode = false;
            
            /*-----------------------------------keep last 15 frames (0)-----------------------------------*/
            // need to improve
            vector<Mat> newVid;
            for (int i = len-_eulerianTemporalFilterKernel_size; i < len; ++i)
                newVid.push_back(vid[i]);
            vid.clear();
            vid = newVid;
        }
        vidOut.release();
        
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        printf("temporal_mean:\n");
        for (int i = 0, sz = temporal_mean.size(); i < sz; ++i)
            printf("%lf, ", temporal_mean[i]);
        printf("\n");
        
        // Block 2: Heart-rate calculation
        // - Basis takes 15secs to generate an HR estimate
        // - Cardiio takes 30secs to generate an HR estimate
        hrResult hr_output = hr_signal_calc(temporal_mean, firstSample, window_size, frameRate,
                                            _overlap_ratio, _max_bpm, threshold_fraction);
        
        // debug info
        String vidType = "mp4";
        printf("run_hr(vidType = %s, colourspace = %s, min_hr = %lf, max_hr = %lf, \
alpha = %lf, level = %lf, chromAtn = %lf)\n",
               vidType.c_str(), _colourspace.c_str(), _eulerian_minHR, _eulerian_maxHR,
               _eulerian_alpha, _eulerian_pyrLevel, _eulerian_chromaMagnifier);
        printf("Heart Rate result {autocorr, pda} = {%lf, %lf}\n", hr_output.autocorr, hr_output.pda);
        return hr_output;
    }
}
