//
//  run_algorithms.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__run_algorithms__
#define __MisfitHeartRate__run_algorithms__

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "eulerian.h"
#include "temporal_mean_calc.h"
#include "globals.h"

using namespace cv;
using namespace std;

namespace MHR
{
    hrResult run_algorithms(const String &srcDir, const String &fileName, const String &outDir, hrResult &currHrResult);
}

#endif /* defined(__MisfitHeartRate__run_algorithms__) */
