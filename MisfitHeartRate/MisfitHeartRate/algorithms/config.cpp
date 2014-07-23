//
//  config.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 23/7/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "config.h"


namespace MHR {
    void setFaceParams()
    {
        if (DEBUG_MODE) printf("setFaceParams()\n");
        _FACE_MODE = true;
        
        _eulerian_alpha = _face_eulerian_alpha;
        _eulerian_pyrLevel = _face_eulerian_pyrLevel;
        _eulerian_minHR = _face_eulerian_minHR;
        _eulerian_maxHR = _face_eulerian_maxHR;
        _eulerian_frameRate = _face_eulerian_frameRate;
        _eulerian_chromaMagnifier = _face_eulerian_chromaMagnifier;
        
        _frameRate = _face_frameRate;
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
    }


    void setFingerParams()
    {
        if (DEBUG_MODE) printf("setFingerParams()\n");
        _FACE_MODE = false;
        
        _eulerian_alpha = _finger_eulerian_alpha;
        _eulerian_pyrLevel = _finger_eulerian_pyrLevel;
        _eulerian_minHR = _finger_eulerian_minHR;
        _eulerian_maxHR = _finger_eulerian_maxHR;
        _eulerian_frameRate = _finger_eulerian_frameRate;
        _eulerian_chromaMagnifier = _finger_eulerian_chromaMagnifier;
        
        _frameRate = _finger_frameRate;
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
    }
};