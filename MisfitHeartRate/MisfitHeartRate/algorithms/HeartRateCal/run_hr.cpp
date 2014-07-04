//
//  run_hr.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "run_hr.h"


namespace MHR {
    // run Heart Rate calculation
    hrResult run_hr(vector<Mat> &vid, String resultsDir,
                double min_hr, double max_hr,
                double alpha, int level, double chromAtn)
    {
        // Notes:
        // - Basis takes 15secs to generate an HR estimate
        // - Cardiio takes 30secs to generate an HR estimate
        
        double window_size_in_sec = 10;
        double overlap_ratio = 0;
        double max_bpm = 200;   // BPM
        double cutoff_freq = 5;    // Hz
        double time_lag = 3;       // seconds
        
        String colourspace = "tsl";
        int colour_channel = _channels_to_process;
        return heartRate_calc(vid, window_size_in_sec, overlap_ratio,
                              max_bpm, cutoff_freq, colour_channel, colourspace, time_lag);
    }
}