//
//  UserListViewController.m
//  MPinApp
//
//  Created by Georgi Georgiev on 11/19/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "UserListViewController.h"
#import "OTPViewController.h"
#import "IUser.h"
#import "Constants.h"
#import "ATMHud.h"
#import "SettingsManager.h"
#import "UIViewController+Helper.h"
#import "AccountSummaryViewController.h"
#import "SettingsManager.h"


#define ON_LOGOUT 403
#define LOGOUT_BUTTON_INDEX 1
#define NOT_SELECTED    -1
#define DELETE_TAG  204
#define DELETE_BUTTON_INDEX  1


static NSString * const kMpinStatus = @"MpinStatus";
static NSString * const kOTP = @"OTP";
static NSString * const kAN = @"AN";


@implementation UserListTableViewCell
@end

@interface UserListViewController ()
{
    NSIndexPath * selectedIndexPath;
    ATMHud *hud;
    int accN;
    BOOL boolIsInitialised;
}
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *constraintLeadingSpace;
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *constraintTrailingSpace;
-(void) authenticate:(id<IUser>) iuser;
-(void) authenticateOTP:(id<IUser>) iuser;
-(void) authenticateAN:(id<IUser>) iuser;

- (IBAction)btnAddIDTap:(id)sender;
- (IBAction)btnEditTap:(id)sender;
- (IBAction)btnDeleteTap:(id)sender;
- (IBAction)btnAuthTap:(id)sender;

- (void) deleteSelectedUser;

@property(nonatomic, retain, readwrite) IBOutlet UITableView        *table;
@property(nonatomic, retain, readwrite) IBOutlet UIButton           *btnAdd;
@property(nonatomic, retain, readwrite) IBOutlet UIButton           *btnDelete;
@property(nonatomic, retain, readwrite) IBOutlet UIButton           *btnEdit;
@property(nonatomic, retain, readwrite) IBOutlet UIButton           *btnAuthenticate;


@end

@implementation UserListViewController

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        selectedIndexPath = [NSIndexPath indexPathForRow:NOT_SELECTED inSection:NOT_SELECTED];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.title = @"Identity List";
    
    boolIsInitialised = NO;
    
    NSInteger selectedIndex = [[NSUserDefaults standardUserDefaults]
                               integerForKey:kCurrentSelectionIndex];
    
    NSArray *settings = [[NSUserDefaults standardUserDefaults] objectForKey:kSettings];
    if (settings == nil)
    {
       NSDictionary * data = [NSDictionary  dictionaryWithObjectsAndKeys:  @"http://tcb.certivox.org", kRPSURL,
                               [NSNumber numberWithBool:NO], kIS_OTP,
                               [NSNumber numberWithBool:NO], kIS_ACCESS_NUMBER,
                                @"Mobile banking login",kCONFIG_NAME,
                               nil];
        NSDictionary * dataOTP = [NSDictionary  dictionaryWithObjectsAndKeys:  @"http://otp.m-pin.id", kRPSURL,
                                  [NSNumber numberWithBool:YES], kIS_OTP,
                                  [NSNumber numberWithBool:NO], kIS_ACCESS_NUMBER,
                                  @"VPN login",kCONFIG_NAME,
                                  nil];
        NSDictionary * dataAN = [NSDictionary  dictionaryWithObjectsAndKeys:  @"http://tcb.certivox.org", kRPSURL,
                                 [NSNumber numberWithBool:NO], kIS_OTP,
                                 [NSNumber numberWithBool:YES], kIS_ACCESS_NUMBER,
                                 @"Online banking login",kCONFIG_NAME,
                                 nil];
        
        
        settings =[NSMutableArray array];
        [(NSMutableArray *)settings addObject:data];
        [(NSMutableArray *)settings addObject:dataOTP];
        [(NSMutableArray *)settings addObject:dataAN];
        
        selectedIndex = [settings indexOfObject:data];
        
        [[NSUserDefaults standardUserDefaults] setInteger:selectedIndex forKey:kCurrentSelectionIndex];
        [[NSUserDefaults standardUserDefaults] setObject:settings forKey:kSettings];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    
    
    
    hud = [[ATMHud alloc] initWithDelegate:self];
    [hud setCaption:@"Changing configuration. Please wait."];
    [hud setActivity:YES];
    [hud showInView:self.view];
    
    double delayInSeconds = 1.0;
    dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC)); // 1
    dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
        
        
    });
    
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSLog(@"########################################");
        MpinStatus* status = [MPin initWithConfig:[settings objectAtIndex:selectedIndex]];
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            if (status.status != OK) {
                [self showError:[status getStatusCodeAsString] desc:status.errorMessage];
            }
            [hud hide];
            self.users = [MPin listUsers];
            [self.table reloadData];
        });
    });
    _constraintLeadingSpace.constant = self.navigationController.navigationBar.frame.size.width;
    _constraintTrailingSpace.constant = -self.navigationController.navigationBar.frame.size.width;
    
    _btnDelete.backgroundColor = [[SettingsManager sharedManager] color1];
    [_btnDelete setTitleColor:[[SettingsManager sharedManager] color8] forState:UIControlStateNormal];
    [_btnDelete  setTitle:@"DELETE" forState:UIControlStateNormal];
    
    _btnEdit.backgroundColor = [[SettingsManager sharedManager] color1];
    [_btnEdit setTitleColor:[[SettingsManager sharedManager] color6] forState:UIControlStateNormal];
    [_btnEdit  setTitle:@"EDIT" forState:UIControlStateNormal];
    
    _btnAuthenticate.backgroundColor = [[SettingsManager sharedManager] color6];
    [_btnAuthenticate setTitleColor:[[SettingsManager sharedManager] color1] forState:UIControlStateNormal];
    
    
    _btnAdd.backgroundColor = [[SettingsManager sharedManager] color6];
    [_btnAdd  setTitle:@"ADD NEW IDENTITY +" forState:UIControlStateNormal];
    _btnAdd.titleLabel.font = [UIFont fontWithName:@"OpenSans" size:16.0];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self.menuContainerViewController setPanMode:MFSideMenuPanModeDefault];
    [themeManager beautifyViewController:self];
    [self stopLoading];
    self.users = [MPin listUsers];
    if ([self.users count] == 0)
    {
        _constraintLeadingSpace.constant = self.navigationController.navigationBar.frame.size.width;
        _constraintTrailingSpace.constant = -self.navigationController.navigationBar.frame.size.width;
        [self.view layoutIfNeeded];

    }
    [self.table reloadData];
    
}

