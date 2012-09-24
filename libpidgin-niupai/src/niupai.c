/**
 * @file np.c
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "internal.h"

#include "niupai.h"
#include "accountopt.h"
//#include "debug.h"
#include "notify.h"
//#include "prefs.h"
#include "core.h"
#include "prpl.h"
//#include "privacy.h"
//#include "request.h"
//#include "roomlist.h"
//#include "server.h"
//#include "util.h"
#include "notification.h"
#include "version.h"
#include "NIDebuggingTools.h"


static GList *server_list_build(gchar select)
{
    purple_debug_warning("np","\n===>");

	GList *list = NULL;
    
	if ( select == 'T' || select == 'A') {
    purple_debug_warning("np","\n===>");

		list = g_list_append(list, "219.133.60.173:443");
		list = g_list_append(list, "219.133.49.125:443");
		list = g_list_append(list, "58.60.15.33:443");
		list = g_list_append(list, "tcpconn.tencent.com:8000");
		list = g_list_append(list, "tcpconn2.tencent.com:8000");
		list = g_list_append(list, "tcpconn3.tencent.com:8000");
		list = g_list_append(list, "tcpconn4.tencent.com:8000");
		list = g_list_append(list, "tcpconn5.tencent.com:8000");
		list = g_list_append(list, "tcpconn6.tencent.com:8000");
	}
	if ( select == 'U' || select == 'A') {
    purple_debug_warning("np","\n===>");

		list = g_list_append(list, "219.133.49.171:8000");
		list = g_list_append(list, "58.60.14.37:8000");
		list = g_list_append(list, "219.133.60.36:8000");
		list = g_list_append(list, "sz.tencent.com:8000");
		list = g_list_append(list, "sz2.tencent.com:8000");
		list = g_list_append(list, "sz3.tencent.com:8000");
		list = g_list_append(list, "sz4.tencent.com:8000");
		list = g_list_append(list, "sz5.tencent.com:8000");
		list = g_list_append(list, "sz6.tencent.com:8000");
		list = g_list_append(list, "sz7.tencent.com:8000");
		list = g_list_append(list, "sz8.tencent.com:8000");
		list = g_list_append(list, "sz9.tencent.com:8000");
	}
	return list;
}


///
//

void
np_send_im_message(NPSession *session, NPMessage *msg)
{
    
}


//

static void server_list_create(PurpleAccount *account)
{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc;
//	np_data *qd;
//	const gchar *custom_server;
//    
//	gc = purple_account_get_connection(account);
//	g_return_if_fail(gc != NULL  && gc->proto_data != NULL);
//	qd = gc->proto_data;
//    
//	qd->use_tcp = purple_account_get_bool(account, "use_tcp", TRUE);
//    
//	custom_server = purple_account_get_string(account, "server", NULL);
//    
//	if (custom_server != NULL) {
    purple_debug_warning("np","\n===>");

//		purple_debug_info("np", "Select server '%s'\n", custom_server);
//		if (*custom_server != '\0' && g_ascii_strcasecmp(custom_server, "auto") != 0) {
    purple_debug_warning("np","\n===>");

//			qd->servers = g_list_append(qd->servers, g_strdup(custom_server));
//			return;
//		}
//	}
//    
//	if (qd->use_tcp) {
    purple_debug_warning("np","\n===>");

//		qd->servers =	server_list_build('T');
//		return;
//	}
//    
//	qd->servers =	server_list_build('U');
}

static void np_login(PurpleAccount *account)
{
    PurpleConnection *gc;
    NPSession *session;
	PurplePresence *presence;
	const char *host;
	gboolean http_method = FALSE;
	int port;

    purple_debug_warning("np","\n===> %p ",account);

    fprintf(stdout, "=================> Hello\n");
    
    purple_debug_warning("np","account->alias = %s\n",account->alias);
    purple_debug_warning("np","account->username = %s\n",account->username);
    purple_debug_warning("np","account->password = %s\n",account->password);
    purple_debug_warning("np","account->user_info = %s\n",account->user_info);
    
    NIDINFO("=======> LOGIN === \n");
    
    g_return_if_fail(account != NULL);
    

    http_method = purple_account_get_bool(account, "http_method", FALSE);
    
	if (http_method)
		host = purple_account_get_string(account, "http_method_server", NP_HTTPCONN_SERVER);
	else
		host = purple_account_get_string(account, "server", NP_IM_SERVER);
	port = purple_account_get_int(account, "port", NP_IM_PORT);

    session = np_session_new(account);

    purple_debug_warning("np","=====> session : %p ",session);
    
    gc = purple_account_get_connection(account);
	g_return_if_fail(gc != NULL);

    gc->proto_data = session;
    
	gc->flags |= PURPLE_CONNECTION_HTML | PURPLE_CONNECTION_NO_BGCOLOR | PURPLE_CONNECTION_AUTO_RESP;

    np_session_set_login_step(session, NP_LOGIN_STEP_START);
    presence = purple_account_get_presence(account);

	if (!np_session_connect(session, host, port, http_method))
		purple_connection_error_reason(gc,
                                       PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                                       _("Unable to connect"));
    
}

/* clean up the given np connection and free all resources */
static void np_close(PurpleConnection *gc)
{
    purple_debug_warning("np","\n===>");
    NPSession *session;
    
	session = gc->proto_data;
    
	g_return_if_fail(session != NULL);
    
	np_session_destroy(session);
    
	gc->proto_data = NULL;

}

/* returns the icon name for a buddy or protocol */
static const gchar *np_list_icon(PurpleAccount *a, PurpleBuddy *b)
{
    purple_debug_warning("np","\n===>");

	return "np";
}


