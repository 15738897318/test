//
//  frames2signal.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "frames2signal.h"

vector<double> frames2signal(const Mat& monoframes, String conversion_method, double fr, double cutoff_freq, Mat &debug_monoframes)
{
    //=== Block 1. Convert the frame stream into a 1-D signal
    
    vector<double> temporal_mean;
    int height = monoframes.size.p[0];
    int width = monoframes.size.p[1];
    int total_frames = monoframes.size.p[2];
    
    if(conversion_method == "simple-mean"){
        
        double size = height * width;
        for(int i=0; i<total_frames; ++i){
            double sum = 0;
            for(int x=0; x<height; ++x)
                for(int y=0; y<width; ++y)
                    sum+=monoframes.at<double>(x,y,i);
            temporal_mean.push_back(sum/size);
        }
        
    }else if(conversion_method == "trimmed-mean"){
        
        //Set the trimmed size here
        int trimmed_size = _trimmed_size;
        
        double size = (height - trimmed_size * 2) * (width - trimmed_size * 2);
        for(int i=0; i<total_frames; ++i){
            double sum = 0;
            for(int x=trimmed_size; x<height-trimmed_size; ++x)
                for(int y=trimmed_size; y<width-trimmed_size; ++y)
                    sum+=monoframes.at<double>(x,y,i);
            temporal_mean.push_back(sum/size);
        }
        
    }else if(conversion_method == "mode-balance"){
        
        // Selection parameters
        double training_time = _training_time;
        double lower_pct_range = _lower_pct_range;
        double upper_pct_range = _upper_pct_range;
        
        int first_tranning_frames = min( (int) round(fr * training_time), total_frames );
        int nbins = (int) (50);
        
        // this arr stores values of pixels from first trainning frames
        vector<double> arr;
        for(int i=0; i<first_tranning_frames; ++i)
            for(int x=0; x<height; ++x)
                for(int y=0; y<width; ++y)
                    arr.push_back(monoframes.at<double>(x,y,i));
        
        //find the mode
        vector<double> centres;
        vector<int> counts;
        
        hist(arr, nbins, counts, centres);
        
        int argmax=0;
        for(int i=0; i<(int)counts.size(); ++i) if(counts[i]>counts[argmax]) argmax = i;
        double centre_mode = centres[argmax];
        
        // find the percentile range centred on the mode
        double percentile_of_centre_mode = invprctile(arr, centre_mode);
        double percentile_lower_range = max(0.0, percentile_of_centre_mode - lower_pct_range);
        double percentile_upper_range = min(100.0, percentile_of_centre_mode + upper_pct_range);
        // correct the percentile range for the boundary cases
        if(percentile_upper_range == 100)
            percentile_lower_range = 100 - (lower_pct_range + upper_pct_range);
        if(percentile_lower_range == 0)
            percentile_upper_range = (lower_pct_range + upper_pct_range);
        
        //convert the percentile range into pixel-value range
        double lower_range = prctile(arr, percentile_lower_range);
        double upper_range = prctile(arr, percentile_upper_range);
        
        //now calc the avg of each frame while inogre the values outside the range
        double size = height * width;
        
        debug_monoframes = monoframes.clone(); //this is the debug Mat
        
        for(int i=0; i<total_frames; ++i){
            double sum = 0;
            int cnt = 0; //number of not-NaN-pixels
            for(int x=0; x<height; ++x)
                for(int y=0; y<width; ++y){
                    double val=monoframes.at<double>(x,y,i);
                    if(val<lower_range - 1e-9 || val>upper_range + 1e-9){
                        val=0;
                        debug_monoframes.at<double>(x,y,i)=NaN; //set 0 for rejected pixels
                    }else ++cnt;
                    sum+=val;
                }
            
            if(cnt==0) //push NaN for all-NaN-frames
                temporal_mean.push_back(NaN);
            else
                temporal_mean.push_back(sum/size);
        }
        
    }// end of mode-balance
    
    //=== Block 2. Low-pass-filter the signal stream to remove unwanted noises
    vector<double> temporal_mean_filt = low_pass_filter(temporal_mean);
    return temporal_mean_filt;
}


