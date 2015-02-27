//
//  MenuViewController.h
//  MPinApp
//
//  Created by Tihomir Ganev on 6.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseViewController.h"


@interface MenuViewController : BaseViewController <UITableViewDelegate, UITableViewDataSource>

-(void) setCenterWithID: (int)vcId;
@end
