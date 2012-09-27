/**
 * @file transaction.c NP transaction functions
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
#include "transaction.h"

NPTransaction *
np_transaction_new(NPCmdProc *cmdproc, const char *command,
					const char *format, ...)
{
	NPTransaction *trans;
	va_list arg;

	g_return_val_if_fail(command != NULL, NULL);

	trans = g_new0(NPTransaction, 1);

	trans->cmdproc = cmdproc;
	trans->command = g_strdup(command);
	trans->saveable = TRUE;

	if (format != NULL)
	{
		va_start(arg, format);
		trans->params = g_strdup_vprintf(format, arg);
		va_end(arg);
	}
    
	return trans;
}

void
np_transaction_destroy(NPTransaction *trans)
{
	g_return_if_fail(trans != NULL);

	g_free(trans->command);
	g_free(trans->params);
	g_free(trans->payload);

	if (trans->data_free)
		trans->data_free(trans->data);

#if 0
	if (trans->pendent_cmd != NULL)
		np_message_unref(trans->pendent_msg);
#endif

#if 0
	NPTransaction *elem;
	if (trans->queue != NULL)
	{
		while ((elem = g_queue_pop_head(trans->queue)) != NULL)
			np_transaction_destroy(elem);

		g_queue_free(trans->queue);
	}
#endif

	if (trans->callbacks != NULL && trans->has_custom_callbacks)
		g_hash_table_destroy(trans->callbacks);

	if (trans->timer)
		purple_timeout_remove(trans->timer);

	g_free(trans);
}

char *
np_transaction_to_string(NPTransaction *trans)
{
	char *str;

	g_return_val_if_fail(trans != NULL, FALSE);

#if 1
	if (trans->params != NULL)
        str = g_strdup(trans->params);
    else if (trans->saveable)
		str = g_strdup_printf("%s", trans->command);
	else
		str = g_strdup_printf("%s\r\n", trans->command);
#else
	g_return_val_if_fail(trans != NULL, FALSE);
	if (trans->params != NULL)
		str = g_strdup_printf("%s %u %s\r\n", trans->command, trans->trId, trans->params);
	else
		str = g_strdup_printf("%s %u\r\n", trans->command, trans->trId);
#endif
	return str;
}


void
np_transaction_queue_cmd(NPTransaction *trans, NPCommand *cmd)
{
	purple_debug_info("np", "queueing command.\n");
	trans->pendent_cmd = cmd;
	np_command_ref(cmd);
}

void
np_transaction_unqueue_cmd(NPTransaction *trans, NPCmdProc *cmdproc)
{
	NPCommand *cmd;

	if (!cmdproc->servconn->connected)
		return;

	purple_debug_info("np", "unqueueing command.\n");
	cmd = trans->pendent_cmd;

	g_return_if_fail(cmd != NULL);

	np_cmdproc_process_cmd(cmdproc, cmd);
	np_command_unref(cmd);

	trans->pendent_cmd = NULL;
}

#if 0
void
np_transaction_queue(NPTransaction *trans, NPTransaction *elem)
{
	if (trans->queue == NULL)
		trans->queue = g_queue_new();

	g_queue_push_tail(trans->queue, elem);
}

void
np_transaction_unqueue(NPTransaction *trans, NPCmdProc *cmdproc)
{
	NPTransaction *elem;

	while ((elem = g_queue_pop_head(trans->queue)) != NULL)
		np_cmdproc_send_trans(cmdproc, elem);
}
#endif

void
np_transaction_set_payload(NPTransaction *trans,
							const char *payload, int payload_len)
{
	g_return_if_fail(trans   != NULL);
	g_return_if_fail(payload != NULL);

	trans->payload = g_strdup(payload);
	trans->payload_len = payload_len ? payload_len : strlen(trans->payload);
}

void
np_transaction_set_data(NPTransaction *trans, void *data)
{
	g_return_if_fail(trans != NULL);

	trans->data = data;
}

void np_transaction_set_data_free(NPTransaction *trans, GDestroyNotify fn)
{
	g_return_if_fail(trans != NULL);
	trans->data_free = fn;
}

void
np_transaction_set_saveable(NPTransaction  *trans, gboolean saveable)
{
	g_return_if_fail(trans != NULL);

	trans->saveable = saveable;
}

void
np_transaction_add_cb(NPTransaction *trans, char *answer,
					   NPTransCb cb)
{
	g_return_if_fail(trans  != NULL);
	g_return_if_fail(answer != NULL);

	if (trans->callbacks == NULL)
	{
		trans->has_custom_callbacks = TRUE;
		trans->callbacks = g_hash_table_new_full(g_str_hash, g_str_equal, NULL,
												 NULL);
	}
	else if (trans->has_custom_callbacks != TRUE)
		g_return_if_reached ();

	g_hash_table_insert(trans->callbacks, answer, cb);
}

static gboolean
transaction_timeout(gpointer data)
{
	NPTransaction *trans;

	trans = data;
	g_return_val_if_fail(trans != NULL, FALSE);

#if 0
	purple_debug_info("np", "timed out: %s %d %s\n", trans->command, trans->trId, trans->params);
#endif

	trans->timer = 0;

	if (trans->timeout_cb != NULL)
		trans->timeout_cb(trans->cmdproc, trans);

	return FALSE;
}

void
np_transaction_set_timeout_cb(NPTransaction *trans, NPTimeoutCb cb)
{
	if (trans->timer)
	{
		purple_debug_error("np", "This shouldn't be happening\n");
		purple_timeout_remove(trans->timer);
	}
	trans->timeout_cb = cb;
	trans->timer = purple_timeout_add_seconds(60, transaction_timeout, trans);
}

void
np_transaction_set_error_cb(NPTransaction *trans, NPErrorCb cb)
{
	trans->error_cb = cb;
}
