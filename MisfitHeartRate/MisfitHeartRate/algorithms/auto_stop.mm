#import "auto_stop.hpp"

using namespace std;
using namespace cv;


@implementation auto_stop
    + (BOOL)faceCheck:(Mat)tmp {        
        Vec3f avgVal(0, 0, 0);
        for (int x = 0; x < tmp.cols; ++x)
            for (int y = 0; y < tmp.rows; ++ y)
            {
                avgVal[0] += tmp.at<Vec3b>(y, x)[0];
                avgVal[1] += tmp.at<Vec3b>(y, x)[1];
                avgVal[2] += tmp.at<Vec3b>(y, x)[2];
            }
        avgVal /= tmp.cols * tmp.rows;

        
        Vec3f diff(0, 0, 0);
        for (int x = 0; x < firstFrameWithFace.cols; ++x)
            for (int y = 0; y < firstFrameWithFace.rows; ++ y)
            {
                diff[0] += firstFrameWithFace.at<Vec3b>(y, x)[0];
                diff[1] += firstFrameWithFace.at<Vec3b>(y, x)[1];
                diff[2] += firstFrameWithFace.at<Vec3b>(y, x)[2];
            }
        diff /= firstFrameWithFace.cols * firstFrameWithFace.rows;
        
        absdiff(avgVal, diff, diff);
        
        if (diff[0] < faceDetectionThreshold && diff[1] < faceDetectionThreshold && diff[2] < faceDetectionThreshold)
            return YES;
        
        return NO;
    }

    + (BOOL)fingerCheck:(Mat)frame {
        return [auto_start isRedColored:frame];
    }

@end