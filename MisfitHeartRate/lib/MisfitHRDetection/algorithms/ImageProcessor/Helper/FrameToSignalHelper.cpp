//
//  SimpleMeanFrameToSignalHelper.cpp
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 10/1/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#include "FrameToSignalHelper.h"
#include "matlab.h"

void SimpleMeanFrameToSignalHelper::convert(std::vector<Mat> &eulerianVid, std::vector<double> &sig, double frameRate, bool isCalcMode) {
    int height = eulerianVid[0].rows;
    int width = eulerianVid[0].cols;
    int total_frames = (int)eulerianVid.size();
    double size = height * width;
    sig.reserve(total_frames);
    for(int i=0; i<total_frames; ++i){
        double sum = 0;
        for(int x=0; x<height; ++x)
            for(int y=0; y<width; ++y)
                sum+=eulerianVid[i].at<double>(x,y);
        sig.push_back(sum/size);
    }
}

void TrimmedMeanFrameToSignalHelper::setFaceParams() {
    AbstractFrameToSignalHelper::setFaceParams();
    _trimmed_size = MHR::_face_trimmed_size;
}

void TrimmedMeanFrameToSignalHelper::setFingerParams() {
    AbstractFrameToSignalHelper::setFingerParams();
    _trimmed_size = MHR::_finger_trimmed_size;
}

void TrimmedMeanFrameToSignalHelper::convert(std::vector<Mat> &eulerianVid, std::vector<double> &sig, double frameRate, bool isCalcMode) {
    int height = eulerianVid[0].rows;
    int width = eulerianVid[0].cols;
    int total_frames = (int)eulerianVid.size();
    sig.reserve(total_frames);
    double size = (height - _trimmed_size * 2) * (width - _trimmed_size * 2);
    for(int i=0; i<total_frames; ++i){
        double sum = 0;
        for(int x=_trimmed_size; x<height-_trimmed_size; ++x)
            for(int y=_trimmed_size; y<width-_trimmed_size; ++y)
                sum+=eulerianVid[i].at<double>(x,y);
        sig.push_back(sum/size);
    }
}


void BalancedModeFrameToSignalHelper::setFaceParams() {
    AbstractFrameToSignalHelper::setFaceParams();
    _training_time_start = MHR::_face_training_time_start;
    _training_time_end = MHR::_face_training_time_end;
    _number_of_bins = MHR::_face_number_of_bins;
    _pct_reach_below_mode = MHR::_face_pct_reach_below_mode;
    _pct_reach_above_mode = MHR::_face_pct_reach_above_mode;}

void BalancedModeFrameToSignalHelper::setFingerParams() {
    AbstractFrameToSignalHelper::setFingerParams();
    _training_time_start = MHR::_finger_training_time_start;
    _training_time_end = MHR::_finger_training_time_end;
    _number_of_bins = MHR::_finger_number_of_bins;
    _pct_reach_below_mode = MHR::_finger_pct_reach_below_mode;
    _pct_reach_above_mode = MHR::_finger_pct_reach_above_mode;
}

void BalancedModeFrameToSignalHelper::convert(std::vector<Mat> &eulerianVid, std::vector<double> &sig, double frameRate, bool isCalcMode) {
    int height = eulerianVid[0].rows;
    int width = eulerianVid[0].cols;
    int total_frames = (int)eulerianVid.size();
    sig.reserve(total_frames);
    if (isCalcMode) {
            // Selection parameters
        double lower_pct_range = _pct_reach_below_mode;
        double upper_pct_range = _pct_reach_above_mode;
        
        int first_training_frames_start = min( (int)round(frameRate * _training_time_start), total_frames );
        int first_training_frames_end = min( (int)round(frameRate * _training_time_end), total_frames) - 1;
        
            // this arr stores values of pixels from first trainning frames
        vector<double> arr;
        for(int i = first_training_frames_start; i <= first_training_frames_end; ++i)
            for(int y=0; y<width; ++y)
                for(int x=0; x<height; ++x)
                    arr.push_back(eulerianVid[i].at<double>(x,y));
        
            //find the mode
        vector<double> centres;
        vector<int> counts;
        
        MHR::hist(arr, _number_of_bins, counts, centres);
        
        int argmax=0;
        for(int i=0; i<(int)counts.size(); ++i) if(counts[i]>counts[argmax]) argmax = i;
        double centre_mode = centres[argmax];
        
            // find the percentile range centred on the mode
        double percentile_of_centre_mode = MHR::invprctile(arr, centre_mode);
        double percentile_lower_range = max(0.0, percentile_of_centre_mode - lower_pct_range);
        double percentile_upper_range = min(100.0, percentile_of_centre_mode + upper_pct_range);
            // correct the percentile range for the boundary cases
        if(percentile_upper_range == 100)
            percentile_lower_range = 100 - (lower_pct_range + upper_pct_range);
        if(percentile_lower_range == 0)
            percentile_upper_range = (lower_pct_range + upper_pct_range);
        
            //convert the percentile range into pixel-value range
        lower_range = MHR::prctile(arr, percentile_lower_range);
        upper_range = MHR::prctile(arr, percentile_upper_range);
        
        if (MHR::_DEBUG_MODE)
            printf("lower_range = %lf, upper_range = %lf\n", lower_range, upper_range);
        }
    
        //now calc the avg of each frame while inogre the values outside the range
        //this is the debug vector<Mat>
    for(int i=0; i<total_frames; ++i){
        double sum = 0;
        int cnt = 0; //number of not-NaN-pixels
        for(int x=0; x<height; ++x)
            for(int y=0; y<width; ++y){
                double val=eulerianVid[i].at<double>(x,y);
                if(val<lower_range - 1e-9 || val>upper_range + 1e-9){
                    val=0;
                }else ++cnt;
                sum+=val;
            }
        
        if(cnt==0) //push NaN for all-NaN-frames
            sig.push_back(MHR::NaN);
        else
            sig.push_back(sum/cnt);
    }
    
}

