//
//  UIViewController+Helper.h
//  MPinApp
//
//  Created by Georgi Georgiev on 2/11/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIViewController (Helper)

- (void) invalidateNavBar;
-(void) showError:(NSString *) title desc:(NSString *) desc;

@end
