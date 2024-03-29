//
//  AppDelegate.m
//  MPinApp
//
//  Created by Georgi Georgiev on 11/17/14.
//  Copyright (c) 2014 Certivox. All rights reserved.
//

#import "AppDelegate.h"
#import "MPin.h"
#import "Constants.h"
#import "MFSideMenuContainerViewController.h"
#import "SettingsManager.h"

@interface AppDelegate ()
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication*)application
    didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    [[UIApplication sharedApplication]
        setStatusBarStyle:UIStatusBarStyleDefault];

    UIStoryboard* storyboard =
        [UIStoryboard storyboardWithName:@"Main_iPhone"
                                  bundle:[NSBundle mainBundle]];

    MFSideMenuContainerViewController* container = (MFSideMenuContainerViewController*)self.window.rootViewController;
    _vcUserList = [storyboard
        instantiateViewControllerWithIdentifier:@"UserListViewController"];

    UIViewController* leftSideMenuViewController = [storyboard instantiateViewControllerWithIdentifier:@"MenuViewController"];

    [container setLeftMenuViewController:leftSideMenuViewController];
    [container setCenterViewController:[[UINavigationController alloc] initWithRootViewController:_vcUserList]];

    
    self.window.rootViewController = container;

    return YES;
}

/// http://192.168.98.109:8005
/// https://mpindemo-qa-v3.certivox.org
//// @"http://ec2-54-77-232-113.eu-west-1.compute.amazonaws.com"
/// @"http://risso.certivox.org"

- (void)applicationWillResignActive:(UIApplication*)application
{
    // Sent when the application is about to move from active to inactive state.
    // This can occur for certain types of temporary interruptions (such as an
    // incoming phone call or SMS message) or when the user quits the application
    // and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down
    // OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication*)application
{
    // Use this method to release shared resources, save user data, invalidate
    // timers, and store enough application state information to restore your
    // application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called
    // instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication*)application
{
    // Called as part of the transition from the background to the inactive state;
    // here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication*)application
{
    // Restart any tasks that were paused (or not yet started) while the
    // application was inactive. If the application was previously in the
    // background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication*)application
{
    // Called when the application is about to terminate. Save data if
    // appropriate. See also applicationDidEnterBackground:.
}

@end
