/**
 * @file msg.c Message functions
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

#include "niupai.h"
#include "msg.h"
#include "switchboard.h"
//#include "nputils.h"
//#include "slpmsg.h"
//#include "slpmsg_part.h"

NPMessage *
np_message_new(NPMsgType type)
{
	NPMessage *msg;

	msg = g_new0(NPMessage, 1);
	msg->type = type;

	if (purple_debug_is_verbose())
		purple_debug_info("np", "message new (%p)(%d)\n", msg, type);

	msg->header_table = g_hash_table_new_full(g_str_hash, g_str_equal,
											g_free, g_free);

	np_message_ref(msg);

	return msg;
}

/**
 * Destroys a message.
 *
 * @param msg The message to destroy.
 */
static void
np_message_destroy(NPMessage *msg)
{
	g_return_if_fail(msg != NULL);

	if (purple_debug_is_verbose())
		purple_debug_info("np", "message destroy (%p)\n", msg);

	g_free(msg->remote_user);
	g_free(msg->body);
	g_free(msg->content_type);
	g_free(msg->charset);

	g_hash_table_destroy(msg->header_table);
	g_list_free(msg->header_list);
//	if (msg->part)
//		np_slpmsgpart_unref(msg->part);

	g_free(msg);
}

NPMessage *
np_message_ref(NPMessage *msg)
{
	g_return_val_if_fail(msg != NULL, NULL);

	msg->ref_count++;

	if (purple_debug_is_verbose())
		purple_debug_info("np", "message ref (%p)[%u]\n", msg, msg->ref_count);

	return msg;
}

void
np_message_unref(NPMessage *msg)
{
	g_return_if_fail(msg != NULL);
	g_return_if_fail(msg->ref_count > 0);

	msg->ref_count--;

	if (purple_debug_is_verbose())
		purple_debug_info("np", "message unref (%p)[%u]\n", msg, msg->ref_count);

	if (msg->ref_count == 0)
		np_message_destroy(msg);
}

NPMessage *
np_message_new_plain(const char *message)
{
	NPMessage *msg;
	char *message_cr;

	msg = np_message_new(NP_MSG_TEXT);
	msg->retries = 1;
//	np_message_set_header(msg, "User-Agent", PACKAGE_NAME "/" VERSION);
	np_message_set_content_type(msg, "text/plain");
	np_message_set_charset(msg, "UTF-8");
	np_message_set_flag(msg, 'A');
	np_message_set_header(msg, "X-MMS-IM-Format",
						 "FN=Segoe%20UI; EF=; CO=0; CS=1;PF=0");

	message_cr = purple_str_add_cr(message);
	np_message_set_bin_data(msg, message_cr, strlen(message_cr));
	g_free(message_cr);

	return msg;
}

NPMessage *
np_message_new_npslp(void)
{
	NPMessage *msg;

	msg = np_message_new(NP_MSG_SLP);

	np_message_set_header(msg, "User-Agent", NULL);

	np_message_set_flag(msg, 'D');
	np_message_set_content_type(msg, "application/x-npmsgrp2p");

	return msg;
}

NPMessage *
np_message_new_nudge(void)
{
	NPMessage *msg;

	msg = np_message_new(NP_MSG_NUDGE);
	np_message_set_content_type(msg, "text/x-npmsgr-datacast");
	np_message_set_flag(msg, 'N');
	np_message_set_bin_data(msg, "ID: 1\r\n", 7);

	return msg;
}

