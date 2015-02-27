//
//  AddSettingViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 1/20/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AddSettingViewController : UIViewController <UITextFieldDelegate>

@property (nonatomic, weak) IBOutlet UITextField * url;
@property(nonatomic,weak) IBOutlet UISwitch * otp;
@property(nonatomic,weak) IBOutlet UISwitch * an;
@property (nonatomic,assign) NSMutableArray * confSettings;

-(IBAction)onSave:(id)sender;
-(IBAction)onOTPValueChanged:(id)sender;
-(IBAction)onAccessNumberValueChanged:(id)sender;

@end
