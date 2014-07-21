//
//  MHRMainViewController.m
//  videoHeartRate
//
//  Created by Bao Nguyen on 6/23/14.
//  Copyright (c) 2014 misfit. All rights reserved.
//

#import "MHRMainViewController.hpp"
#import "UIImageCVMatConverter.hpp"
#import "matlab.h"

const int IOS6_Y_DELTA = 60;
const int CAMERA_WIDTH = 352;
const int CAMERA_HEIGHT = 288;
const int IMAGE_WIDTH = 128;
const int IMAGE_HEIGHT = 128;
const int WIDTH_PADDING = (CAMERA_WIDTH-IMAGE_WIDTH)/2;
const int HEIGHT_PADDING = (CAMERA_HEIGHT-IMAGE_HEIGHT)/2;
static NSString * const FACE_MESSAGE = @"Make sure your face fitted in the Aqua rectangle!";
static NSString * const FINGER_MESSAGE = @"Completely cover the back-camera and the flash with your finger!";


@interface MHRMainViewController ()
{
    BOOL isCapturing;
    cv::Rect cropArea;
}

@property (retain, nonatomic) CvVideoCamera *videoCamera;
@property (assign, nonatomic) VideoWriter videoWriter;
@property (strong, nonatomic) NSString *outPath;
@property (strong, nonatomic) NSString *outFile;
@property (assign, nonatomic) NSInteger nFrames;
@property (assign, nonatomic) NSInteger recordTime;
@property (strong, nonatomic) NSTimer *recordTimer;

@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (weak, nonatomic) IBOutlet UISwitch *cameraSwitch;
@property (weak, nonatomic) IBOutlet UILabel *faceLabel;
@property (weak, nonatomic) IBOutlet UILabel *fingerLabel;
@property (weak, nonatomic) IBOutlet UILabel *fingerSwitchLabel;
@property (weak, nonatomic) IBOutlet UILabel *faceSwitchLabel;
@property (weak, nonatomic) IBOutlet UILabel *recordTimeLabel;
@property (strong, nonatomic) IBOutlet UIBarButtonItem *startButton;
@property (strong, nonatomic) IBOutlet UIBarButtonItem *stopButton;

@end


@implementation MHRMainViewController

@synthesize videoCamera = _videoCamera;
@synthesize videoWriter = _videoWriter;
@synthesize cameraSwitch = _cameraSwitch;
@synthesize outPath = _outPath;
@synthesize outFile = _outFile;
@synthesize nFrames = _nFrames;
@synthesize recordTime = _recordTime;
@synthesize recordTimer = _recordTimer;


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
    _videoCamera = [[CvVideoCamera alloc] initWithParentView:self.imageView];
    _videoCamera.delegate = self;
    _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
    _videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
    _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
//    _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset640x480;
    _videoCamera.defaultFPS = _frameRate;
    _videoCamera.rotateVideo = YES;
    _videoCamera.grayscaleMode = NO;
    [_videoCamera start];
    
    isCapturing = NO;
    cropArea = cv::Rect(WIDTH_PADDING, HEIGHT_PADDING, IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // add start and stop button
    _startButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(startButtonDidTap:)];
    _stopButton = [[UIBarButtonItem alloc] initWithTitle:@"Stop" style:UIBarButtonItemStylePlain target:self action:@selector(stopButtonDidTap:)];
    self.navigationItem.leftBarButtonItem = _startButton;
    self.navigationItem.rightBarButtonItem = _stopButton;
    // draw Aqua rectangle
    [self drawFaceCaptureRect:@"MHRCameraCaptureRect"];
    // update Layout (iOS6 vs iOS7)
    [self updateLayout];
}


-(void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    _videoWriter.release();
}


-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self switchCamera:self];
}


- (void)didReceiveMemoryWarning
{
    //[super didReceiveMmoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)startButtonDidTap:(id)sender
{
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
    
    isCapturing = YES;
//    self.navigationItem.leftBarButtonItem.enabled = NO;
    _startButton.enabled = NO;
    _cameraSwitch.enabled = NO;
    _nFrames = 0;
    _faceLabel.text = @"Recording....";
    _recordTimer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                    target:self
                                                  selector:@selector(updateRecordTime:)
                                                  userInfo:nil
                                                   repeats:YES];
}


