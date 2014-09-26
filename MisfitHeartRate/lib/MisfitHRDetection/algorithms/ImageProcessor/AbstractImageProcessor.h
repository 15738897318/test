//
//  AbstractImageProcessor.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/25/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef __MIsfitHRDetection__AbstractImageProcessor__
#define __MIsfitHRDetection__AbstractImageProcessor__

#include <stdint.h>
#include "TemporalArray.h"
#include "config.h"

class AbstractImageProcessor{
protected:
    char *srcDir;
    char *dstDir;
    bool _DEBUG;
public:
    AbstractImageProcessor();
    ~AbstractImageProcessor();
    void setSrcDir(const char *dir);
    void setDstDir(const char *dir);
    virtual int readFrameInfo() = 0;
    virtual void readFrames() = 0;
//    virtual void writeFrames(uint8_t numFrame, uint16_t offset) = 0;
//    virtual void setArrayInfo(TemporalArray &arr) = 0;
    virtual void writeArray(vector<double> &arr) = 0;
};

#endif /* defined(__MIsfitHRDetection__AbstractImageProcessor__) */