void
np_message_parse_payload(NPMessage *msg,
						  const char *payload, size_t payload_len,
						  const char *line_dem,const char *body_dem)
{
	char *tmp_base, *tmp;
	const char *content_type;
	char *end;
	char **elems, **cur, **tokens;

	g_return_if_fail(payload != NULL);
	tmp_base = tmp = g_malloc(payload_len + 1);
	memcpy(tmp_base, payload, payload_len);
	tmp_base[payload_len] = '\0';

	/* Find the end of the headers */
	end = strstr(tmp, body_dem);
	/* TODO? some clients use \r delimiters instead of \r\n, the official client
	 * doesn't send such messages, but does handle receiving them. We'll just
	 * avoid crashing for now */
	if (end == NULL) {
		g_free(tmp_base);
		g_return_if_reached();
	}
	*end = '\0';

	/* Split the headers and parse each one */
	elems = g_strsplit(tmp, line_dem, 0);
	for (cur = elems; *cur != NULL; cur++)
	{
		const char *key, *value;

		/* If this line starts with whitespace, it's been folded from the
		   previous line and won't have ':'. */
		if ((**cur == ' ') || (**cur == '\t')) {
			tokens = g_strsplit(g_strchug(*cur), "=\"", 2);
			key = tokens[0];
			value = tokens[1];

			/* The only one I care about is 'boundary' (which is folded from
			   the key 'Content-Type'), so only process that. */
			if (!strcmp(key, "boundary")) {
				char *end = strchr(value, '\"');
				*end = '\0';
				np_message_set_header(msg, key, value);
			}

			g_strfreev(tokens);
			continue;
		}

		tokens = g_strsplit(*cur, ": ", 2);

		key = tokens[0];
		value = tokens[1];

		/*if not MIME content ,then return*/
		if (!strcmp(key, "MIME-Version"))
		{
			g_strfreev(tokens);
			continue;
		}

		if (!strcmp(key, "Content-Type"))
		{
			char *charset, *c;

			if ((c = strchr(value, ';')) != NULL)
			{
				if ((charset = strchr(c, '=')) != NULL)
				{
					charset++;
					np_message_set_charset(msg, charset);
				}

				*c = '\0';
			}

			np_message_set_content_type(msg, value);
		}
		else
		{
			np_message_set_header(msg, key, value);
		}

		g_strfreev(tokens);
	}
	g_strfreev(elems);

	/* Proceed to the end of the "\r\n\r\n" */
	tmp = end + strlen(body_dem);

	/* Now we *should* be at the body. */
	content_type = np_message_get_content_type(msg);

	if (payload_len - (tmp - tmp_base) > 0) {
		msg->body_len = payload_len - (tmp - tmp_base);
		g_free(msg->body);
		msg->body = g_malloc(msg->body_len + 1);
		memcpy(msg->body, tmp, msg->body_len);
		msg->body[msg->body_len] = '\0';
	}

	if (msg->body && content_type && purple_str_has_prefix(content_type, "text/")) {
		char *body = NULL;

		if (msg->charset == NULL || g_str_equal(msg->charset, "UTF-8")) {
			/* Charset is UTF-8 */
			if (!g_utf8_validate(msg->body, msg->body_len, NULL)) {
				purple_debug_warning("np", "Message contains invalid "
						"UTF-8. Attempting to salvage.\n");
				body = purple_utf8_salvage(msg->body);
				payload_len = strlen(body);
			}
		} else {
			/* Charset is something other than UTF-8 */
			GError *err = NULL;
			body = g_convert(msg->body, msg->body_len, "UTF-8",
					msg->charset, NULL, &payload_len, &err);
			if (!body || err) {
				purple_debug_warning("np", "Unable to convert message from "
						"%s to UTF-8: %s\n", msg->charset,
						err ? err->message : "Unknown error");
				if (err)
					g_error_free(err);

				/* Fallback to ISO-8859-1 */
				g_free(body);
				body = g_convert(msg->body, msg->body_len, "UTF-8",
						"ISO-8859-1", NULL, &payload_len, NULL);
				if (!body) {
					g_free(msg->body);
					msg->body = NULL;
					msg->body_len = 0;
				}
			}
		}

		if (body) {
			g_free(msg->body);
			msg->body = body;
			msg->body_len = payload_len;
			np_message_set_charset(msg, "UTF-8");
		}
	}

	g_free(tmp_base);
}

NPMessage *
np_message_new_from_cmd(NPSession *session, NPCommand *cmd)
{
	NPMessage *msg;

	g_return_val_if_fail(cmd != NULL, NULL);

	msg = np_message_new(NP_MSG_UNKNOWN);

	msg->remote_user = g_strdup(cmd->params[0]);
	/* msg->size = atoi(cmd->params[2]); */
	msg->cmd = cmd;

	return msg;
}

