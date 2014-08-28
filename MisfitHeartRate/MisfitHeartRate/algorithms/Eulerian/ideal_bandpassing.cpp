//
//  ideal_bandpassing.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/8/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "ideal_bandpassing.h"


namespace MHR {
    // Apply ideal band pass filter on SRC
    // WL: lower cutoff frequency of ideal band pass filter
    // WH: higher cutoff frequency of ideal band pass filter
    // SAMPLINGRATE: sampling rate of SRC
    void ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, double wl, double wh, double samplingRate) {
//        src: T*M*N*C;
//        new src: vector<M*N*C>
        
        // extract src info
        int nTime = (int)src.size();
        int nRow = src[0].rows;
        int nCol = src[0].cols;
        int nChannel = (_THREE_CHAN_MODE > 0) ? _number_of_channels : 1;
        
        // copy and convert data from src to dst (CV_32FC(nChannels))
        Mat tmp;
        dst.clear();
        for (int i = 0; i < nTime; ++i)
        {
        	if (_THREE_CHAN_MODE)
            	src[i].convertTo(tmp, CV_32FC3);
            else
            	src[i].convertTo(tmp, CV_32F);
            dst.push_back(tmp.clone());
        }
        // masking indexes
        int f1 = ceil(wl * nTime/samplingRate);
        int f2 = floor(wh * nTime/samplingRate);
        int ind1 = 2*f1, ind2 = 2*f2 - 1;
        
        if (_DEBUG_MODE)
            printf("ind1 = %d, ind2 = %d, nTime = %d\n", ind1, ind2, nTime);
        
        // FFT
        Mat dft_out = Mat::zeros(nRow, nTime, CV_32F);
        
        for (int channel = 0; channel < nChannel; ++channel) {
            for (int col = 0; col < nCol; ++col) {
                for (int time = 0; time < nTime; ++time)
                    for (int row = 0; row < nRow; ++row)
                        if (_THREE_CHAN_MODE)
                            dft_out.at<float>(row, time) = dst[time].at<Vec3f>(row, col)[channel];
                        else
                            dft_out.at<float>(row, time) = dst[time].at<float>(row, col);
                
                dft(dft_out, dft_out, DFT_ROWS);
                // masking
                for (int row = 0; row < nRow; ++row) {
                    for (int time = 0; time <= ind1; ++time)
                        dft_out.at<float>(row, time) = 0;
                    for (int time = ind2; time < nTime; ++time)
                        dft_out.at<float>(row, time) = 0;
                }
                // output
                dft(dft_out, dft_out, DFT_ROWS + DFT_INVERSE + DFT_REAL_OUTPUT + DFT_SCALE);
                for (int time = 0; time < nTime; ++time)
                    for (int row = 0; row < nRow; ++row)
                        if (_THREE_CHAN_MODE)
                            dst[time].at<Vec3f>(row, col)[channel] = dft_out.at<float>(row, time);
                        else
                            dst[time].at<float>(row, col) = dft_out.at<float>(row, time);
            }
        }
        
        
        for (int i = 0; i < nTime; ++i)
        	if (_THREE_CHAN_MODE)
            	dst[i].convertTo(dst[i], CV_64FC3);
            else
            	dst[i].convertTo(dst[i], CV_64F);
        
        if (_DEBUG_MODE)
            frameChannelToFile(dst[0], _outputPath + "2_dst[0]_ideal_bandpassing.txt", _channels_to_process);
    }
}