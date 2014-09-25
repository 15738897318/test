//
//  CV2ImageProcessor.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/25/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef __MIsfitHRDetection__CV2ImageProcessor__
#define __MIsfitHRDetection__CV2ImageProcessor__

#include <iostream>
#include "AbstractImageProcessor.h"

class CV2ImageProcessor: public AbstractImageProcessor {
    vector<Mat> vid;
    vector<Mat> eulerianVid;
    int nFrames;
    int currentFrame;
    bool isCalcMode;
    int window_size;
    int firstSample;
    double threshold_fraction;
    double lower_range;
    double upper_range;
        //-------------------------------------------------------------------------------//
    /*
     Params
     */
    
    const double NaN = -1e9;
    
    const int _framesBlock_size = 128;  // number of frames to be processed in each block
    const int _minVidLength = 15;       // seconds
    const int _maxVidLength = 30;       // seconds
    
    bool _FACE_MODE;     // switch between Face mode and Finger mode
    
    /*--------------for run_eulerian()--------------*/
    double _eulerian_alpha;          // Eulerian magnifier, standard < 50
    double _eulerian_pyrLevel;        // Standard: 4, but updated by the real frame size
    double _eulerian_minHR;          // BPM Standard: 50
    double _eulerian_maxHR;         // BPM Standard: 90
    double _eulerian_frameRate;      // Standard: 30, but updated by the real frame-rate
    double _eulerian_chromaMagnifier; // Standard: 1
    
        // Native params of the algorithm
    int _frameRate;
    int _number_of_channels;
    int _Gpyr_filter_length;
    int _startFrame;
    int _endFrame; // >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    /*--------------for run_hr()--------------*/
    double _window_size_in_sec;
    double _overlap_ratio;
    double _max_bpm;             // BPM
    double _cutoff_freq;         // Hz
    double _time_lag;              // seconds
    char *_colourspace;
    int _channels_to_process;     // If only 1 channel: 1 for tsl, 0 for rgb
    int _number_of_bins_heartRate;
    
        // heartRate_calc: Native params of the algorithm
    int _flagDebug;
    int _flagGetRaw;
    
    int _startIndex;  //400
    int _endIndex;    //1400  >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    double _peakStrengthThreshold_fraction;
    char *_frames2signalConversionMethod;
    
    int _frame_downsampling_filt_rows;
    int _frame_downsampling_filt_cols;
    Mat _frame_downsampling_filt;
    
    
    /*--------------for frames2signal()--------------*/
        //trimmed-mean
    int _trimmed_size;
    
        //mode-balance
    double _training_time_start;    // seconds
    double _training_time_end;        // seconds
    int _number_of_bins;             // 50 * round(fr * training_time);
    double _pct_reach_below_mode;    // Percent
    double _pct_reach_above_mode;    // Percent
    
private:
    void eulerianGaussianPyramidMagnification();

public:
    CV2ImageProcessor();
    ~CV2ImageProcessor();
    void setFaceParams();
    void setFingerParams();
    int readFrameInfo();
    void readFrames();
//    void writeFrames(uint8_t numFrame, uint16_t offset);
//    void setArrayInfo(TemporalArray &arr);
//    void writeArray(TemporalArray &arr, uint8_t numSig, uint16_t offset);
        /*---------Test functions, to be removed------------*/
    
    vector<Mat> &getVid() {
        return vid;
    }
    
    vector<Mat> &getEulerienVid() {
        return eulerianVid;
    };
    int getNFrame() {
        return nFrames;
    }
    int getCurrentFrame() {
        return currentFrame;
    }
};

#endif /* defined(__MIsfitHRDetection__CV2ImageProcessor__) */
