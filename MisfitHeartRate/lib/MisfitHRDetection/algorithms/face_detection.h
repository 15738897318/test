//
//  FaceDetection.h
//  MIsfitHRDetection
//
//  Created by Nguyen Tien Viet on 10/1/14.
//  Copyright (c) 2014 Nguyen Tien Viet. All rights reserved.
//

#ifndef MIsfitHRDetection_FaceDetection_h
#define MIsfitHRDetection_FaceDetection_h

namespace MHR {
    /*---------------for face auto start--------------*/
    const int _THRESHOLD_NO_FACE_FRAMES_MIN = 2;
    const int _THRESHOLD_FACE_FRAMES_MIN = 2;
    const int _THRESHOLD_FACE_FRAMES_FOR_START = 5;
    
    const float _ROI_RATIO_UPPER = 1.5f;
    const float _ROI_RATIO_LOWER = 0.8f;
}
#endif
