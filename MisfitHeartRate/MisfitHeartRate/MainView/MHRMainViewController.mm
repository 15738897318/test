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
    CvCapture *videoCapture;
}

@property (strong, nonatomic) CvVideoCamera *videoCamera;
@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (weak, nonatomic) IBOutlet UIButton *startButton;

@end

@implementation MHRMainViewController

@synthesize videoCamera = _videoCamera;


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
    NSLog(@"resourcePath = %@", resourcePath);
    NSLog(@"%@", [resourcePath stringByDeletingLastPathComponent]);
    //    NSLog(@"%@", [[resourcePath stringByDeletingLastPathComponent] stringByDeletingLastPathComponent]);
    
    String fileName("test.mp4");
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:[[resourcePath stringByDeletingLastPathComponent] stringByAppendingString:@"/test.mp4"]])
    {
        NSLog(@"file is not exists!");
    }
    
    VideoCapture _videoCapture;
    _videoCapture.open(fileName);
    if (!_videoCapture.isOpened())
    {
        NSLog(@"_videoCapture is not opened!");
        return;
    }
    
    
    CvCapture *capture = cvCreateFileCapture("2014-06-10-Self-Face_crop.mp4");
    
    if (!capture)
    {
        NSLog(@"Cannot read file \"2014-06-10-Self-Face_crop.mp4\"");
        return;
    }
    
    IplImage *frame;
    Mat mat;
    for (int i = 0; i < 10; ++i)
    {
        frame = cvQueryFrame(capture);
        if (!frame)
            break;
        mat = cvarrToMat(frame);
        _imageView.image = MatToUIImage(mat);
        cvWaitKey(30);
        //        waitKey(30);
    }
    cvReleaseCapture(&capture);
    
    
    //    _videoCamera = [[CvVideoCamera alloc] initWithParentView:self.imageView];
    //    _videoCamera.delegate = self;
    //    _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    //    _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
    //    _videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
    //    _videoCamera.rotateVideo = YES;
    //    _videoCamera.defaultFPS = 30;
    //    _videoCamera.grayscaleMode = NO;
    //    [_videoCamera start];'
    
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


#pragma - Protocol CvVideoCameraDelegate

- (void)processImage:(cv::Mat &)image
{
    NSLog(@"Image");
    // Do some OpenCV stuff with the image
    Mat image_copy;
    cvtColor(image, image_copy, CV_BGRA2BGR);
    
    // invert image
    bitwise_not(image_copy, image_copy);
    //    cvtColor(image_copy, image, CV_BGR2BGRA);
}

@end
