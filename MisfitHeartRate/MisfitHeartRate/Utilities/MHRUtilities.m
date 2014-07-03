//
//  MHRUtilities.m
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/30/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import "MHRUtilities.h"

@implementation MHRUtilities

+ (NSString *) report_memory
{
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(),
                                   TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);
    if( kerr == KERN_SUCCESS )
    {
        return [NSString stringWithFormat:@"Memory in use (KB): %lu", (unsigned long)(info.resident_size / 1000)];
    }
    else
    {
        return [NSString stringWithFormat:@"Error with task_info(): %s", mach_error_string(kerr)];
    }
}


+ (void)createDirectory:(NSString *)directoryPath
{
    NSLog(@"Create directory: %@", directoryPath);
    if (![[NSFileManager defaultManager] fileExistsAtPath:directoryPath])
        [[NSFileManager defaultManager] createDirectoryAtPath:directoryPath
                                  withIntermediateDirectories:YES
                                                   attributes:nil
                                                        error:nil];
}

@end
