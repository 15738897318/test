//
//  MHREulerian.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/25/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__MHREulerian__
#define __MisfitHeartRate__MHREulerian__

#include <string>
#include <vector>
#include "MHRImage.h"
//#import <opencv2/highgui/ios.h>
//#import <opencv2/highgui/cap_ios.h>


namespace cv {
	// Spatial Filtering: Gaussian blur and down sample
	// Temporal Filtering: Ideal bandpass
	void amplifySpatialGdownTemporalIdeal(String vidFile, String outDir,
										  double alpha, int level,
										  double freqBandLowEnd, double freqBandHighEnd,
										  double samplingRate, double chromAttenuation);
    
	// run Eulerian
	void runEulerian(String srcDir, String fileName, String fileTemplate, String resultsDir);
}


#endif /* defined(__MisfitHeartRate__MHREulerian__) */