//
//  AppDelegate.m
//  projectM Visualizer
//
//  Created by Mischa Spiegelmock on 8/16/14.
//  Copyright (c) 2014 projectM. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    //[self installPlugin:nil];
//    _prefs = [[Preferences alloc] initWithWindowNibName:@"Preferences" owner:self];
//    [_prefs showWindow:self];
}

- (IBAction)installPlugin:(id)sender {
    NSBundle* me = [NSBundle mainBundle];
    NSString* installerPkg = [me pathForResource:@"iProjectM" ofType:@"pkg"];
    NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
    [workspace openFile:installerPkg];
}

@end
