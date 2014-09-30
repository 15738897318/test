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
    double lower_range;
    double upper_range;
        //-------------------------------------------------------------------------------//
    
    /*--------------for run_eulerian()--------------*/
    double _eulerian_alpha;          // Eulerian magnifier, standard < 50
    double _eulerian_pyrLevel;        // Standard: 4, but updated by the real frame size
    double _eulerian_minHR;          // BPM Standard: 50
    double _eulerian_maxHR;         // BPM Standard: 90
    double _eulerian_frameRate;      // Standard: 30, but updated by the real frame-rate
    double _eulerian_chromaMagnifier; // Standard: 1
    
        // Native params of the algorithm
    int _number_of_channels;
    int _Gpyr_filter_length;
    int _startFrame;
    int _endFrame; // >= 0 to get definite end-frame, < 0 to get end-frame relative to stream length
    
    /*--------------for run_hr()--------------*/
    char *_colourspace;
    int _channels_to_process;     // If only 1 channel: 1 for tsl, 0 for rgb
    
    
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
    /**
	 * Apply Gaussian pyramid decomposition on \a vid from \a startIndex to \a endIndex,
	 * and select a specific band indicated by \a level. \n
	 * \return \a GDownStack is stack of one band of Gaussian pyramid of each frame
     * \param vid,GDownStack:
	 *  + the first dimension is the time axis \n
	 *  + the second dimension is the y axis of the video's frames \n
	 *  + the third dimension is the x axis of the video's frames \n
	 *  + the forth dimension is the color channel \n
     * Data type: CV_64FC3 or CV_64F
	 */
	void build_Gdown_Stack(vector<Mat> &GDownStack, int startIndex, int endIndex, int level);
    /**
	 * Apply ideal band pass filter on \a src. \n
     * \ref: http://en.wikipedia.org/wiki/Band-pass_filter
     * \param src,dst:
	 *  + the first dimension is the time axis \n
	 *  + the second dimension is the y axis of the video's frames \n
	 *  + the third dimension is the x axis of the video's frames \n
	 *  + the forth dimension is the color channel \n
	 * \param samplingRate sampling rate of \a src \n
     * Data type: CV_64FC3 or CV_64F
	 */
    void ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, double samplingRate);
    /**
     * Convert frames of <vid> to signals.
     * \param vid data type is CV_64FC3 or CV_64F
     * \param overlap_ratio overlap ratio between 2 consecutive segments
     * \param max_bpm maximum heart rate detectable (use in determining minPeaksDistance in findpeaks())
     * \param colour_channel if in _THREE_CHAN_MODE, then convert all frames of \a vid to
     *  monoframes by select only one channel of each frame.
     * \param colourspace if in _THREE_CHAN_MODE, then convert colourspace of
     *  all frames of \a vid to "hsv", "ycbcr" or "tsl" before converting them to monoframes
     * \param cutoff_freq,lower_range,upper_range>,isCalcMode: see frames2signal()
     *
     * NOTE: params are provided in the class member
     */
    
        //!
        //! The function will convert the array of frames into an array of signal value (type double)
        //! note that the frame is mono channel.
        //! \param fr video's frame rate
        //! \param conversion_method we have 3 method for converting a frame into a double value:
        //! + simple-mean
        //! + trimmed-mean
        //! + mode-balance
        //!
    vector<double> frames2signal(const vector<Mat>& monoframes, const String &conversion_method,
                                 double fr, double cutoff_freq,
                                 double &lower_range, double &upper_range, bool isCalcMode);
    void temporal_mean_calc(vector<double> &temp);

public:
    CV2ImageProcessor();
    ~CV2ImageProcessor();
    void setFaceParams();
    void setFingerParams();
    int readFrameInfo();
    void readFrames();
//    void writeFrames(uint8_t numFrame, uint16_t offset);
//    void setArrayInfo(TemporalArray &arr);
    void writeArray(vector<double> &arr);
        /*---------Test functions, to be removed------------*/
    
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
