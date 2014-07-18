//
//  hrDebug.h
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hrDebug__
#define __MisfitHeartRate__hrDebug__

#include <iostream>
#include <vector>
#include "matlab.h"
#include "ideal_bandpassing.h"
#include "image.h"

using namespace std;


namespace MHR {
    struct hrDebug {
        vector<pair<double,int>> heartBeats;
        vector<double> heartRates;
        vector<double> autocorrelation;
    };
}

#endif /* defined(__MisfitHeartRate__hrDebug__) */
