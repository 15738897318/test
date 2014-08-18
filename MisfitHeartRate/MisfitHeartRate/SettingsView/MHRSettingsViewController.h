//
//  MHRSettingsViewController.h
//  Pulsar
//
//  Created by Bao Nguyen on 8/18/14.
//  Copyright (c) 2014 Misfit Wearables Corporation. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol MHRSettingsViewDelegate <NSObject>

@required
- (void)debugModeChanged:(BOOL)mode;
- (void)threeChanModeChanged:(BOOL)mode;

@end


@interface MHRSettingsViewController : UIViewController
{
    id <MHRSettingsViewDelegate> _delegate;
}

@property (strong, nonatomic) id delegate;

@property (assign, nonatomic) BOOL debugModeOn;
@property (assign, nonatomic) BOOL threeChanModeOn;


@end
