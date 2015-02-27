//
//  SettingsViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 1/19/15.
//  Copyright (c) 2015 Certivox. All rights reserved.
//

#import "SettingsViewController.h"
#import "SettingsViewCell.h"
#import "SettingsData.h"
#import "AddSettingViewController.h"
#import "AppDelegate.h"
#import "Constants.h"
#import "ATMHud.h"
#import "MPin.h"

@interface SettingsViewController () {
    NSInteger currentSelectedIndex;
    ATMHud *hud;
}

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
    self.navigationController.navigationBar.barTintColor = [UIColor colorWithRed:0.59 green:0.6 blue:0.72 alpha:1];
    self.navigationController.navigationBar.titleTextAttributes = [NSDictionary dictionaryWithObject:[UIColor whiteColor] forKey:NSForegroundColorAttributeName];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView                               {   return [self.settings count];   }
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section        {    return 1;  }
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath  {    return 150.f;   }

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath  {
    static NSString *SettingsTableIdentifier = @"SettingsCell";
    SettingsViewCell *cell = [tableView dequeueReusableCellWithIdentifier:SettingsTableIdentifier];
    if (cell == nil)    cell = [[SettingsViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:SettingsTableIdentifier];
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    [cell invalidate:self.settings atIndex:indexPath.section];
    if(currentSelectedIndex == indexPath.section)   cell.backgroundColor = [UIColor colorWithHexString:@"C0C0C0"];
    else cell.backgroundColor = [UIColor colorWithHexString:@"F0F0F0"];
    return cell;
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (currentSelectedIndex == indexPath.section)
    {
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
    }
    else
    {
        cell.accessoryType = UITableViewCellAccessoryNone;
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (currentSelectedIndex == indexPath.section) return;
    currentSelectedIndex = indexPath.section;
    [hud showInView:self.view];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSDictionary * dataDict = [self.settings objectAtIndex:indexPath.section];
        [MPin SetBackend:[dataDict objectForKey:kRPSURL]];
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [hud hide];
            [tableView reloadData];
        });
    });
}

-(void) viewWillAppear:(BOOL)animated {     [self.tableView reloadData];    }

- (void) viewWillDisappear:(BOOL)animated {
    [[NSUserDefaults standardUserDefaults] setObject:self.settings forKey:kSettings];
    [[NSUserDefaults standardUserDefaults] setInteger:currentSelectedIndex forKey:kCurrentSelectionIndex];
    [[NSUserDefaults standardUserDefaults] synchronize];
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

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath {  return UITableViewCellEditingStyleDelete;   }
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {    return indexPath.section != 0;   }
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






@end
