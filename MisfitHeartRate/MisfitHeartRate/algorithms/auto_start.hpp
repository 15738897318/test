#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <stdio.h>


#import "MHRMainViewController.hpp"


@interface auto_start:NSObject
    /** For face detection */
    + (NSArray*) detectFrontalFaces:(cv::Mat*) frame;

    + (int) assessFaces:(NSArray *)faces withLowerBound:(cv::Rect)ROI_lower;

    + (NSMutableData*) NsDataFromCvMat:(cv::Mat*)image;

    + (UIImage *)imageWithCVMat:(const cv::Mat&)cvMat;

    + (void) removeEyesAndMouth:(cv::Mat*)new_image;

    /** For finger detection */
    + (BOOL)isDarkOrDarkRed:(Mat)tmp;

    + (BOOL)isUniformColored:(Mat)tmp;

    + (BOOL)isSameAsPreviousFrame:(Mat)tmp;

    + (BOOL)isRedColored:(Mat)tmp;

    + (float)calculateAverageRedValue:(Mat)tmp;

    + (BOOL)isHeartBeat:(vector <float>)val;

@end