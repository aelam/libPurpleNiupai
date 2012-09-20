/**
 * @file switchboard.c NP switchboard functions
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
#include "debug.h"

//#include "nputils.h"
#include "switchboard.h"
//#include "sbconn.h"
//#include "slplink.h"
#include "user.h"
#include "userlist.h"

static NPTable *cbs_table;

/**************************************************************************
 * Main
 **************************************************************************/

NPSwitchBoard *
np_switchboard_new(NPSession *session)
{
	NPSwitchBoard *swboard;

	g_return_val_if_fail(session != NULL, NULL);

	swboard = g_new0(NPSwitchBoard, 1);

	swboard->session = session;
	swboard->servconn = np_servconn_new(session, NP_SERVCONN_SB);
	np_servconn_set_idle_timeout(swboard->servconn, 60);
	swboard->cmdproc = swboard->servconn->cmdproc;

	swboard->msg_queue = g_queue_new();
	swboard->empty = TRUE;

	swboard->cmdproc->data = swboard;
	swboard->cmdproc->cbs_table = cbs_table;

    //TODO
//	session->switches = g_list_prepend(session->switches, swboard);

	if (purple_debug_is_verbose())
		purple_debug_info("np", "switchboard new: swboard(%p)\n", swboard);

	return swboard;
}

void
np_switchboard_destroy(NPSwitchBoard *swboard)
{
	NPSession *session;
	NPMessage *msg;
	GList *l;

	if (purple_debug_is_verbose())
		purple_debug_info("np", "switchboard destroy: swboard(%p)\n", swboard);

	g_return_if_fail(swboard != NULL);

	if (swboard->destroying)
		return;

	swboard->destroying = TRUE;

	if (swboard->reconn_timeout_h > 0)
		purple_timeout_remove(swboard->reconn_timeout_h);

	/* If it linked us is because its looking for trouble */
//	while (swboard->slplinks != NULL) {
////		NPSlpLink *slplink = swboard->slplinks->data;
//
//		swboard->slplinks = g_list_remove(swboard->slplinks, slplink);
//
//		/* Destroy only those slplinks which use the switchboard */
//		if (slplink->dc == NULL)
//			np_slplink_unref(slplink);
//		else {
//			swboard->slplinks = g_list_remove(swboard->slplinks, slplink);
//			slplink->swboard = NULL;
//		}
//	}

	/* Destroy the message queue */
	while ((msg = g_queue_pop_head(swboard->msg_queue)) != NULL)
	{
		if (swboard->error != NP_SB_ERROR_NONE)
		{
			/* The messages could not be sent due to a switchboard error */
			msg_error_helper(swboard->cmdproc, msg,
							 NP_MSG_ERROR_SB);
		}
		np_message_unref(msg);
	}

	g_queue_free(swboard->msg_queue);

	/* msg_error_helper will both remove the msg from ack_list and
	   unref it, so we don't need to do either here */
	while ((l = swboard->ack_list) != NULL)
		msg_error_helper(swboard->cmdproc, l->data, NP_MSG_ERROR_SB);

	g_free(swboard->im_user);
	g_free(swboard->auth_key);
	g_free(swboard->session_id);

	for (; swboard->users; swboard->users = g_list_delete_link(swboard->users, swboard->users))
		np_user_unref(swboard->users->data);
//
//	session = swboard->session;
//	session->switches = g_list_remove(session->switches, swboard);
//
//	for (l = session->slplinks; l; l = l->next) {
//		NPSlpLink *slplink = l->data;
//		if (slplink->swboard == swboard) slplink->swboard = NULL;
//	}

#if 0
	/* This should never happen or we are in trouble. */
	if (swboard->servconn != NULL)
		np_servconn_destroy(swboard->servconn);
#endif

	swboard->cmdproc->data = NULL;

	np_servconn_set_disconnect_cb(swboard->servconn, NULL);

	np_servconn_destroy(swboard->servconn);

	g_free(swboard);
}

void
np_switchboard_set_auth_key(NPSwitchBoard *swboard, const char *key)
{
	g_return_if_fail(swboard != NULL);
	g_return_if_fail(key != NULL);

	swboard->auth_key = g_strdup(key);
}

const char *
np_switchboard_get_auth_key(NPSwitchBoard *swboard)
{
	g_return_val_if_fail(swboard != NULL, NULL);

	return swboard->auth_key;
}

