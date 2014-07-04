//
//  run_algorithms.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "run_algorithms.h"


namespace MHR {
    hrResult run_algorithms(const String &srcDir, const String &fileName, const String &resultsDir)
    {
        String inFile = srcDir + "/" + fileName;
		printf("Processing file: %s\n", inFile.c_str());
        
        
        /*-----------------------------------run_eulerian()-----------------------------------*/
        clock_t t1 = clock();
        vector<Mat> vid = amplifySpatialGdownTemporalIdeal(inFile, resultsDir,
                                                           _eulerian_alpha, _eulerian_pyrLevel,
                                                           _eulerian_minHR/60.0, _eulerian_maxHR/60.0,
                                                           _eulerian_frameRate, _eulerian_chromaMagnifier
                                                           );
        clock_t t2 = clock();
        printf("runEulerian() time = %f\n", ((float)t2 - (float)t1)/1000.0);
        
        
        /*-----------------------------------run_hr()-----------------------------------*/
        t1 = clock();
        // - Basis takes 15secs to generate an HR estimate
        // - Cardiio takes 30secs to generate an HR estimate
        hrResult hr_output = heartRate_calc(vid, _window_size_in_sec, _overlap_ratio,
                                            _max_bpm, _cutoff_freq, _channels_to_process,
                                            _colourspace, _time_lag);
        // debug info
        String vidType = "mp4";
        printf("run_hr(vidType = %s, colourspace = %s, min_hr = %lf, max_hr = %lf, \
               alpha = %lf, level = %lf, chromAtn = %lf)\n",
               vidType.c_str(), _colourspace.c_str(), _eulerian_minHR, _eulerian_maxHR,
               _eulerian_alpha, _eulerian_pyrLevel, _eulerian_chromaMagnifier);
        printf("Heart Rate result {autocorr, pda} = {%lf, %lf}\n", hr_output.autocorr, hr_output.pda);
        
        t2 = clock();
        printf("run_hr() time = %f\n", ((float)t2 - (float)t1)/1000.0);
        return hr_output;
    }
}
