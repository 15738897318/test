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
    void ideal_bandpassing(const Mat &src, Mat &dst, double wl, double wh, double samplingRate) {
        src.convertTo(dst, CV_32F);
        int nTime = dst.size.p[0];
        int nRow = dst.size.p[1];
        int nCol = dst.size.p[2];
        
        int f1 = ceil(wl * nTime/samplingRate);
        int f2 = floor(wh * nTime/samplingRate);
        int ind1 = 2*f1, ind2 = 2*f2 - 1;
        
        // FFT
        Mat dft_out = Mat::zeros(nRow, nTime, CV_32F), tmp_dft;
        for (int channel = 0; channel < _number_of_channels; ++channel) {
            for (int k = 0; k < nCol; ++k) {
                for (int i = 0; i < nTime; ++i)
                    for (int j = 0; j < nRow; ++j)
                        dft_out.at<float>(j, i) = dst.at<Vec3f>(i, j, k)[channel];
                dft(dft_out, tmp_dft, DFT_ROWS);
                // masking
                for (int j = 0; j < nRow; ++j) {
                    for (int i = 0; i <= ind1; ++i)
                        dft_out.at<float>(j, i) = 0;
                    for (int i = ind2; i < nTime; ++i)
                        dft_out.at<float>(j, i) = 0;
                }
                // output
                dft(dft_out, dft_out, DFT_ROWS + DFT_INVERSE + DFT_REAL_OUTPUT);
                for (int i = 0; i < nTime; ++i)
                    for (int j = 0; j < nRow; ++j)
                        dst.at<Vec3f>(i, j, k)[channel] = dft_out.at<float>(j, i);
            }
        }
        
//        printf("ind1 = %i, ind2 = %i\n", ind1, ind2);
//        printf("dft_out: rows = %i, cols = %i\n", dft_out.rows, dft_out.cols);
//        for (int i = 0; i < nTime; ++i) {
//            printf("%i --->     ", i);
//            for (int j = 0; j < nRow; ++j)
//                printf("%lf, ", dft_out.at<float>(j, i));
//            printf("\n");
//        }
//        printf("\n\n\n\n");
        
        Mat tmp;
        dst.convertTo(tmp, CV_64FC3);
        dst = tmp.clone();
    }
}