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
    _cutoff_freq = MHR::_face_cutoff_freq;
    
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
    
    _eHelper->setFingerParams();

    
    _colourspace = new char[_finger_colourspace.length()];
    strcpy(_colourspace, _finger_colourspace.c_str());
    _cutoff_freq = MHR::_finger_cutoff_freq;
    _channels_to_process = _finger_channels_to_process;

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
    double frameRate = MHR::_frameRate;
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
    vector<double> tmp = frames2signal(monoframes, _frames2signalConversionMethod, frameRate, _cutoff_freq,
                         lower_range, upper_range, isCalcMode);
    isCalcMode = false;
    temp.insert(temp.end(),tmp.begin(),tmp.end());
}

void CV2ImageProcessor::writeArray(vector<double> &arr) {
    temporal_mean_calc(arr);
}


vector<double>  CV2ImageProcessor::frames2signal(const vector<Mat>& monoframes, const String &conversion_method,
                             double fr, double cutoff_freq,
                             double &lower_range, double &upper_range, bool isCalcMode)
{
    clock_t t1 = clock();
    
        //=== Block 1. Convert the frame stream into a 1-D signal
    
    vector<double> temporal_mean;
    int height = monoframes[0].rows;
    int width = monoframes[0].cols;
    int total_frames = (int)monoframes.size();
    
    
    
    if(conversion_method == "simple-mean"){
            //!
            //! mode : 'simple-mean'
            //! get the mean of all pixel's value in the picture frame
            //!
        double size = height * width;
        for(int i=0; i<total_frames; ++i){
            double sum = 0;
            for(int x=0; x<height; ++x)
                for(int y=0; y<width; ++y)
                    sum+=monoframes[i].at<double>(x,y);
            temporal_mean.push_back(sum/size);
        }
        
    }else if(conversion_method == "trimmed-mean"){
        
            //Set the trimmed size here
        int trimmed_size = _trimmed_size;
            //!
            //! mode : 'trimmed-mean'
            //! get the mean of all pixel's value in a smaller rectangle inside the picture frame
            //!
        double size = (height - trimmed_size * 2) * (width - trimmed_size * 2);
        for(int i=0; i<total_frames; ++i){
            double sum = 0;
            for(int x=trimmed_size; x<height-trimmed_size; ++x)
                for(int y=trimmed_size; y<width-trimmed_size; ++y)
                    sum+=monoframes[i].at<double>(x,y);
            temporal_mean.push_back(sum/size);
        }
        
    }else if(conversion_method == "mode-balance"){
            //!
            //! this method will calculate the histogram of pixel's value from the first_training_frames_start to first_training_frames_end. Then get the bin that has the most number of value, get the centre of that bin as a centre value, then use the prctile function to get the percentile of that centre value.
            //! Finally we calculate the mean of values that have the inverted percentile in the range from (centre value's percentile - lower_pct_range) to (centre value's percentile + upper_pct_range).
            //!
        if (isCalcMode)
            {
                // Selection parameters
            double lower_pct_range = _pct_reach_below_mode;
            double upper_pct_range = _pct_reach_above_mode;
            
            int first_training_frames_start = min( (int)round(fr * _training_time_start), total_frames );
            int first_training_frames_end = min( (int)round(fr * _training_time_end), total_frames) - 1;
            
                // this arr stores values of pixels from first trainning frames
            vector<double> arr;
            for(int i = first_training_frames_start; i <= first_training_frames_end; ++i)
                for(int y=0; y<width; ++y)
                    for(int x=0; x<height; ++x)
                        arr.push_back(monoframes[i].at<double>(x,y));
            
                //find the mode
            vector<double> centres;
            vector<int> counts;
            
            hist(arr, _number_of_bins, counts, centres);
            
            int argmax=0;
            for(int i=0; i<(int)counts.size(); ++i) if(counts[i]>counts[argmax]) argmax = i;
            double centre_mode = centres[argmax];
            
                // find the percentile range centred on the mode
            double percentile_of_centre_mode = invprctile(arr, centre_mode);
            double percentile_lower_range = max(0.0, percentile_of_centre_mode - lower_pct_range);
            double percentile_upper_range = min(100.0, percentile_of_centre_mode + upper_pct_range);
                // correct the percentile range for the boundary cases
            if(percentile_upper_range == 100)
                percentile_lower_range = 100 - (lower_pct_range + upper_pct_range);
            if(percentile_lower_range == 0)
                percentile_upper_range = (lower_pct_range + upper_pct_range);
            
                //convert the percentile range into pixel-value range
            lower_range = prctile(arr, percentile_lower_range);
            upper_range = prctile(arr, percentile_upper_range);
            
            if (_DEBUG_MODE)
                printf("lower_range = %lf, upper_range = %lf\n", lower_range, upper_range);
            }
        
            //now calc the avg of each frame while inogre the values outside the range
            //this is the debug vector<Mat>
        for(int i=0; i<total_frames; ++i){
            double sum = 0;
            int cnt = 0; //number of not-NaN-pixels
            for(int x=0; x<height; ++x)
                for(int y=0; y<width; ++y){
                    double val=monoframes[i].at<double>(x,y);
                    if(val<lower_range - 1e-9 || val>upper_range + 1e-9){
                        val=0;
                    }else ++cnt;
                    sum+=val;
                }
            
            if(cnt==0) //push NaN for all-NaN-frames
                temporal_mean.push_back(NaN);
            else
                temporal_mean.push_back(sum/cnt);
        }
        
    }
    
    if (_DEBUG_MODE)
        printf("frames2signal() runtime = %f\n", ((float)clock() - (float)t1)/CLOCKS_PER_SEC);
    
    return temporal_mean;
}