char *
np_message_gen_payload(NPMessage *msg, size_t *ret_size)
{
	GList *l;
	char *n, *base, *end;
	int len;
	size_t body_len = 0;
	const void *body;

	g_return_val_if_fail(msg != NULL, NULL);

	len = NP_BUF_LEN;

	base = n = end = g_malloc(len + 1);
	end += len;

	/* Standard header. */
	if (msg->charset == NULL)
	{
		g_snprintf(n, len,
				   "MIME-Version: 1.0\r\n"
				   "Content-Type: %s\r\n",
				   msg->content_type);
	}
	else
	{
		g_snprintf(n, len,
				   "MIME-Version: 1.0\r\n"
				   "Content-Type: %s; charset=%s\r\n",
				   msg->content_type, msg->charset);
	}

	n += strlen(n);

	for (l = msg->header_list; l != NULL; l = l->next)
	{
		const char *key;
		const char *value;

		key = l->data;
		value = np_message_get_header_value(msg, key);

		g_snprintf(n, end - n, "%s: %s\r\n", key, value);
		n += strlen(n);
	}

	n += g_strlcpy(n, "\r\n", end - n);

	body = np_message_get_bin_data(msg, &body_len);

	if (body != NULL)
	{
		memcpy(n, body, body_len);
		n += body_len;
		*n = '\0';
	}

	if (ret_size != NULL)
	{
		*ret_size = n - base;

		if (*ret_size > 1664)
			*ret_size = 1664;
	}

	return base;
}

void
np_message_set_flag(NPMessage *msg, char flag)
{
	g_return_if_fail(msg != NULL);
	g_return_if_fail(flag != 0);

	msg->flag = flag;
}

char
np_message_get_flag(const NPMessage *msg)
{
	g_return_val_if_fail(msg != NULL, 0);

	return msg->flag;
}

void
np_message_set_bin_data(NPMessage *msg, const void *data, size_t len)
{
	g_return_if_fail(msg != NULL);

	/* There is no need to waste memory on data we cannot send anyway */
	if (len > 1664)
		len = 1664;

	if (msg->body != NULL)
		g_free(msg->body);

	if (data != NULL && len > 0)
	{
		msg->body = g_malloc(len + 1);
		memcpy(msg->body, data, len);
		msg->body[len] = '\0';
		msg->body_len = len;
	}
	else
	{
		msg->body = NULL;
		msg->body_len = 0;
	}
}

const void *
np_message_get_bin_data(const NPMessage *msg, size_t *len)
{
	g_return_val_if_fail(msg != NULL, NULL);

	if (len)
		*len = msg->body_len;

	return msg->body;
}

void
np_message_set_content_type(NPMessage *msg, const char *type)
{
	g_return_if_fail(msg != NULL);

	g_free(msg->content_type);
	msg->content_type = g_strdup(type);
}

const char *
np_message_get_content_type(const NPMessage *msg)
{
	g_return_val_if_fail(msg != NULL, NULL);

	return msg->content_type;
}

void
np_message_set_charset(NPMessage *msg, const char *charset)
{
	g_return_if_fail(msg != NULL);

	g_free(msg->charset);
	msg->charset = g_strdup(charset);
}

const char *
np_message_get_charset(const NPMessage *msg)
{
	g_return_val_if_fail(msg != NULL, NULL);

	return msg->charset;
}

void
np_message_set_header(NPMessage *msg, const char *name, const char *value)
{
	const char *temp;
	char *new_name;

	g_return_if_fail(msg != NULL);
	g_return_if_fail(name != NULL);

	temp = np_message_get_header_value(msg, name);

	if (value == NULL)
	{
		if (temp != NULL)
		{
			GList *l;

			for (l = msg->header_list; l != NULL; l = l->next)
			{
				if (!g_ascii_strcasecmp(l->data, name))
				{
					msg->header_list = g_list_remove(msg->header_list, l->data);

					break;
				}
			}

			g_hash_table_remove(msg->header_table, name);
		}

		return;
	}

	new_name = g_strdup(name);

	g_hash_table_insert(msg->header_table, new_name, g_strdup(value));

	if (temp == NULL)
		msg->header_list = g_list_append(msg->header_list, new_name);
}

const char *
np_message_get_header_value(const NPMessage *msg, const char *name)
{
	g_return_val_if_fail(msg != NULL, NULL);
	g_return_val_if_fail(name != NULL, NULL);

	return g_hash_table_lookup(msg->header_table, name);
}

