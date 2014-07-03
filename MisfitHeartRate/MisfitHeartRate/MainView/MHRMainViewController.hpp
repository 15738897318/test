//
//  OHRMainViewController.h
//  opticalHeartRate
//
//  Created by Bao Nguyen on 6/23/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <opencv2/core/core.hpp>
#import <opencv2/imgproc/types_c.h>
#import <opencv2/imgproc/imgproc.hpp>
#import <opencv2/highgui/ios.h>
#import <opencv2/highgui/cap_ios.h>
#import <opencv2/highgui/highgui.hpp>
#import <opencv2/highgui/highgui_c.h>
#import "MHREulerian.h"
#import "MHRUtilities.h"
#import "matlab.h"
#import "frames2signal.h"


using namespace cv;

@interface MHRMainViewController : UIViewController <CvVideoCameraDelegate>


@end
