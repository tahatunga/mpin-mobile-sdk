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

@property(nonatomic, retain, readwrite) IBOutlet UILabel *identity;
@property(nonatomic, retain, readwrite) IBOutlet UILabel * titles;

@property(nonatomic, retain, readwrite) NSString * userId;
@property(nonatomic, retain, readwrite) NSString * headerTitle;

@end
