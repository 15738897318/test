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

static const int kBlockFrameSize = 128;

@interface MHRMainViewController ()
    {
        BOOL isCapturing;
        cv::Rect cropArea, ROI_upper, ROI_lower;
        MHRResultViewController *resultView;
        hrResult currentResult;
        int framesWithFace; // Count the number of frames having a face in the region of interest
        int framesWithNoFace; // Count the number of frames NOT having a face in the region of interest
        MBProgressHUD *progressHUD;
        
        bool isCalcMode;
        double lower_range;
        double upper_range;
        hrResult result;
        std::vector<double> temporal_mean;
        NSMutableArray *frameIndexArray;
        
        NSOperationQueue *myQueue;
        int blockCount;
    }

    @property (retain, nonatomic) CvVideoCamera *videoCamera;
    @property (strong, nonatomic) NSString *outPath;
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
    @synthesize cameraSwitch = _cameraSwitch;
    @synthesize outPath = _outPath;
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
        
        setFaceParams();
        [self setUpThreads];
        
        currentResult = hrResult(55, 55);
        isCapturing = NO;
        cropArea = cv::Rect(WIDTH_PADDING, HEIGHT_PADDING, IMAGE_WIDTH, IMAGE_HEIGHT);
        
        // Create the upper & lower bounds for the face-detection area
        int ROI_x;
        int ROI_y;
        int ROI_width;
        int ROI_height;
        
        ROI_x = cropArea.x - (int)((double)cropArea.width * (_ROI_RATIO_UPPER - 1)) / 2;
        ROI_y = cropArea.y - (int)((double)cropArea.height * (_ROI_RATIO_UPPER - 1)) / 2;
        ROI_width = (int)((double)cropArea.width * _ROI_RATIO_UPPER);
        ROI_height = (int)((double)cropArea.height * _ROI_RATIO_UPPER);
        ROI_upper = cv::Rect(ROI_x, ROI_y, ROI_width, ROI_height);
        
        ROI_x = cropArea.x - (int)((double)cropArea.width * (_ROI_RATIO_LOWER - 1)) / 2;
        ROI_y = cropArea.y - (int)((double)cropArea.height * (_ROI_RATIO_LOWER - 1)) / 2;
        ROI_width = (int)((double)cropArea.width * _ROI_RATIO_LOWER);
        ROI_height = (int)((double)cropArea.height * _ROI_RATIO_LOWER);
        ROI_lower = cv::Rect(ROI_x, ROI_y, ROI_width, ROI_height);
        
        framesWithFace = framesWithNoFace = 0;
        
        _videoCamera = [[CvVideoCamera alloc] initWithParentView:self.imageView];
        _videoCamera.delegate = self;
        _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
        _videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationLandscapeLeft;
        _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
//        _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset640x480;
        _videoCamera.defaultFPS = _frameRate;
        _videoCamera.rotateVideo = YES;
        _videoCamera.grayscaleMode = NO;
        [_videoCamera start];
        
        // add start and stop button
        _startButton = [[UIBarButtonItem alloc] initWithTitle:@"Start" style:UIBarButtonItemStylePlain target:self action:@selector(startButtonDidTap:)];
        _stopButton = [[UIBarButtonItem alloc] initWithTitle:@"Stop" style:UIBarButtonItemStylePlain target:self action:@selector(stopButtonDidTap:)];
        self.navigationItem.leftBarButtonItem = _startButton;
        self.navigationItem.rightBarButtonItem = _stopButton;
        // draw Aqua rectangle
        [self drawFaceCaptureRect:@"MHRCameraCaptureRect"];
        // update Layout (iOS6 vs iOS7)
        [self updateLayout];
        
        frameIndexArray = [[NSMutableArray alloc] init];
        myQueue = [[NSOperationQueue alloc] init];
        myQueue.maxConcurrentOperationCount = 1;
        
        blockCount = 0;
        
