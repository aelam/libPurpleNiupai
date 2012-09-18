//
//  NPAccount.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import <Adium/AIStatus.h>

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

- (void)configurePurpleAccount
{
	[super configurePurpleAccount];
	
	/* We could add a pref for this, but not without some enhancements to mail notifications. Currently, this being
	 * enabled means ugly nasty "You have new mail!" popups continuously, since that's how 'notifications' are passed
	 * to us.
	 */
//	purple_account_set_bool(account, "facebook_get_notifications", FALSE);
//	
//	// We do our own history; don't let the server's history get displayed as new messages
//	purple_account_set_bool(account, "facebook_show_history", FALSE);
//	
//	// Use friends list as groups.
//	purple_account_set_bool(account, "facebook_use_groups", TRUE);
//	
//	// Allow for moving through libpurple
//	purple_account_set_bool(account, "facebook_manage_friends", TRUE);
//	
//	// Disable the Facebook CAPTCHA since it causes heartache and pain.
//	purple_account_set_bool(account, "ignore-facebook-captcha", TRUE);
}

- (const char *)purpleStatusIDForStatus:(AIStatus *)statusState arguments:(NSMutableDictionary *)arguments
{
	if (statusState.statusType == AIOfflineStatusType) {
		return "offline";
	} else {
		return "available";
	}
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
