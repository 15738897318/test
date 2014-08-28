//
//  files.h
//  Pulsar
//
//  Created by Bao Nguyen on 7/28/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#ifndef __Pulsar__files__
#define __Pulsar__files__

#include "matrix.h"
#include "image.h"

namespace MHR {
    // read frames from a VideoCapture to a vector<Mat>
    // return true if endOfFile
    void readFrame(const String& srcFile, vector<Mat> &dst);
    
    Mat read2DMatFromFile(FILE* &file, int rows, int cols);
    
    vector<double> readVectorFromFile(FILE* &file, int n);
    
    int readInt(FILE* &file);
    
    double readDouble(FILE* &file);
    
    void writeVector(const vector<double>& src, const String& outFile, bool append = false);
    
    // print a frame to file
    bool frameToFile(const Mat& frame, const String& outFile);
    
    void frameChannelToFile(const Mat& frame, const String& outFile, int channel);
}

#endif /* defined(__Pulsar__files__) */
