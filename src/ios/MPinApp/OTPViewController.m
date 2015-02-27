//
//  OTPViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 12/19/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "OTPViewController.h"
#import "CycleProgressBar.h"

@interface OTPViewController ()
@property (nonatomic, retain, readwrite)	CycleProgressBar * cpb;
- (void) onProgressBarFinish:(id)sender;
@end

@implementation OTPViewController
- (void)viewDidLoad
{
    [super viewDidLoad];
    self.otp.text = self.otpData.otp;
    CGRect screenRect = [[UIScreen mainScreen] bounds];
     self.cpb = [[CycleProgressBar alloc] initWithFrame:CGRectMake(screenRect.size.width / 2 - 25.0, screenRect.size.height / 2 - 25.0, 50.0, 44.0)];
    [self.cpb addTarget:self action:@selector(onProgressBarFinish:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.cpb];
}
- (void) viewWillAppear:(BOOL)animated  {   [self.cpb startAnimation:90];   }
- (void) viewDidDisappear:(BOOL)animated    {    [self.cpb stopAnimation];  }
- (void) onProgressBarFinish:(id)sender {   [self.navigationController popViewControllerAnimated:NO];   }
@end
