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
     * \return the mean value of a double vector
     */
    double mean(const vector<double> &a);

    /*!
     * \ref: http://www.cs.cornell.edu/courses/CS1114/2013sp/sections/S06_convolution.pdf
     * \return 1D convolution operation of 2 vectors signal and kernel
     * \param subtractMean if is true, then before all calculations,
     * each elements of the signal vector will be subtracted by mean(\a signal),
     * and each elements of the kernel vector will be subtracted by mean(\a kernel).
     */
    void corr_linear(std::vector<double> &signal, std::vector<double> &kernel, std::vector<double> &result, bool subtractMean = true);

    /*!
     remove all identical item in the arr, two items are equal if the second value (type int) of them are equal.\n
     all identical item will be removed just left the first appearing value.\n
     the order will be reserve.
    */
    vector<pair<double,int>> unique_stable(const vector<pair<double,int>> &arr);

    
    //!
    //! \ref: http://www.mathworks.com/help/matlab/ref/hist.html
    //! \return \a counts: number of elements in each bin,
    //! \return \a centers: the center value of each bin
    //! get the histogram of arr's value, the range from min value to max value of the arr will be divided into \a nbins bins,
    //! each bin will have a centres point and a count value denoting number of value in the array belong to that bin's range
    //!
    void hist(const vector<double> &arr, int nbins, vector<int> &counts, vector<double> &centers);


    //!
    //! get the invert percentile of arr with value x.
    //! \return the percent of number of values in arr that smaller or equal x.
    //!
    double invprctile(const vector<double> &arr, double x);


    //!
    //! get the percentile of arr with a percent value.
    //!
    double prctile(vector<double> arr, double percent);


    /**
     * filter function for frames2signal function, apply low pass filter on vector \a arr.
     * \ref: http://en.wikipedia.org/wiki/Low-pass_filter
     */
    vector<double> low_pass_filter(vector<double> arr);
    
    
    /**
     * Generate a vector of Gaussian values of a desired length and properties\n
     * \ref: http://www.mathworks.com/help/images/ref/fspecial.html \n
     * \ref: http://docs.opencv.org/modules/imgproc/doc/filtering.html#Mat%20getGaussianKernel%28int%20ksize,%20double%20sigma,%20int%20ktype%29 \n
     */
    void gaussianFilter(int length, double sigma, vector<double> &ans);


    /**
     * \return 100*|a-b|/b
     */
    double diff_percent(double a, double b);
}

#endif /* defined(__MisfitHeartRate__matlab__) */
