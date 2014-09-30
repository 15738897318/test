//
//  run_algorithms.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/4/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__run_algorithms__
#define __MisfitHeartRate__run_algorithms__

#include "temporal_mean_calc.h"
#include "SelfCorrPeakHRCounter.h"
#include "globals.h"

using namespace cv;
using namespace std;

namespace MHR {
    hrResult run_algorithms(const String &srcDir, const String &outDir, hrResult &currHrResult);
}

#endif /* defined(__MisfitHeartRate__run_algorithms__) */
