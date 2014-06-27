//
//  MHRMath.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/25/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "MHRMath.h"
#include <complex>

using namespace std;


namespace cv {
    // return Discrete Fourier Transform of a 2-2 Mat by dimension
	Mat fft(const Mat &src, int dimension) {
		Mat ans = Mat_<complex<double> > (src.rows, src.cols);
		if (dimension == 0) {
			Mat tmp = Mat_<complex<double>>(1, src.rows);
			for (int i = 0; i < src.cols; ++i) {
				for (int j = 0; j < src.rows; ++j)
					tmp.at<complex<double>>(1, j) = (src.at<double>(j, i), 0);
				dft(tmp, tmp);
				for (int j = 0; j < src.rows; ++j)
					ans.at<complex<double>>(j, i) = tmp.at<complex<double>>(1, j);
			}
		}
		else {
			Mat tmp = Mat_<complex<double>>(1, src.cols);
			for (int i = 0; i < src.rows; ++i) {
				for (int j = 0; j < src.cols; ++j)
					tmp.at<complex<double>>(1, j) = (src.at<double>(i, j), 0);
				dft(tmp, tmp);
				for (int j = 0; j < src.cols; ++j)
					ans.at<complex<double>>(i, j) = tmp.at<complex<double>>(1, j), 0;
			}
		}
		return ans;
	}
    
    
    // return Inverse Discrete Fourier Transform of a 2-2 Mat by dimension
	Mat ifft(const Mat &src, int dimension) {
		Mat ans = Mat_<complex<double> > (src.rows, src.cols);
		if (dimension == 0) {
			Mat tmp = Mat_<complex<double>>(1, src.rows);
			for (int i = 0; i < src.cols; ++i) {
				for (int j = 0; j < src.rows; ++j)
					tmp.at<complex<double>>(1, j) = (src.at<double>(j, i), 0);
				idft(tmp, tmp);
				for (int j = 0; j < src.rows; ++j)
					ans.at<complex<double>>(j, i) = tmp.at<complex<double>>(1, j);
			}
		}
		else {
			Mat tmp = Mat_<complex<double>>(1, src.cols);
			for (int i = 0; i < src.rows; ++i) {
				for (int j = 0; j < src.cols; ++j)
					tmp.at<complex<double>>(1, j) = (src.at<double>(i, j), 0);
				idft(tmp, tmp);
				for (int j = 0; j < src.cols; ++j)
					ans.at<complex<double>>(i, j) = tmp.at<complex<double>>(1, j), 0;
			}
		}
		return ans;
	}
    
    
    // convert a frame to signal
    void frames2signal(Mat monoframes, String conversion_method, double frameRate, double cutoff_freq,
                       double &temporal_mean, Mat &debug_frames2signal)
    {
        
    }
};
