//
//  BaseViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 12/1/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MpinStatus.h"
#import "MPin.h"

#define SUCCESSFUL_AUTHENTICATION 200
#define USER_ACTIVATION_REQUIRED    403

#define GOTO_IDENTITY_LIST 0
#define ICONFIRM    1
#define RESEND      2

static  NSString * kSetupPin =  @"Setup new PIN";
static  NSString * kEnterPin = @"Enter your PIN";


@interface BaseViewController : UIViewController <UIAlertViewDelegate> {
    id<IUser> currentUser;
    NSString * pinpadHeaderTitle;
}

@property(nonatomic,retain, readwrite) IBOutlet UIActivityIndicatorView * activity;
@property(nonatomic, retain, readwrite) IBOutlet UIButton * addButton;

-(void) showPinPad;
-(void) startLoading;
-(void) stopLoading;

-(void) finishRegistration:(id<IUser>) iuser;
-(void) onFinishRegistration:(MpinStatus*) mpinStatus;
-(void) sendReactivationEmail:(id<IUser>) iuser;

@end
