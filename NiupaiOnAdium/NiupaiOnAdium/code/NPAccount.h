//
//  NPAccount.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import <AdiumLibpurple/CBPurpleAccount.h>

#define KEY_QQ_TCP_CONNECT @"use_tcp"
#define KEY_QQ_CONNECT_HOST @"server"
#define DEFAULT_PORT @"8000"
#define NP_NETWORK_ERROR @"NPNetworkError"


@interface NPAccount : CBPurpleAccount

@end
