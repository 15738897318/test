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
        clock_t t1 = clock();
        
        String inFile = srcDir + "/" + fileName;
        if (DEBUG_MODE) printf("Processing file: %s\n", inFile.c_str());
        
        // Get the filename-only part of the full path
		String vidName = inFile.substr(inFile.find_last_of('/') + 1);
        
		// Create the output file with full path
        String outFile = outDir + vidName 
							+ "-ideal-from-" + std::to_string(_eulerian_minHR/60.0)
							+ "-to-" + to_string(_eulerian_maxHR/60.0)
							+ "-alpha-" + to_string(_eulerian_alpha)
							+ "-level-" + to_string(_eulerian_pyrLevel)
							+ "-chromAtn-" + to_string(_eulerian_chromaMagnifier)
							+ ".mp4";
        if (DEBUG_MODE) printf("outFile = %s\n", outFile.c_str());
        
		// Read video
		VideoCapture vidIn(inFile);
        if (!vidIn.isOpened())
        {
            if (DEBUG_MODE) printf("%s is not opened!\n", inFile.c_str());
            return hrResult(-1, -1);
        }
        
        /*-----------------read the first block of M frames to extract video params---------------*/
        vector<Mat> vid;
        videoCaptureToVector(vidIn, vid, _framesBlock_size);
        
        // Extract video info
        int vidHeight = vid[0].rows;
        int vidWidth = vid[0].cols;
        int frameRate = _frameRate;
        int len = (int)vid.size();
        
        // Create an output video based on the input video's params
    	VideoWriter vidOut;
        if (DEBUG_MODE > 0 && WRITE_EULERIAN_VID_MODE > 0) {
            vidOut.open(outFile, CV_FOURCC('M','P','4','2'), frameRate, cvSize(vidWidth, vidHeight), true);
            if (!vidOut.isOpened()) {
                 printf("outFile %s is not opened!\n", outFile.c_str());
                return hrResult(-1, -1);
            }
        }
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Block 1: turn frames to signals
        double threshold_fraction = 0, lower_range, upper_range;
        int window_size = round(_window_size_in_sec * frameRate);
        int firstSample = round(frameRate * _time_lag);
        int blockCount = 1;
        bool isCalcMode = true;
        vector<Mat> monoframes, debug_monoframes, eulerianVid;
        vector<double> temporal_mean;
        Mat tmp_eulerianVid;
        
        while(1) {
            clock_t t1 = clock();
            
            if (DEBUG_MODE) printf("len before = %d\n", (int)vid.size());
            bool endOfFile = false;
            
            /*-----------------read M frames, add to odd frames (0)-----------------*/
            if (!isCalcMode) {
                endOfFile = videoCaptureToVector(vidIn, vid, _framesBlock_size);
                len = (int)vid.size();
                if (DEBUG_MODE) printf("len after = %d\n", len);
            }
            if (endOfFile) break;
            
            if (DEBUG_MODE) printf("load block: %d\n", blockCount);
            
            /*-----------------run_eulerian(): M frames (1)-----------------*/
            eulerianGaussianPyramidMagnification(vid, eulerianVid,
												 outDir, _eulerian_alpha, _eulerian_pyrLevel,
												 _eulerian_minHR/60.0, _eulerian_maxHR/60.0,
												 _eulerian_frameRate, _eulerian_chromaMagnifier);
            
            /*-----------------keep last 15 frames if using filter_bandpassing for Eulerian stage (0)-----------------*/
            // need to improve
            vector<Mat> newVid;
            int eulerianLen = (int)eulerianVid.size();
            int startPos = len;
            if (eulerianLen != len)
                startPos -= _eulerianTemporalFilterKernel_size;
            for (int i = startPos; i < len; ++i)
                newVid.push_back(vid[i]);
            vid.clear();
            vid = newVid;
            
            /*-----------------write frames to file in DEBUG_MODE, WRITE_EULERIAN_VID_MODE-----------------*/
            if (DEBUG_MODE > 0 && WRITE_EULERIAN_VID_MODE > 0) {
                for (int i = isCalcMode ? 0:(len-startPos); i < eulerianLen; ++i) {
                    eulerianVid[i].convertTo(tmp_eulerianVid, CV_8UC3);
                    cvtColor(tmp_eulerianVid, tmp_eulerianVid, CV_RGB2BGR);
                    vidOut << tmp_eulerianVid;
                }
            }
            
            /*-----------------turn eulerianLen (1) frames to signals-----------------*/
            
            vector<double> tmp = temporal_mean_calc(eulerianVid, _overlap_ratio, _max_bpm, _cutoff_freq,
                                                    _channels_to_process, _colourspace,
                                                    lower_range, upper_range, isCalcMode);
            for (int i = 0; i < eulerianLen; ++i)
                temporal_mean.push_back(tmp[i]);
            
            isCalcMode = false;

//            if (DEBUG_MODE)
                printf("block %d runtime = %f\n", blockCount++, ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
                printf("_colourspace = %s\n", _colourspace.c_str());
        }
        vidOut.release();
                
        // Low-pass-filter the signal stream to remove unwanted noises
        vector<double> temporal_mean_filt = low_pass_filter(temporal_mean);
        
        // Block 2: Heart-rate calculation
        // - Basis takes 15secs to generate an HR estimate
        // - Cardiio takes 30secs to generate an HR estimate
        hrResult hr_output = hr_signal_calc(temporal_mean_filt, firstSample, window_size, frameRate,
                                            _overlap_ratio, _max_bpm, threshold_fraction);
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        if (DEBUG_MODE) {
            String resultFilePath = outDir + "result.txt";
            FILE *resultFile = fopen(resultFilePath.c_str(), "w");
            fprintf(resultFile, "temporal_mean_before_low_pass_filter:\n");
            for (int i = 0, sz = (int)temporal_mean.size(); i < sz; ++i)
                fprintf(resultFile, "%lf, ", temporal_mean[i]);
            fprintf(resultFile, "\n\ntemporal_mean:\n");
            for (int i = 0, sz = (int)temporal_mean_filt.size(); i < sz; ++i)
                fprintf(resultFile, "%lf, ", temporal_mean_filt[i]);
            fprintf(resultFile, "\n\n\n");
            String vidType = "mp4";
            fprintf(resultFile, "run_hr(vidType = %s, colourspace = %s, min_hr = %lf, max_hr = %lf, \
										alpha = %lf, level = %lf, chromAtn = %lf)\n",
                   	vidType.c_str(), _colourspace.c_str(), _eulerian_minHR, _eulerian_maxHR,
                   	_eulerian_alpha, _eulerian_pyrLevel, _eulerian_chromaMagnifier);
            fprintf(resultFile, "Heart Rate result {autocorr, pda} = {%lf, %lf}\n", hr_output.autocorr, hr_output.pda);
            fclose(resultFile);
            printf("run_algorithm() runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
        }
        
        return hr_output;
    }
}