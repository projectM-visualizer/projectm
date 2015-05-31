//
//  AppDelegate.h
//  projectM Visualizer
//
//  Created by Snooz on 8/16/14.
//  Copyright (c) 2014 projectM. All rights reserved.
//

#import "Preferences.h"
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (retain) IBOutlet Preferences *prefs;

@end
