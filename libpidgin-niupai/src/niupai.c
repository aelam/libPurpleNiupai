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


static void
np_login(PurpleAccount *account)
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

static gboolean
niupai_load (PurplePlugin * plugin)
{
	purple_notify_message (plugin, PURPLE_NOTIFY_MSG_INFO, "Hello World!",
                           "This is the Hello World! plugin :)", NULL, NULL,
                           NULL);
    
//	helloworld_plugin = plugin; /* assign this here so we have a valid handle later */
    
	return TRUE;
}

static gboolean
niupai_unload(PurplePlugin * plugin)
{
    
    return TRUE;
}


//static PurplePluginProtocolInfo prpl_info =
//{
//	0, //OPT_PROTO_MAIL_CHECK,
//	NULL,						/* user_splits */
//	NULL,						/* protocol_options */
//	{ "bmp,jpg", 0, 0, 500, 500, 0, PURPLE_ICON_SCALE_SEND},    /* icon_spec */
//	niupai_list_icon,			/* list_icon */
//	NULL,						/* list_emblems */
//	niupai_status_text,			/* status_text */
//	niupai_tooltip_text,		/* tooltip_text */
//	niupai_status_types,		/* away_states */
//	niupai_blist_node_menu,		/* blist_node_menu */
//	NULL,						/* chat_info */
//	NULL,						/* chat_info_defaults */
//	niupai_login,				/* login */
//	niupai_close,				/* close */
//	niupai_send_im,				/* send_im */
//	NULL,						/* set_info */
//	NULL,                       /* send_typing */
//	niupai_get_info,			/* get_info */
//	niupai_set_status,			/* set_away */
//	niupai_set_idle,			/* set_idle */
//	NULL,						/* change_passwd */
//	niupai_add_buddy,			/* add_buddy */
//	NULL,						/* add_buddies */
//	niupai_rem_buddy,			/* remove_buddy */
//	NULL,						/* remove_buddies */
//	niupai_add_permit,			/* add_permit */
//	niupai_add_deny,			/* add_deny */
//	niupai_rem_permit,			/* rem_permit */
//	niupai_rem_deny,			/* rem_deny */
//	niupai_set_permit_deny,		/* set_permit_deny */
//	NULL,						/* join_chat */
//	NULL,						/* reject chat invite */
//	NULL,						/* get_chat_name */
//	niupai_chat_invite,			/* chat_invite */
//	niupai_chat_leave,			/* chat_leave */
//	NULL,						/* chat_whisper */
//	niupai_chat_send,			/* chat_send */
//	NULL, //niupai_keepalive,	/* keepalive */
//	NULL,						/* register_user */
//	NULL,						/* get_cb_info */
//	NULL,						/* get_cb_away */
//	NULL,						/* alias_buddy */
//	niupai_group_buddy,			/* group_buddy */
//	niupai_rename_group,		/* rename_group */
//	NULL,						/* buddy_free */
//	niupai_convo_closed,		/* convo_closed */
//	niupai_normalize,			/* normalize */
//	NULL, //niupai_set_buddy_icon,	/* set_buddy_icon */
//	niupai_remove_group,		/* remove_group */
//	NULL,						/* get_cb_real_name */
//	NULL,						/* set_chat_topic */
//	NULL,						/* find_blist_chat */
//	NULL,						/* roomlist_get_list */
//	NULL,						/* roomlist_cancel */
//	NULL,						/* roomlist_expand_category */
//	niupai_can_receive_file,	/* can_receive_file */
//	niupai_send_file,			/* send_file */
//	NULL, //niupai_new_xfer,	/* new_xfer */
//	NULL,						/* offline_message */
//	NULL,						/* whiteboard_prpl_ops */
//	NULL,						/* send_raw */
//	NULL,						/* roomlist_room_serialize */
//    NULL,						/* unregister_user */
//    NULL,						/* send_attention */
//    NULL,						/* get_attention_types */
//    sizeof(PurplePluginProtocolInfo), /* struct_size */
//    NULL,						/* get_account_text_table */
//    NULL,						/* initiate_media */
//    NULL,						/* get_media_caps */
//    NULL,						/* get_moods */
//    NULL,						/* set_public_alias */
//    NULL,						/* get_public_alias */
//    
//	/* padding */
//    /*
//     NULL,
//     NULL,
//     NULL,
//     NULL
//     */
//};


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
	NULL,//niupai_actions,
    
	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin)
{
    
}

PURPLE_INIT_PLUGIN(np, init_plugin, info);
