//
//  niupai.c
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-13.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#include "niupai.h"
#include <util.h>
#include <prpl.h>
#include <log.h>
#include <version.h>
#include "NIDebuggingTools.h"

//
// Addition
//

static GList *
niupai_buddy_menu(PurpleBuddy *buddy)
{
//	niupaiUser *user;
//    
	GList *m = NULL;
	PurpleMenuAction *act;
//
//	g_return_val_if_fail(buddy != NULL, NULL);
//    
//	user = buddy->proto_data;
//    
//	if (user != NULL)
//	{
//		PurpleBlistNode *gnode;
//		GList *m_copy = NULL;
//		// GList *m_move = NULL;
//        
//		//		if (user->mobile)
//		//		{
//		//			act = purple_menu_action_new(_("Send to Mobile"),
//		//			                           PURPLE_CALLBACK(show_send_to_mobile_cb),
//		//			                           NULL, NULL);
//		//			m = g_list_append(m, act);
//		//		}
//		//
//		act = purple_menu_action_new(_("Send memo"), PURPLE_CALLBACK(show_send_memo_cb), NULL, NULL);
//		m = g_list_append(m, act);
//        
//		act = purple_menu_action_new(_("Send SMS message"), PURPLE_CALLBACK(show_send_sms_cb), NULL, NULL);
//		m = g_list_append(m, act);
//        
//		/* Copy buddy */
//		for (gnode = purple_blist_get_root(); gnode; gnode = gnode->next)
//		{
//			PurpleGroup *group = (PurpleGroup *)gnode;
//            
//			if(!PURPLE_BLIST_NODE_IS_GROUP(gnode))
//				continue;
//            
//			act = purple_menu_action_new(group->name, NULL, NULL, NULL);
//			m_copy = g_list_append(m_copy, act);
//		}
//		act = purple_menu_action_new(_("Copy buddy"), NULL, NULL, m_copy);
//		m = g_list_append(m, act);
//	}
//    
//	if (g_ascii_strcasecmp(buddy->name,
//                           purple_account_get_username(buddy->account)))
//	{
//		//act = purple_menu_action_new(_("Initiate _Chat"), NULL, NULL, NULL);
////		act = purple_menu_action_new(_("Initiate _Chat"),
////                                     PURPLE_CALLBACK(initiate_chat_cb),
//                                     NULL, NULL);
//		m = g_list_append(m, act);
//	}
//    
	return m;
}



//
// PurplePluginProtocolInfo
//

static const char* niupai_list_icon(PurpleAccount *a, PurpleBuddy *b)
{
	return "niupai";
}

static char* niupai_status_text(PurpleBuddy *buddy)
{
	PurplePresence *presence;
	PurpleStatus *status;
    
	presence = purple_buddy_get_presence(buddy);
	status = purple_presence_get_active_status(presence);
    
	if (!purple_presence_is_available(presence) && !purple_presence_is_idle(presence))
	{
		return g_strdup(purple_status_get_name(status));
	}
    
	return NULL;
}

static void
niupai_tooltip_text(PurpleBuddy *buddy, PurpleNotifyUserInfo *user_info, gboolean full)
{
//	NiupaiUser *user;
//	PurplePresence *presence = purple_buddy_get_presence(buddy);
//	PurpleStatus *status = purple_presence_get_active_status(presence);
//    
//	user = buddy->proto_data;
//    
//	if (user)
//	{
//		purple_notify_user_info_add_pair(user_info, _("Name"), user->friendly_name);
//	}
//    
//	if (purple_presence_is_online(presence))
//	{
//		purple_notify_user_info_add_pair(user_info, _("Status"), purple_presence_is_idle(presence) ? _("Idle") : purple_status_get_name(status));
//	}
    
	//	if (full && user)
	//	{
	//		purple_notify_user_info_add_pair(user_info, _("Has you"), ((user->list_op & (1 << niupai_LIST_RL)) ? _("Yes") : _("No")));
	//
	//	}
	//
	//	/* XXX: This is being shown in non-full tooltips because the
	//	 * XXX: blocked icon overlay isn't always accurate for niupai.
	//	 * XXX: This can die as soon as purple_privacy_check() knows that
	//	 * XXX: this prpl always honors both the allow and deny lists. */
	//	if (user)
	//	{
	//		purple_notify_user_info_add_pair(user_info, _("Blocked"), ((user->list_op & (1 << niupai_LIST_BL)) ? _("Yes") : _("No")));
	//	}
}

