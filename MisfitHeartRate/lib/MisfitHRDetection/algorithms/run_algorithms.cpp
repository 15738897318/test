//
//  run_algorithms.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "run_algorithms.h"
#include "CV2ImageProcessor.h"
#include "SelfCorrPeakHRCounter.h"

namespace MHR {
    hrResult run_algorithms(const String &srcDir, const String &outDir, hrResult &currHrResult) {
        clock_t t1 = clock();
        CV2ImageProcessor *proc = new CV2ImageProcessor();
        SelfCorrPeakHRCounter *hrCounter = new SelfCorrPeakHRCounter();
        proc->setSrcDir(srcDir.c_str());
        proc->setDstDir(outDir.c_str());
        proc->readFrameInfo();
        if (!MHR::_FACE_MODE) {
            hrCounter->setFingerParameter();
            proc->setFingerParams();
        }
        else {
            hrCounter->setFaceParameters();
            proc->setFaceParams();
        }
        
        
        int nFrames = proc->getNFrame();

        // Block 1: turn frames to signals
        vector<double> temporal_mean;
        temporal_mean.reserve(nFrames);
        Mat tmp_eulerianVid;
        
        while(1) {
            /*-----------------read M frames, add to odd frames (0)-----------------*/
            proc->readFrames();
            
            /*-----------------turn eulerianLen (1) frames to signals-----------------*/
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
            vector<double> temporal_mean_filt = hrCounter->getTemporalMeanFilt();
            for (int i = 0, sz = (int)temporal_mean_filt.size(); i < sz; ++i)
                fprintf(resultFile, "%lf, ", temporal_mean_filt[i]);
            fprintf(resultFile, "\n\n\n");
            String vidType = "mp4";
            fprintf(resultFile, "Heart Rate result {autocorr, pda} = {%lf, %lf}\n", currHrResult.autocorr, currHrResult.pda);
            fclose(resultFile);
            printf("run_algorithm() runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
        }

        return currHrResult;
    }
}