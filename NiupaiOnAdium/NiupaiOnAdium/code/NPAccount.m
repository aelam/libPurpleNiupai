//
//  NPAccount.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import "NPAccount.h"

@implementation NPAccount

- (void) initAccount {
    [super initAccount];
}

- (const char*) protocolPlugin {
    return "prpl-np";
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


/*!
 * @brief The UID will be changed. The account has a chance to perform modifications
 *
 * For example, MSN adds @hotmail.com to the proposedUID and returns the new value
 *
 * @param proposedUID The proposed, pre-filtered UID (filtered means it has no characters invalid for this servce)
 * @result The UID to use; the default implementation just returns proposedUID.
 */
- (NSString *)accountWillSetUID:(NSString *)proposedUID
{
	NSString	*correctUID;
	
//	if (([proposedUID length] > 0) &&
//        ([proposedUID rangeOfString:@"@"].location == NSNotFound)) {
//		correctUID = [proposedUID stringByAppendingString:DEFAULT_MSN_PASSPORT_DOMAIN];
//	} else {
		correctUID = proposedUID;
//	}
	
	return correctUID;
}


@end
