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
//#include "jsmn.h"
//#include <json-glib/json-glib.h>
#include "json-glib/json-glib.h"

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
    if(notification->socket_server != NULL)
        g_free(notification->socket_server);
    
    if (notification->http_server != NULL) {
        g_free(notification->http_server);
    }

    
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
    FILE *f = fopen("/Users/ryan/Desktop/test.json", "a+");
    fprintf(f,"%s",url_text);
    fclose(f);
    JsonParser *parser = json_parser_new();
    GError *error = NULL;
    json_parser_load_from_data(parser, url_text, len, &error);
    if (error) {
        purple_debug_info("np", "error happenned : %p\n",error);

    }
}

// HTTP Part
/////////////////////////////////////////////////////////////////////////////////////

static void
np_update_cookies(NPNotification *notification,gchar *headers,gsize header_len,GError **error)
{
    purple_debug_info("np", "header_len: %zd header : %s",header_len,headers);
    
    NPSession *session;
    const gchar *cookie_start;
	const gchar *cookie_end;
	gchar *cookie_name;
	gchar *cookie_value;
//    PurpleAccount *account;
    
    
    session = notification->session;
    
    
    
	g_return_if_fail(headers != NULL);
    
	header_len = strlen(headers);
    
	/* look for the next "Set-Cookie: " */
	/* grab the data up until ';' */
	cookie_start = headers;
	while ((cookie_start = strstr(cookie_start, "\r\nSet-Cookie: ")) &&
           (cookie_start - headers) < header_len)
	{
		cookie_start += 14;
		cookie_end = strchr(cookie_start, '=');
		cookie_name = g_strndup(cookie_start, cookie_end-cookie_start);
		cookie_start = cookie_end + 1;
		cookie_end = strchr(cookie_start, ';');
		cookie_value= g_strndup(cookie_start, cookie_end-cookie_start);
		cookie_start = cookie_end;
        purple_debug_info("np", "cookie %s name %s ",cookie_name,cookie_value);
        
		g_hash_table_replace(session->cookie_table, cookie_name,
                             cookie_value);
	}
    
    purple_debug_info("npc", "%s session = %p %p",__FUNCTION__,session,session->cookie_table);
    //    account = session->account;
    
}

static void
np_update_account(NPNotification *notification,gchar *body,gsize body_len,GError **error)
{
    //    purple_debug_info("np", "body len");
    purple_debug_info("np", "body len : %zd content:",body_len);
    
    NPSession *session;

    PurpleAccount *account;
    PurpleConnection *gc;
    
    const gchar *login_string_start;
	const gchar *login_string_end;
    gchar *login_string;
    
    const char *prefix = "loginString\":\"";
    const char *suffix = "\"}";
    
    session = notification->session;
    account = session->account;
    gc = purple_account_get_connection(account);
    
    
    login_string_start = body;
    
    login_string_start = strstr(login_string_start, prefix);
    
    if (login_string_start) {
        login_string_end = login_string_start;
        login_string_end = strstr(login_string_start, suffix);
        
        if (login_string_end > login_string_start) {
            gsize len = login_string_end - login_string_start - strlen(prefix);
            login_string = g_strndup(login_string_start + strlen(prefix), len);
            purple_debug_info("np", "login_string: %s",login_string);
            purple_connection_update_progress(gc, _("Got Token!"), 1, 3);
            
            purple_debug_info("npc", "%s session = %p %p",__FUNCTION__,session,session->cookie_table);

            np_notification_socket_connect(notification);
            
            return;
        } else {
            goto login_fail;
        }
    } else {
        goto login_fail;
    }
    
login_fail:
    purple_connection_update_progress(gc, _("Fail"), 1, 3);
    
}

static void
np_update_cookies_and_account(NPNotification *notification,gchar *buffer,gsize len,GError **error)
{
    NPSession *session = notification->session;
    PurpleConnection *gc = session->account->gc;
    
    char *headers;
    size_t headers_len;
    char *body;
    
    body = g_strstr_len(buffer, len, "\r\n\r\n");
    if (body) {
        body = body + 4;
        headers_len = body - buffer;
        
        headers = g_strndup(buffer,headers_len);
        headers[headers_len] = '\0';
        
        np_update_account(notification, body, len - headers_len, error);
        np_update_cookies(notification, headers, headers_len, error);
        
        g_free(headers);
    } else {
        // FAIL
        purple_connection_error_reason(gc,
                                       PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                                       _("Unable to connect"));
        
    }
}


