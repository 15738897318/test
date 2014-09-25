//
//  AbstractImageProcessor.cpp
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 9/25/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#include "AbstractImageProcessor.h"

void AbstractImageProcessor::setSrcDir(const char *dir) {
    this -> srcDir = new char[strlen(dir) + 1];
    strcpy(this -> srcDir, dir);
}

void AbstractImageProcessor::setDstDir(const char *dir) {
    this -> dstDir = new char[strlen(dir) + 1];
    strcpy(this -> dstDir, dir);
}

AbstractImageProcessor::AbstractImageProcessor() {
    srcDir = NULL;
    dstDir = NULL;
    _DEBUG = DEBUG;
}

AbstractImageProcessor::~AbstractImageProcessor() {
    if (srcDir!= NULL)
        delete srcDir;
    if (dstDir!= NULL)
        delete dstDir;
}