/* a short status text beside buddy icon*/
static gchar *np_status_text(PurpleBuddy *b)
{
    purple_debug_warning("np","\n===>");

    purple_debug_warning("np","\n===> ");
    
	PurpleStatus *status;
	PurplePresence *presence;
//	gchar *moodtext, *ret;
    
	presence = purple_buddy_get_presence(b);
	status = purple_presence_get_status(presence, "mood");
    
	/* we only provide Mood here
     switch(bd->status) {
    purple_debug_warning("np","\n===>");

     case np_BUDDY_OFFLINE:
     g_string_append(status, _("Offline"));
     break;
     case np_BUDDY_ONLINE_NORMAL:
     g_string_append(status, _("Online"));
     break;
     case np_BUDDY_CHANGE_TO_OFFLINE:
     g_string_append(status, _("Offline"));
     break;
     case np_BUDDY_ONLINE_AWAY:
     g_string_append(status, _("Away"));
     break;
     case np_BUDDY_ONLINE_INVISIBLE:
     g_string_append(status, _("Invisible"));
     break;
     case np_BUDDY_ONLINE_BUSY:
     g_string_append(status, _("Busy"));
     break;
     default:
     g_string_printf(status, _("Unknown-%d"), bd->status);
     }
     */
//	moodtext = purple_status_get_attr_string(status, PURPLE_MOOD_COMMENT);
//	if (moodtext)
//	{
    purple_debug_warning("np","\n===>");

//		ret = g_strdup(moodtext);		//ret will be free by invoker
//		return ret;
//	}
    
	return NULL;
}


/* a floating text when mouse is on the icon, show connection status here */
static void np_tooltip_text(PurpleBuddy *b, PurpleNotifyUserInfo *user_info, gboolean full)
{
    purple_debug_warning("np","\n===>");

//	np_buddy_data *bd;
//	gchar *tmp;
//	GString *str;
//	PurplePresence *presence;
//	PurpleStatus *status;
//	gchar *moodtext;
//    
//	g_return_if_fail(b != NULL);
//	presence = purple_buddy_get_presence(b);
//	bd = purple_buddy_get_protocol_data(b);
//	if (bd == NULL)
//		return;
//    
//	if (bd->level == NULL)
//		np_request_get_level(purple_account_get_connection(purple_buddy_get_account(b)), bd->uid);
//    
//	/* if (PURPLE_BUDDY_IS_ONLINE(b) && bd != NULL) */
//	if (bd->ip.s_addr != 0) {
    purple_debug_warning("np","\n===>");

//		str = g_string_new(NULL);
//		g_string_printf(str, "%s:%d", inet_ntoa(bd->ip), bd->port);
//		if (bd->comm_flag & np_COMM_FLAG_TCP_MODE) {
    purple_debug_warning("np","\n===>");

//			g_string_append(str, " TCP");
//		} else {
    purple_debug_warning("np","\n===>");

//			g_string_append(str, " UDP");
//		}
//		g_string_free(str, TRUE);
//	}
//    
//	tmp = g_strdup_printf("%d", bd->age);
//	purple_notify_user_info_add_pair(user_info, _("Age"), tmp);
//	g_free(tmp);
//    
//	switch (bd->gender) {
    purple_debug_warning("np","\n===>");

//		case np_BUDDY_GENDER_GG:
//			purple_notify_user_info_add_pair(user_info, _("Gender"), _("Male"));
//			break;
//		case np_BUDDY_GENDER_MM:
//			purple_notify_user_info_add_pair(user_info, _("Gender"), _("Female"));
//			break;
//		default:
//			purple_notify_user_info_add_pair(user_info, _("Gender"), _("Unknown"));
//	}
//    
//	if (bd->level) {
    purple_debug_warning("np","\n===>");

//		tmp = g_strdup_printf("%d", bd->level);
//		purple_notify_user_info_add_pair(user_info, _("Level"), tmp);
//		g_free(tmp);
//	}
//    
//	str = g_string_new(NULL);
//	if (bd->comm_flag & np_COMM_FLAG_np_MEMBER) {
    purple_debug_warning("np","\n===>");

//		g_string_append( str, _("Member") );
//	}
//	if (bd->comm_flag & np_COMM_FLAG_np_VIP) {
    purple_debug_warning("np","\n===>");

//		g_string_append( str, _(" VIP") );
//	}
//	if (bd->comm_flag & np_COMM_FLAG_TCP_MODE) {
    purple_debug_warning("np","\n===>");

//		g_string_append( str, _(" TCP") );
//	}
//	if (bd->comm_flag & np_COMM_FLAG_MOBILE) {
    purple_debug_warning("np","\n===>");

//		g_string_append( str, _(" FromMobile") );
//	}
//	if (bd->comm_flag & np_COMM_FLAG_BIND_MOBILE) {
    purple_debug_warning("np","\n===>");

//		g_string_append( str, _(" BindMobile") );
//	}
//	if (bd->comm_flag & np_COMM_FLAG_VIDEO) {
    purple_debug_warning("np","\n===>");

//		g_string_append( str, _(" Video") );
//	}
//	if (bd->ext_flag & np_EXT_FLAG_ZONE) {
    purple_debug_warning("np","\n===>");

//		g_string_append( str, _(" Zone") );
//	}
//    
//	purple_notify_user_info_add_pair(user_info, _("Flag"), str->str);
//	g_string_free(str, TRUE);
//    
//	status = purple_presence_get_status(presence, PURPLE_MOOD_NAME);
//	moodtext = purple_status_get_attr_string(status, PURPLE_MOOD_COMMENT);
//	if (moodtext)
//	{
    purple_debug_warning("np","\n===>");

//		purple_notify_user_info_add_pair(user_info, _("Signature"), moodtext);
//	}
//    
//#ifdef DEBUG
//	tmp = g_strdup_printf( "%s (%04X)",
//                          np_get_ver_desc(bd->client_tag),
//                          bd->client_tag );
//	purple_notify_user_info_add_pair(user_info, _("Ver"), tmp);
//	g_free(tmp);
//    
//	tmp = g_strdup_printf( "Ext 0x%X, Comm 0x%X",
//                          bd->ext_flag, bd->comm_flag );
//	purple_notify_user_info_add_pair(user_info, _("Flag"), tmp);
//	g_free(tmp);
//#endif
}

