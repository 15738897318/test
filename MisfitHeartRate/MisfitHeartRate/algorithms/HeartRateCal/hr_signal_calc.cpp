//
//  hr_signal_calc.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hr_signal_calc.h"


namespace MHR {
    hrResult::hrResult(mTYPE autocorr, mTYPE pda)
    {
        this->autocorr = autocorr;
        this->pda = pda;
    }
    
    
    void hrResult::operator = (const hrResult &other)
    {
        this->autocorr = other.autocorr;
        this->pda = other.pda;
    }
    
    
    hrResult hr_signal_calc(vector<mTYPE> &temporal_mean, int firstSample, int window_size,
                            mTYPE frameRate, mTYPE overlap_ratio,
                            mTYPE max_bpm, mTYPE threshold_fraction)
    {
        clock_t t1 = clock();
        // Set peak-detection params
        if (firstSample > temporal_mean.size())
            firstSample = 0;
        mTYPE threshold = threshold_fraction * (*max_element(temporal_mean.begin() + firstSample, temporal_mean.end()));
        int minPeakDistance = round(60 / max_bpm * frameRate);
        
        // Calculate heart-rate using peak-detection on the signal
        hrDebug debug_pda;
        vector<int> hb_locations_pda = hb_counter_pda(temporal_mean, frameRate, firstSample,
                                                      window_size, overlap_ratio,
                                                      minPeakDistance, threshold,
                                                      debug_pda);
        vector<mTYPE> ans_pda;
        hr_calculator(hb_locations_pda, frameRate, ans_pda);
        mTYPE avg_hr_pda = ans_pda[0];     // average heart rate
        
        // Calculate heart-rate using peak-detection on the signal
        hrDebug debug_autocorr;
        vector<int> hb_locations_autocorr = hb_counter_autocorr(temporal_mean, frameRate, firstSample,
                                                                window_size, overlap_ratio,
                                                                minPeakDistance,
                                                                debug_autocorr);
        vector<mTYPE> ans_autocorr;
        hr_calculator(hb_locations_autocorr, frameRate, ans_autocorr);
        mTYPE avg_hr_autocorr = ans_autocorr[0];     // average heart rate
        

        clock_t t2 = clock();
        printf("hr_signal_calc() time = %f\n", ((float)t2 - (float)t1)/1000.0);
        return hrResult(avg_hr_autocorr, avg_hr_pda);
    }
}