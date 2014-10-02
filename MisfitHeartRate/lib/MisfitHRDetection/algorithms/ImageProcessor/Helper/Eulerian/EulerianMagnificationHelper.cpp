//
//  EulerianMagnificationHelper.cpp
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 10/1/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#include "EulerianMagnificationHelper.h"
#include "image.h"

void EulerianMagnificationHelper::setFaceParams() {
    _eulerian_alpha = MHR::_face_eulerian_alpha;
    _eulerian_pyrLevel = MHR::_face_eulerian_pyrLevel;
    _eulerian_minHR = MHR::_face_eulerian_minHR;
    _eulerian_maxHR = MHR::_face_eulerian_maxHR;
    _eulerian_frameRate = MHR::_face_eulerian_frameRate;
    _eulerian_chromaMagnifier = MHR::_face_eulerian_chromaMagnifier;
    
    _number_of_channels = MHR::_face_number_of_channels;
    _Gpyr_filter_length = MHR::_face_Gpyr_filter_length;
    _startFrame = MHR::_face_startFrame;
    _endFrame = MHR::_face_endFrame;
}

void EulerianMagnificationHelper::setFingerParams() {
    _eulerian_alpha = MHR::_finger_eulerian_alpha;
    _eulerian_pyrLevel = MHR::_finger_eulerian_pyrLevel;
    _eulerian_minHR = MHR::_finger_eulerian_minHR;
    _eulerian_maxHR = MHR::_finger_eulerian_maxHR;
    _eulerian_frameRate = MHR::_finger_eulerian_frameRate;
    _eulerian_chromaMagnifier = MHR::_finger_eulerian_chromaMagnifier;
    
    _number_of_channels = MHR::_finger_number_of_channels;
    _Gpyr_filter_length = MHR::_finger_Gpyr_filter_length;
    _startFrame = MHR::_finger_startFrame;
    _endFrame = MHR::_finger_endFrame;
}

void EulerianMagnificationHelper::eulerianGaussianPyramidMagnification(vector<Mat> &vid, vector<Mat> &eulerianVid) {
    eulerianVid.clear();
		// Extract video info
    int vidHeight = vid[0].rows;
    int vidWidth = vid[0].cols;
    int nChannels = _number_of_channels;
    int len = (int)vid.size();
    
    double samplingRate = MHR::_frameRate;
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
    if (MHR::_DEBUG_MODE) printf("Spatial filtering...\n");
    vector<Mat> GdownStack;
    build_Gdown_Stack(vid, GdownStack, startIndex, endIndex, level);
    if (MHR::_DEBUG_MODE) printf("Finished\n");
    
		// Temporal filtering
    if (MHR::_DEBUG_MODE) printf("Temporal filtering...\n");
    vector<Mat> filteredStack;
    int nChannel = (MHR::_THREE_CHAN_MODE) ? _number_of_channels : 1;
    MHR::ideal_bandpassing(GdownStack, filteredStack, _eulerian_minHR/60.0, _eulerian_maxHR/60.0, nChannel, samplingRate);
    if (MHR::_DEBUG_MODE) printf("Finished\n");
    
    
		// amplify
    int nTime = (int)filteredStack.size();
    int nRow = filteredStack[0].rows;
    int nCol = filteredStack[0].cols;
    if (MHR::_THREE_CHAN_MODE) {
        Mat base_B = (Mat_<double>(3, 3) <<
                      _eulerian_alpha, 0, 0,
                      0, _eulerian_alpha*_eulerian_chromaMagnifier, 0,
                      0, 0, _eulerian_alpha*_eulerian_chromaMagnifier);
        Mat base_C = (MHR::ntsc2rgb_baseMat * base_B) * MHR::rgb2ntsc_baseMat;
        
            // calculate filteredStack[t] = baseC * filteredStack[t]
        Mat tmp = Mat::zeros(3, nCol, CV_64F);
        Mat rgb_channels[3];
        for (int t = 0; t < nTime; ++t) {
            split(filteredStack[t], rgb_channels);
            for (int i = 0; i < nRow; ++i) {
                for (int channel = 0; channel < 3; ++channel)
                    rgb_channels[channel].row(i).copyTo(tmp.row(channel));
                
                tmp = base_C * tmp;
                
                for (int channel = 0; channel < 3; ++channel)
                    tmp.row(channel).copyTo(rgb_channels[channel].row(i));
            }
            merge(rgb_channels, 3, filteredStack[t]);
        }
        
    }
    else {
        for (int t = 0; t < nTime; ++t)
            filteredStack[t] = _eulerian_alpha * filteredStack[t];
    }
    
    
		// =================
    
		// Render on the input video
    if (MHR::_DEBUG_MODE) printf("Rendering...\n");
		// output video
		// Convert each frame from the filtered stream to movie frame
    Mat frame, filtered;
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
            
            filtered = max(min(filtered, 255.0), 0.0);
            eulerianVid.emplace_back(filtered.clone());
        }
}


void EulerianMagnificationHelper::build_Gdown_Stack(vector<Mat> &vid, vector<Mat> &GDownStack, int startIndex, int endIndex, int level) {
        // firstFrame
    Mat frame;
    if (MHR::_THREE_CHAN_MODE)
        vid[startIndex].convertTo(frame, CV_64FC3);
    else
        vid[startIndex].convertTo(frame, CV_64F);
    
        // Blur and downsample the frame
    Mat blurred;
    MHR::blurDnClr(frame, blurred, level);
    
        // create pyr stack
        // Note that this stack is actually just a SINGLE level of the pyramid
        // The first frame in the stack is saved
    GDownStack.clear();
    GDownStack.emplace_back(blurred.clone());
    
    for (int i = startIndex+1, k = 1; i <= endIndex; ++i, ++k) {
            // Create a frame from the ith array in the stream
        if (MHR::_THREE_CHAN_MODE)
            vid[i].convertTo(frame, CV_64FC3);
        else
            vid[i].convertTo(frame, CV_64F);
        
            // Blur and downsample the frame
        MHR::blurDnClr(frame, blurred, level);
        
            // The kth element in the stack is saved
            // Note that this stack is actually just a SINGLE level of the pyramid
        GDownStack.emplace_back(blurred.clone());
    }
}