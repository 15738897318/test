//
//  MHRUtilities.m
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/30/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import "MHRUtilities.h"


@implementation MHRUtilities

#pragma - Debug

+ (NSString *) report_memory
{
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(),
                                   TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);
    if (kerr == KERN_SUCCESS)
    {
        return [NSString stringWithFormat:@"Memory in use (KB): %lu", (unsigned long)(info.resident_size / 1000)];
    }
    else
    {
        return [NSString stringWithFormat:@"Error with task_info(): %s", mach_error_string(kerr)];
    }
}


#pragma - Files

+ (void)createDirectory:(NSString *)directoryPath
{
    if (![[NSFileManager defaultManager] fileExistsAtPath:directoryPath])
        [[NSFileManager defaultManager] createDirectoryAtPath:directoryPath
                                  withIntermediateDirectories:YES
                                                   attributes:nil
                                                        error:nil];
}


#pragma - Video Camera

+ (void)setTorchModeOn:(BOOL)isOn
{
    AVCaptureDevice *device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    if ([device hasTorch])
    {
        [device lockForConfiguration:nil];
        if (isOn)
        {
            [device setTorchMode:AVCaptureTorchModeOn];
        }
        else
        {
            [device setTorchMode:AVCaptureTorchModeOff];
        }
        [device unlockForConfiguration];
    }
}


#pragma - Colors

UIColor* MFUIColorMakeFromRGBArray(NSArray* rgbArray)
{
    return MFRGB(
                 [(NSNumber*)[rgbArray objectAtIndex:0] intValue],
                 [(NSNumber*)[rgbArray objectAtIndex:1] intValue],
                 [(NSNumber*)[rgbArray objectAtIndex:2] intValue]);
}


UIColor* MFUIColorMakeFromRGBAArray(NSArray* rgbaArray)
{
    return MFRGBA(
                  [(NSNumber*)[rgbaArray objectAtIndex:0] intValue],
                  [(NSNumber*)[rgbaArray objectAtIndex:1] intValue],
                  [(NSNumber*)[rgbaArray objectAtIndex:2] intValue],
                  [(NSNumber*)[rgbaArray objectAtIndex:3] floatValue]);
}


+ (CALayer*)newRectangleLayer:(CGRect)frame pListKey:(NSString *)pListKey
//CALayer* newRectangleLayer(CGRect frame)
{
    MHRAppDelegate* appDelegate = (MHRAppDelegate *)[UIApplication sharedApplication].delegate;
    CALayer *rect = [CALayer layer];
    rect.frame = frame;
    rect.backgroundColor = MFUIColorMakeFromRGBAArray([[appDelegate getPlistSkinDict] objectForKey:pListKey]).CGColor;
    return rect;
}

@end
