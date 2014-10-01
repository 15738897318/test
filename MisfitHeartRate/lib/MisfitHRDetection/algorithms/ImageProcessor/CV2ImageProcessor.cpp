//
//  CV2ImageProcessor.cpp
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/25/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#include "CV2ImageProcessor.h"
#include "files.h"
#include "matlab.h"
#include "EulerianMagnificationHelper.h"
#include "FrameToSignalHelper.h"
#define READ_INFO_FAILED_NO_FRAME 1
#define READ_INFO_SUCCESS 0

using namespace MHR;

void CV2ImageProcessor::setFaceParams() {
    if (_DEBUG_MODE) printf("setFaceParams()\n");
    
    _eHelper->setFaceParams();
    
    
    _colourspace = new char[_face_colourspace.length() + 1];
    strcpy(_colourspace, _face_colourspace.c_str());
    _channels_to_process = _face_channels_to_process;
    
    _frames2signalConversionMethod = new char[_face_frames2signalConversionMethod.length()];
    strcpy(_frames2signalConversionMethod,_face_frames2signalConversionMethod.c_str());
    if (!strcmp(_frames2signalConversionMethod,"simple-mean")) {
        _f2sHelper = new SimpleMeanFrameToSignalHelper();
    } else if (!strcmp(_frames2signalConversionMethod, "trimmed-mean")) {
        _f2sHelper = new TrimmedMeanFrameToSignalHelper();
    } else if (!strcmp(_frames2signalConversionMethod,"mode-balance")) {
        _f2sHelper = new BalancedModeFrameToSignalHelper();
    }
    _f2sHelper->setFaceParams();
    
    _frame_downsampling_filt_rows = _face_frame_downsampling_filt_rows;
    _frame_downsampling_filt_cols = _face_frame_downsampling_filt_cols;
    _frame_downsampling_filt = _face_frame_downsampling_filt.clone();
}

void CV2ImageProcessor::setFingerParams()
{
    if (_DEBUG_MODE) printf("setFingerParams()\n");
    
    _eHelper->setFingerParams();

    
    _colourspace = new char[_finger_colourspace.length()];
    strcpy(_colourspace, _finger_colourspace.c_str());
    _channels_to_process = _finger_channels_to_process;

    _frames2signalConversionMethod = new char[_finger_frames2signalConversionMethod.length()];
    strcpy(_frames2signalConversionMethod,_finger_frames2signalConversionMethod.c_str());
    if (!strcmp(_frames2signalConversionMethod,"simple-mean")) {
        _f2sHelper = new SimpleMeanFrameToSignalHelper();
    } else if (!strcmp(_frames2signalConversionMethod, "trimmed-mean")) {
        _f2sHelper = new TrimmedMeanFrameToSignalHelper();
    } else if (!strcmp(_frames2signalConversionMethod,"mode-balance")) {
        _f2sHelper = new BalancedModeFrameToSignalHelper();
    }
    _f2sHelper->setFingerParams();
    _frame_downsampling_filt_rows = _finger_frame_downsampling_filt_rows;
    _frame_downsampling_filt_cols = _finger_frame_downsampling_filt_cols;
    _frame_downsampling_filt = _finger_frame_downsampling_filt.clone();
}


CV2ImageProcessor::CV2ImageProcessor() {
    nFrames = 0;
    currentFrame = -1;
    isCalcMode = true;
    vid.reserve(_framesBlock_size);
    eulerianVid.reserve(_framesBlock_size);
    _eHelper = new EulerianMagnificationHelper();
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
    Mat frame;
    char *fileName = new char[strlen(srcDir) + 30];
    for (int i = 0; i < _framesBlock_size; ++i) {
        ++currentFrame;
        sprintf(fileName,"%s/input_frame[%d].png",srcDir,currentFrame);
            //what happens if readFrame fails
        frame = imread(fileName);
            //skip if read image fail
        if (frame.data == NULL) {
            vid.push_back(frame.clone());
            continue;
        }
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
    _eHelper->eulerianGaussianPyramidMagnification(vid, eulerianVid);
}

void CV2ImageProcessor::temporal_mean_calc(vector<double> &temp) {
        // Block 1 ==== Load the video & convert it to the desired colour-space
        // Extract video info
    int vidHeight = vid[0].rows;
    int vidWidth = vid[0].cols;
    int len = (int)vid.size();
    
        // Define the indices of the frames to be processed
    int startIndex = 0;     // 400
    int endIndex = len;   // 1400
    
        // Convert colourspaces for each frame
    Mat filt = _frame_downsampling_filt.clone();
    Mat tmp_monoframe = Mat::zeros(vidHeight/4 + int(vidHeight%4 > 0), vidWidth/4 + int(vidWidth%4 > 0), CV_64F);
    Mat frame, monoframe = Mat::zeros(vidHeight, vidWidth, CV_64F);
    vector<Mat> monoframes;
    
    if (_THREE_CHAN_MODE) {
        for (int i = startIndex, k = 0; i < endIndex; ++i, ++k) {
                // convert each frame to right colourspace
            eulerianVid[i].convertTo(frame, CV_64FC3);
            if (!strcmp(_colourspace,"hsv"))
                cvtColor(frame, frame, CV_RGB2HSV);
            else if (!strcmp(_colourspace,"ycbcr"))
                cvtColor(frame, frame, CV_RGB2YCrCb);
            else if (!strcmp(_colourspace,"tsl"))
                rgb2tsl(frame, frame);
			
				// Extract the right channel from the colour frame
				// if only 1 channel ---> don't use monoframe.
            for (int x = 0; x < vidHeight; ++x)
                for (int y = 0; y < vidWidth; ++y)
                    monoframe.at<double>(x, y) = frame.at<Vec3d>(x, y)[_channels_to_process];
			
				// Downsample the frame for ease of computation
            corrDn(monoframe, tmp_monoframe, filt, 4, 4);
			
				// Put the frame into the video stream
            monoframes.push_back(tmp_monoframe.clone());
        }
    }
    else {
        for (int i = startIndex, k = 0; i < endIndex; ++i, ++k) {
            eulerianVid[i].convertTo(frame, CV_64F);
			
				// Downsample the frame for ease of computation
            corrDn(frame, tmp_monoframe, filt, 4, 4);
			
				// Put the frame into the video stream
            monoframes.push_back(tmp_monoframe.clone());
        }
    }
    
        // Block 2 ==== Extract a signal stream & pre-process it
        // Convert the frame stream into a 1-D signal
    vector<double> tmp ;
    _f2sHelper->convert(monoframes, tmp, MHR::_frameRate, isCalcMode);
    isCalcMode = false;
    temp.insert(temp.end(),tmp.begin(),tmp.end());
}

void CV2ImageProcessor::writeArray(vector<double> &arr) {
    temporal_mean_calc(arr);
}