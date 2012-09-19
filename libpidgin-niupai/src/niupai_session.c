//
//  niupai_session.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "niupai_session.h"

NPSession *np_session_new(PurpleAccount *account)
{
    NPSession *session;
    
    g_return_val_if_fail(account != NULL, NULL);
    
    session = g_new0(NPSession, 1);
    
    session->account = account;
    
    return session;
}
