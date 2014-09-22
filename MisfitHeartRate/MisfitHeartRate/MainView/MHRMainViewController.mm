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

static NSString * const FACE_MESSAGE = @"Make sure your face fitted in the Aqua rectangle!";
static NSString * const FINGER_MESSAGE = @"Completely cover the back-camera and the flash with your finger!";

static const int kBlockFrameSize = 128;

@interface MHRMainViewController ()
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
    @property (weak, nonatomic) IBOutlet UIView *viewTop;
    @property (weak, nonatomic) IBOutlet UILabel *labelTop;
    @property (weak, nonatomic) IBOutlet UIView *viewTop2;
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
        
        fastMode = false;
        
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
        [self drawFaceCaptureRect:cropArea withColorKey:@"MHRCameraCaptureRect"];
        
        // update Layout (iOS6 vs iOS7)
        [self updateLayout];
        
        frameIndexArray = [[NSMutableArray alloc] init];
        myQueue = [[NSOperationQueue alloc] init];
        myQueue.maxConcurrentOperationCount = 1;
        
        blockNumber = 0;
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

    - (void)updateViewTop:(NSTimer *)timer {
        if (!isCapturing) {
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
        
        if(isCapturing) return;
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
            if (!success || error) {
                // something went wrong
            }
        }
        
        [MHRUtilities createDirectory:_outPath];
        _outputPath = [_outPath UTF8String] + String("/");
        
        if (_DEBUG_MODE)
            NSLog(@"Create directory: %@", _outPath);
        
        isCapturing = YES;
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
        
        // show the view viewTop
        self.viewTop2.hidden = YES;
        self.viewTop.hidden = NO;
        
        _startButton.enabled = YES;
        _cameraSwitch.enabled = YES;
        dispatch_async(dispatch_get_main_queue(), ^{
            [self drawFaceCaptureRect:cropArea withColorKey:@"MHRCameraCaptureRect"];
        });
        
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
        else {
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
        }
        _faceLabel.text = @"Processing....";
        
        // write _nFrames to file
        FILE *file = fopen(([_outPath UTF8String] + string("/input_frames.txt")).c_str(), "w");
        fprintf(file, "%d\n", (int)_nFrames);
        fclose(file);
        
        // Add the final block into the processing queue only if there is at least one full block preceding it
        if (_nFrames > kBlockFrameSize)
            [myQueue addOperationWithBlock: ^ {
                [self heartRateCalculation];
            }];
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
            while (myQueue.operationCount != 0);
            
            if (_DEBUG_MODE)
                printf("_nFrames = %ld, _minVidLength = %d, _frameRate = %f\n", (long)_nFrames, _minVidLength, _frameRate);
            
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
        
        if (blockNumber <= 1)
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


    - (IBAction)switchCamera:(id)sender
    {
        if (_cameraSwitch.isOn)
        {
            // front camera - face capturing
            [MHRUtilities setTorchModeOn:NO];
            _faceLabel.text = FACE_MESSAGE;
            _fingerLabel.text = @"";
            [self drawFaceCaptureRect:cropArea withColorKey:@"MHRCameraCaptureRect"];
            [_videoCamera stop];
            
            // Set the camera to show camera capture onto the screen
            _videoCamera.ParentView = self.imageView;
            _videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
            [_videoCamera start];
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
//            [self drawFaceCaptureRect:cropArea withColorKey:@"MHRWhiteColor"];
//            [self.view bringSubviewToFront:_fingerLabel];
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


- (void)drawFaceCaptureRect:(cv::Rect)rect withColorKey:(NSString *)colorKey
    {
        int yDelta = 0;
        if(SYSTEM_VERSION_LESS_THAN(@"7.0"))
        {
            yDelta = -IOS6_Y_DELTA;
        }
        
        int x0 = rect.x, y0 = rect.y;
        int x1 = rect.x + rect.width, y1 = rect.y + rect.height;
        
//        y1 = CAMERA_WIDTH - y1;
//        y0 = CAMERA_WIDTH - y0;
//        swap(y0, y1);
        x0 += self.imageView.frameX; x1 += self.imageView.frameX;
        y0 += self.imageView.frameY; y1 += self.imageView.frameY;
        
        __weak MHRMainViewController *weakSelf = self;
        
        dispatch_async(dispatch_get_main_queue(), ^{
            NSArray *subLayer = [[NSArray alloc] initWithArray:weakSelf.view.layer.sublayers];
            for (CALayer *layer in subLayer) {
                if ([layer.name isEqual:@"CameraCaptureRect"])
                    [layer removeFromSuperlayer];
            }
            
            
            // horizontal lines
            [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x0, y0, rect.width, 5) pListKey:colorKey]];
            [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x0, y1, rect.width, 5) pListKey:colorKey]];
            // vertical line
            [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x0, y0, 5, rect.height) pListKey:colorKey]];
            [self.view.layer addSublayer:[MHRUtilities newRectangleLayer:CGRectMake(x1, y0, 5, rect.height+5) pListKey:colorKey]];
        });
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



    #pragma mark - Processing each recorded frame
    - (void)processImage:(Mat &)image
    {
        static int framesWithTorchOn = 0;
        if (isCapturing)
        {
            if (!_cameraSwitch.isOn && (++framesWithTorchOn <= delayTorchOnInFrames))
                return;
            
            static int failedFrames = 0;
            if (_nFrames == 0)
                firstFrameWithFace = image(ROI_upper).clone();
            Mat new_image = image(cropArea);
            cvtColor(new_image, new_image, CV_BGRA2BGR);
            imwrite([_outPath UTF8String] + string("/input_frame[") + to_string(_nFrames) + string("].png"), new_image);
            [frameIndexArray addObject:[NSNumber numberWithInt:(int)_nFrames]];
            ++_nFrames;

            // Update the frame-rate
            _frameRate = ((float)_nFrames - 1) / (float)_recordTime;
                        
            // Wait until there are enough unprocessed frames for one block then add the block
            int size = (int)frameIndexArray.count;
            if ((size > 0) && (size % kBlockFrameSize == 0))
            {
                [myQueue addOperationWithBlock: ^ {
                    [self heartRateCalculation];
                }];
            }
            

            dispatch_async(dispatch_get_main_queue(), ^{
                [self drawFaceCaptureRect:cropArea withColorKey:@"MHRCameraCaptureRect"];
            });

            
            if (_cameraSwitch.isOn) {
                if (fastMode)
                {
                    if (![auto_stop fastFaceCheck:image(ROI_upper).clone()])
                        ++failedFrames;
                    else
                        failedFrames = 0;
                }
                else
                {
                    if (![auto_stop slowFaceCheck:image.clone()])
                        ++failedFrames;
                    else
                        failedFrames = 0;
                }
            }
            else
            {
                if (![auto_stop fingerCheck:new_image])
                    ++failedFrames;
                else
                    failedFrames = 0;
            }
            
            if (failedFrames > 5) {
                failedFrames = 0;
                
                _nFrames -= 6;
                [frameIndexArray removeObjectsInRange:NSMakeRange(frameIndexArray.count - 6, 6)];
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self stopButtonDidTap:self];
                });
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
                    if (assessmentResult)
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
                if(cnt) return;
                
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
                                    
                                    if(isCapturing) return;
                                    
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
                    else {
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
        blockNumber = 0;
    }

    - (void)heartRateCalculation
    {
        int idxStart = blockNumber * kBlockFrameSize;
        if (idxStart >= frameIndexArray.count)
        {
            NSLog(@"Error: Block starts beyond frame count!");
            return;
        }
        
        int idxEnd = min((blockNumber + 1) * kBlockFrameSize, (int)frameIndexArray.count) - 1;
        if (isCapturing && ((idxEnd - idxStart + 1) < kBlockFrameSize))
        {
            NSLog(@"Error: Non-final block length is shorter than allowed");
            return;
        }

        NSNumber *startIndex = frameIndexArray[idxStart];
        NSNumber *endIndex = frameIndexArray[idxEnd];
        
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
        
            processingPerBlock([_outPath UTF8String], [_outPath UTF8String], startIndex.intValue, endIndex.intValue, isCalcMode, lower_range, upper_range, result, temp);
            processingCumulative(temporal_mean, temp, currentResult);
            
            isCalcMode = NO;
            blockNumber ++;
            
            if (_DEBUG_MODE)
            {
                NSLog(@"currentResult: %lf, %lf", currentResult.autocorr, currentResult.pda);
                NSLog(@"hrGlobalResult: %lf, %lf", hrGlobalResult.autocorr, hrGlobalResult.pda);
                NSLog(@"Number of blocks processed: %d", blockNumber);
            }
        }
    }

    - (void)startThreads
    {
        _nFrames = 0;
        [frameIndexArray removeAllObjects];
        temporal_mean.clear();
        isCalcMode = YES;
        blockNumber = 0;
    }

@end
