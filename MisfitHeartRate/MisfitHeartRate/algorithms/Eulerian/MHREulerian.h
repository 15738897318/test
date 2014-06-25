//
//  MHREulerian.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/25/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__MHREulerian__
#define __MisfitHeartRate__MHREulerian__

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "MHRImage.h"
//#import <opencv2/highgui/ios.h>
//#import <opencv2/highgui/cap_ios.h>

namespace cv {
    
    void amplifySpatialGdownTemporalIdeal(String inFile, String resultDir,
                                          double alpha, int level,
                                          double freq_band_low_end, double freq_band_high_end,
                                          double samplingRate, double chromAttenuation);
    
    void runEulerian();
    
}


#endif /* defined(__MisfitHeartRate__MHREulerian__) */