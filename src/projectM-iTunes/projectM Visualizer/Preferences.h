//
//  Preferences.h
//  projectM Visualizer
//
//  Created by Mischa on 5/30/15.
//  Copyright (c) 2015 projectM. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "AudioInputDeviceList.h"

@interface Preferences : NSWindowController

@property IBOutlet AudioInputDeviceList *deviceList;

@end
