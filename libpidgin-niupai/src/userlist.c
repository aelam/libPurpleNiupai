//
//  use_list.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

//#include "internal.h"
#include "userlist.h"
//#include "niupai.h"
#include "NIDebuggingTools.h"

/**************************************************************************
 * UserList functions
 **************************************************************************/

NPUserList
*np_userlist_new(NPSession *session)
{
    NIDPRINT("=============>");
    NPUserList *userlist;
    userlist = g_new0(NPUserList, 1);

    userlist->session = session;
	userlist->buddy_icon_requests = g_queue_new();

    /* buddy_icon_window is the number of allowed simultaneous buddy icon requests.
	 * XXX With smarter rate limiting code, we could allow more at once... 5 was the limit set when
	 * we weren't retrieiving any more than 5 per MSN session. */
	userlist->buddy_icon_window = 1;

    return NULL;
}


void
np_userlist_destroy(NPSession *userlist)
{
    
}