#pragma mark - Table view delegate -

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section    {   return self.users.count;    }


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *userListTableIdentifier = @"UserListTableViewCell";
    UserListTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:userListTableIdentifier];
    if (cell == nil)    cell = [[UserListTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:userListTableIdentifier];
    cell.lblUserID.font = [UIFont fontWithName:@"OpenSans-Regular" size:14.f];
    cell.lblUserID.textColor = [[SettingsManager sharedManager] color6];
    return cell;
}

-(void) tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
    UserListTableViewCell *c = (UserListTableViewCell *)cell;
    id<IUser> iuser = [self.users objectAtIndex:indexPath.row];
    c.lblUserID.text = [iuser getIdentity];

    
    if ((selectedIndexPath != nil) && (indexPath.row == selectedIndexPath.row))
    {
        c.imgViewSelected.image = [UIImage imageNamed:@"checked"];
    }
    else
    {
        c.imgViewSelected.image = [UIImage imageNamed:@"pin-dot-empty"];
    }

    switch ([iuser getState]) {
        case INVALID:
            c.imgViewUser.image = [UIImage imageNamed:@"avatar-list-unregistered"];
            break;
        case STARTED_REGISTRATION:
            c.imgViewUser.image = [UIImage imageNamed:@"avatar-list-unregistered"];
            break;
        case ACTIVATED:
            c.imgViewUser.image = [UIImage imageNamed:@"avatar-list-unregistered"];
            break;
        case REGISTERED:
             c.imgViewUser.image = [UIImage imageNamed:@"avatar-list-registered"];
        break;
        default:
             c.imgViewUser.image = [UIImage imageNamed:@"avatar-list-unregistered"];
        break;
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (selectedIndexPath.row == indexPath.row) {
        
        UserListTableViewCell * prevCell = (UserListTableViewCell *)[tableView cellForRowAtIndexPath:selectedIndexPath];
        prevCell.imgViewSelected.image = [UIImage imageNamed:@"pin-dot-empty"];;
        selectedIndexPath = [NSIndexPath indexPathForRow:NOT_SELECTED inSection:NOT_SELECTED];
        _constraintLeadingSpace.constant = self.navigationController.navigationBar.frame.size.width;
        _constraintTrailingSpace.constant = -self.navigationController.navigationBar.frame.size.width;
        [UIView animateWithDuration:.2
                         animations:^{
                             [self.view layoutIfNeeded];
                         }];
        return;
    }

    if (selectedIndexPath.row == NOT_SELECTED) {
        _constraintLeadingSpace.constant = -20;
        _constraintTrailingSpace.constant = -20;
        [UIView animateWithDuration:.2  animations: ^{   [self.view layoutIfNeeded]; }];
    } else  {
        UserListTableViewCell * prevCell = (UserListTableViewCell *)[tableView cellForRowAtIndexPath:selectedIndexPath];
        prevCell.imgViewSelected.image = [UIImage imageNamed:@"pin-dot-empty"];
    }
    currentUser = [self.users objectAtIndex:indexPath.row];
    selectedIndexPath = indexPath;
    UserListTableViewCell *cell = (UserListTableViewCell *)[tableView cellForRowAtIndexPath:indexPath];
    cell.imgViewSelected.image = [UIImage imageNamed:@"checked"];
}

