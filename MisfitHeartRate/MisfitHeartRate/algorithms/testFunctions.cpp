//
//  testFunctions.cpp
//  MisfitHeartRate
//
//  Created by Bao Nguyen on 7/15/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "testFunctions.h"


namespace MHR {
    void testMathFunctions()
    {
//        int sizeArr[] = {30, 40, 100};
//        Mat img = Mat(3, sizeArr, CV_64F, CvScalar(0));
//        double v = 0, delta = 1;
//        for(int i=0; i<100; ++i){
//            if(v==10){
//                delta=-1;
//            }else if(v==-10){
//                delta=1;
//            }
//            v+=delta;
//            for(int x=0; x<30; ++x) for(int y=0; y<40; ++y) img.at<double>(x,y,i)=v;
//        }
//        Mat debugImg;
//        vector<double> ans = frames2signal(img, "mode-balance", 30, 0, debugImg);
//        for(int i=0; i<(int) ans.size(); ++i) cout<<ans[i]<<' '; cout<<endl;


//        vector<double> arr;
//        for(int i=0; i<100; ++i){
//        arr.push_back(1.0*(rand()%10000)/(rand()%100));
//        }
//        cout<<"[";
//        for(int i=0; i<100-1;++i) cout<<arr[i]<<", "; cout<<arr[99]<<"]"<<endl;
//        arr=low_pass_filter(arr);
//        cout<<"[";
//        for(int i=0; i<100-1;++i) cout<<arr[i]<<", "; cout<<arr[99]<<"]"<<endl;


//        vector<double>seg1{1, 7, 3, 89, 5, 16, 5};
//        vector<double> seg2{0.1, 1.38, 0.76};
//        vector<double> ans = conv(seg1,seg2);
//        cout<<(int)ans.size()<<endl;
//        for(int i=0; i<(int)ans.size(); ++i) cout<<ans[i]<<' '; cout<<endl;


//        vector<double> arr;
//        srand(time(NULL));
//        for(int i=1; i<=100; ++i) arr.push_back(i);
//        for(int i=1; i<=100; ++i){
//        double per=(rand()%10001)/100.0;
//        cout<<i<<' '<<per<<' '<<prctile(arr, per)<<' '<<invprctile(arr, prctile(arr,per))<<endl;
//        }


//        invprctile checking
//        vector<double> arr {5,1,3,2.2,3.1,5.6,10};
//        cout<<invprctile(arr, 10)<<endl;
//        cout<<invprctile(arr, 0)<<endl;
//        cout<<invprctile(arr, 1)<<endl;


//        srand(time(NULL));
//        vector<double> data {0,2,9,2,5,8,7,3,1,9,4,3,5,8,10,0,1,2,9,5,10};
//        vector<int> counts;
//        vector<double> centers;
//        hist(data, 10, counts, centers);
//        cout<<counts.size()<<endl;
//        for(int i=0; i<(int) counts.size(); ++i) cout<<centers[i]<<' '<<counts[i]<<endl;


//        vector<double> segment;
//        for(double x=0; x<=100; x+=0.01)
//            segment.push_back(sin(x*acos(-1)));
//        printf("input: \n");
//        for (int i = 0; i < (int)segment.size(); ++i)
//            printf("%lf\n", segment[i]);
//
//        vector<double> strengths;
//        vector<int> locs;
//        findpeaks(segment, 300, 0, strengths, locs);
//        printf("%d\n",(int)strengths.size());
//
//        for(int i=0; i<strengths.size(); ++i)
//            printf("%lf %d\n",strengths[i], locs[i]);
    }

    
    
//    void test_rgb2ntsc() {
//        int nRow = 10, nCol = 10;
//        double test_array_0[] = {
//            0.8147, 0.1576, 0.6557, 0.7060, 0.4387, 0.2760, 0.7513, 0.8407, 0.3517, 0.0759,
//            0.9058, 0.9706, 0.0357, 0.0318, 0.3816, 0.6797, 0.2551, 0.2543, 0.8308, 0.0540,
//            0.1270, 0.9572, 0.8491, 0.2769, 0.7655, 0.6551, 0.5060, 0.8143, 0.5853, 0.5308,
//            0.9134, 0.4854, 0.9340, 0.0462, 0.7952, 0.1626, 0.6991, 0.2435, 0.5497, 0.7792,
//            0.6324, 0.8003, 0.6787, 0.0971, 0.1869, 0.1190, 0.8909, 0.9293, 0.9172, 0.9340,
//            0.0975, 0.1419, 0.7577, 0.8235, 0.4898, 0.4984, 0.9593, 0.3500, 0.2858, 0.1299,
//            0.2785, 0.4218, 0.7431, 0.6948, 0.4456, 0.9597, 0.5472, 0.1966, 0.7572, 0.5688,
//            0.5469, 0.9157, 0.3922, 0.3171, 0.6463, 0.3404, 0.1386, 0.2511, 0.7537, 0.4694,
//            0.9575, 0.7922, 0.6555, 0.9502, 0.7094, 0.5853, 0.1493, 0.6160, 0.3804, 0.0119,
//            0.9649, 0.9595, 0.1712, 0.0344, 0.7547, 0.2238, 0.2575, 0.4733, 0.5678, 0.3371};
//
//        double test_array_1[] = {
//            0.1622, 0.4505, 0.1067, 0.4314, 0.8530, 0.4173, 0.7803, 0.2348, 0.5470, 0.9294,
//            0.7943, 0.0838, 0.9619, 0.9106, 0.6221, 0.0497, 0.3897, 0.3532, 0.2963, 0.7757,
//            0.3112, 0.2290, 0.0046, 0.1818, 0.3510, 0.9027, 0.2417, 0.8212, 0.7447, 0.4868,
//            0.5285, 0.9133, 0.7749, 0.2638, 0.5132, 0.9448, 0.4039, 0.0154, 0.1890, 0.4359,
//            0.1656, 0.1524, 0.8173, 0.1455, 0.4018, 0.4909, 0.0965, 0.0430, 0.6868, 0.4468,
//            0.6020, 0.8258, 0.8687, 0.1361, 0.0760, 0.4893, 0.1320, 0.1690, 0.1835, 0.3063,
//            0.2630, 0.5383, 0.0844, 0.8693, 0.2399, 0.3377, 0.9421, 0.6491, 0.3685, 0.5085,
//            0.6541, 0.9961, 0.3998, 0.5797, 0.1233, 0.9001, 0.9561, 0.7317, 0.6256, 0.5108,
//            0.6892, 0.0782, 0.2599, 0.5499, 0.1839, 0.3692, 0.5752, 0.6477, 0.7802, 0.8176,
//            0.7482, 0.4427, 0.8001, 0.1450, 0.2400, 0.1112, 0.0598, 0.4509, 0.0811, 0.7948};
//
//        double test_array_2[] = {
//            0.6443, 0.2077, 0.3111, 0.5949, 0.0855, 0.9631, 0.0377, 0.1068, 0.0305, 0.1829,
//            0.3786, 0.3012, 0.9234, 0.2622, 0.2625, 0.5468, 0.8852, 0.6538, 0.7441, 0.2399,
//            0.8116, 0.4709, 0.4302, 0.6028, 0.8010, 0.5211, 0.9133, 0.4942, 0.5000, 0.8865,
//            0.5328, 0.2305, 0.1848, 0.7112, 0.0292, 0.2316, 0.7962, 0.7791, 0.4799, 0.0287,
//            0.3507, 0.8443, 0.9049, 0.2217, 0.9289, 0.4889, 0.0987, 0.7150, 0.9047, 0.4899,
//            0.9390, 0.1948, 0.9797, 0.1174, 0.7303, 0.6241, 0.2619, 0.9037, 0.6099, 0.1679,
//            0.8759, 0.2259, 0.4389, 0.2967, 0.4886, 0.6791, 0.3354, 0.8909, 0.6177, 0.9787,
//            0.5502, 0.1707, 0.1111, 0.3188, 0.5785, 0.3955, 0.6797, 0.3342, 0.8594, 0.7127,
//            0.6225, 0.2277, 0.2581, 0.4242, 0.2373, 0.3674, 0.1366, 0.6987, 0.8055, 0.5005,
//            0.5870, 0.4357, 0.4087, 0.5079, 0.4588, 0.9880, 0.7212, 0.1978, 0.5767, 0.4711};
//
//        Mat input = Mat::zeros(nRow, nCol, CV_64FC3);
//        for (int i = 0; i < nRow; ++i)
//            for (int j = 0; j < nCol; ++j) {
//                input.at<Vec3d>(i, j)[0] = test_array_0[i*nCol + j];
//                input.at<Vec3d>(i, j)[1] = test_array_1[i*nCol + j];
//                input.at<Vec3d>(i, j)[2] = test_array_2[i*nCol + j];
//            }
//        printf("Input mat:\n");
//        for (int channel = 0; channel < 3; ++channel) {
//            for (int i = 0; i < input.rows; ++i) {
//                for (int j = 0; j < input.cols; ++j)
//                    printf("%lf, ", input.at<Vec3d>(i, j)[channel]);
//                printf("\n");
//            }
//            printf("\n\n\n\n\n");
//        }
//
//        Mat output;
//        rgb2ntsc(input, output);
//        printf("Output mat:\n");
//        for (int channel = 0; channel < 3; ++channel) {
//            for (int i = 0; i < output.rows; ++i) {
//                for (int j = 0; j < output.cols; ++j)
//                    printf("%lf, ", output.at<Vec3d>(i, j)[channel]);
//                printf("\n");
//            }
//            printf("\n\n\n\n\n");
//        }
//
//        ntsc2rgb(output, input);
//        printf("NTSC mat:\n\n");
//        for (int channel = 0; channel < 3; ++channel) {
//            for (int i = 0; i < input.rows; ++i) {
//                for (int j = 0; j < input.cols; ++j)
//                    printf("%lf, ", input.at<Vec3d>(i, j)[channel]);
//                printf("\n");
//            }
//            printf("\n\n\n\n\n");
//        }
//    }

    
    void test_function(const vector<int> &src, vector<int> &dst) {
        if (&src == &dst)
            printf("&src == &dst\n");
        dst.clear();
        for (int i = 0; i < (int)src.size(); ++i)
            dst.push_back(src[i]*src[i]);
    }
    
    
    void test_openCV()
    {
        printf("test_openCV()\n");
        int n = 100;
//        int m = 200;
//        int array_size[] = {100, 200, 10};
//        Mat a = Mat(3, array_size, CV_64FC3, cv::Scalar(0));
//        Mat b = Mat(3, array_size, CV_32FC3, cv::Scalar(0));
//        Mat a = Mat::ones(n, m, CV_64FC3);
//        Mat b = Mat::ones(n, m, CV_32FC3);
//        a.convertTo(b, CV_32FC3);
//        b.convertTo(b, CV_64FC3);
        
        vector<int> a;
        vector<int> b;
        for (int i = 0; i < n; ++i)
            a.push_back(i);
        test_function(a, a);
        for (int i = 0; i < (int)a.size(); ++i)
            printf("%d, ", a[i]);
//        test_function(a, b);
//        for (int i = 0; i < (int)b.size(); ++i)
//            printf("%d, ", b[i]);
        printf("\n");
    }
}