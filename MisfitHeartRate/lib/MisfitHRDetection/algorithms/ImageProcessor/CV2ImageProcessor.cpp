//
//  CV2ImageProcessor.cpp
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/25/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#include "CV2ImageProcessor.h"
#include "files.h"
#include "build_Gdown_stack.h"
#include "ideal_bandpassing.h"
#define READ_INFO_FAILED_NO_FRAME 1
#define READ_INFO_SUCCESS 0

using namespace MHR;

void CV2ImageProcessor::setFaceParams() {
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
    
    _window_size_in_sec = _face_window_size_in_sec;
    _overlap_ratio = _face_overlap_ratio;
    _max_bpm = _face_max_bpm;
    _cutoff_freq = _face_cutoff_freq;
    _time_lag = _face_time_lag;
    _colourspace = new char[_face_colourspace.length() + 1];
    strcpy(_colourspace, _face_colourspace.c_str());
    _channels_to_process = _face_channels_to_process;
    _number_of_bins_heartRate = _face_number_of_bins_heartRate;
    
    _flagDebug = _face_flagDebug;
    _flagGetRaw = _face_flagGetRaw;
    
    _startIndex = _face_startIndex;
    _endIndex = _face_endIndex;
    
    _peakStrengthThreshold_fraction = _face_peakStrengthThreshold_fraction;
    _frames2signalConversionMethod = new char[_face_frames2signalConversionMethod.length()];
    strcpy(_frames2signalConversionMethod,_face_frames2signalConversionMethod.c_str());
    
    _frame_downsampling_filt_rows = _face_frame_downsampling_filt_rows;
    _frame_downsampling_filt_cols = _face_frame_downsampling_filt_cols;
    _frame_downsampling_filt = _face_frame_downsampling_filt.clone();
    
    _trimmed_size = _face_trimmed_size;
    
    _training_time_start = _face_training_time_start;
    _training_time_end = _face_training_time_end;
    _number_of_bins = _face_number_of_bins;
    _pct_reach_below_mode = _face_pct_reach_below_mode;
    _pct_reach_above_mode = _face_pct_reach_above_mode;
}

void CV2ImageProcessor::setFingerParams()
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
    
    _window_size_in_sec = _finger_window_size_in_sec;
    _overlap_ratio = _finger_overlap_ratio;
    _max_bpm = _finger_max_bpm;
    _cutoff_freq = _finger_cutoff_freq;
    _time_lag = _finger_time_lag;
    _colourspace = new char[_finger_colourspace.length()];
    strcpy(_colourspace, _finger_colourspace.c_str());
    _channels_to_process = _finger_channels_to_process;
    _number_of_bins_heartRate = _finger_number_of_bins_heartRate;
    
    _flagDebug = _finger_flagDebug;
    _flagGetRaw = _finger_flagGetRaw;
    
    _startIndex = _finger_startIndex;
    _endIndex = _finger_endIndex;
    
    _peakStrengthThreshold_fraction = _finger_peakStrengthThreshold_fraction;
    _frames2signalConversionMethod = new char[_finger_frames2signalConversionMethod.length()];
    strcpy(_frames2signalConversionMethod,_finger_frames2signalConversionMethod.c_str());
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


CV2ImageProcessor::CV2ImageProcessor() {
    nFrames = 0;
    currentFrame = -1;
    isCalcMode = true;
    window_size = round(_window_size_in_sec * _frameRate);
    firstSample = round(_frameRate * _time_lag);
    threshold_fraction = 0;
    vid.reserve(_framesBlock_size);
    eulerianVid.reserve(_framesBlock_size);
    setFaceParams();
};

int CV2ImageProcessor::readFrameInfo() {
    char *fileName = new char[strlen(srcDir) + 20];
    sprintf(fileName, "%s/input_frames.txt", srcDir);
    FILE *file = fopen(fileName, "r");
    if (file) fscanf(file, "%d", &nFrames);
    fclose(file);
    if (nFrames <= 0)
    {
        if (_DEBUG_MODE) printf("nFrames == 0\n");
        return READ_INFO_FAILED_NO_FRAME;
    } else {
        return READ_INFO_SUCCESS;
    }
};

void CV2ImageProcessor::readFrames() {
    vid.clear();
    eulerianVid.clear();
    char *fileName = new char[strlen(srcDir) + 30];
    for (int i = 0; i < _framesBlock_size; ++i) {
        ++currentFrame;
        sprintf(fileName,"%s/input_frame[%d].png",srcDir,currentFrame);
            //what happens if readFrame fails
        Mat frame = imread(fileName);
        cvtColor(frame, frame, CV_BGR2RGB);
        
        if (_THREE_CHAN_MODE)
            vid.push_back(frame.clone());
        else {
                // if using 1-chan mode, then do the colour conversion here
            frame.convertTo(frame, CV_64FC3);
            if (!strcmp(_colourspace,"hsv"))
                cvtColor(frame, frame, CV_RGB2HSV);
            else if (!strcmp(_colourspace,"ycbcr"))
                cvtColor(frame, frame, CV_RGB2YCrCb);
            else if (!strcmp(_colourspace,"tsl"))
                rgb2tsl(frame, frame);
            
                // extracts 1 channel only from the frame
            Mat tmp = Mat::zeros(frame.rows, frame.cols, CV_64F);
            for (int i = 0; i < frame.rows; ++i)
                for (int j = 0; j < frame.cols; ++j)
                    tmp.at<double>(i, j) = frame.at<Vec3d>(i, j)[_channels_to_process];
            vid.push_back(tmp.clone());
        }

        if (currentFrame >= nFrames - 1) break;
    }
    eulerianGaussianPyramidMagnification();
}

