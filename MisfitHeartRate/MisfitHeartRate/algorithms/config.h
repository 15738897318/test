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
//#include "params.cpp"

using namespace std;
using namespace cv;

namespace MHR {
#ifdef DEBUG
    const int DEBUG_MODE = 1;
#else
    const int DEBUG_MODE = 0;
#endif
    
#define ELEMENT_COUNT(X) (sizeof(X) / sizeof((X)[0]))
    
    const String _outputPath = "/var/mobile/Applications/40BBE745-97D5-4BEA-B486-AB77BCE9B3B2/Documents/";
    
    const double NaN = -1e9;
    
    const int _framesBlock_size = 64;
    const int _minVidLength = 15;       // seconds
    const int _maxVidLength = 30;       // seconds
    
    extern bool _FACE_MODE;
    
    /*--------------for run_eulerian()--------------*/
    extern double _eulerian_alpha;          // Eulerian magnifier, standard < 50
    extern double _eulerian_pyrLevel;        // Standard: 4, but updated by the real frame size
    extern double _eulerian_minHR;          // BPM Standard: 50
    extern double _eulerian_maxHR;         // BPM Standard: 90
    extern double _eulerian_frameRate;      // Standard: 30, but updated by the real frame-rate
    extern double _eulerian_chromaMagnifier; // Standard: 1
    
    // Native params of the algorithm
    extern int _frameRate;
    extern int _number_of_channels;
    extern int _Gpyr_filter_length;
    extern int _startFrame;
    extern int _endFrame; // >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    // filter_bandpassing:
    extern bool _isUseFilterBandPassing;     // use ideal bandpassing
    extern int _eulerianTemporalFilterKernel_size;
    extern Mat _eulerianTemporalFilterKernel;
    
    
    /*--------------for run_hr()--------------*/
    extern double _window_size_in_sec;
    extern double _overlap_ratio;
    extern double _max_bpm;             // BPM
    extern double _cutoff_freq;         // Hz
    extern double _time_lag;              // seconds
    extern String _colourspace;
    extern int _channels_to_process;     // If only 1 channel: 1 for tsl, 0 for rgb
    extern int _number_of_bins_heartRate;
    
    // heartRate_calc: Native params of the algorithm
    extern int _flagDebug;
    extern int _flagGetRaw;
    
    extern int _startIndex;  //400
    extern int _endIndex;    //1400  >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    extern double _peakStrengthThreshold_fraction;
    extern String _frames2signalConversionMethod;
    
    extern int _frame_downsampling_filt_rows;
    extern int _frame_downsampling_filt_cols;
    extern Mat _frame_downsampling_filt;
    
    
    /*--------------for frames2signal()--------------*/
    //trimmed-mean
    extern int _trimmed_size;
    
    //mode-balance
    extern double _training_time_start;    // seconds
    extern double _training_time_end;        // seconds
    extern int _number_of_bins;             // 50 * round(fr * training_time);
    extern double _pct_reach_below_mode;    // Percent
    extern double _pct_reach_above_mode;    // Percent
    
    
    /*--------------for matlab functions--------------*/
    
    //kernel for low_pass_filter(), used in frames2sinal()
    extern int _beatSignalFilterKernel_size;
    extern Mat _beatSignalFilterKernel;
    
    
    /*--------------change params functions--------------*/
    
    void setFaceParams();
    void setFingerParams();
}

#endif
