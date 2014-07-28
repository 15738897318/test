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

const double EPSILON = 1e-4;
const double EPSILON_PERCENT = 2.8;
String resourcePath = "/Users/baonguyen/Library/Application Support/iPhone Simulator/7.1-64/Applications/7CD329BE-D62D-4B46-BBCB-7512C37724D4/Pulsar.app/";


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
    
    
    ///////////
//    for (int col = 0; col < nCol; ++col) {
//        for (int i = 0; i < nFrame; ++i) {
//            for (int row = 0; row < nRow; ++row)
//                printf("%lf ", monoframes[i].at<double>(row, col));
//            printf("\n");
//        }
//        printf("\n\n\n");
//    }
    //////////
    
    
    double fr = 1, cutoff_freq = 0;
    double lower_range, upper_range;
    vector<double> output = frames2signal(monoframes, "mode-balance", fr, cutoff_freq, lower_range, upper_range, true);
    output = low_pass_filter(output);
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
//    vector<double> input;
//    for(double x=0; x<=100; x+=0.01)
//        input.push_back(sin(x*acos(-1)));
//    
//    int length = (int)input.size();
//    printf("input.size() = %d\n", length);
//    for (int i = 0; i < length; ++i)
//        printf("%lf, ", input[i]);
//    printf("\n\n");
//
//    vector<double> strengths;
//    vector<int> locs;
//    findpeaks(input, 0, 0, strengths, locs);
//    printf("strengths.size() = %d\n",(int)strengths.size());
//
//    hrDebug debug;
//    vector<int> locations = hb_counter_autocorr(input, 30.0, 0, 100, 0.0, 0.0, debug);
//    printf("hb_counter_autocorr locations:\n");
//    for (int i = 0; i < (int)locations.size(); ++i)
//        printf("%d\n", locations[i]);
//    printf("\n\n");
//    printf("debug.heartBeats.size() = %d\n", (int)debug.heartBeats.size());
//    printf("debug.heartRates.size() = %d\n", (int)debug.heartRates.size());
//    printf("debug.autocorrelation.size() = %d\n", (int)debug.autocorrelation.size());

    FILE *file = fopen(String(resourcePath + "hb_counter_autocorr_test.in").c_str(), "r");
    int n = readInt(file);
    vector<double> temporal_mean;
    for (int i = 0; i < n; ++i) {
        double x = readDouble(file);
        temporal_mean.push_back(x);
//        printf("%lf\n", x);
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
    n = output.size();
    
//    file = fopen(String(resourcePath + "hb_counter_autocorr_test.out").c_str(), "r");
//    int m = readInt(file);
//    if (m != n) {
//        XCTFail(@"wrong output.size() - expected: %d, found: %d", m, n);
//        fclose(file);
//        return;
//    }
//    
//    double max_percent = 0, max_correct_ans = 0, max_ans = 0;
//    for (int i = 0; i < n; ++i)
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


- (void)test_hb_counter_pda
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
    
//    vector<double> strengths;
//    vector<int> locs;
//    findpeaks(input, 0, 0, strengths, locs);
//    printf("strengths.size() = %d\n",(int)strengths.size());
//    
//    hrDebug debug;
//    double ans = hb_counter_pda(input, 30.0, 0, 120, 0.0, 0.0, 0, debug);
//    printf("hb_counter_pda = %lf\n", ans);
//    printf("debug.heartBeats.size() = %d\n", (int)debug.heartBeats.size());
//    printf("debug.heartRates.size() = %d\n", (int)debug.heartRates.size());
//    printf("debug.autocorrelation.size() = %d\n", (int)debug.autocorrelation.size());
//    
//    if (diff_percent(ans, correct_ans) > EPSILON_PERCENT)
//    {
//        XCTFail(@"wrong output - expected: %lf, found: %lf, percent = %lf", correct_ans, ans, diff_percent(ans, correct_ans));
//        return;
//    }

//        printf("\n\nautocorrelation:\n");
//        for (int i = 0; i < (int)debug.autocorrelation.size(); ++i)
//            printf("%lf, ", debug.autocorrelation[i]);
//        printf("\n\n");
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


@end
