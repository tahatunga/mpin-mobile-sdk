//
//  OTPViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 12/19/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "OTP.h"

@interface OTPViewController : UIViewController
@property (nonatomic, retain, readwrite) IBOutlet UILabel * otp;
@property (nonatomic, weak) IBOutlet UILabel *lblEmail;
@property (nonatomic, strong) NSString *strEmail;
@property (nonatomic, retain, readwrite) OTP * otpData;
@property (nonatomic, weak) IBOutlet UIView *viewPreloaderContainer;
-(IBAction)OnClickNavButton:(id)sender;
@end
