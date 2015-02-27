//
//  AddIdentityViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 11/20/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseViewController.h"
#import "BackButton.h"

@interface AddIdentityViewController : BaseViewController <UITextFieldDelegate>

@property(nonatomic, retain, readwrite) IBOutlet BackButton     *btnBack;
@property(nonatomic, retain, readwrite) IBOutlet UITextField    *txtIdentity;
@property(nonatomic, retain, readwrite) IBOutlet UITextField    *txtDevName;
@property(nonatomic, retain, readwrite) IBOutlet UILabel        *lblIdentity;
@property(nonatomic, retain, readwrite) IBOutlet UILabel        *lblDevName;

@end
