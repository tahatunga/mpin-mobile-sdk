//
//  MenuViewController.m
//  MPinApp
//
//  Created by Tihomir Ganev on 6.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import "MenuViewController.h"
#import "MFSideMenu.h"
#import "AboutViewController.h"
#import "SettingsViewController.h"
#import "UserListViewController.h"
#import "AppDelegate.h"
#import "MenuTableViewCell.h"

#define USER_LIST 0
#define SETTINGS 1
#define ABOUT 2
@interface MenuViewController ()
{
    AboutViewController     *vcAbout;
    SettingsViewController  *vcSettings;
    UserListViewController  *vcUserList;

}

@property (nonatomic, weak) IBOutlet UITableView* tblMenu;
@property (nonatomic, weak) IBOutlet UILabel* lblAppVersion;

@end

@implementation MenuViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    // Tisho: App version and build version is initialized in BaseViewController
    _lblAppVersion.text  = [NSString stringWithFormat:@"Ver. %@",strAppVersion];
    _lblAppVersion.textColor = [UIColor whiteColor];
    self.view.backgroundColor = [UIColor colorWithRed:0.22 green:0.29 blue:0.31 alpha:1];
    _tblMenu.backgroundColor = [UIColor colorWithRed:0.22 green:0.29 blue:0.31 alpha:1];
    _lblAppVersion.font = [UIFont fontWithName:@"OpenSans-Bold" size:12.f];
    self.view.backgroundColor = [[SettingsManager sharedManager] colorSideMenuBackground];
    vcAbout     = [self.storyboard instantiateViewControllerWithIdentifier:@"AboutViewController"];
    AppDelegate *appDelegate  = [UIApplication sharedApplication].delegate;
    vcUserList  = appDelegate.vcUserList;
    vcSettings  = [self.storyboard instantiateViewControllerWithIdentifier:@"SettingsViewController"];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - UITableViewDataSource -

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 60.f;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
{
    return 3;
}

- (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath
{

    static NSString *userListTableIdentifier = @"MenuTableViewCell";
    MenuTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:userListTableIdentifier];
    if (cell == nil)    cell = [[MenuTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:userListTableIdentifier];
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.backgroundColor = [[SettingsManager sharedManager] colorSideMenuBackground];
    cell.lblMenuID.font = [UIFont fontWithName:@"OpenSans-Regular" size:14.f];
    cell.lblMenuID.textColor = [[SettingsManager sharedManager] colorButtonsText];
    cell.viewSeparator.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"border-line"]];

    return cell;


}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
    switch (indexPath.row) {
        case USER_LIST:
            ((MenuTableViewCell *)cell).lblMenuID.text = @"IDENTITY LIST";
            break;
            
        case SETTINGS:
            ((MenuTableViewCell *)cell).lblMenuID.text = @"CONFIGURATION LIST";
            break;
            
        case ABOUT:
            ((MenuTableViewCell *)cell).lblMenuID.text = @"ABOUT";
            break;
    }
}

#pragma mark -
#pragma mark - UITableViewDelegate

- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
{
    UIViewController* vc;

    [tableView deselectRowAtIndexPath:indexPath animated:YES];

    
    switch (indexPath.row) {
    case USER_LIST:
            vc = vcUserList;
        break;

    case SETTINGS:
            vc = vcSettings;
        break;

    case ABOUT:
            vc = vcAbout;
        break;
    }

    UINavigationController* navigationController = self.menuContainerViewController.centerViewController;
    NSArray* controllers = [NSArray arrayWithObject:vc];
    navigationController.viewControllers = controllers;
    [self.menuContainerViewController setMenuState:MFSideMenuStateClosed];
}


-(void) setCenterWithID: (int)vcId
{
    UIViewController* vc;
    switch (vcId) {
        case USER_LIST:
            vc = vcUserList;
            break;
            
        case SETTINGS:
            vc = vcSettings;
            break;
            
        case ABOUT:
            vc = vcAbout;
            break;
    }
    
    UINavigationController* navigationController = self.menuContainerViewController.centerViewController;
    NSArray* controllers = [NSArray arrayWithObject:vc];
    navigationController.viewControllers = controllers;
    [self.menuContainerViewController setMenuState:MFSideMenuStateClosed];

}

@end
