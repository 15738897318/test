//
//  MHRMainViewController.h
//  videoHeartRate
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
#import <time.h>
#import "UIView+Position.h"
#import "MBProgressHUD.h"
#import "MHRUtilities.h"
#import "MHRResultViewController.hpp"
#import "MHRSettingsViewController.h"
#import "run_algorithms.h"
#import "auto_start.hpp"
#import "auto_stop.hpp"
#import "globals.h"
#import "processingCumulative.h"
#import "processingPerBlock.h"
#import "matlab.h"

const int IOS6_Y_DELTA = 60;
const int CAMERA_WIDTH = 352;
const int CAMERA_HEIGHT = 288;
const int IMAGE_WIDTH = 128;
const int IMAGE_HEIGHT = 128;
const int WIDTH_PADDING = (CAMERA_WIDTH-IMAGE_WIDTH)/2;
const int HEIGHT_PADDING = (CAMERA_HEIGHT-IMAGE_HEIGHT)/2;

using namespace cv;
using namespace MHR;


@interface MHRMainViewController : UIViewController <CvVideoCameraDelegate, MHRSettingsViewDelegate>
{
    BOOL isCapturing;
    MHRResultViewController *resultView;
    hrResult currentResult;
    int framesWithFace; // Count the number of frames having a face in the region of interest
    int framesWithNoFace; // Count the number of frames NOT having a face in the region of interest
    MBProgressHUD *progressHUD;
    
    BOOL isTorchOn;
    
    bool isCalcMode;
    double lower_range;
    double upper_range;
    hrResult result;
    std::vector<double> temporal_mean;
    NSMutableArray *frameIndexArray[2];
    
    NSOperationQueue *myQueue;
    int blockNumber[2];
    int blockCount[2];
    int nFrames[2];
}
@end