-(void) authenticateOTP:(id<IUser>) iuser {
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        OTP *otp = nil;
        MpinStatus* mpinStatus = [MPin Authenticate:iuser otp:&otp];
        
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [self stopLoading];
            switch (mpinStatus.status) {
                case OK: {
                    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main_iPhone" bundle:nil];
                    OTPViewController * otpViewController = [storyboard instantiateViewControllerWithIdentifier:@"OTP"];
                    otpViewController.otpData = otp;
                    otpViewController.strEmail = [currentUser    getIdentity];
                    [self.navigationController pushViewController:otpViewController animated:YES];
                }
                    break;
                case INCORRECT_PIN: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Authentication Failed!"
                                                                    message:@"Wrong MPIN"
                                                                   delegate:nil
                                                          cancelButtonTitle:@"Close"
                                                          otherButtonTitles:nil,
                                          nil];
                    [alert show];
                } break;
                default: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[mpinStatus getStatusCodeAsString]
                                                                    message:mpinStatus.errorMessage
                                                                   delegate:nil
                                                          cancelButtonTitle:@"Close"
                                                          otherButtonTitles:nil,
                                          nil];
                    [alert show];
                } break;
            }
        });
    });
}

-(void) onAccessNumber:(int) an {
    accN = an;
    [self authenticateAN:[self.users objectAtIndex:selectedIndexPath.row]];
}

-(void) authenticateAN:(id<IUser>) iuser {
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        MpinStatus* status = [MPin AuthenticateAccessNumber:iuser accessNumber:accN];
        
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [self stopLoading];
            NSString * descritpion = (status.status == OK )?(@"User Authentication is SUCCESSFUL!"):(status.errorMessage);
            switch (status.status) {
                case OK: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[status getStatusCodeAsString] message:descritpion delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                    [alert setTitle:nil];
                    [alert setMessage:@"Authentication Successful!"];
                    [alert show];
                } break;
                case INCORRECT_PIN: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Authentication Failed!"
                                                                    message:@"Wrong MPIN or Access Number!"
                                                                   delegate:nil
                                                          cancelButtonTitle:@"Close"
                                                          otherButtonTitles:nil,
                                          nil];
                    [alert show];
                } break;
                case HTTP_REQUEST_ERROR: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Authentication Failed!"
                                                                    message:@"Wrong MPIN or Access Number!"
                                                                   delegate:nil cancelButtonTitle:@"Close"
                                                          otherButtonTitles:nil,
                                          nil];
                    [alert show];
                } break;
                default: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[status getStatusCodeAsString] message:descritpion delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                    [alert show];
                } break;
            }
        });
    });
}

