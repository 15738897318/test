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
    void ideal_bandpassing(const vector<Mat> &src, vector<Mat> &dst, mTYPE wl, mTYPE wh, mTYPE samplingRate) {
//        src: T*M*N*C;
//        new src: vector<M*N*C>
        
        // extract src info
        int nTime = (int)src.size();
        int nRow = src[0].rows;
        int nCol = src[0].cols;
        // copy and convert data from src to dst (CV_32FC3)
        Mat tmp;
        dst.clear();
        for (int i = 0; i < nTime; ++i)
        {
            src[i].convertTo(tmp, CV_32FC3);
            dst.push_back(tmp.clone());
        }
        // masking indexes
        int f1 = ceil(wl * nTime/samplingRate);
        int f2 = floor(wh * nTime/samplingRate);
        int ind1 = 2*f1, ind2 = 2*f2 - 1;
        printf("ind1 = %d, ind2 = %d, nTime = %d\n", ind1, ind2, nTime);
        
        // FFT
        Mat dft_out = Mat::zeros(nRow, nTime, CV_32F);
        for (int channel = 0; channel < _number_of_channels; ++channel) {
            for (int col = 0; col < nCol; ++col) {
                for (int time = 0; time < nTime; ++time)
                    for (int j = 0; j < nRow; ++j)
                        dft_out.at<float>(j, time) = dst[time].at<Vec3f>(j, col)[channel];
                dft(dft_out, dft_out, DFT_ROWS);
                // masking
                for (int j = 0; j < nRow; ++j) {
                    for (int time = 0; time <= ind1; ++time)
                    for (int i = 0; i <= ind1; ++i)
                        dft_out.at<float>(j, i) = 0;
                    for (int i = ind2; i < nTime; ++i)
                        dft_out.at<float>(j, i) = 0;
                }
                // output
                dft(dft_out, dft_out, DFT_ROWS + DFT_INVERSE + DFT_REAL_OUTPUT + DFT_SCALE);
                for (int i = 0; i < nTime; ++i)
                    for (int j = 0; j < nRow; ++j)
                        dst[i].at<Vec3f>(j, col)[channel] = dft_out.at<float>(j, i);
            }
        }
        
        for (int i = 0; i < nTime; ++i)
            dst[i].convertTo(dst[i], mCV_FC3);
    }
}