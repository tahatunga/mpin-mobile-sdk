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

@interface UserListViewController : BaseViewController <UITableViewDataSource, UITableViewDelegate, AccessNumberDelegate>
@property (nonatomic, retain, readwrite) NSMutableArray * users;
@property(nonatomic, retain, readwrite) IBOutlet UITableView * table;

- (IBAction)addAction:(id)sender;
- (IBAction)editButton:(id)sender;


@end
