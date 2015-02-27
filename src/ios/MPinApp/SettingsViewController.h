//
//  SettingsViewController.h
//  MPinApp
//
//  Created by Georgi Georgiev on 1/19/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BaseViewController.h"

@interface SettingsViewController : BaseViewController <UITableViewDataSource,UITableViewDelegate, UITabBarDelegate>

@property(nonatomic, readwrite, weak) IBOutlet UITableView * tableView;

-(IBAction)add:(id)sender;
-(IBAction)edit:(id)sender;

@end
