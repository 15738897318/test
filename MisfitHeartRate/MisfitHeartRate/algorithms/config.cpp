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
    int _LOAD_FROM_FILE = 0;
    
    bool _FACE_MODE = true;
    
    String _outputPath = "NULL";
    
    /*--------------for run_eulerian()--------------*/
    double _eulerian_alpha = -1;          // Eulerian magnifier, standard < 50
    double _eulerian_pyrLevel = -1;        // Standard: 4, but updated by the real frame size
    double _eulerian_minHR = -1;          // BPM Standard: 50
    double _eulerian_maxHR = -1;         // BPM Standard: 90
    double _eulerian_frameRate = -1;      // Standard: 30, but updated by the real frame-rate
    double _eulerian_chromaMagnifier = -1; // Standard: 1
    
    // Native params of the algorithm
    double _frameRate = 30;
    int _number_of_channels = -1;
    int _Gpyr_filter_length = -1;
    int _startFrame = -1;
    int _endFrame = -1; // >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    // ideal bandpassing:
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
    
    double _hrThreshold = -1; // BPM
    double _hrStanDev = -1; // BPM
    
    
    /*--------------change params functions--------------*/

    void setFaceParams()
    {
        if (_DEBUG_MODE) printf("setFaceParams()\n");
        _FACE_MODE = true;
        
        _eulerian_alpha = _face_eulerian_alpha;
        _eulerian_pyrLevel = _face_eulerian_pyrLevel;
        _eulerian_minHR = _face_eulerian_minHR;
        _eulerian_maxHR = _face_eulerian_maxHR;
        _eulerian_frameRate = _face_eulerian_frameRate;
        _eulerian_chromaMagnifier = _face_eulerian_chromaMagnifier;
        
        _number_of_channels = _face_number_of_channels;
        _Gpyr_filter_length = _face_Gpyr_filter_length;
        _startFrame = _face_startFrame;
        _endFrame = _face_endFrame;
        
        _isUseFilterBandPassing = _face_isUseFilterBandPassing;
        _eulerianTemporalFilterKernel_size = _face_eulerianTemporalFilterKernel_size;
        _eulerianTemporalFilterKernel = _face_eulerianTemporalFilterKernel.clone();
        
        _window_size_in_sec = _face_window_size_in_sec;
        _overlap_ratio = _face_overlap_ratio;
        _max_bpm = _face_max_bpm;
        _cutoff_freq = _face_cutoff_freq;
        _time_lag = _face_time_lag;
        _colourspace = _face_colourspace;
        _channels_to_process = _face_channels_to_process;
        _number_of_bins_heartRate = _face_number_of_bins_heartRate;
        
        _flagDebug = _face_flagDebug;
        _flagGetRaw = _face_flagGetRaw;
        
        _startIndex = _face_startIndex;
        _endIndex = _face_endIndex;
        
        _peakStrengthThreshold_fraction = _face_peakStrengthThreshold_fraction;
        _frames2signalConversionMethod = _face_frames2signalConversionMethod;
        
        _frame_downsampling_filt_rows = _face_frame_downsampling_filt_rows;
        _frame_downsampling_filt_cols = _face_frame_downsampling_filt_cols;
        _frame_downsampling_filt = _face_frame_downsampling_filt.clone();
        
        _trimmed_size = _face_trimmed_size;
        
        _training_time_start = _face_training_time_start;
        _training_time_end = _face_training_time_end;
        _number_of_bins = _face_number_of_bins;
        _pct_reach_below_mode = _face_pct_reach_below_mode;
        _pct_reach_above_mode = _face_pct_reach_above_mode;
        
        _beatSignalFilterKernel_size = _face_beatSignalFilterKernel_size;
        _beatSignalFilterKernel = _face_beatSignalFilterKernel.clone();
        
        _hrThreshold = _face_hrThreshold;
        _hrStanDev = _face_hrStanDev;
    }


    void setFingerParams()
    {
        if (_DEBUG_MODE) printf("setFingerParams()\n");
        _FACE_MODE = false;
        
        _eulerian_alpha = _finger_eulerian_alpha;
        _eulerian_pyrLevel = _finger_eulerian_pyrLevel;
        _eulerian_minHR = _finger_eulerian_minHR;
        _eulerian_maxHR = _finger_eulerian_maxHR;
        _eulerian_frameRate = _finger_eulerian_frameRate;
        _eulerian_chromaMagnifier = _finger_eulerian_chromaMagnifier;
        
        _number_of_channels = _finger_number_of_channels;
        _Gpyr_filter_length = _finger_Gpyr_filter_length;
        _startFrame = _finger_startFrame;
        _endFrame = _finger_endFrame;
        
        _isUseFilterBandPassing = _finger_isUseFilterBandPassing;
        _eulerianTemporalFilterKernel_size = _finger_eulerianTemporalFilterKernel_size;
        _eulerianTemporalFilterKernel = _finger_eulerianTemporalFilterKernel.clone();
        
        _window_size_in_sec = _finger_window_size_in_sec;
        _overlap_ratio = _finger_overlap_ratio;
        _max_bpm = _finger_max_bpm;
        _cutoff_freq = _finger_cutoff_freq;
        _time_lag = _finger_time_lag;
        _colourspace = _finger_colourspace;
        _channels_to_process = _finger_channels_to_process;
        _number_of_bins_heartRate = _finger_number_of_bins_heartRate;
        
        _flagDebug = _finger_flagDebug;
        _flagGetRaw = _finger_flagGetRaw;
        
        _startIndex = _finger_startIndex;
        _endIndex = _finger_endIndex;
        
        _peakStrengthThreshold_fraction = _finger_peakStrengthThreshold_fraction;
        _frames2signalConversionMethod = _finger_frames2signalConversionMethod;
        
        _frame_downsampling_filt_rows = _finger_frame_downsampling_filt_rows;
        _frame_downsampling_filt_cols = _finger_frame_downsampling_filt_cols;
        _frame_downsampling_filt = _finger_frame_downsampling_filt.clone();
        
        _trimmed_size = _finger_trimmed_size;
        
        _training_time_start = _finger_training_time_start;
        _training_time_end = _finger_training_time_end;
        _number_of_bins = _finger_number_of_bins;
        _pct_reach_below_mode = _finger_pct_reach_below_mode;
        _pct_reach_above_mode = _finger_pct_reach_above_mode;
        
        _beatSignalFilterKernel_size = _finger_beatSignalFilterKernel_size;
        _beatSignalFilterKernel = _finger_beatSignalFilterKernel.clone();
        
        _hrThreshold = _finger_hrThreshold;
        _hrStanDev = _finger_hrStanDev;
    }
};