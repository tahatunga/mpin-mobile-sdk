//
//  ViewController.h
//  MPinSDK
//
//  Created by Georgi Georgiev on 11/14/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DigitPadViewController.h"

static NSString * const kOnFinishShowingPinPadNotification = @"onFinishShowingPinPad";

@interface PinPadViewController : DigitPadViewController

@property (nonatomic, weak) IBOutlet UIView * pinView;
@property(nonatomic, weak) IBOutlet UILabel *lblEmail;
@property(nonatomic, weak) UILabel *strEmail;

@property(nonatomic) BOOL boolShouldShowBackButton;
@property(nonatomic, retain, readwrite) NSString * userId;

@end
