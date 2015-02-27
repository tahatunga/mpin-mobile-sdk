//
//  GraphicUtill.h
//  MPinApp
//
//  Created by Georgi Georgiev on 11/21/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface GraphicUtill : NSObject
+ (void) roundBorder:(UIView *) v withRadius:(NSInteger) r  colour:(UIColor *) colour width:(int) width;
+ (void) addGradient:(UIView*) v colours:(NSArray *) colours;
@end
