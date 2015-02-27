//
//  GraphicUtill.m
//  MPinApp
//
//  Created by Georgi Georgiev on 11/21/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "GraphicUtill.h"
#import <QuartzCore/QuartzCore.h>

@implementation GraphicUtill

+ (void) roundBorder:(UIView *) v withRadius:(NSInteger) r  colour:(UIColor *) colour width:(int) width{
    if (v== nil) return;
    v.clipsToBounds = YES;
    CALayer *layer = v.layer;
    [layer setMasksToBounds:YES];
    if(r>=0)    [layer setCornerRadius: r];
    if(width >= 0)  [layer setBorderWidth:width];
    if(colour != nil)   [layer setBorderColor:[colour CGColor]];
}

+ (void) addGradient:(UIView*) v colours:(NSArray *) colours  {
    if((v == nil) || (colours == nil)) return;
    CAGradientLayer *gradient = [CAGradientLayer layer];
    gradient.frame = v.bounds;
    gradient.colors = colours;
    [v.layer insertSublayer:gradient atIndex:0];
}

@end
