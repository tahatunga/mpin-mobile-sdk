//
//  SettingsViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 1/19/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "SettingsViewController.h"
#import "ConfigListTableViewCell.h"
#import "SettingsData.h"
#import "AddSettingViewController.h"
#import "AppDelegate.h"
#import "Constants.h"
#import "ATMHud.h"
#import "MPin.h"
#import "UIViewController+Helper.h"
#import "MenuViewController.h"

#define NONE    0
#define OTP     1
#define AN      2

@interface SettingsViewController () {
    NSInteger currentSelectedIndex;
    ATMHud *hud;
}
-(IBAction)gotoIdentityList:(id)sender;
@property (nonatomic, retain) NSMutableArray * settings;
@end

@implementation SettingsViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    hud = [[ATMHud alloc] initWithDelegate:self];
    currentSelectedIndex = [[NSUserDefaults standardUserDefaults] integerForKey:kCurrentSelectionIndex];
    self.settings = [[NSUserDefaults standardUserDefaults] objectForKey:kSettings];
    
    if(self.settings == NULL)  {
        self.settings = [NSMutableArray array];
        [[NSUserDefaults standardUserDefaults] setObject:self.settings forKey:kSettings];
    } else { self.settings = [NSMutableArray arrayWithArray:self.settings]; }

}



- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.settings count];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 60.f;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *SettingsTableIdentifier = @"ConfigListTableViewCell";
    ConfigListTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:SettingsTableIdentifier];
    if (cell == nil)    cell = [[ConfigListTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:SettingsTableIdentifier];
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    cell.lblConfigurationName.font = [UIFont fontWithName:@"OpenSans" size:16.f];
    cell.lblConfigurationType.font = [UIFont fontWithName:@"OpenSans" size:14.f];
    cell.lblConfigurationName.textColor = [[SettingsManager sharedManager] color6];
    cell.lblConfigurationType.textColor = [[SettingsManager sharedManager] color6];
    [cell setIsSelectedImage:(currentSelectedIndex != indexPath.row)];
    return cell;
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
    ConfigListTableViewCell *customCell = (ConfigListTableViewCell *)cell;
    customCell.lblConfigurationName.text = [[self.settings objectAtIndex:indexPath.row] objectForKey:kCONFIG_NAME];
    switch (indexPath.row) {
        case NONE:
            customCell.lblConfigurationType.text = @"Local Authentication";
            break;
        case OTP:
            customCell.lblConfigurationType.text = @"One Time Password";
            break;
        case AN:
            customCell.lblConfigurationType.text = @"Access Number";
            break;
        default:
            break;
    }
   [customCell setIsSelectedImage:(currentSelectedIndex == indexPath.row)];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (currentSelectedIndex == indexPath.row) return;
    currentSelectedIndex = indexPath.row;
    [hud setCaption:@"Changing configuration. Please wait."];
    [hud setActivity:YES];
    [hud showInView:self.view];
    NSMutableDictionary * dataDict = [NSMutableDictionary dictionaryWithDictionary:[self.settings objectAtIndex:currentSelectedIndex]];
    switch (currentSelectedIndex) {
        case NONE:
            [dataDict setObject:[NSNumber numberWithBool:NO] forKey:kIS_OTP];
            [dataDict setObject:[NSNumber numberWithBool:NO] forKey:kIS_ACCESS_NUMBER];
            break;
        case OTP:
            [dataDict setObject:[NSNumber numberWithBool:YES] forKey:kIS_OTP];
            [dataDict setObject:[NSNumber numberWithBool:NO] forKey:kIS_ACCESS_NUMBER];
            break;
        case AN:
            [dataDict setObject:[NSNumber numberWithBool:NO] forKey:kIS_OTP];
            [dataDict setObject:[NSNumber numberWithBool:YES] forKey:kIS_ACCESS_NUMBER];
            break;
        default:
            break;
    }
    [self.settings replaceObjectAtIndex:currentSelectedIndex withObject:dataDict];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        MpinStatus * status = [MPin SetBackend:[dataDict objectForKey:kRPSURL] rpsPrefix:[dataDict objectForKey:kRPSPrefix]];
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            if (status.status != OK) {
                [self showError:[status getStatusCodeAsString] desc:status.errorMessage];
            }
            [hud hide];
            [[NSUserDefaults standardUserDefaults] setObject:self.settings forKey:kSettings];
            [[NSUserDefaults standardUserDefaults] setInteger:currentSelectedIndex forKey:kCurrentSelectionIndex];
            [[NSUserDefaults standardUserDefaults] synchronize];
            [tableView reloadData];
        });
    });
}

-(void) viewWillAppear:(BOOL)animated {     [self.tableView reloadData];    }

- (void) viewWillDisappear:(BOOL)animated
{
}

-(IBAction)add:(id)sender {
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main_iPhone" bundle:nil];
    AddSettingViewController * addViewController = [storyboard instantiateViewControllerWithIdentifier:@"AddConfig"];
    addViewController.confSettings = self.settings;
    [self.navigationController pushViewController:addViewController animated:YES];
}

-(IBAction)edit:(id)sender {
    UIBarButtonItem * button = (UIBarButtonItem *) sender;
    if([@"Edit" isEqualToString: [button title]]) {
        [self.tableView setEditing:YES animated:YES];
        [button setTitle:@"Done"];
    } else {
        [self.tableView setEditing:NO animated:YES];
        [button setTitle:@"Edit"];
    }
}
//
//- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView
//           editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
//{
//    return UITableViewCellEditingStyleDelete;
//}

- (BOOL)tableView :(UITableView *)tableView canEditRowAtIndexPath :(NSIndexPath *)indexPath
{
    return NO;//indexPath.section != 0;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 0)     return;
    if( currentSelectedIndex == indexPath.section )   currentSelectedIndex = 0;
    else if (indexPath.section < currentSelectedIndex ) currentSelectedIndex--;
    else {}
    [[NSUserDefaults standardUserDefaults] setInteger:currentSelectedIndex forKey:kCurrentSelectionIndex];
    [self.settings removeObjectAtIndex:indexPath.section];
    [self.tableView reloadData];
}



-(IBAction)gotoIdentityList:(id)sender
{
    MenuViewController *menuVC = (MenuViewController *)self.menuContainerViewController.leftMenuViewController;
    
    [menuVC setCenterWithID:0];
    
}


@end
