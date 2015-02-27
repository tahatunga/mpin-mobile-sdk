//
//  AccessNumberViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 1/22/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "AccessNumberViewController.h"
#import "BackButton.h"
#import "UIView+Helper.m"

#define ACCESSNUMBER_LENGTH 7

@interface AccessNumberViewController ()
- (void) clear;
-(IBAction)btnBackTap:(id)sender;
@property(nonatomic, weak) IBOutlet UITextField *txtAN;


@end

@implementation AccessNumberViewController

- (void) viewDidLoad
{
    [super viewDidLoad];
    max = ACCESSNUMBER_LENGTH;
    BackButton *btnBack = [[BackButton alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:@selector(btnBackTap:)];
    [btnBack setup];

    self.navigationItem.leftBarButtonItem = btnBack;
}

-(void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    ThemeManager *themeManager = [[ThemeManager alloc] init];
    [themeManager beautifyViewController:self];
    _lblEmail.text = _strEmail;
    _txtAN.text = @"";
    [_txtAN setBottomBorder:[[SettingsManager sharedManager] color10] width:2.f alpha:.5f];
}

- (IBAction)logInAction:(id)sender {
    if( ( self.delegate != nil ) && ( [self.delegate respondsToSelector:@selector(onAccessNumber:)]) )
    {
        int an = [self.number intValue];
        if(an == 0) {
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"WORNING" message:@"Wrong Access Number" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
            [self clear];
            [alert show];
            return;
        }
        [self.delegate onAccessNumber:an];
    }
    [self.navigationController popViewControllerAnimated:NO];
}

-(IBAction)btnBackTap:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)numberSelectedAction:(id)sender    {
    UIButton * button = (UIButton *) sender;
    if (++numberIndex >= max) {
        [self disableNumButtons];
    }
    self.number = [self.number stringByAppendingString:button.titleLabel.text];
    self.txtAN.text =  [NSString stringWithFormat:@"%@ %@",self.txtAN.text, button.titleLabel.text];
}

- (void) clear {
    numberIndex = 0;
    self.number = @"";
    [self enableNumButtons];
    _txtAN.text = @"";
}

- (IBAction)clearAction:(id)sender {
    [self clear];
}
@end
