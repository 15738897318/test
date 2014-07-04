//
//  config.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef MisfitHeartRate_config_h
#define MisfitHeartRate_config_h

const double NaN = -1e9;

/*--------------for run_hr()--------------*/
const int _channels_to_process = 1;

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