void
np_switchboard_set_session_id(NPSwitchBoard *swboard, const char *id)
{
	g_return_if_fail(swboard != NULL);
	g_return_if_fail(id != NULL);

	g_free(swboard->session_id);
	swboard->session_id = g_strdup(id);
}

const char *
np_switchboard_get_session_id(NPSwitchBoard *swboard)
{
	g_return_val_if_fail(swboard != NULL, NULL);

	return swboard->session_id;
}

int
np_switchboard_get_chat_id(void)
{
	static int chat_id = 1;

	return chat_id++;
}

void
np_switchboard_set_invited(NPSwitchBoard *swboard, gboolean invited)
{
	g_return_if_fail(swboard != NULL);

	swboard->invited = invited;
}

gboolean
np_switchboard_is_invited(NPSwitchBoard *swboard)
{
	g_return_val_if_fail(swboard != NULL, FALSE);

	return swboard->invited;
}

/**************************************************************************
 * Utility
 **************************************************************************/

static void
send_clientcaps(NPSwitchBoard *swboard)
{
	NPMessage *msg;

	msg = np_message_new(NP_MSG_CAPS);
	np_message_set_content_type(msg, "text/x-clientcaps");
	np_message_set_flag(msg, 'U');
//	np_message_set_bin_data(msg, NP_CLIENTINFO, strlen(NP_CLIENTINFO));

	np_switchboard_send_msg(swboard, msg, TRUE);

	np_message_unref(msg);
}

static void
np_switchboard_add_user(NPSwitchBoard *swboard, const char *user)
{
	NPCmdProc *cmdproc;
	PurpleAccount *account;
	NPUserList *userlist;
	NPUser *npuser;
	char *semicolon;
	char *passport;

	g_return_if_fail(swboard != NULL);

	cmdproc = swboard->cmdproc;
	account = cmdproc->session->account;

	semicolon = strchr(user, ';');
	/* We don't really care about the machine ID. */
	if (semicolon)
		passport = g_strndup(user, semicolon - user);
	else
		passport = g_strdup(user);

	userlist = swboard->session->userlist;
	npuser = np_userlist_find_user(userlist, passport);
//
//	/* Don't add multiple endpoints to the conversation. */
//	if (g_list_find_custom(swboard->users, passport, (GCompareFunc)np_user_passport_cmp)) {
//		g_free(passport);
//		return;
//	}

	/* Don't add ourselves either... */
	if (g_str_equal(passport, purple_account_get_username(account))) {
		g_free(passport);
		return;
	}

	if (!npuser) {
		purple_debug_info("np","User %s is not on our list.\n", passport);
		npuser = np_user_new(userlist, passport, NULL);
	} else
		np_user_ref(npuser);

	g_free(passport);

	swboard->users = g_list_prepend(swboard->users, npuser);
	swboard->current_users++;
	swboard->empty = FALSE;

	if (purple_debug_is_verbose())
		purple_debug_info("np", "user=[%s], total=%d\n",
		                  user, swboard->current_users);

	if (!(swboard->flag & NP_SB_FLAG_IM) && (swboard->conv != NULL))
	{
		/* This is a helper switchboard. */
		purple_debug_error("np", "switchboard_add_user: conv != NULL\n");
		return;
	}

	if ((swboard->conv != NULL) &&
		(purple_conversation_get_type(swboard->conv) == PURPLE_CONV_TYPE_CHAT))
	{
		purple_conv_chat_add_user(PURPLE_CONV_CHAT(swboard->conv), npuser->passport, NULL,
								PURPLE_CBFLAGS_NONE, TRUE);
		np_servconn_set_idle_timeout(swboard->servconn, 0);
	}
	else if (swboard->current_users > 1)
	{
		np_servconn_set_idle_timeout(swboard->servconn, 0);
		if (swboard->conv == NULL ||
			purple_conversation_get_type(swboard->conv) != PURPLE_CONV_TYPE_CHAT)
		{
			GList *l;

#if 0
			/* this is bad - it causes np_switchboard_close to be called on the
			 * switchboard we're in the middle of using :( */
			if (swboard->conv != NULL)
				purple_conversation_destroy(swboard->conv);
#endif

			swboard->chat_id = np_switchboard_get_chat_id();
			swboard->flag |= NP_SB_FLAG_IM;
			swboard->conv = serv_got_joined_chat(account->gc,
												 swboard->chat_id,
												 "NP Chat");

			for (l = swboard->users; l != NULL; l = l->next)
			{
				const char *tmp_user;

				tmp_user = ((NPUser*)l->data)->passport;

				purple_conv_chat_add_user(PURPLE_CONV_CHAT(swboard->conv),
										tmp_user, NULL, PURPLE_CBFLAGS_NONE, TRUE);
			}

			purple_conv_chat_add_user(PURPLE_CONV_CHAT(swboard->conv),
									purple_account_get_username(account),
									NULL, PURPLE_CBFLAGS_NONE, TRUE);

			g_free(swboard->im_user);
			swboard->im_user = NULL;
		}
	}
	else if (swboard->conv == NULL)
	{
		swboard->conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM,
															npuser->passport, account);
	}
	else
	{
		purple_debug_warning("np", "switchboard_add_user: This should not happen!\n");
	}
}