/* we can show tiny icons on the four corners of buddy icon, */
static const char *np_list_emblem(PurpleBuddy *b)
{
    purple_debug_warning("np","\n===>");

//	PurpleAccount *account;
//	np_buddy_data *buddy;
//    
//	if (!b || !(account = purple_buddy_get_account(b)) ||
//		!purple_account_get_connection(account))
//		return NULL;
//    
//	buddy = purple_buddy_get_protocol_data(b);
//	if (!buddy) {
    purple_debug_warning("np","\n===>");

//		return "not-authorized";
//	}
//    
//	if (buddy->comm_flag & np_COMM_FLAG_MOBILE)
//		return "mobile";
//	if (buddy->comm_flag & np_COMM_FLAG_VIDEO)
//		return "video";
//	if (buddy->comm_flag & np_COMM_FLAG_np_MEMBER)
//		return "np_member";
    
	return NULL;
}

/* np away status (used to initiate np away packet) */
static GList *np_status_types(PurpleAccount *ga)
{
    purple_debug_warning("np","\n===> %s",ga->alias);
    purple_debug_warning("np","\n===> %p",ga->system_log);
    purple_debug_warning("np","\n===> %s",ga->username);
    purple_debug_warning("np","\n===> %s",ga->password);

	PurpleStatusType *status;
	GList *types = NULL;
    
	status = purple_status_type_new_full(PURPLE_STATUS_AVAILABLE,
                                         "available", _("Available"), TRUE, TRUE, FALSE);
	types = g_list_append(types, status);
    
    status = purple_status_type_new_full(PURPLE_STATUS_OFFLINE,
                                         "offline", _("Offline"), TRUE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	status = purple_status_type_new_with_attrs(PURPLE_STATUS_MOOD,
                                               PURPLE_MOOD_NAME, _("Mood"), TRUE, TRUE, TRUE, PURPLE_MOOD_COMMENT, _("Mood Text"), purple_value_new(PURPLE_TYPE_STRING), NULL);
	types = g_list_append(types, status);
    
	return types;
}

static void
np_change_status(PurpleAccount *account, PurpleStatus *status)
{
    // TODO
    //	PurpleConnection *gc = purple_account_get_connection(account);
    //	np_request_change_status(gc, 0);
}

static void
np_keepalive(PurpleConnection *gc)
{
    purple_debug_warning("np","keeping alive %ld\n",time(NULL));
    
    NPSession *session;
	NPTransaction *trans;
    
	session = gc->proto_data;
    

    NPCmdProc *cmdproc;
    
	cmdproc = session->notification->cmdproc;
        
    trans = np_transaction_new(cmdproc, "HEART","%s","HEART",NULL);
    trans->trId = 'H';
    np_transaction_set_saveable(trans, FALSE);
    np_cmdproc_send_trans(cmdproc, trans);
}

/* send packet to get who's detailed information */
static void np_show_buddy_info(PurpleConnection *gc, const gchar *who)
{
    purple_debug_warning("np","\n===>");

//	guint32 uid;
//	np_data *qd;
//    
//	qd = gc->proto_data;
//	uid = purple_name_to_uid(who);
//    
//	if (uid <= 0) {
    purple_debug_warning("np","\n===>");

//		purple_debug_error("np", "Not valid npid: %s\n", who);
//		purple_notify_error(gc, NULL, _("Invalid name"), NULL);
//		return;
//	}
//    
//	np_request_get_level(gc, uid);
//    
//	np_request_get_buddy_info(gc, uid, 0, np_BUDDY_INFO_DISPLAY);
//}
//
//static void action_update_all_rooms(PurplePluginAction *action)
//{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc = (PurpleConnection *) action->context;
//	np_data *qd;
//    
//	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
//	qd = (np_data *) gc->proto_data;
//    
//	if ( !qd->is_login ) {
    purple_debug_warning("np","\n===>");

//		return;
//	}
//    
//	np_update_all_rooms(gc, 0, 0);
}

static void action_change_icon(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc = (PurpleConnection *) action->context;
//	np_data *qd;
//	gchar *icon_name;
//	gchar *icon_path;
//    
//	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
//	qd = (np_data *) gc->proto_data;
//    
//	if ( !qd->is_login ) {
    purple_debug_warning("np","\n===>");

//		return;
//	}
//    
//	icon_name = np_get_icon_name(qd->my_icon);
//	icon_path = np_get_icon_path(icon_name);
//	g_free(icon_name);
//    
//	purple_debug_info("np", "Change prev icon %s to...\n", icon_path);
//	purple_request_file(action, _("Select icon..."), icon_path,
//                        FALSE,
//                        G_CALLBACK(np_change_icon_cb), NULL,
//                        purple_connection_get_account(gc), NULL, NULL,
//                        gc);
//	g_free(icon_path);
}

static void action_modify_info_base(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc = (PurpleConnection *) action->context;
//	np_data *qd;
//    
//	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
//	qd = (np_data *) gc->proto_data;
//	np_request_get_buddy_info(gc, qd->uid, 0, np_BUDDY_INFO_MODIFY_BASE);
}

static void action_modify_info_ext(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc = (PurpleConnection *) action->context;
//	np_data *qd;
//    
//	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
//	qd = (np_data *) gc->proto_data;
//	np_request_get_buddy_info(gc, qd->uid, 0, np_BUDDY_INFO_MODIFY_EXT);
}

static void action_modify_info_addr(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc = (PurpleConnection *) action->context;
//	np_data *qd;
//    
//	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
//	qd = (np_data *) gc->proto_data;
//	np_request_get_buddy_info(gc, qd->uid, 0, np_BUDDY_INFO_MODIFY_ADDR);
}

static void action_modify_info_contact(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc = (PurpleConnection *) action->context;
//	np_data *qd;
//    
//	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
//	qd = (np_data *) gc->proto_data;
//	np_request_get_buddy_info(gc, qd->uid, 0, np_BUDDY_INFO_MODIFY_CONTACT);
}

static void action_change_password(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

	PurpleConnection *gc = (PurpleConnection *) action->context;
    
	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
	purple_notify_uri(NULL, "https://password.np.com");
}

/* show a brief summary of what we get from login packet */
static void action_show_account_info(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

//	PurpleConnection *gc = (PurpleConnection *) action->context;
//	np_data *qd;
//	GString *info;
//	struct tm *tm_local;
//	int index;
//    
//	g_return_if_fail(NULL != gc && NULL != gc->proto_data);
//	qd = (np_data *) gc->proto_data;
//	info = g_string_new("<html><body>");
//    
//	tm_local = localtime(&qd->login_time);
//	g_string_append_printf(info, _("<b>Login time</b>: %d-%d-%d, %d:%d:%d<br>\n"),
//                           (1900 +tm_local->tm_year), (1 + tm_local->tm_mon), tm_local->tm_mday,
//                           tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
//	g_string_append_printf(info, _("<b>Total Online Buddies</b>: %d<br>\n"), qd->online_total);
//	tm_local = localtime(&qd->online_last_update);
//	g_string_append_printf(info, _("<b>Last Refresh</b>: %d-%d-%d, %d:%d:%d<br>\n"),
//                           (1900 +tm_local->tm_year), (1 + tm_local->tm_mon), tm_local->tm_mday,
//                           tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
//    
//	g_string_append(info, "<hr>");
//    
//	g_string_append_printf(info, _("<b>Server</b>: %s<br>\n"), qd->curr_server);
//	g_string_append_printf(info, _("<b>Client Tag</b>: %s<br>\n"), np_get_ver_desc(qd->client_tag));
//	g_string_append_printf(info, _("<b>Connection Mode</b>: %s<br>\n"), qd->use_tcp ? "TCP" : "UDP");
//	g_string_append_printf(info, _("<b>My Internet IP</b>: %s:%d<br>\n"), inet_ntoa(qd->my_ip), qd->my_port);
//    
//	g_string_append(info, "<hr>");
//	g_string_append(info, "<i>Network Status</i><br>\n");
//	g_string_append_printf(info, _("<b>Sent</b>: %lu<br>\n"), qd->net_stat.sent);
//	g_string_append_printf(info, _("<b>Resend</b>: %lu<br>\n"), qd->net_stat.resend);
//	g_string_append_printf(info, _("<b>Lost</b>: %lu<br>\n"), qd->net_stat.lost);
//	g_string_append_printf(info, _("<b>Received</b>: %lu<br>\n"), qd->net_stat.rcved);
//	g_string_append_printf(info, _("<b>Received Duplicate</b>: %lu<br>\n"), qd->net_stat.rcved_dup);
//    
//	g_string_append(info, "<hr>");
//	g_string_append(info, "<i>Last Login Information</i><br>\n");
//    
//	for (index = 0; index < sizeof(qd->last_login_time) / sizeof(time_t); index++) {
    purple_debug_warning("np","\n===>");

//		tm_local = localtime(&qd->last_login_time[index]);
//		g_string_append_printf(info, _("<b>Time</b>: %d-%d-%d, %d:%d:%d<br>\n"),
//                               (1900 +tm_local->tm_year), (1 + tm_local->tm_mon), tm_local->tm_mday,
//                               tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
//	}
//	if (qd->last_login_ip.s_addr != 0) {
    purple_debug_warning("np","\n===>");

//		g_string_append_printf(info, _("<b>IP</b>: %s<br>\n"), inet_ntoa(qd->last_login_ip));
//	}
//    
//	g_string_append(info, "</body></html>");
//    
//	purple_notify_formatted(gc, NULL, _("Login Information"), NULL, info->str, NULL, NULL);
//    
//	g_string_free(info, TRUE);
}

static void action_about_libnp(PurplePluginAction *action)
{
    purple_debug_warning("np","\n===>");

	PurpleConnection *gc = (PurpleConnection *) action->context;
	GString *info;
	gchar *title;
    
	g_return_if_fail(NULL != gc);
    
	info = g_string_new("<html><body>");
	g_string_append(info, _("<p><b>Original Author</b>:<br>\n"));
	g_string_append(info, "puzzlebird<br>\n");
	g_string_append(info, "<br>\n");
    
	g_string_append(info, _("<p><b>Code Contributors</b>:<br>\n"));
	g_string_append(info, "gfhuang(poppyer) : patches for libpurple 2.0.0beta2, maintainer<br>\n");
	g_string_append(info, "Yuan Qingyun : patches for libpurple 1.5.0, maintainer<br>\n");
	g_string_append(info, "henryouly : file transfer, udp sock5 proxy and np_show, maintainer<br>\n");
	g_string_append(info, "hzhr : maintainer<br>\n");
	g_string_append(info, "joymarquis : maintainer<br>\n");
	g_string_append(info, "arfankai : fixed bugs in char_conv.c<br>\n");
	g_string_append(info, "rakescar : provided filter for HTML tag<br>\n");
	g_string_append(info, "yyw : improved performance on PPC linux<br>\n");
	g_string_append(info, "lvxiang : provided ip to location original code<br>\n");
	g_string_append(info, "markhuetsch : OpenQ merge into libpurple, maintainer 2006-2007<br>\n");
	g_string_append(info, "ccpaging : maintainer since 2007<br>\n");
	g_string_append(info, "icesky : maintainer since 2007<br>\n");
	g_string_append(info, "csyfek : faces, maintainer since 2007<br>\n");
	g_string_append(info, "V.E.O : maintainer since 2011, OpenQ rename to Libnp<br>\n");
	g_string_append(info, "cnangel : maintainer since 2012<br>\n");
	g_string_append(info, "worr : maintainer / Adiumnp developer<br>\n");
	g_string_append(info, "<br>\n");
    
	g_string_append(info, _("<p><b>Lovely Patch Writers</b>:<br>\n"));
	g_string_append(info, "gnap : message displaying, documentation<br>\n");
	g_string_append(info, "manphiz : room processing<br>\n");
	g_string_append(info, "moo : room processing<br>\n");
	g_string_append(info, "Coly Li : room processing<br>\n");
	g_string_append(info, "Emil Alexiev : captcha verification on login based on Lumanp for MAC (2007), login, add buddy, remove buddy, message exchange and logout<br>\n");
	g_string_append(info, "Chengming Wang : buddy memo<br>\n");
	g_string_append(info, "lonicerae : chat room window bugfix, server list bugfix, buddy memo<br>\n");
	g_string_append(info, "<br>\n");
    
	g_string_append(info, _("<p><b>Acknowledgement</b>:<br>\n"));
	g_string_append(info, "Shufeng Tan : http://sf.net/projects/perl-oicq<br>\n");
	g_string_append(info, "Jeff Ye : http://www.sinomac.com<br>\n");
	g_string_append(info, "Hu Zheng : http://forlinux.yeah.net<br>\n");
	g_string_append(info, "yunfan : http://www.myswear.net<br>\n");
	g_string_append(info, "OpenQ Team : http://openq.linuxsir.org<br>\n");
	g_string_append(info, "Lumanp Team : http://lumanp.linuxsir.org<br>\n");
	g_string_append(info, "Pidgin Team : http://www.pidgin.im<br>\n");
	g_string_append(info, "Huang Guan : http://home.xxsyzx.com<br>\n");
	g_string_append(info, "OpenQ Google Group : http://groups.google.com/group/openq<br>\n");
	g_string_append(info, "Libnp Google Code : http://libnp-pidgin.googlecode.com<br>\n");
	g_string_append(info, "Libnp Github Code : https://github.com/cnangel/pidgin-libnp<br>\n");
	g_string_append(info, "<br>\n");
    
	g_string_append(info, _("<p><b>Scrupulous Testers</b>:<br>\n"));
	g_string_append(info, "yegle<br>\n");
	g_string_append(info, "cnzhangbx<br>\n");
	g_string_append(info, "casparant<br>\n");
	g_string_append(info, "wd<br>\n");
	g_string_append(info, "x6719620<br>\n");
	g_string_append(info, "netelk<br>\n");
	g_string_append(info, _("and more, please let me know... thank you!))"));
	g_string_append(info, "<br>\n<br>\n");
	g_string_append(info, _("<p><i>And, all the boys in the backroom...</i><br>\n"));
	g_string_append(info, _("<i>Feel free to join us!</i> :)"));
	g_string_append(info, "</body></html>");
    
	title = g_strdup_printf(_("About Libnp %s"), LIBNP_VERSION);
	purple_notify_formatted(gc, title, title, NULL, info->str, NULL, NULL);
    
	g_free(title);
	g_string_free(info, TRUE);
}

/*
 static void _np_menu_search_or_add_permanent_group(PurplePluginAction *action)
 {
    purple_debug_warning("np","\n===>");

 purple_roomlist_show_with_account(NULL);
 }
 */

/*
 static void _np_menu_create_permanent_group(PurplePluginAction * action)
 {
    purple_debug_warning("np","\n===>");

 PurpleConnection *gc = (PurpleConnection *) action->context;
 purple_request_input(gc, _("Create np Qun"),
 _("Input Qun name here"),
 _("Only np members can create permanent Qun"),
 "libnp", FALSE, FALSE, NULL,
 _("Create"), G_CALLBACK(np_create_room), _("Cancel"), NULL, gc);
 }
 */

static void action_chat_quit(PurpleBlistNode * node)
{
    purple_debug_warning("np","\n===>");

//	PurpleChat *chat = (PurpleChat *)node;
//	PurpleAccount *account = purple_chat_get_account(chat);
//	PurpleConnection *gc = purple_account_get_connection(account);
//	GHashTable *components = purple_chat_get_components(chat);
//	gchar *num_str;
//	guint32 room_id;
//
//	
//	g_return_if_fail(PURPLE_BLIST_NODE_IS_CHAT(node));
//
//	g_return_if_fail(components != NULL);
//
//	num_str = g_hash_table_lookup(components, np_ROOM_KEY_INTERNAL_ID);
//	if (!num_str)
//	{
    purple_debug_warning("np","\n===>");

//		purple_debug_error("np", "Cannot find Room! Wait and Retry");
//		return;
//	}
//	room_id = strtoul(num_str, NULL, 10);
//	g_return_if_fail(room_id != 0);
//    
//	np_room_quit(gc, room_id);
}

static void action_show_chat(PurpleBlistNode * node, gpointer flag)
{
    purple_debug_warning("np","\n===>");

//	PurpleChat *chat = (PurpleChat *)node;
//	PurpleAccount *account = purple_chat_get_account(chat);
//	PurpleConnection *gc = purple_account_get_connection(account);
//	GHashTable *components = purple_chat_get_components(chat);
//	gchar *num_str;
//	guint32 room_id;
//	np_room_data *rmd;
//    
//	g_return_if_fail(PURPLE_BLIST_NODE_IS_CHAT(node));
//    
//	g_return_if_fail(components != NULL);
//    
//	num_str = g_hash_table_lookup(components, np_ROOM_KEY_INTERNAL_ID);
//	if (!num_str)
//	{
    purple_debug_warning("np","\n===>");

//		purple_debug_error("np", "Cannot find Room! Wait and Retry");
//		return;
//	}
//	room_id = strtoul(num_str, NULL, 10);
//	g_return_if_fail(room_id != 0);
//    
//	rmd = np_room_data_find(gc, room_id);
//	g_return_if_fail(rmd != NULL);
//	
//	if (flag)
//	{
    purple_debug_warning("np","\n===>");

//		rmd->is_show_chat = TRUE;
//		g_hash_table_replace(components, g_strdup(np_ROOM_KEY_ISSHOW), g_strdup_printf("%u", TRUE));
//		purple_notify_info(gc, _("np Chat Room"), _("Receive and Show np Chat Room Message"), num_str);
//	} else {
    purple_debug_warning("np","\n===>");

//		rmd->is_show_chat = FALSE;
//		g_hash_table_replace(components, g_strdup(np_ROOM_KEY_ISSHOW), g_strdup_printf("%u", FALSE));
//		purple_notify_info(gc, _("np Chat Room"), _("np Chat Room Message Blocked"), num_str);
//	}
}
static void action_chat_get_info(PurpleBlistNode * node)
{
    purple_debug_warning("np","\n===>");

//	PurpleChat *chat = (PurpleChat *)node;
//	PurpleAccount *account = purple_chat_get_account(chat);
//	PurpleConnection *gc = purple_account_get_connection(account);
//	GHashTable *components = purple_chat_get_components(chat);
//	gchar *num_str;
//	guint32 room_id;
//    
//	g_return_if_fail(PURPLE_BLIST_NODE_IS_CHAT(node));
//    
//	g_return_if_fail(components != NULL);
//    
//	num_str = g_hash_table_lookup(components, np_ROOM_KEY_INTERNAL_ID);
//	if (!num_str)
//	{
    purple_debug_warning("np","\n===>");

//		purple_debug_error("np", "Cannot find Room! Wait and Retry");
//		return;
//	}
//	room_id = strtoul(num_str, NULL, 10);
//	g_return_if_fail(room_id != 0);
//    
//	np_send_room_cmd_mess(gc, np_ROOM_CMD_GET_INFO, room_id, NULL, 0,
//                          0, np_ROOM_INFO_DISPLAY);
}

#if 0
/* TODO: re-enable this */
static void _np_menu_send_file(PurpleBlistNode * node, gpointer ignored)
{
    purple_debug_warning("np","\n===>");

	PurpleBuddy *buddy;
	PurpleConnection *gc;
	np_buddy_data *bd;
    
	g_return_if_fail (PURPLE_BLIST_NODE_IS_BUDDY (node));
	buddy = (PurpleBuddy *) node;
	bd = (np_buddy_data *) buddy->proto_data;
	/*	if (is_online (bd->status)) {
    purple_debug_warning("np","\n===>");
 */
	gc = purple_account_get_connection (buddy->account);
	g_return_if_fail (gc != NULL && gc->proto_data != NULL);
	np_send_file(gc, buddy->name, NULL);
	/*	} */
}
#endif

/* protocol related menus */
static GList *np_actions(PurplePlugin *plugin, gpointer context)
{
    purple_debug_warning("np","\n===>");

	GList *m;
	PurplePluginAction *act;
    
	m = NULL;
	/* TODO: MASK ALL UNFIXED BUT TOUCHABLE FUNCTION !!*/
    
     act = purple_plugin_action_new(_("Change Icon"), action_change_icon);
     m = g_list_append(m, act);
     
     act = purple_plugin_action_new(_("Modify Information"), action_modify_info_base);
     m = g_list_append(m, act);
     
     act = purple_plugin_action_new(_("Modify Extended Information"), action_modify_info_ext);
     m = g_list_append(m, act);
     
     act = purple_plugin_action_new(_("Modify Address"), action_modify_info_addr);
     m = g_list_append(m, act);
     
     act = purple_plugin_action_new(_("Modify Contact"), action_modify_info_contact);
     m = g_list_append(m, act);
     
     act = purple_plugin_action_new(_("Change Password"), action_change_password);
     m = g_list_append(m, act);
     
     act = purple_plugin_action_new(_("Account Information"), action_show_account_info);
     m = g_list_append(m, act);
     
	act = purple_plugin_action_new(_("About LibNiupai"), action_about_libnp);
	m = g_list_append(m, act);
    
	return m;
}

static void np_add_buddy_from_menu_cb(PurpleBlistNode *node, gpointer data)
{
    purple_debug_warning("np","\n===>");

//	PurpleBuddy *buddy;
//	PurpleConnection *gc;
//    
//	g_return_if_fail(PURPLE_BLIST_NODE_IS_BUDDY(node));
//    
//	buddy = (PurpleBuddy *) node;
//	gc = purple_account_get_connection(purple_buddy_get_account(buddy));
//    
//	np_add_buddy(gc, buddy, NULL);
}

static void np_modify_buddy_memo_from_menu_cb(PurpleBlistNode *node, gpointer data)
{
    purple_debug_warning("np","\n===>");

//	PurpleBuddy *buddy;
//	np_buddy_data *bd;
//	PurpleConnection *gc;
//	guint32 bd_uid;
//    
//	g_return_if_fail(PURPLE_BLIST_NODE_IS_BUDDY(node));
//    
//	buddy = (PurpleBuddy *)node;
//	g_return_if_fail(NULL != buddy);
//    
//	gc = purple_account_get_connection(purple_buddy_get_account(buddy));
//	g_return_if_fail(NULL != gc);
//    
//	bd = (np_buddy_data *)purple_buddy_get_protocol_data(buddy);
//	g_return_if_fail(NULL != bd);
//	bd_uid = bd->uid;
//    
//	/* param: gc, uid, update_class, action
//	 * here, update_class is set to bd_uid. because some memo packages returned
//	 * without uid, which will make us confused */
//	np_request_buddy_memo(gc, bd_uid, 0, np_BUDDY_MEMO_MODIFY);
}

static GList *np_buddy_menu(PurpleBuddy *buddy)
{
    purple_debug_warning("np","\n===>");

	GList *m = NULL;
//	PurpleMenuAction *act;
//	np_buddy_data *bd = purple_buddy_get_protocol_data(buddy);
//    
//	if (bd == NULL) {
    purple_debug_warning("np","\n===>");

//		act = purple_menu_action_new(_("Add Buddy"),
//                                     PURPLE_CALLBACK(np_add_buddy_from_menu_cb),
//                                     NULL, NULL);
//		m = g_list_append(m, act);
//		return m;
//	}
//    
//	/* TODO: MASK ALL UNFIXED BUT TOUCHABLE FUNCTION !!
//     act = purple_menu_action_new(_("Modify Buddy Alias"),
//     PURPLE_CALLBACK(np_modify_buddy_memo_from_menu_cb),
//     NULL, NULL);
//     m = g_list_append(m, act);
//     */
//    
//    
//	/* TODO : not working, temp commented out by gfhuang */
//#if 0
//	if (bd && is_online(bd->status)) {
    purple_debug_warning("np","\n===>");

//		act = purple_menu_action_new(_("Send File"), PURPLE_CALLBACK(_np_menu_send_file), NULL, NULL); /* add NULL by gfhuang */
//		m = g_list_append(m, act);
//	}
//#endif
	return m;
}

/* chat-related (np Qun) menu shown up with right-click */
static GList *np_chat_menu(PurpleBlistNode *node)
{
    purple_debug_warning("np","\n===>");

	GList *m;
	PurpleMenuAction *act;
    
	m = NULL;
    
	act = purple_menu_action_new(_(" Block chat room msg"), PURPLE_CALLBACK(action_show_chat), NULL, NULL);
	m = g_list_append(m, act);
    
	act = purple_menu_action_new(_(" Show chat room msg"), PURPLE_CALLBACK(action_show_chat), (gpointer)1, NULL);
	m = g_list_append(m, act);
    
	act = purple_menu_action_new(_("Get Info"), PURPLE_CALLBACK(action_chat_get_info), NULL, NULL);
	m = g_list_append(m, act);
    
	/* TODO: MASK ALL UNFIXED BUT TOUCHABLE FUNCTION !!
     act = purple_menu_action_new(_("Quit Qun"), PURPLE_CALLBACK(action_chat_quit), NULL, NULL);
     m = g_list_append(m, act);
     */
	return m;
}

/* buddy-related menu shown up with right-click */
static GList *np_blist_node_menu(PurpleBlistNode * node)
{
    purple_debug_warning("np","\n===>");

	if(PURPLE_BLIST_NODE_IS_CHAT(node))
		return np_chat_menu(node);
    
	if(PURPLE_BLIST_NODE_IS_BUDDY(node))
		return np_buddy_menu((PurpleBuddy *) node);
    
	return NULL;
}

/* convert name displayed in a chat channel to original np UID */
static gchar *chat_name_to_purple_name(const gchar *const name)
{
    purple_debug_warning("np","\n===>");

	const char *start;
	const char *end;
	gchar *ret;
    
	g_return_val_if_fail(name != NULL, NULL);
    
	/* Sample: (1234567)*/
	start = strchr(name, '(');
	g_return_val_if_fail(start != NULL, NULL);
	end = strchr(start, ')');
	g_return_val_if_fail(end != NULL && (end - start) > 1, NULL);
    
	ret = g_strndup(start + 1, end - start - 1);
    
	return ret;
}

/* convert chat nickname to uid to get this buddy info */
/* who is the nickname of buddy in np chat-room (Qun) */
//static void np_get_chat_buddy_info(PurpleConnection *gc, gint channel, const gchar *who)
//{

//	np_data *qd;
//	gchar *uid_str;
//	guint32 uid;
//    
//	purple_debug_info("np", "Get chat buddy info of %s\n", who);
//	g_return_if_fail(who != NULL);
//    
//	uid_str = chat_name_to_purple_name(who);
//	if (uid_str == NULL) {

//		return;
//	}
//    
//	qd = gc->proto_data;
//	uid = purple_name_to_uid(uid_str);
//	g_free(uid_str);
//    
//	if (uid <= 0) {

//		purple_debug_error("np", "Not valid chat name: %s\n", who);
//		purple_notify_error(gc, NULL, _("Invalid name"), NULL);
//		return;
//	}
//    
//	np_request_get_buddy_info(gc, uid, 0, np_BUDDY_INFO_DISPLAY);
//}

/* convert chat nickname to uid to invite individual IM to buddy */
/* who is the nickname of buddy in np chat-room (Qun) */
static gchar *np_get_chat_buddy_real_name(PurpleConnection *gc, gint channel, const gchar *who)
{
    purple_debug_warning("np","\n===>");

	g_return_val_if_fail(who != NULL, NULL);
	return chat_name_to_purple_name(who);
}


static gboolean np_load(PurplePlugin *plugin)
{
    purple_debug_warning("np","=======>n");
    
    purple_debug_warning("np","====================> \n");
    np_notification_init();
	return TRUE;
}

static gboolean np_unload(PurplePlugin *plugin)
{
    np_notification_end();
    return TRUE;
}



static PurplePluginProtocolInfo prpl_info =
{

	OPT_PROTO_CHAT_TOPIC | OPT_PROTO_USE_POINTSIZE | OPT_PROTO_IM_IMAGE,
	NULL,						/* user_splits	*/
	NULL,						/* protocol_options */
	{"png", 96, 96, 96, 96, 0, PURPLE_ICON_SCALE_SEND}, /* icon_spec */
	np_list_icon,				/* list_icon */
	np_list_emblem,				/* list_emblems */
	np_status_text,				/* status_text	*/
	np_tooltip_text,			/* tooltip_text */
	np_status_types,			/* away_states	*/
	np_blist_node_menu,			/* blist_node_menu */
	NULL,				/* chat_info */
	NULL,		/* chat_info_defaults */
	np_login,					/* open */
	np_close,					/* close */
	NULL,                 /* send_im */
	NULL,                       /* set_info */
	NULL,				/* send_typing	*/
	np_show_buddy_info,         /* get_info */
	np_change_status,			/* change status */
	NULL,						/* set_idle */
	NULL,						/* change_passwd */
	NULL,               /* add_buddy */
	NULL,						/* add_buddies	*/
	NULL,            /* remove_buddy */
	NULL,						/* remove_buddies */
	NULL,						/* add_permit */
	NULL,						/* add_deny */
	NULL,						/* rem_permit */
	NULL,						/* rem_deny */
	NULL,						/* set_permit_deny */
	NULL,              /* join_chat */
	NULL,						/* reject chat	invite */
	NULL,						/* get_chat_name */
	NULL,						/* chat_invite	*/
	NULL,						/* chat_leave */
	NULL,						/* chat_whisper */
	NULL,                       /* chat_send */
	np_keepalive,               /* keepalive */
	NULL,						/* register_user */
	NULL,                       /* get_cb_info	*/
	NULL,							/* get_cb_away	*/
	NULL,							/* alias_buddy	*/
	NULL,							/* change buddy's group	*/
	NULL,							/* rename_group */
	NULL,							/* buddy_free */
	NULL,							/* convo_closed */
	NULL,							/* normalize */
	NULL,
	NULL,							/* remove_group */
	NULL,    /* get_cb_real_name */
	NULL,							/* set_chat_topic */
	NULL,							/* find_blist_chat */
	purple_roomlist_get_list,           /* roomlist_get_list */
	NULL,             /* roomlist_cancel */
	NULL,							/* roomlist_expand_category */
	NULL,							/* can_receive_file */
	NULL,							/* np_send_file send_file */
	NULL,							/* new xfer */
	NULL,							/* offline_message */
	NULL,							/* PurpleWhiteboardPrplOps */
	NULL,							/* send_raw */
	NULL,							/* roomlist_room_serialize */
	NULL,							/* unregister_user */
	NULL,							/* send_attention */
	NULL,							/* get attention_types */
    
	sizeof(PurplePluginProtocolInfo), /* struct_size */
	NULL,							/* get_account_text_table */
	NULL,							/* initiate_media */
	NULL,							/* get_media_caps */
	NULL,							/* get_moods */
	NULL,							/* set_public_alias */
	NULL							/* get_public_alias */
};

static PurplePluginInfo info = {

	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_PROTOCOL,         /**< type           */
	NULL,                           /**< ui_requirement	*/
	0,                              /**< flags          */
	NULL,                           /**< dependencies	*/
	PURPLE_PRIORITY_DEFAULT,		/**< priority		*/
    
	NP_PLUGIN_ID,                   /**< id             */
	NP_PLUGIN_NAME,                 /**< name           */
	LIBNP_VERSION,                  /**< version		*/
	N_(NP_PLUGIN_SUMMARY),              /**< summary		*/

	N_(NP_PLUGIN_DESCRIPTION),          /**< description	*/
	NP_PLUGIN_AUTHOR,               /**< author         */
	PURPLE_WEBSITE,                 /**< homepage       */
    
	np_load,                        /**< load		*/
	np_unload,                      /**< unload		*/
	NULL,                           /**< destroy		*/
    
	NULL,                           /**< ui_info		*/
	&prpl_info,                     /**< extra_info		*/
	NULL,                           /**< prefs_info		*/
	np_actions,
    
	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static void
init_plugin(PurplePlugin *plugin)
{
    purple_debug_warning("np","\n=== START LOG ==\n");
    
    PurpleAccountOption *option;

    option = purple_account_option_string_new(_("Server"), "server",
                                              NP_IM_SERVER);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,
											   option);
    
	option = purple_account_option_int_new(_("Port"), "port", NP_IM_PORT);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,
											   option);
    
	option = purple_account_option_bool_new(_("Use HTTP Method"),
                                            "http_method", FALSE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,
											   option);
    
	option = purple_account_option_string_new(_("HTTP Method Server"),
                                              "http_method_server", NP_HTTPCONN_SERVER);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,
											   option);
    
    option = purple_account_option_bool_new(_("Allow direct connections"),
                                            "direct_connect", TRUE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,
											   option);
    
	option = purple_account_option_bool_new(_("Allow connecting from multiple locations"),
                                            "mpop", TRUE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,
											   option);
    
    purple_prefs_remove("/plugins/prpl/np");
    
//	purple_signal_connect(purple_get_core(), "uri-handler", plugin,
//                          PURPLE_CALLBACK(np_uri_handler), NULL);
}

PURPLE_INIT_PLUGIN(np, init_plugin, info);
