//
//  AbstractHRCounter.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/25/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef __MIsfitHRDetection__AbstractHRCounter__
#define __MIsfitHRDetection__AbstractHRCounter__

#include <iostream>
#include "hr_structures.h"

class AbstractHRCounter {
public:
    MHR::hrResult getHR();
};

#endif /* defined(__MIsfitHRDetection__AbstractHRCounter__) */