//        test_findpeak();
//        [MHRTest test_run_algorithm];
    }

    #pragma - Threads process
    - (void)setUpThreads
    {
        isCalcMode = NO;
    }

    - (void)heartRateCalculation
    {
        int idx = blockCount * kBlockFrameSize;
        if (idx >= frameIndexArray.count)
        {
            return;
        }
        
        NSNumber *startIndex = frameIndexArray[idx];
        int value = min((blockCount + 1) * kBlockFrameSize, (int)frameIndexArray.count) - 1;
        NSNumber *endIndex = frameIndexArray[value];
        
        NSLog(@"start index: %@", startIndex);
        NSLog(@"end index: %@", endIndex);
        NSLog(@"block count: %d", blockCount);
        blockCount ++;
        
        // Run algorithm
        //isProcessing = YES;
        std::vector<double> temp;
        processingPerBlock([_outPath UTF8String], [_outPath UTF8String], startIndex.intValue, endIndex.intValue, isCalcMode, lower_range, upper_range, result, temp);
        processingCumulative(temporal_mean, temp, currentResult);
        NSLog(@"Result: %lf, %lf", currentResult.autocorr, currentResult.pda);
        //isProcessing = NO;
        
    }

    - (void)startThreads
    {
        _nFrames = 0;
        
        //[frameIndexArray addObject:[NSNumber numberWithInt:(int)_nFrames]];

        //isProcessing = NO;
    }

    -(void)viewDidDisappear:(BOOL)animated
    {
        [super viewDidDisappear:animated];
    }


    -(void)viewDidAppear:(BOOL)animated
    {
        [super viewDidAppear:animated];
        [self switchCamera:self];
    }


    - (void)didReceiveMemoryWarning
    {
//        [super didReceiveMmoryWarning];
        // Dispose of any resources that can be recreated.
    }


    - (IBAction)startButtonDidTap:(id)sender
    {
        printf("_DEBUG_MODE = %d\n", _DEBUG_MODE);
        printf("_THREE_CHAN_MODE = %d\n", _THREE_CHAN_MODE);
        
        if(isCapturing) return;
        isCapturing = TRUE;
        static int touchCount = 0;
        touchCount ++;
        NSLog(@"%d",touchCount);
        
        // create new directory for this session
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
        _outPath = [paths objectAtIndex:0];
        _outPath = [_outPath substringToIndex:([_outPath length] - [@"Library/Documentation/" length] + 1)];
        
        // 
        if (_DEBUG_MODE)
        {
            NSDateFormatter *formater = [[NSDateFormatter alloc] init];
            formater.dateFormat = @"yyyy-MM-dd-HH-mm-ss";
            _outPath = [_outPath stringByAppendingFormat:@"Documents/%@/",
                        [formater stringFromDate:[NSDate date]]];
        }
        else
        {
            _outPath = [_outPath stringByAppendingFormat:@"Documents/temp/"];
            
            // Clear the /temp folder in advance, before populating it with new PNG files
            NSFileManager *fm = [NSFileManager defaultManager];
            NSError *error = nil;
            BOOL success = [fm removeItemAtPath:_outPath error:&error];
            if (!success || error) {
                // something went wrong
            }
        }
        
        [MHRUtilities createDirectory:_outPath];
        _outputPath = [_outPath UTF8String] + String("/");
        
        isCapturing = YES;
//        self.navigationItem.leftBarButtonItem.enabled = NO;
        _startButton.enabled = NO;
        _cameraSwitch.enabled = NO;
        _nFrames = 0;
        _faceLabel.text = [NSString stringWithFormat:@"Recording.... (keep at least %d seconds)", _minVidLength ];
        _recordTimer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                        target:self
                                                      selector:@selector(updateRecordTime:)
                                                      userInfo:nil
                                                       repeats:YES];
        // Start thread
        [self startThreads];
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
        
        // write _nFrames to file
        FILE *file = fopen(([_outPath UTF8String] + string("/input_frames.txt")).c_str(), "w");
        fprintf(file, "%d\n", (int)_nFrames);
        fclose(file);
        
        // heartRate cal
        __block hrResult result(-1, -1);
        
        // Show the waiting animation
        progressHUD = [MBProgressHUD showHUDAddedTo:self.view animated:YES];
        progressHUD.mode = MBProgressHUDModeIndeterminate;
        
        // Create a timer event that regularly calls the updateUI function to display the HR whilst performing calculations
        [NSTimer scheduledTimerWithTimeInterval:3 target:self selector:@selector(updateUI) userInfo:nil repeats:YES];
        
        // Initialise the result-storing variables
        hrGlobalResult.autocorr = hrGlobalResult.pda = 0;
        hrOldGlobalResult.autocorr = hrOldGlobalResult.pda = 0;
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
            while (myQueue.operationCount != 0);
            
            if (_DEBUG_MODE)
                printf("_nFrames = %ld, _minVidLength = %d, _frameRate = %d\n", (long)_nFrames, _minVidLength, _frameRate);
            
            dispatch_async(dispatch_get_main_queue(), ^{
                // show result
                [myQueue cancelAllOperations];
                resultView = [[MHRResultViewController alloc] init];
                resultView.autocorrResult = currentResult.autocorr;
                resultView.pdaResult = currentResult.pda;
                [self.navigationController pushViewController:resultView animated:YES];
                
                // update UI
                [MBProgressHUD hideHUDForView:self.view animated:YES];
                
                //Reset counters of mainView
                framesWithFace = 0;
                framesWithNoFace = 0;
                isCapturing = false;
                
                _recordTimeLabel.text = @"0";
            });
        });
    }


    - (void)updateUI
    {
        double hr = hrGlobalResult.autocorr;
        
        // If the HR is smaller than 55, then show a randomised number based on 55
        if (hr < 55)
            hr = 55 + arc4random() % (10 + 5 + 1) - 5;
        else
        {
            double old_hr = hrOldGlobalResult.autocorr;
            
            // If the new HR is same as the old HR, then show a randomised number based on the old HR
            if (hr == old_hr)
            {
                hr = old_hr + arc4random() % (6 + 3 + 1) - 3;
            }
            else
            {
                hrOldGlobalResult.autocorr = hr;
            }
        }
        progressHUD.labelText = [NSString stringWithFormat:@"Calculating: %d BPM...",int(hr)];
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
            
            // Set the camera to show camera capture onto the screen
            _videoCamera.ParentView = self.imageView;
            _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
            [_videoCamera start];
            setFaceParams();
        }
        else
        {
            // back camera - finger capturing
            [MHRUtilities setTorchModeOn:YES];
            _faceLabel.text = @"";
            _fingerLabel.text = FINGER_MESSAGE;
            [self drawFaceCaptureRect:@"MHRWhiteColor"];
            [_videoCamera stop];
            
            // Set the camera to hide camera capture from the screen
            _videoCamera.ParentView = nil;
            _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
            [_videoCamera start];
            setFingerParams();
        }
    }


    - (void)updateRecordTime:(id)sender
    {
        ++_recordTime;
        _recordTimeLabel.text = [NSString stringWithFormat:@"%i", (int)_recordTime];
        if (_recordTime >= _maxVidLength)
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
        if (isCapturing)
        {
            Mat new_image = image(cropArea);
            cvtColor(new_image, new_image, CV_BGRA2BGR);
            imwrite([_outPath UTF8String] + string("/input_frame[") + to_string(_nFrames) + string("].png"), new_image);
            [frameIndexArray addObject:[NSNumber numberWithInt:(int)_nFrames]];
            ++_nFrames;
            
            
            NSLog(@"%ld",(long)_nFrames);
            
            // Add new block to queue
            int upper = (blockCount + 1) * kBlockFrameSize;
            int size = (int)frameIndexArray.count;
            
            if ( size >= upper)
            {
                [myQueue addOperationWithBlock: ^ {
                    [self heartRateCalculation];
                }];
            }
        }
        else
        {
            if (_cameraSwitch.isOn)
            {
                static Mat tmp;
                static int cnt = 0;
                cnt = (cnt + 1) % 3;
                if(cnt) return;
                
                tmp = image(ROI_upper).clone();
                
                dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
                    // Cut the frame down to the upper bound of ROI
                    Mat frame_ROI = tmp;
                    
                    // Rotate the frame to fit the orientation preferred by the detector
                    transpose(frame_ROI, frame_ROI);
                    for(int i=0; i < frame_ROI.rows / 2; ++i) for(int j = 0; j < frame_ROI.cols * 4; ++j)
                        swap(frame_ROI.at<unsigned char>(i, j), frame_ROI.at<unsigned char>(frame_ROI.rows - i - 1, j));
                    
                    // If the main thread is already running the capture algo, then dont do the face detection
                    if(isCapturing) return;
                    
                    // Face detection
                    NSArray *faces = [auto_start detectFrontalFaces:&frame_ROI];
                    
                    // If in the meantime, the main thread already transitions into running the capture algo, then dont do the counting increment
                    if(isCapturing) return;
                    
                    // If this iteration detects valid faces
                    // - Increment the framesWithFace variable
                    int assessmentResult = [auto_start assessFaces:faces withLowerBound:ROI_lower];
                    faces = nil;
                    if (assessmentResult == 1)
                    {
                        framesWithFace += 1;
                    }
                    else
                    {
                        framesWithNoFace += 1;
                    }
                    
                    // If a face is not detected in N frames, then reset the face-detected streak
                    if (framesWithNoFace > _THRESHOLD_NO_FACE_FRAMES_MIN)
                    {
                        framesWithFace = 0;
                        framesWithNoFace = 0;
                    }
                    
                    // If a face is detected in more than M frames, then reset the no-face streak
                    if (framesWithFace > _THRESHOLD_FACE_FRAMES_MIN)
                    {
                        framesWithNoFace = 0;
                    }
                    
                    if (framesWithFace > _THRESHOLD_FACE_FRAMES_FOR_START)
                    {
                        // tap the startButton
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [self startButtonDidTap:self];
                        });
                        
                    }
                    // NSLog(@"%d %d %d",assessmentResult, framesWithFace, framesWithNoFace);
                });
            }
            else
            {
                return;
            }
            
        }
        
    }


- (IBAction)settingsButtonDidTap:(id)sender {
    MHRSettingsViewController *settingsView = [[MHRSettingsViewController alloc] init];
    settingsView.delegate = self;
    settingsView.debugModeOn = (_DEBUG_MODE > 0);
    settingsView.threeChanModeOn = (_THREE_CHAN_MODE > 0);
    [self.navigationController pushViewController:settingsView animated:YES];
}


- (void)debugModeChanged:(BOOL)mode
{
    _DEBUG_MODE = int(mode);
}


- (void)threeChanModeChanged:(BOOL)mode
{
    _THREE_CHAN_MODE = int(mode);
}


@end
