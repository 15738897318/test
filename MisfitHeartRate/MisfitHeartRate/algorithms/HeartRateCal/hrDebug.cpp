//
//  hrDebug.cpp
//  MisfitHeartRate
//
//  Created by Thanh Le on 7/2/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#include "hrDebug.h"


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
//        for(double x=0; x<=100; x+=0.01) segment.push_back(sin(x*acos(-1)));
//        vector<double> strengths;
//        vector<int> locs;
//        findpeaks(segment, 300, 0, strengths, locs);
//        printf("%d\n",(int)strengths.size());
//        for(int i=0; i<strengths.size(); ++i)
//         printf("%lf %d\n",strengths[i], locs[i]);
    }
    
    
    void test_fft() {
        int n = 10;
        double test[] = {0.5377, 1.8339, -2.2588, 0.8622, 0.3188, -1.3077, -0.4336, 0.3426, 3.5784, 2.7694};
        vector<double> test_vector(test, test+n);
        Mat input = Mat(test_vector);
        Mat output;
        dft(input, output, cv::DFT_SCALE|cv::DFT_COMPLEX_OUTPUT);
        
        
        
        printf("Output mat:\n");
        printf("rows = %i, cols = %i\n", output.rows, output.cols);
        for (int i = 0; i < output.rows; ++i) {
            for (int j = 0; j < output.cols; ++j)
                printf("%lf, ", output.at<double>(i, j));
            printf("\n");
        }
    }
    
        
    void test_ideal_bandpassing() {
        int n = 100;
        double test[] = {0.5377, 1.8339, -2.2588, 0.8622, 0.3188, -1.3077, -0.4336, 0.3426, 3.5784, 2.7694, -1.3499, 3.0349, 0.7254, -0.0631, 0.7147, -0.2050, -0.1241, 1.4897, 1.4090, 1.4172, 0.6715, -1.2075, 0.7172, 1.6302, 0.4889, 1.0347, 0.7269, -0.3034, 0.2939, -0.7873, 0.8884, -1.1471, -1.0689, -0.8095, -2.9443, 1.4384, 0.3252, -0.7549, 1.3703, -1.7115, -0.1022, -0.2414, 0.3192, 0.3129, -0.8649, -0.0301, -0.1649, 0.6277, 1.0933, 1.1093, -0.8637, 0.0774, -1.2141, -1.1135, -0.0068, 1.5326, -0.7697, 0.3714, -0.2256, 1.1174, -1.0891, 0.0326, 0.5525, 1.1006, 1.5442, 0.0859, -1.4916, -0.7423, -1.0616, 2.3505, -0.6156, 0.7481, -0.1924, 0.8886, -0.7648, -1.4023, -1.4224, 0.4882, -0.1774, -0.1961, 1.4193, 0.2916, 0.1978, 1.5877, -0.8045, 0.6966, 0.8351, -0.2437, 0.2157, -1.1658, -1.1480, 0.1049, 0.7223, 2.5855, -0.6669, 0.1873, -0.0825, -1.9330, -0.4390, -1.7947};
        Mat input = Mat::zeros(n, 1, CV_64F);
        for (int i = 0; i < n; ++i)
            input.at<double>(i, 0) = test[i];
        Mat output;
        ideal_bandpassing(input, output, 0.5, 0.7, 10);
        
        printf("Output mat:\n");
        printf("rows = %i, cols = %i\n", output.rows, output.cols);
        for (int i = 0; i < output.rows; ++i) {
            for (int j = 0; j < output.cols; ++j)
                printf("%lf, ", output.at<double>(i, j));
            printf("\n");
        }
    }
}