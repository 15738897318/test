//
//  files.cpp
//  Pulsar
//
//  Created by Bao Nguyen on 7/28/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#include "files.h"


namespace MHR
{
    void readFrame(const String& srcFile, vector<Mat> &dst)
    {
        Mat frame = imread(srcFile);
        cvtColor(frame, frame, CV_BGR2RGB);
        
        if (_THREE_CHAN_MODE)
            dst.push_back(frame.clone());
        else
        {
            /*-----------------if using 1-chan mode, then do the colour conversion here (0)-----------------*/
            frame.convertTo(frame, CV_64FC3);
            if (_colourspace == "hsv")
                cvtColor(frame, frame, CV_RGB2HSV);
            else if (_colourspace == "ycbcr")
                cvtColor(frame, frame, CV_RGB2YCrCb);
            else if (_colourspace == "tsl")
                rgb2tsl(frame, frame);
            
            Mat tmp = Mat::zeros(frame.rows, frame.cols, CV_64F);
            for (int i = 0; i < frame.rows; ++i)
                for (int j = 0; j < frame.cols; ++j)
                    tmp.at<double>(i, j) = frame.at<Vec3d>(i, j)[_channels_to_process];
            dst.push_back(tmp.clone());
        }
    }
    
    
    Mat read2DMatFromFile(FILE* &file, int rows, int cols)
    {
        Mat ans = Mat::zeros(rows, cols, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                fscanf(file, "%lf", &ans.at<double>(i, j));
        return ans;
    }
    
    
    vector<double> readVectorFromFile(FILE* &file, int n)
    {
        vector<double> ans;
        double value;
        for (int i = 0; i < n; ++i)
        {
            fscanf(file, "%lf", &value);
            ans.push_back(value);
        }
        return ans;
    }
    
    
    int readInt(FILE* &file)
    {
        int value;
        fscanf(file, "%d", &value);
        return value;
    }
    
    
    double readDouble(FILE* &file)
    {
        double value;
        fscanf(file, "%lf", &value);
        return value;
    }
    
    
    void writeVector(const vector<double>& src, const String& outFile, bool append)
    {
        FILE *file;
        if (append) file = fopen(outFile.c_str(), "a");
        else file = fopen(outFile.c_str(), "w");
        if (file == NULL) return;
        printf("Write vector to file %s\n", outFile.c_str());

        int n = (int)src.size();
        fprintf(file, "\n");
        fprintf(file, "size = %d\n", n);
        for (int i = 0; i < n; ++i)
            fprintf(file, "%lf, ", src[i]);
        fprintf(file, "\n");
        fclose(file);
    }
    
    
    // print a frame to file
    bool frameToFile(const Mat& frame, const String& outFile)
    {
        return imwrite(outFile, frame);
    }
    
    
    void frameChannelToFile(const Mat& frame, const String& outFile, int channel)
    {
        FILE *file = fopen(outFile.c_str(), "w");
        if (file == NULL) return;
        printf("Write frame[%d] to file %s\n", channel, outFile.c_str());
        
        for (int i = 0; i < frame.rows; ++i)
        {
            for (int j = 0; j < frame.cols; ++j)
                if (_THREE_CHAN_MODE)
                {
                    if (frame.type() == CV_8UC4)
                        fprintf(file, "%d, ", (int)frame.at<Vec4b>(i, j)[channel]);
                    if (frame.type() == CV_8UC3)
                        fprintf(file, "%d, ", (int)frame.at<Vec3b>(i, j)[channel]);
                    else if (frame.type() == CV_64FC3)
                        fprintf(file, "%lf, ", frame.at<Vec3d>(i, j)[channel]);
                }
                else
                {
                    if (frame.type() == CV_8U)
                        fprintf(file, "%d, ", (int)frame.at<unsigned char>(i, j));
                    else if (frame.type() == CV_64F)
                        fprintf(file, "%lf, ", frame.at<double>(i, j));

                }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}