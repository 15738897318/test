//
//  hr_signal_calc.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hr_signal_calc.h"


namespace MHR {
    hrResult::hrResult(double autocorr, double pda)
    {
        this->autocorr = autocorr;
        this->pda = pda;
    }
    
    
    void hrResult::operator = (const hrResult &other)
    {
        this->autocorr = other.autocorr;
        this->pda = other.pda;
    }
    
    
    hrResult hr_signal_calc(vector<double> &temporal_mean, int firstSample, int window_size,
                            double frameRate, double overlap_ratio,
                            double max_bpm, double threshold_fraction)
    {
        // Set peak-detection params
        if (firstSample > temporal_mean.size())
            firstSample = 0;
        double threshold = threshold_fraction * (*max_element(temporal_mean.begin() + firstSample, temporal_mean.end()));
        int minPeakDistance = round(60 / max_bpm * frameRate);
        
        // Calculate heart-rate using peak-detection on the signal
        hrDebug debug_pda;
        double avg_hr_pda = hr_calc_pda(temporal_mean, frameRate, firstSample,
                                        window_size, overlap_ratio,
                                        minPeakDistance, threshold,
                                        debug_pda);
        
        // Calculate heart-rate using peak-detection on the signal
        hrDebug debug_autocorr;
        double avg_hr_autocorr = hr_calc_autocorr(temporal_mean, frameRate, firstSample,
                                                  window_size, overlap_ratio,
                                                  minPeakDistance,
                                                  debug_autocorr);
        
        return hrResult(avg_hr_autocorr, avg_hr_pda);
    }
}