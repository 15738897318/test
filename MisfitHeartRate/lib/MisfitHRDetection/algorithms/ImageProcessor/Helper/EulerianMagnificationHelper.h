//
//  EulerianMagnificationHelper.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 10/1/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef __MIsfitHRDetection__EulerianMagnificationHelper__
#define __MIsfitHRDetection__EulerianMagnificationHelper__

#include <iostream>
#include "config.h"
using namespace std;

class EulerianMagnificationHelper {
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
public:
    void setFaceParams();
    void setFingerParams();
    
    void eulerianGaussianPyramidMagnification(vector<Mat> &vid, vector<Mat> &eulerianVid);
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
	void build_Gdown_Stack(vector<Mat> &vid, vector<Mat> &GDownStack, int startIndex, int endIndex, int level);
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
};

#endif /* defined(__MIsfitHRDetection__EulerianMagnificationHelper__) */
