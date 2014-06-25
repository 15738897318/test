//
//  ImageUtilities.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/24/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "ImageUtilities.h"


namespace cv {
    
    // convert a RGB Mat to a TSL Mat
    Mat rgb2tsl(const Mat& srcRGBmap)
    {
        int nRow = srcRGBmap.rows;
        int nCol = srcRGBmap.cols;
        Mat rbgmap(nRow, nCol, CV_32FC3, srcRGBmap.data);
        
        //        r_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 1), sum(rgbmap, 3)), 1/3);
        //        r_primes(isnan(r_primes)) = -1/3;
        Mat r_primes = Mat::zeros(nRow, nCol, CV_32FC1);
        divide(cloneWithChannel(rbgmap, 0), sumChannels(rbgmap), r_primes);
        subtract(r_primes, Mat(nRow, nCol, CV_32FC1, 1.0/3.0), r_primes);
        
        //        g_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 2), sum(rgbmap, 3)), 1/3);
        //        g_primes(isnan(g_primes)) = -1/3;
        Mat g_primes = Mat::zeros(nRow, nCol, CV_32FC1);
        divide(cloneWithChannel(rbgmap, 1), sumChannels(rbgmap), g_primes);
        subtract(r_primes, Mat(nRow, nCol, CV_32FC1, 1.0/3.0), g_primes);
        
        //        temp1 = zeros(size(g_primes));
        //        temp1(bsxfun(@gt, g_primes, 0)) = 1/4;
        //        temp1(bsxfun(@lt, g_primes, 0)) = 3/4;
        //        temp2 = ones(size(g_primes));
        //        temp2(bsxfun(@eq, g_primes, 0)) = 0;
        Mat temp1 = Mat::zeros(nRow, nCol, CV_32FC1);
        Mat temp2 = Mat::ones(nRow, nCol, CV_32FC1);
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
                if (g_primes.at<double>(i, j) > 0)
                {
                    temp1.at<double>(i, j) = 1.0/4.0;
                }
                else if (g_primes.at<double>(i, j) > 0)
                {
                    temp1.at<double>(i, j) = 3.0/4.0;
                }
                else
                {
                    temp2.at<double>(i, j) = 0;
                }
        
        Mat tslmap = Mat::zeros(nRow, nCol, CV_32FC3);
        //        tslmap(:, :, 1) = 1 / (2 * pi) * bsxfun(@atan2, r_primes, g_primes) .* temp2 + temp1;
        Mat tmp0 = atan2Mat(r_primes, g_primes);
        multiply(tmp0, Mat(nRow, nCol, CV_32FC1, 1/(2*M_PI)), tmp0);
        multiply(tmp0, temp2, tmp0);
        add(tmp0, temp1, tmp0);
        //        tslmap(:, :, 2) = bsxfun(@power, (9/5 * (r_primes.^2 + g_primes.^2)), 1/2);
        Mat tmp1 = powMat(r_primes, 2);
        add(tmp1, powMat(g_primes, 2), tmp1);
        multiply(tmp1, Mat(nRow, nCol, 9.0/5.0), tmp1);
        pow(tmp1, 0.5, tmp1);
        //        tslmap(:, :, 3) = 0.299 * rgbmap(:, :, 1) + 0.587 * rgbmap(:, :, 2) + 0.114 * rgbmap(:, :, 3);
        Mat tmp2 = add(multiply(cloneWithChannel(rbgmap, 0), 0.299),
                       multiply(cloneWithChannel(rbgmap, 1), 0.587));
        add(tmp2, multiply(cloneWithChannel(rbgmap, 2), 0.114), tmp2);
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
            {
                tslmap.at<Vec3d>(i, j)[0] = tmp0.at<double>(i, j);
                tslmap.at<Vec3d>(i, j)[1] = tmp1.at<double>(i, j);
                tslmap.at<Vec3d>(i, j)[2] = tmp2.at<double>(i, j);
            }
        return tslmap;
    }

}