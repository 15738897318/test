//
//  MHRUtilities.h
//  Pulsar
//
//  Created by Bao Nguyen on 6/30/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <mach/mach.h>
#import <AVFoundation/AVFoundation.h>
#import "MHRAppDelegate.hpp"


@interface MHRUtilities : NSObject

#define SCREEN_SIZE_IS_4_INCHES                   ([UIScreen mainScreen].bounds.size.height >= 568)

#define SYSTEM_VERSION_EQUAL_TO(v)                  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)

#define SYSTEM_VERSION_GREATER_THAN(v)              ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)

#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)

#define SYSTEM_VERSION_LESS_THAN(v)                 ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)

#define SYSTEM_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)


#pragma - Debug

+ (NSString *)report_memory;


#pragma - Files

+ (void)createDirectory:(NSString *)directoryPath;


#pragma - Video Camera

+ (void)setTorchModeOn:(BOOL)isOn;


#pragma - Colors

#define MFRGB(r, g, b)          [UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1]
#define MFRGBA(r, g, b, a)      [UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:a]

UIColor* MFUIColorMakeFromRGBArray(NSArray* rgbArray);
UIColor* MFUIColorMakeFromRGBAArray(NSArray* rgbaArray);

//CALayer* newRectangleLayer(CGRect frame/*, NSString *pListKey*/);
+ (CALayer*)newRectangleLayer:(CGRect)frame pListKey:(NSString *)pListKey;

@end
