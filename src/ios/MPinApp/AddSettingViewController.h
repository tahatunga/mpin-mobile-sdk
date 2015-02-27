//
//  AddSettingViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 1/20/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseViewController.h"

@interface AddSettingViewController : BaseViewController <UITextFieldDelegate>

@property (nonatomic, weak) IBOutlet UITextField *txtName;
@property (nonatomic, weak) IBOutlet UITextField *txtURL;
@property (nonatomic,assign) NSMutableArray * confSettings;

-(IBAction)onSave:(id)sender;
-(IBAction)onOTPValueChanged:(id)sender;
-(IBAction)onAccessNumberValueChanged:(id)sender;

@end
