//
//  BaseViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 12/1/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "BaseViewController.h"
#import "PinPadViewController.h"
#import "Constants.h"
#import "ATMHud.h"

static NSString * const kConfirm = @"I confirmed my email";
static NSString * const kResend = @"Resend confirmation e-mail";
static NSString * const kGotoIdList = @"Go to the identities list";

@interface BaseViewController ()
{
    ATMHud *hud;
}
@end

@implementation BaseViewController


- (void)viewDidLoad
{
    [super viewDidLoad];
    self.automaticallyAdjustsScrollViewInsets = NO;
    self.activity.hidden = YES;
    pinpadHeaderTitle = kSetupPin;
    
    hud = [[ATMHud alloc] initWithDelegate:self];
    [hud setActivity:YES];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    self.addButton = nil;
    self.activity = nil;
}

-(void) startLoading {
    
    [hud showInView:self.view];

}

-(void) stopLoading {
    
    [hud hide];
}

-(void) finishRegistration:(id<IUser>) iuser {
    currentUser = iuser;
    MpinStatus * s = [MPin FinishRegistration:iuser];
    [self performSelectorOnMainThread:@selector(onFinishRegistration:) withObject:s waitUntilDone:NO];
}

-(void) onFinishRegistration:(MpinStatus*) mpinStatus {
    NSString * status = [mpinStatus getStatusCodeAsString];
    NSString * description = mpinStatus.errorMessage;
    UIAlertView *alert = nil;
    if(mpinStatus.status == NETWORK_ERROR) {
        alert = [[UIAlertView alloc] initWithTitle:@"Setup new identity..."
                                           message:[NSString stringWithFormat:@"Your M-Pin identity:\n %s \n has not been activated via the M-Pin email we send you.",
                                                    [[currentUser getIdentity] UTF8String]]
                                          delegate:self
                                 cancelButtonTitle:kGotoIdList
                                 otherButtonTitles:kConfirm, kResend, nil];
        alert.tag = USER_ACTIVATION_REQUIRED;
    } else {
        alert = [[UIAlertView alloc] initWithTitle:status message:description delegate:self cancelButtonTitle:@"Close" otherButtonTitles:nil];
        if(mpinStatus.status == OK)     alert.tag = SUCCESSFUL_AUTHENTICATION;
    }

    [alert show];
    [self stopLoading];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if(alertView.tag == USER_ACTIVATION_REQUIRED) {
        if(buttonIndex == ICONFIRM) {
            [self startLoading];
            [self performSelectorInBackground:@selector(finishRegistration:) withObject:currentUser];
            return;
        }
        if(buttonIndex == RESEND) {
            [self startLoading];
            [self performSelectorInBackground:@selector(sendReactivationEmail:) withObject:currentUser];
            return;
        }
    } 
}

-(void) sendReactivationEmail:(id<IUser>) iuser {
    currentUser = iuser;
    MpinStatus * s = [MPin RestartRegistration:iuser];
    [self performSelectorOnMainThread:@selector(onFinishRegistration:) withObject:s waitUntilDone:NO];
}

- (void) showPinPad {
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main_iPhone" bundle:nil];
    PinPadViewController * pinpadViewController = [storyboard instantiateViewControllerWithIdentifier:@"pinpad"];
    pinpadViewController.userId = [currentUser getIdentity];
    pinpadViewController.headerTitle = pinpadHeaderTitle;
    [self.navigationController pushViewController:pinpadViewController animated:NO];
}

- (void)viewDidAppear:(BOOL)animated {      [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showPinPad) name:kShowPinPadNotification object:nil];     }
- (void) viewDidDisappear:(BOOL)animated {      [[NSNotificationCenter defaultCenter] removeObserver:self name:kShowPinPadNotification object:nil];     }

@end
