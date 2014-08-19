//
//  hr_signal_calc.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hr_signal_calc__
#define __MisfitHeartRate__hr_signal_calc__

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
#include "hb_counter_pda.h"
#include "hb_counter_autocorr.h"
#include "hr_calculator.h"


namespace MHR {
    struct hrResult
    {
        double autocorr;        // avg_hr_autocorr
        double pda;             // avg_hr_pda
        hrResult();
        hrResult(double autocorr, double pda);
        
        void operator = (const hrResult &other);
    };
    
    
    hrResult hr_signal_calc(vector<double> &temporal_mean, int firstSample, int window_size,
                            double frameRate, double overlap_ratio,
                            double max_bpm, double threshold_fraction);
}

#endif /* defined(__MisfitHeartRate__hr_signal_calc__) */
