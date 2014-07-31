//
//  MHRTest.mm
//  Pulsar
//
//  Created by Bao Nguyen on 7/30/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#import "MHRTest.hpp"

@implementation MHRTest

+ (void)test_run_algorithm
{
    // create new directory for this session
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
    NSString *outPath = [paths objectAtIndex:0];
    outPath = [outPath substringToIndex:([outPath length] - [@"Library/Documentation/" length] + 1)];
    NSDateFormatter *formater = [[NSDateFormatter alloc] init];
    formater.dateFormat = @"yyyy-MM-dd-HH-mm-ss";
    outPath = [outPath stringByAppendingFormat:@"Documents/%@/",
               [formater stringFromDate:[NSDate date]]];
    [MHRUtilities createDirectory:outPath];
    NSLog(@"output path = %@", outPath);
    
    // run run_algorithms()
    FILE *outFile = fopen([[outPath stringByAppendingPathComponent:@"all_files_result.csv"] UTF8String], "w");
    fprintf(outFile, "name, autocorr, pda\n");
    
    NSString *inPath = [[NSBundle mainBundle] bundlePath];
    NSLog(@"input path = %@", inPath);
    NSArray *directoryContent = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:inPath error:NULL];
    for (NSString *fileName in directoryContent) {
        if ([fileName hasSuffix:@".mp4"]) {
            NSLog(@"File %@", fileName);
            
            usleep(1000); //Delay the operation a bit to allow garbage collector to clear the memory
            
            MHR::hrResult result = MHR::run_algorithms([inPath UTF8String], [fileName UTF8String], [outPath UTF8String]);
            fprintf(outFile, "%s, %lf, %lf\n", [fileName UTF8String], result.autocorr, result.pda);
        }
    }
    
    fclose(outFile);
}

@end