static PurpleConversation *
np_switchboard_get_conv(NPSwitchBoard *swboard)
{
	PurpleAccount *account;

	g_return_val_if_fail(swboard != NULL, NULL);

	if (swboard->conv != NULL)
		return swboard->conv;

	purple_debug_error("np", "Switchboard with unassigned conversation\n");

	account = swboard->session->account;

	return (swboard->conv = purple_conversation_new(PURPLE_CONV_TYPE_IM,
												  account, swboard->im_user));
}

static void
np_switchboard_report_user(NPSwitchBoard *swboard, PurpleMessageFlags flags, const char *msg)
{
	PurpleConversation *conv;

	g_return_if_fail(swboard != NULL);
	g_return_if_fail(msg != NULL);

	if ((conv = np_switchboard_get_conv(swboard)) != NULL)
	{
		purple_conversation_write(conv, NULL, msg, flags, time(NULL));
	}
}

static void
swboard_error_helper(NPSwitchBoard *swboard, int reason, const char *passport)
{
	g_return_if_fail(swboard != NULL);

	purple_debug_warning("np", "Error: Unable to call the user %s for reason %i\n",
					   passport ? passport : "(null)", reason);

	/* TODO: if current_users > 0, this is probably a chat and an invite failed,
	 * we should report that in the chat or something */
	if (swboard->current_users == 0)
	{
		swboard->error = reason;
		np_switchboard_close(swboard);
	}
}

static void
cal_error_helper(NPTransaction *trans, int reason)
{
	NPSwitchBoard *swboard;
	const char *passport;
	char **params;

	params = g_strsplit(trans->params, " ", 0);

	passport = params[0];

	swboard = trans->data;

	purple_debug_warning("np", "cal_error_helper: command %s failed for reason %i\n",trans->command,reason);

	swboard_error_helper(swboard, reason, passport);

	g_strfreev(params);
}

static gboolean
msg_resend_cb(gpointer data)
{
	NPSwitchBoard *swboard = data;

	purple_debug_info("np", "unqueuing unsent message to %s\n", swboard->im_user);

	if (np_switchboard_request(swboard)) {
		np_switchboard_request_add_user(swboard, swboard->im_user);
		swboard->reconn_timeout_h = 0;
	}
	return FALSE;
}