static void
np_http_login0_cb(PurpleUtilFetchUrlData *url_data, gpointer user_data, const gchar *url_text, gsize len, const gchar *error_message)
{
    purple_debug_info("np", "url_text: %s\n",url_text);
    NPNotification *notification = user_data;
    GError *error = NULL;
    NPSession *session = notification->session;
    
    np_session_set_login_step(session, NP_LOGIN_STEP_LOGIN_IM_SERVER);
    np_update_cookies_and_account(notification,(char *)url_text, len , &error);
    purple_debug_info("npc", "%s session = %p %p",__FUNCTION__,session,session->cookie_table);

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
    
    purple_debug_info("npc", "%s session = %p %p",__FUNCTION__,session,session->cookie_table);
    purple_debug_info("npc", "step = %d",session->login_step);

    
	if (session->login_step == NP_LOGIN_STEP_LOGIN_IM_SERVER) {
        trans = np_transaction_new(cmdproc, "LOGIN_OK", "%s",NP_LOGIN_STRING);
        trans->trId = 1;
        np_cmdproc_send_trans(cmdproc, trans);
    }
	else {
        trans = np_transaction_new(cmdproc,"HEART",NULL);
        trans->trId = 2;
        np_cmdproc_send_trans(cmdproc, trans);
    }
}

gboolean np_notification_socket_connect(NPNotification *notification)
{
    gchar *im_host = notification->socket_server;
    int im_port = notification->socket_port;
    NPSession *session;
    PurpleAccount *account;
    PurpleConnection *gc;
    NPServConn *servconn;
    
    purple_debug_warning("np","host : %s port : %d \n",im_host,im_port);
    

    session = notification->session;
    account = session->account;
    
    gc = purple_account_get_connection(account);
    g_return_val_if_fail(gc != NULL, FALSE);
    
    gc->proto_data = session;
    
	gc->flags |= PURPLE_CONNECTION_HTML | PURPLE_CONNECTION_NO_BGCOLOR | PURPLE_CONNECTION_AUTO_RESP;
    gc->flags |= PURPLE_CONNECTION_HTML | PURPLE_CONNECTION_NO_BGCOLOR | PURPLE_CONNECTION_AUTO_RESP;

    g_return_val_if_fail(notification != NULL, FALSE);
    
    servconn = notification->servconn;
    
    np_servconn_set_connect_cb(servconn, connect_cb);
    
    notification->in_use = np_servconn_connect(servconn, im_host, im_port, TRUE);
    
    return notification->in_use;
}


gboolean np_notification_connect0(NPNotification *notification,
                                  const char *socket_server, int socket_port,
                                  const char *http_server, int http_port)
{
    purple_debug_warning("np","host : %s port : %d \n",socket_server,socket_port);
    
    NPSession *session;

    if(notification->socket_server != NULL)
        g_free(notification->socket_server);
    
    notification->socket_server = g_strdup(socket_server);
    notification->socket_port = socket_port;
    
    if (notification->http_server != NULL) {
        g_free(notification->http_server);
    }
    
    notification->http_server = g_strdup(http_server);
    notification->http_port = http_port;
    
    
    
    session = notification->session;
    
    purple_debug_info("npc", "%s session = %p %p",__FUNCTION__,session,session->cookie_table);

    if (session->login_step == NP_LOGIN_STEP_START) {
        np_session_set_login_step(session, NP_LOGIN_STEP_LOGIN_HTTP_SERVER);

        np_http_login0(notification,np_http_login0_cb);
        notification->in_use = TRUE;
    }
    
//    NPServConn *servconn;
//    
//    g_return_val_if_fail(notification != NULL, FALSE);
//    
//    servconn = notification->servconn;
//
//    np_servconn_set_connect_cb(servconn, connect_cb);
//
//    notification->in_use = np_servconn_connect(servconn, host, port, TRUE);
    
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
    NPSession *session;
    session = cmdproc->session;
    np_session_finish_login(session);

    //
    http_get_friend_list(session->notification, np_got_friend_list_cb);
    
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
    
//    purple_debug_warning("np","===============\n");
//
//	NPSession *session;
//	char *host;
//	int port;
//    
//	session = cmdproc->session;
//    
//	host = g_strdup(cmd->params[2]);
//	port = atoi(cmd->params[3]);
//    
//	np_session_set_login_step(session, NP_LOGIN_STEP_SOCKET_AUTH_START);
//	np_notification_connect(session->notification, host, port);
//    
//	g_free(host);
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

