//
//  AppDelegate.h
//  MPinApp
//
//  Created by Georgi Georgiev on 11/17/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "UserListViewController.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) UserListViewController *vcUserList;
@end
