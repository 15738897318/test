//
//  MHRSettingsViewController.m
//  Pulsar
//
//  Created by Bao Nguyen on 8/18/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#import "MHRSettingsViewController.h"

@interface MHRSettingsViewController ()

@property (weak, nonatomic) IBOutlet UISwitch *debugModeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *threeChanModeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *loadFromFileSwitch;

@end

@implementation MHRSettingsViewController

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
    // Do any additional setup after loading the view from its nib.
    [self.debugModeSwitch setOn:self.debugModeOn animated:NO];
    [self.threeChanModeSwitch setOn:self.threeChanModeOn animated:NO];
    [self.loadFromFileSwitch setOn:self.loadFromFileOn animated:NO];
    [self.delegate debugModeChanged:self.debugModeSwitch.isOn];
    [self.delegate threeChanModeChanged:self.threeChanModeSwitch.isOn];
    [self.delegate loadFromFileChanged:self.loadFromFileSwitch.isOn];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)switchDebugMode:(id)sender {
    [self.delegate debugModeChanged:self.debugModeSwitch.isOn];
}


- (IBAction)switchThreeChanMode:(id)sender {
    [self.delegate threeChanModeChanged:self.threeChanModeSwitch.isOn];
}

- (IBAction)switchLoadFromFileMode:(id)sender {
    [self.delegate loadFromFileChanged:self.loadFromFileSwitch.isOn];
}

@end
