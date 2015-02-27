//
//  UserListViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 11/19/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseViewController.h"
#import "AccessNumberViewController.h"

@interface UserListTableViewCell : UITableViewCell

@property(nonatomic, retain, readwrite) IBOutlet UILabel            *lblUserID;
@property(nonatomic, retain, readwrite) IBOutlet UIImageView        *imgViewUser;
@property(nonatomic, retain, readwrite) IBOutlet UIImageView        *imgViewSelected;

@end

@interface UserListViewController : BaseViewController <UITableViewDataSource, UITableViewDelegate, AccessNumberDelegate>
@property (nonatomic, retain, readwrite) NSMutableArray *users;

@end
