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
}