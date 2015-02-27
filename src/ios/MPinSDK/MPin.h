//
//  MPinSDK.h
//  MPinSDK
//
//  Created by Georgi Georgiev on 11/17/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IUser.h"
#import "MpinStatus.h"
#import "OTP.h"

@interface MPin : NSObject

+(MpinStatus *) initWithConfig:(const NSDictionary*) config;
+(MpinStatus *) TestBackend:(const NSString * ) url;
+(MpinStatus *) SetBackend:(const NSString * ) url;
+(MpinStatus *) TestBackend:(const NSString * ) url rpsPrefix:(NSString *) rpsPrefix;
+(MpinStatus *) SetBackend:(const NSString * ) url rpsPrefix:(NSString *) rpsPrefix;
+ (id<IUser>) MakeNewUser: (const NSString *) identity;
+ (id<IUser>) MakeNewUser: (const NSString *) identity deviceName:(const NSString *) devName;
+(void) DeleteUser:(const id<IUser>) user;
+ (MpinStatus *) StartRegistration:(const  id<IUser>) user;
+ (MpinStatus *) RestartRegistration:(const id<IUser>) user;
+ (MpinStatus *) FinishRegistration:(const id<IUser>) user;
+ (MpinStatus *) Authenticate:(const id<IUser>) user;
+ (MpinStatus *) Authenticate:(id<IUser>) user  otp:(OTP **) otp;
+ (MpinStatus *) AuthenticateAccessNumber:(id<IUser>) user  accessNumber:(int) an;
+ (Boolean) Logout:(const  id<IUser>) user;
+ (Boolean) CanLogout:(const  id<IUser>) user;
+(NSMutableArray*) listUsers;
+(void) sendPin:(const NSString *) pin;

/// TEMPORARY FIX
+(NSString * ) getRPSUrl;

@end
