//
//  MHRUtilities.h
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 6/30/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <mach/mach.h>

@interface MHRUtilities : NSObject

+ (NSString *)report_memory;

+ (void)createDirectory:(NSString *)directoryPath;

@end
