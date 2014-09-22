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
#import "files.h"
#include "time.h"

static NSString * const FACE_MESSAGE = @"Make sure your face fitted in the Aqua rectangle!";
static NSString * const FINGER_MESSAGE = @"Completely cover the back-camera and the flash with your finger!";

static const int kBlockFrameSize = 128;

@interface MHRMainViewController () {
    FILE *_fout;
    clock_t Last;
}
    @property (retain, nonatomic) CvVideoCamera *videoCamera;
    @property (strong, nonatomic) NSString *outPath;
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
    @property (weak, nonatomic) IBOutlet UIView *viewTop;
    @property (weak, nonatomic) IBOutlet UILabel *labelTop;
    @property (weak, nonatomic) IBOutlet UIView *viewTop2;
@end


@implementation MHRMainViewController

    @synthesize videoCamera = _videoCamera;
    @synthesize cameraSwitch = _cameraSwitch;
    @synthesize outPath = _outPath;
    @synthesize recordTime = _recordTime;
    @synthesize recordTimer = _recordTimer;



    - (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
    {
        self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
        if (self)
        {
            // Custom initialization
            
        }
        return self;
    }


    - (void)viewDidLoad
    {
        [super viewDidLoad];
        
        self.viewTop.hidden = NO;
        self.labelTop.text = @"Calculating...";
        self.viewTop2.hidden = YES;
        
        isTorchOn = NO;
        
        setFaceParams();
        [self setUpThreads];
        
        // Initialise the result-storing variables
        hrGlobalResult.autocorr = hrGlobalResult.pda = 0;
        hrOldGlobalResult.autocorr = hrOldGlobalResult.pda = 0;
        currentResult = hrResult(-1, -1);
        
        isCapturing = NO;
        cropArea = cv::Rect(HEIGHT_PADDING, WIDTH_PADDING, IMAGE_HEIGHT, IMAGE_WIDTH);
        
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
        _videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
        _videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
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
//        [self drawFaceCaptureRect:@"MHRCameraCaptureRect"];
        
        // update Layout (iOS6 vs iOS7)
        [self updateLayout];
        
        frameIndexArray[0] = [[NSMutableArray alloc] init];
        frameIndexArray[1] = [[NSMutableArray alloc] init];
        myQueue = [[NSOperationQueue alloc] init];
        myQueue.maxConcurrentOperationCount = 1;
        
        blockNumber[0] = blockNumber[1] = 0;
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
        _outPath = [paths objectAtIndex:0];
        _outPath = [_outPath substringToIndex:([_outPath length] - [@"Library/Documentation/" length] + 1)];
        _outPath = [_outPath stringByAppendingFormat:@"Documents/frameRate.txt"];
        
        _fout = fopen([_outPath UTF8String], "w");

        
//        [MHRTest test_run_algorithm];
        
    }


    -(void)viewDidDisappear:(BOOL)animated
    {
        fclose(_fout);
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

    - (void)updateViewTop:(NSTimer *)timer
    {
        if (!isCapturing)
        {
            [timer invalidate];
            timer = nil;
        }
        else
        {
            double hr;
            double old_hr;
            
            hr = min(_hrThreshold + 20, hrGlobalResult.autocorr);
            old_hr = min(_hrThreshold + 20, hrOldGlobalResult.autocorr);
            
            hr_polisher(hr, old_hr, _hrThreshold, _hrStanDev);
            hrOldGlobalResult.autocorr = old_hr;
            
            self.labelTop.text = [NSString stringWithFormat:@"Calculating... %d BPM", int(hr)];
        }
    }


    - (IBAction)startButtonDidTap:(id)sender
    {
        NSLog(@"_DEBUG_MODE = %d", _DEBUG_MODE);
        NSLog(@"_THREE_CHAN_MODE = %d", _THREE_CHAN_MODE);
        
        if (isCapturing)
            return;
        isCapturing = TRUE;
        
        if (!_cameraSwitch.isOn) {
            [MHRUtilities setTorchModeOn:YES];
        }
        
        // hide the view viewTop
        self.viewTop.hidden = YES;
        self.viewTop2.hidden = NO;
        [NSTimer scheduledTimerWithTimeInterval:5 target:self selector:@selector(updateViewTop:) userInfo:nil repeats:YES];
        
        static int touchCount = 0;
        touchCount ++;
        
        if (_DEBUG_MODE)
        {
            NSLog(@"%d",touchCount);
            [self drawFaceCaptureRect:cv::Rect(leftEye.x + cropArea.x, leftEye.y + cropArea.y, leftEye.width, leftEye.height) withColorKey:@"MHRRedColor"];
            [self drawFaceCaptureRect:cv::Rect(rightEye.x + cropArea.x, rightEye.y + cropArea.y, rightEye.width, rightEye.height) withColorKey:@"MHRRedColor"];
            [self drawFaceCaptureRect:cv::Rect(mouth.x + cropArea.x, mouth.y + cropArea.y, mouth.width, mouth.height) withColorKey:@"MHRWhiteColor"];
        }
        
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
            if (!success || error)
            {
                // something went wrong
            }
        }
        
        if (_cameraSwitch.isOn)
        {
            for (int i = 0; i < nFaces; ++i)
            {
                [MHRUtilities createDirectory:[_outPath stringByAppendingString:[NSString stringWithFormat:@"%d", i]]];
                [self drawFaceCaptureRect:faceCropArea[i] withColorKey:@"MHRCameraCaptureRect"];
            }
        }
        else
        {
            [MHRUtilities createDirectory:_outPath];
        }
        _outputPath = [_outPath UTF8String] + String("/");
        
        if (_DEBUG_MODE)
            NSLog(@"Create directory: %@", _outPath);
        
        isCapturing = YES;
        _startButton.enabled = NO;
        _cameraSwitch.enabled = NO;
        nFrames[0] = nFrames[1] = 0;
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
        
        // show the view viewTop
        self.viewTop2.hidden = YES;
        self.viewTop.hidden = NO;
        
        _startButton.enabled = YES;
        _cameraSwitch.enabled = YES;
//        [self drawFaceCaptureRect:@"MHRWhiteColor"];
        
        // stop camera capturing
        [_videoCamera stop];
        
        // Show the waiting animation
        progressHUD = [MBProgressHUD showHUDAddedTo:self.view animated:YES];
        progressHUD.mode = MBProgressHUDModeIndeterminate;
        
        // Create a timer event that regularly calls the updateUI function to display the HR whilst performing calculations
        [NSTimer scheduledTimerWithTimeInterval:3 target:self selector:@selector(updateUI) userInfo:nil repeats:YES];
        
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
        
        [self.view bringSubviewToFront:self.imageView];
//        [self drawFaceCaptureRect:@"MHRCameraCaptureRect"];
        
        // write _nFrames to file
        if (_cameraSwitch.isOn) {
            for (int i = 0; i < nFaces; ++i)
            {
                FILE *file = fopen(([_outPath UTF8String] + string ("/") + to_string(i) + string("/input_frames.txt")).c_str(), "w");
                fprintf(file, "%d\n", (int)nFrames[i]);
                fclose(file);
            }
        }
        else
        {
            FILE *file = fopen(([_outPath UTF8String] + string("/input_frames.txt")).c_str(), "w");
            fprintf(file, "%d\n", (int)nFrames[0]);
            fclose(file);
        }
        
        // Add the final block into the processing queue only if there is at least one full block preceding it
        for (int i = 0; i < nFaces; ++i)
            if (nFrames[i] > kBlockFrameSize)
                [myQueue addOperationWithBlock: ^ {
                    [self heartRateCalculation:i];
                }];
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
            while (myQueue.operationCount != 0);
            
            if (_DEBUG_MODE)
                printf("_nFrames = %ld, _minVidLength = %d, _frameRate = %f\n", (long)nFrames, _minVidLength, _frameRate);
            
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
        double hr;
        double old_hr;
        
        if (blockNumber[0] <= 1)
        {
            hr = _hrThreshold + 20;
            old_hr = _hrThreshold + 20;
            
            hr_polisher(hr, old_hr, _hrThreshold, _hrStanDev);
        }
        else
        {
            hr = hrGlobalResult.autocorr;
            old_hr = hrOldGlobalResult.autocorr;
            
            hr_polisher(hr, old_hr, _hrThreshold, _hrStanDev);
        }
        hrOldGlobalResult.autocorr = old_hr;
        
        progressHUD.labelText = [NSString stringWithFormat:@"Calculating... %d BPM",int(hr)];
    }


    - (IBAction)settingsButtonDidTap:(id)sender
    {
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


    - (IBAction)switchCamera:(id)sender
    {
//        currentFace = 0;
        if (_cameraSwitch.isOn)
        {
            // front camera - face capturing
            [MHRUtilities setTorchModeOn:NO];
            _faceLabel.text = FACE_MESSAGE;
            _fingerLabel.text = @"";
//            [self drawFaceCaptureRect:@"MHRCameraCaptureRect"];
            [_videoCamera stop];
            
            // Set the camera to show camera capture onto the screen
            _videoCamera.ParentView = self.imageView;
            _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
            [_videoCamera start];
            for (AVCaptureDeviceInput *deviceInput in _videoCamera.captureSession.inputs) {
                NSError *error;
                [deviceInput.device lockForConfiguration:&error];
                deviceInput.device.activeVideoMinFrameDuration = CMTimeMake(1, 30);
                deviceInput.device.activeVideoMaxFrameDuration = CMTimeMake(1, 30);
                //                [deviceInput.device unlockForConfiguration];
            }
            Last = clock();

            setFaceParams();
            currentResult = hrResult(-1, -1);
        }
        else
        {
            // back camera - finger capturing
            [MHRUtilities setTorchModeOn:NO];
            _faceLabel.text = @"";
            _fingerLabel.text = FINGER_MESSAGE;
            _fingerLabel.textColor = [UIColor blackColor];
//            [self drawFaceCaptureRect:@"MHRWhiteColor"];
            [self.view bringSubviewToFront:_fingerLabel];
            [_videoCamera stop];
            
            // Set the camera to hide camera capture from the screen
            _videoCamera.ParentView = nil; //self.imageView;
            _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
            [_videoCamera start];
            setFingerParams();
            currentResult = hrResult(-1, -1);
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
        if (SYSTEM_VERSION_LESS_THAN(@"7.0"))
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

    - (void)drawFaceCaptureRect:(cv::Rect)rect withColorKey:(NSString *)colorKey
    {
//        int yDelta = 0;
//        if(SYSTEM_VERSION_LESS_THAN(@"7.0"))
//        {
//            yDelta = -IOS6_Y_DELTA;
//        }
        
        int X0 = rect.y, Y0 = rect.x + rect.width;
        int X1 = rect.y + rect.height, Y1 = rect.x;

        Y1 = CAMERA_WIDTH - Y1;
        Y0 = CAMERA_WIDTH - Y0;
        //swap(Y0, Y1);
        
        X0 += self.imageView.frameX; X1 += self.imageView.frameX;
        Y0 += self.imageView.frameY; Y1 += self.imageView.frameY;
        
        // horizontal lines
        [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(X0, Y0, rect.width, 5) pListKey:colorKey]];
        [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(X0, Y1, rect.width, 5) pListKey:colorKey]];
        // vertical line
        [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(X0, Y0, 5, rect.height) pListKey:colorKey]];
        [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(X1, Y0, 5, rect.height + 5) pListKey:colorKey]];
    }

    - (void)updateLayout
    {
        [_faceLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
        [_fingerLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
        [_fingerSwitchLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
        [_faceSwitchLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
        [_cameraSwitch adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
        [_recordTimeLabel adjustFrameFormiOS7ToiOS6:IOS6_Y_DELTA];
        if (SYSTEM_VERSION_LESS_THAN(@"7.0"))
        {
            _cameraSwitch.frameX -= 20;
        }
    }



    #pragma mark - Processing each recorded frame
    - (void)processImage:(Mat &)image
    {
        static int framesWithTorchOn = 0;
        if (isCapturing)
        {
            if (!_cameraSwitch.isOn && (++framesWithTorchOn <= delayTorchOnInFrames))
                return;
            
            static int failedFrames = 0;

            if (_cameraSwitch.isOn)
            {
                for (int i = 0; i < nFaces; ++i)
                {
                    Mat new_image = image(faceCropArea[i]);
                    cvtColor(new_image, new_image, CV_BGRA2BGR);
                    [auto_start removeEyesAndMouth:&new_image];
                    imwrite([_outPath UTF8String] + string("/") + to_string(i) + string("/input_frame[") + to_string(nFrames[i]) + string("].png"), new_image);
                    NSLog(@"%s", ([_outPath UTF8String] + string("/") + to_string(i) + string("/input_frame[") + to_string(nFrames[i]) + string("].png")).c_str());
                    [frameIndexArray[i] addObject:[NSNumber numberWithInt:(int)nFrames[i]]];
                    ++nFrames[i];
                    
                    _frameRate = ((float)nFrames[i] - 1) / (float)_recordTime;
                    
                    // Add new block to queue
                    int upper = (blockNumber[i] + 1) * kBlockFrameSize;
                    int size = (int)frameIndexArray[i].count;
                    
                    if (size >= upper)
                    {
                        [myQueue addOperationWithBlock: ^{
                            [self heartRateCalculation:i];
                        }];
                    }
                    
//                    failedFrames += ![auto_stop faceCheck:image(ROI_upper).clone()];
//                    
//                    if (failedFrames > 5)
//                    {
//                        failedFrames = 0;
//                        dispatch_async(dispatch_get_main_queue(), ^{
//                            [self stopButtonDidTap:self];
//                        });
//                    }
                }
            }
            else
            {
                Mat new_image = image(cropArea);
                cvtColor(new_image, new_image, CV_BGRA2BGR);
                [auto_start removeEyesAndMouth:&new_image];
                imwrite([_outPath UTF8String] + string("/input_frame[") + to_string(nFrames[0]) + string("].png"), new_image);
                [frameIndexArray[0] addObject:[NSNumber numberWithInt:(int)nFrames[0]]];
                ++nFrames[0];
                
                _frameRate = ((float)nFrames[0] - 1) / (float)_recordTime;
                
                // Add new block to queue
                int upper = (blockNumber[0] + 1) * kBlockFrameSize;
                int size = (int)frameIndexArray[0].count;
                
                if (size >= upper)
                {
                    [myQueue addOperationWithBlock: ^{
                        [self heartRateCalculation:0];
                    }];
                }
                
                failedFrames += ![auto_stop fingerCheck:new_image];
                
                if (failedFrames > 5)
                {
                    failedFrames = 0;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [self stopButtonDidTap:self];
                    });
                }
            }
        }
        else
        {
            if (_cameraSwitch.isOn)
            {
                static Mat tmp;
                static int cnt = 0;
                cnt = (cnt + 1) % 3;
                if (cnt)
                    return;
                
                tmp = image.clone();
                
                dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
                    // Cut the frame down to the upper bound of ROI
                    Mat frame_ROI = tmp;
                    
                    // If the main thread is already running the capture algo, then dont do the face detection
                    if (isCapturing)
                        return;
                    
                    // Face detection
                    NSArray *faces = [auto_start detectFrontalFaces:&frame_ROI];
                    
                    // If in the meantime, the main thread already transitions into running the capture algo, then dont do the counting increment
                    if (isCapturing)
                        return;
                    
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
                });
            }
            else
            {
                static Mat tmpFinger;
                static int framesWithTorchOff = delayTorchOffInFrames;
                static vector <float> avgRedVal;
                
                static int cnt = 0;
                cnt = (cnt + 1) % 3;
                if (cnt)
                    return;
                
                tmpFinger = image(cropArea).clone();
                cvtColor(tmpFinger, tmpFinger, CV_BGRA2BGR);
                
                dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
                    if (isTorchOn)
                    {
                        if (++framesWithTorchOn <= delayTorchOnInFrames)
                            return;
                        
                        if ([auto_start isRedColored:tmpFinger])
                        {
                            avgRedVal.push_back([auto_start calculateAverageRedValue:tmpFinger]);
                            if (avgRedVal.size() >= 20)
                            {
                                if ([auto_start isHeartBeat:avgRedVal])
                                {
                                    isTorchOn = NO;
                                    
                                    if (isCapturing)
                                        return;
                                    
                                    dispatch_async(dispatch_get_main_queue(), ^{
                                        [self startButtonDidTap:self];
                                    });
                                }
                                else
                                {
                                    isTorchOn = NO;
                                    [MHRUtilities setTorchModeOn:NO];
                                    
                                    framesWithTorchOff = 0;
                                }
                            }
                        }
                        else
                        {
                            isTorchOn = NO;
                            [MHRUtilities setTorchModeOn:NO];
                            
                            framesWithTorchOff = 0;
                        }
                    }
                    else
                    {
                        if (++framesWithTorchOff <= delayTorchOffInFrames)
                            return;
                        
                        if (![auto_start isSameAsPreviousFrame:tmpFinger] && [auto_start isUniformColored:tmpFinger] && [auto_start isDarkOrDarkRed:tmpFinger])
                        {
                            isTorchOn = YES;
                            avgRedVal.clear();
                            
                            [MHRUtilities setTorchModeOn:YES];
                        
                            framesWithTorchOn = 0;
                        }
                        else
                        {
                            return;
                        }
                    }
                });
            }
        }
    }


    #pragma mark - Threads process
    - (void)setUpThreads
    {
        isCalcMode = YES;
        blockNumber[0] = blockNumber[1] = 0;
    }

