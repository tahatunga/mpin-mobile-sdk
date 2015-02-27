//
//  MPinSDK.m
//  MPinSDK
//
//  Created by Georgi Georgiev on 11/17/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "MPin.h"
#import "mpin_sdk.h"
#import "def.h"
#import "Context.h"
#import <vector>
#import "User.h"
#import "Constants.h"

static MPinSDK mpin;

/// TEMPORARY FIX
static NSString * rpsURL;


typedef MPinSDK::UserPtr UserPtr;
typedef MPinSDK::Status Status;
typedef sdk::Context Context;

@implementation MPin

/// TEMPORARY FIX
+(NSString * ) getRPSUrl {
    return rpsURL;
}

+(MpinStatus *) initWithConfig:(NSDictionary*) config {
    /// TEMPORARY FIX
    rpsURL = [config objectForKey:kRPSURL];
    ///
    StringMap sm;
    sm[([kRPSURL UTF8String])] = [rpsURL UTF8String];
    Status s =  mpin.Init(sm, Context::Instance());
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+(MpinStatus *) TestBackend:(const NSString * ) url {
    Status s = mpin.TestBackend([url UTF8String]);
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+(MpinStatus *) SetBackend:(const NSString * ) url {
    Status s = mpin.SetBackend([url UTF8String]);
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (id<IUser>) MakeNewUser:(const NSString *) identity {
    UserPtr userPtr = mpin.MakeNewUser([identity UTF8String]);
    return [[User alloc] initWith:userPtr];
}

+(void) DeleteUser:(const id<IUser>) user {
    mpin.DeleteUser([((User *) user) getUserPtr]);
}

+ (MpinStatus*) StartRegistration:(const  id<IUser>) user {
    Status s = mpin.StartRegistration([((User *) user) getUserPtr]);
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) RestartRegistration:(const id<IUser>) user {
    Status s = mpin.RestartRegistration([((User *) user) getUserPtr]);
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) FinishRegistration:(const id<IUser>) user {
    Status s = mpin.FinishRegistration([((User *) user) getUserPtr]);
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}
+ (MpinStatus*) Authenticate:(const id<IUser>) user {
    Status s = mpin.Authenticate([((User *) user) getUserPtr]);
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];;
}

+ (MpinStatus*) Authenticate:(id<IUser>) user otp:(OTP **) otp {
    MPinSDK::OTP c_otp;
    Status s = mpin.Authenticate([((User *) user) getUserPtr], c_otp);
    *otp = [[OTP alloc] initWith:[[MpinStatus alloc] initWith:(MPinStatus)c_otp.status.GetStatusCode() errorMessage:[NSString stringWithUTF8String:c_otp.status.GetErrorMessage().c_str()]]
                             otp:[NSString stringWithUTF8String:c_otp.otp.c_str()]
                      expireTime:c_otp.expireTime
                      ttlSeconds:c_otp.ttlSeconds
                         nowTime:c_otp.nowTime];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus *) AuthenticateAccessNumber:(id<IUser>) user  accessNumber:(int) an {
     Status s = mpin.AuthenticateAccessNumber([((User *) user) getUserPtr], an);
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (Boolean) Logout:(const  id<IUser>) user {
    return mpin.Logout([((User *) user) getUserPtr]);
}

+ (Boolean) CanLogout:(const  id<IUser>) user {
    return mpin.CanLogout([((User *) user) getUserPtr]);;
}

+(NSMutableArray *) listUsers {
    NSMutableArray * users = [NSMutableArray array];
    std::vector<UserPtr> vUsers;
    mpin.ListUsers(vUsers);
    for (int i = 0; i<vUsers.size(); i++) {
        [users addObject:[[User alloc] initWith:vUsers[i]]];
    }
    return users;
}

+(void) sendPin:(int) pin {
    Context *ctx = Context::Instance();
    ctx->setPin(pin);
}

@end
