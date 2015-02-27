//
//  AboutViewController.m
//  MPinApp
//
//  Created by Tihomir Ganev on 6.февр..15.
//  Copyright (c) 2015 г. Certivox. All rights reserved.
//

#import "AboutViewController.h"


@interface AboutViewController ()
{
    
}
@property (nonatomic, weak) IBOutlet UILabel *lblBuildNumber;
@property (nonatomic, weak) IBOutlet UILabel *lblAppVersion;

@end

@implementation AboutViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    
    self.title = @"About";
    _lblBuildNumber.font = [UIFont fontWithName:@"OpenSans-Bold" size:12.f];
    _lblAppVersion.font = [UIFont fontWithName:@"OpenSans-Bold" size:12.f];

    _lblBuildNumber.text = [NSString stringWithFormat:@"Build: %@",strBuildNumber];
    _lblAppVersion.text  = [NSString stringWithFormat:@"Ver. %@",strAppVersion];

}

- (void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [themeManager beautifyViewController:self];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