GHashTable *
np_message_get_hashtable_from_body(const NPMessage *msg)
{
	GHashTable *table;
	size_t body_len;
	const char *body;
	char **elems, **cur, **tokens, *body_str;

	g_return_val_if_fail(msg != NULL, NULL);

	table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	body = np_message_get_bin_data(msg, &body_len);

	g_return_val_if_fail(body != NULL, NULL);

	body_str = g_strndup(body, body_len);
	elems = g_strsplit(body_str, "\r\n", 0);
	g_free(body_str);

	for (cur = elems; *cur != NULL; cur++)
	{
		if (**cur == '\0')
			break;

		tokens = g_strsplit(*cur, ": ", 2);

		if (tokens[0] != NULL && tokens[1] != NULL) {
			g_hash_table_insert(table, tokens[0], tokens[1]);
			g_free(tokens);
		} else
			g_strfreev(tokens);
	}

	g_strfreev(elems);

	return table;
}

char *
np_message_to_string(NPMessage *msg)
{
	size_t body_len;
	const char *body;

	g_return_val_if_fail(msg != NULL, NULL);
	g_return_val_if_fail(msg->type == NP_MSG_TEXT, NULL);

	body = np_message_get_bin_data(msg, &body_len);

	return g_strndup(body, body_len);
}

void
np_message_show_readable(NPMessage *msg, const char *info,
						  gboolean text_body)
{
	GString *str;
	size_t body_len;
	const char *body;
	GList *l;

	g_return_if_fail(msg != NULL);

	str = g_string_new(NULL);

	/* Standard header. */
	if (msg->charset == NULL)
	{
		g_string_append_printf(str,
				   "MIME-Version: 1.0\r\n"
				   "Content-Type: %s\r\n",
				   msg->content_type);
	}
	else
	{
		g_string_append_printf(str,
				   "MIME-Version: 1.0\r\n"
				   "Content-Type: %s; charset=%s\r\n",
				   msg->content_type, msg->charset);
	}

	for (l = msg->header_list; l; l = l->next)
	{
		char *key;
		const char *value;

		key = l->data;
		value = np_message_get_header_value(msg, key);

		g_string_append_printf(str, "%s: %s\r\n", key, value);
	}

	g_string_append(str, "\r\n");

	body = np_message_get_bin_data(msg, &body_len);

	if (body != NULL)
	{
		if (msg->type == NP_MSG_TEXT)
		{
			g_string_append_len(str, body, body_len);
			g_string_append(str, "\r\n");
		}
		else
		{
			size_t i;
			for (i = 0; i < body_len; i++, body++)
			{
				g_string_append_printf(str, "%02x ", (unsigned char)*body);
				if (i % 16 == 0 && i != 0)
					g_string_append_c(str, '\n');
			}
			g_string_append_c(str, '\n');
		}
	}

	purple_debug_info("np", "Message %s:\n{%s}\n", info, str->str);

	g_string_free(str, TRUE);
}

/**************************************************************************
 * Message Handlers
 **************************************************************************/
void
np_plain_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
	PurpleConnection *gc;
	const char *body;
	char *body_enc;
	char *body_final;
	size_t body_len;
	const char *passport;
	const char *value;

	gc = cmdproc->session->account->gc;

	body = np_message_get_bin_data(msg, &body_len);
	body_enc = g_markup_escape_text(body, body_len);

	passport = msg->remote_user;

	if (!strcmp(passport, "messenger@microsoft.com") &&
		strstr(body, "immediate security update"))
	{
		return;
	}

#if 0
	if ((value = np_message_get_header_value(msg, "User-Agent")) != NULL)
	{
		purple_debug_misc("np", "User-Agent = '%s'\n", value);
	}
