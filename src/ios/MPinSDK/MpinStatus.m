//
//  MpinStatus.m
//  MPinSDK
//
//  Created by Georgi Georgiev on 11/24/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "MpinStatus.h"

@implementation MpinStatus

-(id) initWith:(MPinStatus)status errorMessage:(NSString*) error {
    self = [super init];
    if (self) {
        self.status = status;
        self.errorMessage = error;
    }
    return self;

}

- (NSString *) getStatusCodeAsString {
    NSString * result = @"";
    switch (self.status) {
        case OK:
            result = @"OK";
            break;
        case CRYPTO_ERROR:
            result = @"CRYPTO_ERROR";
            break;
        case STORAGE_ERROR:
            result = @"STORAGE_ERROR";
            break;
        case NETWORK_ERROR:
            result = @"NETWORK_ERROR";
            break;
        case FLOW_ERROR:
            result = @"FLOW_ERROR";
            break;
        default:
            break;
    }
    return result;
}

@end