void
msg_error_helper(NPCmdProc *cmdproc, NPMessage *msg, NPMsgErrorType error)
{
	NPSwitchBoard *swboard;

	g_return_if_fail(cmdproc != NULL);
	g_return_if_fail(msg     != NULL);

	if ((error != NP_MSG_ERROR_SB) && (msg->nak_cb != NULL))
		msg->nak_cb(msg, msg->ack_data);

	swboard = cmdproc->data;

	/* This is not good, and should be fixed somewhere else. */
	g_return_if_fail(swboard != NULL);

	if (msg->type == NP_MSG_TEXT)
	{
		const char *format, *str_reason;
		char *body_str, *body_enc, *pre, *post;

#if 0
		if (swboard->conv == NULL)
		{
			if (msg->ack_ref)
				np_message_unref(msg);

			return;
		}
#endif

		if (error == NP_MSG_ERROR_TIMEOUT)
		{
			str_reason = _("Message may have not been sent "
						   "because a timeout occurred:");
		}
		else if (error == NP_MSG_ERROR_SB)
		{
			NPSession *session = swboard->session;

			if (!session->destroying && msg->retries &&	swboard->im_user &&
				(swboard->error == NP_SB_ERROR_CONNECTION ||
					swboard->error == NP_SB_ERROR_UNKNOWN)) {
				NPSwitchBoard *new_sw = np_session_find_swboard(session,
					swboard->im_user);

				if (new_sw == NULL || new_sw->reconn_timeout_h == 0) {
					new_sw = np_switchboard_new(session);
					new_sw->im_user = g_strdup(swboard->im_user);
					new_sw->reconn_timeout_h = purple_timeout_add_seconds(3, msg_resend_cb, new_sw);
					new_sw->flag |= NP_SB_FLAG_IM;
				}

				body_str = np_message_to_string(msg);
				body_enc = g_markup_escape_text(body_str, -1);
				g_free(body_str);

				purple_debug_info("np", "queuing unsent message to %s: %s\n",
					swboard->im_user, body_enc);
				g_free(body_enc);
				np_send_im_message(session, msg);
				msg->retries--;

				return;
			}

			switch (swboard->error)
			{
				case NP_SB_ERROR_OFFLINE:
					str_reason = _("Message could not be sent, "
								   "not allowed while invisible:");
					break;
				case NP_SB_ERROR_USER_OFFLINE:
					str_reason = _("Message could not be sent "
								   "because the user is offline:");
					break;
				case NP_SB_ERROR_CONNECTION:
					str_reason = _("Message could not be sent "
								   "because a connection error occurred:");
					break;
				case NP_SB_ERROR_TOO_FAST:
					str_reason = _("Message could not be sent "
								   "because we are sending too quickly:");
					break;
				case NP_SB_ERROR_AUTHFAILED:
					str_reason = _("Message could not be sent "
								   "because we were unable to establish a "
								   "session with the server. This is "
								   "likely a server problem, try again in "
								   "a few minutes:");
					break;
				default:
					str_reason = _("Message could not be sent "
								   "because an error with "
								   "the switchboard occurred:");
					break;
			}
		}
		else
		{
			str_reason = _("Message may have not been sent "
						   "because an unknown error occurred:");
		}

		body_str = np_message_to_string(msg);
		body_enc = g_markup_escape_text(body_str, -1);
		g_free(body_str);

		format = np_message_get_header_value(msg, "X-MMS-IM-Format");
		np_parse_format(format, &pre, &post);
		body_str = g_strdup_printf("%s%s%s", pre ? pre : "",
								   body_enc ? body_enc : "", post ? post : "");
		g_free(body_enc);
		g_free(pre);
		g_free(post);

		np_switchboard_report_user(swboard, PURPLE_MESSAGE_ERROR,
									str_reason);
		np_switchboard_report_user(swboard, PURPLE_MESSAGE_RAW,
									body_str);

		g_free(body_str);
	}

	/* If a timeout occures we will want the msg around just in case we
	 * receive the ACK after the timeout. */
	if (msg->ack_ref && error != NP_MSG_ERROR_TIMEOUT)
	{
		swboard->ack_list = g_list_remove(swboard->ack_list, msg);
		np_message_unref(msg);
	}
}

/**************************************************************************
 * Message Stuff
 **************************************************************************/

/** Called when we receive an error of a message. */
static void
msg_error(NPCmdProc *cmdproc, NPTransaction *trans, int error)
{
	msg_error_helper(cmdproc, trans->data, NP_MSG_ERROR_UNKNOWN);
}

gboolean
np_switchboard_can_send(NPSwitchBoard *swboard)
{
	g_return_val_if_fail(swboard != NULL, FALSE);

	if (swboard->empty || !g_queue_is_empty(swboard->msg_queue))
		return FALSE;

	return TRUE;
}

/**************************************************************************
 * Switchboard Commands
 **************************************************************************/

static void
ans_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPSwitchBoard *swboard;

	swboard = cmdproc->data;
	swboard->ready = TRUE;
}

