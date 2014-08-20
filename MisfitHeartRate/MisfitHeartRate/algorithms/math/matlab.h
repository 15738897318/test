//
//  matlab.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__matlab__
#define __MisfitHeartRate__matlab__

#include <set>
#include "matrix.h"

using namespace std;
using namespace cv;


namespace MHR {
    /**
     * return the mean value of a double vector
     */
    double mean(const vector<double> &a);


    /**
     * find peaks in vector <segment>,
     * with <minPeakDistance>: the minimum distance between 2 adjacent peaks,
     * and <threhold>: the minimum height-different between a peak and its 2 adjacent points.
     * return 2 vectors: <max_peak_strengths> and <max_peak_locs> (locations).
     * complexity: O(n^2) with n = number of peaks
     */
    void findpeaks(const vector<double> &segment, double minPeakDistance, double threshold,
                   vector<double> &max_peak_strengths, vector<int> &max_peak_locs);


    /**
     *return a vector in which all elements (from the original vector) have unique second values.
     */
    vector<pair<double,int>> unique_stable(const vector<pair<double,int>> &arr);


    /**
     * return 1D convolution operation of 2 vectors signal and kernel
     * ref: http://www.cs.cornell.edu/courses/CS1114/2013sp/sections/S06_convolution.pdf
     * if subtractMean == true, then before all calculations,
     * each elements of the signal vector will be subtracted by mean(signal),
     * and each elements of the kernel vector will be subtracted by mean(kernel).
     */
    vector<double> corr_linear(vector<double> signal, vector<double> kernel, bool subtractMean = true);

    
    /**
     * sorts all elements of <arr> vector number of bins specified by <nbins>,
     * return <counts>: number of elements in each bin,
     * and <centers>: the center value of each bin
     * ref: http://www.mathworks.com/help/matlab/ref/hist.html
     */
    void hist(const vector<double> &arr, int nbins, vector<int> &counts, vector<double> &centers);


    // invprctile
    double invprctile(const vector<double> &arr, double x);


    // prctile
    double prctile(vector<double> arr, double percent);


    /**
     * filter function for frames2signal function,
     * apply low pass filter on vector <arr>.
     * ref: http://en.wikipedia.org/wiki/Low-pass_filter
     */
    vector<double> low_pass_filter(vector<double> arr);
    
    
    /**
     * Generate a vector of Gaussian values of a desired length and properties
     * ref: http://www.mathworks.com/help/images/ref/fspecial.html
     * ref: http://docs.opencv.org/modules/imgproc/doc/filtering.html#Mat%20getGaussianKernel%28int%20ksize,%20double%20sigma,%20int%20ktype%29
     */
    void gaussianFilter(int length, double sigma, vector<double> &ans);


    /**
     * return 100*|a-b|/b
     */
    double diff_percent(double a, double b);
}

#endif /* defined(__MisfitHeartRate__matlab__) */
