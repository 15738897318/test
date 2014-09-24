//
//  hr_signal_calc.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hr_signal_calc__
#define __MisfitHeartRate__hr_signal_calc__

#include "hb_counter_pda.h"
#include "hb_counter_autocorr.h"
#include "hr_calculator.h"

namespace MHR {
    /**
     * Return the average heart-rate calculated by autocorr algorithm and pda algorithm. \n
     * \param: see hb_counter_pda() or hb_counter_autocorr().
     */
    hrResult hr_signal_calc(vector<double> &temporal_mean, int firstSample, int window_size,
                            double frameRate, double overlap_ratio,
                            double max_bpm, double threshold_fraction);
}

#endif /* defined(__MisfitHeartRate__hr_signal_calc__) */
