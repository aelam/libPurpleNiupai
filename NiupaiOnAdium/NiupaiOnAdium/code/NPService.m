//
//  NPService.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#import "NPService.h"
#import "NPAccount.h"
#import "NPAccountViewController.h"
#import <Adium/AIStatusControllerProtocol.h>
#import <Adium/AISharedAdium.h>
#import <AIUtilities/AIImageAdditions.h>

@implementation NPService

- (void) registerStatuses {
    [[adium statusController] registerStatus:STATUS_NAME_AVAILABLE
                             withDescription:[[adium statusController] localizedDescriptionForCoreStatusName:STATUS_NAME_AVAILABLE]
                                      ofType:AIAvailableStatusType
                                  forService:self];
    [[adium statusController] registerStatus:STATUS_NAME_INVISIBLE
                             withDescription:[[adium statusController]
                                              localizedDescriptionForCoreStatusName:STATUS_NAME_INVISIBLE]
                                      ofType:AIInvisibleStatusType
                                  forService:self];
    [[adium statusController] registerStatus:STATUS_NAME_BUSY
							 withDescription:[[adium statusController] localizedDescriptionForCoreStatusName:STATUS_NAME_BUSY]
                                      ofType:AIAwayStatusType
                                  forService:self];
}

- (Class) accountClass {
    return [NPAccount class];
}

- (AIAccountViewController*) accountViewController {
    return [NPAccountViewController accountViewController];
}

- (DCJoinChatViewController *)joinChatView{
	return nil;
}


- (NSString*) serviceCodeUniqueID {
    return @"libpurple-niupai";
}

- (NSString*) serviceID {
    return @"Niupai";
}

- (NSString*) serviceClass {
    return @"Niupai";
}

- (NSString*) shortDescription {
    return @"Niupai";
}

- (NSString*) longDescription {
    return @"牛排.Niupai";
}

- (NSString*) userNameLabel {
    return @"User Name";
}

- (NSString*) contactUserNameLabel {
    return @"Contact UID";
}

- (NSCharacterSet *)allowedCharacters{
	return [NSCharacterSet characterSetWithCharactersInString:@"+abcdefghijklmnopqrstuvwxyz0123456789@._-"];
}

- (NSUInteger)allowedLength{
	return 113;
}

- (NSCharacterSet*) allowedCharactersForUIDs {
    return [NSCharacterSet decimalDigitCharacterSet];
}

- (NSUInteger) allowedLengthForUIDs {
    return 20;
}

- (BOOL) caseSensitive {
    return YES;
}

- (BOOL) requiresPassword {
    return YES;
}

- (BOOL) canCreateGroupChats {
    return YES;
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
