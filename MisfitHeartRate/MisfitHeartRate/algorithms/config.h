//
//  config.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef MisfitHeartRate_config_h
#define MisfitHeartRate_config_h

using namespace cv;


const double NaN = -1e9;


/*--------------for run_eulerian()--------------*/
double _eulerian_alpha = 30;          // Eulerian magnifier
double _eulerian_pyrLevel = 6;        // Standard: 4, but updated by the real frame size
double _eulerian_minHR = 30;          // BPM Standard: 50
double _eulerian_maxHR = 240;         // BPM Standard: 90
double _eulerian_frameRate = 30;      // Standard: 30, but updated by the real frame-rate
double _eulerian_chromaMagnifier = 1; // Standard: 1


/*--------------for run_hr()--------------*/
const double _run_hr_window_size_in_sec = 10;
const double _run_hr_overlap_ratio = 0;
const double _run_hr_max_bpm = 200;             // BPM
const double _run_hr_cutoff_freq = 5;           // Hz
const double _run_hr_time_lag = 3;              // seconds
const int _run_hr_channels_to_process = 1;
const String _run_hr_colourspace = "tsl";


/*--------------for frames2signal()--------------*/

//trimmed-mean const
const int _trimmed_size = 30;

//mode-balance const
const double _lower_pct_range = 45.0;
const double _upper_pct_range = 45.0;
const double _training_time=0.5;


/*--------------for matlab functions--------------*/

//kernel for low_pass_filter(), used in frames2sinal()
const double _filtArray[] = {
    -0.0265, -0.0076, 0.0217, 0.0580, 0.0956,
    0.1285, 0.1509, 0.1589, 0.1509, 0.1285,
    0.0956, 0.0580, 0.0217, -0.0076, -0.0265   };

#endif
