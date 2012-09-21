/**
 * @file transaction.h NP transaction functions
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
#ifndef __NP_TRANSACTION_H__
#define __NP_TRANSACTION_H__

#include "internal.h"

typedef struct _NPTransaction NPTransaction;

#include "cmdproc.h"
#include "command.h"

typedef void (*NPTransCb)(NPCmdProc *cmdproc, NPCommand *cmd);
typedef void (*NPTimeoutCb)(NPCmdProc *cmdproc, NPTransaction *trans);
typedef void (*NPErrorCb)(NPCmdProc *cmdproc, NPTransaction *trans,
						   int error);

/**
 * A transaction. A sending command that will initiate the transaction.
 */
struct _NPTransaction
{
	NPCmdProc *cmdproc;

	gboolean saveable;	/**< Whether to save this transaction in the history */
	unsigned int trId;	/**< The ID of this transaction, if it's being saved */

	char *command;
	char *params;

	guint timer;

	void *data; /**< The data to be used on the different callbacks. */
	GDestroyNotify data_free;  /**< The function to free 'data', or @c NULL */

	GHashTable *callbacks;
	gboolean has_custom_callbacks;
	NPErrorCb error_cb;
	NPTimeoutCb timeout_cb;

	char *payload;
	size_t payload_len;

	GQueue *queue;
	NPCommand *pendent_cmd; /**< The command that is waiting for the result of
							   this transaction. */
};

NPTransaction *np_transaction_new(NPCmdProc *cmdproc, const char *command,
	const char *format, ...) G_GNUC_PRINTF(3, 4);
void np_transaction_destroy(NPTransaction *trans);

char *np_transaction_to_string(NPTransaction *trans);
void np_transaction_queue_cmd(NPTransaction *trans, NPCommand *cmd);
void np_transaction_unqueue_cmd(NPTransaction *trans, NPCmdProc *cmdproc);
void np_transaction_set_payload(NPTransaction *trans,
								 const char *payload, int payload_len);
void np_transaction_set_data(NPTransaction *trans, void *data);
void np_transaction_set_data_free(NPTransaction *trans, GDestroyNotify fn);
void np_transaction_set_saveable(NPTransaction  *trans, gboolean saveable);
void np_transaction_add_cb(NPTransaction *trans, char *answer,
							NPTransCb cb);
void np_transaction_set_error_cb(NPTransaction *trans, NPErrorCb cb);
void np_transaction_set_timeout_cb(NPTransaction *trans, NPTimeoutCb cb);

#endif /* __NP_TRANSACTION_H__ */
