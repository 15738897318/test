//
//  files.cpp
//  Pulsar
//
//  Created by Bao Nguyen on 7/28/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#include "files.h"


namespace MHR {
    Mat read2DMatFromFile(FILE* &file, int rows, int cols)
    {
        Mat ans = Mat::zeros(rows, cols, CV_64F);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                fscanf(file, "%lf", &ans.at<double>(i, j));
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
    
    
    vector<double> readVectorFromFile(FILE* &file, int n)
    {
        vector<double> ans;
        double value;
        for (int i = 0; i < n; ++i) {
            fscanf(file, "%lf", &value);
            ans.push_back(value);
        }
        return ans;
    }

    
    void writeVector(const vector<double>& src, const String& outFile, bool append)
    {
        FILE *file;
        if (append) file = fopen(outFile.c_str(), "a");
        else file = fopen(outFile.c_str(), "w");
        if (file == NULL) return;
        int n = (int)src.size();
        fprintf(file, "\n");
        fprintf(file, "size = %d\n", n);
        for (int i = 0; i < n; ++i)
            fprintf(file, "%lf, ", src[i]);
        fprintf(file, "\n");
        fclose(file);
    }
}