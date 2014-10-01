//
//  config.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 23/7/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "config.h"

namespace MHR {
    int _DEBUG_MODE = 0;
    int _THREE_CHAN_MODE = 0;
    
    /*
     Params
     */
    
    bool _FACE_MODE;     // switch between Face mode and Finger mode
    
    const double NaN = -1e9;
    
    const int _framesBlock_size = 128;  // number of frames to be processed in each block
    const int _minVidLength = 15;       // seconds
    const int _maxVidLength = 30;       // seconds
    String _outputPath = "NULL";
    
    /*--------------for run_eulerian()--------------*/
    
    // Native params of the algorithm
    int _frameRate = 30;
    
    /*--------------for run_hr()--------------*/
    double _cutoff_freq = -1;         // Hz
    int _channels_to_process = -1;     // If only 1 channel: 1 for tsl, 0 for rgb
    int _number_of_bins_heartRate = -1;
    
    // heartRate_calc: Native params of the algorithm
    int _flagDebug = -1;
    int _flagGetRaw = -1;
    
    
    double _peakStrengthThreshold_fraction = -1;
    
    
    /*--------------change params functions--------------*/

    void setFaceParams()
    {
        if (_DEBUG_MODE) printf("setFaceParams()\n");
        _FACE_MODE = true;

        _cutoff_freq = _face_cutoff_freq;
        _channels_to_process = _face_channels_to_process;
        _number_of_bins_heartRate = _face_number_of_bins_heartRate;
        
        _flagDebug = _face_flagDebug;
        _flagGetRaw = _face_flagGetRaw;
        
        _peakStrengthThreshold_fraction = _face_peakStrengthThreshold_fraction;
    
    }


    void setFingerParams()
    {
        if (_DEBUG_MODE) printf("setFingerParams()\n");
        _FACE_MODE = false;

        _cutoff_freq = _finger_cutoff_freq;
        _channels_to_process = _finger_channels_to_process;
        _number_of_bins_heartRate = _finger_number_of_bins_heartRate;
        
        _flagDebug = _finger_flagDebug;
        _flagGetRaw = _finger_flagGetRaw;
        
        _peakStrengthThreshold_fraction = _finger_peakStrengthThreshold_fraction;
    }
};