static void
bye_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPSwitchBoard *swboard;
	const char *user;

	swboard = cmdproc->data;
	user = cmd->params[0];

	/* cmdproc->data is set to NULL when the switchboard is destroyed;
	 * we may get a bye shortly thereafter. */
	g_return_if_fail(swboard != NULL);

	if (!(swboard->flag & NP_SB_FLAG_IM) && (swboard->conv != NULL))
		purple_debug_error("np", "bye_cmd: helper bug\n");

	if (swboard->conv == NULL)
	{
		/* This is a helper switchboard */
		np_switchboard_destroy(swboard);
	}
	else if ((swboard->current_users > 1) ||
			 (purple_conversation_get_type(swboard->conv) == PURPLE_CONV_TYPE_CHAT))
	{
		GList *passport;
		/* This is a switchboard used for a chat */
		purple_conv_chat_remove_user(PURPLE_CONV_CHAT(swboard->conv), user, NULL);

		passport = g_list_find_custom(swboard->users, user, (GCompareFunc)strcmp);
		if (passport)
			g_free(passport->data);
		else
			purple_debug_warning("np", "Can't find user %s in the switchboard\n", user);
		swboard->users = g_list_delete_link(swboard->users, passport);
		swboard->current_users--;
		if (swboard->current_users == 0)
			np_switchboard_destroy(swboard);
	}
	else
	{
		/* This is a switchboard used for a im session */
		np_switchboard_destroy(swboard);
	}
}

static void
iro_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPSwitchBoard *swboard;

	swboard = cmdproc->data;

	swboard->total_users = atoi(cmd->params[2]);

	np_switchboard_add_user(swboard, cmd->params[3]);
}

static void
joi_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPSession *session;
	NPSwitchBoard *swboard;
	const char *passport;

	passport = cmd->params[0];

	session = cmdproc->session;
	swboard = cmdproc->data;

	np_switchboard_add_user(swboard, passport);

	np_sbconn_process_queue(swboard);

	if (!session->http_method)
		send_clientcaps(swboard);

	if (swboard->closed)
		np_switchboard_close(swboard);
}

static void
msg_cmd_post(NPCmdProc *cmdproc, NPCommand *cmd, char *payload, size_t len)
{
	NPMessage *msg;

	msg = np_message_new_from_cmd(cmdproc->session, cmd);

	np_message_parse_payload(msg, payload, len,
					MSG_LINE_DEM,MSG_BODY_DEM);
	if (purple_debug_is_verbose())
		np_message_show_readable(msg, "SB RECV", FALSE);

	g_free (msg->remote_user);
	msg->remote_user = g_strdup(cmd->params[0]);

	np_cmdproc_process_msg(cmdproc, msg);

	np_message_unref(msg);
}

static void
msg_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	cmd->payload_len = atoi(cmd->params[2]);
	cmdproc->last_cmd->payload_cb = msg_cmd_post;
}

static void
ubm_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	purple_debug_misc("np", "get UBM...\n");
	cmd->payload_len = atoi(cmd->params[5]);
	cmdproc->last_cmd->payload_cb = msg_cmd_post;
}

static void
nak_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPMessage *msg;

	msg = cmd->trans->data;
	g_return_if_fail(msg != NULL);

	msg_error_helper(cmdproc, msg, NP_MSG_ERROR_NAK);
	cmd->trans->data = NULL;
}

static void
ack_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPSwitchBoard *swboard;
	NPMessage *msg;

	msg = cmd->trans->data;

//	if (msg->part && msg->part->ack_cb != NULL)
//		msg->part->ack_cb(msg->part, msg->part->ack_data);

	swboard = cmdproc->data;
	if (swboard)
		swboard->ack_list = g_list_remove(swboard->ack_list, msg);
	np_message_unref(msg);
	cmd->trans->data = NULL;
}

static void
out_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	PurpleConnection *gc;
	NPSwitchBoard *swboard;

	gc = cmdproc->session->account->gc;
	swboard = cmdproc->data;

	if (swboard->current_users > 1)
		serv_got_chat_left(gc, swboard->chat_id);

	np_switchboard_disconnect(swboard);
}

static void
usr_cmd(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPSwitchBoard *swboard;

	swboard = cmdproc->data;

#if 0
	GList *l;

	for (l = swboard->users; l != NULL; l = l->next)
	{
		const char *user;
		user = l->data;

		np_cmdproc_send(cmdproc, "CAL", "%s", user);
	}
#endif

	swboard->ready = TRUE;
	np_cmdproc_process_queue(cmdproc);
}

