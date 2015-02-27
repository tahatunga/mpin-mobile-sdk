//
//  SettingsViewCell.m
//  MPinApp
//
//  Created by Georgi Georgiev on 1/20/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "SettingsViewCell.h"
#import "Constants.h"

@interface SettingsViewCell ()

@property(atomic, assign) NSInteger cellIndex;
@property(nonatomic, assign) NSMutableArray * settings;
- (void) resetData;
@end

@implementation SettingsViewCell

- (void) resetData {
    NSDictionary * data = [NSDictionary  dictionaryWithObjectsAndKeys:  self.url.text, kRPSURL,
                           [NSNumber numberWithBool:[self.otp isOn]], kIS_OTP,
                           [NSNumber numberWithBool:[self.an isOn]], kIS_ACCESS_NUMBER,
                           [NSNumber numberWithBool:[self.devName isOn]], kIS_DN,
                           nil];
    [self.settings replaceObjectAtIndex:self.cellIndex withObject:data];
}

- (void) invalidate:(NSMutableArray *) settings atIndex:(NSInteger) index {
    self.settings = settings;
    self.cellIndex = index;
    NSDictionary * data = [settings objectAtIndex:index];
    self.url.text = [data objectForKey:kRPSURL];
    [self.otp       setOn:[[data objectForKey:kIS_OTP] boolValue]];
    [self.an        setOn:[[data objectForKey:kIS_ACCESS_NUMBER] boolValue]];
    [self.devName   setOn:[[data objectForKey:kIS_DN] boolValue]];
}

-(IBAction)onOTPValueChanged:(id)sender {
    if([self.otp isOn] && [self.an isOn])       [self.an setOn:NO];
    [self resetData];
}
-(IBAction)onAccessNumberValueChanged:(id)sender {
    if([self.an isOn] && [self.otp isOn])       [self.otp setOn:NO];
    [self resetData];
}

-(IBAction)onDeviceNameValueChanged:(id)sender {
     [self resetData];
}

@end
