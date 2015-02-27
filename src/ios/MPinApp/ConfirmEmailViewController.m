//
//  ConfirmEmailViewController.m
//  MPinApp
//
//  Created by Tihomir Ganev on 12.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import "ConfirmEmailViewController.h"

@interface ConfirmEmailViewController ()

@property(nonatomic,weak) IBOutlet UILabel *lblMessage;

@end

@implementation ConfirmEmailViewController


- (void) viewWillAppear:(BOOL)animated {
    self.userId.text = [self.iuser getIdentity];
    
    self.lblMessage.text = [NSString stringWithFormat:@"We have sent you an email to: \r\n %@ \r\n Click the link in the email to confirm your identity and proceed.", [self.iuser getIdentity]];
}

- (void) showConfirmationScreen {   NSLog(@"Unsupported operation!");   }

- (void)onFinishRegistration:(MpinStatus*)mpinStatus {
    if (mpinStatus.status == IDENTITY_NOT_VERIFIED) {
        NSString* status = [mpinStatus getStatusCodeAsString];
        NSString* description = [NSString   stringWithFormat:@"%@ Please check your e-mail and follow the activation link!", mpinStatus.errorMessage];
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:status message:description delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil];
        [self stopLoading];
        [alert show];
        return;
    }
    
    [super onFinishRegistration:mpinStatus];
}

-(IBAction)OnConfirmEmail:(id)sender {
    [self finishRegistration:self.iuser];
}

-(IBAction)OnResendEmail:(id)sender {
    [hud setTitle:@"Sending Email ..."];
    [self startLoading];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        MpinStatus* mpinStatus = [MPin RestartRegistration:self.iuser];
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [self stopLoading];
            if (mpinStatus.status == OK && [self.iuser getState] == STARTED_REGISTRATION) {
                // TODO:::
                 [hud setTitle:@""];
//                [hud setTitle:@"E-mail has been sent!"];
//                [hud showInView:self.view];
//                [hud hideAfter:3.0];
            } else  {
                NSString* status = [mpinStatus getStatusCodeAsString];
                NSString* description = mpinStatus.errorMessage;
                UIAlertView* alert = [[UIAlertView alloc] initWithTitle:status message:description delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil];
                [alert show];
            }
        });
    });
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    [self.navigationController popToRootViewControllerAnimated:YES];
}

-(IBAction)backToIDList:(id)sender
{
    [self.navigationController popToRootViewControllerAnimated:YES];
}
@end