-(void) authenticate:(id<IUser>) iuser {
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        MpinStatus * mpinStatus = [MPin Authenticate:iuser];
       
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [self stopLoading];
            NSString * descritpion = (mpinStatus.status == OK )?(@"User Authentication is SUCCESSFUL!"):(mpinStatus.errorMessage);
            switch (mpinStatus.status) {
                case OK: {
                    UIStoryboard* storyboard = [UIStoryboard storyboardWithName:@"Main_iPhone" bundle:nil];
                    AccountSummaryViewController *vcAccountSummary = [storyboard instantiateViewControllerWithIdentifier:@"AccountSummary"];
                    vcAccountSummary.strEmail = [currentUser getIdentity];
                    [self.navigationController pushViewController:vcAccountSummary animated:YES];
                } break;
                case INCORRECT_PIN: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Authentication Failed!" message:@"Wrong MPIN" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                    [alert show];
                } break;
                default: {
                    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[mpinStatus getStatusCodeAsString] message:descritpion delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                    [alert show];
                } break;
            }
        });
    });
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    [super alertView:alertView clickedButtonAtIndex:buttonIndex];
    if ((alertView.tag == ON_LOGOUT) && (buttonIndex == LOGOUT_BUTTON_INDEX)) {
        hud = [[ATMHud alloc] initWithDelegate:self];
        [hud setCaption:@"Logging OUT ..."];
        [hud setActivity:YES];
        [hud showInView:self.view];
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            BOOL isSuccessful = [MPin Logout:[self.users objectAtIndex:selectedIndexPath.row]];
            dispatch_async(dispatch_get_main_queue(), ^(void) {
                [hud hide];
                NSString * descritpion = (isSuccessful)?(@"Successful"):(@"Unsuccessful");
                UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Logout" message:descritpion delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil];
                [alert show];
            });
        });
        return;
    }
    
    if ((alertView.tag == DELETE_TAG) && (buttonIndex == DELETE_BUTTON_INDEX)) {
        [self deleteSelectedUser];
    }
}


- (void) deleteSelectedUser {
    _constraintLeadingSpace.constant = self.navigationController.navigationBar.frame.size.width;
    _constraintTrailingSpace.constant = -self.navigationController.navigationBar.frame.size.width;
    [UIView animateWithDuration:.8  animations:^{   [self.view layoutIfNeeded]; }];
    id<IUser> iuser = [self.users objectAtIndex:selectedIndexPath.row];
    [MPin DeleteUser:iuser];
    [self.users removeObjectAtIndex:selectedIndexPath.row];
    selectedIndexPath = [NSIndexPath indexPathForRow:NOT_SELECTED inSection:NOT_SELECTED];
    [self.table reloadData];
}


#pragma mark - My actions -

- (IBAction)btnAddIDTap:(id)sender
{
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main_iPhone" bundle:nil];
    UIViewController * addViewController = [storyboard instantiateViewControllerWithIdentifier:@"Add"];
    [self.navigationController pushViewController:addViewController animated:YES];

}

- (IBAction)btnEditTap:(id)sender
{
    
}
- (IBAction)btnDeleteTap:(id)sender     {
    id<IUser> iuser = [self.users objectAtIndex:selectedIndexPath.row];
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"DELETE"
                                                    message:[NSString stringWithFormat:@"User \"%@\" will be deleted!", [iuser getIdentity]]
                                                   delegate:self
                                          cancelButtonTitle:@"CANCEL"
                                          otherButtonTitles:@"DELETE",
                          nil];
    alert.tag = DELETE_TAG;
    [alert show];
}
- (IBAction)btnAuthTap:(id)sender
{
    id<IUser> iuser = [self.users objectAtIndex:selectedIndexPath.row];
    switch ([iuser getState]) {
        case INVALID:
            [[[UIAlertView alloc] initWithTitle:@"Invalid User" message:@"Reactivate an Account!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil] show];
        break;
        case STARTED_REGISTRATION:
            [self startLoading];
            pinpadHeaderTitle = kSetupPin;
            [self finishRegistration:iuser];
        break;
        case ACTIVATED:
            [self startLoading];
            pinpadHeaderTitle = kSetupPin;
            [self finishRegistration:iuser];
            break;
        case REGISTERED: {
            [self startLoading];
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
                accessViewController.strEmail = [currentUser getIdentity];
                [self.navigationController pushViewController:accessViewController animated:YES];
            }
            else if ( isOTP )
            {
                [self authenticateOTP:iuser];
            }
            else
            {
                [self authenticate:iuser];
            }
        }
        break;
        default:
            [[[UIAlertView alloc] initWithTitle:@"INFO" message:@"Unsupported State and Action!" delegate:nil cancelButtonTitle:@"Close" otherButtonTitles:nil, nil] show];
        break;
            
    }
}


@end
