//
//  MisfitHeartRateTests.m
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/16/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import <XCTest/XCTest.h>
//#import "MHRMainViewController.hpp"
#import "run_algorithms.h"

using namespace MHR;
using namespace std;
using namespace cv;

//const double EPSILON = 1e-5;
const double EPSILON_PERCENT = 0.1;
String resourcePath = "/Users/baonguyen/Library/Application Support/iPhone Simulator/7.1-64/Applications/2926CAAB-4B49-49FE-903E-82908E53D35A/MisfitHeartRate.app/";


@interface MisfitHeartRateTests : XCTestCase

@end


@implementation MisfitHeartRateTests

- (void)setUp
{
    [super setUp];
//    NSString *path = [[NSBundle mainBundle] resourcePath];

}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}


- (void)testResourceFiles
{
    NSString *fileName = @"ideal_bandpassing_test_0.out";
    NSString *filePath = [[NSString stringWithUTF8String:resourcePath.c_str()]
                          stringByAppendingPathComponent:fileName];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:filePath])
    {
        XCTFail(@"File %@ is not exists!", filePath);
    }
}


- (void)test_atan2Mat
{
    FILE *file = fopen(String(resourcePath + "atan2Mat_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file);
    Mat src1 = read2DMatFromFile(file, nRow, nCol);
    Mat src2 = read2DMatFromFile(file, nRow, nCol);
    fclose(file);
    
    Mat output = atan2Mat(src1, src2);
    
    file = fopen(String(resourcePath + "atan2Mat_test.out").c_str(), "r");
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
            {
                XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                fclose(file);
                return;
            }
        }
    fclose(file);
}


- (void)test_powMat
{
    FILE *file = fopen(String(resourcePath + "powMat_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file);
    Mat src =  read2DMatFromFile(file, nRow, nCol);
    double n = readDouble(file);
    fclose(file);
    
    Mat output = powMat(src, n);
    
    file = fopen(String(resourcePath + "powMat_test.out").c_str(), "r");
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
            {
                XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                fclose(file);
                return;
            }
        }
    fclose(file);
}


- (void)test_add
{
    FILE *file = fopen(String(resourcePath + "add_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file);
    Mat src1 = read2DMatFromFile(file, nRow, nCol);
    Mat src2 = read2DMatFromFile(file, nRow, nCol);
    fclose(file);
    
    Mat output = add(src1, src2);
    
    file = fopen(String(resourcePath + "add_test.out").c_str(), "r");
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
            {
                XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                fclose(file);
                return;
            }
        }
    fclose(file);
}


- (void)test_multiply
{
    FILE *file = fopen(String(resourcePath + "multiply_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file);
    Mat src1 = read2DMatFromFile(file, nRow, nCol);
    Mat src2 = read2DMatFromFile(file, nRow, nCol);
    fclose(file);
    
    Mat output = multiply(src1, src2);
    
    file = fopen(String(resourcePath + "multiply_test.out").c_str(), "r");
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
            {
                XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                fclose(file);
                return;
            }
        }
    fclose(file);
}


- (void)test_multiply_num
{
    FILE *file = fopen(String(resourcePath + "multiply_num_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file);
    Mat src =  read2DMatFromFile(file, nRow, nCol);
    double n = readDouble(file);
    fclose(file);
    
    Mat output = multiply(src, n);
    
    file = fopen(String(resourcePath + "multiply_num_test.out").c_str(), "r");
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
            {
                XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                fclose(file);
                return;
            }
        }
    fclose(file);
}


- (void)test_findpeaks
{
    FILE *file = fopen(String(resourcePath + "findpeaks_test.in").c_str(), "r");
    int n = readInt(file);
    vector<double> segment = readVectorFromFile(file, n);
    fclose(file);
    
    vector<double> max_peak_strengths;
    vector<int> max_peak_locs;
    findpeaks(segment, 0.0, 0.0, max_peak_strengths, max_peak_locs);
    n = (int)max_peak_strengths.size();
    
    file = fopen(String(resourcePath + "findpeaks_test.out").c_str(), "r");
    int m = readInt(file);
    if (n != m) {
        XCTFail(@"wrong max_peak_strengths.size() - expected: %d, found: %d", m, n);
        return;
    }

    for (int i = 0; i < n; ++i) {
        double correct_ans = readDouble(file);
        double ans = max_peak_strengths[i];
        if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
        {
            XCTFail(@"wrong max_peak_strengths - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
            fclose(file);
            return;
        }
    }
    
    for (int i = 0; i < n; ++i) {
        double correct_ans = readDouble(file);
        double ans = max_peak_locs[i];
        if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
        {
            XCTFail(@"wrong max_peak_locs - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
            fclose(file);
            return;
        }
    }
    fclose(file);
}


- (void)test_unique_stable
{
    
}


- (void)test_conv
{
    FILE *file = fopen(String(resourcePath + "conv_test.in").c_str(), "r");
    int n1 = readInt(file);
    vector<double> seg1 = readVectorFromFile(file, n1);
    int n2 = readInt(file);
    vector<double> seg2 = readVectorFromFile(file, n2);
    fclose(file);
    
    vector<double> output = conv(seg1, seg2);
   
    file = fopen(String(resourcePath + "conv_test.out").c_str(), "r");
    for (int i = 0; i < n1; ++i) {
        double correct_ans = readDouble(file);
        double ans = output[i];
        if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
        {
            XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
            fclose(file);
            return;
        }
    }
    fclose(file);
}


- (void)test_hist
{
    FILE *file = fopen(String(resourcePath + "hist_test.in").c_str(), "r");
    int nBin = readInt(file);
    int n = readInt(file);
    vector<double> arr = readVectorFromFile(file, n);
    fclose(file);
    
    vector<int> counts;
    vector<double> centers;
    hist(arr, nBin, counts, centers);
    
    file = fopen(String(resourcePath + "hist_test.out").c_str(), "r");
    for (int i = 0; i < nBin; ++i) {
        int correct_ans = readInt(file);
        int ans = counts[i];
        if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
        {
            XCTFail(@"wrong counts - expected: %d, found: %d, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
            fclose(file);
            return;
        }
    }
    for (int i = 0; i < n; ++i) {
        double correct_ans = readDouble(file);
        double ans = centers[i];
        if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
        {
            XCTFail(@"wrong centers - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
            fclose(file);
            return;
        }
    }
    fclose(file);
}


- (void)test_invprctile
{
    FILE *file = fopen(String(resourcePath + "invprctile_test.in").c_str(), "r");
    int n = readInt(file);
    vector<double> arr = readVectorFromFile(file, n);
    int nTest = readInt(file);
    vector<double> x = readVectorFromFile(file, nTest);
    fclose(file);
    
    file = fopen(String(resourcePath + "invprctile_test.out").c_str(), "r");
    for (int i = 0; i < nTest; ++i) {
        double correct_ans = readDouble(file);
        double ans = invprctile(arr, x[i]);
        if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
        {
            XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
            fclose(file);
            return;
        }
    }
    fclose(file);
}


- (void)test_prctile
{
    FILE *file = fopen(String(resourcePath + "prctile_test.in").c_str(), "r");
    int n = readInt(file);
    vector<double> arr = readVectorFromFile(file, n);
    double percent = readDouble(file);
    fclose(file);
    
    file = fopen(String(resourcePath + "prctile_test.out").c_str(), "r");
    double correct_ans = readDouble(file);
    double ans = prctile(arr, percent);
    if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
    {
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
        fclose(file);
        return;
    }
    fclose(file);
}


- (void)test_low_pass_filter
{
    FILE *file = fopen(String(resourcePath + "low_pass_filter_test.in").c_str(), "r");
    int n = readInt(file);
    vector<double> arr = readVectorFromFile(file, n);
    fclose(file);
    
    vector<double> output = low_pass_filter(arr);
    n = (int)output.size();
    
    file = fopen(String(resourcePath + "low_pass_filter_test.out").c_str(), "r");
    int m = readInt(file);
    if (n != m) {
        XCTFail(@"wrong output.size() - expected: %d, found: %d", m, n);
        return;
    }
    for (int i = 0; i < n; ++i) {
        double correct_ans = readDouble(file);
        double ans = output[i];
        if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
        {
            XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
            fclose(file);
            return;
        }
    }
    fclose(file);
}


- (void)test_rgb2tsl
{
    FILE *file = fopen(String(resourcePath + "rgb2tsl_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file), nChannel = readInt(file);
    Mat rgbmap = Mat::zeros(nRow, nCol, CV_64FC3);
    for (int k = 0; k < nChannel; ++k)
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
                rgbmap.at<Vec3d>(i, j)[k] = readDouble(file);
    fclose(file);
    
    Mat output;
    rgb2tsl(rgbmap, output);
    
    file = fopen(String(resourcePath + "rgb2tsl_test.out").c_str(), "r");
    for (int k = 0; k < nChannel; ++k)
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j) {
                double correct_ans = readDouble(file);
                double ans = output.at<Vec3d>(i, j)[k];
                if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
                {
                    XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                    fclose(file);
                    return;
                }
            }
    fclose(file);
}


- (void)test_blurDnClr
{
    FILE *file = fopen(String(resourcePath + "blurDnClr_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file), nChannel = readInt(file);
    Mat src = Mat::zeros(nRow, nCol, CV_64FC3);
    for (int k = 0; k < nChannel; ++k)
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j)
                src.at<Vec3d>(i, j)[k] = readDouble(file);
    fclose(file);
    
    Mat output;
    blurDnClr(src, output, 2);
    nRow = output.rows;
    nCol = output.cols;
    printf("nRow = %d, nCol = %d\n", nRow, nCol);
    if (nRow != 5 || nCol != 3)
    {
        XCTFail(@"wrong nRow or nCol");
        return;
    }
    
    file = fopen(String(resourcePath + "blurDnClr_test.out").c_str(), "r");
    for (int k = 0; k < nChannel; ++k)
        for (int i = 0; i < nRow; ++i)
            for (int j = 0; j < nCol; ++j) {
                double correct_ans = readDouble(file);
                double ans = output.at<Vec3d>(i, j)[k];
                if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
                {
                    XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                    fclose(file);
                    return;
                }
            }
    fclose(file);
}


- (void)test_corrDn
{
    FILE *file = fopen(String(resourcePath + "corrDn_test.in").c_str(), "r");
    int nRow = readInt(file), nCol = readInt(file);
    Mat src =  read2DMatFromFile(file, nRow, nCol);
    int rectRow = readInt(file), rectCol = readInt(file);
    Mat filter =  read2DMatFromFile(file, rectRow, rectCol);
    fclose(file);
    
    Mat output;
    corrDn(src, output, filter, rectRow, rectCol);
    
    file = fopen(String(resourcePath + "corrDn_test.out").c_str(), "r");
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
            {
                XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                fclose(file);
                return;
            }
        }
    fclose(file);
}


- (void)test_frames2signal
{
    FILE *file = fopen(String(resourcePath + "frames2signal_test.in").c_str(), "r");
    int nFrame = readInt(file);
    int nRow = readInt(file), nCol = readInt(file);
    Mat tmp = Mat::zeros(nRow, nCol, CV_64F);
    vector<Mat> monoframes;
    for (int i = 0; i < nFrame; ++i)
        monoframes.push_back(tmp.clone());
    for (int col = 0; col < nCol; ++col)
        for (int i = 0; i < nFrame; ++i)
            for (int row = 0; row < nRow; ++row)
                monoframes[i].at<double>(row, col) = readDouble(file);
    fclose(file);
    
    double fr = 300, cutoff_freq = 10;
    double lower_range = 0, upper_range = 10;
    vector<double> output = frames2signal(monoframes, "tsl", fr, cutoff_freq, lower_range, upper_range, false);
    int nSignal = (int)output.size();
    
    file = fopen(String(resourcePath + "frames2signal_test.out").c_str(), "r");
    int m = readInt(file);
    if (m != nSignal) {
        XCTFail(@"wrong output.size() - expected: %d, found: %d", m, nSignal);
        return;
    }
    for (int i = 0; i < nSignal; ++i) {
            double correct_ans = readDouble(file);
            double ans = output[i];
            if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
            {
                XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
                fclose(file);
                return;
            }
        }
    fclose(file);
}


- (void)test_ideal_bandpassing {
    return;
    
    int nTime = 100, nRow = 5, nCol = 2;
    double samplingRate = 20;
    double wl = 2;
    double wh = 7;
    vector<double> array[2][3];
    String inFilePath = resourcePath + "ideal_bandpassing_test_0.in";
    FILE *inFile = fopen(inFilePath.c_str(), "r");
    double value = -1;
    for (int col = 0; col < nCol; ++col)
        for (int channel = 0; channel < 3; ++channel)
            for (int time = 0; time < nTime; ++time)
                for (int row = 0; row < nRow; ++row) {
                    fscanf(inFile, "%lf", &value);
                    printf("%lf, ", value);
                    array[col][channel].push_back(value);
                }
    fclose(inFile);

//        int input_size[] = {nTime, nRow, nCol};
    vector<Mat> input;
    Mat tmp = Mat::zeros(nRow, nCol, CV_64FC3);
    for (int i = 0; i < nTime; ++i)
        input.push_back(tmp.clone());
    for (int time = 0; time < nTime; ++time)
        for (int row = 0; row < nRow; ++row)
            for (int col = 0; col < nCol; ++col)
                for (int channel = 0; channel < 3; ++channel)
                    input[time].at<Vec3d>(row, col)[channel] = array[col][channel][time*nRow + row];
//            input[k].at<Vec3d>(i, 0)[0] = array_0_0[k*nRow + i];
//            input[k].at<Vec3d>(i, 1)[0] = array_1_0[k*nRow + i];
//            input[k].at<Vec3d>(i, 0)[1] = array_0_1[k*nRow + i];
//            input[k].at<Vec3d>(i, 1)[1] = array_1_1[k*nRow + i];
//            input[k].at<Vec3d>(i, 0)[2] = array_0_2[k*nRow + i];
//            input[k].at<Vec3d>(i, 1)[2] = array_1_2[k*nRow + i];
    
    vector<Mat> output;
    ideal_bandpassing(input, output, wl, wh, samplingRate);
    
    String resultFilePath = resourcePath + "ideal_bandpassing_test_0.out";
    FILE *resultFile = fopen(resultFilePath.c_str(), "r");
//    printf("Output vector<Mat>: size() = %i, nRow = %i, nCol = %i\n", (int)input.size(), nRow, nCol);
    double ans, correct_ans;
    for (int channel = 0; channel < 3; ++channel)
        for (int col = 0; col < nCol; ++col) {
            for (int time = 0; time < nTime; ++time) {
                for (int row = 0; row < nRow; ++row) {
                    fscanf(resultFile, "%lf", &correct_ans);
                    ans = output[time].at<Vec3d>(row, col)[channel];
                    if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
                    {
                        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf at time = %d, row = %d, col = %d, channel = %d", correct_ans, ans, diff_percent(ans, correct_ans), time, row, col, channel);
                        fclose(resultFile);
                        return;
                    }
//                    printf("%lf, ", output[time].at<Vec3d>(row, col)[channel]);
                }
//                printf("\n");
            }
//            printf("\n\n\n");
        }
    fclose(resultFile);
}


- (void)test_hr_calc_autocorr
{
    vector<double> input;
    for(double x=0; x<=100; x+=0.01)
        input.push_back(sin(x*acos(-1)));
    double correct_ans = 9.0;
    
    int length = (int)input.size();
    printf("input.size() = %d\n", length);
//        for (int i = 0; i < length; ++i)
//            printf("%lf, ", input[i]);
//        printf("\n\n");
    
    vector<double> strengths;
    vector<int> locs;
    findpeaks(input, 0, 0, strengths, locs);
    printf("strengths.size() = %d\n",(int)strengths.size());
    
    hrDebug debug;
    double ans = hr_calc_autocorr(input, 30.0, 0, 100, 0.0, 0.0, debug);
    printf("hr_calc_autocorr = %lf\n", ans);
    printf("debug.heartBeats.size() = %d\n", (int)debug.heartBeats.size());
    printf("debug.heartRates.size() = %d\n", (int)debug.heartRates.size());
    printf("debug.autocorrelation.size() = %d\n", (int)debug.autocorrelation.size());
    
    if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
    {
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
        return;
    }
    
//    printf("\n\nautocorrelation:\n");
//    for (int i = 0; i < (int)debug.autocorrelation.size(); ++i)
//        printf("%lf, ", debug.autocorrelation[i]);
//    printf("\n\n");
}


- (void)test_hr_calc_pda
{
    vector<double> input;
    for(double x=0; x<=100; x+=0.01)
        input.push_back(sin(x*acos(-1)));
    double correct_ans = 9.0;
    
    int length = (int)input.size();
    printf("input.size() = %d\n", length);
//        for (int i = 0; i < length; ++i)
//            printf("%lf, ", input[i]);
//        printf("\n\n");
    
    vector<double> strengths;
    vector<int> locs;
    findpeaks(input, 0, 0, strengths, locs);
    printf("strengths.size() = %d\n",(int)strengths.size());
    
    hrDebug debug;
    double ans = hr_calc_pda(input, 30.0, 0, 120, 0.0, 0.0, 0, debug);
    printf("hr_calc_pda = %lf\n", ans);
    printf("debug.heartBeats.size() = %d\n", (int)debug.heartBeats.size());
    printf("debug.heartRates.size() = %d\n", (int)debug.heartRates.size());
    printf("debug.autocorrelation.size() = %d\n", (int)debug.autocorrelation.size());
    
    if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
    {
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
        return;
    }

//        printf("\n\nautocorrelation:\n");
//        for (int i = 0; i < (int)debug.autocorrelation.size(); ++i)
//            printf("%lf, ", debug.autocorrelation[i]);
//        printf("\n\n");
}


@end
