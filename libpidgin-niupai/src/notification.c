//
//  notification.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "notification.h"
#include "NIDebuggingTools.h"

static NPTable *cbs_table;


void np_notification_end(void)
{
    np_table_destroy(cbs_table);

}

void np_notification_init(void)
{
    cbs_table = np_table_new();
}

static void
destroy_cb(NPServConn *servconn)
{
	NPNotification *notification = NULL;
    
	notification = servconn->cmdproc->data;
	g_return_if_fail(notification != NULL);
    
	np_notification_destroy(notification);
}


NPNotification *np_notification_new(NPSession *session)
{
    NPNotification *notification;
	NPServConn *servconn;
    NIDPRINT("=========== \n");
    
    g_return_val_if_fail(session != NULL, NULL);
    
	notification = g_new0(NPNotification, 1);

    NIDPRINT("=========== \n");
    
    notification->session = session;
    notification->servconn = servconn = np_servconn_new(session, NP_SERVCONN_NS);
    np_servconn_set_destroy_cb(servconn, destroy_cb);
    NIDPRINT("=========== \n");
    
	notification->cmdproc = servconn->cmdproc;
	notification->cmdproc->data = notification;
	notification->cmdproc->cbs_table = cbs_table;
    NIDPRINT("=========== \n");
    
	return notification;
}

void np_notification_destroy(NPNotification *notification)
{
    // TODO
    // Service Connect destroy
    g_free(notification);
}

/**************************************************************************
 * Connect
 **************************************************************************/

static void
connect_cb(NPServConn *servconn)
{
    NIDPRINT("__FILE__ : %s",__FILE__);

    //TODO
	NPCmdProc *cmdproc;
	NPSession *session;
	NPTransaction *trans;
	GString *vers;
	const char *ver_str;
	int i;
    
	g_return_if_fail(servconn != NULL);
    
	cmdproc = servconn->cmdproc;
	session = servconn->session;
    
	vers = g_string_new("");
    
//	for (i = WLM_MAX_PROTOCOL; i >= WLM_MIN_PROTOCOL; i--)
//		g_string_append_printf(vers, " MSNP%d", i);
    
	g_string_append(vers, " CVR0");
    
	if (session->login_step == NP_LOGIN_STEP_START)
		np_session_set_login_step(session, NP_LOGIN_STEP_HANDSHAKE);
	else
		np_session_set_login_step(session, NP_LOGIN_STEP_HANDSHAKE2);
    
	/* Skip the initial space */
	ver_str = (vers->str + 1);
//	trans = np_transaction_new(cmdproc, "FUCK", "%s", ver_str);

    trans = np_transaction_new(cmdproc, "", "%s",NP_LOGIN_STRING);
    NIDPRINT("strelen %zd  string %s",strlen(NP_LOGIN_STRING),NP_LOGIN_STRING); 
//    trans = np_transaction_new(cmdproc, "", "%s",,NP_LOGIN_STRING);
	np_cmdproc_send_trans(cmdproc, trans);
    
	g_string_free(vers, TRUE);

}

gboolean np_notification_connect(NPNotification *notification,
                                 const char *host, int port)
{
    NIDPRINT("host : %s port : %d \n",host,port);
    
    NPServConn *servconn;
    
    g_return_val_if_fail(notification != NULL, FALSE);
    
    servconn = notification->servconn;
    
    np_servconn_set_connect_cb(servconn, connect_cb);
    
    notification->in_use = np_servconn_connect(servconn, host, port, TRUE);

    return notification->in_use;
}

void np_notification_disconnect(NPNotification *notification)
{
    g_return_if_fail(notification != NULL);
	g_return_if_fail(notification->in_use);
    
	np_servconn_disconnect(notification->servconn);
    
	notification->in_use = FALSE;

}
