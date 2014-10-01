//
//  config.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef MisfitHeartRate_config_h
#define MisfitHeartRate_config_h

#include "face_params.h"
#include "finger_params.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;


namespace MHR {
    extern int _DEBUG_MODE;
    extern int _THREE_CHAN_MODE;
    
    const extern double NaN;
    
    const extern int _framesBlock_size;  // number of frames to be processed in each block
    const extern int _minVidLength;       // seconds
    const extern int _maxVidLength;       // seconds
    extern String _outputPath;
    
    extern bool _FACE_MODE;     // switch between Face mode and Finger mode
    
    
    /*--------------for run_hr()--------------*/
    extern int _frameRate;      // BPM
    extern int _channels_to_process;     // If only 1 channel: 1 for tsl, 0 for rgb
    extern int _number_of_bins_heartRate;
    
    // heartRate_calc: Native params of the algorithm
    extern int _flagDebug;
    extern int _flagGetRaw;
    
    /*--------------change params functions--------------*/
    void setFaceParams();
    void setFingerParams();
}

#endif
