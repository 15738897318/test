//
//  hr_calculator.cpp
//  MisfitHeartRate
//
//  Created by Tuan-Anh Tran on 7/14/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hr_calculator.h"


namespace MHR {
	// return a vector of integer from a to b with specific step
	void hr_calculator(const vector<int> &heartBeatPositions, double frameRate, vector<double> &ans) {
		//Calculate the instantaneous heart-rates
		vector<double> heartRate_inst;
		for (int i = 1, sz = (int)heartBeatPositions.size(); i < sz; ++i)
			heartRate_inst.push_back( 1.0 / (heartBeatPositions[i] - heartBeatPositions[i-1]) );
        
		//Find the mode
		vector<double> centres;
		vector<int> counts;
        
		hist(heartRate_inst, _number_of_bins_heartRate, counts, centres);
        
		int argmax = 0;
		for(int i = 0, sz = (int)counts.size(); i < sz; ++i)
			if(counts[i] > counts[argmax]) argmax = i;
		double centre_mode = centres[argmax];
        
		//Create a convolution kernel from the found frequency
		vector<double> kernel;
		gaussianFilter(cvCeil(2.0 / centre_mode), 1.0 / (4.0 * centre_mode), kernel);
		double threshold = 2.0 * kernel[cvCeil(1.0 / (4.0 * centre_mode)) - 1];
        
		//Create a heart-beat count signal
		vector<double> count_signal;
		int temp = heartBeatPositions[heartBeatPositions.size() - 1] - heartBeatPositions[0] + 1;
		for (int i = 0; i < temp; ++i) {
			count_signal.push_back(0);
		}
		for (int i = 0, sz = (int)heartBeatPositions.size(); i < sz; ++i) {
			temp = heartBeatPositions[i] - heartBeatPositions[0];
			count_signal[temp] = 1;
		}
        
		//Convolve the count_signal with the kernel to generate a score_signal
        vector<double> score_signal = corr_linear(count_signal, kernel);
//		filter2D(count_signal, score_signal, -1, kernel, Point(-1,-1), 0, BORDER_CONSTANT);
		for (int i = 0, sz = (int)score_signal.size(); i < sz; ++i)
			score_signal[i] = -score_signal[i];
        
		//Decide if the any beats are missing and fill them in if need be
		vector<double> min_peak_strengths;
		vector<int> min_peak_locs;
		findpeaks(score_signal, 0, 0, min_peak_strengths, min_peak_locs);
		for (int i = 0, sz = (int)min_peak_strengths.size(); i < sz; ++i)
			min_peak_strengths[i] = -min_peak_strengths[i];
        
        for (int i = 0, sz = (int)score_signal.size(); i < sz; ++i)
			score_signal[i] = -score_signal[i];
        
		double factor = 1.5;
		threshold *= factor;
		for (int i = 0, len = (int)min_peak_locs.size(); i < len; ++i) {
			if (min_peak_strengths[i] < threshold) {
				count_signal[min_peak_locs[i]] = -1;
			}
		}
        
		//Calculate the heart-rate from the new beat count
		ans.clear();
        
		ans.push_back(0);
		int len = (int)count_signal.size();
		for (int i = 0; i < len; ++i)
			ans[0] += abs(count_signal[i]);
		ans[0] /= (double(len) + 1.0/centre_mode);
        ans[0] *= frameRate * 60;
        
		ans.push_back(centre_mode * frameRate * 60);
	}
    
    
	// Generate a vector of Gaussian values of a desired length and properties
	void gaussianFilter(int length, double sigma, vector<double> &ans) {
		ans.clear();
        Mat kernel = getGaussianKernel(length, sigma, CV_64F);
        for (int i = 0; i < kernel.size.p[0]; ++i)
            for (int j = 0; j < kernel.size.p[1]; ++j)
                ans.push_back(kernel.at<double>(i, j));
        double max_value = *max_element(ans.begin(), ans.end());
        for (int i = 0, sz = (int)ans.size(); i < sz; ++i)
            ans[i] /= max_value;
	}
}