//
//  hr_calculator.h
//  MisfitHeartRate
//
//  Created by Tuan-Anh Tran on 7/14/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#ifndef __MisfitHeartRate__hr_calculator__
#define __MisfitHeartRate__hr_calculator__

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <string.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include "matrix.h"
#include "matlab.h"
#include "config.h"

using namespace cv;
using namespace std;


namespace MHR
{
	void hr_calculator(const vector<int> &heartBeatPositions, double frameRate, vector<double> &ans);
    
	void gaussianFilter(int length, double sigma, vector<double> &ans);
}

#endif /* defined(__MisfitHeartRate__hr_calculator__) */
