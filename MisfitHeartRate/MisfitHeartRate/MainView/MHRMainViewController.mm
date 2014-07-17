//
//  MHRMainViewController.m
//  opticalHeartRate
//
//  Created by Bao Nguyen on 6/23/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import "MHRMainViewController.hpp"
#import "UIImageCVMatConverter.hpp"
#import "matlab.h"


@interface MHRMainViewController ()
{
//    CvCapture *videoCapture;
}

@property (strong, nonatomic) CvVideoCamera *videoCamera;
@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UISwitch *cameraSwitch;
@property (weak, nonatomic) IBOutlet UIButton *stopButton;

@end

@implementation MHRMainViewController

@synthesize videoCamera = _videoCamera;
@synthesize cameraSwitch = _cameraSwitch;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    NSString *filePath = [resourcePath stringByAppendingPathComponent:@"test.mp4"];
    NSLog(@"filePath = %@", filePath);
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:filePath])
    {
        NSLog(@"file is not exists!");
    }
   
    _videoCamera = [[CvVideoCamera alloc] initWithParentView:self.imageView];
    _videoCamera.delegate = self;
    _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
    _videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
    _videoCamera.rotateVideo = YES;
    _videoCamera.defaultFPS = 30;
    _videoCamera.grayscaleMode = NO;
}


-(void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}


- (void)didReceiveMemoryWarning
{
    //[super didReceiveMmoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)startButtonDidTap:(id)sender {
//    testMathFunctions();
//    return;
    
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
//    NSString *filePath = [resourcePath stringByAppendingPathComponent:@"test0.mp4"];
    NSDateFormatter *formater = [[NSDateFormatter alloc] init];
    formater.dateFormat = @"-yyyy-MM-dd-HH-mm-ss";
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
    NSString *tmp = @"Library/Documentation/";
    NSString *outputPath = [paths objectAtIndex:0];
    outputPath = [outputPath substringToIndex:([outputPath length] - [tmp length] + 1)];
    outputPath = [outputPath stringByAppendingFormat:@"Documents/"];
//                  [formater stringFromDate:[NSDate date]]];
    [MHRUtilities createDirectory:outputPath];
    
    
//    run_algorithms([resourcePath UTF8String], "test0.mp4", [outputPath UTF8String]);
    run_algorithms([resourcePath UTF8String], "2014-06-10-Self-Face_crop.mp4", [outputPath UTF8String]);
}


- (void)updateImageView:(NSInteger)index vid:(vector<Mat>)vid
{
    if (index >= vid.size())
        return;
    self.imageView.image = [UIImageCVMatConverter UIImageFromCVMat:vid[index]];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self updateImageView:index+1 vid:vid];
    });
}


- (IBAction)stopButtonDidTap:(id)sender {
    _cameraSwitch.enabled = YES;
    if (_cameraSwitch.isOn)
    {
        [_videoCamera stop];
        return;
    }
}


#pragma - Protocol CvVideoCameraDelegate

- (void)processImage:(cv::Mat &)image
{
    // Do some OpenCV stuff with the image
    Mat image_copy;
    cvtColor(image, image_copy, CV_BGRA2BGR);
    
    // invert image
//    bitwise_not(image_copy, image_copy);
//    cvtColor(image_copy, image, CV_BGR2BGRA);
    
//    NSLog(@"channels = %i", image.channels());
//    for (int i = 0; i < image.rows; ++i) {
//        for (int j = 0; j < image.cols; ++j)
//            NSLog(@"p(%i, %i) = %i, %i, %i, %i", i, j, image.at<Vec3b>(i, j)[0], image.at<Vec3b>(i, j)[1], image.at<Vec3b>(i, j)[2], image.at<Vec3b>(i, j)[3]);
//    }
}


#pragma - Test Image/Video
- (void) testImageVideo
{
    /*----------------read image file----------------*/
    //    NSString *imageFile = [resourcePath stringByAppendingPathComponent:@"test.jpg"];
    //    Mat frame = imread([imageFile UTF8String], CV_LOAD_IMAGE_COLOR);
    //    if(!frame.data)                              // Check for invalid input
    //    {
    //        NSLog(@"Could not open or find the image: %@", imageFile);
    //        return;
    //    }
    //    NSLog(@"Load image....");
    //    [self.imageView setImage:[UIImageCVMatConverter UIImageFromCVMat:frame]];
    //    NSLog(@"Save image....");
    //    NSString *imageOut = [outputPath stringByAppendingString:@"test_out.jpg"];
    //    imwrite([imageOut UTF8String], frame);
    //    NSLog(@"Done!");
    /*------------------------------------------------*/
    
    
    //    Mat frame;
    //    VideoCapture videoCapture([filePath UTF8String]);
    //    if (!videoCapture.isOpened())
    //    {
    //        NSLog(@"Error when reading %@", filePath);
    //    }
    //
    //    vector<Mat> vid = videoCaptureToVector(videoCapture);
    //
    //    int nFrame = videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
    //    NSLog(@"nFrame = %i", nFrame);
    //    NSLog(@"Frame rate = %f", videoCapture.get(CV_CAP_PROP_FPS));
    //
    //    NSLog(@"width = %f, height = %f",
    //          videoCapture.get(CV_CAP_PROP_FRAME_WIDTH),
    //          videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT));
    //
    //    NSString *vidOut = [outputPath stringByAppendingString:@"vidOut.mp4"];
    //    VideoWriter vidWriter([vidOut UTF8String], CV_FOURCC('M','J','P','G'), 30, cvSize(vid[0].cols, vid[0].rows), true);
    //    for (int i = 0, sz = vid.size(); i < sz; ++i) {
    //        vidWriter << vid[i];
    //    }
    //    vidWriter.release();
    
    //    CvVideoWriter *writer = cvCreateVideoWriter(
    //        [vidOut UTF8String], CV_FOURCC('M', 'J', 'P', 'G'), 30,
    //        cvSize(vid[0].cols, vid[0].rows)
    //    );
    //    for (int i = 0, sz = vid.size(); i < sz; ++i) {
    //        IplImage tmp = vid[i];
    //        cvWriteFrame(writer, &tmp);
    //        cvWriteFrame(writer, &tmp );
    //    }
    //    cvReleaseVideoWriter( &writer );
    
    //    [self updateImageView:0 vid:vid];
}

@end