- (IBAction)stopButtonDidTap:(id)sender
{
    if (!isCapturing)
        return;
    isCapturing = NO;
    _startButton.enabled = YES;
    _cameraSwitch.enabled = YES;
    [self drawFaceCaptureRect:@"MHRWhiteColor"];
    // stop camera capturing
    [_videoCamera stop];
    _videoWriter.release();
    // stop timer
    [_recordTimer invalidate];
    _recordTimer = nil;
    _recordTime = 0;
    [MHRUtilities setTorchModeOn:NO];
    
    if (!_cameraSwitch.isOn)
    {
        _fingerLabel.text = @"";
    }
    _faceLabel.text = @"Processing....";
    
    __block hrResult result(-1, -1);
    [MBProgressHUD showHUDAddedTo:self.view animated:YES];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
        if (DEBUG_MODE) printf("nFrames = %d\n", (int)_nFrames);
        if (_nFrames >= _minVidLength*_frameRate)
            result = run_algorithms([_outPath UTF8String], "input.mp4", [_outPath UTF8String]);
//            NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
//            result = run_algorithms([resourcePath UTF8String], "test1.mp4", [_outPath UTF8String]);
//            result = run_algorithms([resourcePath UTF8String], "test0.mp4", [_outPath UTF8String]);
//          result = run_algorithms([resourcePath UTF8String], "2014-06-10-Self-Face_crop.mp4", [outputPath UTF8String]);
        dispatch_async(dispatch_get_main_queue(), ^{
            // show result
            MHRResultViewController *resultView = [[MHRResultViewController alloc] init];
            resultView.autocorrResult = result.autocorr;
            resultView.pdaResult = result.pda;
            [self.navigationController pushViewController:resultView animated:YES];
            // update UI
            [MBProgressHUD hideHUDForView:self.view animated:YES];
//            [self switchCamera:self];
            _recordTimeLabel.text = @"0";
        });
    });
}


- (IBAction)switchCamera:(id)sender
{
    if (_cameraSwitch.isOn)
    {
        // front camera - face capturing
        [MHRUtilities setTorchModeOn:NO];
        _faceLabel.text = FACE_MESSAGE;
        _fingerLabel.text = @"";
        [self drawFaceCaptureRect:@"MHRCameraCaptureRect"];
        [_videoCamera stop];
        _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
        [_videoCamera start];
    }
    else
    {
        // back camera - finger capturing
        [MHRUtilities setTorchModeOn:YES];
        _faceLabel.text = @"";
        _fingerLabel.text = FINGER_MESSAGE;
        [self drawFaceCaptureRect:@"MHRWhiteColor"];
        [_videoCamera stop];
        _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
        [_videoCamera start];
    }
}


- (void)updateRecordTime:(id)sender
{
    ++_recordTime;
    _recordTimeLabel.text = [NSString stringWithFormat:@"%i", (int)_recordTime];
    if (_recordTime >= 30)
    {
        [self stopButtonDidTap:self];
    }
}


- (void)drawFaceCaptureRect:(NSString *)colorKey
{
    int x0 = self.imageView.frameX;
    int y0 = self.imageView.frameY;
    int x1 = x0 + self.imageView.frameWidth;
    int y1 = y0 + self.imageView.frameHeight;
    int dx = (CAMERA_HEIGHT - IMAGE_WIDTH)/2;
    int dy = (CAMERA_WIDTH - IMAGE_HEIGHT)/2;
    int yDelta = 0;
    if(SYSTEM_VERSION_LESS_THAN(@"7.0"))
    {
        yDelta = -IOS6_Y_DELTA;
    }
    
    // horizontal lines
    [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x0 + dx, y0 + dy + yDelta, IMAGE_WIDTH, 5) pListKey:colorKey]];
    [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x0 + dx, y1 - dy + yDelta, IMAGE_WIDTH, 5) pListKey:colorKey]];
    // vertical lines
    [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x0 + dx, y0 + dy + yDelta, 5, IMAGE_HEIGHT) pListKey:colorKey]];
    [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x1 - dx, y0 + dy + yDelta, 5, IMAGE_HEIGHT+5) pListKey:colorKey]];
}


- (void)updateLayout
{
    [_faceLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
    [_fingerLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
    [_fingerSwitchLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
    [_faceSwitchLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
    [_cameraSwitch adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
    [_recordTimeLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
    if(SYSTEM_VERSION_LESS_THAN(@"7.0"))
    {
        _cameraSwitch.frameX -= 20;
    }
}


#pragma - Protocol CvVideoCameraDelegate

- (void)processImage:(Mat &)image
{
    if (isCapturing && (&_videoWriter != nullptr) && _videoWriter.isOpened())
    {
        ++_nFrames;
        Mat new_image = image(cropArea);
//        frameToFile(new_image, [[_outPath stringByAppendingString:@"test_frame_frome_camera.jpg"] UTF8String]);
        cvtColor(new_image, new_image, CV_BGRA2BGR);
        _videoWriter << new_image;
//        printf("image after resize = (%d, %d)\n", new_image.rows, new_image.cols);
    }
}


#pragma - Test Image/Video
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
