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
    /**
     *read frames from a file to a a Mat vector
     */
    void readFrame(const String& srcFile, vector<Mat> &dst);

    
    /**
     *read a 2D Mat (\a rows * \a cols) from an opened file
     */
    Mat read2DMatFromFile(FILE* &file, int rows, int cols);

    
    /**
     *read an integer from an opened file
     */
    int readInt(FILE* &file);

    
    /**
     *read a double number from an opened file
     */
    double readDouble(FILE* &file);

    
    /**
     *read a vector<double> with \a n elements from an opened file
     */
    vector<double> readVectorFromFile(FILE* &file, int n);

    
    /**
     * write a vector<double> to a file. \n
     * \param append if == true, then the function will append the vector at the end of the file,
     * otherwise, it will overwrite the old file or create a new file
     */
    void writeVector(const vector<double>& src, const String& outFile, bool append = false);
}

#endif /* defined(__Pulsar__files__) */
