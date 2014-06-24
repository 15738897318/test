//
//  OHRMainViewController.m
//  opticalHeartRate
//
//  Created by Bao Nguyen on 6/23/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import "MHRMainViewController.hpp"

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
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)startButtonDidTap:(id)sender {
    _cameraSwitch.enabled = NO;
    if (_cameraSwitch.isOn)
    {
        [_videoCamera start];
        return;
    }
    
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    NSString *filePath = [resourcePath stringByAppendingPathComponent:@"test.mp4"];
    VideoCapture videoCapture([filePath UTF8String]);
    Mat frame;
    if (!videoCapture.isOpened())
    {
        NSLog(@"Error when reading %@", filePath);
    }
    
    int nFrame = videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
    NSLog(@"nFrame = %i", nFrame);
    NSLog(@"Frame rate = %f", videoCapture.get(CV_CAP_PROP_FPS));

    _imageView.image = nil;
    while(1)
    {
        videoCapture >> frame;
        if (frame.empty())
        {
            break;
        }
        ++nFrame;
        
        NSLog(@"nFrame = %i", nFrame);
//        NSLog(@"channels = %i", frame.channels());
//        NSLog(@"type = %i", frame.type());
//        NSLog(@"test type = %i", CV_8UC3);
        
        if (_imageView.image == nil)
        {
//            _imageView.image = [self UIImageFromCVMat:frame];
            _imageView.image = MatToUIImage(frame);
        }
        
//        for (int i = 0; i < frame.rows; ++i)
//            for (int j = 0; j < frame.cols; ++j) {
//                NSLog(@"p(%i, %i) = %i, %i, %i", i, j, frame.at<Vec3b>(i, j)[0], frame.at<Vec3b>(i, j)[1], frame.at<Vec3b>(i, j)[2]);
//            }
//        [NSThread sleepForTimeInterval:1];
    }
   NSLog(@"nFrame = %i", nFrame);
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
    NSLog(@"Image");
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

@end
