//
//  MHRResultViewController.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/8/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "config.h"

using namespace MHR;

@interface MHRResultViewController : UIViewController

@property (assign, nonatomic) mTYPE autocorrResult;
@property (assign, nonatomic) mTYPE pdaResult;

@end