#endif

	if ((value = np_message_get_header_value(msg, "X-MMS-IM-Format")) != NULL)
	{
		char *pre, *post;

		np_parse_format(value, &pre, &post);

		body_final = g_strdup_printf("%s%s%s", pre ? pre : "",
									 body_enc ? body_enc : "", post ? post : "");

		g_free(pre);
		g_free(post);
		g_free(body_enc);
	}
	else
	{
		body_final = body_enc;
	}

	if (cmdproc->servconn->type == NP_SERVCONN_SB) {
//		NPSwitchBoard *swboard = cmdproc->data;
//
//		swboard->flag |= NP_SB_FLAG_IM;
//
//		if (swboard->current_users > 1 ||
//			((swboard->conv != NULL) &&
//			 purple_conversation_get_type(swboard->conv) == PURPLE_CONV_TYPE_CHAT))
//		{
//			/* If current_users is always ok as it should then there is no need to
//			 * check if this is a chat. */
//			if (swboard->current_users <= 1)
//				purple_debug_misc("np", "plain_msg: current_users(%d)\n",
//								swboard->current_users);
//
//			serv_got_chat_in(gc, swboard->chat_id, passport, 0, body_final,
//							 time(NULL));
//			if (swboard->conv == NULL)
//			{
//				swboard->conv = purple_find_chat(gc, swboard->chat_id);
//				swboard->flag |= NP_SB_FLAG_IM;
//			}
//		}
//		else if (!g_str_equal(passport, purple_account_get_username(gc->account)))
//		{
//			/* Don't im ourselves ... */
//			serv_got_im(gc, passport, body_final, 0, time(NULL));
//			if (swboard->conv == NULL)
//			{
//				swboard->conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM,
//										passport, purple_connection_get_account(gc));
//				swboard->flag |= NP_SB_FLAG_IM;
//			}
//		}

	} else {
		serv_got_im(gc, passport, body_final, 0, time(NULL));
	}

	g_free(body_final);
}

void
np_control_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
//	PurpleConnection *gc;
//	char *passport;
//
//	gc = cmdproc->session->account->gc;
//	passport = msg->remote_user;
//
//	if (np_message_get_header_value(msg, "TypingUser") == NULL)
//		return;
//
//	if (cmdproc->servconn->type == NP_SERVCONN_SB) {
//		NPSwitchBoard *swboard = cmdproc->data;
//
//		if (swboard->current_users == 1)
//		{
//			serv_got_typing(gc, passport, NP_TYPING_RECV_TIMEOUT,
//							PURPLE_TYPING);
//		}
//
//	} else {
//		serv_got_typing(gc, passport, NP_TYPING_RECV_TIMEOUT,
//						PURPLE_TYPING);
//	}
}

static void
datacast_inform_user(NPSwitchBoard *swboard, const char *who,
                     const char *msg, const char *filename)
{
	char *username, *str;
	PurpleAccount *account;
	PurpleBuddy *b;
	PurpleConnection *pc;
	gboolean chat;

	account = swboard->session->account;
	pc = purple_account_get_connection(account);

	if ((b = purple_find_buddy(account, who)) != NULL)
		username = g_markup_escape_text(purple_buddy_get_alias(b), -1);
	else
		username = g_markup_escape_text(who, -1);
	str = g_strdup_printf(msg, username, filename);
	g_free(username);

	swboard->flag |= NP_SB_FLAG_IM;
	if (swboard->current_users > 1)
		chat = TRUE;
	else
		chat = FALSE;

	if (swboard->conv == NULL) {
		if (chat)
			swboard->conv = purple_find_chat(account->gc, swboard->chat_id);
		else {
			swboard->conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM,
									who, account);
			if (swboard->conv == NULL)
				swboard->conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, account, who);
		}
	}

	if (chat)
		serv_got_chat_in(pc,
		                 purple_conv_chat_get_id(PURPLE_CONV_CHAT(swboard->conv)),
		                 who, PURPLE_MESSAGE_RECV|PURPLE_MESSAGE_SYSTEM, str,
		                 time(NULL));
	else
		serv_got_im(pc, who, str, PURPLE_MESSAGE_RECV|PURPLE_MESSAGE_SYSTEM,
		            time(NULL));
	g_free(str);

}

