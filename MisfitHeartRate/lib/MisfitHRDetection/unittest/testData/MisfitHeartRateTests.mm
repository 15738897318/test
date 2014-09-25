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

const double EPSILON = 1e-4;
const double EPSILON_PERCENT = 2.8;
String resourcePath = "get simulator's resource path in setUp() function";
///Users/baonguyen/Library/Application Support/iPhone Simulator/7.1-64/Applications/7CD329BE-D62D-4B46-BBCB-7512C37724D4/Pulsar.app/";


@interface MisfitHeartRateTests : XCTestCase

@end


@implementation MisfitHeartRateTests

- (void)setUp
{
    [super setUp];
    resourcePath = [[[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/"] UTF8String];
    printf("path = %s\n", resourcePath.c_str());
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
                          stringByAppendingString:fileName];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:filePath])
    {
        XCTFail(@"File %@ is not exists!", filePath);
    }
}


- (void)testOpenCV
{
    int n = 10, m = 10;
    Mat a = Mat::zeros(n, m, CV_8U);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            a.at<unsigned char>(i, j) = ((i+100)*(j+100))%256;
    
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            printf("%d, ", a.at<unsigned char>(i, j));
        printf("\n");
    }
    printf("\n");
    
    Mat b;
    a.convertTo(b, CV_64F);
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            printf("%lf, ", b.at<double>(i, j));
        printf("\n");
    }
    printf("\n");
    
    
    b.convertTo(b, CV_32F);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            printf("%lf, ", b.at<float>(i, j));
        printf("\n");
    }
    printf("\n");
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
    double max_percent = 0;
    double max_correct_ans = 0, max_ans = 0;
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            double tmp = diff_percent(ans, correct_ans);
            if (tmp > max_percent)
            {
                max_percent = tmp;
                max_correct_ans = correct_ans;
                max_ans = ans;
            }
        }
    fclose(file);
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
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
    FILE *file = fopen(String(resourcePath + "unique_stable_test.in").c_str(), "r");
    int n = readInt(file);
    vector<pair<double,int> > arr;
    for (int i = 0; i < n; ++i) {
        arr.push_back(make_pair<double, int>(0.0, 0));
        arr[i].first = readDouble(file);
        arr[i].second = readInt(file);
    }
    fclose(file);
    
    vector<pair<double,int>> output = unique_stable(arr);
    n = (int)output.size();
//    printf("output.size() = %d\n", n);
//    for (int i = 0; i < (int)output.size(); ++i)
//        printf("%lf\n", output[i]);
    
    file = fopen(String(resourcePath + "unique_stable_test.out").c_str(), "r");
    int m = readInt(file);
    if (n != m) {
        XCTFail(@"wrong output.size() - expected: %d, found: %d", m, n);
        fclose(file);
        return;
    }
    for (int i = 0; i < n; ++i) {
        double a = readDouble(file);
        int b = readInt(file);
        if (diff_percent(a, output[i].first) > EPSILON_PERCENT || diff_percent(b, output[i].second) > EPSILON_PERCENT) {
            XCTFail(@"wrong output - expected: (%lf, %d), found: (%lf, %d)", a, b, output[i].first, output[i].second);
        }
    }
    fclose(file);

}


