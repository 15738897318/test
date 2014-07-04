//
//  heartRate_calc.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__heartRate_calc__
#define __MisfitHeartRate__heartRate_calc__

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "image.h"
#include "matrix.h"
#include "frames2signal.h"
#include "hr_calc_pda.h"
#include "hr_calc_autocorr.h"


namespace MHR {
    struct hrResult
    {
        double autocorr;        // avg_hr_autocorr
        double pda;             // avg_hr_pda
        
        hrResult(double autocorr, double pda);
    };
    
    
    hrResult heartRate_calc(vector<Mat> &vid, double window_size_in_sec, double overlap_ratio,
                                  double max_bpm, double cutoff_freq, int colour_channel,
                                  String colourspace, double time_lag);
}

#endif /* defined(__MisfitHeartRate__heartRate_calc__) */
