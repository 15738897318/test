//
//  processingPerBlock.h
//  Pulsar
//
//  Created by HaiPhan on 8/19/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#ifndef __Pulsar__processingPerBlock__
#define __Pulsar__processingPerBlock__

#include <iostream>
#include "processingCumulative.h"
#include "eulerian.h"

namespace MHR
{
    void processingPerBlock(const string &srcDir, const string &outDir,
                            int fileStartIndex, int fileEndIndex,
                            bool &isCalcMode, double &lower_range, double &upper_range,
                            hrResult &result, vector<double> &tmp);
}
#endif /* defined(__Pulsar__processingPerBlock__) */
