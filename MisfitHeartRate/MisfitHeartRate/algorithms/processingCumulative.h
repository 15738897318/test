//
//  processingCumulative.h
//  Pulsar
//
//  Created by HaiPhan on 8/19/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#ifndef __Pulsar__processingCumulative__
#define __Pulsar__processingCumulative__

#include <iostream>
#include <vector>
#include "temporal_mean_calc.h"
#include "globals.h"

using namespace std;
using namespace cv;

namespace MHR
{
    void processingCumulative(vector<double> &temporal_mean, vector<double> temp, hrResult &currentResult);
}



#endif /* defined(__Pulsar__processingCumulative__) */
