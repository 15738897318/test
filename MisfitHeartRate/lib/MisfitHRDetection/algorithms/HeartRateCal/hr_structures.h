//
//  hr_structures.h
//  Pulsar
//
//  Created by Bao Nguyen on 8/20/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#ifndef __Pulsar__hr_structures__
#define __Pulsar__hr_structures__

#include <vector>
#include <utility>

using namespace std;

namespace MHR {
    struct hrDebug {
        double avg_hr;
        vector<pair<double,int>> heartBeats;
        vector<double> heartRates;
        vector<double> autocorrelation;
    };
    
    
    struct hrResult
    {
        double autocorr;        // avg_hr_autocorr
        double pda;             // avg_hr_pda
        hrResult();
        hrResult(double autocorr, double pda);
        
        void operator = (const hrResult &other);
    };
}
#endif /* defined(__Pulsar__hr_structures__) */