static GList *
niupai_status_types(PurpleAccount *account)
{
	PurpleStatusType *status;
	GList *types = NULL;
    
	status = purple_status_type_new_full(PURPLE_STATUS_AVAILABLE,
                                         "O", NULL, FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	status = purple_status_type_new_full(PURPLE_STATUS_AWAY,
                                         "A", NULL, FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	status = purple_status_type_new_full(PURPLE_STATUS_UNAVAILABLE,
                                         "B", _("Busy"), FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	status = purple_status_type_new_full(PURPLE_STATUS_UNAVAILABLE,
                                         "P", _("On The Phone"), FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	status = purple_status_type_new_full(PURPLE_STATUS_AWAY,
                                         "M", _("In meeting"), FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	status = purple_status_type_new_full(PURPLE_STATUS_INVISIBLE,
                                         "X", NULL, FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	status = purple_status_type_new_full(PURPLE_STATUS_OFFLINE,
                                         NULL, NULL, FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
    
	return types;
}

static GList *niupai_blist_node_menu(PurpleBlistNode *node)
{
	if(PURPLE_BLIST_NODE_IS_BUDDY(node))
	{
		return niupai_buddy_menu((PurpleBuddy *) node);
	}
	else
	{
		return NULL;
	}
}

static void
niupai_login(PurpleAccount *account)
{
    
    NIDINFO("Login\n");

	PurpleConnection *gc;
//	niupaiSession *session;
	const char *username;
	const char *host;
	gboolean prs_method = FALSE;
	int port;
    
    NIDPRINT("Login\n");

	gc = purple_account_get_connection(account);
    
	//	if (!purple_ssl_is_supported())
	//	{
	//		gc->wants_to_die = TRUE;
	//		purple_connection_error(gc,
	//			_("SSL support is needed for niupai. Please install a supported "
	//			  "SSL library. See http://purple.sf.net/faq-ssl.php for more "
	//			  "information."));
	//
	//		return;
	//	}
	//
//	prs_method = purple_account_get_bool(account, "prs_method", FALSE);
//    
//	host = purple_account_get_string(account, "server", niupai_SERVER);
//	port = purple_account_get_int(account, "port", niupai_PORT);
//    
//	session = niupai_session_new(account);
//    
//	gc->proto_data = session;
//	gc->flags |= PURPLE_CONNECTION_HTML | PURPLE_CONNECTION_FORMATTING_WBFO |
//    PURPLE_CONNECTION_NO_BGCOLOR | PURPLE_CONNECTION_NO_FONTSIZE |
//    PURPLE_CONNECTION_NO_URLDESC | PURPLE_CONNECTION_ALLOW_CUSTOM_SMILEY;
//    
//	niupai_session_set_login_step(session, niupai_LOGIN_STEP_START);
//    
//	/* Hmm, I don't like this. */
//	/* XXX shx: Me neither */
//	username = niupai_normalize(account, purple_account_get_username(account));
//    
//	if (strcmp(username, purple_account_get_username(account)))
//		purple_account_set_username(account, username);
//    
//	if (!niupai_session_connect(session, host, port, prs_method))
//		purple_connection_error(gc, _("Failed to connect to server."));
}

static void
niupai_close(PurpleConnection *gc)
{
    NIDPRINT("Close\n");

//	niupaiSession *session;
//    
//	session = gc->proto_data;
//    
//	g_return_if_fail(session != NULL);
//    
//	niupai_session_destroy(session);
//    
//	gc->proto_data = NULL;
}


static int
niupai_send_im(PurpleConnection *gc, const char *who, const char *message,
               PurpleMessageFlags flags)
{
    
    return FALSE;
}


static void
niupai_get_info(PurpleConnection *gc, const char *name)
{

}

static void niupai_set_status(PurpleAccount *account, PurpleStatus *status)
{
	PurpleConnection *gc;

}

static void
niupai_add_buddy(PurpleConnection *gc, PurpleBuddy *buddy, PurpleGroup *group)
{
    
}

static void
niupai_rem_buddy(PurpleConnection *gc, PurpleBuddy *buddy, PurpleGroup *group)
{
    
}

static void
niupai_add_permit(PurpleConnection *gc, const char *who)
{
    
}

static void
niupai_chat_leave(PurpleConnection *gc, int id)
{
    
}


static int niupai_chat_send(PurpleConnection *gc, int id, const char *message, PurpleMessageFlags flags)
{
    
    return 1;
}


//static unsigned int
//niupai_send_typing(PurpleConnection *gc, const char *who, PurpleTypingState state)
//{
//    return FALSE;
//}

//static void niupai_set_status(PurpleAccount *account, PurpleStatus *status)
//{
//	PurpleConnection *gc;
////	niupaiSession *session;
//    
////	purple_debug_info("niupai" ,"[%s]\n", __FUNCTION__);
////    
////	gc = purple_account_get_connection(account);
////    
////	if (gc != NULL)
////	{
////		purple_debug_info("niupai" ,"[%s] gc존재\n", __FUNCTION__);
////		session = gc->proto_data;
////		niupai_change_status(session);
////	}
//}

static gboolean
niupai_can_receive_file(PurpleConnection *gc, const char *who)
{
    return FALSE;
}


static void
niupai_send_file(PurpleConnection *gc, const char *who, const char *file)
{
    
}

/* This function is the callback for the plugin action we added. All we're
 * doing here is displaying a message. When the user selects the plugin
 * action, this function is called. */
//static void
//plugin_action_test_cb (PurplePluginAction * action)
//{
//	purple_notify_message (helloworld_plugin, PURPLE_NOTIFY_MSG_INFO,
//                           "Plugin Actions Test", "This is a plugin actions test :)", NULL, NULL,
//                           NULL);
//}

/* we tell libpurple in the PurplePluginInfo struct to call this function to
 * get a list of plugin actions to use for the plugin.  This function gives
 * libpurple that list of actions. */
//static GList *
//plugin_actions (PurplePlugin * plugin, gpointer context)
//{
//	/* some C89 (a.k.a. ANSI C) compilers will warn if any variable declaration
//	 * includes an initilization that calls a function.  To avoid that, we
//	 * generally initialize our variables first with constant values like NULL
//	 * or 0 and assign to them with function calls later */
//	GList *list = NULL;
//	PurplePluginAction *action = NULL;
//    
//	/* The action gets created by specifying a name to show in the UI and a
//	 * callback function to call. */
//	action = purple_plugin_action_new ("Plugin Action Test", plugin_action_test_cb);
//    
//	/* libpurple requires a GList of plugin actions, even if there is only one
//	 * action in the list.  We append the action to a GList here. */
//	list = g_list_append (list, action);
//    
//	/* Once the list is complete, we send it to libpurple. */
//	return list;
//}
//

/**************************************************************************
 * Protocol Plugin ops
 **************************************************************************/

static gboolean
niupai_load (PurplePlugin * plugin)
{
    NIDINFO("Loaded\n");
//	purple_notify_message (plugin, PURPLE_NOTIFY_MSG_INFO, "Hello World!",
//                           "This is the Hello World! plugin :)", NULL, NULL,
//                           NULL);
//    NIDINFO("Loaded\n");
    
	return TRUE;
}

static gboolean
niupai_unload(PurplePlugin * plugin)
{
    
    NIDINFO("Unloaded\n");
    return TRUE;
}


static GList *
niupai_actions(PurplePlugin *plugin, gpointer context)
{
	//	PurpleConnection *gc = (PurpleConnection *)context;
	//	PurpleAccount *account;
	//	const char *user;
    
	GList *m = NULL;
	PurplePluginAction *act;
    
//	act = purple_plugin_action_new(_("Set Friendly Name..."), niupai_show_set_friendly_name);
//	m = g_list_append(m, act);
//	m = g_list_append(m, NULL);
//    
//	act = purple_plugin_action_new(_("View Buddies By..."), niupai_show_view_buddies_by);
//	m = g_list_append(m, act);
//	m = g_list_append(m, NULL);
//    
//	act = purple_plugin_action_new(_("Send SMS message..."), niupai_show_send_sms);
//	m = g_list_append(m, act);
    
	//	act = purple_plugin_action_new(_("Set Home Phone Number..."),
	//								 niupai_show_set_home_phone);
	//	m = g_list_append(m, act);
	//
	//	act = purple_plugin_action_new(_("Set Work Phone Number..."),
	//			niupai_show_set_work_phone);
	//	m = g_list_append(m, act);
	//
	//	act = purple_plugin_action_new(_("Set Mobile Phone Number..."),
	//			niupai_show_set_mobile_phone);
	//	m = g_list_append(m, act);
	//	m = g_list_append(m, NULL);
	//
	//#if 0
	//	act = purple_plugin_action_new(_("Enable/Disable Mobile Devices..."),
	//			niupai_show_set_mobile_support);
	//	m = g_list_append(m, act);
	//#endif
	//
	//	act = purple_plugin_action_new(_("Allow/Disallow Mobile Pages..."),
	//			niupai_show_set_mobile_pages);
	//	m = g_list_append(m, act);
	//
	//	account = purple_connection_get_account(gc);
	//	user = niupai_normalize(account, purple_account_get_username(account));
	//
	//	if (strstr(user, "@hotmail.com") != NULL)
	//	{
	//		m = g_list_append(m, NULL);
	//		act = purple_plugin_action_new(_("Open Hotmail Inbox"),
	//				niupai_show_hotmail_inbox);
	//		m = g_list_append(m, act);
	//	}
    
	return m;
}


static PurplePluginProtocolInfo prpl_info =
{
	0, //OPT_PROTO_MAIL_CHECK,
	NULL,						/* user_splits */
	NULL,						/* protocol_options */
	{ "bmp,jpg", 0, 0, 500, 500, 0, PURPLE_ICON_SCALE_SEND},    /* icon_spec */
	niupai_list_icon,			/* list_icon */
	NULL,						/* list_emblems */
	niupai_status_text,			/* status_text */
	niupai_tooltip_text,		/* tooltip_text */
	niupai_status_types,		/* away_states */
	niupai_blist_node_menu,		/* blist_node_menu */
	NULL,						/* chat_info */
	NULL,						/* chat_info_defaults */
	niupai_login,				/* login */
	niupai_close,				/* close */
	niupai_send_im,				/* send_im */
	NULL,						/* set_info */
	NULL,                       /* send_typing */
	niupai_get_info,			/* get_info */
	niupai_set_status,			/* set_away */
	NULL,                       /* set_idle */
	NULL,						/* change_passwd */
	niupai_add_buddy,			/* add_buddy */
	NULL,						/* add_buddies */
	niupai_rem_buddy,			/* remove_buddy */
	NULL,						/* remove_buddies */
	niupai_add_permit,			/* add_permit */
	NULL,                       /* add_deny */
	NULL,                       /* rem_permit */
	NULL,                       /* rem_deny */
	NULL,                       /* set_permit_deny */
	NULL,						/* join_chat */
	NULL,						/* reject chat invite */
	NULL,						/* get_chat_name */
	NULL,                       /* chat_invite */
	niupai_chat_leave,			/* chat_leave */
	NULL,						/* chat_whisper */
	niupai_chat_send,			/* chat_send */
	NULL, //niupai_keepalive,	/* keepalive */
	NULL,						/* register_user */
	NULL,						/* get_cb_info */
	NULL,						/* get_cb_away */
	NULL,						/* alias_buddy */
	NULL,//niupai_group_buddy,			/* group_buddy */
	NULL,                       /* rename_group */
	NULL,						/* buddy_free */
	NULL,//niupai_convo_closed,		/* convo_closed */
	NULL,                       /* normalize */
	NULL, //niupai_set_buddy_icon,	/* set_buddy_icon */
	NULL,		/* remove_group */
	NULL,						/* get_cb_real_name */
	NULL,						/* set_chat_topic */
	NULL,						/* find_blist_chat */
	NULL,						/* roomlist_get_list */
	NULL,						/* roomlist_cancel */
	NULL,						/* roomlist_expand_category */
	niupai_can_receive_file,	/* can_receive_file */
	niupai_send_file,			/* send_file */
	NULL, //niupai_new_xfer,	/* new_xfer */
	NULL,						/* offline_message */
	NULL,						/* whiteboard_prpl_ops */
	NULL,						/* send_raw */
	NULL,						/* roomlist_room_serialize */
    NULL,						/* unregister_user */
    NULL,						/* send_attention */
    NULL,						/* get_attention_types */
    sizeof(PurplePluginProtocolInfo), /* struct_size */
    NULL,						/* get_account_text_table */
    NULL,						/* initiate_media */
    NULL,						/* get_media_caps */
    NULL,						/* get_moods */
    NULL,						/* set_public_alias */
    NULL,						/* get_public_alias */
    
	/* padding */
    /*
     NULL,
     NULL,
     NULL,
     NULL
     */
};


/* For specific notes on the meanings of each of these members, consult the C Plugin Howto
 * on the website. */
static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,
    
	"prpl-np",
	"Niupai",
	LIBNIUPAI_VERSION,
	N_("niupai Protocol Plugin"),
	N_("niupai Protocol Plugin"),
	"Ryan Wang <wanglun02@gmail.com>",	/**< author         */
	"http://www.niupai.com",
    
	niupai_load,				/**< load           */
	niupai_unload,				/**< unload         */
	NULL,					/**< destroy        */
    
	NULL,					/**< ui_info        */
	NULL,//&prpl_info, 				/**< extra_info     */
	NULL, 					/**< prefs_info     */
	niupai_actions,
    
	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin)
{
    
    freopen("/Users/ryan/Desktop/purple.log", "a+",stdout);
//    fprintf(stdout, "=============== Log Start ================ \n");
    NIMaxLogLevel = NILOGLEVEL_INFO;
    NIDINFO("\n=============== Log Start ================ \n");
}

PURPLE_INIT_PLUGIN(np, init_plugin, info);