/* TODO: Make these not be such duplicates of each other */
//static void
//got_wink_cb(NPSlpCall *slpcall, const guchar *data, gsize size)
//{
//	FILE *f = NULL;
//	char *path = NULL;
//	const char *who = slpcall->slplink->remote_user;
//	purple_debug_info("np", "Received wink from %s\n", who);
//
//	if ((f = purple_mkstemp(&path, TRUE)) &&
//	    (fwrite(data, 1, size, f) == size)) {
//		datacast_inform_user(slpcall->slplink->swboard,
//		                     who,
//		                     _("%s sent a wink. <a href='np-wink://%s'>Click here to play it</a>"),
//		                     path);
//	} else {
//		purple_debug_error("np", "Couldn\'t create temp file to store wink\n");
//		datacast_inform_user(slpcall->slplink->swboard,
//		                     who,
//		                     _("%s sent a wink, but it could not be saved"),
//		                     NULL);
//	}
//	if (f)
//		fclose(f);
//	g_free(path);
//}
//
//static void
//got_voiceclip_cb(NPSlpCall *slpcall, const guchar *data, gsize size)
//{
//	FILE *f = NULL;
//	char *path = NULL;
//	const char *who = slpcall->slplink->remote_user;
//	purple_debug_info("np", "Received voice clip from %s\n", who);
//
//	if ((f = purple_mkstemp(&path, TRUE)) &&
//	    (fwrite(data, 1, size, f) == size)) {
//		datacast_inform_user(slpcall->slplink->swboard,
//		                     who,
//		                     _("%s sent a voice clip. <a href='audio://%s'>Click here to play it</a>"),
//		                     path);
//	} else {
//		purple_debug_error("np", "Couldn\'t create temp file to store sound\n");
//		datacast_inform_user(slpcall->slplink->swboard,
//		                     who,
//		                     _("%s sent a voice clip, but it could not be saved"),
//		                     NULL);
//	}
//	if (f)
//		fclose(f);
//	g_free(path);
//}

void
np_p2p_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
//	NPSession *session;
//	NPSlpLink *slplink;
//	NPP2PVersion p2p;
//
//	session = cmdproc->servconn->session;
//	slplink = np_session_get_slplink(session, msg->remote_user);
//
//	if (slplink->swboard == NULL)
//	{
//		/*
//		 * We will need swboard in order to change its flags.  If its
//		 * NULL, something has probably gone wrong earlier on.  I
//		 * didn't want to do this, but NP 7 is somehow causing us
//		 * to crash here, I couldn't reproduce it to debug more,
//		 * and people are reporting bugs. Hopefully this doesn't
//		 * cause more crashes. Stu.
//		 */
//		if (cmdproc->data == NULL)
//			g_warning("np_p2p_msg cmdproc->data was NULL\n");
//		else {
//			slplink->swboard = (NPSwitchBoard *)cmdproc->data;
//			slplink->swboard->slplinks = g_list_prepend(slplink->swboard->slplinks, slplink);
//		}
//	}
//
//	p2p = np_slplink_get_p2p_version(slplink);
//	msg->part = np_slpmsgpart_new_from_data(p2p, msg->body, msg->body_len);
//
//	if (msg->part)
//		np_slplink_process_msg(slplink, msg->part);
//	else
//		purple_debug_warning("np", "P2P message failed to parse.\n");
}

