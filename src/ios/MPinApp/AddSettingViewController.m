//
//  AddSettingViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 1/20/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "AddSettingViewController.h"
#import "AppDelegate.h"
#import "Constants.h"
#import "MPin.h"
#import "ATMHud.h"

static NSString * const kErrorTitle = @"Validation ERROR!";


@interface AddSettingViewController () {
     ATMHud *hud;
}

@property (nonatomic, assign) id currentResponder;

- (void) startLoading;
- (void) stopLoading;

@end

@implementation AddSettingViewController

-(void) viewDidLoad
{
    [super viewDidLoad];
    self.navigationController.navigationBar.tintColor = [UIColor whiteColor];
    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(resignOnTap:)];
    [singleTap setNumberOfTapsRequired:1];
    [singleTap setNumberOfTouchesRequired:1];
    [self.view addGestureRecognizer:singleTap];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField  {
    [textField resignFirstResponder];
    return YES;
}
- (void)textFieldDidBeginEditing:(UITextField *)textField   {    self.currentResponder = textField;     }
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField    {   return YES;     }
- (void)resignOnTap:(id)sender  {   [self.currentResponder resignFirstResponder];   }

- (void) startLoading {
    hud = [[ATMHud alloc] initWithDelegate:self];
    [hud setCaption:@"Loading please wait!"];
    [hud setActivity:YES];
    [hud showInView:self.view];
}
- (void) stopLoading {
    [hud hide];
}

-(IBAction)onOTPValueChanged:(id)sender {   if([self.otp isOn] && [self.an isOn])  [self.an setOn:NO];  }
-(IBAction)onAccessNumberValueChanged:(id)sender    {   if([self.an isOn] && [self.otp isOn])  [self.otp setOn:NO];     }

-(IBAction)onSave:(id)sender {
    if ([self.url.text isEqualToString:@""]) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:kErrorTitle message:@"Empty rps url !" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
        [alert show];
        return;
    }

    if (![self isValidURL:_url.text]) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:kErrorTitle message:@"RPS URL is not valid!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
        [alert show];
        return;
    }
    
    [self startLoading];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        MpinStatus * mpinStatus = [MPin TestBackend:_url.text];
        
        if(mpinStatus.status == OK)  {
            NSDictionary * data = [NSDictionary  dictionaryWithObjectsAndKeys:  self.url.text, kRPSURL,
                                   [NSNumber numberWithBool:[self.otp isOn]], kIS_OTP,
                                   [NSNumber numberWithBool:[self.an isOn]], kIS_ACCESS_NUMBER,
                                   nil];
            
            if(self.confSettings != nil) {
                [self.confSettings addObject:data];
            } else NSLog(@"WORNING: confSettings Array is not set. The application will not funciton properly");
        }
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [self stopLoading];
            if(mpinStatus.status == OK){
                [self.navigationController popViewControllerAnimated:YES];
            }
            else {
                UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[mpinStatus getStatusCodeAsString] message:@"RPS URL does not point to a valid RPS Server!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                [alert show];
            }
        });
    });
}

- (BOOL) isValidURL:(NSString*) strURL {
    
    if([strURL length]==0)
    {
        return NO;
    }
    
    NSString *regExPattern = @"(http|https)://((\\w)*|([0-9]*)|([-|_])*)+([\\.|/]((\\w)*|([0-9]*)|([-|_])*))+";
    NSRegularExpression *regEx = [[NSRegularExpression alloc] initWithPattern:regExPattern options:NSRegularExpressionCaseInsensitive error:nil];
    NSUInteger regExMatches = [regEx numberOfMatchesInString:strURL options:0 range:NSMakeRange(0, [strURL length])];
    
    if (regExMatches == 0)
    {
        return NO;
    }
    else
    {
        return YES;
    }
}


@end
