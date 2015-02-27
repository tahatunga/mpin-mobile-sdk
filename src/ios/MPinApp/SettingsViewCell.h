//
//  SettingsViewCell.h
//  MPinApp
//
//  Created by Georgi Georgiev on 1/20/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
@interface SettingsViewCell : UITableViewCell
@property (nonatomic, readwrite, retain) IBOutlet UILabel * url;
@property(nonatomic,weak) IBOutlet UISwitch * otp;
@property(nonatomic,weak) IBOutlet UISwitch * an;

- (void) invalidate:(NSMutableArray *) settings atIndex:(NSInteger) index;

-(IBAction)onOTPValueChanged:(id)sender;
-(IBAction)onAccessNumberValueChanged:(id)sender;

@end
