//
//  ConfirmEmailViewController.h
//  MPinApp
//
//  Created by Tihomir Ganev on 12.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseViewController.h"

@interface ConfirmEmailViewController : BaseViewController

@property(nonatomic, retain) IBOutlet UILabel *userId;
@property(nonatomic, retain) id<IUser> iuser;

-(IBAction)OnConfirmEmail:(id)sender;
-(IBAction)OnResendEmail:(id)sender;
-(IBAction)backToIDList:(id)sender;
@end
