#import "auto_start.hpp"

using namespace std;
using namespace cv;




@implementation auto_start



//    /**Global variables */
//    String face_cascade_name = "haarcascade_frontalface_alt.xml";
//    CascadeClassifier face_cascade;
//
//    String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
//    CascadeClassifier eyes_cascade;
//
    RNG rng(12345);




    /** @function detectFrontalFaces */
    + (NSArray*) detectFrontalFaces:(cv::Mat*) frame
    {
        //-- Detect faces
        //face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
        
        static CIContext *context = [CIContext contextWithOptions:nil];
        static NSDictionary *opts = @{ CIDetectorAccuracy : CIDetectorAccuracyLow };
        
        @autoreleasepool {
            CIDetector *detector = [CIDetector detectorOfType:CIDetectorTypeFace
                                                      context:context
                                                      options:opts];
            UIImage *uiImage = [self imageWithCVMat:*frame];
            CIImage *ciImage = [CIImage imageWithCGImage:uiImage.CGImage];
            
            NSArray *features = [detector featuresInImage:ciImage]; // Potential memory leak is caused by the ARC not being used in queues other than Main. Need @autoreleasepool
            
            //NSLog(@"features = %d", features.count);
            
            return features;
        }
        
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