/**************************************************************************
 * Message Handlers
 **************************************************************************/
static void
clientcaps_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
#if 0
	NPSession *session;
	NPSwitchBoard *swboard;
	NPUser *user;
	GHashTable *clientcaps;
	const char *value;

	char *passport = msg->sender;

	session = cmdproc->session;
	swboard = cmdproc->servconn->swboard;

	clientcaps = np_message_get_hashtable_from_body(msg);
#endif
}

void
np_switchboard_show_ink(NPSwitchBoard *swboard, const char *passport,
                         const char *data)
{
	PurpleConnection *gc;
	guchar *image_data;
	size_t image_len;
	int imgid;
	char *image_msg;

	if (!purple_str_has_prefix(data, "base64:"))
	{
		purple_debug_error("np", "Ignoring Ink not in Base64 format.\n");
		return;
	}

	gc = purple_account_get_connection(swboard->session->account);

	data += sizeof("base64:") - 1;
	image_data = purple_base64_decode(data, &image_len);
	if (!image_data || !image_len)
	{
		purple_debug_error("np", "Unable to decode Ink from Base64 format.\n");
		return;
	}

	imgid = purple_imgstore_add_with_id(image_data, image_len, NULL);
	image_msg = g_strdup_printf("<IMG ID='%d'>", imgid);

	if (swboard->current_users > 1 ||
		((swboard->conv != NULL) &&
		 purple_conversation_get_type(swboard->conv) == PURPLE_CONV_TYPE_CHAT))
		serv_got_chat_in(gc, swboard->chat_id, passport, 0, image_msg,
						 time(NULL));
	else
		serv_got_im(gc, passport, image_msg, 0, time(NULL));

	purple_imgstore_unref_by_id(imgid);
	g_free(image_msg);
}

/**************************************************************************
 * Connect stuff
 **************************************************************************/
static void
ans_usr_error(NPCmdProc *cmdproc, NPTransaction *trans, int error);

static void
connect_cb(NPServConn *servconn)
{
	NPSwitchBoard *swboard;
	NPTransaction *trans;
	NPCmdProc *cmdproc;
	PurpleAccount *account;
	char *username;

	cmdproc = servconn->cmdproc;
	g_return_if_fail(cmdproc != NULL);

	account = cmdproc->session->account;
	swboard = cmdproc->data;
	g_return_if_fail(swboard != NULL);

//	username = g_strdup_printf("%s;{%s}",
//	                           purple_account_get_username(account),
//	                           servconn->session->guid);
//    TODO
    username = "1234";
    
	if (np_switchboard_is_invited(swboard))
	{
		swboard->empty = FALSE;

		trans = np_transaction_new(cmdproc, "ANS", "%s %s %s",
									username,
									swboard->auth_key, swboard->session_id);
	}
	else
	{
		trans = np_transaction_new(cmdproc, "USR", "%s %s",
									username,
									swboard->auth_key);
	}

	np_transaction_set_error_cb(trans, ans_usr_error);
	np_transaction_set_data(trans, swboard);
	np_cmdproc_send_trans(cmdproc, trans);

	g_free(username);
}

static void
ans_usr_error(NPCmdProc *cmdproc, NPTransaction *trans, int error)
{
	NPSwitchBoard *swboard;
	char **params;
	char *passport;
	int reason = NP_SB_ERROR_UNKNOWN;

	if (error == 911)
	{
		reason = NP_SB_ERROR_AUTHFAILED;
	}

	purple_debug_warning("np", "ans_usr_error: command %s gave error %i\n", trans->command, error);

	params = g_strsplit(trans->params, " ", 0);
	passport = params[0];
	swboard = trans->data;

	swboard_error_helper(swboard, reason, passport);

	g_strfreev(params);
}

static void
disconnect_cb(NPServConn *servconn)
{
	NPSwitchBoard *swboard;

	swboard = servconn->cmdproc->data;
	g_return_if_fail(swboard != NULL);

	np_servconn_set_disconnect_cb(swboard->servconn, NULL);

	np_switchboard_destroy(swboard);
}

gboolean
np_switchboard_connect(NPSwitchBoard *swboard, const char *host, int port)
{
	g_return_val_if_fail(swboard != NULL, FALSE);

	np_servconn_set_connect_cb(swboard->servconn, connect_cb);
	np_servconn_set_disconnect_cb(swboard->servconn, disconnect_cb);

	return np_servconn_connect(swboard->servconn, host, port, FALSE);
}

