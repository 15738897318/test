//
//  params.cpp
//  Pulsar
//
//  Created by Bao Nguyen on 7/23/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#ifndef MisfitHeartRate_params_cpp
#define MisfitHeartRate_params_cpp

#include "face_params.h"
#include "finger_params.h"
#include "config.h"

using namespace std;
using namespace cv;

namespace MHR {
    bool _FACE_MODE = true;

    /*--------------for run_eulerian()--------------*/
    double _eulerian_alpha = -1;          // Eulerian magnifier, standard < 50
    double _eulerian_pyrLevel = -1;        // Standard: 4, but updated by the real frame size
    double _eulerian_minHR = -1;          // BPM Standard: 50
    double _eulerian_maxHR = -1;         // BPM Standard: 90
    double _eulerian_frameRate = -1;      // Standard: 30, but updated by the real frame-rate
    double _eulerian_chromaMagnifier = -1; // Standard: 1

    // Native params of the algorithm
    int _frameRate = -1;
    int _number_of_channels = -1;
    int _Gpyr_filter_length = -1;
    int _startFrame = -1;
    int _endFrame = -1; // >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length

    // filter_bandpassing:
    bool _isUseFilterBandPassing = -1;     // use ideal bandpassing
    int _eulerianTemporalFilterKernel_size = -1;
    Mat _eulerianTemporalFilterKernel;


    /*--------------for run_hr()--------------*/
    double _window_size_in_sec = -1;
    double _overlap_ratio = -1;
    double _max_bpm = -1;             // BPM
    double _cutoff_freq = -1;         // Hz
    double _time_lag = -1;              // seconds
    String _colourspace = "-1";
    int _channels_to_process = -1;     // If only 1 channel: 1 for tsl, 0 for rgb
    int _number_of_bins_heartRate = -1;

    // heartRate_calc: Native params of the algorithm
    int _flagDebug = -1;
    int _flagGetRaw = -1;

    int _startIndex = -1;  //400
    int _endIndex = -1;    //1400  >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length

    double _peakStrengthThreshold_fraction = -1;
    String _frames2signalConversionMethod = "-1";

    int _frame_downsampling_filt_rows = -1;
    int _frame_downsampling_filt_cols = -1;
    Mat _frame_downsampling_filt;


    /*--------------for frames2signal()--------------*/
    //trimmed-mean
    int _trimmed_size = -1;

    //mode-balance
    double _training_time_start = -1;    // seconds
    double _training_time_end = -1;        // seconds
    int _number_of_bins = -1;             // 50 * round(fr * training_time);
    double _pct_reach_below_mode = -1;    // Percent
    double _pct_reach_above_mode = -1;    // Percent


    /*--------------for matlab functions--------------*/

    //kernel for low_pass_filter(), used in frames2sinal()
    int _beatSignalFilterKernel_size = -1;
    Mat _beatSignalFilterKernel;
}

#endif