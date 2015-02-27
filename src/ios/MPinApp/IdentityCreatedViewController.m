//
//  IdentityCreatedViewController.m
//  MPinApp
//
//  Created by Tihomir Ganev on 27.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import "IdentityCreatedViewController.h"

@interface IdentityCreatedViewController ()

-(IBAction)gotoIDList:(id)sender;
@end

@implementation IdentityCreatedViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    _lblEmail.text = _strEmail;
    _lblMessage.text = [NSString stringWithFormat:@"Congratulations!\r\n Your M-Pin identity: %@ had been set up successfully.",_strEmail];
}
/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

-(IBAction)gotoIDList:(id)sender
{
    [self.navigationController popToRootViewControllerAnimated:YES];
}

@end
