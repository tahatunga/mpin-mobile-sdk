//
//  UIViewController+Helper.m
//  MPinApp
//
//  Created by Georgi Georgiev on 2/11/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "UIViewController+Helper.h"
#import "Constants.h"

@implementation UIViewController (Helper)

- (void) invalidateNavBar {
    UIBarButtonItem * newBackButton = [[UIBarButtonItem alloc]  initWithImage:[UIImage imageNamed:kBackBarButtonItem]  style:UIBarButtonItemStylePlain target:nil action:nil];
    self.navigationItem.leftBarButtonItem = newBackButton;
}

-(void) showError:(NSString *) title desc:(NSString *) desc {
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:title message:desc delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil];
    [alert show];
}


@end
