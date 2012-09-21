/**
 * @file cmdproc.h NP command processor functions
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
#ifndef NP_CMDPROC_H__
#define NP_CMDPROC_H__

typedef struct _NPCmdProc NPCmdProc;

#include "command.h"
#include "history.h"
#include "servconn.h"
#include "session.h"
#include "table.h"

struct _NPCmdProc
{
	NPSession *session;
	NPServConn *servconn;

	GQueue *txqueue;

	NPCommand *last_cmd;

	NPTable *cbs_table;

	NPHistory *history;

	GHashTable *multiparts; /**< Multi-part message ID's */

	void *data; /**< Extra data, like the switchboard. */
};

/**
 * Creates a NPCmdProc structure.
 *
 * @param session 	The session to associate with.
 *
 * @return A new NPCmdProc structure.
 */
NPCmdProc *np_cmdproc_new(NPSession *session);

/**
 * Destroys an NPCmdProc.
 *
 * @param cmdproc 	The object structure.
 */
void np_cmdproc_destroy(NPCmdProc *cmdproc);

/**
 * Process the queued transactions.
 *
 * @param cmdproc 	The NPCmdProc.
 */
void np_cmdproc_process_queue(NPCmdProc *cmdproc);

/**
 * Sends transaction using this servconn.
 *
 * @param cmdproc 	The NPCmdProc to be used.
 * @param trans 	The NPTransaction to be sent.
 */
gboolean np_cmdproc_send_trans(NPCmdProc *cmdproc, NPTransaction *trans);

/**
 * Add a transaction to the queue to be processed latter.
 *
 * @param cmdproc 	The NPCmdProc in which the transaction will be queued.
 * @param trans 	The NPTransaction to be queued.
 */
void np_cmdproc_queue_trans(NPCmdProc *cmdproc,
							 NPTransaction *trans);

void np_cmdproc_process_msg(NPCmdProc *cmdproc,
							 NPMessage *msg);
void np_cmdproc_process_cmd(NPCmdProc *cmdproc, NPCommand *cmd);
void np_cmdproc_process_cmd_text(NPCmdProc *cmdproc, const char *command);
void np_cmdproc_process_payload(NPCmdProc *cmdproc,
								 char *payload, int payload_len);

#endif /* NP_CMDPROC_H__ */
