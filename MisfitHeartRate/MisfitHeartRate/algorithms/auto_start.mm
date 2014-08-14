#import "auto_start.hpp"

using namespace std;
using namespace cv;



@implementation auto_start


//    @synthesize videoCamera = _videoCamera;
//
//
//    /**Global variables */
//    String face_cascade_name = "haarcascade_frontalface_alt.xml";
//    CascadeClassifier face_cascade;
//
//    String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
//    CascadeClassifier eyes_cascade;
//
//    RNG rng(12345);
//
//
//    bool manualClick = false;
//
//    - (void) manualStartButtonClick
//    {
//        manualClick = true;
//    }
//
//    - (int) autoStartForFinger
//    {
//        // Sample the frames to make sure that the back camera is covered by the finger
//        // The frames should look black
//        
//        
//        // If so, then turn on the torch
//        //[MHRUtilities setTorchModeOn:YES];
//        
//        // If the frames acquired are red on average, then it is likely that it is covered with a finger
//        // So release the camera, and just start the real operation
//        
//        return 0;
//    }
//
//
//    /** @function autoStartForFace */
//    - (int) autoStartForFace:(cv::Rect)ROI_upper lowerBound:(cv::Rect)ROI_lower
//    {
//        CvCapture* capture;
//        Mat frame, frame_ROI;
//        
//        NSArray* faces;
//        
//        int assessmentResult;
//        int framesWithFace; // Count the number of frames having a face in the region of interest
//        int framesWithNoFace; // Count the number of frames NOT having a face in the region of interest
//        int startButtonStatus;
//        
//        
//        //-- 2. Read the video stream
//        _videoCamera = [[CvVideoCamera alloc] initWithParentView:self.imageView];
//        _videoCamera.delegate = self;
//        _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
//        _videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
//        _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
//        //    _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset640x480;
//        _videoCamera.defaultFPS = _frameRate;
//        _videoCamera.rotateVideo = YES;
//        _videoCamera.grayscaleMode = NO;
//        [_videoCamera start];
//        
//        
//        capture = cvCaptureFromCAM(0); //?????????????????????? Need to use a different method to initialise the camera. Try the delegate method in MainViewController
//        if (capture)
//        {
//            framesWithFace = 0; // Initialise the number of frames w faces
//            framesWithNoFace = 0; // Initialise the number of frames w/o faces
//            
//            while(true)
//            {
//                if(manualClick) return 0;
//                
//                startButtonStatus = 0; // While looping into a new iteration, startButton is obv not activated
//                
//                //frame = cvQueryFrame(capture);
//                IplImage *tmp = cvQueryFrame(capture);
//                frame = cvarrToMat(tmp);
//                
//                //-- 3. Apply the classifier to the frame
//                if (!frame.empty())
//                {
//                    // Cut the frame down to the upper bound of ROI
//                    frame_ROI = frame(ROI_upper);
//                    
//                    //detectFrontalFaces(frame_ROI, faces);
//                    
//                    faces = [self detectFrontalFaces:frame_ROI];
//                                    
//                    // If this iteration detects valid faces
//                    // - Increment the framesWithFace variable
//                    assessmentResult = [self assessFaces:faces withLowerBound:ROI_lower];
//                    if (assessmentResult == 1)
//                    {
//                        framesWithFace += 1;
//                    }
//                    else
//                    {
//                        framesWithNoFace += 1;
//                    }
//                        
//                    // If a face is not detected in N frames, then reset the face-detected streak
//                    if (framesWithNoFace > _THRESHOLD_NO_FACE_FRAMES_MIN)
//                    {
//                        framesWithFace = 0;
//                    }
//                    
//                    // If a face is detected in more than M frames, then reset the no-face streak
//                    if (framesWithFace > _THRESHOLD_FACE_FRAMES_MIN)
//                    {
//                        framesWithNoFace = 0;
//                    }
//                }
//                else
//                {
//                    printf(" --(!) No captured frame -- Break!");
//                    //break;
//                }
//                
//                if (framesWithFace > _THRESHOLD_FACE_FRAMES_FOR_START)
//                {
//                    // Kill the camera object to prepare for the capture routine called by the startButton
//                    cvReleaseCapture(&capture);
//                    
//                    // tap the startButton
//                    startButtonStatus = 1;
//                    
//                    break;
//                }
//            }
//            
//            if (startButtonStatus == 1)
//            {
//                return 1;
//            }
//        }
//        return 0;
//    }



    /** @function detectFrontalFaces */
    + (NSArray*) detectFrontalFaces:(cv::Mat*) frame
    {
        Mat frame_gray;

        cvtColor(*frame, frame_gray, CV_BGR2GRAY);
        equalizeHist(frame_gray, frame_gray);

        //-- Detect faces
        //face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
        
        CIContext *context = [CIContext contextWithOptions:nil];
        NSDictionary *opts = @{ CIDetectorAccuracy : CIDetectorAccuracyLow };
        CIDetector *detector = [CIDetector detectorOfType:CIDetectorTypeFace
                                                  context:context
                                                  options:opts];

        UIImage *uiImage = [self imageWithCVMat:frame_gray];
        CIImage *ciImage = [CIImage imageWithCGImage:uiImage.CGImage];
        opts = @{};
        NSArray *features = [detector featuresInImage:ciImage options:opts];
        NSLog(@"features = %d", features.count);
        
        frame_gray.deallocate();
        return features;
        
    }

    + (UIImage *)imageWithCVMat:(const cv::Mat&)cvMat
        {
            NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize() * cvMat.total()];
            
            CGColorSpaceRef colorSpace;
            
            if (cvMat.elemSize() == 1) {
                colorSpace = CGColorSpaceCreateDeviceGray();
            } else {
                colorSpace = CGColorSpaceCreateDeviceRGB();
            }
            
            CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
            
            CGImageRef imageRef = CGImageCreate(cvMat.cols,                                     // Width
                                                cvMat.rows,                                     // Height
                                                8,                                              // Bits per component
                                                8 * cvMat.elemSize(),                           // Bits per pixel
                                                cvMat.step[0],                                  // Bytes per row
                                                colorSpace,                                     // Colorspace
                                                kCGImageAlphaNone | kCGBitmapByteOrderDefault,  // Bitmap info flags
                                                provider,                                       // CGDataProviderRef
                                                NULL,                                           // Decode
                                                false,                                          // Should interpolate
                                                kCGRenderingIntentDefault);                     // Intent
            
            UIImage *image = [[UIImage alloc] initWithCGImage:imageRef];
            CGImageRelease(imageRef);
            CGDataProviderRelease(provider);
            CGColorSpaceRelease(colorSpace);
            
            return image;
        }

    /** @function assessFaces */
    //int assessFaces(std::vector<Rect> faces, cv::Rect ROI_lower)
    + (int) assessFaces:(NSArray *)faces withLowerBound:(cv::Rect)ROI_lower
    {
        // There must be at least one face in the detected faces that is bigger than the minimum size of the ROI
        for(int i = 0; i < faces.count; i++) {
            CIFeature *face = [faces objectAtIndex:i];
            
            if ((face.bounds.size.width >= ROI_lower.width) && (face.bounds.size.height >= ROI_lower.height))
            {
                return 1;
            }
        }
        
        return 0;
    }


    /** @function NsDataFromCvMat */
+ (NSMutableData*) NsDataFromCvMat:(cv::Mat*)image
{
	int matRows = image->rows;
	int matCols = image->cols;
	
	NSMutableData* data = [[NSMutableData alloc] init];
	
	unsigned char *pix;
	
	for (int i = 0; i < matRows; i++)
	{
		for (int j = 0; j < matCols; j++)
		{
			pix = &image->data[i * matCols + j];
			[data appendBytes:(void*)pix length:1];
            
		}
	}
	return data;
}

@end