#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <stdio.h>


//#include <UIControl.h>
#import "MHRMainViewController.hpp"


@interface auto_stop:NSObject

    + (BOOL)faceCheck:(Mat)tmp;

    + (BOOL)fingerCheck:(Mat)frame;
@end