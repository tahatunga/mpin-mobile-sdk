//
//  MpinStatus.h
//  MPinSDK
//
//  Created by Georgi Georgiev on 11/24/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MPinStatus)
{
    OK = 0,
    CRYPTO_ERROR,
    STORAGE_ERROR,
    NETWORK_ERROR,
    FLOW_ERROR
};

@interface MpinStatus : NSObject

@property(nonatomic,readwrite) MPinStatus status;
@property(nonatomic, retain, readwrite) NSString * errorMessage;

-(id) initWith:(MPinStatus)status errorMessage:(NSString*) error;
- (NSString *) getStatusCodeAsString;

@end
