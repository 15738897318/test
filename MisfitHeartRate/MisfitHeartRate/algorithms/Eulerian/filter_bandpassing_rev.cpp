//
//  filter_bandpassing.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/8/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "filter_bandpassing.h"


namespace MHR {
    void filter_bandpassing(const Mat &GdownStack, Mat &filteredStack)
    {
        int nChannels = GdownStack.channels();
        
        int firstFramesRemove = _eulerianTemporalFilterKernel_size/2;
        
        int filteredSize[3] = {GdownStack.size.p[0] - firstFramesRemove, GdownStack.size.p[1], GdownStack.size.p[2]};
        
        filteredStack = Mat(3, filteredSize, CV_64FC(nChannels), CvScalar(0));
        Mat kernel = arrayToMat(_eulerianTemporalFilterKernel, 1, _eulerianTemporalFilterKernel_size);
        
        Mat tmp = Mat::zeros(1, GdownStack.size.p[0], CV_64FC(nChannels));
        for (int x = 0; x < GdownStack.size.p[1]; ++x)
            for (int y = 0; y < GdownStack.size.p[2]; ++y) {
                for (int t = 0; t < GdownStack.size.p[0]; ++t)
                    if (THREE_CHAN_MODE | DEBUG_MODE)
                    	tmp.at<Vec3d>(0, t) = GdownStack.at<Vec3d>(t, x, y);
                    else
                    	tmp.at<double>(0, t) = GdownStack.at<double>(t, x, y);
                
                filter2D(tmp, tmp, -1, kernel);
                
                for (int t = firstFramesRemove; t < GdownStack.size.p[0]; ++t)
                    if (THREE_CHAN_MODE | DEBUG_MODE)
                    	filteredStack.at<Vec3d>(t - firstFramesRemove, x, y) = tmp.at<Vec3d>(0, t);
                    else
                    	filteredStack.at<double>(t - firstFramesRemove, x, y) = tmp.at<double>(0, t);
            }
    }
}