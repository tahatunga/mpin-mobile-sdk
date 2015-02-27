//
//  UserListViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 11/19/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "UserListViewController.h"
#import "OTPViewController.h"
#import "GraphicUtill.h"
#import "IUser.h"
#import "Constants.h"
#import "ATMHud.h"

#define ON_LOGOUT 201
#define LOGOUT_BUTTON_INDEX 1


static NSString * const kMpinStatus = @"MpinStatus";
static NSString * const kOTP = @"OTP";
static NSString * const kAN = @"AN";


@interface UserListViewController ()
{
    ATMHud *hud;
    int accN;
}
-(void) authenticate:(id<IUser>) iuser;
-(void) onAuthenticateCompleted:(MpinStatus *) mpinStatus;
-(void) authenticateAN:(id<IUser>) iuser;
-(void) onAuthenticateCompletedAN:(MpinStatus *) mpinStatus;

@end

@implementation UserListViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [GraphicUtill roundBorder:self.table withRadius:4.0 colour:[UIColor colorWithWhite: 0.8 alpha: 1.0]  width:1.0];
    self.title = @"Users";
    self.navigationController.navigationBar.barTintColor = [UIColor colorWithRed:0.59 green:0.6 blue:0.72 alpha:1];
    self.navigationController.navigationBar.titleTextAttributes = [NSDictionary dictionaryWithObject:[UIColor whiteColor] forKey:NSForegroundColorAttributeName];
    
    NSInteger selectedIndex = [[NSUserDefaults standardUserDefaults]
                               integerForKey:kCurrentSelectionIndex];
    
    NSArray *settings = [[NSUserDefaults standardUserDefaults] objectForKey:kSettings];
    if (settings == nil)
    {
        NSDictionary * data = [NSDictionary  dictionaryWithObjectsAndKeys:  @"http://ec2-54-77-232-113.eu-west-1.compute.amazonaws.com", kRPSURL,
                               [NSNumber numberWithBool:NO], kIS_OTP,
                               [NSNumber numberWithBool:NO], kIS_ACCESS_NUMBER,
                               nil];
        settings =[NSMutableArray array];
        [(NSMutableArray *)settings addObject:data];
        selectedIndex = [settings indexOfObject:data];
        [[NSUserDefaults standardUserDefaults] setInteger:selectedIndex forKey:kCurrentSelectionIndex];
        [[NSUserDefaults standardUserDefaults] setObject:settings forKey:kSettings];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    
    
    
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [MPin initWithConfig:[settings objectAtIndex:selectedIndex]];
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            self.users = [MPin listUsers];
            [self.table reloadData];
        });
    });
}

- (void)viewWillAppear:(BOOL)animated {
    [self stopLoading];
    self.users = [MPin listUsers];
    [self.table reloadData];
}

#pragma mark - Table view delegate -
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 45.f;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.users.count;
}

// Row display. Implementers should *always* try to reuse cells by setting each cell's reuseIdentifier and querying for available reusable cells with dequeueReusableCellWithIdentifier:
// Cell gets various attributes set automatically based on table (separators) and data source (accessory views, editing controls)
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *userListTableIdentifier = @"UserListTableCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:userListTableIdentifier];
    if (cell == nil)    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:userListTableIdentifier];
    id<IUser> iuser = [self.users objectAtIndex:indexPath.row];
    cell.textLabel.text = [iuser getIdentity];
    UserState us = [iuser getState];
    if (us == STARTED_REGISTRATION)                 cell.imageView.image = [UIImage imageNamed:@"inactive.png"];
    else if(us == REGISTERED || us == REVOKED)      cell.imageView.image = [UIImage imageNamed:@"active.png"];
    else                                            cell.imageView.image = [UIImage imageNamed:@"problem.png"];
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    id<IUser> iuser = [self.users objectAtIndex:indexPath.row];
    [MPin DeleteUser:iuser];
    [self.users removeObjectAtIndex:indexPath.row];
    [self.table reloadData];
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleDelete;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    id<IUser> iuser = [self.users objectAtIndex:indexPath.row];
    UserState state = [iuser getState];
    if(state == STARTED_REGISTRATION)
    {
        [self startLoading];
        pinpadHeaderTitle = kSetupPin;
        [self performSelectorInBackground:@selector(finishRegistration:) withObject:iuser];
    }
    else if (state == REGISTERED)
    {
        [self startLoading];
        currentUser = iuser;
        pinpadHeaderTitle = kEnterPin;
        NSInteger currentSelectedIndex = [[NSUserDefaults standardUserDefaults] integerForKey:kCurrentSelectionIndex];
        NSArray * settings = [[NSUserDefaults standardUserDefaults] objectForKey:kSettings];
        NSDictionary * config = [settings objectAtIndex:currentSelectedIndex];
        
        Boolean isOTP = [(NSNumber *)[config objectForKey:kIS_OTP] boolValue];
        Boolean isAN  = [(NSNumber *)[config objectForKey:kIS_ACCESS_NUMBER] boolValue];
        
        if(isAN)
        {
            UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main_iPhone" bundle:nil];
            AccessNumberViewController * accessViewController = [storyboard instantiateViewControllerWithIdentifier:@"accessnumber"];
            accessViewController.delegate = self;
            [self.navigationController pushViewController:accessViewController animated:YES];
        }
        else if(isOTP)
        {
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                
                OTP *otp = nil;
                MpinStatus* status = [MPin Authenticate:iuser otp:&otp];
                
                NSDictionary *dictParams = @{kOTP:otp, kMpinStatus:status};
                
                dispatch_async(dispatch_get_main_queue(), ^(void) {
                    
                    MpinStatus * mpinStatus = [dictParams objectForKey:kMpinStatus];
                    if(mpinStatus.status != OK )
                    {
                        [self stopLoading];
                        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[mpinStatus getStatusCodeAsString] message:mpinStatus.errorMessage delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                        [alert show];
                    }
                    else
                    {
                        UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main_iPhone" bundle:nil];
                        OTPViewController * otpViewController = [storyboard instantiateViewControllerWithIdentifier:@"OTP"];
                        otpViewController.otpData = [dictParams objectForKey:kOTP];
                        [self stopLoading];
                        [self.navigationController pushViewController:otpViewController animated:YES];

                    }
                    
                });
            });
        }
        else
        {
            [self performSelectorInBackground:@selector(authenticate:) withObject:iuser];
        }
    }
    else if(state == INVALID)
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Invalid User" message:@"Reactivate an Account!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
        [alert show];
    }
    else
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"INFO" message:@"Unsupported State and Action!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
        [alert show];
    }
    [self.table deselectRowAtIndexPath:indexPath animated:YES];
}