- (void)heartRateCalculation:(int)currentFace
    {
        int idxStart = blockNumber[currentFace] * kBlockFrameSize;
        if (idxStart >= frameIndexArray[currentFace].count)
        {
            NSLog(@"Error: Block starts beyond frame count!");
            return;
        }
        

        int idxEnd = min((blockNumber[currentFace] + 1) * kBlockFrameSize, (int)frameIndexArray[currentFace].count) - 1;
        if (isCapturing && ((idxEnd - idxStart + 1) < kBlockFrameSize))
        {
            NSLog(@"Error: Non-final block length is shorter than allowed");
            return;
        }

        NSNumber *startIndex = frameIndexArray[currentFace][idxStart];
        NSNumber *endIndex = frameIndexArray[currentFace][idxEnd];
        
        if (_DEBUG_MODE)
        {
            NSLog(@"=====");
            NSLog(@"Start index: %@", startIndex);
            NSLog(@"End index: %@", endIndex);
        }
        
        // Run algorithm only if there are at least 10 frames left
        if (endIndex.intValue - startIndex.intValue >= 10)
        {
            std::vector<double> temp;
        
            processingPerBlock([_outPath UTF8String] + string("/") + to_string(currentFace), [_outPath UTF8String] + string("/") + to_string(currentFace), startIndex.intValue, endIndex.intValue, isCalcMode, lower_range, upper_range, result, temp);
            processingCumulative(temporal_mean, temp, currentResult);
            for (int i = 0; i < temporal_mean.size(); ++i)
                printf("%f ", temporal_mean[i]);
            printf("\n");
            isCalcMode = NO;
            blockNumber[currentFace]++;
            
            if (_DEBUG_MODE)
            {
                NSLog(@"currentResult: %lf, %lf", currentResult.autocorr, currentResult.pda);
                NSLog(@"hrGlobalResult: %lf, %lf", hrGlobalResult.autocorr, hrGlobalResult.pda);
                NSLog(@"Number of blocks processed: %d", blockNumber[currentFace]);
            }
        }
    }

    - (void)startThreads
    {
        nFrames[0] = nFrames[1] = 0;
        [frameIndexArray[0] removeAllObjects];
        [frameIndexArray[1] removeAllObjects];
        temporal_mean.clear();
        isCalcMode = YES;
        blockNumber[0] = blockNumber[1] = 0;
    }

@end
