//
//  notification.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-19.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "notification.h"
#include "npconfig.h"
#include "nphttputil.h"
#include <glib-object.h>
#include <json-glib.h>

static NPTable *cbs_table;

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
    
    g_return_val_if_fail(session != NULL, NULL);
    
	notification = g_new0(NPNotification, 1);

    notification->session = session;
    notification->servconn = servconn = np_servconn_new(session, NP_SERVCONN_NS);
    np_servconn_set_destroy_cb(servconn, destroy_cb);
    
	notification->cmdproc = servconn->cmdproc;
	notification->cmdproc->data = notification;
	notification->cmdproc->cbs_table = cbs_table;
    purple_debug_warning("np","=========== \n");
    
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
np_got_friend_list_cb(PurpleUtilFetchUrlData *url_data, gpointer user_data, const gchar *url_text, gsize len, const gchar *error_message)
{
    purple_debug_info("np", "url_text: %s\n",url_text);
//    NPSession *session = user_data;

    JsonParser *parser = json_parser_new();
    GError *error = NULL;
    json_parser_load_from_data(parser, url_text, len, &error);
    if (error) {
        purple_debug_info("np", "error happenned : %p\n",error);

    }
}

static void
connect_cb(NPServConn *servconn)
{
	NPCmdProc *cmdproc;
	NPSession *session;
	NPTransaction *trans;
    
	g_return_if_fail(servconn != NULL);
    
	cmdproc = servconn->cmdproc;
	session = servconn->session;
    
	if (session->login_step == NP_LOGIN_STEP_START) {
		np_session_set_login_step(session, NP_LOGIN_STEP_SOCKET_AUTH_START);
        trans = np_transaction_new(cmdproc, "LOGIN_OK", "%s",NP_LOGIN_STRING);
        trans->trId = 1;
        np_cmdproc_send_trans(cmdproc, trans);
    }
	else {
		np_session_set_login_step(session, NP_LOGIN_STEP_SOCKET_AUTH_END);
        trans = np_transaction_new(cmdproc,"HEART",NULL);
        trans->trId = 2;
        np_cmdproc_send_trans(cmdproc, trans);
    }
}

gboolean np_notification_connect(NPNotification *notification,
                                 const char *host, int port)
{
    purple_debug_warning("np","host : %s port : %d \n",host,port);
    
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


/**************************************************************************
 * Login
 **************************************************************************/

static void
login_ok_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
    purple_debug_warning("np","===============\n");
    NPSession *session;
    session = cmdproc->session;
//    session->logged_in = TRUE;
    np_session_finish_login(session);
    
//    PurpleGroup *np_group;
//    PurpleBuddy *buddy;
//
//    np_group = purple_group_new("好友");
//    purple_blist_add_group(np_group, NULL);
//    buddy = purple_buddy_new(session->account, "name", "alias");
    purple_debug_info("npc", "session = %p",session);

    gchar *cookie =  np_session_get_encoded_cookie(cmdproc->session);
    purple_debug_warning("np","==============cookie : %s=\n",cookie);

    http_get_friend_list(session, np_got_friend_list_cb);
    
}

static void
heartbeat_ok_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
    purple_debug_warning("np","===============\n");
}

static void
ntfy_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
    purple_debug_warning("np"," %s ==============> %s\n",cmd->command,__FUNCTION__);
}

static void
pver_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
    purple_debug_warning("np","===============\n");
//	np_cmdproc_send(cmdproc, "AUTH", "DES");
}

static void auth_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	PurpleAccount *account;
    
	account = cmdproc->session->account;
    purple_debug_warning("np","===============\n");
  
//	np_cmdproc_send(cmdproc, "REQS", "DES %s", purple_account_get_username(account));
}

static void reqs_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
    
    purple_debug_warning("np","===============\n");

	NPSession *session;
	char *host;
	int port;
    
	session = cmdproc->session;
    
	host = g_strdup(cmd->params[2]);
	port = atoi(cmd->params[3]);
    
	np_session_set_login_step(session, NP_LOGIN_STEP_SOCKET_AUTH_START);
	np_notification_connect(session->notification, host, port);
    
	g_free(host);
}

/**************************************************************************
 * Log out
 **************************************************************************/
static void
logout_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
    purple_debug_warning("np","============\n");
//	if (cmd->param_count == 0)
//		np_session_set_error(cmdproc->session, -1, NULL);
//	else if (!g_ascii_strcasecmp(cmd->params[0], "OTH"))
//		msn_session_set_error(cmdproc->session, MSN_ERROR_SIGN_OTHER,
//							  NULL);
//	else if (!g_ascii_strcasecmp(cmd->params[0], "SSD"))
//		msn_session_set_error(cmdproc->session, MSN_ERROR_SERV_DOWN, NULL);
}


void
np_notification_close(NPNotification *notification)
{
	NPTransaction *trans;
    purple_debug_warning("np","============\n");

	g_return_if_fail(notification != NULL);
    
	if (!notification->in_use)
		return;
    
	trans = np_transaction_new(notification->cmdproc, "OUT", NULL);
	np_transaction_set_saveable(trans, FALSE);
	np_cmdproc_send_trans(notification->cmdproc, trans);
    
	np_notification_disconnect(notification);
}


void np_notification_end(void)
{
    np_table_destroy(cbs_table);
    
}

void np_notification_init(void)
{
    cbs_table = np_table_new();
    
    // PRS server
    
	// Login/Dispatch server
	np_table_add_cmd(cbs_table, "LOGIN_OK", "LOGIN_OK", login_ok_cmd);
	np_table_add_cmd(cbs_table, "HEART", "HEART", heartbeat_ok_cmd);
	np_table_add_cmd(cbs_table, "PVER", "PVER", pver_cmd);
	np_table_add_cmd(cbs_table, "AUTH", "AUTH", auth_cmd);
	np_table_add_cmd(cbs_table, "REQS", "REQS", reqs_cmd);
    
    /* Asynchronous */
    np_table_add_cmd(cbs_table, NULL, "NOTIFY", ntfy_cmd);

    
}

