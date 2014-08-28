//
//  UIAlertView+Block.h
//  SensorsCapture
//
//  Created by Bao Nguyen on 6/18/14.
//  Copyright (c) 2014 Misfit Wearables. All rights reserved.
//
//  ref: https://github.com/MugunthKumar/UIKitCategoryAdditions/blob/master/MKAdditions/UIAlertView%2BMKBlockAdditions.h
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "BlockAdditions.h"

@interface UIAlertView (Block) <UIAlertViewDelegate>

+ (UIAlertView*) alertViewWithTitle:(NSString*) title
                            message:(NSString*) message;

+ (UIAlertView*) alertViewWithTitle:(NSString*) title
                            message:(NSString*) message
                  cancelButtonTitle:(NSString*) cancelButtonTitle;

+ (UIAlertView*) alertViewWithTitle:(NSString*) title
                            message:(NSString*) message
                  cancelButtonTitle:(NSString*) cancelButtonTitle
                  otherButtonTitles:(NSArray*) otherButtons
                          onDismiss:(DismissBlock) dismissed
                           onCancel:(CancelBlock) cancelled;

@property (nonatomic, copy) DismissBlock dismissBlock;
@property (nonatomic, copy) CancelBlock cancelBlock;

@end