//
//  niupai_session.m
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "niupai_session.h"
#include "NIDebuggingTools.h"

NPSession *np_session_new(PurpleAccount *account)
{
    NPSession *session;
    
    g_return_val_if_fail(account != NULL, NULL);
    
    session = g_new0(NPSession, 1);
    
    session->account = account;
    
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
//		purple_debug_error("msn", "This shouldn't happen\n");
		g_return_val_if_reached(FALSE);
	}
    
	return np_notification_connect(session->notification, host, port);
    return TRUE;
}

void
np_session_disconnect(NPSession *session)
{
    
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
