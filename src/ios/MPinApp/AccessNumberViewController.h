//
//  AccessNumberViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 1/22/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PinPadViewController.h"

@protocol AccessNumberDelegate <NSObject>
-(void) onAccessNumber:(int) an;
@end

@interface AccessNumberViewController : DigitPadViewController

@property (weak) id <AccessNumberDelegate> delegate;
@property(nonatomic,weak) IBOutlet UILabel *lblEmail;
@property(nonatomic,strong) NSString *strEmail;

@end
