//
//  config.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef MisfitHeartRate_config_h
#define MisfitHeartRate_config_h

using namespace std;
using namespace cv;

namespace MHR {
#ifdef DEBUG
    const int DEBUG_MODE = 0;
#else
    const int DEBUG_MODE = 0;
#endif
    
    #define ELEMENT_COUNT(X) (sizeof(X) / sizeof((X)[0]))
    
    const String _outputPath = "/var/mobile/Applications/40BBE745-97D5-4BEA-B486-AB77BCE9B3B2/Documents/";
    
    const double NaN = -1e9;
    
    const int _framesBlock_size = 64;
    const int _minVidLength = 15;        // seconds

    /*--------------for run_eulerian()--------------*/
    const double _eulerian_alpha = 50;          // Eulerian magnifier, standard < 50
    const double _eulerian_pyrLevel = 6;        // Standard: 4, but updated by the real frame size
    const double _eulerian_minHR = 30;          // BPM Standard: 50
    const double _eulerian_maxHR = 240;         // BPM Standard: 90
    const double _eulerian_frameRate = 30;      // Standard: 30, but updated by the real frame-rate
    const double _eulerian_chromaMagnifier = 1; // Standard: 1

    // Native params of the algorithm
    const int _frameRate = 30;
    const int _number_of_channels = 3;
    const int _Gpyr_filter_length = 5;
    const int _startFrame = 0;
    const int _endFrame = 0; // >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length

    // filter_bandpassing:
    const bool _isUseFilterBandPassing = false;     // use ideal bandpassing
    const int _eulerianTemporalFilterKernel_size = 15;
    const double _eulerianTemporalFilterKernel[] = {0.0034, 0.0087, 0.0244, 0.0529, 0.0909, 0.1300, 0.1594, 0.1704, 0.1594,0.1300, 0.0909, 0.0529, 0.0244, 0.0087, 0.0034};


    /*--------------for run_hr()--------------*/
    const double _window_size_in_sec = 10;
    const double _overlap_ratio = 0;
    const double _max_bpm = 200;             // BPM
    const double _cutoff_freq = 2.5;         // Hz
    const double _time_lag = 3;              // seconds
    const String _colourspace = "tsl";
    const int _channels_to_process = 1;     // If only 1 channel: 1 for tsl, 0 for rgb
    const int _number_of_bins_heartRate = 5;

    // heartRate_calc: Native params of the algorithm
    const int _flagDebug = 0;
    const int _flagGetRaw = 0;

    const int _startIndex = 1;  //400
    const int _endIndex = 0;    //1400	>= 0 to get definite end-frame, < 0 to get end-frame relative to stream length

    const double _peakStrengthThreshold_fraction = 0;
    const String _frames2signalConversionMethod = "mode-balance";

    const int _frame_downsampling_filt_rows = 7;
    const int _frame_downsampling_filt_cols = 7;
    const double _frame_downsampling_filt[] = {
        0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085,
        0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127,
        0.0162, 0.0241, 0.0307, 0.0332, 0.0307, 0.0241, 0.0162,
        0.0175, 0.0261, 0.0332, 0.0360, 0.0332, 0.0261, 0.0175,
        0.0162, 0.0241, 0.0307, 0.0332, 0.0307, 0.0241, 0.0162,
        0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127,
        0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085
    };


    /*--------------for frames2signal()--------------*/
    //trimmed-mean const
    const int _trimmed_size = 30;

    //mode-balance const
    const double _training_time_start = 0.5;    // seconds
    const double _training_time_end = 3;        // seconds
    const int _number_of_bins = 50;             // 50 * round(fr * training_time);
    const double _pct_reach_below_mode = 45;    // Percent
    const double _pct_reach_above_mode = 45;    // Percent


    /*--------------for matlab functions--------------*/

    //kernel for low_pass_filter(), used in frames2sinal()
    const int _beatSignalFilterKernel_size = 15;
    const double _beatSignalFilterKernel[] = {
        -0.0265, -0.0076, 0.0217, 0.0580, 0.0956,
        0.1285, 0.1509, 0.1589, 0.1509, 0.1285,
        0.0956, 0.0580, 0.0217, -0.0076, -0.0265
    };
}

#endif
