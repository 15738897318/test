//
//  finger_params.h
//  Pulsar
//
//  Created by Bao Nguyen on 7/23/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#ifndef __Pulsar__finger_params__
#define __Pulsar__finger_params__

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace std;
using namespace cv;

namespace MHR {    
    /*--------------for run_eulerian()--------------*/
    const double _finger_eulerian_alpha = 50;          // Eulerian magnifier, standard < 50
    const double _finger_eulerian_pyrLevel = 6;        // Standard: 4, but updated by the real frame size
    const double _finger_eulerian_minHR = 30;          // BPM Standard: 50
    const double _finger_eulerian_maxHR = 240;         // BPM Standard: 90
    const double _finger_eulerian_frameRate = 30;      // Standard: 30, but updated by the real frame-rate
    const double _finger_eulerian_chromaMagnifier = 1; // Standard: 1
    
    // Native params of the algorithm
    const int _finger_number_of_channels = 3;
    const int _finger_Gpyr_filter_length = 5;
    const int _finger_startFrame = 0;
    const int _finger_endFrame = 0; // >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    /*--------------for run_hr()--------------*/
    const double _finger_window_size_in_sec = 10;
    const double _finger_overlap_ratio = 0;
    const double _finger_max_bpm = 200;             // BPM
    const double _finger_time_lag = 3;              // seconds
    const String _finger_colourspace = "rgb";
    const int _finger_channels_to_process = 0;     // If only 1 channel: 1 for tsl, 0 for rgb
    const int _finger_number_of_bins_heartRate = 5;
    
    // heartRate_calc: Native params of the algorithm
    const int _finger_flagDebug = 0;
    const int _finger_flagGetRaw = 0;
    
    const int _finger_startIndex = 1;  //400
    const int _finger_endIndex = 0;    //1400  >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    const double _finger_peakStrengthThreshold_fraction = 0;
    const String _finger_frames2signalConversionMethod = "mode-balance";
    
    const int _finger_frame_downsampling_filt_rows = 7;
    const int _finger_frame_downsampling_filt_cols = 7;
    const Mat _finger_frame_downsampling_filt =
        (Mat_<double>(_finger_frame_downsampling_filt_rows, _finger_frame_downsampling_filt_cols) <<
             0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085,
             0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127,
             0.0162, 0.0241, 0.0307, 0.0332, 0.0307, 0.0241, 0.0162,
             0.0175, 0.0261, 0.0332, 0.0360, 0.0332, 0.0261, 0.0175,
             0.0162, 0.0241, 0.0241307, 0.0332, 0.0307, 0.0241, 0.0162,
             0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127,
             0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085);
    
    
    /*--------------for frames2signal()--------------*/
    //trimmed-mean
    const int _finger_trimmed_size = 30;
    
    //mode-balance
    const double _finger_training_time_start = 0;    // seconds
    const double _finger_training_time_end = 0.2;        // seconds
    const int _finger_number_of_bins = 50;             // 50 * round(fr * training_time);
    const double _finger_pct_reach_below_mode = 45;    // Percent
    const double _finger_pct_reach_above_mode = 45;    // Percent
    
    
    /*--------------for matlab functions--------------*/
    
    //kernel for low_pass_filter(), used in frames2sinal()
    const int _finger_beatSignalFilterKernel_size = 15;
    const Mat _finger_beatSignalFilterKernel = (Mat_<double>(1, _finger_beatSignalFilterKernel_size) <<
                                              -0.0265, -0.0076, 0.0217, 0.0580, 0.0956,
                                              0.1285, 0.1509, 0.1589, 0.1509, 0.1285,
                                              0.0956, 0.0580, 0.0217, -0.0076, -0.0265);
}

#endif /* defined(__Pulsar__finger_params__) */
