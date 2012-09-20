/**
 * @file command.h NP command functions
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
#ifndef NP_COMMAND_H
#define NP_COMMAND_H

typedef struct _NPCommand NPCommand;

#include "cmdproc.h"
#include "transaction.h"

typedef void (*NPPayloadCb)(NPCmdProc *cmdproc, NPCommand *cmd,
							 char *payload, size_t len);

/**
 * A received command.
 */
struct _NPCommand
{
	unsigned int trId;

	char *command;
	char **params;
	int param_count;

	guint ref_count;

	NPTransaction *trans;

	char *payload;
	size_t payload_len;

	NPPayloadCb payload_cb;
	void *payload_cbdata;
};

/**
 * Create a command object from the incoming string and ref it.
 *
 * @param string 	The incoming string.
 *
 * @return 			A NPCommand object.
 */
NPCommand *np_command_from_string(const char *string);

/**
 * Increment the ref count.
 *
 * @param cmd 	The NPCommand to be ref.
 *
 * @return 		The ref command.
 */
NPCommand *np_command_ref(NPCommand *cmd);

/**
 * Decrement the ref count. If the count goes to 0, destroy it.
 *
 * @param cmd	The NPCommand to be unref.
 *
 */
void np_command_unref(NPCommand *cmd);

#endif /* NP_COMMAND_H */

