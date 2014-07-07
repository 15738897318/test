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

const int CAMERA_WIDTH = 352;
const int CAMERA_HEIGHT = 288;
const int IMAGE_WIDTH = 256;
const int IMAGE_HEIGHT = 256;
const int WIDTH_PADDING = (CAMERA_WIDTH-IMAGE_WIDTH)/2;
const int HEIGHT_PADDING = (CAMERA_HEIGHT-IMAGE_HEIGHT)/2;


@interface MHRMainViewController ()
{
    BOOL isCapturing;
    cv::Rect cropArea;
}

@property (retain, nonatomic) CvVideoCamera *videoCamera;
@property (assign, nonatomic) VideoWriter videoWriter;
@property (strong, nonatomic) NSString *outPath;
@property (strong, nonatomic) NSString *outFile;

@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UISwitch *cameraSwitch;
@property (weak, nonatomic) IBOutlet UIButton *stopButton;
@property (weak, nonatomic) IBOutlet UILabel *statusLabel;

@end


@implementation MHRMainViewController

@synthesize videoCamera = _videoCamera;
@synthesize videoWriter = _videoWriter;
@synthesize cameraSwitch = _cameraSwitch;
@synthesize outPath = _outPath;
@synthesize outFile = _outFile;


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
//    _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
    _videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
    _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
//    _videoCamera.imageWidth = 256;
//    _videoCamera.imageHeight = 256;
    _videoCamera.defaultFPS = _frameRate;
    _videoCamera.rotateVideo = YES;
    _videoCamera.grayscaleMode = NO;
    [_videoCamera start];
    
    isCapturing = NO;
    cropArea = cv::Rect(WIDTH_PADDING, HEIGHT_PADDING, IMAGE_WIDTH, IMAGE_HEIGHT);
    
//    [self startButtonDidTap:self];
}


-(void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    _videoWriter.release();
}


- (void)didReceiveMemoryWarning
{
    //[super didReceiveMmoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)startButtonDidTap:(id)sender {
//    test_ideal_bandpassing();
//    testMathFunctions();
//    test_fft();
//    return;

    // create new directory for this session
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
    _outPath = [paths objectAtIndex:0];
    _outPath = [_outPath substringToIndex:([_outPath length] - [@"Library/Documentation/" length] + 1)];
    NSDateFormatter *formater = [[NSDateFormatter alloc] init];
    formater.dateFormat = @"yyyy-MM-dd-HH-mm-ss";
    _outPath = [_outPath stringByAppendingFormat:@"Documents/%@/",
                [formater stringFromDate:[NSDate date]]];
    [MHRUtilities createDirectory:_outPath];
    
    // output new file to write input video
    _outFile = [_outPath stringByAppendingString:@"input.mp4"];
    _videoWriter.open([_outFile UTF8String],
                      CV_FOURCC('M','P','4','2'),
                      _frameRate,
                      cvSize(IMAGE_WIDTH, IMAGE_HEIGHT),
                      true);
//    [_videoCamera start];
    
    isCapturing = YES;
    _startButton.enabled = NO;
    _cameraSwitch.enabled = NO;
    _statusLabel.text = @"Capturing....";

//     NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
//    run_algorithms([resourcePath UTF8String], "test0.mp4", [outputPath UTF8String]);
//    run_algorithms([resourcePath UTF8String], "2014-06-10-Self-Face_crop.mp4", [outputPath UTF8String]);
}


- (IBAction)stopButtonDidTap:(id)sender {
    if (isCapturing)
    {
        isCapturing = NO;
        _startButton.enabled = YES;
        _cameraSwitch.enabled = YES;
//        [_videoCamera stop];
        _videoWriter.release();
        _statusLabel.text = @"Calculating....";
//        hrResult result = run_algorithms([_outPath UTF8String], "input.mp4", [_outPath UTF8String]);
//        _statusLabel.text = [NSString stringWithFormat:@"%f, %f", result.autocorr, result.pda];
    }
}


- (IBAction)switchCamera:(id)sender {
    if (_cameraSwitch.isOn)
    {
        _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
    }
    else
    {
        _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    }
}


#pragma - Protocol CvVideoCameraDelegate

- (void)processImage:(Mat &)image
{
    if (isCapturing)
    {
        printf("image size = (%d, %d)\n", image.rows, image.cols);
        Mat new_image = image.clone();
        frameToFile(new_image, [[_outPath stringByAppendingString:@"test_frame_frome_camera_before_resize.jpg"] UTF8String]);
    }
    if (isCapturing && _videoWriter.isOpened())
    {
        Mat new_image = image(cropArea);
        frameToFile(new_image, [[_outPath stringByAppendingString:@"test_frame_frome_camera.jpg"] UTF8String]);
        cvtColor(new_image, new_image, CV_BGRA2BGR);
        _videoWriter << new_image;
//        printf("image after resize = (%d, %d)\n", new_image.rows, new_image.cols);
    }
    
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


//- (void)updateImageView:(NSInteger)index vid:(vector<Mat>)vid
//{
//    if (index >= vid.size())
//        return;
//    self.imageView.image = [UIImageCVMatConverter UIImageFromCVMat:vid[index]];
//    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
//        [self updateImageView:index+1 vid:vid];
//    });
//}

@end
