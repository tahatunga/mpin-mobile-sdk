//
//  AddIdentityViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 11/20/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "AddIdentityViewController.h"
#import "AFNetworkReachabilityManager.h"

static NSString * const kEmpty = @"";
static NSString * const kMpinStatus = @"MpinStatus";
static NSString * const kUser = @"User";

@interface AddIdentityViewController ()

-(void) addNewUser:(NSString*) userId;
-(void) onFinishAddingNewUser:(MpinStatus*) mpinStatus;

@end

@implementation AddIdentityViewController

- (void) viewDidLoad{
    [super viewDidLoad];
    self.navigationController.navigationBar.tintColor = [UIColor whiteColor];
}

- (IBAction)addAction:(id)sender {
    if([kEmpty isEqualToString:self.label.text]) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Empty User Identity" message:@"Enter text in user id text field!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
        [alert show];
        return;
    }

    if (![self isValidEmail:_label.text])
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
    [self performSelectorInBackground:@selector(addNewUser:) withObject:self.label.text];
}

-(void) addNewUser:(NSString*) userId {
    id<IUser> iuser = [MPin MakeNewUser:userId];
    MpinStatus *s = [MPin StartRegistration:iuser];
    NSDictionary * paramDict = [NSDictionary dictionaryWithObjectsAndKeys:iuser,kUser, s,kMpinStatus ,nil];
    [self performSelectorOnMainThread:@selector(onFinishAddingNewUser:) withObject:paramDict waitUntilDone:YES];
}

-(void) onFinishAddingNewUser:(NSDictionary *) paramDict {
    MpinStatus * mpinStatus = [paramDict objectForKey:kMpinStatus];
    if(mpinStatus.status != 0) {
        [self stopLoading];
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[mpinStatus getStatusCodeAsString] message:mpinStatus.errorMessage delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
        [alert show];
        return;
    }

    id<IUser> iuser = [paramDict objectForKey:kUser];
    [self performSelectorInBackground:@selector(finishRegistration:) withObject:iuser];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    [super alertView:alertView clickedButtonAtIndex:buttonIndex];
    if ((alertView.tag == SUCCESSFUL_AUTHENTICATION) ||
        ( (alertView.tag == USER_ACTIVATION_REQUIRED) && (buttonIndex == GOTO_IDENTITY_LIST) )    ) {
        [self.navigationController popViewControllerAnimated:YES];
    }
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField  {
    [textField resignFirstResponder];
    return YES;
}
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField    {    return YES;    }

- (BOOL) isValidEmail:(NSString*) emailString {
    
    if([emailString length]==0)
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

@end
