//
//  ViewController.m
//  MPinSDK
//
//  Created by Georgi Georgiev on 11/14/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "PinPadViewController.h"
#import "MPin.h"

#define PIN_LENGTH 4

static NSMutableArray * kCircles;

@interface PinPadViewController ()

- (void) renderNumberTextField:(NSInteger) numberLenght;

@end

@implementation PinPadViewController

- (void) renderNumberTextField:(NSInteger) numberLenght {
    kCircles = [NSMutableArray arrayWithCapacity:PIN_LENGTH + 1];
    NSString * pinField = @"";
    for (int i=0; i<PIN_LENGTH; i++)  pinField = [pinField stringByAppendingString:@"◎"];
    [kCircles  addObject:pinField];
    for (int i=0; i<PIN_LENGTH; i++) {
        pinField = [pinField stringByReplacingCharactersInRange:NSMakeRange(i, 1) withString:@"◉"];
        [kCircles  addObject:pinField];
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    max = PIN_LENGTH;
    [self renderNumberTextField:PIN_LENGTH];
    self.identity.text = self.userId;
    self.titles.text = self.headerTitle;
    self.label.text = [kCircles objectAtIndex:0];
    self.navigationItem.hidesBackButton = YES;
    UIBarButtonItem *newBackButton = [[UIBarButtonItem alloc] initWithTitle:@"Back" style:UIBarButtonItemStyleBordered target:self action:@selector(back:)];
    self.navigationItem.leftBarButtonItem = newBackButton;

}

- (void) back:(UIBarButtonItem *)sender
{
    [MPin sendPin:1];
    [self.navigationController popViewControllerAnimated:YES];
}

-(void) viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
}
- (IBAction)logInAction:(id)sender {
    int int_pin = [self.number intValue];
    [MPin sendPin:int_pin];
    [self.navigationController popViewControllerAnimated:NO];
}

- (IBAction)clearAction:(id)sender {
    [super clearAction:sender];
    self.label.text = kCircles[numberIndex];
}

- (IBAction)numberSelectedAction:(id)sender {
    [super numberSelectedAction:sender];
     self.label.text = kCircles[numberIndex];
}

@end
