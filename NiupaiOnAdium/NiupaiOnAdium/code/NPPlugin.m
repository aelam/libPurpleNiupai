//
//  NPPlugin.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import "NPPlugin.h"
#import "NPService.h"

#import <Adium/AIListObject.h>
#import <Adium/AIService.h>
#import <Adium/ESDebugAILog.h>					//Everybody needs to debug
#import <glib.h>

extern gboolean purple_init_np_plugin(void);

@implementation NPPlugin

- (void)installPlugin {

    [NPService registerService];
    purple_init_np_plugin();
    
    NSString *logPath = @"/Users/ryan/Desktop/libpurple-np.log";
    [[NSFileManager defaultManager] removeItemAtPath:logPath error:nil];
    freopen([logPath cStringUsingEncoding:NSASCIIStringEncoding], "a+", stderr);
    
	NSLog(@"Security details: %@",self);
}

- (void) uninstallPlugin {
}

- (void) installLibpurplePlugin {
}

- (void) loadLibpurplePlugin {
}

- (NSString*) pluginAuthor {
    return @NP_PLUGIN_AUTHOR;
}

- (NSString*) pluginVersion {
    return @LIBNP_VERSION;
}

- (NSString*) pluginDescription {
    return @NP_PLUGIN_DESCRIPTION;
}

- (NSString*) pluginWebsite {
    return @NP_PLUGIN_HOMEPAGE;
}

- (void)dealloc{

    [super dealloc];
}


@end
