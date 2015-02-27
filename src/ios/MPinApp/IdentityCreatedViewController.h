//
//  IdentityCreatedViewController.h
//  MPinApp
//
//  Created by Tihomir Ganev on 27.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface IdentityCreatedViewController : UIViewController

@property(nonatomic,strong) NSString *strEmail;
@property(nonatomic,weak) IBOutlet UILabel *lblEmail;
@property(nonatomic,weak) IBOutlet UILabel *lblMessage;

@end
