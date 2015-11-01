//
//  AudioInputDeviceList.m
//  projectM Visualizer
//
//  Created by Mischa on 5/30/15.
//  Copyright (c) 2015 projectM. All rights reserved.
//

#import "AudioInputDeviceList.h"

@implementation AudioInputDeviceList

- (id)init {
    [self setDevices:[[NSMutableArray alloc] init]];
    [self refreshDeviceList];
    
    return self;
}

- (void)refreshDeviceList {
    [_devices removeAllObjects];
    
    NSArray *devs = [AVCaptureDevice devicesWithMediaType:AVMediaTypeAudio];
    AVCaptureDeviceInputSource *src;
    for (int i = 0; i < [devs count]; i++) {
        src = [devs objectAtIndex:i];
        [_devices addObject:src];
    }
}

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox {
    if (_devices == nil)
        return 0;
    
    return [_devices count];
}

//- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index {
//    AVCaptureDeviceInputSource *src = [_devices objectAtIndex:index];
//    return [NSString stringWithFormat:@"%@" [src localizedName]];
//}

@end
