//
//  eulerian.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/3/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__eulerian__
#define __MisfitHeartRate__eulerian__

#include <string>
#include <vector>
#include "config.h"
#include "image.h"
//#import <opencv2/highgui/ios.h>
//#import <opencv2/highgui/cap_ios.h>


namespace MHR {
	// Spatial Filtering: Gaussian blur and down sample
	// Temporal Filtering: Ideal bandpass
	vector<Mat> amplifySpatialGdownTemporalIdeal(String vidFile, String outDir,
										  double alpha, int level,
										  double freqBandLowEnd, double freqBandHighEnd,
										  double samplingRate, double chromAttenuation);
    
	// run Eulerian
	vector<Mat> runEulerian(String srcDir, String fileName, String fileTemplate, String resultsDir);
}

#endif /* defined(__MisfitHeartRate__eulerian__) */
