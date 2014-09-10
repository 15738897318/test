#import "auto_stop.hpp"

using namespace std;
using namespace cv;


@implementation auto_stop
    + (BOOL)fastFaceCheck:(Mat)tmp {
        
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

    + (BOOL)slowFaceCheck:(cv::Mat)tmp {
        // Cut the frame down to the upper bound of ROI
        Mat frame_ROI = tmp;
        
        // Rotate the frame to fit the orientation preferred by the detector
        transpose(frame_ROI, frame_ROI);
        for(int i=0; i < frame_ROI.rows / 2; ++i) for(int j = 0; j < frame_ROI.cols * 4; ++j)
            swap(frame_ROI.at<unsigned char>(i, j), frame_ROI.at<unsigned char>(frame_ROI.rows - i - 1, j));
    
        // Face detection
        NSArray *faces = [auto_start detectFrontalFaces:&frame_ROI];

        
        // If this iteration detects valid faces
        // - Increment the framesWithFace variable
        
        for (CIFeature *face in faces)
//            if (face.bounds.origin.x + face.bounds.size.width / 2 >= searchArea.x && face.bounds.origin.x + face.bounds.size.width / 2 <= searchArea.x + searchArea.width
//                && face.bounds.origin.y + face.bounds.size.height / 2 >= searchArea.y && face.bounds.origin.y + face.bounds.size.height / 2 <= searchArea.y + searchArea.height)
        {
                cropArea = cv::Rect(face.bounds.origin.y, face.bounds.origin.x, IMAGE_WIDTH, IMAGE_HEIGHT);
                searchArea = cv::Rect(MAX(0, double(face.bounds.origin.y) - 0.25 * IMAGE_WIDTH), MAX(0, double(face.bounds.origin.x) - 0.25 * IMAGE_HEIGHT), 1.5 * IMAGE_WIDTH, 1.5 * IMAGE_HEIGHT);
//                int ROI_x;
//                int ROI_y;
//                int ROI_width;
//                int ROI_height;
//                
//                ROI_x = cropArea.x - (int)((double)cropArea.width * (_ROI_RATIO_UPPER - 1)) / 2;
//                ROI_y = cropArea.y - (int)((double)cropArea.height * (_ROI_RATIO_UPPER - 1)) / 2;
//                ROI_width = (int)((double)cropArea.width * _ROI_RATIO_UPPER);
//                ROI_height = (int)((double)cropArea.height * _ROI_RATIO_UPPER);
//                ROI_upper = cv::Rect(ROI_x, ROI_y, ROI_width, ROI_height);
//                
//                ROI_x = cropArea.x - (int)((double)cropArea.width * (_ROI_RATIO_LOWER - 1)) / 2;
//                ROI_y = cropArea.y - (int)((double)cropArea.height * (_ROI_RATIO_LOWER - 1)) / 2;
//                ROI_width = (int)((double)cropArea.width * _ROI_RATIO_LOWER);
//                ROI_height = (int)((double)cropArea.height * _ROI_RATIO_LOWER);
//                ROI_lower = cv::Rect(ROI_x, ROI_y, ROI_width, ROI_height);
                return YES;
            }
    
        return NO;
    }

    + (BOOL)fingerCheck:(Mat)frame {
        return [auto_start isRedColored:frame];
    }

@end