//static void
//got_emoticon(NPSlpCall *slpcall,
//			 const guchar *data, gsize size)
//{
//	PurpleConversation *conv;
//	NPSwitchBoard *swboard;
//
//	swboard = slpcall->slplink->swboard;
//	conv = swboard->conv;
//
//	if (conv) {
//		/* FIXME: it would be better if we wrote the data as we received it
//		   instead of all at once, calling write multiple times and
//		   close once at the very end
//		 */
//		purple_conv_custom_smiley_write(conv, slpcall->data_info, data, size);
//		purple_conv_custom_smiley_close(conv, slpcall->data_info );
//	}
//	if (purple_debug_is_verbose())
//		purple_debug_info("np", "Got smiley: %s\n", slpcall->data_info);
//}
//
void np_emoticon_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
//	NPSession *session;
//	NPSlpLink *slplink;
//	NPSwitchBoard *swboard;
//	NPObject *obj;
//	char **tokens;
//	char *smile, *body_str;
//	const char *body, *who, *sha1;
//	guint tok;
//	size_t body_len;
//
//	PurpleConversation *conv;
//
//	session = cmdproc->servconn->session;
//
//	if (!purple_account_get_bool(session->account, "custom_smileys", TRUE))
//		return;
//
//	swboard = cmdproc->data;
//	conv = swboard->conv;
//
//	body = np_message_get_bin_data(msg, &body_len);
//	if (!body || !body_len)
//		return;
//	body_str = g_strndup(body, body_len);
//
//	/* NP Messenger 7 may send more than one NPObject in a single message...
//	 * Maybe 10 tokens is a reasonable max value. */
//	tokens = g_strsplit(body_str, "\t", 10);
//
//	g_free(body_str);
//
//	for (tok = 0; tok < 9; tok += 2) {
//		if (tokens[tok] == NULL || tokens[tok + 1] == NULL) {
//			break;
//		}
//
//		smile = tokens[tok];
//		obj = np_object_new_from_string(purple_url_decode(tokens[tok + 1]));
//
//		if (obj == NULL)
//			break;
//
//		who = np_object_get_creator(obj);
//		sha1 = np_object_get_sha1(obj);
//
//		slplink = np_session_get_slplink(session, who);
//		if (slplink->swboard != swboard) {
//			if (slplink->swboard != NULL)
//				/*
//				 * Apparently we're using a different switchboard now or
//				 * something?  I don't know if this is normal, but it
//				 * definitely happens.  So make sure the old switchboard
//				 * doesn't still have a reference to us.
//				 */
//				slplink->swboard->slplinks = g_list_remove(slplink->swboard->slplinks, slplink);
//			slplink->swboard = swboard;
//			slplink->swboard->slplinks = g_list_prepend(slplink->swboard->slplinks, slplink);
//		}
//
//		/* If the conversation doesn't exist then this is a custom smiley
//		 * used in the first message in a NP conversation: we need to create
//		 * the conversation now, otherwise the custom smiley won't be shown.
//		 * This happens because every GtkIMHtml has its own smiley tree: if
//		 * the conversation doesn't exist then we cannot associate the new
//		 * smiley with its GtkIMHtml widget. */
//		if (!conv) {
//			conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, session->account, who);
//		}
//
//		if (purple_conv_custom_smiley_add(conv, smile, "sha1", sha1, TRUE)) {
//			np_slplink_request_object(slplink, smile, got_emoticon, NULL, obj);
//		}
//
//		np_object_destroy(obj);
//		obj =   NULL;
//		who =   NULL;
//		sha1 = NULL;
//	}
//	g_strfreev(tokens);
}

void
np_datacast_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
//	GHashTable *body;
//	const char *id;
//	body = np_message_get_hashtable_from_body(msg);
//
//	id = g_hash_table_lookup(body, "ID");
//
//	if (!strcmp(id, "1")) {
//		/* Nudge */
//		PurpleAccount *account;
//		const char *user;
//
//		account = cmdproc->session->account;
//		user = msg->remote_user;
//
//		if (cmdproc->servconn->type == NP_SERVCONN_SB) {
//			NPSwitchBoard *swboard = cmdproc->data;
//			if (swboard->current_users > 1 ||
//				((swboard->conv != NULL) &&
//				 purple_conversation_get_type(swboard->conv) == PURPLE_CONV_TYPE_CHAT))
//				purple_prpl_got_attention_in_chat(account->gc, swboard->chat_id, user, NP_NUDGE);
//
//			else
//				purple_prpl_got_attention(account->gc, user, NP_NUDGE);
//		} else {
//			purple_prpl_got_attention(account->gc, user, NP_NUDGE);
//		}
//
//	} else if (!strcmp(id, "2")) {
//		/* Wink */
//		NPSession *session;
//		NPSlpLink *slplink;
//		NPObject *obj;
//		const char *who;
//		const char *data;
//
//		session = cmdproc->session;
//
//		data = g_hash_table_lookup(body, "Data");
//		obj = np_object_new_from_string(data);
//		who = np_object_get_creator(obj);
//
//		slplink = np_session_get_slplink(session, who);
//		np_slplink_request_object(slplink, data, got_wink_cb, NULL, obj);
//
//		np_object_destroy(obj);
//
//
//	} else if (!strcmp(id, "3")) {
//		/* Voiceclip */
//		NPSession *session;
//		NPSlpLink *slplink;
//		NPObject *obj;
//		const char *who;
//		const char *data;
//
//		session = cmdproc->session;
//
//		data = g_hash_table_lookup(body, "Data");
//		obj = np_object_new_from_string(data);
//		who = np_object_get_creator(obj);
//
//		slplink = np_session_get_slplink(session, who);
//		np_slplink_request_object(slplink, data, got_voiceclip_cb, NULL, obj);
//
//		np_object_destroy(obj);
//
//	} else if (!strcmp(id, "4")) {
//		/* Action */
//
//	} else {
//		purple_debug_warning("np", "Got unknown datacast with ID %s.\n", id);
//	}
//
//	g_hash_table_destroy(body);
}

