//
//  AddIdentityViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 11/20/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseViewController.h"

@interface AddIdentityViewController : BaseViewController <UITextFieldDelegate>
@property(nonatomic, retain, readwrite) IBOutlet UITextField *label;
- (IBAction)addAction:(id)sender;
@end
