//
//  niupai_session.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "contact.h"
#include "session.h"
#include "NIDebuggingTools.h"

NPSession *np_session_new(PurpleAccount *account)
{
    NPSession *session;
    
    g_return_val_if_fail(account != NULL, NULL);
    
    session = g_new0(NPSession, 1);
    
    session->account = account;
    session->notification = np_notification_new(session);
    session->userlist = np_userlist_new(session);

    session->user = np_user_new(session->userlist, purple_account_get_username(account), NULL);
    np_userlist_add_user(session->userlist, session->user);
    
    session->protocol_ver = 0;

    return session;
}

void
np_session_destroy(NPSession *session)
{
	g_return_if_fail(session != NULL);
    
	session->destroying = TRUE;
}

gboolean
np_session_connect(NPSession *session,
                            const char *host, int port,
                            gboolean http_method)
{
    NIDPRINT("host: %s  port : %d http_method : %d",host,port,http_method);
	g_return_val_if_fail(session != NULL, FALSE);
	g_return_val_if_fail(!session->connected, TRUE);

    session->connected = TRUE;
	session->http_method = http_method;
    
	if (session->notification == NULL)
	{
        NIDERROR("NP:This shouldn't happen\n");
//		purple_debug_error("np", "This shouldn't happen\n");
		g_return_val_if_reached(FALSE);
	}
    
	return np_notification_connect(session->notification, host, port);
}

void
np_session_disconnect(NPSession *session)
{
    
}


static void
np_session_sync_users(NPSession *session)
{
//	PurpleConnection *gc = purple_account_get_connection(session->account);
//	GList *to_remove = NULL;
//	GSList *buddies;
//    
//	g_return_if_fail(gc != NULL);
//    
//	/* The core used to use np_add_buddy to add all buddies before
//	 * being logged in. This no longer happens, so we manually iterate
//	 * over the whole buddy list to identify sync issues.
//	 */
//	for (buddies = purple_find_buddies(session->account, NULL); buddies;
//         buddies = g_slist_delete_link(buddies, buddies)) {
//		PurpleBuddy *buddy = buddies->data;
//		const gchar *buddy_name = purple_buddy_get_name(buddy);
//		const gchar *group_name = purple_group_get_name(purple_buddy_get_group(buddy));
//		NPUser *remote_user;
//		gboolean found = FALSE;
//        
//		remote_user = np_userlist_find_user(session->userlist, buddy_name);
//		if (remote_user && remote_user->list_op & NP_LIST_FL_OP) {
//			GList *l;
//			for (l = remote_user->group_ids; l; l = l->next) {
//				const char *name = np_userlist_find_group_name(remote_user->userlist, l->data);
//				if (name && !g_ascii_strcasecmp(group_name, name)) {
//					found = TRUE;
//					break;
//				}
//			}
//            
//			/* We don't care if they're in a different group, as long as they're on the
//			 * list somewhere. If we check for the group, we cause pain, agony and
//			 * suffering for people who decide to re-arrange their buddy list elsewhere.
//			 */
//			if (!found) {
//				if ((remote_user == NULL) || !(remote_user->list_op & NP_LIST_FL_OP)) {
//					/* The user is not on the server list */
//					np_error_sync_issue(session, buddy_name, group_name);
//				} else {
//					/* The user is not in that group on the server list */
//					to_remove = g_list_prepend(to_remove, buddy);
//				}
//			}
//		}
//	}
//    
//	if (to_remove != NULL) {
//		g_list_foreach(to_remove, (GFunc)purple_blist_remove_buddy, NULL);
//		g_list_free(to_remove);
//	}
}


void
np_session_set_error(NPSession *session, NPErrorType error,
					  const char *info)
{
	PurpleConnection *gc;
	PurpleConnectionError reason;
	char *msg;
    
	if (session->destroying)
		return;
    
	gc = purple_account_get_connection(session->account);
    
	switch (error)
	{
		case NP_ERROR_SERVCONN:
			reason = PURPLE_CONNECTION_ERROR_NETWORK_ERROR;
			msg = g_strdup(info);
			break;
		case NP_ERROR_UNSUPPORTED_PROTOCOL:
			reason = PURPLE_CONNECTION_ERROR_NETWORK_ERROR;
			msg = g_strdup(_("Our protocol is not supported by the "
							 "server"));
			break;
		case NP_ERROR_HTTP_MALFORMED:
			reason = PURPLE_CONNECTION_ERROR_NETWORK_ERROR;
			msg = g_strdup(_("Error parsing HTTP"));
			break;
		case NP_ERROR_SIGN_OTHER:
			reason = PURPLE_CONNECTION_ERROR_NAME_IN_USE;
			msg = g_strdup(_("You have signed on from another location"));
			if (!purple_account_get_remember_password(session->account))
				purple_account_set_password(session->account, NULL);
			break;
		case NP_ERROR_SERV_UNAVAILABLE:
			reason = PURPLE_CONNECTION_ERROR_NETWORK_ERROR;
			msg = g_strdup(_("The NP servers are temporarily "
							 "unavailable. Please wait and try "
							 "again."));
			break;
		case NP_ERROR_SERV_DOWN:
			reason = PURPLE_CONNECTION_ERROR_NETWORK_ERROR;
			msg = g_strdup(_("The NP servers are going down "
							 "temporarily"));
			break;
		case NP_ERROR_AUTH:
			reason = PURPLE_CONNECTION_ERROR_AUTHENTICATION_FAILED;
			msg = g_strdup_printf(_("Unable to authenticate: %s"),
								  (info == NULL ) ?
								  _("Unknown error") : info);
			/* Clear the password if it isn't being saved */
			if (!purple_account_get_remember_password(session->account))
				purple_account_set_password(session->account, NULL);
			break;
		case NP_ERROR_BAD_BLIST:
			reason = PURPLE_CONNECTION_ERROR_NETWORK_ERROR;
			msg = g_strdup(_("Your NP buddy list is temporarily "
							 "unavailable. Please wait and try "
							 "again."));
			break;
		default:
			reason = PURPLE_CONNECTION_ERROR_NETWORK_ERROR;
			msg = g_strdup(_("Unknown error"));
			break;
	}
    
	np_session_disconnect(session);
    
	purple_connection_error_reason(gc, reason, msg);
    
	g_free(msg);
}


static const char *
get_login_step_text(NPSession *session)
{
	const char *steps_text[] = {
		_("Connecting"),
		_("Handshaking"),
		_("Transferring"),
		_("Handshaking"),
		_("Starting authentication"),
		_("Getting cookie"),
		_("Authenticating"),
		_("Sending cookie"),
		_("Retrieving buddy list")
	};
    
	return steps_text[session->login_step];
}

void
np_session_set_login_step(NPSession *session, NPLoginStep step)
{
//    NIDPRINT("====== \n");
	PurpleConnection *gc;
    
	/* Prevent the connection progress going backwards, eg. if we get
	 * transferred several times during login */
	if (session->login_step >= step)
		return;
    
	/* If we're already logged in, we're probably here because of a
	 * mid-session XFR from the notification server, so we don't want to
	 * popup the connection progress dialog */
	if (session->logged_in)
		return;
    
	gc = session->account->gc;
    
	session->login_step = step;
    
	purple_connection_update_progress(gc, get_login_step_text(session), step,
                                      NP_LOGIN_STEPS);
}