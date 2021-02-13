//
//  AudioInputDeviceList.h
//  projectM Visualizer
//
//  Created by Mischa on 5/30/15.
//  Copyright (c) 2015 projectM. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface AudioInputDeviceList : NSObject <NSComboBoxDataSource>

@property (retain) NSMutableArray *devices;

- (id)init;

- (void)refreshDeviceList;

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox;

- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index;

@end