- (void)test_corr_linear
{
    FILE *file = fopen(String(resourcePath + "corr_linear_test.in").c_str(), "r");
    int n1 = readInt(file);
    vector<double> seg1 = readVectorFromFile(file, n1);
    int n2 = readInt(file);
    vector<double> seg2 = readVectorFromFile(file, n2);
    fclose(file);
    
    vector<double> output = corr_linear(seg1, seg2);

    int n = (int)output.size();
    if (n != n1) {
        XCTFail(@"wrong output.size() - expected: %d, found: %d", n1, n);
        return;
    }
   
    file = fopen(String(resourcePath + "corr_linear_test.out").c_str(), "r");
    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
    for (int i = 0; i < n1; ++i) {
        double correct_ans = readDouble(file);
        double ans = output[i];
        double tmp = diff_percent(ans, correct_ans);
        if (tmp > max_percent)
        {
            max_percent = tmp;
            max_correct_ans = correct_ans;
            max_ans = ans;
        }
    }
    fclose(file);
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
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
        printf("count = %d\n", ans);
    }
    for (int i = 0; i < nBin; ++i) {
        double correct_ans = readDouble(file);
        double ans = centers[i];
        printf("centers = %lf\n", ans);
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
    int filterRow = readInt(file), filterCol = readInt(file);
    Mat filter =  read2DMatFromFile(file, filterRow, filterCol);
    fclose(file);
    
    Mat output;
    corrDn(src, output, filter, 1, 1);
    for (int i = 0; i < output.rows; ++i) {
        for (int j = 0; j < output.cols; ++j)
            printf("%lf, ", output.at<double>(i, j));
        printf("\n");
    }
    
    file = fopen(String(resourcePath + "corrDn_test.out").c_str(), "r");
    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
    for (int i = 0; i < nRow; ++i)
        for (int j = 0; j < nCol; ++j) {
            double correct_ans = readDouble(file);
            double ans = output.at<double>(i, j);
            double tmp = diff_percent(ans, correct_ans);
            if (tmp > max_percent)
            {
                max_percent = tmp;
                max_correct_ans = correct_ans;
                max_ans = ans;
            }
        }
    fclose(file);
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
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
    for (int i = 0; i < nFrame; ++i)
        for (int row = 0; row < nRow; ++row)
            for (int col = 0; col < nCol; ++col)
                monoframes[i].at<double>(row, col) = readDouble(file);
    fclose(file);
    
    double fr = 30, cutoff_freq = 0;
    double lower_range, upper_range;
    vector<double> output = frames2signal(monoframes, "mode-balance", fr, cutoff_freq, lower_range, upper_range, true);

//    vector<double> kernel;
//    for (int i = 0; i < _beatSignalFilterKernel.size.p[0]; ++i)
//        for (int j = 0; j < _beatSignalFilterKernel.size.p[1]; ++j)
//            kernel.push_back(_beatSignalFilterKernel.at<double>(i, j));
//    output = corr_linear(output, kernel);
    
    output = low_pass_filter(output);
    int nSignal = (int)output.size();
    
    for (int i = 0; i < nSignal; ++i)
        printf("%d, %lf\n", i, output[i]);
    
    file = fopen(String(resourcePath + "frames2signal_test.out").c_str(), "r");
    int m = readInt(file);
    if (m != nSignal) {
        XCTFail(@"wrong output.size() - expected: %d, found: %d", m, nSignal);
        return;
    }
    for (int i = 0; i < nSignal; ++i) {
            double correct_ans = readDouble(file);
            double ans = output[i];
            printf("%d, %lf\n", i, ans);
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
    FILE *file = fopen(String(resourcePath + "ideal_bandpassing_test_0.in").c_str(), "r");
    int nTime = readInt(file);
    int nRow = readInt(file), nCol = readInt(file);
    double samplingRate = readDouble(file);
    double wl = readDouble(file), wh = readDouble(file);
    
    vector<Mat> input;
    Mat tmp = Mat::zeros(nRow, nCol, CV_64FC3);
    for (int i = 0; i < nTime; ++i)
        input.push_back(tmp.clone());
    for (int channel = 0; channel < 3; ++channel)
        for (int col = 0; col < nCol; ++col)
            for (int t = 0; t < nTime; ++t)
                for (int row = 0; row < nRow; ++row)
                    input[t].at<Vec3d>(row, col)[channel] = readDouble(file);
    fclose(file);
    
    vector<Mat> output;
    ideal_bandpassing(input, output, wl, wh, samplingRate);
    
    file = fopen(String(resourcePath + "ideal_bandpassing_test_0.out").c_str(), "r");
//    printf("Output vector<Mat>: size() = %i, nRow = %i, nCol = %i\n", (int)input.size(), nRow, nCol);
    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
    for (int channel = 0; channel < 3; ++channel)
        for (int col = 0; col < nCol; ++col) {
            for (int t = 0; t < nTime; ++t) {
                for (int row = 0; row < nRow; ++row) {
                    double correct_ans = readDouble(file);
                    double ans = output[t].at<Vec3d>(row, col)[channel];
//                    double tmp = diff_percent(ans, correct_ans);
                    double tmp = (abs(ans-correct_ans) > EPSILON)*100;
                    if (tmp > max_percent)
                    {
                        max_percent = tmp;
                        max_correct_ans = correct_ans;
                        max_ans = ans;
                    }
                    printf("%lf, ", output[t].at<Vec3d>(row, col)[channel]);
                }
                printf("\n");
            }
            printf("\n\n\n");
        }
    fclose(file);
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
}


- (void)test_gaussianFilter
{
    int length = 20;
    double sigma = 1.5;
    vector<double> ans;
    gaussianFilter(length, sigma, ans);
    for (int i = 0, sz = (int)ans.size(); i < sz; ++i)
        printf("ans[%d] = %lf\n", i, ans[i]);
}


- (void)test_hr_calculator
{
    FILE *file = fopen(String(resourcePath + "hr_calculator_test.in").c_str(), "r");
    int n = readInt(file);
    vector<int> heartBeatPositions;
    for (int i = 0; i < n; ++i)
        heartBeatPositions.push_back(readInt(file));
    fclose(file);
    
    vector<double> output;
    hr_calculator(heartBeatPositions, 10, output);
    
    file = fopen(String(resourcePath + "hr_calculator_test.out").c_str(), "r");
    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
    for (int i = 0; i < 2; ++i)
    {
        double correct_ans = readDouble(file);
        double ans = output[i];
        double tmp = diff_percent(ans, correct_ans);
        if (tmp > max_percent)
        {
            max_percent = tmp;
            max_correct_ans = correct_ans;
            max_ans = ans;
        }
    }
    fclose(file);
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
}


- (void)test_hb_counter_autocorr
{
    FILE *file = fopen(String(resourcePath + "hb_counter_autocorr_test.in").c_str(), "r");
    int n = readInt(file);
    vector<double> temporal_mean;
    for (int i = 0; i < n; ++i) {
        double x = readDouble(file);
        temporal_mean.push_back(x);
    }
    double frameRate = readDouble(file);
    int firstSample = readInt(file);
    int window_size = readInt(file);
    double overlap_ratio = readDouble(file);
    double minPeakDistance = readDouble(file);
//    double threshold = readDouble(file);
    fclose(file);
    
    hrDebug debug;
    vector<int> output = hb_counter_autocorr(temporal_mean, frameRate, firstSample, window_size,
                                             overlap_ratio, minPeakDistance, debug);
    
    file = fopen(String(resourcePath + "hb_counter_autocorr_test.out").c_str(), "r");
    n = (int)output.size();
    int m = readInt(file);
    if (m != n) {
        XCTFail(@"wrong output.size() - expected: %d, found: %d", m, n);
        return;
    }
    
    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
    for (int i = 0; i < n; ++i)
    {
        int correct_ans = readInt(file);
        int ans = output[i];
        double tmp = diff_percent(ans, correct_ans);
        if (tmp > max_percent)
        {
            max_percent = tmp;
            max_correct_ans = correct_ans;
            max_ans = ans;
        }
    }
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    n = (int)debug.heartBeats.size();
    m = readInt(file);
    if (m != n) {
        XCTFail(@"wrong heartBeats.size() - expected: %d, found: %d", m, n);
        return;
    }
    for (int i = 0; i < n; ++i)
    {
        double tmp = max(diff_percent(debug.heartBeats[i].first, readDouble(file)),
                         diff_percent(debug.heartBeats[i].second, readInt(file)));
        max_percent = max(max_percent, tmp);
    }
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - percent = %lf", max_percent);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    n = (int)debug.heartRates.size();
    m = readInt(file);
    if (m != n) {
        XCTFail(@"wrong heartRates.size() - expected: %d, found: %d", m, n);
        return;
    }
    for (int i = 0; i < n; ++i)
    {
        double correct_ans = readDouble(file);
        double ans = debug.heartRates[i];
        printf("debug.heartRates[%d] = %lf\n", i, ans);
        double tmp = diff_percent(ans, correct_ans);
        if (tmp > max_percent)
        {
            max_percent = tmp;
            max_correct_ans = correct_ans;
            max_ans = ans;
        }
    }
    if (max_percent > EPSILON_PERCENT)
        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
    fclose(file);
}


- (void)test_hb_counter_pda
{
    double input[] = {0.019163, 0.020138, 0.021193, 0.021776, 0.021532, 0.020445, 0.018137, 0.014522, 0.010996, 0.009046, 0.009033, 0.010771, 0.013807, 0.017501, 0.021010, 0.023283, 0.024015, 0.023065, 0.021282, 0.019326, 0.017607, 0.017003, 0.018093, 0.019642, 0.020950, 0.021776, 0.021543, 0.020373, 0.018921, 0.017814, 0.017638, 0.017861, 0.017560, 0.016170, 0.014260, 0.012239, 0.010717, 0.010010, 0.010976, 0.013243, 0.015991, 0.018853, 0.021250, 0.022504, 0.022213, 0.020542, 0.018220, 0.016679, 0.016479, 0.017185, 0.018489, 0.019903, 0.021360, 0.022451, 0.023018, 0.023617, 0.023854, 0.023437, 0.022331, 0.020245, 0.017035, 0.013034, 0.008712, 0.005406, 0.004146, 0.004960, 0.007422, 0.010891, 0.014817, 0.018490, 0.021178, 0.022762, 0.023843, 0.024710, 0.025377, 0.025312, 0.024527, 0.023359, 0.022139, 0.020787, 0.019047, 0.017924, 0.017275, 0.017203, 0.017081, 0.016548, 0.016244, 0.015722, 0.014059, 0.012100, 0.011015, 0.011092, 0.012459, 0.014394, 0.016413, 0.019121, 0.021683, 0.023989, 0.025060, 0.025250, 0.024404, 0.021913, 0.018562, 0.015886, 0.014216, 0.013628, 0.014076, 0.015021, 0.015871, 0.016721, 0.017275, 0.017571, 0.017676, 0.017318, 0.016390, 0.014759, 0.013115, 0.011958, 0.012070, 0.013519, 0.017129, 0.020676, 0.024422, 0.027904, 0.030421, 0.031612, 0.030826, 0.027662, 0.022788, 0.017126, 0.012298, 0.009203, 0.008132, 0.008167, 0.008672, 0.008686, 0.009879, 0.011314, 0.012825, 0.013960, 0.014473, 0.014941, 0.015741, 0.017710, 0.020662, 0.023977, 0.026824, 0.028454, 0.028730, 0.028073, 0.026343, 0.023880, 0.020974, 0.018118, 0.015960, 0.015437, 0.016298, 0.018047, 0.019275, 0.019164, 0.017797, 0.015001, 0.011002, 0.006698, 0.002464, 0.000000, 0.000387, 0.003876, 0.010276, 0.018190, 0.025424, 0.030369, 0.032250, 0.031429, 0.028853, 0.025259, 0.021959, 0.020093, 0.020096, 0.021736, 0.023741, 0.024918, 0.024904, 0.022803, 0.019099, 0.014582, 0.010426, 0.007668, 0.006514, 0.006814, 0.008320, 0.010568, 0.012931, 0.014979, 0.017179, 0.019315, 0.021046, 0.021716, 0.021369, 0.019993, 0.018164, 0.016355, 0.015088, 0.015075, 0.016816, 0.020088, 0.024280, 0.027952, 0.029782, 0.029054, 0.025848, 0.020640, 0.014412, 0.009073, 0.005511, 0.004434, 0.006302, 0.010570, 0.016109, 0.021519, 0.025735, 0.027588, 0.027211, 0.024730, 0.020638, 0.016001, 0.012225, 0.010112, 0.009605, 0.010994, 0.013955, 0.017492, 0.020818, 0.023263, 0.024359, 0.023702, 0.021836, 0.018760, 0.015516, 0.012781, 0.011159, 0.010854, 0.011816, 0.012543, 0.012784, 0.013025, 0.013503, 0.015210, 0.018254, 0.024434, 0.029592, 0.032864, 0.034041, 0.032678, 0.029036, 0.023646, 0.017199, 0.010881, 0.006951, 0.006127, 0.007923, 0.011906, 0.015910, 0.018932, 0.018132, 0.016999, 0.015891, 0.014898, 0.014078, 0.013939, 0.014605, 0.015512, 0.016854, 0.018462, 0.020650, 0.022848, 0.024320, 0.025032, 0.024807, 0.023726, 0.021877, 0.019271, 0.016462, 0.014218, 0.012567, 0.011493, 0.011611, 0.012689, 0.014631, 0.016466, 0.017862, 0.018714, 0.019125, 0.019124, 0.019181, 0.019235, 0.018943, 0.018838, 0.018847, 0.018763, 0.018476, 0.017796, 0.017045, 0.016368, 0.016410, 0.017198, 0.018492, 0.019825, 0.020725, 0.020337, 0.018871, 0.017454, 0.016122, 0.015136, 0.014472, 0.014193, 0.014764, 0.015956, 0.017713, 0.019264, 0.020318, 0.020519, 0.020172, 0.019901, 0.020062, 0.020449, 0.020648, 0.020169, 0.018802, 0.017136, 0.015708, 0.014513, 0.014105, 0.014478, 0.015483, 0.016726, 0.017904, 0.018941, 0.018790, 0.017226, 0.014747, 0.012334, 0.011311, 0.012441, 0.015400, 0.018910, 0.022066, 0.023788, 0.023538, 0.021887, 0.019849, 0.018771, 0.018438, 0.019187, 0.020773, 0.022595, 0.023709, 0.023381, 0.021507, 0.018989, 0.016635, 0.014942, 0.013923, 0.013551, 0.013299, 0.012789, 0.011713, 0.010426, 0.009423, 0.009040, 0.009341, 0.010671, 0.013504, 0.016886, 0.019926, 0.022520, 0.026741, 0.029339, 0.031778, 0.033037, 0.032611, 0.030037, 0.025920, 0.021005, 0.015918, 0.011851, 0.009511, 0.008467, 0.008458, 0.009120, 0.010389, 0.009856, 0.010775, 0.011325, 0.012285, 0.013626, 0.015369, 0.017098, 0.018800, 0.020654, 0.022396, 0.023923, 0.025254, 0.026666, 0.027501, 0.027261, 0.026064, 0.023833, 0.020754, 0.017602, 0.014990, 0.013142, 0.012060, 0.011676, 0.012037, 0.013122, 0.014270, 0.014845, 0.014648, 0.014520, 0.014735, 0.014853, 0.014755, 0.014824, 0.015274, 0.016233, 0.017876, 0.020157, 0.022748, 0.025102, 0.026563, 0.026898, 0.026205, 0.024746, 0.022284, 0.019200, 0.016181, 0.013972, 0.012722, 0.011972, 0.011820, 0.012074, 0.012591, 0.013342, 0.014436, 0.015581, 0.016326, 0.016769, 0.016981, 0.017287, 0.017726, 0.018437, 0.019197, 0.019933, 0.021007, 0.022533, 0.024450, 0.025642, 0.025258, 0.023231, 0.020499, 0.018130, 0.016229, 0.014539, 0.013022, 0.011852, 0.010947, 0.010355, 0.010580, 0.011048, 0.011388, 0.012335, 0.014525, 0.017732, 0.021240, 0.024155, 0.025694, 0.025533, 0.024538, 0.023618, 0.023077, 0.022831, 0.022559, 0.021674, 0.020505, 0.019318, 0.017595, 0.015568, 0.013796, 0.012326, 0.011357, 0.011018, 0.011449, 0.012359, 0.013572, 0.015153, 0.016895, 0.018248, 0.018673, 0.017475, 0.015110, 0.012251, 0.010090, 0.009927, 0.012829, 0.019557, 0.025869, 0.030936, 0.034237, 0.035149, 0.032969, 0.027872, 0.021086, 0.014305, 0.009495, 0.007208, 0.007493, 0.009786, 0.012885, 0.015666, 0.016257, 0.017645, 0.019502, 0.021292, 0.022121, 0.021912, 0.020813, 0.019387, 0.017955, 0.016823, 0.015954, 0.015328, 0.014701, 0.014140, 0.013824, 0.013985, 0.014678, 0.015954, 0.017474, 0.019227, 0.020738, 0.021744, 0.021919, 0.021331, 0.019913, 0.018173, 0.016550, 0.015717, 0.015921, 0.016892, 0.018137, 0.019265, 0.019984, 0.020134, 0.019603, 0.018521, 0.017446, 0.016678, 0.015954, 0.015351, 0.014902, 0.014774, 0.014795, 0.014874, 0.015070, 0.015794, 0.016863, 0.018087, 0.019327, 0.020353, 0.020853, 0.020492, 0.019446, 0.018139, 0.017029, 0.016949, 0.018049, 0.019968, 0.022049, 0.023665, 0.024180, 0.023522, 0.021856, 0.019099, 0.015840, 0.012905, 0.010702, 0.009527, 0.009547, 0.010461, 0.011604, 0.012905, 0.014265, 0.015736, 0.017309, 0.019355, 0.021565, 0.023341, 0.024471, 0.024655, 0.024460, 0.023948, 0.022983, 0.021638, 0.019999, 0.018147, 0.016045, 0.014355, 0.013136, 0.012616, 0.012461, 0.012839, 0.013687, 0.015128, 0.016914, 0.018200, 0.018741, 0.018606, 0.018158, 0.017633, 0.016982, 0.016552, 0.016246, 0.016413, 0.017160, 0.018638, 0.020638, 0.022649, 0.023763, 0.023479, 0.021840, 0.019454, 0.017487, 0.017297, 0.017688, 0.018580, 0.019469, 0.019985, 0.020022, 0.019113, 0.016550, 0.012710, 0.008883, 0.006631, 0.006942, 0.009724, 0.014360, 0.019115, 0.021815, 0.023325, 0.023659, 0.022898, 0.021343, 0.019355, 0.017720, 0.017418, 0.018465, 0.020265, 0.022094, 0.023259, 0.023421, 0.022603, 0.021030, 0.018981, 0.016583, 0.014277, 0.012215, 0.010581, 0.009314, 0.008839, 0.009292, 0.010809, 0.013253, 0.016237, 0.019256, 0.021526, 0.022208, 0.021473, 0.020084, 0.019124, 0.019259, 0.020727};
    int n = 682;
    vector<double> temporal_mean;
    for (int i = 0; i < n; ++i)
        temporal_mean.push_back(input[i]);
    hrDebug debug;
    vector<int> positions = hb_counter_pda(temporal_mean, 30, 90, 300, 0, 9, 0, debug);
    
//    int m = (int)positions.size();
//    printf("size = %d\n", m);
//    for (int i = 0; i < m; ++i)
//        printf("%d, %d\n", i, positions[i]);
    
    vector<double> ans;
    hr_calculator(positions, 30, ans);
    
    
    int m = (int)ans.size();
    printf("size = %d\n", m);
    for (int i = 0; i < m; ++i)
        printf("%d, %lf\n", i, ans[i]);
    
    
//    FILE *file = fopen(String(resourcePath + "hb_counter_pda_test.in").c_str(), "r");
//    int n = readInt(file);
//    vector<double> temporal_mean;
//    for (int i = 0; i < n; ++i) {
//        double x = readDouble(file);
//        temporal_mean.push_back(x);
//    }
//    double frameRate = readDouble(file);
//    int firstSample = readInt(file);
//    int window_size = readInt(file);
//    double overlap_ratio = readDouble(file);
//    double minPeakDistance = readDouble(file);
//    double threshold = readDouble(file);
//    fclose(file);
//    
//    hrDebug debug;
//    vector<int> output = hb_counter_pda(temporal_mean, frameRate, firstSample, window_size,
//                                        overlap_ratio, minPeakDistance, threshold, debug);
//    
//    file = fopen(String(resourcePath + "hb_counter_pda_test.out").c_str(), "r");
//    n = (int)output.size();
//    int m = readInt(file);
//    if (m != n) {
//        XCTFail(@"wrong output.size() - expected: %d, found: %d", m, n);
//        return;
//    }
//    
//    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
//    for (int i = 0; i < n; ++i)
//    {
//        int correct_ans = readInt(file);
//        int ans = output[i]+1;
//        double tmp = diff_percent(ans, correct_ans);
//        if (tmp > max_percent)
//        {
//            max_percent = tmp;
//            max_correct_ans = correct_ans;
//            max_ans = ans;
//        }
//    }
//    if (max_percent > EPSILON_PERCENT)
//        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
//    ////////////////////////////////////////////////////////////////////////////////////////////////////
//    n = (int)debug.heartBeats.size();
//    m = readInt(file);
//    if (m != n) {
//        XCTFail(@"wrong heartBeats.size() - expected: %d, found: %d", m, n);
//        return;
//    }
//    max_percent = 0;
//    sort(debug.heartBeats.begin(), debug.heartBeats.end());
//    for (int i = 0; i < n; ++i)
//    {
//        double a = readDouble(file);
//        int b = readInt(file);
//        double fr = debug.heartBeats[n-1-i].first;
//        int sc = debug.heartBeats[n-1-i].second + 1;
//        printf("(%lf, %d) ----- (%lf, %d)\n", a, b, fr, sc);
//        double tmp = max(diff_percent(fr, a),
//                         diff_percent(sc, b));
//        max_percent = max(max_percent, tmp);
//    }
//    if (max_percent > EPSILON_PERCENT)
//        XCTFail(@"wrong output - percent = %lf", max_percent);
//    ////////////////////////////////////////////////////////////////////////////////////////////////////
//    n = (int)debug.heartRates.size();
//    m = readInt(file);
//    if (m != n) {
//        XCTFail(@"wrong heartRates.size() - expected: %d, found: %d", m, n);
//        return;
//    }
//    max_percent = 0;
//    for (int i = 0; i < n; ++i)
//    {
//        double correct_ans = readDouble(file);
//        double ans = debug.heartRates[i];
//        if (abs(correct_ans - 0) < 1e-9 && abs(ans - 0) < 1e-9)
//            continue;
////        printf("debug.heartRates[%d] = %lf, correct_ans = %lf\n", i, ans, correct_ans);
//        double tmp = diff_percent(ans, correct_ans);
//        if (tmp > max_percent)
//        {
//            max_percent = tmp;
//            max_correct_ans = correct_ans;
//            max_ans = ans;
//        }
//    }
//    if (max_percent > EPSILON_PERCENT)
//        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
//    fclose(file);
}


- (void)test_hr_signal_calc
{
    FILE *file = fopen(String(resourcePath + "hr_signal_calc_test.in").c_str(), "r");
    int n = readInt(file);
    vector<double> temporal_mean;
    for (int i = 0; i < n; ++i)
        temporal_mean.push_back(readDouble(file));
    int firstSample = readInt(file);
    int window_size = readInt(file);
    double frameRate = readDouble(file);
    double overlap_ratio = readDouble(file);
    double max_bpm = readDouble(file);
    double threshold_fraction = readDouble(file);
    fclose(file);
    
    hrResult output = hr_signal_calc(temporal_mean, firstSample, window_size, frameRate,
                                     overlap_ratio, max_bpm, threshold_fraction);
    
    printf("avg_hr_autocorr = %lf, avg_hr_pda = %lf\n", output.autocorr, output.pda);
    
//    file = fopen(String(resourcePath + "hr_signal_calc_test.out").c_str(), "r");
//    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
//    for (int i = 0; i < 2; ++i)
//    {
//        double correct_ans = readDouble(file);
//        double ans = output[i];
//        double tmp = diff_percent(ans, correct_ans);
//        if (tmp > max_percent)
//        {
//            max_percent = tmp;
//            max_correct_ans = correct_ans;
//            max_ans = ans;
//        }
//    }
//    fclose(file);
//    if (max_percent > EPSILON_PERCENT)
//        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", max_correct_ans, max_ans, max_percent);
}


- (void)test_amplify
{
    int n = 10;
    double a[] = {
        0.1622,    0.4505,    0.1067,    0.4314,    0.8530,    0.4173,    0.7803,    0.2348,    0.5470,    0.9294,
        0.7943,    0.0838,    0.9619,    0.9106,    0.6221,    0.0497,    0.3897,    0.3532,    0.2963,    0.7757,
        0.3112,    0.2290,    0.0046,    0.1818,    0.3510,    0.9027,    0.2417,    0.8212,    0.7447,    0.4868
    };
    
    
    // amplify
    double alpha = 50;
    double chromAttenuation = 1;
    Mat base_B = (Mat_<double>(3, 3) <<
                  alpha, 0, 0,
                  0, alpha*chromAttenuation, 0,
                  0, 0, alpha*chromAttenuation);
    Mat base_C = (ntsc2rgb_baseMat * base_B) * rgb2ntsc_baseMat;
    Mat tmp = Mat::zeros(3, n, CV_64F);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < n; ++j)
            tmp.at<double>(i ,j) = a[i*n + j];
    tmp = base_C * tmp;
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < n; ++j)
            printf("%lf, ", tmp.at<double>(i ,j));
        printf("\n");
    }
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
    
    NSString *inPath = [[NSBundle mainBundle] bundlePath];
    NSLog(@"input path = %@", inPath);
    NSArray *directoryContent = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:inPath error:NULL];
    for (NSString *fileName in directoryContent) {
        if ([fileName hasSuffix:@".mp4"]) {
            NSLog(@"File %@", fileName);
            
            usleep(1000); //Delay the operation a bit to allow garbage collector to clear the memory
            
//            MHR::hrResult result = MHR::run_algorithms([inPath UTF8String], [fileName UTF8String], [outPath UTF8String]);
//            fprintf(outFile, "%s, %lf, %lf\n", [fileName UTF8String], result.autocorr, result.pda);
        }
    }
    
    fclose(outFile);
}

@end
