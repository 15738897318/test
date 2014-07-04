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
    hrResult run_hr(vector<Mat> &vid, String resultsDir, const String &vidType,
                    double min_hr, double max_hr,
                    double alpha, int level, double chromAtn)
    {
        // - Basis takes 15secs to generate an HR estimate
        // - Cardiio takes 30secs to generate an HR estimate
    
        hrResult hr_output = heartRate_calc(vid, _run_hr_window_size_in_sec, _run_hr_overlap_ratio,
                                            _run_hr_max_bpm, _run_hr_cutoff_freq, _run_hr_channels_to_process,
                                            _run_hr_colourspace, _run_hr_time_lag);
        // debug info
        printf("run_hr(vidType = %s, colourspace = %s, min_hr = %lf, max_hr = %lf, alpha = %lf, level = %d, chromAtn = %lf)",
               vidType.c_str(), _run_hr_colourspace.c_str(), min_hr, max_hr, alpha, level, chromAtn);
        printf("hr_output = {%lf, %lf}", hr_output.autocorr, hr_output.pda);
        
        return hr_output;
    }
}