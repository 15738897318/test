//
//  UIView+Position.h
//  Pulsar
//
//  Created by Bao Nguyen on 7/11/14.
//  Copyleft 2010 Bynomial.
//
//  These properties enable repositioning of a UIView
//  by changing any single coordinate or size parameter.
//  Don't forget that you can also use UIView's autoresizing
//  mask to achieve some automatic layout.
//
//  Sample usage:
//
//   myView.frameX += 10;  // Move 10 points right.
//   [myView addCenteredSubview:aSubView];
//

#import <UIKit/UIKit.h>
#import "MHRUtilities.h"

@interface UIView (Position)

@property (nonatomic) CGPoint frameOrigin;
@property (nonatomic) CGSize frameSize;

@property (nonatomic) CGFloat frameX;
@property (nonatomic) CGFloat frameY;

// Setting these modifies the origin but not the size.
@property (nonatomic) CGFloat frameRight;
@property (nonatomic) CGFloat frameBottom;

@property (nonatomic) CGFloat frameWidth;
@property (nonatomic) CGFloat frameHeight;

// Methods for centering.
- (void)addCenteredSubview:(UIView *)subview;
- (void)moveToCenterOfSuperview;
- (void)centerVerticallyInSuperview;
- (void)centerHorizontallyInSuperview;

- (void)setFrameXPlus:(CGFloat)dX yPlus:(CGFloat)dY widthPlus:(CGFloat)dWidth heightPlus:(CGFloat)dHeight;

- (void)adjustFrameFormiOS6ToiOS7:(CGFloat)distance;
- (void)adjustFrameFormiOS7ToiOS6:(CGFloat)distance;

@end
