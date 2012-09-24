//
//  use_list.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "userlist.h"
#include "NIDebuggingTools.h"

/**************************************************************************
 * UserList functions
 **************************************************************************/

NPUserList
*np_userlist_new(NPSession *session)
{
    purple_debug_warning("np","=============>");
    NPUserList *userlist;
    userlist = g_new0(NPUserList, 1);

    userlist->session = session;
	userlist->buddy_icon_requests = g_queue_new();

    /* buddy_icon_window is the number of allowed simultaneous buddy icon requests.
	 * XXX With smarter rate limiting code, we could allow more at once... 5 was the limit set when
	 * we weren't retrieiving any more than 5 per NP session. */
	userlist->buddy_icon_window = 1;
    purple_debug_warning("np","=============>");

    return userlist;
}


void
np_userlist_destroy(NPUserList *userlist)
{
    //TODO
    GList *l;

	/*destroy userlist*/
	for (l = userlist->users; l != NULL; l = l->next)
	{
		np_user_unref(l->data);
	}
	g_list_free(userlist->users);

    g_queue_free(userlist->buddy_icon_requests);

	if (userlist->buddy_icon_request_timer)
		purple_timeout_remove(userlist->buddy_icon_request_timer);

	g_free(userlist);

}

void
np_userlist_add_user(NPUserList *userlist, NPUser *user)
{
    np_user_ref(user);
	userlist->users = g_list_prepend(userlist->users, user);
}

void
np_userlist_remove_user(NPUserList *userlist, NPUser *user)
{
    userlist->users = g_list_remove(userlist->users, user);
    np_user_unref(user);
}

