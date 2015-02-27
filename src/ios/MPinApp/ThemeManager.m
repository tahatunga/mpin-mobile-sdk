//
//  ThemeManager.m
//  MPinApp
//
//  Created by Tihomir Ganev on 19.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import "ThemeManager.h"
#import "AboutViewController.h"
#import "AddIdentityViewController.h"
#import "AddSettingViewController.h"
#import "AccessNumberViewController.h"
#import "OTPViewController.h"
#import "UIView+Helper.h"

@interface ThemeManager()


@end

@implementation ThemeManager

-(void) beautifyViewController: (id) vc
{
    
    UIViewController *v = (UIViewController *)vc;
    
    v.navigationController.navigationBar.barTintColor = [[SettingsManager sharedManager] colorNavigationBar];
    v.navigationController.navigationBar.tintColor = [[SettingsManager sharedManager] colorNavigationBarText];
    [v.navigationController.navigationBar
     setTitleTextAttributes:@{NSForegroundColorAttributeName : [[SettingsManager sharedManager] colorNavigationBarText],
                              NSFontAttributeName : [UIFont fontWithName:@"OpenSans" size:18.0f]}];
    v.navigationController.navigationBar.translucent = NO;

    
    if ([vc isMemberOfClass:[AccessNumberViewController class]])
    {
        AccessNumberViewController *myVc = (AccessNumberViewController *)vc;
        myVc.lblEmail.textColor = [[SettingsManager sharedManager] color6];

    }
    else if ([vc isMemberOfClass:[AboutViewController class]])
    {
        
    }
    else if ([vc isMemberOfClass:[AddIdentityViewController class]])
    {
        AddIdentityViewController *myVc = (AddIdentityViewController *)vc;
        [myVc.txtIdentity setBottomBorder:[[SettingsManager sharedManager] color5] width:2.f alpha:.5f];
        [myVc.txtDevName setBottomBorder:[[SettingsManager sharedManager] color5]  width:2.f alpha:.5f];
        [myVc.btnBack setup];
    }
    else if ([vc isMemberOfClass:[AddSettingViewController class]])
    {
        AddSettingViewController *myVc = (AddSettingViewController *)vc;
        [myVc.txtName setBottomBorder:[[SettingsManager sharedManager] color10] width:2.f alpha:.5f];
        [myVc.txtURL setBottomBorder:[[SettingsManager sharedManager] color5]  width:2.f alpha:.5f];
        
    }else if ([vc isMemberOfClass:[OTPViewController class]])
    {
        OTPViewController *myVc = (OTPViewController *)vc;
        [myVc.lblEmail setBottomBorder:[[SettingsManager sharedManager] color10] width:2.f alpha:.5f];
        myVc.lblEmail.textColor = [[SettingsManager sharedManager] color6];
    }
    
    
}


@end