void
np_invite_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
//	GHashTable *body;
//	const gchar *command;
//	const gchar *cookie;
//	gboolean accepted = FALSE;
//
//	g_return_if_fail(cmdproc != NULL);
//	g_return_if_fail(msg != NULL);
//
//	body = np_message_get_hashtable_from_body(msg);
//
//	if (body == NULL) {
//		purple_debug_warning("np",
//				"Unable to parse invite msg body.\n");
//		return;
//	}
//
//	/*
//	 * GUID is NOT always present but Invitation-Command and Invitation-Cookie
//	 * are mandatory.
//	 */
//	command = g_hash_table_lookup(body, "Invitation-Command");
//	cookie = g_hash_table_lookup(body, "Invitation-Cookie");
//
//	if (command == NULL || cookie == NULL) {
//		purple_debug_warning("np",
//			"Invalid invitation message: either Invitation-Command "
//			"or Invitation-Cookie is missing or invalid.\n"
//		);
//		return;
//
//	} else if (!strcmp(command, "INVITE")) {
//		const gchar	*guid = g_hash_table_lookup(body, "Application-GUID");
//
//		if (guid == NULL) {
//			purple_debug_warning("np",
//			                     "Invite msg missing Application-GUID.\n");
//
//			accepted = TRUE;
//
//		} else if (!strcmp(guid, NP_FT_GUID)) {
//
//		} else if (!strcmp(guid, "{02D3C01F-BF30-4825-A83A-DE7AF41648AA}")) {
//			purple_debug_info("np", "Computer call\n");
//
//			if (cmdproc->session) {
//				PurpleConversation *conv = NULL;
//				gchar *from = msg->remote_user;
//				gchar *buf = NULL;
//
//				if (from)
//					conv = purple_find_conversation_with_account(
//							PURPLE_CONV_TYPE_IM, from,
//							cmdproc->session->account);
//				if (conv)
//					buf = g_strdup_printf(
//							_("%s sent you a voice chat "
//							"invite, which is not yet "
//							"supported."), from);
//				if (buf) {
//					purple_conversation_write(conv, NULL, buf,
//							PURPLE_MESSAGE_SYSTEM |
//							PURPLE_MESSAGE_NOTIFY,
//							time(NULL));
//					g_free(buf);
//				}
//			}
//		} else {
//			const gchar *application = g_hash_table_lookup(body, "Application-Name");
//			purple_debug_warning("np", "Unhandled invite msg with GUID %s: %s.\n",
//			                     guid, application ? application : "(null)");
//		}
//
//		if (!accepted) {
//			NPSwitchBoard *swboard = cmdproc->data;
//			char *text;
//			NPMessage *cancel;
//
//			cancel = np_message_new(NP_MSG_TEXT);
//			np_message_set_content_type(cancel, "text/x-msmsgsinvite");
//			np_message_set_charset(cancel, "UTF-8");
//			np_message_set_flag(cancel, 'U');
//
//			text = g_strdup_printf("Invitation-Command: CANCEL\r\n"
//			                       "Invitation-Cookie: %s\r\n"
//			                       "Cancel-Code: REJECT_NOT_INSTALLED\r\n",
//			                       cookie);
//			np_message_set_bin_data(cancel, text, strlen(text));
//			g_free(text);
//
//			np_switchboard_send_msg(swboard, cancel, TRUE);
//			np_message_unref(cancel);
//		}
//
//	} else if (!strcmp(command, "CANCEL")) {
//		const gchar *code = g_hash_table_lookup(body, "Cancel-Code");
//		purple_debug_info("np", "MSMSGS invitation cancelled: %s.\n",
//		                  code ? code : "no reason given");
//
//	} else {
//		/*
//		 * Some other already established invitation session.
//		 * Can be retrieved by Invitation-Cookie.
//		 */
//	}
//
//	g_hash_table_destroy(body);
}

/* Only called from chats. Handwritten messages for IMs come as a SLP message */
void
np_handwritten_msg(NPCmdProc *cmdproc, NPMessage *msg)
{
	const char *body;
	size_t body_len;

	body = np_message_get_bin_data(msg, &body_len);
	np_switchboard_show_ink(cmdproc->data, msg->remote_user, body);
}

