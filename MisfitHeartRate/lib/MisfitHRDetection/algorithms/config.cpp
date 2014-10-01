//
//  config.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 23/7/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "config.h"
#include "face_params.h"
#include "finger_params.h"

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
    int _number_of_bins_heartRate = -1;
    
    // heartRate_calc: Native params of the algorithm
    int _flagDebug = -1;
    int _flagGetRaw = -1;

    
    
    /*--------------change params functions--------------*/

    void setFaceParams()
    {
        if (_DEBUG_MODE) printf("setFaceParams()\n");
        _FACE_MODE = true;

        _number_of_bins_heartRate = _face_number_of_bins_heartRate;
        
        _flagDebug = _face_flagDebug;
        _flagGetRaw = _face_flagGetRaw;
    
    }


    void setFingerParams()
    {
        if (_DEBUG_MODE) printf("setFingerParams()\n");
        _FACE_MODE = false;

        _number_of_bins_heartRate = _finger_number_of_bins_heartRate;
        
        _flagDebug = _finger_flagDebug;
        _flagGetRaw = _finger_flagGetRaw;
        
    }
};