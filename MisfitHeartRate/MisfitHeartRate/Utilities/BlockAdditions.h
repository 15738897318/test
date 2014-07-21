//
//  BlockAdditions.h
//  SensorsCapture
//
//  Created by Bao Nguyen on 6/18/14.
//  Copyright (c) 2014 Misfit Wearables. All rights reserved.
//
//  ref: https://github.com/MugunthKumar/UIKitCategoryAdditions/blob/master/MKAdditions/MKBlockAdditions.h
//


typedef void (^VoidBlock)();

typedef void (^DismissBlock)(int buttonIndex);
typedef void (^CancelBlock)();
typedef void (^PhotoPickedBlock)(UIImage *chosenImage);

#define kPhotoActionSheetTag 10000
