//
//  hr_structures.cpp
//  Pulsar
//
//  Created by Bao Nguyen on 8/20/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#include "hr_structures.h"

namespace MHR {
    hrResult::hrResult() {}
    hrResult::hrResult(double autocorr, double pda)
    {
        this->autocorr = autocorr;
        this->pda = pda;
    }
    
    
    void hrResult::operator = (const hrResult &other)
    {
        this->autocorr = other.autocorr;
        this->pda = other.pda;
    }
}