void
np_switchboard_disconnect(NPSwitchBoard *swboard)
{
	g_return_if_fail(swboard != NULL);

	np_servconn_disconnect(swboard->servconn);
}

/**************************************************************************
 * Call stuff
 **************************************************************************/
static void
got_cal(NPCmdProc *cmdproc, NPCommand *cmd)
{
#if 0
	NPSwitchBoard *swboard;
	const char *user;

	swboard = cmdproc->data;

	user = cmd->params[0];

	np_switchboard_add_user(swboard, user);
#endif
}

static void
cal_timeout(NPCmdProc *cmdproc, NPTransaction *trans)
{
	purple_debug_warning("np", "cal_timeout: command %s timed out\n", trans->command);

	cal_error_helper(trans, NP_SB_ERROR_UNKNOWN);
}

static void
cal_error(NPCmdProc *cmdproc, NPTransaction *trans, int error)
{
	int reason = NP_SB_ERROR_UNKNOWN;
	NPMessage *msg;
	NPSwitchBoard *swboard = trans->data;

	if (error == 215)
	{
		purple_debug_info("np", "Invited user already in switchboard\n");
		return;
	}
	else if (error == 217)
	{
		reason = NP_SB_ERROR_USER_OFFLINE;
	}

	purple_debug_warning("np", "cal_error: command %s gave error %i\n", trans->command, error);

	while ((msg = g_queue_pop_head(swboard->msg_queue)) != NULL){
		purple_debug_warning("np", "Unable to send msg: {%s}\n", msg->body);
		/* The messages could not be sent due to a switchboard error */
		swboard->error = NP_SB_ERROR_USER_OFFLINE;
		msg_error_helper(swboard->cmdproc, msg,
							 NP_MSG_ERROR_SB);
	}
	cal_error_helper(trans, reason);
}

void
np_switchboard_request_add_user(NPSwitchBoard *swboard, const char *user)
{
	NPTransaction *trans;
	NPCmdProc *cmdproc;

	g_return_if_fail(swboard != NULL);

	cmdproc = swboard->cmdproc;

	trans = np_transaction_new(cmdproc, "CAL", "%s", user);
	/* this doesn't do anything, but users seem to think that
	 * 'Unhandled command' is some kind of error, so we don't report it */
	np_transaction_add_cb(trans, "CAL", got_cal);

	np_transaction_set_data(trans, swboard);
	np_transaction_set_timeout_cb(trans, cal_timeout);

	if (swboard->ready)
		np_cmdproc_send_trans(cmdproc, trans);
	else
		np_cmdproc_queue_trans(cmdproc, trans);
}

/**************************************************************************
 * Create & Transfer stuff
 **************************************************************************/

static void
got_swboard(NPCmdProc *cmdproc, NPCommand *cmd)
{
	NPSwitchBoard *swboard;
	char *host;
	int port;
	swboard = cmd->trans->data;

//	if (g_list_find(cmdproc->session->switches, swboard) == NULL)
		/* The conversation window was closed. */
//		return;

	purple_debug_info("np", "Switchboard:auth:{%s} socket:{%s}\n", cmd->params[4], cmd->params[2]);
	np_switchboard_set_auth_key(swboard, cmd->params[4]);

	np_parse_socket(cmd->params[2], &host, &port);

	if (!np_switchboard_connect(swboard, host, port))
		np_switchboard_destroy(swboard);

	g_free(host);
}

static void
xfr_error(NPCmdProc *cmdproc, NPTransaction *trans, int error)
{
	NPSwitchBoard *swboard;
	int reason = NP_SB_ERROR_UNKNOWN;

	if (error == 913)
		reason = NP_SB_ERROR_OFFLINE;
	else if (error == 800)
		reason = NP_SB_ERROR_TOO_FAST;

	swboard = trans->data;

	purple_debug_info("np",
		"xfr_error %i for %s: trans %p, command %s, reason %i\n",
		error, (swboard->im_user ? swboard->im_user : "(null)"), trans,
		(trans->command ? trans->command : "(null)"), reason);

	swboard_error_helper(swboard, reason, swboard->im_user);
}

