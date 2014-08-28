//
//  processingPerBlock.cpp
//  Pulsar
//
//  Created by HaiPhan on 8/19/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#include "processingPerBlock.h"

namespace MHR {
    void processingPerBlock(const string &srcDir, const string &outDir,
                            int fileStartIndex, int fileEndIndex,
                            bool &isCalcMode, double &lower_range, double &upper_range,
                            hrResult &result, vector<double> &tmp)
    {
        // Read first frames
        int nFrames = fileEndIndex - fileStartIndex + 1;
        
        if (nFrames == 0)
        {
            result = hrResult(-1, -1);
            return;
        }
        
        
        /*-----------------read the first block of M frames to extract video params---------------*/
        int currentFrame = -1;
        vector<Mat> vid;
        for (int i = 0; i < min(_framesBlock_size, nFrames); ++i) {
            ++currentFrame;
            readFrame(srcDir + string("/input_frame[") + to_string(fileStartIndex + i) + "].png", vid);
        }
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Block 1: turn frames to signals
        vector<Mat> monoframes, debug_monoframes, eulerianVid;
        vector<double> temporal_mean;
        vector<double> temporal_mean_filt;
        Mat tmp_eulerianVid;

        
        /*-----------------run_eulerian(): M frames (1)-----------------*/
        eulerianGaussianPyramidMagnification(vid, eulerianVid,
                                             outDir, _eulerian_alpha, _eulerian_pyrLevel,
                                             _eulerian_minHR/60.0, _eulerian_maxHR/60.0,
                                             _eulerian_frameRate, _eulerian_chromaMagnifier);

        /*-----------------turn eulerianLen (1) frames to signals-----------------*/
        tmp = temporal_mean_calc(eulerianVid, _overlap_ratio, _max_bpm, _cutoff_freq,
                                                _channels_to_process, _colourspace,
                                                lower_range, upper_range, isCalcMode);
    }
}