//
//  user.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "internal.h"
#include "user.h"

NPUser
*np_user_new(NPUserList *userlist, const char *passport,
                    const char *friendly_name)
{
    NPUser *user;
    user = g_new0(NPUser, 1);
    
    user->userlist = userlist;
	np_user_set_passport(user, passport);
	np_user_set_friendly_name(user, friendly_name);
    
	return np_user_ref(user);
}

#pragma mark -
#pragma mark TODO 
/*destroy a user object*/
static void
np_user_destroy(NPUser *user)
{
    //	while (user->endpoints != NULL) {
//		free_user_endpoint(user->endpoints->data);
//		user->endpoints = g_slist_delete_link(user->endpoints, user->endpoints);
//	}
//    
//	if (user->clientcaps != NULL)
//		g_hash_table_destroy(user->clientcaps);
//    
//	if (user->group_ids != NULL)
//	{
//		GList *l;
//		for (l = user->group_ids; l != NULL; l = l->next)
//		{
//			g_free(l->data);
//		}
//		g_list_free(user->group_ids);
//	}
//    
//	if (user->npobj != NULL)
//		np_object_destroy(user->npobj);
//    
	g_free(user->passport);
	g_free(user->friendly_name);
	g_free(user->uid);
//	if (user->extinfo) {
//		g_free(user->extinfo->media_album);
//		g_free(user->extinfo->media_artist);
//		g_free(user->extinfo->media_title);
//		g_free(user->extinfo->phone_home);
//		g_free(user->extinfo->phone_mobile);
//		g_free(user->extinfo->phone_work);
//		g_free(user->extinfo);
//	}
	g_free(user->statusline);
//	g_free(user->invite_message);
//
	g_free(user);
}

NPUser *
np_user_ref(NPUser *user)
{
	g_return_val_if_fail(user != NULL, NULL);
    
	user->refcount++;
    
	return user;
}

void
np_user_unref(NPUser *user)
{
	g_return_if_fail(user != NULL);
    
	user->refcount--;
    
	if(user->refcount == 0)
		np_user_destroy(user);
}


void np_user_update(NPUser *user)
{
    //TODO
    PurpleAccount *account;
	gboolean offline;
    
	g_return_if_fail(user != NULL);
    
	account = user->userlist->session->account;
    
	offline = (user->status == NULL);
    
	if (!offline) {
		purple_prpl_got_user_status(account, user->passport, user->status,
                                    "message", user->statusline, NULL);
	} else {
    }
    
}

void np_user_set_statusline(NPUser *user, const char *statusline)
{
    //TODO
    g_return_if_fail(user != NULL);
    
	g_free(user->statusline);
	user->passport = g_strdup(statusline);

}

void np_user_set_state(NPUser *user, const char *state)
{
    //TODO
    const char *status;
    
	g_return_if_fail(user != NULL);
    
	if (state == NULL) {
		user->status = NULL;
		return;
	}
    
	if (!g_ascii_strcasecmp(state, "BSY"))
		status = "busy";
	else if (!g_ascii_strcasecmp(state, "BRB"))
		status = "brb";
	else if (!g_ascii_strcasecmp(state, "AWY"))
		status = "away";
	else if (!g_ascii_strcasecmp(state, "PHN"))
		status = "phone";
	else if (!g_ascii_strcasecmp(state, "LUN"))
		status = "lunch";
	else if (!g_ascii_strcasecmp(state, "HDN"))
		status = NULL;
	else
		status = "available";
    
	if (!g_ascii_strcasecmp(state, "IDL"))
		user->idle = TRUE;
	else
		user->idle = FALSE;
    
	user->status = status;
}

void np_user_set_passport(NPUser *user, const char *passport)
{
    g_return_if_fail(user != NULL);
    
	g_free(user->passport);
	user->passport = g_strdup(passport);
}

gboolean np_user_set_friendly_name(NPUser *user, const char *name)
{
    //TODO
    g_return_val_if_fail(user != NULL, FALSE);
    
	if (!name)
		return FALSE;
    
	if (user->friendly_name && (!strcmp(user->friendly_name, name) ||
                                !strcmp(user->passport, name)))
		return FALSE;
    
	g_free(user->friendly_name);
	user->friendly_name = g_strdup(name);
    
	serv_got_alias(purple_account_get_connection(user->userlist->session->account),
                   user->passport, name);
	return TRUE;
    return TRUE;
}

void
np_user_set_buddy_icon(NPUser *user, PurpleStoredImage *img)
{
    //TODO
}

void
np_user_set_uid(NPUser *user, const char *uid)
{
    //TODO
}

const char
*np_user_get_passport(const NPUser *user)
{
    g_return_val_if_fail(user != NULL, NULL);
    
	return user->passport;
}

const char *np_user_get_friendly_name(const NPUser *user)
{
    g_return_val_if_fail(user != NULL, NULL);

	return user->friendly_name;
}















