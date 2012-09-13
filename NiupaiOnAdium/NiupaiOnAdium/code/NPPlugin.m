//
//  NPPlugin.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import "NPPlugin.h"
#import "NPService.h"
#import <glib.h>
#import <Adium/AIListObject.h>
#import <Adium/AIService.h>

extern gboolean purple_init_np_plugin(void);

@implementation NPPlugin

- (void)installPlugin {

    [NPService registerService];
    purple_init_np_plugin();
}

- (void) uninstallPlugin {
}

- (void) installLibpurplePlugin {
}

- (void) loadLibpurplePlugin {
}

- (NSString*) pluginAuthor {
    return @"Ryan Wang";
}

- (NSString*) pluginVersion {
    return @"0.1";
}

- (NSString*) pluginDescription {
    return @"Allows Adium to connect to Niupai accounts";
}

- (NSString*) pluginWebsite {
    return @"www.niupai.com";
}

- (void)dealloc{

    [super dealloc];
}


@end
