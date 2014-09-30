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
    SelfCorrPeakHRCounter *hrCounter = new SelfCorrPeakHRCounter();
    proc->setSrcDir(srcDir.c_str());
    proc->setDstDir(outDir.c_str());
    proc->readFrameInfo();
    if (!_FACE_MODE) proc->setFingerParams();
    int nFrames = proc->getNFrame();

        // Block 1: turn frames to signals
        vector<double> temporal_mean;
        vector<double> temporal_mean_filt;
        Mat tmp_eulerianVid;
        
        while(1) {
            /*-----------------read M frames, add to odd frames (0)-----------------*/
            proc->readFrames();
            
            /*-----------------turn eulerianLen (1) frames to signals-----------------*/
//            vector<double> tmp = temporal_mean_calc(proc->getEulerienVid(), _overlap_ratio, _max_bpm, _cutoff_freq,
//                                                    _channels_to_process, _colourspace,
//                                                    lower_range, upper_range, isCalcMode);
//            for (int i = 0; i < tmp.size(); ++i)
//                temporal_mean.push_back(tmp[i]);

            proc->writeArray(temporal_mean);
            if (proc->getCurrentFrame() == nFrames - 1) break;
            
			// Block 2: Heart-rate calculation
			// - Basis takes 15secs to generate an HR estimate
			// - Cardiio takes 30secs to generate an HR estimate
			currHrResult = hrCounter->getHR(temporal_mean);
            hrGlobalResult = currHrResult;
            
            if (_DEBUG_MODE) {
                printf("%lf %lf\n",currHrResult.autocorr,currHrResult.pda);
            }
        }


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (_DEBUG_MODE) {
            // print the result to file result.txt
            String resultFilePath = outDir + "result.txt";
            FILE *resultFile = fopen(resultFilePath.c_str(), "a");
            fprintf(resultFile, "temporal_mean_before_low_pass_filter:\n");
            for (int i = 0, sz = (int)temporal_mean.size(); i < sz; ++i)
                fprintf(resultFile, "%lf, ", temporal_mean[i]);
            fprintf(resultFile, "\n\ntemporal_mean:\n");
            temporal_mean_filt = hrCounter->getTemporalMeanFilt();
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