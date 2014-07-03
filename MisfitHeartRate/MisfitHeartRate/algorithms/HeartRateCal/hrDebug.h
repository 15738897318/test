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

using namespace std;


namespace MHR {
    struct hrDebug {
    public:
        vector<pair<double,int>> heartBeats;
        vector<double> heartRates;
        vector<double> autocorrelation;
    };
    
    void testMathFunctions();
}

#endif /* defined(__MisfitHeartRate__hrDebug__) */
