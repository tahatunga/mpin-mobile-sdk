//
//  OTPViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 12/19/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "OTPViewController.h"
#import "CycleProgressBar.h"
#import "SettingsManager.h"
#import "ThemeManager.h"
#import "BackButton.h"

@interface OTPViewController ()
{
    ThemeManager *themeManager;
    BackButton *btnBack;
}
- (IBAction) back:(UIBarButtonItem *)sender;

@property (nonatomic, retain, readwrite)	CycleProgressBar * cpb;

- (void) onProgressBarFinish:(id)sender;
@end

@implementation OTPViewController
- (void)viewDidLoad
{
    [super viewDidLoad];
    themeManager = [[ThemeManager alloc] init];
    self.navigationController.navigationBar.barTintColor = [[SettingsManager sharedManager] colorNavigationBar];
    self.navigationController.navigationBar.tintColor = [[SettingsManager sharedManager] colorNavigationBarText];
    [self.navigationController.navigationBar
     setTitleTextAttributes:@{NSForegroundColorAttributeName : [[SettingsManager sharedManager] colorNavigationBarText],
                              NSFontAttributeName : [UIFont fontWithName:@"OpenSans" size:18.0f]}];
    self.navigationController.navigationBar.translucent = NO;
    
    self.title = @"One time password";
    self.otp.text = self.otpData.otp;
    CGRect screenRect = [[UIScreen mainScreen] bounds];
     self.cpb = [[CycleProgressBar alloc] initWithFrame:CGRectMake(screenRect.size.width/2 - 60.0, (2*screenRect.size.height)/3 - 60.0, 120.0, 120.0)];
    [self.cpb addTarget:self action:@selector(onProgressBarFinish:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.cpb];
    btnBack = [[BackButton alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:@selector(back:)];
}
- (void) viewWillAppear:(BOOL)animated
{
    [self.cpb startAnimation:self.otpData.ttlSeconds];
    [themeManager beautifyViewController:self];
    _lblEmail.text = _strEmail;
    
    [btnBack setup];
    
    self.navigationItem.leftBarButtonItem = btnBack;
}
- (void) viewDidDisappear:(BOOL)animated    {    [self.cpb stopAnimation];  }
- (void) onProgressBarFinish:(id)sender {   [self.navigationController popViewControllerAnimated:NO];   }

-(IBAction)OnClickNavButton:(id)sender {      [self.navigationController popToRootViewControllerAnimated:YES];  }

- (IBAction) back:(UIBarButtonItem *)sender
{
    
    [self.navigationController popViewControllerAnimated:YES];
    
}
@end
