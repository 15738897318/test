//
//  processingCumulative.cpp
//  Pulsar
//
//  Created by HaiPhan on 8/19/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#include "processingCumulative.h"

namespace MHR
{
    void processingCumulative(vector<double> &temporal_mean, vector<double> temp, hrResult &currentResult)
    {
        int eulerianLen = (int)temp.size();
        
        for (int i = 0; i < eulerianLen; ++i)
            temporal_mean.push_back(temp[i]);
        
        double threshold_fraction = 0;
        int window_size = round(_window_size_in_sec * _frameRate);
        int firstSample = round(_frameRate * _time_lag);
        
//        if (currentFrame == nFrames - 1) break;
        
        /*-----------------Perform HR calculation for the frames processed so far-----------------*/
        
        // Low-pass-filter the signal stream to remove unwanted noises
        vector<double> temporal_mean_filt;
        temporal_mean_filt = low_pass_filter(temporal_mean);
        
        // Block 2: Heart-rate calculation
        // - Basis takes 15secs to generate an HR estimate
        // - Cardiio takes 30secs to generate an HR estimate
        currentResult = hr_signal_calc(temporal_mean_filt, firstSample, window_size, _frameRate,
                                      _overlap_ratio, _max_bpm, threshold_fraction);
        
        printf("%lf %lf\n",currentResult.autocorr,currentResult.pda);
        
        hrGlobalResult = currentResult;
    }
}
