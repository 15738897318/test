//
//  frames2signal.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "frames2signal.h"


namespace MHR {
    vector<double> frames2signal(const vector<Mat>& monoframes, String conversion_method,
                                 double fr, double cutoff_freq,
                                 double &lower_range, double &upper_range, bool isCalcMode,
                                 vector<Mat>& debug_monoframes)
    {
        clock_t t1 = clock();
        
        //=== Block 1. Convert the frame stream into a 1-D signal
        
        vector<double> temporal_mean;
        int height = monoframes[0].size.p[0];
        int width = monoframes[0].size.p[1];
        int total_frames = (int)monoframes.size();
        
        if(conversion_method == "simple-mean"){

            double size = height * width;
            for(int i=0; i<total_frames; ++i){
                double sum = 0;
                for(int x=0; x<height; ++x)
                    for(int y=0; y<width; ++y)
                        sum+=monoframes[i].at<double>(x,y);
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
                        sum+=monoframes[i].at<double>(x,y);
                temporal_mean.push_back(sum/size);
            }
            
        }else if(conversion_method == "mode-balance"){
            
            // Selection parameters
//            double training_time = _training_time_end - _training_time_start;
            double lower_pct_range = _pct_reach_below_mode;
            double upper_pct_range = _pct_reach_above_mode;
            
            int first_tranning_frames_start = min( (int)round(fr * _training_time_start), total_frames );
            int first_tranning_frames_end = min( (int)round(fr * _training_time_end), total_frames );
//            int first_tranning_frames = min( (int)round(fr * training_time), total_frames );

            // this arr stores values of pixels from first trainning frames
            vector<double> arr;
            for(int i = first_tranning_frames_start; i < first_tranning_frames_end; ++i)
                for(int x=0; x<height; ++x)
                    for(int y=0; y<width; ++y)
                        arr.push_back(monoframes[i].at<double>(x,y));
            
            if (isCalcMode)
            {
                //find the mode
                vector<double> centres;
                vector<int> counts;
                
                hist(arr, _number_of_bins, counts, centres);
                
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
                lower_range = prctile(arr, percentile_lower_range);
                upper_range = prctile(arr, percentile_upper_range);
            }
            
            printf("lower_range = %lf, upper_range = %lf\n", lower_range, upper_range);
            
            //now calc the avg of each frame while inogre the values outside the range
            double size = height * width;
            
            //this is the debug vector<Mat>
            debug_monoframes = vector<Mat>(monoframes);
            for(int i=0; i<total_frames; ++i){
                double sum = 0;
                int cnt = 0; //number of not-NaN-pixels
                for(int x=0; x<height; ++x)
                    for(int y=0; y<width; ++y){
                        double val=monoframes[i].at<double>(x,y);
                        if(val<lower_range - 1e-9 || val>upper_range + 1e-9){
                            val=0;
                            debug_monoframes[i].at<double>(x,y)=NaN; //set 0 for rejected pixels
                        }else ++cnt;
                        sum+=val;
                    }
                
                if(cnt==0) //push NaN for all-NaN-frames
                    temporal_mean.push_back(NaN);
                else
                    temporal_mean.push_back(sum/size);
            }
            
        }// end of mode-balance
        
        clock_t t2 = clock();
        printf("frames2signal() - Block 1 runtime = %f\n", ((float)t2 - (float)t1)/CLOCKS_PER_SEC);
        
        //=== Block 2. Low-pass-filter the signal stream to remove unwanted noises
        vector<double> temporal_mean_filt = low_pass_filter(temporal_mean);
        return temporal_mean_filt;
    }
}