//
//  testFunctions.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/15/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__testFunctions__
#define __MisfitHeartRate__testFunctions__

#include <iostream>
#include <vector>
#include "matlab.h"
#include "ideal_bandpassing.h"
#include "image.h"
#include "hb_counter_autocorr.h"
#include "hb_counter_pda.h"

using namespace std;


namespace MHR {
    void testMathFunctions();
    
    void test_rgb2ntsc();
    
    void test_openCV();    
}

#endif /* defined(__MisfitHeartRate__testFunctions__) */
