//
//  run_algorithms.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "run_algorithms.h"
#include "CV2ImageProcessor.h"


namespace MHR {
    hrResult run_algorithms(const String &srcDir, const String &outDir, hrResult &currHrResult)
    {
        clock_t t1 = clock();
    CV2ImageProcessor *proc = new CV2ImageProcessor();
    proc->setSrcDir(srcDir.c_str());
    proc->setDstDir(outDir.c_str());
    proc->readFrameInfo();
    int nFrames = proc->getNFrame();

        // Block 1: turn frames to signals
        int currentFrame = -1;
        bool isCalcMode = true;
        int window_size = round(_window_size_in_sec * _frameRate);
        int firstSample = round(_frameRate * _time_lag);
        double threshold_fraction = 0;
        vector<double> temporal_mean;
        vector<double> temporal_mean_filt;
        Mat tmp_eulerianVid;
        
        while(1) {
            /*-----------------read M frames, add to odd frames (0)-----------------*/
            if (proc->getCurrentFrame() >= nFrames -1) break;
            proc->readFrames();
            
            /*-----------------turn eulerianLen (1) frames to signals-----------------*/
            proc->writeArray(temporal_mean);
            
            isCalcMode = false;
            if (currentFrame == nFrames - 1) break;
            
            /*-----------------Perform HR calculation for the frames processed so far-----------------*/
			// Low-pass-filter the signal stream to remove unwanted noises
			temporal_mean_filt = low_pass_filter(temporal_mean);
            
			// Block 2: Heart-rate calculation
			// - Basis takes 15secs to generate an HR estimate
			// - Cardiio takes 30secs to generate an HR estimate
			currHrResult = hr_signal_calc(temporal_mean_filt, firstSample, window_size, _frameRate,
                                          _overlap_ratio, _max_bpm, threshold_fraction);
            hrGlobalResult = currHrResult;
            
            if (_DEBUG_MODE) {
                printf("%lf %lf\n",currHrResult.autocorr,currHrResult.pda);
            }
        }


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (_DEBUG_MODE) {
            // print the result to file result.txt
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
            fprintf(resultFile, "Heart Rate result {autocorr, pda} = {%lf, %lf}\n", currHrResult.autocorr, currHrResult.pda);
            fclose(resultFile);
            printf("run_algorithm() runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
        }

        return currHrResult;
    }
}