gboolean
np_switchboard_request(NPSwitchBoard *swboard)
{
	NPCmdProc *cmdproc;
	NPTransaction *trans;

	g_return_val_if_fail(swboard != NULL, FALSE);

	cmdproc = swboard->session->notification->cmdproc;

	trans = np_transaction_new(cmdproc, "XFR", "%s", "SB");
	np_transaction_add_cb(trans, "XFR", got_swboard);

	np_transaction_set_data(trans, swboard);
	np_transaction_set_error_cb(trans, xfr_error);

	return np_cmdproc_send_trans(cmdproc, trans);
}

void
np_switchboard_close(NPSwitchBoard *swboard)
{
	g_return_if_fail(swboard != NULL);

	if (swboard->error != NP_SB_ERROR_NONE)
	{
		np_switchboard_destroy(swboard);
	}
	else if (g_queue_is_empty(swboard->msg_queue) ||
			 !swboard->session->connected)
	{
		NPCmdProc *cmdproc;
		NPTransaction *trans;
		cmdproc = swboard->cmdproc;
		trans = np_transaction_new(cmdproc, "OUT", NULL);
		np_transaction_set_saveable(trans, FALSE);
		np_cmdproc_send_trans(cmdproc, trans);

		np_switchboard_destroy(swboard);
	}
	else
	{
		swboard->closed = TRUE;
	}
}

void
np_switchboard_release(NPSwitchBoard *swboard, NPSBFlag flag)
{
	g_return_if_fail(swboard != NULL);

	swboard->flag &= ~flag;

	if (flag == NP_SB_FLAG_IM)
		/* Forget any conversation that used to be associated with this
		 * swboard. */
		swboard->conv = NULL;

	if (swboard->flag == 0)
		/* Nothing else is using this switchboard, so close it */
		np_switchboard_close(swboard);
}

/**************************************************************************
 * Init stuff
 **************************************************************************/

void
np_switchboard_init(void)
{
	cbs_table = np_table_new();

	np_table_add_cmd(cbs_table, "ANS", "ANS", ans_cmd);
	np_table_add_cmd(cbs_table, "ANS", "IRO", iro_cmd);

	np_table_add_cmd(cbs_table, "MSG", "ACK", ack_cmd);
	np_table_add_cmd(cbs_table, "MSG", "NAK", nak_cmd);

	np_table_add_cmd(cbs_table, "USR", "USR", usr_cmd);

	np_table_add_cmd(cbs_table, NULL, "MSG", msg_cmd);
	np_table_add_cmd(cbs_table, NULL, "UBM", ubm_cmd);
	np_table_add_cmd(cbs_table, NULL, "JOI", joi_cmd);
	np_table_add_cmd(cbs_table, NULL, "BYE", bye_cmd);
	np_table_add_cmd(cbs_table, NULL, "OUT", out_cmd);

#if 0
	/* They might skip the history */
	np_table_add_cmd(cbs_table, NULL, "ACK", NULL);
#endif

	np_table_add_error(cbs_table, "MSG", msg_error);
	np_table_add_error(cbs_table, "CAL", cal_error);

	/* Register the message type callbacks. */
	np_table_add_msg_type(cbs_table, "text/plain",
						   np_plain_msg);
	np_table_add_msg_type(cbs_table, "text/x-msmsgscontrol",
						   np_control_msg);
	np_table_add_msg_type(cbs_table, "text/x-clientcaps",
						   clientcaps_msg);
	np_table_add_msg_type(cbs_table, "text/x-clientinfo",
						   clientcaps_msg);
	np_table_add_msg_type(cbs_table, "application/x-npmsgrp2p",
						   np_p2p_msg);
	np_table_add_msg_type(cbs_table, "text/x-mms-emoticon",
						   np_emoticon_msg);
	np_table_add_msg_type(cbs_table, "text/x-mms-animemoticon",
	                       np_emoticon_msg);
	np_table_add_msg_type(cbs_table, "text/x-npmsgr-datacast",
						   np_datacast_msg);
	np_table_add_msg_type(cbs_table, "text/x-msmsgsinvite",
						   np_invite_msg);
	np_table_add_msg_type(cbs_table, "image/gif",
						   np_handwritten_msg);
}

void
np_switchboard_end(void)
{
	np_table_destroy(cbs_table);
}
