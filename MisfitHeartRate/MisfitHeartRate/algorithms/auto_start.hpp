#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <stdio.h>


//#include <UIControl.h>
#import "MHRMainViewController.hpp"


@interface auto_start:NSObject
    /** @function detectFrontalFaces */
    + (NSArray*) detectFrontalFaces:(cv::Mat*) frame;

    /** @function assessFaces */
    //int assessFaces(std::vector<Rect> faces, cv::Rect ROI_lower)
    + (int) assessFaces:(NSArray *)faces withLowerBound:(cv::Rect)ROI_lower;

    /** @function NsDataFromCvMat */
+ (NSMutableData*) NsDataFromCvMat:(cv::Mat*)image;

+ (UIImage *)imageWithCVMat:(const cv::Mat&)cvMat;


@end