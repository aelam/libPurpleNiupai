/**
 * @file table.h NP helper structure
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
#ifndef NP_TABLE_H
#define NP_TABLE_H

typedef struct _NPTable NPTable;

#include "cmdproc.h"
#include "transaction.h"
#include "msg.h"

typedef void (*NPMsgTypeCb)(NPCmdProc *cmdproc, NPMessage *msg);

struct _NPTable
{
	GHashTable *cmds; 		/**< Callbacks that manage command response. */
	GHashTable *msgs; 		/**< Callbacks that manage incoming messages. */
	GHashTable *errors; 	/**< Callbacks that manage command errors. */

	GHashTable *async; 		/**< Callbacks that manage incoming asyncronous messages. */
	/* TODO: Does this one is really needed? */
	GHashTable *fallback; 	/**< Fallback callback. */
};

/**
 * Create a new instance of a NPTable which map commands, errors and messages
 * with callbacks that will handle it.
 *
 * @return A new NPTable.
 */
NPTable *np_table_new(void);

/**
 * Destroy a NPTable.
 *
 * @param table The NPTable to be destroyed.
 */
void np_table_destroy(NPTable *table);

/**
 * Relate an incomming command from server with a callback able to handle
 * the event.
 *
 * @param table 	The NPTable.
 * @param command 	If NULL this add an incoming asyncronous command set in answer.
 * 					Else, the command sent.
 * @param answer 	The server answer to 'command'. If 'command' is NULL,
 * 					the asyncronous command sent by the server.
 * @param cb 		Callback to handle this event.
 */
void np_table_add_cmd(NPTable *table, char *command, char *answer,
					   NPTransCb cb);

/**
 * Set a callback to handle incoming command errors.
 *
 * @param table 	The NPTable.
 * @param answer 	Incoming command with error.
 * @param cb 		Callback to handle this error.
 */
void np_table_add_error(NPTable *table, char *answer, NPErrorCb cb);

/**
 * Relate a message Content-type with a callback able to handle it.
 *
 * @param table 	The NPTable.
 * @param type 		The Message Content-Type.
 * @param cb 		Callback to handle this Content-type.
 */
void np_table_add_msg_type(NPTable *table, char *type, NPMsgTypeCb cb);

#endif /* NP_TABLE_H */
