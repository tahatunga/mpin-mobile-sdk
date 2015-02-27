//
//  DigitPadViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 1/22/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "DigitPadViewController.h"
#import "GraphicUtill.h"

#define ROUND_BUTTON_RADIUS    40
#define ROUND_BUTTON_WIDTH   2.0
#define ROUND_LABLE_RADIUS 10


@interface DigitPadViewController ()
@end

@implementation DigitPadViewController


- (void) renderUIButtons {
    for(UIButton *button in self.numButtonsCollection)  {
        [GraphicUtill roundBorder:button withRadius:ROUND_BUTTON_RADIUS colour:[UIColor blackColor] width:ROUND_BUTTON_WIDTH];
    }
    [GraphicUtill roundBorder:self.actionButton withRadius:ROUND_BUTTON_RADIUS colour:[UIColor blackColor] width:0];
    [GraphicUtill roundBorder:self.clearButton withRadius:ROUND_BUTTON_RADIUS colour:[UIColor blackColor]  width:0];
    [GraphicUtill roundBorder:self.label withRadius:ROUND_LABLE_RADIUS colour:[UIColor blackColor] width:ROUND_BUTTON_WIDTH];
    self.actionButton.enabled = FALSE;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self renderUIButtons];
    numberIndex = 0;
    self.number = @"";
}

-(void) enableNumButtons {
    for(UIButton * button in self.numButtonsCollection) {
        button.enabled = TRUE;
    }
    self.actionButton.enabled = FALSE;
}

-(void) disableNumButtons {
    for(UIButton * button in self.numButtonsCollection) {
        button.enabled = FALSE;
    }
    self.actionButton.enabled = YES;
}

- (IBAction)clearAction:(id)sender {
    numberIndex = 0;
    self.number = @"";
    self.label.text = @"";
    [self enableNumButtons];
}

- (IBAction)numberSelectedAction:(id)sender {
    UIButton * button = (UIButton *) sender;
    self.number = [self.number stringByAppendingString:button.titleLabel.text];
    self.label.text = self.number;
    if (++numberIndex >= max) {
        [self disableNumButtons];
    }
}

- (IBAction)logInAction:(id)sender
{
    
}

@end
