    //
//  AddIdentityViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 11/20/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "AddIdentityViewController.h"
#import "AFNetworkReachabilityManager.h"
#import "Constants.h"
#import "IUser.h"
#import "UIViewController+Helper.h"


static NSString * const kEmpty = @"";
static NSString * const kMpinStatus = @"MpinStatus";
static NSString * const kUser = @"User";

@interface AddIdentityViewController()
{

}


-(void) showDeviceName;
- (IBAction)addAction:(id)sender;
- (IBAction)back:(id)sender;
@end

@implementation AddIdentityViewController

- (void) viewDidLoad
{
    [super viewDidLoad];
//    self.txtDevName.text = kDevName;
    
    [themeManager  beautifyViewController:self];
    
    NSInteger selectedIndex = [[NSUserDefaults standardUserDefaults]
                               integerForKey:kCurrentSelectionIndex];
    
    NSArray *settings = [[NSUserDefaults standardUserDefaults] objectForKey:kSettings];
    NSDictionary * setting = [settings objectAtIndex:selectedIndex];
    NSNumber * isDevName = [setting objectForKey:kIS_DN];
    self.txtDevName.hidden = ((isDevName != nil) && ([isDevName boolValue]))?(NO):(YES);
    self.lblDevName.hidden = ((isDevName != nil) && ([isDevName boolValue]))?(NO):(YES);
}

-(void) showDeviceName
{
    
}

- (IBAction)addAction:(id)sender {
    if([kEmpty isEqualToString:self.txtIdentity.text])
    {
        
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Empty User Identity" message:@"Enter text in user id text field!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
        [alert show];
        return;
    }

    if (![self isValidEmail:self.txtIdentity.text])
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Invalid email"
                                                        message:@"Please enter a valid email address!"
                                                       delegate:nil
                                              cancelButtonTitle:@"Close"
                                              otherButtonTitles:nil, nil];
        [alert show];
        return;
    }
    [self startLoading];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        id<IUser> iuser;
        if ([self.txtDevName.text isEqualToString:kEmpty] || [self.txtDevName.text isEqualToString:kDevName]) {
            iuser= [MPin MakeNewUser:self.txtIdentity.text];
        } else {
            iuser= [MPin MakeNewUser:self.txtIdentity.text deviceName:self.txtDevName.text];
        }
        
        MpinStatus* mpinStatus = [MPin StartRegistration:iuser];
        
        dispatch_async(dispatch_get_main_queue(), ^(void) {
           
            switch (mpinStatus.status)
            {
                case OK:
                    switch ([iuser getState]) {
                        case STARTED_REGISTRATION:
                            [self showConfirmationScreen:iuser];
                            break;
                        case ACTIVATED:
                            [self finishRegistration:iuser];
                            break;
                        default:
                            [self stopLoading];
                            [self showError:[mpinStatus getStatusCodeAsString] desc:mpinStatus.errorMessage];
                            break;
                    }
                    break;
                    
                default:
                    [self stopLoading];
                    [self showError:[mpinStatus getStatusCodeAsString] desc:mpinStatus.errorMessage];
                break;
            }
        });
    });
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField  {
    [textField resignFirstResponder];
    return YES;
}
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField    {    return YES;    }

- (BOOL) isValidEmail:(NSString*) emailString {
    
    if([emailString length]==0 || [emailString rangeOfString:@" "].location != NSNotFound)
    {
        return NO;
    }
    
    NSString *regExPattern = @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}";

    NSRegularExpression *regEx = [[NSRegularExpression alloc] initWithPattern:regExPattern options:NSRegularExpressionCaseInsensitive error:nil];
    NSUInteger regExMatches = [regEx numberOfMatchesInString:emailString options:0 range:NSMakeRange(0, [emailString length])];
    
    if (regExMatches == 0)
    {
        return NO;
    }
    else
    {
        return YES;
    }
}

- (IBAction)back:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

/// TODO :: to be removed when new design is ready
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    [self.navigationController popToRootViewControllerAnimated:YES];
}


@end
