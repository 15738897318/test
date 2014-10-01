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
    ideal_bandpassing(GdownStack, filteredStack, samplingRate);
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
    if (MHR::_DEBUG_MODE) printf("Rendering...\n");
		// output video
		// Convert each frame from the filtered stream to movie frame
    Mat frame, filtered;
    if (MHR::_THREE_CHAN_MODE) {
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
    GDownStack.push_back(blurred.clone());
    
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
        GDownStack.push_back(blurred.clone());
    }
}

void EulerianMagnificationHelper::ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, double samplingRate) {
        //        src: T*M*N*C;
    
        // extract src info
    int nTime = (int)src.size();
    int nRow = src[0].rows;
    int nCol = src[0].cols;
    int nChannel = (MHR::_THREE_CHAN_MODE) ? _number_of_channels : 1;
    
        // copy and convert data from src to dst (CV_32FC(nChannels))
    Mat tmp;
    dst.clear();
    for (int i = 0; i < nTime; ++i)
        {
        if (MHR::_THREE_CHAN_MODE)
            src[i].convertTo(tmp, CV_32FC3);
        else
            src[i].convertTo(tmp, CV_32F);
        dst.push_back(tmp.clone());
        }
    
        // masking indexes
    int f1 = ceil(_eulerian_minHR/60.0 * nTime/samplingRate);
    int f2 = floor(_eulerian_maxHR/60.0 * nTime/samplingRate);
    int ind1 = 2*f1, ind2 = 2*f2 - 1;
    
        // FFT: http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#dft
    Mat dft_out = Mat::zeros(nRow, nTime, CV_32F);
    for (int channel = 0; channel < nChannel; ++channel) {
        for (int col = 0; col < nCol; ++col) {
                // select only 1 channel in the dst's Mats
            for (int time = 0; time < nTime; ++time)
                for (int row = 0; row < nRow; ++row)
                    if (MHR::_THREE_CHAN_MODE)
                        dft_out.at<float>(row, time) = dst[time].at<Vec3f>(row, col)[channel];
                    else
                        dft_out.at<float>(row, time) = dst[time].at<float>(row, col);
            
                // call FFT
            dft(dft_out, dft_out, DFT_ROWS);
            
                // masking: all elements with time-index in ranges [0, ind1] and [ind2, nTime-1]
                // will be set to 0
            for (int row = 0; row < nRow; ++row) {
                for (int time = 0; time <= ind1; ++time)
                    dft_out.at<float>(row, time) = 0;
                for (int time = ind2; time < nTime; ++time)
                    dft_out.at<float>(row, time) = 0;
            }
            
                // assign values in dft_out to dst
            dft(dft_out, dft_out, DFT_ROWS + DFT_INVERSE + DFT_REAL_OUTPUT + DFT_SCALE);
            for (int time = 0; time < nTime; ++time)
                for (int row = 0; row < nRow; ++row)
                    if (MHR::_THREE_CHAN_MODE)
                        dst[time].at<Vec3f>(row, col)[channel] = dft_out.at<float>(row, time);
                    else
                        dst[time].at<float>(row, col) = dft_out.at<float>(row, time);
        }
    }
    
        // convert the dst Mat to CV_64FC3 or CV_64F
    for (int i = 0; i < nTime; ++i)
        if (MHR::_THREE_CHAN_MODE)
            dst[i].convertTo(dst[i], CV_64FC3);
        else
            dst[i].convertTo(dst[i], CV_64F);
}
