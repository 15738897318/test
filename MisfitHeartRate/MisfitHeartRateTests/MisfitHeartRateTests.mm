//
//  MisfitHeartRateTests.m
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/16/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import <XCTest/XCTest.h>
//#import "MHRMainViewController.hpp"
#import <MIsfitHRDetection/run_algorithms.h>
#import "MHRUtilities.h"

using namespace MHR;
using namespace std;
using namespace cv;

String resourcePath = "get simulator's resource path in setUp() function";
///Users/baonguyen/Library/Application Support/iPhone Simulator/7.1-64/Applications/7CD329BE-D62D-4B46-BBCB-7512C37724D4/Pulsar.app/";


@interface MisfitHeartRateTests : XCTestCase

@end


@implementation MisfitHeartRateTests

- (void)setUp
{
    [super setUp];
    resourcePath = [[[[NSBundle bundleForClass:[self class]] bundlePath] stringByAppendingString:@"/"] UTF8String];
    printf("path = %s\n", resourcePath.c_str());
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}


- (void)test_run_algorithm
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
    
    NSString *inPath = [[NSBundle bundleForClass:[self class]] bundlePath];
    NSLog(@"input path = %@", inPath);
    NSArray *directoryContent = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:inPath error:NULL];
    for (NSString *fileName in directoryContent) {
        if ([fileName hasSuffix:@".mp4"]) {
            NSLog(@"File %@", fileName);
            
            usleep(1000); //Delay the operation a bit to allow garbage collector to clear the memory
            
            MHR::hrResult currResult;
            MHR::hrResult result = MHR::run_algorithms([inPath UTF8String], [outPath UTF8String], currResult);
            fprintf(outFile, "%s, %lf, %lf\n", [fileName UTF8String], result.autocorr, result.pda);
        }
    }
    
    fclose(outFile);
}

@end
