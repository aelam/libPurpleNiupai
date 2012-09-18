//
//  NPService.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import "NPService.h"
#import "NPAccount.h"
#import <Adium/AIStatusControllerProtocol.h>
#import <Adium/AISharedAdium.h>
#import <AIUtilities/AIImageAdditions.h>
#import <AIUtilities/AIStringUtilities.h>
#import <Adium/AIAccountViewController.h>


@implementation NPService

- (void) registerStatuses {
    
    [[adium statusController] registerStatus:STATUS_NAME_AVAILABLE
                             withDescription:[[adium statusController] localizedDescriptionForCoreStatusName:STATUS_NAME_AVAILABLE]
                                      ofType:AIAvailableStatusType
                                  forService:self];
}

- (Class) accountClass {
    return [NPAccount class];
}

//- (AIAccountViewController*) accountViewController {
//    return [NPAccountViewController accountViewController];
//}

- (DCJoinChatViewController *)joinChatView{
	return nil;
}


- (NSString*) serviceCodeUniqueID {
    return @NP_ADIUM_PLUGIN_ID;
}

- (NSString*) serviceID {
    return @NP_PLUGIN_NAME;
}

- (NSString*) serviceClass {
    return @NP_PLUGIN_NAME;
}

- (NSString*) shortDescription {
    return @"Niupai";
}

- (NSString*) longDescription {
    return @"牛排.Niupai";
}

- (NSString *)UIDPlaceholder
{
	return AILocalizedString(@"steve@me.com", "Niupai accounts");
}

- (NSCharacterSet*) allowedCharactersForUIDs {
	return [NSCharacterSet characterSetWithCharactersInString:@"+abcdefghijklmnopqrstuvwxyz0123456789@._-"];
}


- (NSString*) userNameLabel {
    return @"User Name";
}

- (NSString*) contactUserNameLabel {
    return @"Contact UID";
}

- (NSUInteger)allowedLength{
	return 113;
}

- (NSUInteger) allowedLengthForUIDs {
    return 20;
}

- (BOOL) caseSensitive {
    return YES;
}

- (BOOL) requiresPassword {
    return NO;
}

- (BOOL) canCreateGroupChats {
    return NO;
}

- (AIServiceImportance) serviceImportance {
    return AIServiceSecondary;
}

- (NSImage *)defaultServiceIconOfType:(AIServiceIconType)iconType
{
	if ((iconType == AIServiceIconSmall) || (iconType == AIServiceIconList)) {
		return [NSImage imageNamed:@"Niupai-small" forClass:[self class] loadLazily:YES];
	} else {
		return [NSImage imageNamed:@"Niupai" forClass:[self class] loadLazily:YES];
	}
}

@end
