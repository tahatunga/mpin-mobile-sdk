//
//  AccessNumberViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 1/22/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "AccessNumberViewController.h"

#define ACCESSNUMBER_LENGTH 7

@interface AccessNumberViewController ()

@end

@implementation AccessNumberViewController

- (void) viewDidLoad {
    [super viewDidLoad];
    max = ACCESSNUMBER_LENGTH;
}

- (IBAction)logInAction:(id)sender {
    if((self.delegate != nil) && ([self.delegate respondsToSelector:@selector(onAccessNumber:)]))
       [self.delegate onAccessNumber:[self.number intValue]];
    [self.navigationController popViewControllerAnimated:NO];
}

@end
