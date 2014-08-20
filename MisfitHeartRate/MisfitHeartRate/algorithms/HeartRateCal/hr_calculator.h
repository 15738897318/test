//
//  hr_calculator.h
//  MisfitHeartRate
//
//  Created by Tuan-Anh Tran on 7/14/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hr_calculator__
#define __MisfitHeartRate__hr_calculator__

#include "matlab.h"

using namespace cv;
using namespace std;

namespace MHR {
    /**
     * Calculate the heart-rate from a list of heart-beat positions.
     * <ans>:
     *  + the first number is average heart-rate
     *  + the second number is mode of the instantaneous heart-rates multiply with frameRate*60
     */
	void hr_calculator(const vector<int> &heartBeatPositions, double frameRate, vector<double> &ans);
}

#endif /* defined(__MisfitHeartRate__hr_calculator__) */
