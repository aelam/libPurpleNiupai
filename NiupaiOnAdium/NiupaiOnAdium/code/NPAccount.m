//
//  NPAccount.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import "NPAccount.h"
#include "niupai-config.h"

@implementation NPAccount

- (void) initAccount {
    [super initAccount];

    NSLog(@"%@",self.UID);

}

- (const char*) protocolPlugin {
    return NP_PLUGIN_ID;
}

- (BOOL) canSendFolders {
    return NO;
}

- (void) accountConnectionReportDisconnect:(NSString *)text withReason:(PurpleConnectionError)reason {
    if (reason == PURPLE_CONNECTION_ERROR_NETWORK_ERROR) {
        [[NSNotificationCenter defaultCenter] postNotificationName:NP_NETWORK_ERROR object:nil];
    }
    
    [super accountConnectionReportDisconnect:text withReason:reason];
}



@end