void CV2ImageProcessor::eulerianGaussianPyramidMagnification() {
    eulerianVid.clear();
		// Extract video info
    int vidHeight = vid[0].rows;
    int vidWidth = vid[0].cols;
    int nChannels = _number_of_channels;
    int frameRate = _frameRate;
    int len = (int)vid.size();
    
    double samplingRate = _eulerian_frameRate;
    int level = _eulerian_pyrLevel < (int)floor(log(min(vidHeight, vidWidth) / _Gpyr_filter_length) / log(2))?_eulerian_pyrLevel : (int)floor(log(min(vidHeight, vidWidth) / _Gpyr_filter_length) / log(2));
    
		// Define the indices of the frames to be processed
    int startIndex = _startFrame;
    int endIndex = len - 1;
    if (_endFrame > 0)
        endIndex = min(endIndex, _endFrame);
    else
        endIndex = max(0, endIndex + _endFrame);
    
		// ================= Core part of the algo described in literature
		// compute Gaussian blur stack
		// This stack actually is just a single level of the pyramid
    if (_DEBUG_MODE) printf("Spatial filtering...\n");
    vector<Mat> GdownStack;
    build_Gdown_Stack(vid, GdownStack, startIndex, endIndex, level);
    if (_DEBUG_MODE) printf("Finished\n");
    
		// Temporal filtering
    if (_DEBUG_MODE) printf("Temporal filtering...\n");
    vector<Mat> filteredStack;
    ideal_bandpassing(GdownStack, filteredStack, _eulerian_minHR/60.0, _eulerian_maxHR/60.0, samplingRate);
    if (_DEBUG_MODE) printf("Finished\n");
    
    
		// amplify
    int nTime = (int)filteredStack.size();
    int nRow = filteredStack[0].rows;
    int nCol = filteredStack[0].cols;
    if (_THREE_CHAN_MODE) {
        Mat base_B = (Mat_<double>(3, 3) <<
                      _eulerian_alpha, 0, 0,
                      0, _eulerian_alpha*_eulerian_chromaMagnifier, 0,
                      0, 0, _eulerian_alpha*_eulerian_chromaMagnifier);
        Mat base_C = (ntsc2rgb_baseMat * base_B) * rgb2ntsc_baseMat;
        
            // calculate filteredStack[t] = baseC * filteredStack[t]
        Mat tmp = Mat::zeros(nChannels, nCol, CV_64F);
        for (int t = 0; t < nTime; ++t) {
            for (int i = 0; i < nRow; ++i) {
                for (int j = 0; j < nCol; ++j)
                    for (int channel = 0; channel < nChannels; ++channel)
                        tmp.at<double>(channel, j) = filteredStack[t].at<Vec3d>(i, j)[channel];
                
                tmp = base_C * tmp;
                
                for (int j = 0; j < nCol; ++j)
                    for (int channel = 0; channel < nChannels; ++channel)
                        filteredStack[t].at<Vec3d>(i, j)[channel] = tmp.at<double>(channel, j);
            }
        }
    }
    else {
        for (int t = 0; t < nTime; ++t)
            for (int i = 0; i < nRow; ++i)
                for (int j = 0; j < nCol; ++j)
                    filteredStack[t].at<double>(i, j) = _eulerian_alpha * filteredStack[t].at<double>(i, j);
    }
    
    
		// =================
    
		// Render on the input video
    if (_DEBUG_MODE) printf("Rendering...\n");
		// output video
		// Convert each frame from the filtered stream to movie frame
    Mat frame, filtered;
    if (_THREE_CHAN_MODE) {
        for (int i = startIndex, k = 0; i <= endIndex && k < nTime; ++i, ++k) {
            	// Reconstruct the frame from pyramid stack
				// by removing the singleton dimensions of the kth filtered array
				// since the filtered stack is just a selected level of the Gaussian pyramid
            
				// Format the image to the right size
            resize(filteredStack[k], filtered, cvSize(vidWidth, vidHeight), 0, 0, INTER_CUBIC);
            
				// Convert the ith frame in the video stream to RGB (double-precision) image
            vid[i].convertTo(frame, CV_64FC3);
            
				// Add the filtered frame to the original frame
            filtered = filtered + frame;
            
            for (int i = 0; i < vidHeight; ++i)
                for (int j = 0; j < vidWidth; ++j) {
                    for (int channel = 0; channel < nChannels; ++channel) {
                        double tmp = filtered.at<Vec3d>(i, j)[channel];
                        
                        tmp = min(tmp, 255.0);
                        tmp = max(tmp, 0.0);
                        
                        filtered.at<Vec3d>(i, j)[channel] = tmp;
                    }
                }
            eulerianVid.push_back(filtered.clone());
        }
    }
    else {
        for (int i = startIndex, k = 0; i <= endIndex && k < nTime; ++i, ++k) {
            	// Reconstruct the frame from pyramid stack
				// by removing the singleton dimensions of the kth filtered array
				// since the filtered stack is just a selected level of the Gaussian pyramid
            
				// Format the image to the right size
            resize(filteredStack[k], filtered, cvSize(vidWidth, vidHeight), 0, 0, INTER_CUBIC);
            
				// Convert the ith frame in the video stream to RGB (double-precision) image
            vid[i].convertTo(frame, CV_64F);
            
				// Add the filtered frame to the original frame
            filtered = filtered + frame;
            
            for (int i = 0; i < vidHeight; ++i)
                for (int j = 0; j < vidWidth; ++j) {
                    double tmp = filtered.at<double>(i, j);
                    
                    tmp = min(tmp, 255.0);
                    tmp = max(tmp, 0.0);
                    
                    filtered.at<double>(i, j) = tmp;
                }
            eulerianVid.push_back(filtered.clone());
        }
    }
}



