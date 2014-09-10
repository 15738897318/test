#import "auto_start.hpp"

using namespace std;
using namespace cv;




@implementation auto_start
    RNG rng(12345);

    /* For face detection */
    + (NSArray*) detectFrontalFaces:(cv::Mat*) frame
    {
        //-- Detect faces
        static CIContext *context = [CIContext contextWithOptions:nil];
        static NSDictionary *opts = @{ CIDetectorAccuracy : CIDetectorAccuracyLow };
        
        @autoreleasepool {
            CIDetector *detector = [CIDetector detectorOfType:CIDetectorTypeFace
                                                      context:context
                                                      options:opts];
            UIImage *uiImage = [self imageWithCVMat:*frame];
            CIImage *ciImage = [CIImage imageWithCGImage:uiImage.CGImage];
            
            NSArray *features = [detector featuresInImage:ciImage]; // Potential memory leak is caused by the ARC not being used in queues other than Main. Need @autoreleasepool
            
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



    /** For finger detection */
    + (BOOL)isDarkOrDarkRed:(Mat)tmp
    {
        
        Vec3f testVal = 0;
        
        for (int i = 0; i <= 2; ++i)
        {
            vector <double> arr;
            for (int x = 0; x < tmp.cols; ++x)
                for (int y = 0; y < tmp.rows; ++y)
                    arr.push_back(tmp.at<Vec3b>(y, x)[i]);
            
            testVal[i] = prctile(arr, 95);
        }
        
        if (testVal[0] > notRedThreshold || testVal[1] > notRedThreshold)
        {
            return NO;
        }
        return YES;
    }

    + (BOOL)isUniformColored:(Mat)tmp
    {
        
        vector <double> arr;
        for (int x = 0; x < tmp.cols; ++x)
            for (int y = 0; y < tmp.rows; ++y)
                arr.push_back(tmp.at<Vec3b>(y, x)[2]);
        
        int maxVal = prctile(arr, 90), minVal = prctile(arr, 10);
        
        if (maxVal - minVal > uniformThreshold) {
            return NO;
        }
        
        return YES;
    }

    + (BOOL)isSameAsPreviousFrame:(Mat)tmp
    {
        static Vec3f prevAvg(-1, -1, -1);
        
        Vec3f avgVal(0, 0, 0);
        for (int x = 0; x < tmp.cols; ++x)
            for (int y = 0; y < tmp.rows; ++ y)
            {
                avgVal[0] += tmp.at<Vec3b>(y, x)[0];
                avgVal[1] += tmp.at<Vec3b>(y, x)[1];
                avgVal[2] += tmp.at<Vec3b>(y, x)[2];
            }
        avgVal /= tmp.cols * tmp.rows;
        
        Vec3f diff;
        absdiff(avgVal, prevAvg, diff);
        
        if (prevAvg != Vec3f(-1, -1, -1) && diff[0] < diffThreshold && diff[1] < diffThreshold && diff[2] < diffThreshold)
        {
            prevAvg = avgVal;
            return YES;
        }
        
        prevAvg = avgVal;
        return NO;
    }

    + (BOOL)isRedColored:(Mat)tmp
    {
        Vec3f averageVal = 0;
        
        for (int i = 0; i < tmp.rows; ++i) for (int j = 0; j < tmp.cols; ++j)
        {
            averageVal[0] += tmp.at<Vec3b>(i, j)[0];
            averageVal[1] += tmp.at<Vec3b>(i, j)[1];
            averageVal[2] += tmp.at<Vec3b>(i, j)[2];
        }
        
        averageVal /= tmp.rows * tmp.cols;
        
        if (averageVal[2] > redThreshold && averageVal[0] < notRedThreshold && averageVal[1] < notRedThreshold)
            return YES;
        
        return NO;
    }

    + (float)calculateAverageRedValue:(Mat)tmp
    {
        float averageVal = 0;
        
        for (int i = 0; i < tmp.rows; ++i) for (int j = 0; j < tmp.cols; ++j)
            averageVal += tmp.at<Vec3b>(i, j)[2];
        
        averageVal /= tmp.rows * tmp.cols;
        
        return averageVal;
    }

    + (BOOL)isHeartBeat:(vector <float>)val
    {
        float maxVal = 255, minVal = 0;
        for (int i = 0; i < (int)val.size(); ++i)
        {
            maxVal = max(maxVal, val[i]);
            minVal = min(minVal, val[i]);
        }
        
        return maxVal - minVal > variationThreshold;
    }

@end