#pragma mark -

-(void) onAccessNumber:(int) an {
    accN = an;
    [self performSelectorInBackground:@selector(authenticateAN:) withObject:currentUser];
}

-(void) authenticateAN:(id<IUser>) iuser {
    MpinStatus* status = [MPin AuthenticateAccessNumber:iuser accessNumber:accN];
    [self performSelectorOnMainThread:@selector(onAuthenticateCompletedAN:) withObject:status waitUntilDone:NO];
}

-(void) onAuthenticateCompletedAN:(MpinStatus *) mpinStatus {
    NSString * descritpion = (mpinStatus.status == OK )?(@"User Authentication is SUCCESSFUL!"):(mpinStatus.errorMessage);
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[mpinStatus getStatusCodeAsString] message:descritpion delegate:self cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
    if ((mpinStatus.status == OK ) && [MPin CanLogout:currentUser]) {
        alert.tag = ON_LOGOUT;
        [alert addButtonWithTitle:@"Logout"];
    }
    [alert show];
    [self stopLoading];
}

-(void) authenticate:(id<IUser>) iuser {
    MpinStatus * s = [MPin Authenticate:iuser];
    [self performSelectorOnMainThread:@selector(onAuthenticateCompleted:) withObject:s waitUntilDone:NO];
}

-(void) onAuthenticateCompleted:(MpinStatus *) mpinStatus {
    NSString * descritpion = (mpinStatus.status == OK )?(@"User Authentication is SUCCESSFUL!"):(mpinStatus.errorMessage);
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[mpinStatus getStatusCodeAsString] message:descritpion delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
    [alert show];
    [self stopLoading];
}

-(void) startLoading {  [super startLoading];   }
-(void) stopLoading  {  [super stopLoading];    }

- (IBAction)editButton:(id)sender {
    UIBarButtonItem * button = (UIBarButtonItem *) sender;
    if([@"Edit" isEqualToString: [button title]]) {
        [self.table setEditing:YES
                      animated:YES];
        [button setTitle:@"Done"];
    } else {
        [self.table setEditing:NO
                      animated:YES];
        [button setTitle:@"Edit"];
    }
}

- (IBAction)addAction:(id)sender
{
    UIViewController * addViewController = [[UIStoryboard storyboardWithName:@"Main_iPhone"
                                                                      bundle:nil]
                                            instantiateViewControllerWithIdentifier:@"Add"];
    
    [self.navigationController pushViewController:addViewController
                                         animated:YES];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    [super alertView:alertView clickedButtonAtIndex:buttonIndex];
    if ((alertView.tag == ON_LOGOUT) && (buttonIndex == LOGOUT_BUTTON_INDEX)) {
        hud = [[ATMHud alloc] initWithDelegate:self];
        [hud setCaption:@"Logging OUT ..."];
        [hud setActivity:YES];
        [hud showInView:self.view];
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            Boolean isSuccessful = [MPin Logout:currentUser];
            dispatch_async(dispatch_get_main_queue(), ^(void) {
                [hud hide];
                NSString * descritpion = (isSuccessful)?(@"Successful"):(@"Unsuccessful");
                UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Logout" message:descritpion delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                [alert show];
            });
        });

    }

}


@end
